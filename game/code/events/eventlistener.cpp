//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        eventlistener.cpp
//
// Description: Implementation for EventListener abstract base class.
//              All objects that listen for events must be derived from
//              this class.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <events/eventlistener.h>
#include <events/eventmanager.h>

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
// EventListener::EventListener
//==============================================================================
//
// Description: Constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
EventListener::EventListener()
{
}


//==============================================================================
// EventListener::~EventListener
//==============================================================================
//
// Description: Destructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
EventListener::~EventListener()
{
    //
    // Remove all references to this object from the EventManager cause
    // it's about to go away.
    //
    GetEventManager()->RemoveAll( this );    
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

