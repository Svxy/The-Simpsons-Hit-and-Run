//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   
//
// Description: 
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/attackbehaviour.h>
#include <ai/actor/actor.h>
#include <worldsim/avatarmanager.h>
#include <math.h>
#include <constants/actorenum.h>
#include <render/IntersectManager/IntersectManager.h>
#include <ai/actor/actordsg.h>
#include <radtime.hpp>
#include <worldsim/character/character.h>
#include <main/game.h>
#include <events/eventenum.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>


//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================


// Designer tuning variables

const float ATTACK_BEHAVIOUR_MOVEMENT_INTERVALS = 3.0f;
const float ATTACK_BEHAVIOUR_CAM_ANGLE = 0.7f;
const float AVG_CHARACTER_HEIGHT = 1.2f;



//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

rmt::Randomizer AttackBehaviour::s_Randomizer(0);
bool AttackBehaviour::s_RandomizerSeeded = false;

//===========================================================================
// Member Functions
//===========================================================================

AttackBehaviour::AttackBehaviour( float maxFiringRange, float firingArc ) :
m_ActiveGagCount( 0 ),
m_InConversation( false )
{         
    const float DEFAULT_MOVE_SPEED = 20.0f;

    SetMovementIntervals( ATTACK_BEHAVIOUR_MOVEMENT_INTERVALS );
    SetFiringArc( firingArc );
    SetMaxFiringRange( maxFiringRange );  
    SetActorMoveSpeed( DEFAULT_MOVE_SPEED );

    if (!s_RandomizerSeeded)
    {
        s_Randomizer.Seed (Game::GetRandomSeed ());
        s_RandomizerSeeded = true;
    }

}

AttackBehaviour::~AttackBehaviour()
{

}

void
AttackBehaviour::Apply( Actor* actor, unsigned int timeInMS )
{

    if ( m_InConversation || m_ActiveGagCount > 0 )
        return;



    rmt::Vector targetPos;
    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer(0);
    Character* character = avatar->GetCharacter();
    avatar->GetPosition( targetPos );
    targetPos.y += AVG_CHARACTER_HEIGHT; // Lets not lock onto their feet

    rmt::Vector actorPos;
    actor->GetPosition( &actorPos );
    int currentAnimationState = actor->GetState();              
    actor->LookAt( targetPos, timeInMS );
    // Are we firing? Then just keep trained on the target and
    // thats it
    if ( currentAnimationState == ActorEnum::eAttacking )
        return;

    // Vlad wants the wasp to not use the characters current height if jumping
    // Lets check for this and compensate by using our current height
    if ( character->IsJumping() )
    {
        targetPos.y -= character->GetJumpHeight();
    }


    if ( actor->IsMoving() )
    {
        // Record that we are still moving
        m_TimeOfLastMove = radTimeGetMilliseconds();
        SetExclusive( true );
    }
    else
    {
        // Lets make the actor more jumpy
        // occasionally switch positions,
        // if the shield is on, test for line of sight.
        // if there is line of sight, never move
        // 
        unsigned int currentTime = radTimeGetMilliseconds();

      
        if ( IsTooClose( actorPos, targetPos ) && 
             character->IsJumping() == false &&
             IsMovementDisabled() == false )
        {
            MoveAway( actor, targetPos );
            return;
        }

        bool shouldMove;
        if ( IsMovementDisabled() )
            shouldMove = false;
        else
            shouldMove = m_TimeOfLastMove + m_MovementIntervals < currentTime;

        if ( shouldMove )
        {
            MoveIntoAttackRange( actor, targetPos );
        }
        else
        {
            rmt::Matrix actorTransform;
            actor->GetTransform( &actorTransform );


            switch ( currentAnimationState )
            {
            case ActorEnum::eIdleReadyToAttack:
                {
                // Are we in range?
                // Are we facing the target
                
                bool withinRange = WithinFiringRange( actorPos, targetPos );
                bool withinArc = WithinFiringArc( actorPos, actorTransform.Row(2), targetPos );

                if ( withinRange == false && 
                     IsMovementDisabled() == false )
                {
                    MoveIntoAttackRange( actor, targetPos );
                }
                if ( withinRange && withinArc )
                {
                    // Joe wants it so that they never attack when the user is in a 
                    // vehicle
                    // Test for this

                    if ( avatar->IsInCar() == false )
                    {
                        // Joe ALSO wants them to only fire when in the cameras FOV
                        // Sheesh, is this game supposed to be for toddlers or something?
                       
                        SuperCam* cam = GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
                        tPointCamera* pointCamera = cam->GetCamera();
                        if ( pointCamera->PointVisible( actorPos ) )
                        {
                            actor->SetState( ActorEnum::eAttacking );
                            SetExclusive( true );                          
                        }
                    }
                }
             
                break;
                // We are supposed to attack, can't do that if we are not in the proper animation
                // state
                }
            case ActorEnum::eTransitionToReadyToAttack:
                SetExclusive( false );
                break;
            case ActorEnum::eUnaggressive:
                actor->SetState( ActorEnum::eTransitionToReadyToAttack );
                SetExclusive( false );
                break;

                // We are currently attacking.
            case ActorEnum::eAttacking:
                // As long as we are attacking, don't do anything else
                SetExclusive( true );
                break;

            default:
                SetExclusive( false );
                break;
            };
        }
    }
}

