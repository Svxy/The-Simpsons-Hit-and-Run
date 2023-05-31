//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement TimeOutCondition
//
// History:     08/07/2002 + Created -- NAME
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

#include <mission/gameplaymanager.h>
#include <mission/mission.h>
#include <mission/conditions/timeoutcondition.h>

#include <worldsim/hitnrunmanager.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <events/eventmanager.h>

#include <interiors/interiormanager.h>

#include <worldsim/hitnrunmanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// TimeOutCondition::TimeOutCondition
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TimeOutCondition::TimeOutCondition() :
    //mHitNRun( false ),
    mDone( false )
{
    this->SetType( COND_TIME_OUT );

    //GetEventManager()->AddListener( this, EVENT_HIT_AND_RUN_CAUGHT );
}

//==============================================================================
// TimeOutCondition::~TimeOutCondition
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TimeOutCondition::~TimeOutCondition()
{
    //GetEventManager()->RemoveListener( this, EVENT_HIT_AND_RUN_CAUGHT );
}

//=============================================================================
// TimeOutCondition::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void TimeOutCondition::Update( unsigned int elapsedTime )
{
    if ( !mDone )
    {
        Mission* mission = GetGameplayManager()->GetCurrentMission();
        int timeLeft = mission->GetMissionTimeLeftInSeconds();
        if( timeLeft < 1 && !mission->IsComplete() &&
            !GetInteriorManager()->IsEntering() && 
            !GetInteriorManager()->IsExiting() &&
            !GetHitnRunManager()->BustingPlayer() )
        {
            mDone = true;

            //if ( mHitNRun )
            //{
            //    //Not failed, start the action!
            //    GetHitnRunManager()->MaxHitnRunValue();
            //    //Disable run down of time.
            //    GetHitnRunManager()->DisableMeterDecay();

            //    //Pre-emptive.
            //    GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_TIMER );
            //}
            //else 
            {
                SetIsViolated( true );
            }
        }
    }
}

//=============================================================================
// TimeOutCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool TimeOutCondition::IsClose()
{
    return( GetGameplayManager()->GetCurrentMission()->GetMissionTimeLeftInSeconds() < 20 );
}


//chuck hack hack to for gamble races.
void TimeOutCondition::SetViolated(bool flag)
{
    SetIsViolated(flag);
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
