//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        state.cpp
//
// Description: Implementation of class State
//
// History:     6/12/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <ai/state.h>
#include <ai/statemanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactercontroller.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/hitnrunmanager.h>

#include <ai/sequencer/action.h>
#include <ai/sequencer/actioncontroller.h>
#include <ai/sequencer/sequencer.h>
#include <ai/actionbuttonhandler.h>

#include <events/eventmanager.h>
#include <events/eventenum.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/vehiclecontroller/vehiclecontroller.h>
#include <worldsim/redbrick/vehiclecontroller/vehiclemappable.h>
#include <worldsim/redbrick/vehiclecontroller/humanvehiclecontroller.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/worldphysicsmanager.h>
#include <simcollision/collisionmanager.hpp>

#include <worldsim/avatarmanager.h>

#include <interiors/interiormanager.h>

#include <gameflow/gameflow.h>
#include <main/game.h>

#include <meta/locator.h>

#include <cheats/cheatinputsystem.h>
#include <cheats/cheats.h>
#include <gameflow/gameflow.h>
#include <contexts/contextenum.h>

#include <camera/relativeanimatedcam.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>

#include <contexts/context.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreeniriswipe.h>


namespace CharacterAi
{

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
const int MAX_CLASS_SIZE = 32; // bytes.
FBMemoryPool State::sMemoryPool( MAX_CLASS_SIZE, 32, GMA_LEVEL_OTHER );

static const char* sGetOutPassenger[] =
{
    "get_out_of_car_open_door",
    "get_out_of_car",
    "get_out_of_car_close_door"
};

static const char* sGetOutDriver[] =
{
    "get_out_of_car_open_door_driver",
    "get_out_of_car_driver",
    "get_out_of_car_close_door_driver"
};

static const char* sGetOutHighPassenger[] =
{
    "get_out_of_car_open_door",
    "get_out_of_car_high",
    "get_out_of_car_close_door_high"
};

static const char* sGetOutHighDriver[] =
{
    "get_out_of_car_open_door_driver",
    "get_out_of_car_high_driver",
    "get_out_of_car_close_door_high_driver"
};


static const char* sGetInPassenger[] =
{
    "get_into_car_open_door",
    "get_into_car",
    "get_into_car_close_door"
};

static const char* sGetInDriver[] =
{
    "get_into_car_open_door_driver",
    "get_into_car_driver",
    "get_into_car_close_door_driver"
};

static const char* sGetInHighPassenger[] =
{
    "get_into_car_open_door_high",
    "get_into_car_high",
    "get_into_car_close_door"
};

static const char* sGetInHighDriver[] =
{
    "get_into_car_open_door_high_driver",
    "get_into_car_high_driver",
    "get_into_car_close_door_driver"
};


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************


//==============================================================================
// State::State
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
State::State( Character* pCharacter )
:
mpCharacter( pCharacter )
{
}

//==============================================================================
// State::~State
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
State::~State()
{
    mpCharacter = 0;
}

/*
==============================================================================
InCar::InCar
==============================================================================
Description:    Comment

 Parameters:( Character* pCharacter )
 
Return:         InCar

=============================================================================
*/
InCar::InCar( Character* pCharacter )
:
State( pCharacter )
{
    m_InCarAction = new InCarAction(pCharacter);
    m_InCarAction->AddRef();

    m_GetOutState = GETOUT_NONE;
}

InCar::~InCar( void )
{
    m_InCarAction->Release();
}


void InCar::Enter( void )
{
    if(!mpCharacter->GetTargetVehicle())
    {
        return;
    }

    mpCharacter->DoGroundIntersect(false);

    m_GetOutState = GETOUT_NONE;

    if(mpCharacter == GetCharacterManager()->GetCharacter(0))
    {
        GetVehicleCentral()->ActivateVehicleTriggers(false);
    }

    // if we did a git in, in car shoudl already be true, if not, need to force a little
    if(!mpCharacter->IsInCar()) 
    {
        mpCharacter->GetActionController()->Clear();
        mpCharacter->SetInCar( true );
        mpCharacter->UpdateTransformToInCar();        
    }

    mIsDriver = (mpCharacter->GetTargetVehicle()->mpDriver == mpCharacter) ||
                (!mpCharacter->GetTargetVehicle()->HasDriver());

    rmt::Vector seat = mIsDriver ? 
        mpCharacter->GetTargetVehicle()->GetDriverLocation() : 
        mpCharacter->GetTargetVehicle()->GetPassengerLocation(); 

    bool busy = mpCharacter->GetActionController()->IsBusy();

    if(!m_InCarAction->IsRunning())
    {
        m_InCarAction->SetStatus(SLEEPING);
    }

    // add the in car action (not very interesting) to the character sequencer
    Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();
    pSeq->BeginSequence();
    pSeq->AddAction( new Position(mpCharacter, seat, 0.0f, true));
    if(!busy)
    {
        pSeq->AddAction( m_InCarAction);
    }
    pSeq->EndSequence();

    mpCharacter->SetInCar( true );

    if(!mpCharacter->GetTargetVehicle()->mVisibleCharacters)
    {
        mpCharacter->RemoveFromWorldScene();
    }
    else
    {
        mpCharacter->SetScale(mpCharacter->GetTargetVehicle()->mCharacterScale);
    }

    mpCharacter->RemoveFromPhysics();
}

void InCar::Exit( void )
{
    if(mpCharacter == GetCharacterManager()->GetCharacter(0))
    {
        Vehicle::sDoBounce = false;
    }

    this->m_InCarAction->Done();
    this->m_InCarAction->Clear();

    mpCharacter->AddToPhysics();

    if( mpCharacter->GetTargetVehicle() != NULL )
    {
        if( (mpCharacter->GetTargetVehicle()->GetDriver() == mpCharacter) && (mpCharacter->GetRole() == Character::ROLE_PEDESTRIAN))
        {
            mpCharacter->GetTargetVehicle()->SetDriver(NULL);
        }
    }
}

void InCar::SequenceAction( void )
{
    Vehicle* pVehicle = mpCharacter->GetTargetVehicle( );

    if(!pVehicle)
    {
        return;
    }

    rAssert(pVehicle);

    if(!m_InCarAction->IsRunning())
    {
        m_InCarAction->SetStatus(SLEEPING);
    }

    // add the in car action (not very interesting) to the character sequencer
    Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();
    pSeq->BeginSequence();
    pSeq->AddAction( m_InCarAction);
    pSeq->EndSequence();


    rmt::Vector localPos = mpCharacter->GetPuppet()->GetPosition();
    rmt::Vector seatPos;
    if(mIsDriver)
    {
        seatPos = pVehicle->GetDriverLocation();
    }
    else
    {
        seatPos = pVehicle->GetPassengerLocation();
    }
    seatPos.y = localPos.y;
    mpCharacter->GetPuppet()->SetPosition(seatPos);

    if(mpCharacter == GetCharacterManager()->GetCharacter(0))
    {
        Vehicle::sDoBounce = true;
    }
}

void InCar::Update( float timeins )
{
    Vehicle* pVehicle = mpCharacter->GetTargetVehicle( );

    if((!pVehicle) ||
       ((pVehicle->mVehicleDestroyed) && 
       (GetGameFlow()->GetCurrentContext() != CONTEXT_DEMO) &&
       (GetGameFlow()->GetCurrentContext() != CONTEXT_SUPERSPRINT)))
    {
        mpCharacter->GetActionController()->Clear();
        mpCharacter->GetStateManager()->SetState<GetOut>();
        return;
    }

    CharacterController::eIntention theIntention = CharacterController::NONE; 
    bool actionDown = false;
    if(!GetHitnRunManager()->IsWaitingForReset())
    {
        theIntention = mpCharacter->GetController()->GetIntention();
#ifdef RAD_WIN32
        actionDown = mpCharacter->GetController()->IsButtonDown(CharacterController::GetOutCar);
#else
        actionDown = mpCharacter->GetController()->IsButtonDown(CharacterController::DoAction);
#endif
    }
    bool brake = false;

    if( GetGameFlow()->GetCurrentContext() != CONTEXT_SUPERSPRINT )
    {

        switch(m_GetOutState)
        {
            case GETOUT_NONE:
                {
#ifdef RAD_WIN32
                    if (theIntention == CharacterController::GetOutCar)
#else
                    if (theIntention == CharacterController::DoAction)
#endif
                    {    
                        m_GetOutState = (pVehicle->GetSpeedKmh() < 30.0f) ? GETOUT_COMITTED : GETOUT_TRYING;
                        mpCharacter->GetTargetVehicle()->mGeometryVehicle->ShowBrakeLights();
                    }
                }
                break;

            case GETOUT_TRYING :
                {
                    if(!actionDown)
                    {
                        m_GetOutState = GETOUT_NONE;
                        mpCharacter->GetTargetVehicle()->mGeometryVehicle->HideBrakeLights();

                        VehicleController* controller = GetVehicleCentral()->GetVehicleController(GetVehicleCentral()->GetVehicleId(pVehicle));
                        if(controller)
                        {
                            rAssert(dynamic_cast<HumanVehicleController*>(controller));
                            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::Brake)->SetValue(0.0f);
                            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::HandBrake)->SetValue(0.0f);
                        }
                        break;
                    }

                    if ( pVehicle->GetSpeedKmh() < 30.0f && 
                         !pVehicle->IsUnstable() &&
                         !pVehicle->IsAirborn() )
                    {
                        m_GetOutState = GETOUT_COMITTED;
                    }

                    brake = true;
                }
                break;

            case GETOUT_COMITTED :
                {
                    if ( pVehicle->GetSpeedKmh() < 3.0f )
                    {
                        mpCharacter->GetActionController()->Clear();

                        VehicleController* controller = GetVehicleCentral()->GetVehicleController(GetVehicleCentral()->GetVehicleId(pVehicle));

                        if(controller)
                        {
                            rAssert(dynamic_cast<HumanVehicleController*>(controller));
                            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::Gas)->SetValue(0.0f);
                            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::Brake)->SetValue(0.0f);
                            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::HandBrake)->SetValue(1.0f);
                        }

