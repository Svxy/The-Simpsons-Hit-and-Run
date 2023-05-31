//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   flyingactor
//
// Description: Flying Actor
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/flyingactor.h>
#include <ai/actor/actordsg.h>
#include <ai/actor/attackbehaviour.h>
#include <ai/actor/cutcambehaviour.h>
#include <ai/actor/actoranimation.h>
#include <ai/actor/actoranimationwasp.h>
#include <ai/actor/actoranimationufo.h>
#include <ai/actor/attractionbehaviour.h>
#include <ai/actor/evasionbehaviour.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/IntersectManager/IntersectManager.h>
#include <stateprop/statepropdata.hpp>
#include <constants/actorenum.h>
#include <atc/atcmanager.h>
#include <main/game.h>
#include <memory/srrmemory.h>
#include <render/dsg/staticentitydsg.h>
#include <contexts/bootupcontext.h>
#include <float.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/gameplaymanager.h>


//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

// Rotation speed in degrees per second
const float DEFAULT_ROTATION_SPEED = 40.0f;
const float DEFAULT_FLYING_SPEED = 0.001f;
const float INTERSECTION_LIST_RADIUS = 20.0f;

const float RAYWIDTH_FOR_PATH_FINDING = 1.0f;

const float FLYING_HEIGHT = 2.0f;
const int NUM_MOVEMENT_RETRIES = 6;

const float MIN_FILL_INTERSECTION_DIAMETER = 10.0f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

rmt::Randomizer FlyingActor::s_Randomizer(0);
bool FlyingActor::s_RandomizerSeeded = false;

//===========================================================================
// Member Functions
//===========================================================================



FlyingActor::FlyingActor():
m_CurrentBehaviour( NULL ),    
m_AttackBehaviour( NULL ),
m_EvadeBehaviour( NULL ),
m_AttractionBehaviour( NULL ),
m_ActorAnimation( NULL ),
m_DesiredHeight( 0 ),
m_DesiredHeightEnabled( false ),
m_IsMoving( false ),
m_Speed( DEFAULT_FLYING_SPEED ),
m_CurrentWaypoint( 0 ),
m_GroundIntersectionHeight( FLT_MAX ),
m_HighestIntersectHeight( FLT_MAX ),
m_HighestIntersectNormal( 0,0,0 )
{

    SetRotationSpeed( DEFAULT_ROTATION_SPEED );
    
    if (!s_RandomizerSeeded)
    {
        s_Randomizer.Seed (Game::GetRandomSeed ());
        s_RandomizerSeeded = true;
    }

    m_Waypoints.reserve( NUM_MOVEMENT_RETRIES );

}

FlyingActor::~FlyingActor()
{
    if ( m_CurrentBehaviour != NULL )
    {
        m_CurrentBehaviour->Deactivate();
        m_CurrentBehaviour->Release();
        m_CurrentBehaviour = NULL;
    }

    if ( m_AttackBehaviour != NULL )
    {
        m_AttackBehaviour->Release();
        m_AttackBehaviour = NULL;
    }
    if ( m_EvadeBehaviour != NULL )
    {
        m_EvadeBehaviour->Release();
        m_EvadeBehaviour = NULL;
    }
    if ( m_AttractionBehaviour != NULL )
    {
        m_AttractionBehaviour->Release();
        m_AttractionBehaviour = NULL;
    }
    if ( m_ActorAnimation != NULL )
    {
        delete m_ActorAnimation;
    }

    tRefCounted::Release(mp_StateProp);
}

// Initialize object with its own stateprop and give it a name
// Return true if stateprop found and used to create a dsg object successfully
bool FlyingActor::Init( const char* statePropName, const char* instanceName )
{
    bool success;
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    CStatePropData* pStatePropData = p3d::find< CStatePropData > ( statePropName );
    if ( pStatePropData )
    {
        mp_StateProp = new ActorDSG();
        mp_StateProp->AddRef();

        CollisionAttributes* pCollAttr = GetATCManager()->CreateCollisionAttributes( PROP_MOVEABLE,  1, 2.2425f );

        pCollAttr->AddRef();
        rmt::Matrix transform;
        transform.Identity();
        mp_StateProp->LoadSetup( pStatePropData, 0, transform, pCollAttr, true, false ); 
        mp_StateProp->SetName( instanceName );
        pCollAttr->Release();


        // Quickie test to make wing animations only available on the 
        // beecameras
        // shields too
        // and shadows
        if ( strcmp( statePropName, "beecamera" ) == 0 )
        {
            mp_StateProp->SetProcAnimator( new WingAnimator() );
            // Lets grab a shadow. For now we will use the absolute simplest shadow around
            tDrawable* shadow = p3d::find< tDrawable >( "SimpleCircleShadowShape" );
            if ( shadow != NULL )
            {
                mp_StateProp->SetShadow( shadow );
            }

            m_ActorAnimation = new ActorAnimationWasp();
        }
        else if ( strcmp( statePropName, "spaceship" )==0 )
        {
            m_ActorAnimation = new ActorAnimationUFO();            
        }

        //
        // Register to be notified of state prop state changes
        //
        mp_StateProp->AddStatePropListener( this );

        success = true;
    }
    else 
    {
        success = false;
    }

    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);

    return success;
}

