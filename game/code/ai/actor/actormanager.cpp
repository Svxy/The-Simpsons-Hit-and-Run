//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   actormanager
//
// Description: Actormanager class, for holding stateprops that have AI
//
// Authors:     Michael Riegger
//
//===========================================================================
 
#include <ai/actor/actormanager.h>
#include <ai/actor/actor.h>
#include <memory/srrmemory.h>
#include <console/console.h>
#include <ai/actor/flyingactor.h>
#include <ai/actor/projectile.h>
#include <ai/actor/attackbehaviour.h>
#include <ai/actor/evasionbehaviour.h>
#include <ai/actor/ufoattackbehaviour.h>
#include <ai/actor/attractionbehaviour.h>
#include <ai/actor/ufobeamalwaysonbehaviour.h>
#include <worldsim/avatarmanager.h>
#include <events/eventmanager.h>
#include <p3d/utility.hpp>
#include <raddebugwatch.hpp>
#include <radmemorymonitor.hpp>
#include <sstream>

#include <stack>

static const int MAX_NUM_ACTORS = 50;

// Static variables
float ActorManager::ActorRemovalRangeSqr = 65.0f * 65.0f;
ActorManager* ActorManager::sp_Instance = 0;

ActorManager* ActorManager::CreateInstance()
{
    rAssert( sp_Instance == 0 );

    HeapMgr()->PushHeap( GMA_PERSISTENT );
    sp_Instance = new ActorManager();
    HeapMgr()->PopHeap( GMA_PERSISTENT );

    return sp_Instance;
}

ActorManager* ActorManager::GetInstance()
{
    return sp_Instance;
}

void ActorManager::DestroyInstance()
{
    delete sp_Instance;
}

ActorManager::ActorManager()
{
    m_ActorList.Allocate( MAX_NUM_ACTORS );
    m_SpawnPointList.Allocate( 50 );
    m_ActorBank.Allocate( MAX_NUM_ACTORS );
    m_RemoveQueue.Allocate( MAX_NUM_ACTORS );
    SetupConsoleFunctions();

    GetEventManager()->AddListener( this, EVENT_STATEPROP_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_STATEPROP_ADDED_TO_SIM );

} 

ActorManager::~ActorManager()
{
    for ( int i = 0 ; i < m_ActorList.mUseSize ; i++ )
    {
        m_ActorList[ i ]->Release();
    }
    m_ActorList.ClearUse();
    for ( int i = 0 ; i < m_ActorBank.mUseSize ; i++ )
    {
        m_ActorBank[ i ]->Release();
    }
    m_ActorBank.ClearUse();
    for ( int i = 0 ; i < m_RemoveQueue.mUseSize ; i++ )
    {
        m_RemoveQueue[i]->RemoveFromDSG();
        m_RemoveQueue[ i ]->Release();
    }
    m_RemoveQueue.ClearUse();

    GetEventManager()->RemoveListener( this, EVENT_STATEPROP_DESTROYED );
    GetEventManager()->RemoveListener( this, EVENT_STATEPROP_ADDED_TO_SIM );
}

void
ActorManager::AddActor( Actor* actor )
{
    actor->AddRef();
    m_ActorList.Add( actor );
}

void 
ActorManager::AddActorToBank( Actor* actor )
{
    actor->AddRef();
    m_ActorBank.Add( actor );
}


void
ActorManager::RemoveActor( int index, bool removeFromDSG )
{
    Actor* actor = m_ActorList[ index ];
    GetEventManager()->TriggerEvent( EVENT_ACTOR_REMOVED, actor );

    // Turn off its behaviours
    actor->DeactivateBehaviours();
    m_ActorList.Remove( index );
    m_RemoveQueue.Add( actor );
}

void 
ActorManager::RemoveActorByDSGPointer( ActorDSG* dsgObject, bool removeFromDSG )
{
    for ( int i = 0 ; i < m_ActorList.mUseSize ; i++ )
    {
        if ( dsgObject == m_ActorList[ i ]->GetDSG() )
        {
            RemoveActor( i, removeFromDSG );
            break;
        }
    }
}