                        mpCharacter->GetTargetVehicle()->mGeometryVehicle->HideBrakeLights();

                        mpCharacter->GetStateManager()->SetState<CharacterAi::GetOut>();
                        return;
                    }
                    else
                    {

                        VehicleController* v = GetVehicleCentral()->GetVehicleController(GetVehicleCentral()->GetVehicleId(pVehicle));
                        if(v && (v->GetGas() > 0.0f))
                        {
                            m_GetOutState = GETOUT_NONE;
                            mpCharacter->GetTargetVehicle()->mGeometryVehicle->HideBrakeLights();
                            VehicleController* controller = GetVehicleCentral()->GetVehicleController(GetVehicleCentral()->GetVehicleId(pVehicle));
                            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::HandBrake)->SetValue(0.0f);
                        }
                        else
                        {
                            brake = true;
                        }
                    }
                }
                break;
            break;
        }
    }

    if(brake)
    {
        VehicleController* controller = GetVehicleCentral()->GetVehicleController(GetVehicleCentral()->GetVehicleId(pVehicle));
        if(controller)
        {
            rAssert(dynamic_cast<HumanVehicleController*>(controller));
            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::Gas)->SetValue(0.0f);
            static_cast<HumanVehicleController*>(controller)->GetMappable()->GetButton(VehicleMappable::HandBrake)->SetValue(1.0f);
        }
    }

    if(m_InCarAction->IsRunning())
    {
        /*
        if(mpCharacter->GetController()->IsButtonDown(CharacterController::Attack) && (m_GetOutState == GETOUT_NONE) )
        {
            m_InCarAction->IWannaRock(true);
        }
        */

        VehicleController* v = GetVehicleCentral()->GetVehicleController(GetVehicleCentral()->GetVehicleId(mpCharacter->GetTargetVehicle(), false));
        if(v)
        {
            bool duh;
            float steer = v->GetSteering(duh);

            Action* action = NULL;

            float speed = mpCharacter->GetTargetVehicle()->GetSpeedKmh();

            if(mIsDriver)
            {
                if((rmt::Abs(steer) > 0.5) && (speed > 5.0f)) 
                {
                    action =  new SteerAction(mpCharacter, (steer < 0) ? "turn_left_driver" : "turn_right_driver", 10.0f);
                }
                else if (mpCharacter->GetTargetVehicle()->IsInReverse())
                {
                    action = new ReverseAction(mpCharacter, "look_back_driver", 25.0f);
                }
                else if ((mpCharacter->GetTargetVehicle()->GetAccelMss() > 8.0f) && (speed < 20.0f))
                {
                    action = new AccelAction(mpCharacter, "accelerate_driver", 15.0f);
                }
                else if((mpCharacter->GetTargetVehicle()->GetAccelMss() < -10.0f) && (speed > 50.0f))
                {
                    action = new DecelAction(mpCharacter, "decelerate_driver", 15.0f);
                }
                else if(mpCharacter->GetTargetVehicle()->GetAccelMss() < -25.0f)
                {
                    if(mpCharacter->GetTargetVehicle()->mVelocityCM.DotProduct(mpCharacter->GetTargetVehicle()->mVehicleFacing) > 0.0f)
                    {
                        action = new PlayAnimationAction(mpCharacter, "crash_driver");
                    }
                }
            }
            else // passenger
            {
                if((rmt::Abs(steer) > 0.5) && (speed > 50.0f)) 
                {
                    action = new SteerAction(mpCharacter, (steer < 0) ? "sway_right" : "sway_left", 20.0f);
                }
            }

            if(action)
            {
                m_InCarAction->Done();
                m_InCarAction->Clear();
                mpCharacter->GetActionController()->SequenceSingleAction(action);
            }
        }
    }

    if( theIntention == CharacterController::CelebrateSmall )
    {
        m_InCarAction->Done();
        m_InCarAction->Clear();

        mpCharacter->GetActionController()->SequenceSingleAction( new PlayAnimationAction( mpCharacter, mIsDriver ? "in_car_victory_small_driver" : "in_car_victory_small"));
        return;
    }

    if( theIntention == CharacterController::CelebrateBig )
    {
        m_InCarAction->Done();
        m_InCarAction->Clear();

        mpCharacter->GetActionController()->SequenceSingleAction(new PlayAnimationAction( mpCharacter, mIsDriver ? "in_car_victory_large_driver" : "in_car_victory_large"));
        return;
    }

    if( theIntention == CharacterController::WaveHello)
    {
        m_InCarAction->Done();
        m_InCarAction->Clear();

        mpCharacter->GetActionController()->SequenceSingleAction( new PlayAnimationAction( mpCharacter, "wave_driver"));
        return;
    }

    if( theIntention == CharacterController::WaveGoodbye)
    {
        m_InCarAction->Done();
        m_InCarAction->Clear();

        mpCharacter->GetActionController()->SequenceSingleAction( new PlayAnimationAction( mpCharacter, "wave_goodbye_driver"));
        return;
    }
}