void AttackBehaviour::SetMaxFiringRange( float meters )
{
    m_MaxFiringRange = meters;
    m_MaxFiringRangeSqr = meters * meters;



}

void AttackBehaviour::SetFiringArc( float firingArcDegrees )
{
    float firingArcRadians = firingArcDegrees * 3.1415927f / 180.0f;
    float halfArc = firingArcRadians / 2.0f;
    m_FiringArc = static_cast< float > ( fabs ( cos( halfArc ) )) ;
}

void AttackBehaviour::SetActorMoveSpeed( float kph )
{
    // Convert from kph to meters per millisecond
    // Precision loss here?
    m_Speed = kph * 1000.0f / 3600000.0f;    
    
}   

void AttackBehaviour::SetMovementIntervals( float seconds )
{

    // Convert seconds to milliseconds and store it
    m_MovementIntervals = seconds * 1000.0f;
}

bool AttackBehaviour::IsMovementDisabled()const
{
    // No movement if designers set movement interval to be < 0
    return ( m_MovementIntervals < 0.0f );
}


// Enable this behaviour
void AttackBehaviour::Activate()
{
    // Register for events
    GetEventManager()->AddListener( this, EVENT_GAG_START );
    GetEventManager()->AddListener( this, EVENT_GAG_END );

    GetEventManager()->AddListener( this, EVENT_CONVERSATION_START );
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE );

    m_TimeOfLastMove = 0;
    m_ActiveGagCount = 0;
    m_InConversation = false;
    SetExclusive( false );
}
// Disable this behaviour
void AttackBehaviour::Deactivate()
{
    GetEventManager()->RemoveAll( this );

    m_ActiveGagCount = 0;
    m_InConversation = false;
    SetExclusive( false );
}


bool AttackBehaviour::WithinFiringRange( const rmt::Vector& actorPos, const rmt::Vector& target )const
{
    bool withinRange;
    
    rmt::Vector toTarget = target - actorPos;
    float rangeSqr = toTarget.MagnitudeSqr();
    if ( rangeSqr < m_MaxFiringRangeSqr )
    {
        withinRange = true;         
    }
    else
    {
        withinRange = false;
    }

    return withinRange;
}

bool AttackBehaviour::WithinFiringArc( const rmt::Vector& actorPos, const rmt::Vector& actorFacing, const rmt::Vector& target ) const
{
    bool withinArc;
    
    // Get the toTarget vector
    rmt::Vector toTarget = target - actorPos;
    toTarget.Normalize();
    // Get the cos of the angle between the 2 vectors
    float cosAngle = toTarget.Dot( actorFacing );
    if ( cosAngle > m_FiringArc || cosAngle < -m_FiringArc )
    {
        withinArc = true;
    }
    else 
    {
        withinArc = false;
    }
    return withinArc;
}
        
bool 
AttackBehaviour::IsTooClose( const rmt::Vector& actorPos, const rmt::Vector& target )const
{
    const float TOO_CLOSE_DIST = 2.0f;
    const float TOO_CLOSE_DIST_SQR = TOO_CLOSE_DIST * TOO_CLOSE_DIST;

    bool tooClose;
    if (( actorPos - target ).MagnitudeSqr() < TOO_CLOSE_DIST_SQR )
        tooClose = true;
    else 
        tooClose = false;

    return tooClose;
}


void AttackBehaviour::MoveIntoAttackRange( Actor* actor, const rmt::Vector& target )
{
    // We want it to be inside the camera frustum
    // 

    SuperCam* camera = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam();
    if ( camera == NULL )
        return;

    rmt::Vector cameraHeading;
    camera->GetHeading( &cameraHeading );
    cameraHeading.y = 0;
    cameraHeading.Normalize();

    // Add a random twist to the camera along world Y axis
    float rotationAngle = s_Randomizer.FloatSigned() * ATTACK_BEHAVIOUR_CAM_ANGLE;

    rmt::Matrix randomCameraRotation;
    randomCameraRotation.Identity();
    randomCameraRotation.FillRotateY( rotationAngle );

    cameraHeading.Rotate( randomCameraRotation );

    rmt::Vector destination = target + cameraHeading * m_MaxFiringRange * 0.5f ;
    destination.y += ( 1.5f + s_Randomizer.Float() * 0.5f );
    rmt::Vector actorPosition;
    actor->GetPosition( &actorPosition );
    actor->MoveTo( destination, m_Speed );
}

void 
AttackBehaviour::MoveAway( Actor* actor, const rmt::Vector& target )
{
    rmt::Vector actorPosition;
    actor->GetPosition( &actorPosition );
    // Get the vector pointing from the target to the actor
    rmt::Vector toActor = actorPosition - target;
    toActor.Normalize();
    
    const float EVADE_DIST = 5.0f;

    rmt::Vector destination = actorPosition + toActor * EVADE_DIST;
    destination.y = actorPosition.y;
    // Move directly away from the target
    actor->MoveTo( destination, m_Speed );
}

void 
AttackBehaviour::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
        case EVENT_GAG_START:
            m_ActiveGagCount++;
        break;

        case EVENT_GAG_END:
            m_ActiveGagCount--;
        break;


        case EVENT_CONVERSATION_START:
            m_InConversation = true;
            break;
        case EVENT_CONVERSATION_DONE:
            m_InConversation = false;
            break;
        default:
            break;

    }

}