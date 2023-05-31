//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:       TimerObjective.cpp
//
// Description: ImplementTimerObjective
//
// History:    may 21,2003
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <string.h>

//========================================
// Project Includes
//========================================
#include <mission/objectives/timerobjective.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <events/eventmanager.h>


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
//TimerObjective::TimerObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
TimerObjective::TimerObjective()
    
{
    mDurationTime =0;
    mElapsedTime =0;
   
}

//=============================================================================
//TimerObjective::~TimerObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
TimerObjective::~TimerObjective()
{

}

void TimerObjective::SetTimer(unsigned int milliseconds)
{
    mDurationTime = milliseconds;
}



//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
//TimerObjective::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TimerObjective::OnInitialize()
{
       
}

//=============================================================================
//TimerObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TimerObjective::OnFinalize()
{
    
}


void TimerObjective::Update(unsigned int elaspedTime)
{
    OnUpdate(elaspedTime);
}


void TimerObjective::OnUpdate(unsigned int elapsedTime)
{
    mElapsedTime+=elapsedTime;

    if (mElapsedTime>mDurationTime)
    {
        SetFinished(true);
    }
}
   
   



//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
