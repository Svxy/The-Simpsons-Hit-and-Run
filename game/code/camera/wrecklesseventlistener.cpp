//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        WrecklessEventListener.cpp
//
// Description: Implement WrecklessEventListener
//
// History:     13/06/2002 + Created -- Cary Brisebois
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
#include <camera/WrecklessEventListener.h>
#include <meta/eventlocator.h>
#include <meta/locatorevents.h>



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
// WrecklessEventListener::WrecklessEventListener
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WrecklessEventListener::WrecklessEventListener() :
    mPlayerID( -1 )
{
    mLastPosition.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// WrecklessEventListener::~WrecklessEventListener
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WrecklessEventListener::~WrecklessEventListener()
{
}

//=============================================================================
// WrecklessEventListener::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void WrecklessEventListener::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == (EVENT_LOCATOR + LocatorEvent::CAMERA_CUT) )
    {
        EventLocator* evtLoc = static_cast<EventLocator*>(pEventData);
        rAssert( evtLoc );

        if ( evtLoc->GetPlayerID() == (unsigned int) mPlayerID && evtLoc->GetPlayerEntered() )
        {
            evtLoc->GetLocation( &mLastPosition );
        }
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