/*
==============================================================================
Loco::Loco
==============================================================================
Description:    Comment

Parameters( Character* pCharacter )

Returns:     
 
=============================================================================
*/
Loco::Loco( Character* pCharacter )
:
State( pCharacter ),
mLastActionFrame(0)
{
}

Loco::~Loco( void )
{
}

void Loco::Enter( void )
{
    if( mpCharacter->GetRole() != Character::ROLE_PEDESTRIAN )
    {
        mpCharacter->AddToWorldScene();
    }

    mpCharacter->SetScale(1.0f);

    mpCharacter->UpdateTransformToLoco();        

    mpCharacter->DoGroundIntersect(true);

    mpCharacter->SetInCar( false );

    mpCharacter->SetTargetVehicle( NULL );

    mpCharacter->GetWalkerLocomotionAction()->PlayDriver();

    if(mpCharacter == GetCharacterManager()->GetCharacter(0))
    {
        GetVehicleCentral()->ActivateVehicleTriggers(true);
    }

    if(!mpCharacter->GetActionController()->IsBusy())
    {
        SequenceAction();
    }
}

void Loco::Exit( void )
{
    mpCharacter->GetWalkerLocomotionAction()->StopDriver();
    mpCharacter->GetWalkerLocomotionAction()->Done();;
    mpCharacter->GetWalkerLocomotionAction()->AllowIdle(false);
    mpCharacter->SetSimpleLoco(false);
}

void Loco::SequenceAction( void )
{       
    mpCharacter->SetSimpleLoco(false);
    mpCharacter->GetWalkerLocomotionAction()->AllowIdle(false);

    CharacterController::eIntention theIntention = mpCharacter->GetController()->GetIntention();
    
    if((theIntention == CharacterController::Attack) && GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_KICK_TOGGLES_CHARACTER_MODEL))
    {
        mpCharacter->GetController()->SetIntention(CharacterController::NONE);
        GetCharacterManager()->NextCheatModel();
        return;
    }

    // if we are doing a kick, and in the action range of a power coupling, 
    // turn the kick into an aciton
    // Wow, is this ever a hack.
    if(( theIntention == CharacterController::Attack) && mpCharacter->GetActionButtonHandler( ))
    {
        if(dynamic_cast<ActionButton::DestroyObject*>(mpCharacter->GetActionButtonHandler( )))
        {
            theIntention = CharacterController::DoAction;
            mpCharacter->GetController()->SetIntention(theIntention);
        }
    }

    // give the actionbutton handler (if any) a crack at things
    if ( (mpCharacter->GetActionButtonHandler( ) != (ActionButton::ButtonHandler*)0) && ((mLastActionFrame + 3) <  GetGame()->GetFrameCount()) )
    {
        if ( mpCharacter->GetActionButtonHandler()->ButtonPressed( mpCharacter ) )
        {
            mLastActionFrame = GetGame()->GetFrameCount();
            return;
        }
    } 

    mpCharacter->GetController()->ClearIntention();

    // Sequence in the Dodge animation
    if( theIntention == CharacterController::Dodge )
    {
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        if( mpCharacter->IsNPC() ) // change NPC controller state when dodging is done
        {
            pSeq->BeginSequence();
            pSeq->AddAction(new ChangeNPCControllerState(mpCharacter, NPCController::DODGING));
            pSeq->EndSequence();
        }

        pSeq->BeginSequence();
        Action* pAction = new DodgeAction( mpCharacter );
        pSeq->AddAction( pAction );
        pSeq->EndSequence();

        if( mpCharacter->IsNPC() ) // change NPC controller state when dodging is done
        {
            pSeq->BeginSequence();
            pSeq->AddAction(new ChangeNPCControllerState(mpCharacter, NPCController::FOLLOWING_PATH));
            pSeq->EndSequence();
        }
        mpCharacter->GetWalkerLocomotionAction()->StopDriver();
        return;
    }

    // Sequence in the jump animation
    if ( theIntention == CharacterController::Jump && !GetInteriorManager()->IsInside())
    {
        if( !mpCharacter->IsNPC() && 
            mpCharacter->GetSimState()->GetControl() == sim::simSimulationCtrl )
        {
            // if player character is in simulation control and jump button was pressed,
            // we transit him back to AI control
            mpCharacter->GetSimState()->SetControl( sim::simAICtrl );
            mpCharacter->GetSimState()->ResetVelocities();
            mpCharacter->OnTransitToAICtrl();
        }
        else
        {
            Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

            pSeq->BeginSequence();
            JumpAction* pAction = mpCharacter->GetJumpLocomotionAction();
            pAction->Reset(CharacterTune::sfJumpHeight, false);
            pSeq->AddAction( pAction );

            pSeq->EndSequence();
            mpCharacter->GetWalkerLocomotionAction()->StopDriver();
            return;
        }
    }

    // Sequence in the Cringe animation
    if( theIntention == CharacterController::Cringe )
    {
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        if( mpCharacter->IsNPC() )
        {
            pSeq->BeginSequence();
            pSeq->AddAction(new ChangeNPCControllerState(mpCharacter, NPCController::CRINGING));
            pSeq->EndSequence();
        }

        pSeq->BeginSequence();
        Action* pAction = new CringeAction( mpCharacter );
        pSeq->AddAction( pAction );
        pSeq->EndSequence();

        if( mpCharacter->IsNPC() )
        {
            pSeq->BeginSequence();
            pSeq->AddAction(new ChangeNPCControllerState(mpCharacter, NPCController::FOLLOWING_PATH));
            pSeq->EndSequence();
        }
        mpCharacter->GetWalkerLocomotionAction()->StopDriver();
        return;
    }



    // Sequence in the TurnRight anim
    if( theIntention == CharacterController::TurnRight )
    {
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        pSeq->BeginSequence();
        Action* pAction = new PlayAnimationAction( mpCharacter, "turn60_CW" ); //, 50.0f );
        pSeq->AddAction( pAction );
        pSeq->EndSequence();

        mpCharacter->GetWalkerLocomotionAction()->StopDriver();
        return;
    }

    // Sequence in the TurnLeft anim
    if( theIntention == CharacterController::TurnLeft )
    {
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        pSeq->BeginSequence();
        Action* pAction = new PlayAnimationAction( mpCharacter, "turn60_CCW" );//, 50.0f );
        pSeq->AddAction( pAction );
        pSeq->EndSequence();

        mpCharacter->GetWalkerLocomotionAction()->StopDriver();
        return;
    }

    // Sequence in the attack anim
    if( theIntention == CharacterController::Attack )
    {
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        pSeq->BeginSequence();
        pSeq->AddAction( new PlayAnimationAction( mpCharacter, "break_quick" ));
        pSeq->AddAction( new KickAction( mpCharacter, 0.25f ));
        pSeq->EndSequence();
        mpCharacter->GetWalkerLocomotionAction()->StopDriver();
        return;
    }

    if( theIntention == CharacterController::CelebrateSmall )
    {
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        pSeq->BeginSequence();
        PlayAnimationAction* action = new PlayAnimationAction( mpCharacter, "victory_small" );
        action->AbortWhenMovementOccurs(true);
        pSeq->AddAction(action);
        pSeq->EndSequence();
        mpCharacter->GetWalkerLocomotionAction()->StopDriver();
        return;
    }

    if( theIntention == CharacterController::CelebrateBig )
    {
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        pSeq->BeginSequence();
        pSeq->AddAction( new PlayAnimationAction( mpCharacter, "victory_large" ));
        pSeq->EndSequence();
        mpCharacter->GetWalkerLocomotionAction()->StopDriver();
        return;
    }

    // Only do the test for turbo when we are not jumping.
    //
    if ( mpCharacter->GetController()->IsButtonDown( CharacterController::Dash ) && !GetInteriorManager()->IsInside())
    {
        mpCharacter->SetTurbo( true );
    }
    else
    {
        mpCharacter->SetTurbo( false );
    }

    if ( mpCharacter->IsInCollision() )
    {
    
        // Only play the animation if the character is running fast enough.
        //
        if ( false ) //mpCharacter->IsTurbo() && mpCharacter->GetDesiredSpeed() > 0.0f )
        {
            // Speed is good, now check the hit angle.
            //  [6/26/2002]
            //
            if ( mpCharacter->CanStaggerCollision( ) )
            {
                mpCharacter->GetActionController()->Clear();
                Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();
                Action* pAction = 0;
                // Play run_into_object.
                //
                pSeq->BeginSequence();
                pAction = new PlayAnimationAction( mpCharacter, "run_into_object" );
                pSeq->AddAction( pAction );
                pSeq->EndSequence( );
                mpCharacter->ResetSpeed( );
                mpCharacter->GetWalkerLocomotionAction()->StopDriver();
                return;
            }
        }
    }

    // nothing else got sequenced, do the default
    if(mpCharacter->GetWalkerLocomotionAction()->GetStatus() != RUNNING)
    {
        mpCharacter->GetWalkerLocomotionAction()->SetStatus(SLEEPING);
    }

    mpCharacter->GetWalkerLocomotionAction()->AllowIdle(true);

    mpCharacter->GetWalkerLocomotionAction()->PlayDriver();

    Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();
    pSeq->BeginSequence();
    pSeq->AddAction( mpCharacter->GetWalkerLocomotionAction());
    pSeq->EndSequence();

    mpCharacter->SetSimpleLoco(true);
}