void 
ActorManager::RemoveAllActors()
{
    int i;
    while ( m_ActorList.mUseSize > 0 )
    {
        RemoveActor( 0 );
    }
    for ( i = 0 ; i < m_ActorBank.mUseSize ; i++ )
    {
        m_ActorBank[i]->ReleaseVerified();
    }
    for ( i = 0 ; i < m_RemoveQueue.mUseSize ; i++ )
    {
        m_RemoveQueue[i]->RemoveFromDSG();
        m_RemoveQueue[i]->ReleaseVerified();
    }

    m_ActorList.ClearUse();
    m_ActorBank.ClearUse();
    m_RemoveQueue.ClearUse();
    //radMemoryMonitorSuspend();
}


void 
ActorManager::AddSpawnPoint( SpawnPoint* point )
{
    point->AddRef();
    m_SpawnPointList.Add( point );
}

void 
ActorManager::FireProjectile( tUID typeName, const rmt::Vector& position, const rmt::Vector& direction, unsigned int ownerID )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    char instanceName[200];
    static int count = 0;
    count++;
    sprintf( instanceName, "%s %d","wasp bolt", count);

    rmt::Matrix transform;
    transform.Identity();
    transform.FillHeading( direction, rmt::Vector(0,1,0) );
    transform.FillTranslate( position );

    tName p3dInstanceName( instanceName );
    Actor* actor = CreateActor( typeName, p3dInstanceName.GetUID(), transform );
    if ( actor != NULL )
    {
        Projectile* projectile = static_cast< Projectile* >( actor );
        projectile->Fire();
        projectile->CalculateIntersections();
        actor->SetStatePropID( ownerID );
    }

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

    GetEventManager()->TriggerEvent( EVENT_WASP_BULLET_FIRED );
}



void
ActorManager::Update( unsigned int timeInMS )
{
    // Copy all the elements from the remove queue into the bank
    int i;
    while( m_RemoveQueue.mUseSize > 0 )
    {
        m_RemoveQueue[0]->RemoveFromDSG();
        m_ActorBank.Add( m_RemoveQueue[0] );
        m_RemoveQueue.Remove( 0 );
    } 

    for ( i = m_ActorList.mUseSize - 1 ; i >=0  ; i--)
    {
        // Compare avatar position with actor position, if they 
        // exceed a spawn range, then quit
        
        if ( WithinAliveRange( i ) || m_ActorList[i]->ShouldDespawn() == false )
        {

            m_ActorList[ i ]->Update( timeInMS );
        }
        else
        {
            RemoveActor( i );
        }
    }
}


void 
ActorManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_STATEPROP_DESTROYED:
        {
            StatePropDSG* pStatePropDSG = static_cast< StatePropDSG* >( pEventData );
            // Find the actor that corresponds to the stateprop ( THIS SHOULD PROBABLY BE OPTIMIZED )
            // WITH A MAP!
            for ( int i = 0 ; i < m_ActorList.mUseSize ; i++ )
            {
                if ( m_ActorList[i]->GetUID() == pStatePropDSG->GetUID() )
                {
                    // Do not remove it from the DSG. The Stateprop removes it itself automatically
                    RemoveActor( i, false );
                    break;
                }
            }
        }
        break;
    case EVENT_STATEPROP_ADDED_TO_SIM:
        {
            StatePropDSG* pStatePropDSG = static_cast< StatePropDSG* >( pEventData );
        }
        break;
    default:
        break;
    };
}


Actor* 
ActorManager::GetActorByName( const char* name )
{
    tUID uid = tName::MakeUID( name );
    Actor* actor = NULL;
    for ( int i = 0 ; i < m_ActorList.mUseSize ; i++ )
    {
        if ( m_ActorList[ i ]->GetUID() == uid )
        {
            actor = m_ActorList[ i ];
            break;
        }
    }   
    return actor;
}

Actor* 
ActorManager::GetActorByUID( tUID uid )
{
    Actor* actor = NULL;
    for ( int i = 0 ; i < m_ActorList.mUseSize ; i++ )
    {
        if ( m_ActorList[ i ]->GetUID() ==  uid )
        {
            actor = m_ActorList[ i ];
            break;
        }
    }   
    return actor;
}

