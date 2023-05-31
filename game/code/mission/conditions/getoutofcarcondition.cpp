//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        getoutofcarcondition.cpp
//
// Description: Implement GetOutOfCarCondition
//
// History:     4/7/2003 + Created -- Cary Brisebois
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
#include <mission/conditions/getoutofcarcondition.h>

#include <mission/gameplaymanager.h>
#include <mission/mission.h>
#include <mission/objectives/raceobjective.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guimanageringame.h>

#include <events/eventmanager.h>

#include <worldsim/character/character.h>
#include <worldsim/avatarmanager.h>

#include <interiors/interiormanager.h>

#include <worldsim/hitnrunmanager.h>


//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// GetOutOfCarCondition::GetOutOfCarCondition
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
GetOutOfCarCondition::GetOutOfCarCondition() :
      
mTimeAmount( 11000 )  
{
    SetType( COND_PLAYER_OUT_OF_VEHICLE );
    mTimeRemainingmilliseconds = 10000; //set it to 10 milliseconds by default
    mbIsConditionActive = false;
}

//=============================================================================
// GetOutOfCarCondition::~GetOutOfCarCondition
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
GetOutOfCarCondition::~GetOutOfCarCondition()
{
}

//=============================================================================
// GetOutOfCarCondition::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void GetOutOfCarCondition::Update( unsigned int elapsedTime )
{
    if ( mbIsConditionActive == true )
    {
        mTimeRemainingmilliseconds -= static_cast <int>(elapsedTime);

        if ( ( mTimeRemainingmilliseconds <= 0 || GetInteriorManager()->IsInside() )  //If you go inside you fail.
            &&
            !GetInteriorManager()->IsEntering() && 
            !GetInteriorManager()->IsExiting() &&
            !GetHitnRunManager()->BustingPlayer() )
        {
            mTimeRemainingmilliseconds = 0;
            //HE has failed!
            SetIsViolated( true );
        }
    }
}

//=============================================================================
// GetOutOfCarCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void GetOutOfCarCondition::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == EVENT_GETOUTOFVEHICLE_END )
    {
        //The character has left the car!
        //Start the count down.
        Character* whichCharacter = static_cast< Character* >( pEventData );

        //check to see if the Charactar bailing out of the car is a PC or NPC
        //if its a NPC then this condition should not apply to them
        if ( whichCharacter != GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter())
        {
            return;
        }



        Vehicle* whichVehicle = whichCharacter->GetTargetVehicle();
        rAssert( whichVehicle );

        if ( mbIsConditionActive == false ) //This is the first time and not me cheating by jumping in a traffic car
        {
            //We're gonna look at the car since this is the car we care about.
            mMyVehicle = whichVehicle;
            mbIsConditionActive = true;

            GetEventManager()->TriggerEvent(EVENT_OUTOFCAR_CONDITION_ACTIVE,this);
            CGuiScreenHud* currentHud = GetCurrentHud();
            if( currentHud != NULL )
            {
                currentHud->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_TIMER_TEMP );
                currentHud->SetTimerBlinkingInterval(10000,0);
            }
        } 
        else if ( whichVehicle != mMyVehicle )
        {
            //Ignore.
            return;
        }

        
        //Set the TimeRemainmilliseconds 
        mTimeRemainingmilliseconds = mTimeAmount;       

        CGuiScreenHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            currentHud->DisplayMessage( true, 299 ); //299 is the magic number in the textbible for the gET IN CAR message.
        }

    }
    else if ( id == EVENT_GETINTOVEHICLE_START )
    {
        //Now he's back in!

        Character* whichCharacter = static_cast< Character* >( pEventData );

        Vehicle* whichVehicle = whichCharacter->GetTargetVehicle();
        rAssert( whichVehicle );

        //make sure we get back into the same vehicle
        if ( whichVehicle == mMyVehicle )
        {
            mbIsConditionActive = false;
            GetEventManager()->TriggerEvent(EVENT_OUTOFCAR_CONDITION_INACTIVE);
            mTimeRemainingmilliseconds = mTimeAmount;
            if (GetGameplayManager()->GetCurrentMission()->IsWagerMission() == true)
            {
               RaceObjective* pRaceObjective= NULL;

               pRaceObjective = dynamic_cast <RaceObjective*>( GetGameplayManager()->GetCurrentMission()->GetCurrentStage()->GetObjective());
               rAssert(pRaceObjective != NULL);

               int partime = pRaceObjective->GetParTime();

               GetCurrentHud()->SetTimerBlinkingInterval( (partime-10)*1000,partime*1000);
            }       


        }
        else
        {
            SetIsViolated( true );
        }
    }

    MissionCondition::HandleEvent( id, pEventData );
}


//return the time remaining in milliseconds the player has to get back into there old vehicle

int GetOutOfCarCondition::GetTimeRemainingTilFailuremilliseconds()
{
    return mTimeRemainingmilliseconds;
}



//=============================================================================
// GetOutOfCarCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool GetOutOfCarCondition::IsClose()
{
    return( mTimeRemainingmilliseconds > 1000 && mTimeRemainingmilliseconds < 3000 );
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// GetOutOfCarCondition::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GetOutOfCarCondition::OnInitialize()
{
    GetEventManager()->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_START );
}

//=============================================================================
// GetOutOfCarCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GetOutOfCarCondition::OnFinalize()
{
    GetEventManager()->RemoveListener( this, EVENT_GETOUTOFVEHICLE_END );
    GetEventManager()->RemoveListener( this, EVENT_GETINTOVEHICLE_START );

    GetEventManager()->TriggerEvent(EVENT_OUTOFCAR_CONDITION_INACTIVE);
    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        currentHud->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_TIMER_TEMP );
    }

    mbIsConditionActive = false;
}



void GetOutOfCarCondition::SetConditionActive()
{
     mbIsConditionActive = true;
     mMyVehicle = GetGameplayManager()->GetCurrentVehicle();
    GetEventManager()->TriggerEvent(EVENT_OUTOFCAR_CONDITION_ACTIVE,this);
    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        currentHud->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_TIMER_TEMP );
        currentHud->SetTimerBlinkingInterval(10000,0);
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
