//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ufobehaviour
//
// Description:
//              
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai\actor\ufobehaviour.h>
#include <ai\actor\actor.h>
#include <ai\actor\flyingactor.h>
#include <worldsim\redbrick\vehicle.h>
#include <worldsim\vehiclecentral.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/rendermanager/rendermanager.h>
#include <meta/eventlocator.h>
#include <worldsim/character/character.h>
#include <render/breakables/breakablesmanager.h>
#include <radtime.hpp>
#include <worldsim/avatarmanager.h>
#include <mission/statepropcollectible.h>
#include <camera/supercammanager.h>
#include <mission/gameplaymanager.h>
#include <camera/supercamcentral.h>
#include <ai/state.h>
#include <ai/statemanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactercontroller.h>
#include <worldsim/character/charactermanager.h>
#include <ai/sequencer/action.h>
#include <ai/sequencer/actioncontroller.h>
#include <ai/sequencer/sequencer.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

// How fast objects move up the beam
const float TRACTOR_BEAM_SUCK_SPEED = 0.003f;
// How close an object being sucked up has to be to the center before it gets sucked up forever
const float UFO_TRACTOR_BEAM_DISAPPEAR_RADIUS_SQR = 25.0f;

const float UFO_TRACTOR_BEAM_RADIUS = 7.5f;
const float UFO_TRACTOR_BEAM_RADIUS_SQR = UFO_TRACTOR_BEAM_RADIUS * UFO_TRACTOR_BEAM_RADIUS;

// The cosine of the angle of the tractor beam
const float TRACTOR_BEAM_ANGLE_COS = 0.94f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================



//===========================================================================
// Member Functions
//===========================================================================


UFOBehaviour::UFOBehaviour() :
m_Rotation( 0 )
{
    // Lets just make a default respawn position until the designers script one in.
    m_RespawnLocator = new EventLocator();
    m_RespawnLocator->AddRef();
}

UFOBehaviour::~UFOBehaviour()
{
    if ( m_RespawnLocator != NULL )
    {
        m_RespawnLocator->Release();
        m_RespawnLocator = NULL;
    }
}

void
UFOBehaviour::Apply( Actor* actor, unsigned int timeInMS )
{
    
}

void
UFOBehaviour::SetCharacterRespawnPosition( EventLocator* locator )
{
    rmt::Vector respawnLocation;
    locator->GetLocation( &respawnLocation );
    rmt::Matrix matrix;
    m_RespawnLocator->SetMatrix( locator->GetMatrix() );
    m_RespawnLocator->SetLocation( respawnLocation );
    m_RespawnLocator->SetEventType( LocatorEvent::DEATH );
}