void FlyingActor::Update( unsigned int timeInMS )
{
    FindGroundIntersection( &m_HighestIntersectHeight, &m_HighestIntersectNormal );

    rmt::Matrix currentTransform;
    GetTransform( &currentTransform );
/*
    // Check the intersection list and refill it if it is invalid
    if ( m_IntersectionSphere.Contains( currentTransform.Row(3) ) == false ||
         m_IntersectionList.GetNumStatics() == 0 ) // Problem with these things being initialized without
         // while terrain is still loading
    {
        FillIntersectionList( currentTransform.Row(3), INTERSECTION_LIST_RADIUS );            
    }
    else
    {
        // Refill the intersection list dynamics every frame
        // StaticPhys should not change on a frame to frame basis
        FillIntersectionListDynamics( currentTransform.Row(3), INTERSECTION_LIST_RADIUS );
    }
    */
    FillIntersectionList( currentTransform.Row(3), INTERSECTION_LIST_RADIUS ); 

    UpdateMovement( timeInMS, &currentTransform.Row(3) );

    // Update idle animation

    rmt::Matrix newTransform;

    if ( m_ActorAnimation )
    {
        if ( m_ActorAnimation->Update( currentTransform, &newTransform, static_cast< float >( timeInMS ) ) )
        {
            currentTransform = newTransform;
        }
    }

    SetTransform( currentTransform );
    
       
    if ( m_CurrentBehaviour != NULL )
    {
        if ( m_CurrentBehaviour->IsExclusive() == false ) 
        {
            float chance = s_Randomizer.Float();
            if ( chance < 0.5f )
            {
                if ( m_AttackBehaviour ) 
                {
                    ChangeBehaviour( m_AttackBehaviour );
                }
            }
            else
            {
                if ( m_EvadeBehaviour ) 
                {
                    ChangeBehaviour( m_EvadeBehaviour );
                }
            }
        }
    }


    if ( m_CurrentBehaviour != NULL )
    {
        m_CurrentBehaviour->Apply( this, timeInMS );
    }

    // Update the previous position variable (Actor base class variable)
    m_PreviousPosition = currentTransform.Row(3);
    // Lets set the shadow position
    mp_StateProp->RecomputeShadowPositionNoIntersect( m_HighestIntersectHeight, m_HighestIntersectNormal, 0.05f, 3.0f );
}



void 
FlyingActor::AddBehaviour( Behaviour* behaviour )
{
    if ( dynamic_cast< AttackBehaviour* >( behaviour ) != NULL )
    {
        tRefCounted::Assign( m_AttackBehaviour, behaviour );
        if ( m_CurrentBehaviour == NULL )
        {
            ChangeBehaviour( behaviour );
        }
    }
    else if ( dynamic_cast< EvasionBehaviour* >( behaviour ) != NULL )
    {
        tRefCounted::Assign( m_EvadeBehaviour, behaviour );
        if ( m_CurrentBehaviour == NULL )
        {
            ChangeBehaviour( behaviour );
        }

    }
    else if ( dynamic_cast< AttractionBehaviour* >( behaviour ) != NULL )
    {
        tRefCounted::Assign( m_AttractionBehaviour, behaviour );
        // Lets make it so that when the designer adds this behaviour via
        // a mission script, it automatically becomes the default
        ChangeBehaviour( m_AttractionBehaviour );
    }
    else
    {
        tRefCounted::Assign( m_CurrentBehaviour, behaviour );
        ChangeBehaviour( m_CurrentBehaviour );
    }
}