void Loco::Update( float timeins )
{
}


/*
==============================================================================
GetIn
==============================================================================
*/
GetIn::GetIn( Character* pCharacter )
:
State( pCharacter ),
mObstructed(false),
mFirst(true),
mCollisionFailure(4)
{
}

GetIn::~GetIn( void )
{
}

void GetIn::Enter( void )
{

    if(mFirst)
    {
        GetEventManager()->TriggerEvent( EVENT_GETINTOVEHICLE_START, mpCharacter );
        mFirst = false;
    }
    
    if(!mpCharacter->GetTargetVehicle()->mIrisTransition && !mObstructed && (mpCharacter->GetTargetVehicle()->GetSpeedKmh() <= 1.0f)) 
    {
        if(mpCharacter->GetWalkerLocomotionAction()->GetStatus() != RUNNING)
        {
            mpCharacter->GetWalkerLocomotionAction()->SetStatus(SLEEPING);
        }

        mpCharacter->GetWalkerLocomotionAction()->PlayDriver();

        if(mpCharacter == GetCharacterManager()->GetCharacter(0))
        {
            GetVehicleCentral()->ActivateVehicleTriggers(false);
        }

        // figure out how to get into the car
        int nWaypoints;
        rmt::Vector waypoints[4];
        bool slide;
        bool jump;

        CalcGetInPath(&nWaypoints, waypoints, &slide, &jump);

        if(jump)
        {
            mObstructed = true;
            mpCharacter->GetActionController()->Clear();
            Enter();
            /*
            Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();
            pSeq->BeginSequence();
            pSeq->AddAction( new FragileArrive( mpCharacter, waypoints[0]) );
            mpCharacter->GetJumpLocomotionAction()->Reset(1.9f);
            pSeq->AddAction(mpCharacter->GetJumpLocomotionAction());
            pSeq->EndSequence( );
            */
            return;
        }

        rmt::Vector getinPosition = waypoints[nWaypoints - 1];

        if( ( nWaypoints >= 3 ) && ( mpCharacter->GetTargetVehicle()->mpDriver != NULL ) )
        {
            GetEventManager()->TriggerEvent( EVENT_WRONG_SIDE_DUMBASS, 
                                                static_cast<Vehicle*>(mpCharacter->GetTargetVehicle()) );
        }

        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();
        bool isDriver = mpCharacter->GetTargetVehicle()->HasDriver() ? false : true;
        Vehicle::Door door = (isDriver && !slide) ? Vehicle::DOOR_DRIVER : Vehicle::DOOR_PASSENGER;
        Action* pAction = 0;

        mpCharacter->GetTargetVehicle()->UpdateDoorState();
        
        bool high = false;
        if(isDriver)
        {
            high = mpCharacter->GetTargetVehicle()->GetDriverLocation().y > CharacterTune::sGetInHeightThreshold;
        }
        else
        {
            high = mpCharacter->GetTargetVehicle()->GetPassengerLocation().y > CharacterTune::sGetInHeightThreshold;
        }

        const char** anims = NULL;
        if(high)
        {
            if(isDriver && !slide)
            {
                anims = sGetInHighDriver;
            }
            else
            {
                anims = sGetInHighPassenger;
            }
        }
        else
        {
            if(isDriver && !slide)
            {
                anims = sGetInDriver;
            }
            else
            {
                anims = sGetInPassenger;
            }
        }

        // add actions for all the get in waypoints
        for(int i = 0; i < nWaypoints; i++)
        {
            pSeq->BeginSequence();

            pAction = new Arrive( mpCharacter, waypoints[i], i == nWaypoints - 1 );
            pSeq->AddAction( pAction );
            // SlaveLoco
            //
            pAction = 0;

            pAction = mpCharacter->GetWalkerLocomotionAction();

            pSeq->AddAction( mpCharacter->GetWalkerLocomotionAction() );
            pSeq->EndSequence( );
        }
        
        // Orient
        //
        pSeq->BeginSequence();
        rmt::Matrix mat = mpCharacter->GetTargetVehicle( )->GetTransform();
        pAction = new Orient( mpCharacter, mat.Row( 2 ) );
        pSeq->AddAction( pAction );
        // SlaveLoco
        //
        pAction = 0;
        
        pAction = mpCharacter->GetWalkerLocomotionAction();
        
        pSeq->AddAction( pAction );
        pSeq->EndSequence( );

        // Open Door Anim
        //
        if(mpCharacter->GetTargetVehicle()->NeedToOpenDoor(door))
        {
            float delay = CharacterTune::sGetInOpenDelay;
            float time  = CharacterTune::sGetInOpenSpeed;
            float scale = 30.0f / CharacterTune::sfGetInOutOfCarAnimSpeed;
        
            pSeq->BeginSequence();
            pAction = new PlayAnimationAction( mpCharacter, anims[ 0 ], CharacterTune::sfGetInOutOfCarAnimSpeed);
            pSeq->AddAction( pAction );
            pSeq->AddAction( new CarDoorAction(mpCharacter->GetTargetVehicle(), Vehicle::DOORACTION_OPEN, door, delay * scale, time * scale));
            pSeq->EndSequence( );
        }

        // Get In Car Anim
        //
        pSeq->BeginSequence();
        pAction = new PlayAnimationAction( mpCharacter, anims[ 1 ], CharacterTune::sfGetInOutOfCarAnimSpeed);
        pSeq->AddAction( pAction );
        pSeq->EndSequence( );

        rmt::Vector seat;
        if(isDriver && !slide)
        {
            seat = mpCharacter->GetTargetVehicle()->GetDriverLocation();
        }
        else
        {
            seat = mpCharacter->GetTargetVehicle()->GetPassengerLocation();
        }

        pSeq->BeginSequence();
        pSeq->AddAction( new ChangeLocomotion(mpCharacter, ChangeLocomotion::INCAR) );
        pSeq->AddAction( new ChangeState<InCar>(mpCharacter));
        pSeq->AddAction( new TriggerEventAction(EVENT_GETINTOVEHICLE_END, mpCharacter));
        pSeq->EndSequence( );

        pSeq->BeginSequence();
        pSeq->AddAction( new DelayAction(0.05f));
        pSeq->EndSequence( );

        pSeq->BeginSequence();
        pSeq->AddAction( new Position(mpCharacter, seat, 0.0f, true));
        pSeq->AddAction( new PlayAnimationAction( mpCharacter, "in_car_idle", 300.0f));
        pSeq->EndSequence( );

        // Close Door Anim
        //
        if(mpCharacter->GetTargetVehicle()->HasActiveDoor(door) || mpCharacter->GetTargetVehicle()->NeedToCloseDoor(door))
        {
            float delay = CharacterTune::sGetInCloseDelay;
            float time  = CharacterTune::sGetInCloseSpeed;
            float scale = 30.0f / CharacterTune::sfGetInOutOfCarAnimSpeed;
        
            pSeq->BeginSequence();
            pAction = new PlayAnimationAction( mpCharacter, anims[ 2 ], CharacterTune::sfGetInOutOfCarAnimSpeed);
            pSeq->AddAction( new CarDoorAction(mpCharacter->GetTargetVehicle(), Vehicle::DOORACTION_CLOSE, door, delay * scale, time * scale));
            pSeq->AddAction( pAction );
            pSeq->EndSequence( );
        }

        if(slide)
        {
            rmt::Vector seatDist = mpCharacter->GetTargetVehicle()->GetPassengerLocation() -
                                   mpCharacter->GetTargetVehicle()->GetDriverLocation();
            bool seatClose = seatDist.Magnitude() < 0.1f;
            
            rmt::Vector v = mpCharacter->GetTargetVehicle()->GetPassengerLocation();
            pSeq->BeginSequence();
            pSeq->AddAction( new Position(mpCharacter, v, 0.0f, true));
            pSeq->EndSequence( );

            v = mpCharacter->GetTargetVehicle()->GetDriverLocation();
            pSeq->BeginSequence();
            pSeq->AddAction( new Position(mpCharacter, v, 0.5f, true));
            if(!seatClose)
            {
                pSeq->AddAction( new PlayAnimationAction(mpCharacter, "seatmove"));
            }
            pSeq->EndSequence( );

            if(mpCharacter->GetTargetVehicle()->NeedToCloseDoor(Vehicle::DOOR_DRIVER))
            {
                float delay = CharacterTune::sGetInCloseDelay;
                float time  = CharacterTune::sGetInCloseSpeed;
                float scale = 30.0f / CharacterTune::sfGetInOutOfCarAnimSpeed;
            
                pSeq->BeginSequence();
                pAction = new PlayAnimationAction( mpCharacter, "get_into_car_close_door_driver", CharacterTune::sfGetInOutOfCarAnimSpeed);
                pSeq->AddAction( new CarDoorAction(mpCharacter->GetTargetVehicle(), Vehicle::DOORACTION_CLOSE, Vehicle::DOOR_DRIVER, delay * scale, time * scale));
                pSeq->AddAction( pAction );
                pSeq->EndSequence( );
            }
        }

        pSeq->BeginSequence();
        pSeq->AddAction( new ReleaseDoorsAction(mpCharacter->GetTargetVehicle()));
        pSeq->EndSequence( );
    }
    else
    {
        //Do an iris wipe in
        GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_CLOSE );
        //Set the speed of the wipe
        CGuiScreenIrisWipe* iw = static_cast<CGuiScreenIrisWipe*>(GetGuiSystem()->GetInGameManager()->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE ));
        const float speed = 1.0f;
        iw->SetRelativeSpeed( speed );

        //add listener  
        GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );

        //Pause gameplay
        GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
    }
}

