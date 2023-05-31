//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        nocopbonusobjective.cpp
//
// Description: Implement NoCopBonusObjective
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
#include <mission/nocopbonusobjective.h>
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
// NoCopBonusObjective::NoCopBonusObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
NoCopBonusObjective::NoCopBonusObjective() : mElapsedTime( 0 )
{
    SetType( BonusObjective::NO_CHASE_COLLISIONS );
    SetSuccessful( true );
}

//=============================================================================
// NoCopBonusObjective::~NoCopBonusObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
NoCopBonusObjective::~NoCopBonusObjective()
{
}

//=============================================================================
// NoCopBonusObjective::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoCopBonusObjective::Initialize()
{
    GetEventManager()->AddListener( this, EVENT_COLLISION );
}

//=============================================================================
// NoCopBonusObjective::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoCopBonusObjective::Finalize()
{
    GetEventManager()->RemoveListener( this, EVENT_COLLISION );
}

//=============================================================================
// NoCopBonusObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void NoCopBonusObjective::HandleEvent( EventEnum id, void* pEventData )
{
    if ( GetStarted() && GetSuccessful() )
    {
        SetSuccessful( false );
        //TODO: Update the HUD!
    }
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// NoCopBonusObjective::OnReset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoCopBonusObjective::OnReset()
{
    SetSuccessful( true );
    mElapsedTime = 0;
}

//=============================================================================
// NoCopBonusObjective::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoCopBonusObjective::OnStart()
{
    //Turn on the HUD.
}

//=============================================================================
// NoCopBonusObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void NoCopBonusObjective::OnUpdate( unsigned int milliseconds )
{
    if ( GetStarted() )
    {
        mElapsedTime += milliseconds;
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
