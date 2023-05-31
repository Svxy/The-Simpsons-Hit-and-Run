//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AttractionBehaviour
//
// Description: This behaviour has two states. In the first state, it will
//              cause the actor to remain motionless until something of interest
//              happens. Then this happens, the actor will move down to near the avatar
//              and watch him until the avatar does something violent
//              
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/attractionbehaviour.h>
#include <ai/actor/actordsg.h>
#include <ai/actor/actor.h>
#include <ai/actor/flyingactor.h>
#include <events/eventmanager.h>
#include <worldsim/avatarmanager.h>
#include <memory/srrmemory.h>
#include <render/IntersectManager/IntersectManager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================




//===========================================================================
// Member Functions
//===========================================================================


AttractionBehaviour::AttractionBehaviour( float minWatchDistance, float maxWatchDistance, float speedKPH ):
m_CurrentState( eIdle ),
m_SensoryRangeSqr( maxWatchDistance * maxWatchDistance ),
m_MinWatchDistanceSqr( minWatchDistance * minWatchDistance ),
m_MaxWatchDistanceSqr( maxWatchDistance * maxWatchDistance ),
m_ForceFindNewWatchPosition( false ),
m_ParentActor( NULL )
{
    m_Speed = speedKPH * 1000.0f / 3600000.0f;    
    m_WatchDistanceFromAvatar = ( minWatchDistance + maxWatchDistance ) / 2.0f;

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    SetExclusive( true );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_OBJECT_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_BIG_CRASH );
    GetEventManager()->AddListener( this, EVENT_BIG_VEHICLE_CRASH );
    GetEventManager()->AddListener( this, EVENT_BIG_AIR );
    GetEventManager()->AddListener( this, EVENT_PEDESTRIAN_DODGE );
    GetEventManager()->AddListener( this, EVENT_PEDESTRIAN_SMACKDOWN );
    GetEventManager()->AddListener( this, EVENT_BREAK_CAMERA_OR_BOX );
    GetEventManager()->AddListener( this, EVENT_GAG_END );
    GetEventManager()->AddListener( this, EVENT_PC_NPC_COLLISION );
    GetEventManager()->AddListener( this, EVENT_PLAYER_CAR_HIT_NPC );
    GetEventManager()->AddListener( this, EVENT_HIT_BREAKABLE );
    GetEventManager()->AddListener( this, EVENT_OBJECT_KICKED );
    GetEventManager()->AddListener( this, EVENT_JUMP_LANDING );    

    GetEventManager()->AddListener( this, EVENT_KICK );
    GetEventManager()->AddListener( this, EVENT_STOMP );
    GetEventManager()->AddListener( this, EVENT_DOUBLEJUMP );
    GetEventManager()->AddListener( this, EVENT_COLLECT_OBJECT );    


    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}

AttractionBehaviour::~AttractionBehaviour()
{
    GetEventManager()->RemoveAll( this );
}

void
AttractionBehaviour::Apply( Actor* actor, unsigned int timeInMS )
{
    m_ParentActor = actor;

    // Make the actor stay fixed
    FlyingActor* flyingactor = static_cast< FlyingActor* >( actor );
    flyingactor->SetDesiredHeightEnabled( false );

    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer(0);
    rAssert( avatar != NULL );

    rmt::Vector avatarPosition;
    avatar->GetPosition( avatarPosition );
    rmt::Vector actorPosition;
    actor->GetPosition( &actorPosition );

    if ( m_ForceFindNewWatchPosition )
    {
        rmt::Vector newWatchPosition;

        if ( FindNewWatchPosition( *m_ParentActor, avatarPosition, &newWatchPosition ) )
        {   
            m_ParentActor->MoveTo( newWatchPosition, m_Speed );
            m_ForceFindNewWatchPosition = false;
        }
    }
    float distanceToTargetSqr = (actorPosition - avatarPosition).MagnitudeSqr();
    const float waspRadius = 5.0f;
    if( distanceToTargetSqr < waspRadius * waspRadius )
    {
        //
        // Send a message about approaching a wasp
        //
        GetEventManager()->TriggerEvent( EVENT_WASP_APPROACHED );
    }

    if ( m_CurrentState == eIdle )
    {
        if ( distanceToTargetSqr < m_MinWatchDistanceSqr )
        {
            // The target is too close!
            // Attraction go into watch mode
     //       SetExclusive( false );
        }
    }
    else if ( m_CurrentState == eWatching )
    {   
        // Basically, we want the actor to watch the avatar, follow him around while
        // keeping his distance
        // Lets first check the distance to the avatar


        if ( distanceToTargetSqr < m_MinWatchDistanceSqr )
        {
            // The target is too close!
            // Attraction behaviour is over, lets bail
            SetExclusive( false );
        }
        else if ( IsMovementDisabled() == false && distanceToTargetSqr > m_MaxWatchDistanceSqr )
        {
            // The target is too far!
            // Lets move close
            rmt::Vector newWatchPosition;
            if ( FindNewWatchPosition( *actor, avatarPosition, &newWatchPosition ) )
            {   
                actor->MoveTo( newWatchPosition, m_Speed );
            }
        }
        else
        {
            actor->LookAt( avatarPosition, timeInMS );
        }
    }
}