void GetIn::HandleEvent( EventEnum id, void* pUserData )
{
    if ( id == EVENT_GUI_IRIS_WIPE_CLOSED )
    {
        mpCharacter->RemoveFromPhysics();

        GetEventManager()->RemoveListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );

        Vehicle* pVehicle = mpCharacter->GetTargetVehicle();
        rAssert( pVehicle ); 

        rmt::Vector pos;
        pVehicle->GetPosition( &pos );

        GetAvatarManager()->PutCharacterInCar( mpCharacter, pVehicle );

        if(!pVehicle->mVisibleCharacters)
        {
            mpCharacter->RemoveFromWorldScene();
        }

        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::FOLLOW_CAM, SuperCamCentral::CUT, 0 ); //Override the auto one

        //TODO: SWITCH MODELS

        //Wipe out when we are finished moving.
        GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_OPEN );

        GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Resume();

        GetEventManager()->TriggerEvent( EVENT_GETINTOVEHICLE_END, mpCharacter );

        pVehicle->MoveDoor(Vehicle::DOOR_DRIVER, Vehicle::DOORACTION_CLOSE, 0.0f);
        pVehicle->MoveDoor(Vehicle::DOOR_PASSENGER, Vehicle::DOORACTION_CLOSE, 0.0f);
    }
}

