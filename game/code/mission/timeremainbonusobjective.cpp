//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        timeremainbonusobjective.cpp
//
// Description: Implement TimeRemainBonusObjective
//
// History:     12/6/2002 + Created -- Cary Brisebois
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
#include <mission/timeremainbonusobjective.h>
#include <mission/mission.h>


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
// TimeRemainBonusObjective::TimeRemainBonusObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
TimeRemainBonusObjective::TimeRemainBonusObjective() : mMinTime( 0 )
{
    SetType( BonusObjective::TIME );
    SetSuccessful( true );
}

//=============================================================================
// TimeRemainBonusObjective::~TimeRemainBonusObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
TimeRemainBonusObjective::~TimeRemainBonusObjective()
{
}

//=============================================================================
// TimeRemainBonusObjective::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TimeRemainBonusObjective::Initialize()
{
}

//=============================================================================
// TimeRemainBonusObjective::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TimeRemainBonusObjective::Finalize()
{
}

//=============================================================================
// TimeRemainBonusObjective::GetNumericData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
unsigned int TimeRemainBonusObjective::GetNumericData()
{
    rAssert( mMission );

    return (static_cast<unsigned int>(mMission->GetMissionTimeLeftInMilliSeconds()) );
}


//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// TimeRemainBonusObjective::OnReset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TimeRemainBonusObjective::OnReset()
{
    SetSuccessful( true );
}

//=============================================================================
// TimeRemainBonusObjective::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TimeRemainBonusObjective::OnStart()
{
    //Turn on the HUD.
}

//=============================================================================
// TimeRemainBonusObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void TimeRemainBonusObjective::OnUpdate( unsigned int milliseconds )
{
    if ( GetStarted() && GetSuccessful() )
    {
        if ( mMission->GetMissionTimeLeftInMilliSeconds() > 0 && 
             static_cast<unsigned int>(mMission->GetMissionTimeLeftInMilliSeconds()) < mMinTime )
        {
            SetSuccessful( false );
            //Update the HUD
        }
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
