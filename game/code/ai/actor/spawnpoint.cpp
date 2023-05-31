//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Spawnpoint
//
// Description: 
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/spawnpoint.h>
#include <memory/srrmemory.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>
#include <ai/actor/actormanager.h>
#include <ai/actor/flyingactor.h>
#include <memory/srrmemory.h>
#include <events/eventmanager.h>
#include <data/PersistentWorldManager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================




//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================



//===========================================================================
// Member Functions
//===========================================================================

SpawnPoint::SpawnPoint( const char* spawnPointName , const char* statePropName, const rmt::Sphere& sphere, unsigned int timeOut )
: m_TimeActorDestroyed( 0 ),
m_TimeOutPeriod( timeOut * 1000 ),
m_WasDestroyed( false )
{
    m_Behaviours.Allocate( 6 );

    mPersistentObjectID = GetPersistentWorldManager()->GetPersistentObjectID();
    if( mPersistentObjectID < -1 )
    {
        m_WasDestroyed = true;
        return;
    }


    m_StatePropName = tName::MakeUID( statePropName );
    m_SpawnPointName = tName::MakeUID( spawnPointName );
    m_Sphere = sphere;
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    // Create a new triggervolume and add it to the tracker
    m_TriggerVolume = new SphereTriggerVolume( sphere.centre, sphere.radius );

    SetNumTriggers( 1,  HeapMgr()->GetCurrentAllocator() );

    m_TriggerVolume->SetLocator( this );

    // Make it active
    GetTriggerVolumeTracker()->AddTrigger( m_TriggerVolume );
    AddTriggerVolume( m_TriggerVolume );
    
    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
    GetEventManager()->AddListener( this, EVENT_WASP_BLOWED_UP );
}

SpawnPoint::~SpawnPoint()
{
    if ( m_TriggerVolume != NULL )
    {
        GetTriggerVolumeTracker()->RemoveTrigger( m_TriggerVolume );
        m_TriggerVolume = NULL;
    }
    for ( int i = 0 ; i < m_Behaviours.mUseSize ; i++ )
    {
        m_Behaviours[ i ]->Release();
    }
    
    if ( m_Behaviours.mUseSize > 0 )
        m_Behaviours.ClearUse();
    
    GetEventManager()->RemoveListener( this, EVENT_WASP_BLOWED_UP );
}


void 
SpawnPoint::AddBehaviour( Behaviour* behaviour )
{
    behaviour->AddRef();
    m_Behaviours.Add( behaviour );
}

void 
SpawnPoint::OnTrigger( unsigned int playerID )
{
    // Create a new actor with the behaviours attached to this spawn point and add it to the actormanager
    if ( GetActorManager()->GetActorByUID( m_SpawnPointName ) == NULL
         && CanRespawn() )
    {
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

        rmt::Matrix transform;
        transform.Identity();
        transform.FillTranslate( m_Sphere.centre );
        Actor* actor = GetActorManager()->CreateActor( m_StatePropName, m_SpawnPointName, transform );
        // Copy behaviours over
        
        if ( actor != NULL )
        {
            for ( int i = 0 ; i < m_Behaviours.mUseSize ; i++ )
            {
                actor->AddBehaviour( m_Behaviours[ i ] );
            }           
            actor->Activate();
        }

        
        HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
    }
}
        
bool 
SpawnPoint::CanRespawn()const
{
    return m_WasDestroyed == false;
}


void 
SpawnPoint::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_WASP_BLOWED_UP:
        {
            // Check to see if the wasp that was destroyed was the
            // wasp associated with this spawn point
            Actor* actor = reinterpret_cast< FlyingActor* >( pEventData );
            if ( actor->GetUID() == m_SpawnPointName )
            {
                // The actor associated with this spawn point was destroyed
                // remember the current time as the start point for our timeout counter
                m_TimeActorDestroyed = radTimeGetMilliseconds();
                m_WasDestroyed = true;
                GetPersistentWorldManager()->OnObjectBreak( mPersistentObjectID );
            }
            
        }
        break;
    default:
        break;
    }
}