// figure out how to get into the car from an arbitrary location
void GetIn::CalcGetInPath(int* nWaypoints, rmt::Vector* waypoints, bool* doingSlide, bool* jump)
{
    const float origedge = 0.75f; // how far want to be from the car at the nav points ( TODO : make this tunable?)
    const float edge = origedge / 1.414f; // x & z need to be sacled by sqrt(2) to make the diagonal distance right ;

    rmt::Vector box = mpCharacter->GetTargetVehicle()->GetExtents();

    bool isDriver = mpCharacter->GetTargetVehicle()->HasDriver() ? false : true;

    // grab matrices to get from car space to world space (we do all our work in car space)
    rmt::Matrix carToWorld = mpCharacter->GetTargetVehicle( )->GetTransform();
    rmt::Matrix worldToCar = mpCharacter->GetTargetVehicle( )->GetTransform();
    worldToCar.Invert();

    // get character position in car space
    rmt::Vector characterPosition;
    mpCharacter->GetPosition(characterPosition);
    characterPosition.Transform(worldToCar);

    *doingSlide = false;
    *jump = false;
    *nWaypoints = 0;

    if((characterPosition.y > (-box.y / 2)) &&
       (characterPosition.x < box.x) &&
       (characterPosition.x > -box.x) &&
       (characterPosition.z < box.z) &&
       (characterPosition.z > -box.z))
    {
        // get in position (car space)
        rmt::Vector getinPosition = CharacterTune::sGetInPosition;
        getinPosition = -getinPosition;
        getinPosition.Add(mpCharacter->GetTargetVehicle( )->GetDriverLocation());
    
        // swap get in poistion for driver
        characterPosition.x = getinPosition.x - 0.5f;

        // add final get in position to waypoints
        characterPosition.Transform( carToWorld );
        waypoints[(*nWaypoints)++] = characterPosition;
        *jump = true;
        return;
    }

    if(isDriver && mpCharacter->GetTargetVehicle()->mAllowSlide)
    {
        rmt::Vector driverPosition = CharacterTune::sGetInPosition;
        driverPosition.x = -driverPosition .x;
        driverPosition.Add(mpCharacter->GetTargetVehicle( )->GetDriverLocation());

        rmt::Vector passPosition = CharacterTune::sGetInPosition;
        passPosition.Add(mpCharacter->GetTargetVehicle( )->GetPassengerLocation());

        driverPosition.Sub(characterPosition);
        passPosition.Sub(characterPosition);

        if(characterPosition.x > (box.x - 0.5f))
        {
            isDriver = false;
            *doingSlide = true;
        }
    }

    // these are the edges of the car from a walking perspective 
    float carFront    =  box.z + (edge);
    float carRear     = -box.z - (edge);;
    float carNearSide = (box.x + edge) * (isDriver ? -1 : 1);
    float carFarSide  = (box.x + edge) * (isDriver ? 1 : -1);

    // get in position (car space)
    rmt::Vector getinPosition = CharacterTune::sGetInPosition;
    
    // swap get in poistion for driver
    if ( isDriver )
    {
        // mirror over x.
        getinPosition.x = -getinPosition.x;
        getinPosition.Add(mpCharacter->GetTargetVehicle( )->GetDriverLocation());
    }
    else
    {
        getinPosition.Add(mpCharacter->GetTargetVehicle( )->GetPassengerLocation());
    }

    // are we in fron of the car or behind
    bool front = characterPosition.z > 0;

    bool onside = true;

    if(((characterPosition.z > box.z) || (characterPosition.z < -box.z)) && 
        (isDriver ? (characterPosition.x > -(box.x - (edge / 2))) : (characterPosition.x < (box.x - (edge / 2))) ))
    {
        onside = false;
    }

    if(((characterPosition.z < box.z) && (characterPosition.z > -box.z)) && 
        (isDriver ? (characterPosition.x > 0) : (characterPosition.x < 0)))
    {
        onside = false;
    }

    if(!onside)
    {
        // we are offside, need some more nav points

        // are we on the other side of the car
        if((characterPosition.z < box.z) && (characterPosition.z > -box.z))
        {
            // add far corner to waypoints
            rmt::Vector tmpPosition(0,0,0);
            tmpPosition.z += front ? carFront : carRear;
            tmpPosition.x += carFarSide;

            tmpPosition.Transform( carToWorld );
            waypoints[(*nWaypoints)++] = tmpPosition;
        }

        // add near corner to waypoints
        rmt::Vector tmpPosition(0,0,0);
        tmpPosition.z += front ? carFront : carRear;
        tmpPosition.x += carNearSide;

        tmpPosition.Transform( carToWorld );
        waypoints[(*nWaypoints)++] = tmpPosition;
    }

    // add final get in position to waypoints
    getinPosition.Transform( carToWorld );
    waypoints[(*nWaypoints)++] = getinPosition;
}

void GetIn::SequenceAction( void )
{       
    Enter();
}

void GetIn::Update( float timeins )
{
    if(mpCharacter->CollidedThisFrame() && (mCollisionFailure == 0) && !mpCharacter->GetJumpLocomotionAction()->IsInJump())
    {
        mObstructed = true;
        mpCharacter->GetActionController()->Clear();
        Enter();
    }

    if(mCollisionFailure)
    {
        mCollisionFailure--;
    }
}

void GetIn::Exit( void )
{
    if(!mpCharacter->GetTargetVehicle()->mVisibleCharacters)
    {
        mpCharacter->RemoveFromWorldScene();
    }

    mpCharacter->GetWalkerLocomotionAction()->StopDriver();
    mpCharacter->GetWalkerLocomotionAction()->Done();;
}


/*
==============================================================================
GetOut
==============================================================================
*/
GetOut::GetOut( Character* pCharacter )
:
State( pCharacter ),
mObstructed(false),
mFirst(true),
mPanic(false)
{
}

GetOut::~GetOut( void )
{
}