std::vector< Actor*, s2alloc< Actor* > > 
ActorManager::GetActorsByType( const char* cTypeName )
{
    std::vector< Actor*, s2alloc< Actor* > > actorList;
    int i;

    tName typeName = cTypeName;
    tUID uid = typeName.GetUID();
    actorList.reserve( m_ActorBank.mUseSize + m_ActorList.mUseSize );

    for( i = 0 ; i < m_ActorList.mUseSize ; i++ )
    {
        if ( uid == m_ActorList[i]->GetStatePropUID() ) 
        {
            actorList.push_back( m_ActorList[i] );      
        }
    }
    for ( i = 0 ; i < m_ActorBank.mUseSize ; i++ )
    {
        if ( uid == m_ActorBank[i]->GetStatePropUID() ) 
        {
            actorList.push_back( m_ActorBank[i] );      
        }        
    }

    return actorList;
}


void 
ActorManager::SetupConsoleFunctions()
{
    GetConsole()->AddFunction( "AddFlyingActor", AddFlyingActor, "",5,5); //Adds a new (flying actor) into the world
    GetConsole()->AddFunction( "AddFlyingActorByLocator", AddFlyingActorByLocator, "",3,4); //Adds a new (flying actor) into the world
    GetConsole()->AddFunction( "AddBehaviour", AddBehaviour, "",2,7); //adds a new behaviour pattern to an actor
    GetConsole()->AddFunction( "SetCollisionAttributes", SetCollisionAttributes, "", 4, 4 ); //Sets friction/mass/elasticity values
    GetConsole()->AddFunction( "AddSpawnPoint", AddSpawnPointScript, "", 8,8 ); //Creates a new spawnpoint
    GetConsole()->AddFunction( "AddSpawnPointByLocatorScript", AddSpawnPointByLocatorScript, "", 6, 6 ); //Creates a new spawnpoint
    GetConsole()->AddFunction( "SetProjectileStats", SetProjectileStats, "", 3, 3 ); //Sets stateprop name, speed and number of instances to allocate
    GetConsole()->AddFunction( "PreallocateActors", PreallocateActors, "", 2, 2 ); //Creates N stateprops for later use
    GetConsole()->AddFunction( "SetActorRotationSpeed", SetActorRotationSpeed, "", 2, 2 );    //Creates sets the rotation speed for all actors of the given type. Rotation speed is in degrees/second
    GetConsole()->AddFunction( "AddShield", AddShield, "", 2, 2 ); //Creates a shield around the stateprop. First parameter = recharge time in seconds after being broken.
}

bool 
ActorManager::WithinAliveRange( int index )
{
    bool withinRange;

    Actor* actor = m_ActorList[ index ];

    rmt::Vector position;
    actor->GetPosition( &position );
    // Get the avatar position;
    Avatar* currAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rmt::Vector currAvatarPos;     
    currAvatar->GetPosition( currAvatarPos );
    // calc distance
    float distSqr = (currAvatarPos - position).MagnitudeSqr();
    if ( distSqr < ActorRemovalRangeSqr )
    {
        withinRange = true;
    }
    else
    {
        withinRange = false;
    }
    return withinRange;
}

void
ActorManager::AddFlyingActor( int argc, char** argv )
{
    // First param - statepropname
    // 2nd param - instancename
    // 3rd param - position X
    // 4th param - position Y
    // 5th param - position Z

    const char* statepropName = argv[1];
    const char* instanceName = argv[2];
    rmt::Vector position;
    position.x = static_cast< float > ( atof( argv[3] ) );
    position.y = static_cast< float > ( atof( argv[4] ) );
    position.z = static_cast< float > ( atof( argv[5] ) );

    rmt::Matrix transform;
    transform.Identity();
    transform.FillTranslate( position );
   
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    // Create a new actor and immediately insert it into the world
        Actor* actor = GetActorManager()->CreateActor( tName::MakeUID( statepropName ), tName::MakeUID( instanceName ), transform );
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
}