bool 
UFOBehaviour::WithinTractorBeamRange( const rmt::Vector& position, IEntityDSG* dsg )const
{
    if ( dsg == NULL )
        return false;

    bool withinRange;

    rmt::Vector targetPos;
    dsg->GetPosition( &targetPos );

    // Compare distance in x and z only
    float deltaX = targetPos.x - position.x;
    float deltaZ = targetPos.z - position.z;
    float distSqr = (deltaX*deltaX) + (deltaZ*deltaZ);  
    if ( distSqr < UFO_TRACTOR_BEAM_RADIUS_SQR )
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
UFOBehaviour::SuckIntoUFO( const rmt::Vector& actorPosition, DynaPhysDSG* pDSG, float timeInMS )
{
    bool wasSucked;
    


    sim::SimState* simstate = pDSG->GetSimState();
    if ( simstate == NULL )
        return false;

    const rmt::Matrix& transform = simstate->GetTransform();
    const rmt::Vector& objectPos = transform.Row(3);
    rmt::Vector toTopOfBeam = actorPosition - objectPos;
    toTopOfBeam.Normalize();    

    // Move position upwards 
    rmt::Vector velocityVector = timeInMS * TRACTOR_BEAM_SUCK_SPEED * toTopOfBeam;

    
    // Apply a rotation
  //  rmt::Vector rotationAxis = GetPseudoRandomRotationAxis( reinterpret_cast< int >( pDSG ) );
//
  //  rmt::Matrix rotationMatrix;
  //  rotationMatrix.Identity();
//    rotationMatrix.FillRotation( rotationAxis, timeInMS * 0.001f );

    //rmt::Matrix translation;
    //translation.Identity();
  //  translation.FillTranslate( velocityVector );

    rmt::Matrix newTransform = transform;
    newTransform.Row(3) += velocityVector;
   // newTransform.Mult( rotationMatrix );

    int aiRef = pDSG->GetAIRef();

    switch (aiRef)
    {
    case PhysicsAIRef::redBrickVehicle:
        {
            rAssert( dynamic_cast< Vehicle* >( pDSG ) != NULL );
            Vehicle* vehicle = static_cast< Vehicle* >( pDSG );
            // Don't let the chase cars get sucked up (JeffP)
            if ( vehicle->mVehicleID != VehicleEnum::CBLBART && vehicle->IsVehicleDestroyed() == false
                && vehicle->mVehicleID != VehicleEnum::HUSKA )
            {
                pDSG->GetSimState()->ResetVelocities();
                vehicle->SetTransform( newTransform );
                vehicle->SetSimpleShadow( false );          
                wasSucked = true;
				// If we are in bumper cam, we need to force out.
			    SuperCamCentral* scc = GetSuperCamManager()->GetSCC(0);
				if(scc->GetActiveSuperCam()->GetType() == SuperCam::BUMPER_CAM)
				{
					// Need to switch out of bumper cam.
					scc->SelectSuperCam( SuperCam::FAR_FOLLOW_CAM, SuperCamCentral::FORCE | SuperCamCentral::CUT, 0 );
				}
				if(vehicle->IsUserDrivingCar())
				{
					Character* c = GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();
					if(c)
					{
				        c->GetActionController()->Clear();
						c->GetStateManager()->SetState<CharacterAi::GetOut>();
					}
				}
            }
            else
            {
                wasSucked = false;
            }

        }
        break;
    default:
        wasSucked = false;
        break;
    }

    return wasSucked;
}


bool 
UFOBehaviour::ReachedTopOfTractorBeam( const rmt::Vector& actorPosition, DynaPhysDSG* pDSG )
{
    bool atTop;
    rmt::Vector objectPos;
    pDSG->GetPosition( &objectPos );
    float distToTop = ( actorPosition - objectPos ).MagnitudeSqr();
    if ( distToTop < UFO_TRACTOR_BEAM_DISAPPEAR_RADIUS_SQR )
    {
        atTop = true;
    }
    else
    {
        atTop = false;
    }
    return atTop;
}

bool 
UFOBehaviour::DestroyObject( DynaPhysDSG* object )
{
    bool damagedUFO = false;
    // We need to destroy the object. Perform any cleanup activities required for specific vehicle
    // type
    int airef = object->GetAIRef();
    switch ( airef )
    {
    case PhysicsAIRef::ActorStateProp:
        break;
    case PhysicsAIRef::PlayerCharacter:
        {
//-            GetEventManager()->TriggerEvent( (EventEnum)(EVENT_LOCATOR + LocatorEvent::DEATH) , NULL );
			assert(dynamic_cast<Character*>(object));
			GetEventManager()->TriggerEvent( EVENT_ABDUCTED, static_cast<Character*>(object) );
            if ( !GetAvatarManager()->IsAvatarGettingInOrOutOfCar( 0 ) )
            {
                // Lets set a default spawn position since the designers aren't here
			    SetCharacterRespawnPosition( p3d::find< EventLocator >( "Z3D1" ) );
    //			Character* character = GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();
    //			rAssert( character!= NULL );
    //			GetAvatarManager()->PutCharacterOnGround(character, vehicle);
			    m_RespawnLocator->Trigger( 0, true );
            }
        }
        break;
    case PhysicsAIRef::redBrickPhizMoveable:
    case PhysicsAIRef::StateProp:
    case PhysicsAIRef::redBrickPhizMoveableAnim:
        // Remove the dynaphys object from the worldsim
        {


            WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
            // Sanity check
            rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
            pWorldRenderLayer->pWorldScene()->Remove( object );   
        }
        break;
    case PhysicsAIRef::redBrickVehicle:
        {
            rAssert( dynamic_cast< Vehicle* >( object ) != NULL );
            Vehicle* vehicle = static_cast< Vehicle* >( object );
            // Ok, there are two cases. One is that the vehicle is a player vehicle
            // the other is that it is not.
            // If the object is a player vehicle

            bool characterAbducted = false;
            if ( vehicle->IsUserDrivingCar() && vehicle->GetDriver() != NULL && !GetAvatarManager()->IsAvatarGettingInOrOutOfCar( 0 ))
            {
                // Player got sucked up, better tell, someone.  ( NotAbductedCondition uses this )
                GetEventManager()->TriggerEvent( EVENT_ABDUCTED, vehicle->GetDriver() );
                characterAbducted = true;
                // Lets set a default spawn position since the designers aren't here
                SetCharacterRespawnPosition( p3d::find< EventLocator >( "Z3D1" ) );
                Character* character = GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();
                rAssert( character!= NULL );
                GetAvatarManager()->PutCharacterOnGround(character, vehicle);
                m_RespawnLocator->Trigger( 0, true );
            }   
            else
            {
                vehicle->TriggerDamage( FLT_MAX, false );
            }

            StatePropCollectible* collectible = vehicle->GetAttachedCollectible();
            if ( collectible != NULL )
            {
                vehicle->DetachCollectible(rmt::Vector(0,0,0), false );
                collectible->RemoveFromDSG();
                // The vehicle is carrying a collectible.
                // Well, there is only one type of collectible in the game
                // i.e. the exploding barrels
                // so trigger a boss damaged event   
                // later we may want to switch on collectible type
                GetEventManager()->TriggerEvent( EVENT_BOSS_DAMAGED, NULL );
                // Note that the FMVs show the vehicle just disappearing, not being blown
                // up, so lets just make it go away via RemoveVehicleFromActiveList
                GetVehicleCentral()->RemoveVehicleFromActiveList( vehicle );
                damagedUFO = true;
            }
            else
            {
                // The vehicle is not carrying anything
                // So the UFO ate the player's vehicle
                // signal the destroybossobjective object that
                // the mission has failed
            }
            rmt::Matrix transform;
            transform.Identity();
            transform.FillTranslate( object->rPosition() );
            GetBreakablesManager()->Play( BreakablesEnum::eCarExplosion, transform );
            GetEventManager()->TriggerEvent( EVENT_BIG_BOOM_SOUND, vehicle );

            GetEventManager()->TriggerEvent( EVENT_BIG_BOOM_SOUND, vehicle );
        }
        break;
    default:
        break;
    }
    return damagedUFO;
}


void 
UFOBehaviour::GetTargetsInTractorRange( rmt::Vector& actorPosition, float maxRange, SwapArray< DynaPhysDSG* >* targetList )
{
    targetList->ClearUse();
    ReserveArray< DynaPhysDSG* > dynaPhysList(20);
    GetIntersectManager()->FindDynaPhysElems( actorPosition, maxRange, dynaPhysList );           
    // Copy all the characters into the array
    for ( int i = 0 ; i < dynaPhysList.mUseSize ; i++ )
    {
        // Do an angle check to see that its inside the cone formed by the tractor beam
        rmt::Vector targetPosition;
        dynaPhysList[ i ]->GetPosition( &targetPosition );
        rmt::Vector toTarget = targetPosition - actorPosition;
        toTarget.Normalize();
        // Vector straight down, which is the direction that the tractor beam
        // will be facing
        rmt::Vector down( 0, -1.0f, 0 );
        float distanceToObject = toTarget.MagnitudeSqr(); 
        if ( toTarget.Dot( down ) > TRACTOR_BEAM_ANGLE_COS  )
        {
            targetList->Add( dynaPhysList[i] );           
        }
    }
}


rmt::Vector 
UFOBehaviour::GetPseudoRandomRotationAxis( int hash )const
{
    unsigned int timeInSec = radTimeGetSeconds();
    rmt::Vector axis;
    axis.x = static_cast< float >( timeInSec & 0xF0 );
    axis.y = static_cast< float >( timeInSec & 0xF00 );
    axis.z = static_cast< float >( timeInSec & 0xF000 );

    axis.x -= 8.0f;
    axis.y -= 8.0f;
    axis.z -= 8.0f;    

    axis.Normalize();
    return axis;
}