void GetOut::Enter( void )
{
    bool destroyed = !mpCharacter->GetTargetVehicle() || mpCharacter->GetTargetVehicle()->mVehicleDestroyed;
    bool iris = !destroyed && (mObstructed || mpCharacter->GetTargetVehicle()->mIrisTransition);

    if(mFirst)
    {
        mFirst = false;
        GetEventManager()->TriggerEvent( EVENT_GETOUTOFVEHICLE_START, mpCharacter );
    }

    if ( !iris || mpCharacter != GetCharacterManager()->GetCharacter(0))
    {
        mpCharacter->AddToWorldScene();
        mpCharacter->SetScale(1.0f);
        DoGetOut();
    }
    else
    {
        //Do an iris wipe in
        GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_CLOSE );
        //Set the speed of the wipe
        CGuiScreenIrisWipe* iw = static_cast<CGuiScreenIrisWipe*>(GetGuiSystem()->GetInGameManager()->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE ));
        const float speed = 1.0f;
        iw->SetRelativeSpeed( speed );

        //add listener  
        GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );

        //Pause gameplay
        GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();

        if(mpCharacter->GetTargetVehicle())
        {
            mpCharacter->GetTargetVehicle()->SetOutOfCarSimState();
        }
    }
}

static sim::TArray< sim::RayIntersectionInfo > sIntersectInfo( 64 ); 

void GetOut::HandleEvent( EventEnum id, void* pUserData )
{
    if ( id == EVENT_GUI_IRIS_WIPE_CLOSED )
    {
        GetEventManager()->RemoveListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );

        Character* character = GetAvatarManager()->GetAvatarForPlayer( 0 )->GetCharacter();
        Vehicle* car = GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle();

        GetEventManager()->TriggerEvent( EVENT_GETOUTOFVEHICLE_END, character );

        car->MoveDoor(Vehicle::DOOR_DRIVER, Vehicle::DOORACTION_CLOSE, 0.0f);
        car->MoveDoor(Vehicle::DOOR_PASSENGER, Vehicle::DOORACTION_CLOSE, 0.0f);

        character->SetInCar( false );
        character->SetTargetVehicle( NULL );
        character->AddToWorldScene();
        mpCharacter->SetScale(1.0f);

        // get in position (car space)
        rmt::Vector getOutPosition = CharacterTune::sGetInPosition;
    
        int isDriver = car->HasDriver() ? 0 : 1;
        if(!mObstructed)
        {
            // swap get in poistion for driver
            if ( isDriver )
            {
                // mirror over x.
                getOutPosition.x = -getOutPosition.x;
                getOutPosition.Add( car->GetDriverLocation() );
            }
            else
            {
                getOutPosition.Add( car->GetPassengerLocation() );
            }

            getOutPosition.Transform( car->GetTransform() );
            rmt::Vector carPos;

            car->GetPosition(&carPos);
            getOutPosition.y = carPos.y - car->GetExtents().y;

            // check if point we want to teleport to is objstructed
            sIntersectInfo.Clear();
            float fOldRayThickness = sim::RayIntersectionInfo::sRayThickness;
            static float sfCharacterRayThickness = 0.3f;
            sim::RayIntersectionInfo::sRayThickness = sfCharacterRayThickness;
            bool bFoundIntersect = false;

            float outDist = VERY_LARGE;
            float currentDist = outDist;

            HeapMgr()->PushHeap( GMA_TEMP );

            // adds in the list the collision object interfering with the ray and ordered according to their distance to the source.
            // use sim::RayIntersectionInfo::SetMethod(method) to set the method
            // use sim::RayIntersectionInfo::SetReturnClosestOnly(true/false) if you need only the closest object
            // nb. if SetReturnClosestOnly(true) is used, the previous returned list can be used as a cache.
            sim::RayIntersectionInfo::SetReturnClosestOnly( false );
            sim::RayIntersectionInfo::SetMethod( sim::RayIntersectionVolume );
            rmt::Vector rayOrg;
            mpCharacter->GetPosition(rayOrg);
            rmt::Vector rayEnd = getOutPosition;

            HeapMgr()->PopHeap( GMA_TEMP );

            // Test ray against remaining collision objects.
            //
            GetWorldPhysicsManager()->mCollisionManager->DetectRayIntersection(sIntersectInfo, rayOrg, rayEnd, false, car->mCollisionAreaIndex  );

            for ( int i = 0; i < sIntersectInfo.GetSize( ); i++ )
            {
                if ( sIntersectInfo[ i ].mCollisionVolume && 
                     (sIntersectInfo[ i ].mCollisionVolume->GetCollisionObject()->GetSimState()->mAIRefPointer != mpCharacter ) &&
                     (sIntersectInfo[ i ].mCollisionVolume->GetCollisionObject()->GetSimState()->mAIRefPointer != car ))
                {
                    mObstructed = true;
                    break;
                }
            }
            sim::RayIntersectionInfo::sRayThickness = fOldRayThickness;
        }

        if(mObstructed)
        {
            car->GetPosition(&getOutPosition);
            getOutPosition.y += car->GetExtents().y + 0.2f;
        }

        rmt::Vector facing = car->GetTransform().Row(0);
        if(isDriver)
        {
            facing.Scale(-1.0f);
        }

        GetAvatarManager()->PutCharacterOnGround( character, car );
        character->RelocateAndReset( getOutPosition, choreo::GetWorldAngle(facing.x, facing.z), true, true );
        character->SnapToGround();

        rmt::Vector newPos;
        character->GetPosition(&newPos);

        float dist = mObstructed ? 3.0f : 1.0f;
        if(rmt::Abs(getOutPosition.y - newPos.y) > dist)
        {
            car->GetPosition(&getOutPosition);
            getOutPosition.y += car->GetExtents().y + 0.2f;
            character->RelocateAndReset( getOutPosition, choreo::GetWorldAngle(facing.x, facing.z), true, true );
        }

        GetVehicleCentral()->ActivateVehicleTriggers(true);

#ifdef RAD_WIN32
        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::ON_FOOT_CAM, SuperCamCentral::CUT, 0 );
#else
        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::WALKER_CAM, SuperCamCentral::CUT, 0 );
#endif
        //Wipe out when we are finished moving.
        GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_OPEN );

        GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Resume();
    }
}

void GetOut::Exit( void )
{
}

void GetOut::SequenceAction( void )
{
    Enter();
}

void GetOut::Update( float timeins )
{
    if(mpCharacter->CollidedThisFrame() && !mPanic)
    {
        mObstructed = true;
        mpCharacter->GetActionController()->Clear();
        Enter();
    }
}

