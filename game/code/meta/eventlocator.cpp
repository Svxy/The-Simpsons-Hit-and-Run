//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement EventLocator
//
// History:     04/04/2002 + Created -- Cary Brisebois
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
#include <meta/EventLocator.h>
#include <meta/locatorevents.h>
#include <events/eventmanager.h>

#include <debug/profiler.h>

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
// EventLocator::EventLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
EventLocator::EventLocator()
{
    mMatrix.Identity();
}

//==============================================================================
// EventLocator::~EventLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
EventLocator::~EventLocator()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// EventLocator::OnTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int playerID )
//
// Return:      void 
//
//=============================================================================
void EventLocator::OnTrigger( unsigned int playerID )
{
    //This will fire an event off that uses the data field
    GetEventManager()->TriggerEvent( (EventEnum)( EVENT_LOCATOR + mEventType ), (void*)this );
}
