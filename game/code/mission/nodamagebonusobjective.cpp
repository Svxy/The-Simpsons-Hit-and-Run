//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        nodamagebonusobjective.cpp
//
// Description: Implement NoDamageBonusObjective
//
// History:     12/5/2002 + Created -- Cary Brisebois
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
#include <mission/nodamagebonusobjective.h>
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
// NoDamageBonusObjective::NoDamageBonusObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
NoDamageBonusObjective::NoDamageBonusObjective() : mElapsedTime( 0 )
{
    SetType( BonusObjective::NO_DAMAGE );
    SetSuccessful( true );
}

//=============================================================================
// NoDamageBonusObjective::~NoDamageBonusObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
NoDamageBonusObjective::~NoDamageBonusObjective()
{
}

//=============================================================================
// NoDamageBonusObjective::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoDamageBonusObjective::Initialize()
{
    GetEventManager()->AddListener( this, EVENT_COLLISION );
}

//=============================================================================
// NoDamageBonusObjective::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoDamageBonusObjective::Finalize()
{
    GetEventManager()->RemoveListener( this, EVENT_COLLISION );
}

//=============================================================================
// NoDamageBonusObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void NoDamageBonusObjective::HandleEvent( EventEnum id, void* pEventData )
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
// NoDamageBonusObjective::OnReset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoDamageBonusObjective::OnReset()
{
    SetSuccessful( true );
    mElapsedTime = 0;
}

//=============================================================================
// NoDamageBonusObjective::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NoDamageBonusObjective::OnStart()
{
    //Turn on the HUD.
}

//=============================================================================
// NoDamageBonusObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void NoDamageBonusObjective::OnUpdate( unsigned int milliseconds )
{
    if ( GetStarted() && GetSuccessful() )
    {
        mElapsedTime += milliseconds;
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