void
ActorManager::AddFlyingActorByLocator( int argc, char** argv )
{
    const char* statepropName = argv[1];
    const char* instanceName = argv[2];    
    const char* locatorName = argv[3];
    const char* isPermanent = argv[4];

    Locator* locator = p3d::find< Locator >( locatorName );
    if( locator != NULL )
    {
        rmt::Vector position;
        locator->GetPosition( &position );

        rmt::Matrix transform;
        transform.Identity();
        transform.FillTranslate( position );

        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        // Create a new actor and immediately insert it into the world
            Actor* actor = GetActorManager()->CreateActor( tName::MakeUID( statepropName ), tName::MakeUID( instanceName ), transform );

        // Should this thing despawn if out of range, or should it be permanent
            
        if( argc > 3 )
        {
            if ( strcmp( isPermanent, "NEVER_DESPAWN" ) == 0 )
            {
                rAssert( actor != NULL );
                if( actor != NULL )
                {
                    actor->SetShouldDespawn( false );
                }
            }
        }
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    }
    else
    {
        rTunePrintf("AddFlyingActorByLocator - can't find locator %s\n", locatorName);
    }
}

void
ActorManager::AddSpawnPointScript( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    const char* spawnPointName = argv[1];
    const char* statePropName = argv[2];
    const char* instanceName = argv[3];
    rmt::Vector position;
    position.x = static_cast< float >( atof( argv[4] ) );
    position.y = static_cast< float >( atof( argv[5] ) );
    position.z = static_cast< float >( atof( argv[6] ) );
    float radius = static_cast< float >( atof( argv[7] ) );
    unsigned int timeOut = atoi( argv[8] );
    
    rmt::Sphere sphere( position, radius );

    SpawnPoint* pPoint = new SpawnPoint( spawnPointName, statePropName, sphere, timeOut );
  
    GetActorManager()->AddSpawnPoint( pPoint );

    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}

void 
ActorManager::AddSpawnPointByLocatorScript( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    const char* spawnPointName = argv[1];
    const char* statePropName = argv[2];
    const char* instanceName = argv[3];

    const char* locatorName = argv[4];

    Locator* locator = p3d::find< Locator >( locatorName );
    if ( locator != NULL )
    {
        rmt::Vector position;
        locator->GetPosition( &position );
        float radius = static_cast< float >( atof( argv[5] ));
        unsigned int timeOut = atoi( argv[6] );
        rmt::Sphere sphere( position, radius );
        SpawnPoint* pPoint = new SpawnPoint( spawnPointName, statePropName, sphere, timeOut );
        GetActorManager()->AddSpawnPoint( pPoint );
    }
    else
    {
        rTunePrintf("AddSpawnPointByLocatorScript - could not find locator %s\n", locatorName);
    }

    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}


void 
ActorManager::SetProjectileStats( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    const char* typeName = argv[1];
    float speed = static_cast< float >( atof( argv[2] ) );
    int numInstances = static_cast< int >( atoi( argv[3] ) );
    // Create 
    rAssert( numInstances < 20 );
    for ( int i = 0 ; i < numInstances ; i++)
    {
        // Allocate memory and initialize a new projectile actor
        Projectile* projectile = new Projectile();
        projectile->Init( typeName, "not active" );
        projectile->SetSpeed( speed );
        // Add it to the bank of initialized but unplaced Actors
        GetActorManager()->AddActorToBank( projectile );        
    }

    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}

void 
ActorManager::PreallocateActors( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    const char* stateprop = argv[1];
    int numInstances = atoi( argv[2] );
    rAssert( numInstances >= 0 && numInstances < 20 );
   
    for ( int i = 0 ; i < numInstances ; i++)
    {
        // Allocate memory and initialize a new projectile actor
        Actor* actor = new FlyingActor();
        if ( actor->Init( stateprop, "not active" ))
        {
            // Add it to the bank of initialized but unplaced Actors
            GetActorManager()->AddActorToBank( actor );        
        }
        else
        {
            actor->Release();
        }
    }

    HeapMgr()->PopHeap(GMA_LEVEL_OTHER); 
}