void GetOut::DoGetOut( void )
{
    mPanic = !mpCharacter->GetTargetVehicle() || mpCharacter->GetTargetVehicle()->mVehicleDestroyed;
    bool mIsDriver;

    if(mpCharacter->GetTargetVehicle())
    {
        mIsDriver = (mpCharacter->GetTargetVehicle()->mpDriver == mpCharacter) ||
                    ((mpCharacter->GetTargetVehicle()->mVehicleType != VT_TRAFFIC) && 
                    (mpCharacter->GetTargetVehicle()->mpDriver == NULL));
    }
    else
    {
        mIsDriver = false;
    }

    Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

    if(mPanic)
    {
        mpCharacter->AddToWorldScene();

        if(mpCharacter->GetRole() == Character::ROLE_DRIVER)
        {
            GetCharacterManager()->SetGarbage(mpCharacter, true);
        }

        mpCharacter->SetFacingDir( mIsDriver ? (rmt::PI / 2) : -(rmt::PI / 2));
        mpCharacter->SetDesiredDir( mIsDriver ? (rmt::PI / 2) : -(rmt::PI / 2));

        pSeq->BeginSequence();
        pSeq->AddAction( new DelayAction(0.5f));
        pSeq->EndSequence( );

        pSeq->BeginSequence();
        pSeq->AddAction( new GroundSnap(mpCharacter));
        pSeq->EndSequence( );

        pSeq->BeginSequence();
        pSeq->AddAction( new ChangeLocomotion(mpCharacter, ChangeLocomotion::WALKING) );
        pSeq->EndSequence( );

        pSeq->BeginSequence();
        pSeq->AddAction( new DodgeAction(mpCharacter) );
        pSeq->EndSequence( );

        pSeq->BeginSequence();
        pSeq->AddAction( new ChangeState<Loco>(mpCharacter));
        pSeq->AddAction( new TriggerEventAction(EVENT_GETOUTOFVEHICLE_END, mpCharacter));
        pSeq->EndSequence( );
    }
    else
    {
        int isDriver = mpCharacter->GetTargetVehicle()->HasDriver() ? 0 : 1;
        Vehicle::Door door = isDriver ? Vehicle::DOOR_DRIVER : Vehicle::DOOR_PASSENGER;
        Action* pAction = 0;

        mpCharacter->GetTargetVehicle()->UpdateDoorState();

        bool high = true;
        rmt::Vector seat;

        if(isDriver)
        {
            high = mpCharacter->GetTargetVehicle()->GetDriverLocation().y > CharacterTune::sGetInHeightThreshold;
            seat = mpCharacter->GetTargetVehicle()->GetDriverLocation();
        }
        else
        {
            high = mpCharacter->GetTargetVehicle()->GetPassengerLocation().y > CharacterTune::sGetInHeightThreshold;
            seat = mpCharacter->GetTargetVehicle()->GetPassengerLocation();
        }

        const char** anims = NULL;
        if(high)
        {
            if(isDriver)
            {
                anims = sGetOutHighDriver;
            }
            else
            {
                anims = sGetOutHighPassenger;
            }
        }
        else
        {
            if(isDriver)
            {
                anims = sGetOutDriver;
            }
            else
            {
                anims = sGetOutPassenger;
            }
        }

        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        pSeq->BeginSequence();
        pSeq->AddAction( new DelayAction(0.1f));
        pSeq->EndSequence( );

        pSeq->BeginSequence();
        pSeq->AddAction( new Position(mpCharacter, seat, 0.0f, true));
        pSeq->EndSequence( );

        // Play get_out_of_car_open_door
        if(mpCharacter->GetTargetVehicle()->NeedToOpenDoor(door))
        {
            float delay = CharacterTune::sGetOutOpenDelay;
            float time  = CharacterTune::sGetOutOpenSpeed;
            float scale = 30.0f / CharacterTune::sfGetInOutOfCarAnimSpeed;
    
            pSeq->BeginSequence();
            pAction = new PlayAnimationAction( mpCharacter, anims[ 0 ], CharacterTune::sfGetInOutOfCarAnimSpeed );
            pSeq->AddAction( new CarDoorAction(mpCharacter->GetTargetVehicle(), Vehicle::DOORACTION_OPEN, door, delay * scale, time * scale));
            pSeq->AddAction( pAction );
            pSeq->EndSequence( );
        }

        if(0)
        {
            pSeq->BeginSequence();
            pSeq->AddAction( new DelayAction(2.0f));
            pSeq->EndSequence( );
        }

        // Play get_out_of_car
        //
        pSeq->BeginSequence();
        pSeq->AddAction( new ChangeLocomotion(mpCharacter, ChangeLocomotion::WALKING) );
        pSeq->EndSequence( );

        if(0)
        {
            pSeq->BeginSequence();
            pSeq->AddAction( new DelayAction(0.5f));
            pSeq->EndSequence( );
        }

        pSeq->BeginSequence();
        pSeq->AddAction( new GroundSnap(mpCharacter));
        pAction = new PlayAnimationAction( mpCharacter, anims[ 1 ], CharacterTune::sfGetInOutOfCarAnimSpeed);
        pSeq->AddAction( pAction );
        pSeq->EndSequence( );

        if(0)
        {
            pSeq->BeginSequence();
            pSeq->AddAction( new DelayAction(2.0f));
            pSeq->EndSequence( );
        }

        pSeq->BeginSequence();
        pSeq->AddAction( new ChangeState<Loco>(mpCharacter));
        pSeq->AddAction( new TriggerEventAction(EVENT_GETOUTOFVEHICLE_END, mpCharacter));

        // Play get_out_of_car_close_door
        if(mpCharacter->GetTargetVehicle()->HasActiveDoor(door))
        {
            float delay = CharacterTune::sGetOutCloseDelay;
            float time  = CharacterTune::sGetOutCloseSpeed;
            float scale = 30.0f / CharacterTune::sfGetInOutOfCarAnimSpeed;
    
            PlayAnimationAction* pAction = new PlayAnimationAction( mpCharacter, anims [ 2 ], CharacterTune::sfGetInOutOfCarAnimSpeed);
            pAction->AbortWhenMovementOccurs(true);
            pSeq->AddAction( new CarDoorAction(mpCharacter->GetTargetVehicle(), Vehicle::DOORACTION_CLOSE,  door, delay * scale, time * scale, mpCharacter, pSeq));
            pSeq->AddAction( pAction );
        }
        pSeq->EndSequence( );
    }

}


/*
==============================================================================
InSim::InSim
==============================================================================
Description:    Comment

Parameters( Character* pCharacter )

Returns:     
 
=============================================================================
*/
InSim::InSim( Character* pCharacter )
:
State( pCharacter )
{
}

InSim::~InSim( void )
{
}

void InSim::Enter( void )
{
    // clear previous actions
    mpCharacter->GetActionController()->Clear();

    // Sequence in the flail and get-up actions
    Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

    pSeq->BeginSequence();
    pSeq->AddAction( new FlailAction( mpCharacter ) );
    pSeq->EndSequence();

    pSeq->BeginSequence();
    pSeq->AddAction( new GetUpAction( mpCharacter ) );
    pSeq->EndSequence();

    /*
    pSeq->BeginSequence();
    pSeq->AddAction( new ChangeState<Loco>( mpCharacter ) );
    pSeq->EndSequence();
    */

    return;
}

void InSim::Exit( void )
{
}

void InSim::SequenceAction( void )
{     
    // SequenceAction gets called when no other actions are left to do.
    // In this state, it means that we're just done "getting up"...
    // so time to transit back to Loco state...
    mpCharacter->GetStateManager()->SetState<Loco>();
}

void InSim::Update( float timeins )
{
}



};
