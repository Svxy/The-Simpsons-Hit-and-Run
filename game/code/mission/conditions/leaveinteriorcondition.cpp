//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        LeaveInteriorCondition.cpp
//
// Description: Implement LeaveInteriorCondition
//
// History:     30/08/2002 + Created -- Cary Brisebois
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
#include <events/eventmanager.h>

#include <mission/conditions/LeaveInteriorCondition.h>



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
// LeaveInteriorCondition::LeaveInteriorCondition
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
LeaveInteriorCondition::LeaveInteriorCondition()
{
    this->SetType( COND_LEAVE_INTERIOR );
}

//=============================================================================
// LeaveInteriorCondition::~LeaveInteriorCondition
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
LeaveInteriorCondition::~LeaveInteriorCondition()
{
}

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//=============================================================================
// LeaveInteriorCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void LeaveInteriorCondition::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == (EVENT_LOCATOR + LocatorEvent::INTERIOR_EXIT) )
    {
        SetLeaveInterior( true );
    }

    MissionCondition::HandleEvent( id, pEventData );
}

//=============================================================================
// LeaveInteriorCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool LeaveInteriorCondition::IsClose()
{
    //
    // There's nothing tense about leaving a building
    //
    return( false );
}

//******************************************************************************
//
// ProtectedMember Functions
//
//******************************************************************************

//=============================================================================
// LeaveInteriorCondition::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LeaveInteriorCondition::OnInitialize()
{
    GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::INTERIOR_EXIT) );
}

//=============================================================================
// LeaveInteriorCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LeaveInteriorCondition::OnFinalize()
{
    GetEventManager()->RemoveListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::INTERIOR_EXIT) );
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
