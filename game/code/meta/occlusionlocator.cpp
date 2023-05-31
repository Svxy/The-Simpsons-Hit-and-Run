//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        OcclusionLocator.cpp
//
// Description: Implement OcclusionLocator
//
// History:     02/07/2002 + Created -- Cary Brisebois
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
#include <meta/OcclusionLocator.h>
#include <meta/LocatorEvents.h>
#include <events/EventManager.h>


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
// OcclusionLocator::OcclusionLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
OcclusionLocator::OcclusionLocator() :
    mNumOccTriggers( 0 )
{
}

//==============================================================================
// OcclusionLocator::~OcclusionLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
OcclusionLocator::~OcclusionLocator()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// OcclusionLocator::OnTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int playerID )
//
// Return:      void 
//
//=============================================================================
void OcclusionLocator::OnTrigger( unsigned int playerID )
{
    GetEventManager()->TriggerEvent((EventEnum)(EVENT_LOCATOR+LocatorEvent::OCCLUSION_ZONE),this);
}