void 
FlyingActor::Activate()
{
    if ( m_AttractionBehaviour )
    {
        ChangeBehaviour( m_AttractionBehaviour );
    }
    
    if ( m_CurrentBehaviour == NULL )
    {
        if ( m_AttractionBehaviour )
            ChangeBehaviour( m_AttractionBehaviour );
        else if ( m_AttackBehaviour )
            ChangeBehaviour( m_AttackBehaviour );
    }
    m_IsMoving = false;
    mp_StateProp->RestoreShield();
    m_HighestIntersectHeight = FLT_MAX;
}

void 
FlyingActor::DeactivateBehaviours()
{
    ChangeBehaviour( NULL );    
}   

void 
FlyingActor::SetRotationSpeed( float rotationSpeed )
{
    // convert from degrees per second to radians per millisecond   
    m_RotationSpeed = rotationSpeed * 3.1415927f / ( 180.0f * 1000.0f );
}

void 
FlyingActor::LookAt( const rmt::Vector& target, unsigned int timeInMS )
{
    rmt::Matrix actorTransform;
    GetTransform( &actorTransform );

    rmt::Matrix newTransform;
    rmt::Quaternion actorQ;
    actorQ.BuildFromMatrix( actorTransform );
  
    rmt::Quaternion targetQ;

    rmt::Vector toTarget = actorTransform.Row(3) - target;
    
    rmt::Matrix targetMat;
    targetMat.Identity();
    targetMat.FillHeading( toTarget, rmt::Vector( 0, 1.0f, 0 ) );
    targetQ.BuildFromMatrix( targetMat );

    // Find angle between vectors
    float targetDotActor = targetMat.Row(2).DotProduct( actorTransform.Row(2) );
    float angle;
    // Be careful of the values that get fed to arc cosine from the dot product
    // operation, if they are even slightly out of the range[0,1], NaN and other
    // bad things will occur, so make sure to clamp them beforehand
    if ( targetDotActor >= 1.0f ) 
    {
        angle = 0;
    }
    else if ( targetDotActor <= 0 )
    {
        angle = 3.1415927f / 2.0f;
    }   
    else
    {
        angle = rmt::ACos( targetDotActor );
    }
    // SLERP!
    float deltaAngle = m_RotationSpeed * static_cast< float >( timeInMS );

    float t = deltaAngle / angle;
    if ( t > 1.0f )
    {
        t = 1.0f;
    }
    rmt::Quaternion resultQ;
    resultQ.Slerp( actorQ, targetQ, t );
    resultQ.Normalize();

    rmt::Matrix result;
    result.Identity();
    result.FillRotation( resultQ );
    result.FillTranslate( actorTransform.Row(3) );
    SetTransform( result );

}

void 
FlyingActor::ReleaseBehaviours()
{
    if ( m_AttackBehaviour != NULL )
    {
        m_AttackBehaviour->Release();
        m_AttackBehaviour = NULL;
    }
    if ( m_EvadeBehaviour != NULL )
    {
        m_EvadeBehaviour->Release();
        m_EvadeBehaviour = NULL;
    }
}

void FlyingActor::FindWaypoint( const rmt::Vector& start, const rmt::Vector& end, int depth )
{
    rmt::Vector unused;

    if ( depth <= 0 )
        return;

    // Determine if there is a clear LOS between start and end
    rmt::Vector intersection;
    bool hitObject = m_IntersectionList.TestIntersection( start, end, &intersection );
    if ( hitObject == false )
    {
        m_Waypoints.push_back( end );   
    }
    else
    {
        // Check for an intermediary position somewhere around the intersection point
        const rmt::Vector up( 0, 1.0f, 0 );
        rmt::Vector ray = end - start;
        ray.Normalize();
        rmt::Vector right;
        right.CrossProduct( ray, up );

        float offsetDist = 2.0f;
        float testDir = 1.0f;

        for ( int i = 0 ; i < NUM_MOVEMENT_RETRIES ; i++ )
        {
            rmt::Vector offset;
            offset = right * testDir * offsetDist;
            
            testDir *= -1.0f;
            if ( testDir > 0 )
                offsetDist += 2.5f;

            rmt::Vector groundNormal, groundPlaneIntersectionPoint;
            bool foundPlane;
            
            rmt::Vector intermediary = offset + intersection;

            GetIntersectManager()->FindIntersection( intermediary,
                                                    foundPlane,
                                                    groundNormal,
                                                    groundPlaneIntersectionPoint );
                
            if ( foundPlane )
            {
                intermediary.y = groundPlaneIntersectionPoint.y + FLYING_HEIGHT + s_Randomizer.Float() * 2.0f;
            }
          
            if ( m_IntersectionList.LineOfSight( start, intermediary )&& 
                 m_IntersectionList.LineOfSight( intermediary, end ) )
            {
                // Clear line of sight, add it to the waypoint list and recurse
                m_Waypoints.push_back( intermediary );
                FindWaypoint( intermediary, end, depth - 1 );                               
                break;
            }
        }
    }
}

