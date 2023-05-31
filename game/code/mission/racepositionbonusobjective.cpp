//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        racepositionbonusobjective.cpp
//
// Description: Implement RacePositionBonusObjective
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
#include <mission/racepositionbonusobjective.h>
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
// RacePositionBonusObjective::RacePositionBonusObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
RacePositionBonusObjective::RacePositionBonusObjective() : 
    mPosition( 1 ),
    mDesiredPosition( 1 )
{
    SetType( BonusObjective::POSITION );
}

//=============================================================================
// RacePositionBonusObjective::~RacePositionBonusObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
RacePositionBonusObjective::~RacePositionBonusObjective()
{
}

//=============================================================================
// RacePositionBonusObjective::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RacePositionBonusObjective::Initialize()
{
    GetEventManager()->AddListener( this, EVENT_WAYAI_AT_DESTINATION );
}

//=============================================================================
// RacePositionBonusObjective::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RacePositionBonusObjective::Finalize()
{
    GetEventManager()->RemoveListener( this, EVENT_WAYAI_AT_DESTINATION );
}

//=============================================================================
// RacePositionBonusObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void RacePositionBonusObjective::HandleEvent( EventEnum id, void* pEventData )
{
    //We're handling this: EVENT_WAYAI_AT_DESTINATION
    //Everytime an AI car sends this, we are permanently back one position.

    if ( GetStarted() && GetSuccessful() )
    {
        ++mPosition;

        if ( mPosition > mDesiredPosition )
        {
            SetSuccessful( false );
            //TODO: Update the HUD!
        }
    }
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// RacePositionBonusObjective::OnReset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RacePositionBonusObjective::OnReset()
{
    SetSuccessful( true );
    mPosition = 1;
}

//=============================================================================
// RacePositionBonusObjective::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RacePositionBonusObjective::OnStart()
{
    //Turn on the HUD.
}

//=============================================================================
// RacePositionBonusObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void RacePositionBonusObjective::OnUpdate( unsigned int milliseconds )
{
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