void 
ActorManager::SetActorRotationSpeed( int argc, char** argv )
{
    const char* typeName = argv[1];
    float rotationSpeed = static_cast< float > ( atof( argv[2] ) );
    std::vector< Actor*, s2alloc< Actor*> > actorList = GetActorManager()->GetActorsByType( typeName );
    for ( unsigned int i = 0 ; i < actorList.size() ; i++ )
    {
        actorList[ i ]->SetRotationSpeed( rotationSpeed );
    }
}

void 
ActorManager::AddShield( int argc, char** argv )
{
    const char* name = argv[1];
    const char* shieldStatePropName = argv[2];
    std::vector< Actor*, s2alloc< Actor*> > actorList = GetActorManager()->GetActorsByType( name );
    for ( unsigned int i = 0 ; i < actorList.size() ; i++ )
    {
        actorList[ i ]->LoadShield( shieldStatePropName );
    }


}


SpawnPoint* 
ActorManager::GetSpawnPointByName( const char* name )
{
    SpawnPoint* spawnPoint = NULL;
    tUID uid = tName( name ).GetUID();
    for ( int i = 0 ; i < m_SpawnPointList.mUseSize ; i++ )
    {
        if ( m_SpawnPointList[ i ]->GetUID() == uid )
        {
            spawnPoint = m_SpawnPointList[ i ];
            break;
        }
    }
    return spawnPoint;
}
void 
ActorManager::RemoveAllSpawnPoints()
{
    for ( int i = 0 ; i < m_SpawnPointList.mUseSize ; i++ )
    {
        m_SpawnPointList[ i ]->Release();
    }
    m_SpawnPointList.ClearUse();
}

Actor* 
ActorManager::CreateActor( tUID typeName, tUID instanceName, const rmt::Matrix& transform )
{
    // Find an actor in the actorbank that has a stateprop called typeName.
    Actor* pActor = NULL;
    int i;
    for ( i = 0 ; i < m_ActorBank.mUseSize ; i++ )
    {
        if ( m_ActorBank[ i ]->GetStatePropUID() == typeName )
        {
            pActor = m_ActorBank[i];
            break;
        }
    }
    if ( pActor != NULL && m_ActorBank.mUseSize > 0 )
    {
        // If found, move it from the bank to the active actor list
        // give it instanceName
        pActor->SetUID( instanceName );
        AddActor( pActor );
        // add it to the dsg
        pActor->AddToDSG();

        // Reset default behaviour
        pActor->Activate();

        GetEventManager()->TriggerEvent( EVENT_ACTOR_CREATED, pActor );        
        pActor->SetTransform( transform );
        pActor->SetState( 0 );
        // remove it from the actorbank list
        m_ActorBank.Remove( i );
        pActor->Release();
    }

    return pActor;
}