void 
AttractionBehaviour::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_COLLECT_OBJECT:
        {
            // Hostile action was taken
            if( m_CurrentState == eWatching )
            {
                SetExclusive( false );
            }
        }
    case EVENT_KICK:
    case EVENT_STOMP:
    case EVENT_DOUBLEJUMP:
    case EVENT_VEHICLE_DESTROYED:
    case EVENT_OBJECT_DESTROYED:
    case EVENT_BIG_CRASH:
    case EVENT_BIG_VEHICLE_CRASH:
    case EVENT_BIG_AIR:
    case EVENT_PEDESTRIAN_DODGE:
    case EVENT_PEDESTRIAN_SMACKDOWN:
    case EVENT_BREAK_CAMERA_OR_BOX:
    case EVENT_GAG_END:
    case EVENT_PC_NPC_COLLISION:
    case EVENT_PLAYER_CAR_HIT_NPC:
    case EVENT_HIT_BREAKABLE:
    case EVENT_OBJECT_KICKED:
    case EVENT_JUMP_LANDING:
        {
            if ( m_ParentActor != NULL )
            {
                Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer(0);
                if ( WithinSensoryRange( *m_ParentActor, *avatar ) )
                {
                    switch (m_CurrentState)
                    {
                    case eIdle:
                        {
                            // Make the object fly down to the actor
                            m_CurrentState = eWatching;
                            m_ForceFindNewWatchPosition = true;
                        }
                        break;
                    default:
                        break;
                    };
                }
            }
        }
        break;
    default:
        rAssert( false );
        break;
    };
}

void AttractionBehaviour::Activate()
{   
    m_CurrentState = eWatching;
    SetExclusive( true );
}

void AttractionBehaviour::Deactivate()
{
    m_CurrentState = eIdle;
    m_ForceFindNewWatchPosition = false;
    SetExclusive( false );    

    if ( m_ParentActor != NULL )
    {
        FlyingActor* flyingactor = static_cast< FlyingActor* >( m_ParentActor );
    }
}

bool 
AttractionBehaviour::WithinSensoryRange( const Actor& actor, const Avatar& avatar )const
{
    bool withinRange;
    rmt::Vector actorPosition;
    actor.GetPosition( &actorPosition );

    rmt::Vector avatarPosition;
    avatar.GetPosition( avatarPosition );

    if ( (avatarPosition - actorPosition).MagnitudeSqr() < m_SensoryRangeSqr )
    {
        withinRange = true;
    }
    else
    {
        withinRange = false;
    }
    return withinRange;
}

bool
AttractionBehaviour::FindNewWatchPosition( const Actor& actor, const rmt::Vector& avatarPosition, rmt::Vector* newPosition )const
{
    // We want to find a new position halfway between min and max distance
    // along the line of sight
    rmt::Vector actorPosition;
    actor.GetPosition( &actorPosition );

    rmt::Vector toAvatar = avatarPosition - actorPosition;
    toAvatar.Normalize();

    *newPosition = actorPosition + toAvatar * m_WatchDistanceFromAvatar;

    return true;
}

