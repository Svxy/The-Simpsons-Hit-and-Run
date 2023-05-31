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

#include <ai/actor/UFOAttackBehaviour.h>
#include <ai/actor/actor.h>
#include <worldsim/avatarmanager.h>
#include <math.h>
#include <ai/actor/actormanager.h>
#include <constants/actorenum.h>
#include <render/IntersectManager/IntersectManager.h>
#include <worldsim/character/character.h>
#include <worldsim/vehiclecentral.h>
#include <ai/actor/actordsg.h>
#include <p3d/entity.hpp>
#include <ai/actor/flyingactor.h>
#include <worldsim/character/charactermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/rendermanager/rendermanager.h>
#include <main/game.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================


// Designer tuning variables

#ifndef RAD_RELEASE
    static float s_DesignerTunableMaxUFOFiringRange = 20.0f;
#endif


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

rmt::Randomizer UFOAttackBehaviour::s_Randomizer(0);
bool UFOAttackBehaviour::s_RandomizerSeeded = false;


const float UFO_MOVEMENT_SPEED = 0.005f;
const float UFO_HEIGHT_ABOVE_GROUND = 20.0f;
const float UFO_CRUISE_HEIGHT = 25.0f;

const int TRACTOR_BEAM_STATE_ACTIVATE = 0;
const int TRACTOR_BEAM_STATE_DEACTIVATE = 1;
const int TRACTOR_BEAM_STATE_IDLE_ON = 2;


//===========================================================================
// Member Functions
//===========================================================================

UFOAttackBehaviour::UFOAttackBehaviour( float maxFiringRange ):
m_TargetList( 50 ),
m_CurrentTarget( 0 ),
m_CurrentState( eSearching )
{         
    const float DEFAULT_MOVE_SPEED = 20.0f;

    SetMaxFiringRange( maxFiringRange );  
    SetActorMoveSpeed( DEFAULT_MOVE_SPEED );
    // Try and find the stateprop in the inventory section

    if (!s_RandomizerSeeded)
    {
        s_Randomizer.Seed (Game::GetRandomSeed ());
        s_RandomizerSeeded = true;
    }
}

UFOAttackBehaviour::~UFOAttackBehaviour()
{   
}

void
UFOAttackBehaviour::Apply( Actor* actor, unsigned int timeInMS )
{
    // Force update of designer tunable parameters
#ifndef RAD_RELEASE
    
    static bool addedDesignerVariablesToWatcher = false;
    if ( addedDesignerVariablesToWatcher == false )
    {
        radDbgWatchAddFloat( &s_DesignerTunableMaxUFOFiringRange, "Max firing range (meters)", "Actor", NULL,NULL,1, 75 );
        addedDesignerVariablesToWatcher = true;
    }
    SetMaxFiringRange( s_DesignerTunableMaxUFOFiringRange );
#endif

    // We know that the ufo is a flying actor
    rAssert( dynamic_cast< FlyingActor* >( actor ) != NULL );
    FlyingActor* flyingActor = static_cast< FlyingActor* >( actor );
    flyingActor->SetDesiredHeight( UFO_HEIGHT_ABOVE_GROUND );
    flyingActor->SetDesiredHeightEnabled( true );

    rmt::Vector actorPosition;
    actor->GetPosition( &actorPosition );

    if ( actor->IsTractorBeamOn() )
    {

        // Tractor beam is on,
        // anything under the beam is toast.
        // fetch a list of all the targets under the beam
        SwapArray< DynaPhysDSG* > dynaPhysList( 200 );
        GetTargetsInTractorRange( actorPosition, UFO_HEIGHT_ABOVE_GROUND * 1.5f, &dynaPhysList );
        // Iterate through the list and destroy them
        int objectsLeft = 0;
        for ( int i = 0 ; i < dynaPhysList.mUseSize ; i++ )
        {
            if (SuckIntoUFO( actorPosition, dynaPhysList[i], static_cast< float >( timeInMS ) ) )
            {
                objectsLeft++;
            }
            if ( ReachedTopOfTractorBeam( actorPosition, dynaPhysList[i] ) )
            {
                // Destroy the object, and test to see if it damaged the UFO as well
                if ( DestroyObject( dynaPhysList[i] ) )
                    actor->SetState( 1 );

                objectsLeft--;
            }
        }

        if ( objectsLeft <= 0 )
        {
            actor->DeactivateTractorBeam();
        }
    }
    else
    {
        // Do we have a target
        // if not, get one, then start moving toward it
        if ( m_CurrentTarget == NULL )
        {
            DynaPhysDSG* target = FindTarget( actorPosition, actor->GetDSG() );
            if ( target != NULL )
            {
                // Lets move
                rmt::Vector targetPosition;
                target->GetPosition( &targetPosition );
                targetPosition.y += UFO_HEIGHT_ABOVE_GROUND;

                actor->MoveTo( targetPosition, UFO_MOVEMENT_SPEED );
                m_CurrentTarget = target;
            }
        }
        else
        {
            // if we do have a target
            // check to see if we are in range

            if ( WithinTractorBeamRange( actorPosition, m_CurrentTarget ) )
            {
                // We are within range, suck it up
                actor->ActivateTractorBeam();
                m_CurrentTarget = NULL;
            }
            else
            {
                rmt::Vector targetPosition;
                m_CurrentTarget->GetPosition( &targetPosition );
                actor->MoveTo( targetPosition, UFO_MOVEMENT_SPEED );
            }

        }
    }
        

 
}