void 
FlyingActor::MoveTo( const rmt::Vector& destination, float speed )
{
    if ( speed <= 0 )
        speed = DEFAULT_FLYING_SPEED;

    m_Speed = speed;

    rmt::Vector position;
    GetPosition( &position );

    rmt::Vector midway = (destination + position )* 0.5f;
    float diameter = (destination - position).Magnitude();
    if ( diameter < MIN_FILL_INTERSECTION_DIAMETER )
        diameter = MIN_FILL_INTERSECTION_DIAMETER;
    FillIntersectionList( midway, diameter );


    // Reset waypoint information
    m_Waypoints.resize(0);
    m_CurrentWaypoint = 0;
    

    // 
    // First make sure that the destination is actually above ground
    rmt::Vector intersectTestPosition = destination;
    intersectTestPosition.y += 100.0f;
    rmt::Vector groundAdjustedDest = destination;
    rmt::Vector groundNormal, groundPlaneIntersectionPoint;
    bool foundPlane;
    GetIntersectManager()->FindIntersection( intersectTestPosition,
                                             foundPlane,
                                             groundNormal,
                                             groundPlaneIntersectionPoint );
        
    if ( foundPlane )
    {
        if ( groundAdjustedDest.y <  groundPlaneIntersectionPoint.y + FLYING_HEIGHT )
        {
            groundAdjustedDest.y = groundPlaneIntersectionPoint.y + FLYING_HEIGHT;
        }
    }
    // Start finding waypoints
    // Note that this is a recursive function
    FindWaypoint( position, groundAdjustedDest, m_Waypoints.capacity() );
    // Did we create any waypoints?
    // If so, start moving, and set the appropriate flags
    if ( m_Waypoints.empty() == false )
        m_IsMoving = true;
    else
        m_IsMoving = false;
}


// Change the current behaviour of the object.
// Tell the current behaviour object to deactivate itself
// The reassign the m_CurrentBehaviour pointer to the given
// behaviour and tell it to activate itself
bool 
FlyingActor::ChangeBehaviour( Behaviour* newBehaviour )
{
    bool changed;
    
    if ( m_CurrentBehaviour == newBehaviour )
    {
        changed = false;
    }
    else
    {
        if ( m_CurrentBehaviour == NULL )
        {
            tRefCounted::Assign( m_CurrentBehaviour, newBehaviour );
            m_CurrentBehaviour->Activate();
            changed = true;
        }
        else
        {
            m_CurrentBehaviour->Deactivate();
            tRefCounted::Assign( m_CurrentBehaviour, newBehaviour );
            if ( m_CurrentBehaviour != NULL )
            {
                m_CurrentBehaviour->Activate();
            }
            changed = true;
        }
    }    
    return changed;
}