void
ActorManager::AddBehaviour( int argc, char** argv )
{
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
#endif

    const char* targetObject = argv[1];
    const char* behaviourName = argv[2];
    
    // Can add behaviours to all sorts of things
    // is the given name a spawn point?   
    SpawnPoint* spawnPoint = GetActorManager()->GetSpawnPointByName( targetObject );
    // is it an actor's instance name?
    Actor* actor = GetActorManager()->GetActorByName( targetObject );
    // is it a stateprop type name?
    std::vector< Actor*, s2alloc< Actor*> > actorList = GetActorManager()->GetActorsByType( targetObject );

    int numBehavioursToAllocate = actorList.size();
    if ( spawnPoint ) 
        numBehavioursToAllocate++;
    if ( actor )
        numBehavioursToAllocate++;

    std::stack< Behaviour* > behaviourList;

    // Allocate numBehavioursToAllocate behaviours.
    for ( int i = 0 ; i < numBehavioursToAllocate ; i++ )
    {
        Behaviour* behaviour = NULL;
        if ( strcmp( behaviourName, "ATTACK_PLAYER" ) == 0 )
        {
            float maxFiringRange = static_cast< float >( atof( argv[3] ) );
            float forwardFiringArc = static_cast< float >( atof( argv[4] ) );
            AttackBehaviour* attackBehaviour = new AttackBehaviour( maxFiringRange, forwardFiringArc );
            behaviour = attackBehaviour;
            if ( argc > 5 )
            {   
                float timeBeforeMoving = static_cast< float >( atof( argv[5] ) );
                attackBehaviour->SetMovementIntervals( timeBeforeMoving );
            }
        }
        else if ( strcmp( behaviourName, "EVADE_PLAYER" ) == 0 )
        {
            float minEvadeDistHoriz = static_cast< float >( atof( argv[ 3 ] ) );
            float maxEvadeDistHoriz = static_cast< float >( atof( argv[ 4 ] ) );
            float minEvadeDistVert = static_cast< float >( atof( argv[ 5 ] ) );
            float maxEvadeDistVert = static_cast< float >( atof( argv[ 6 ] ) );
            float speed = static_cast< float >( atof( argv[ 7 ] ) );
            EvasionBehaviour* evasionBehaviour = new EvasionBehaviour( minEvadeDistHoriz, maxEvadeDistHoriz, minEvadeDistVert, maxEvadeDistVert, speed );
            behaviour = evasionBehaviour;
        }   
        else if ( strcmp( behaviourName, "UFO_ATTACK_ALL" ) == 0 )
        {
            const char* tractorbeamName = argv[3];
            behaviour = new UFOAttackBehaviour( 0 );
            // We want the actor to load up a tractor beam
            if ( actor )
                actor->LoadTractorBeam( tractorbeamName );
        }
        else if ( strcmp( behaviourName, "ATTRACTION" ) == 0 )
        {
            float minWatchDistance = static_cast< float >( atof( argv[3] ) );
            float maxWatchDistance = static_cast< float >( atof( argv[4] ) );
            float speed = static_cast< float >( atof( argv[5] ) );
            behaviour = new AttractionBehaviour( minWatchDistance, maxWatchDistance, speed );
        }
        else if ( strcmp( behaviourName, "UFO_BEAM_ALWAYS_ON" ) == 0 )
        {
            // This is for level 7 mission 3, where the ufo is stationary and the beam is always on
            const char* tractorbeamName = argv[3];
            behaviour = new UFOBeamAlwaysOn();
            if ( actor )
                actor->LoadTractorBeam( tractorbeamName );
        }
        else 
        {
            rTunePrintf( "Unknown behaviour %s!", behaviourName );
            rAssert( false );
        }   

        behaviourList.push( behaviour );
    }    
  
    // We now have an array full of preallocated behaviours

    // Attach them to the actorList 
    for ( unsigned int i = 0 ; i < actorList.size() ; i++ )
    {
        actorList[i]->AddBehaviour( behaviourList.top() );
        behaviourList.pop();
    }   

    // Attach it to any individual actors that matched the given name
    if ( actor )
    {
        actor->AddBehaviour( behaviourList.top() );
        behaviourList.pop();
    }
    
    // Attach it to any spawn points that we matched up with
    if ( spawnPoint ) 
    {
        spawnPoint->AddBehaviour( behaviourList.top() );
        behaviourList.pop();
    }
    // Any remaining behaviours left in this stack are probably mem leaks
    rTuneAssert( behaviourList.empty() );

#ifdef RAD_GAMECUBE
    HeapMgr()->PopHeap( GMA_GC_VMM );
#else
    HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
#endif
}

void 
ActorManager::AddRespawnBehaviourPosition( int argc, char** argv )
{
    rmt::Vector position;
    position.x = static_cast< float > ( atof( argv[1] ) );
    position.y = static_cast< float > ( atof( argv[2] ) );
    position.z = static_cast< float > ( atof( argv[3] ) );

    //RespawnBehaviour::AddRespawnPosition( position );
}

void 
ActorManager::SetCollisionAttributes( int argc, char** argv )
{
    const char* instanceName = argv[1];
    
    Actor* actor = GetActorManager()->GetActorByName( instanceName );
    

    float friction = static_cast< float >( atof( argv[2] ) );
    float mass = static_cast< float >( atof( argv[3] ) );
    float elasticity = static_cast< float >( atof( argv[4] ) );

}