void UFOAttackBehaviour::SetMaxFiringRange( float meters )
{
    m_MaxFiringRange = meters;
    m_MaxFiringRangeSqr = meters * meters;

#ifndef RAD_RELEASE
    s_DesignerTunableMaxUFOFiringRange = meters;
#endif

}


void UFOAttackBehaviour::SetActorMoveSpeed( float kph )
{
    // Convert from kph to meters per millisecond
    // Precision loss here?
    m_Speed = kph * 1000.0f / 3600000.0f;    
    
}   

// Enable this behaviour
void UFOAttackBehaviour::Activate()
{
    SetExclusive( false );
}
// Disable this behaviour
void UFOAttackBehaviour::Deactivate()
{
    SetExclusive( false );
}

  
  

        
bool UFOAttackBehaviour::FindPositionInAttackRange( const rmt::Vector& actorPos, const rmt::Vector& targetPos, rmt::Vector* out_destination )
{
    bool success;
    // Find a position close to the target, lets fudge the data a bit
    // make it always 1 meter in ZY from the target

    *out_destination = targetPos;
    out_destination->y += 2.0f;
    out_destination->z += 2.0f;
    success = true;
    return success;
}
        

void UFOAttackBehaviour::MoveIntoAttackRange( const Actor& actor, const rmt::Vector& target )
{
  

}

DynaPhysDSG* 
UFOAttackBehaviour::FindTarget( const rmt::Vector& actorPosition, const ActorDSG* dsgObject )
{
    // Iterate through each pedestrian in the charactermanager and find the closest
    // one
    int i;
    DynaPhysDSG* target = NULL;
    float closestDistance = 50.0f * 50.0f;
    /*
    for ( i = 0 ; i < GetCharacterManager()->GetMaxCharacters() ; i++ )
    {
        Character* character = GetCharacterManager()->GetCharacter( i );
        if ( character != NULL )
        {
            rmt::Vector characterPosition;
            character->GetPosition( &characterPosition );
            float distSqr = (characterPosition - actorPosition).MagnitudeSqr();
            if ( distSqr < closestDistance )
            {
                // We want to see if there is a clear line of sight, between the UFO
                // and the target character.
                // LineOfSight will think that the targetposition is occulded by the
                // target itself, so set the lineofsight target position to 
                // be raised above the character
                rmt::Vector pointAboveCharacter = characterPosition;
                pointAboveCharacter.y += 2.5f;

                if ( GetIntersectManager()->LineOfSight( actorPosition, pointAboveCharacter, dsgObject ) )
                {
                    closestDistance = distSqr;
                    target = character;
                }
            }
        }
    }*/
    // Iterate through each vehicle and find a target
    // As a test, lets just grab the players vehicle
    int numActiveVehicles;
    Vehicle** activeVehicleList;
    GetVehicleCentral()->GetActiveVehicleList( activeVehicleList, numActiveVehicles );
    for ( i = 0 ; i < numActiveVehicles ; i++ )
    {
        if ( activeVehicleList[i] != NULL )
        {
            Vehicle* activeVehicle = activeVehicleList[i];

            if ( activeVehicle->IsVehicleDestroyed() == false )
            {
                rmt::Vector vehiclePosition;
                activeVehicle->GetPosition( &vehiclePosition );
                float distToVehicleSqr = ( vehiclePosition - actorPosition ).MagnitudeSqr();
                if ( distToVehicleSqr < closestDistance )
                {
                    closestDistance = distToVehicleSqr;
                    target = activeVehicle;
                }               
            }
        }
    }


    return target;
}