bool 
FlyingActor::UpdateMovement( unsigned int timeInMS, rmt::Vector* out_newPosition )
{
    rmt::Vector movementOffset(0,0,0);
    rmt::Vector currPosition;
    GetPosition( &currPosition );

    // Sanity check
    {
        bool cleanMove = m_IntersectionList.LineOfSight( currPosition, m_PreviousPosition, RAYWIDTH_FOR_PATH_FINDING );
        if ( cleanMove == false )
        {
            // We just moved through a wall or something crazy. What was physics doing? 
            // Abort current movement, snap back to the old position
            currPosition = m_PreviousPosition;
            m_IsMoving = false;
        }
    }


    // Basically we want to follow waypoints until the final one is reached
    // Check to see if it was reached and set the moving flag to false to disable it in
    // the future
    if ( m_CurrentWaypoint >= m_Waypoints.size() )
        m_IsMoving = false;

    bool positionUpdated; // return value, was the object actually moved?

    if ( m_IsMoving )
    {
        const rmt::Vector& nextDest = m_Waypoints[ m_CurrentWaypoint ];
        // We are moving, follow the waypoints
        // Check that we still have a line of sight. If not, abort
        if ( m_IntersectionList.LineOfSight( currPosition, nextDest ) == false )
        {
            m_IsMoving = false;
            return false;
        }

        float distToWaypoint = ( nextDest - currPosition ).Magnitude();
        float distToTravel = timeInMS * m_Speed;
        // Create vector from current position to the waypoint

        const float CLOSE_ENOUGH_TO_WAYPOINT_DIST = 0.4f;

        if ( distToTravel > ( distToWaypoint - CLOSE_ENOUGH_TO_WAYPOINT_DIST ) )
        {        
            // We are going to travel too far, clamp to the waypoint
            *out_newPosition = nextDest;
            m_CurrentWaypoint++;
        }
        else
        {
            rmt::Vector toWaypoint = ( nextDest - currPosition ) / distToWaypoint;

            // Move to the next waypoint
            *out_newPosition = currPosition + toWaypoint * m_Speed * static_cast< float >( timeInMS );
        }
        positionUpdated = true;
    }
    else
    {       
        // Not moving
        // Check current height versus desired height
        if ( m_HighestIntersectHeight == FLT_MAX )
        {
            // Couldnt find an intersection. Set it now to be the current height - FLYING_HEIGHT
            m_HighestIntersectHeight = currPosition.y - FLYING_HEIGHT;
        }

        if ( currPosition.y < m_HighestIntersectHeight + FLYING_HEIGHT )
        {
            // Move upwards at m_Speed
            // Calc distance to the desired point
            float distToMinHeight = m_HighestIntersectHeight + FLYING_HEIGHT - currPosition.y;
            float distCanTravel = m_Speed * static_cast< float >( timeInMS );
            if ( distToMinHeight < distCanTravel )
            {
                // Just clamp the position to desired
                *out_newPosition = currPosition;
                out_newPosition->y = m_HighestIntersectHeight + FLYING_HEIGHT;
            }
            else
            {
                // Move upwards
                out_newPosition->y += distCanTravel;
            }
            positionUpdated = true;
        }
        else
        {
            positionUpdated = false;
        }
    }   


    return positionUpdated;
}

bool 
FlyingActor::FindGroundIntersection( float* out_height, rmt::Vector* normal )
{

    bool intersectionFound = false;

   	const float INTERSECT_TEST_RADIUS = 1.0f;  
	rmt::Vector deepestIntersectPos, deepestIntersectNormal;
    rmt::Vector position;
    GetPosition( &position );
    
    rmt::Vector deepPosition = position;
    deepPosition.y -= 50.0f;
    rmt::Vector objectIntersection;
    if ( m_IntersectionList.TestIntersectionStatics( position, deepPosition, &objectIntersection ) )
    {
        // Unfort we can't get normal information from colliding with a static or dynamic object.
        *normal = rmt::Vector( 0,1,0 );
        *out_height = objectIntersection.y;        
        intersectionFound = true;
    }
    else
    {
        rmt::Vector groundNormal, groundPlaneIntersectionPoint;
        bool foundPlane;
        GetIntersectManager()->FindIntersection( position,
                                                foundPlane,
                                                groundNormal,
                                                groundPlaneIntersectionPoint );      
        if ( foundPlane )
        {
            *normal = groundNormal;
            *out_height = groundPlaneIntersectionPoint.y;
            intersectionFound = true;
        }

    }

    if ( m_DesiredHeightEnabled && GetDesiredHeight() > *out_height )
    {
        *out_height = GetDesiredHeight();
        intersectionFound = true;
    }

    return intersectionFound;
}

//=============================================================================
// FlyingActor::RecieveEvent
//=============================================================================
// Description: Trap state prop state changes to notify sound system
//
// Parameters:  callback - event type
//              stateProp - state prop undergoing state change
//
// Return:      void 
//
//=============================================================================
void FlyingActor::RecieveEvent( int callback , CStateProp* stateProp )
{
    unsigned int newState;

    if( callback == STATEPROP_CHANGE_STATE_EVENT )
    {
        newState = stateProp->GetState();

        switch( newState )
        {
            case ActorEnum::eTransitionToReadyToAttack:
                GetEventManager()->TriggerEvent( EVENT_WASP_CHARGING, this );
                break;

            case ActorEnum::eIdleReadyToAttack:
                GetEventManager()->TriggerEvent( EVENT_WASP_CHARGED, this );
                break;

            case ActorEnum::eAttacking:
                GetEventManager()->TriggerEvent( EVENT_WASP_ATTACKING, this );
                break;

            case ActorEnum::eDestroyed:
                GetCharacterSheetManager()->IncNumWaspsDestroyed(GetGameplayManager()->GetCurrentLevelIndex());
                GetEventManager()->TriggerEvent( EVENT_WASP_BLOWED_UP, this );
                break;

            default:
                //
                // Unknown state, do nothing
                //
                break;
        }
    }
}
