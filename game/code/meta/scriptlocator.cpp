//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement ScriptLocator
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
#include <meta/ScriptLocator.h>

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
// ScriptLocator::ScriptLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ScriptLocator::ScriptLocator()
{
}

//==============================================================================
// ScriptLocator::~ScriptLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ScriptLocator::~ScriptLocator()
{
}

//=============================================================================
// ScriptLocator::OnTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int playerID )
//
// Return:      void 
//
//=============================================================================
void ScriptLocator::OnTrigger( unsigned int playerID )
{
    //This will fire an event off that tell the sound system that a positional
    //sound is to be played using the position parameters given in the
    //the object indicated by the text key.
    //
    GetEventManager()->TriggerEvent( EVENT_POSITIONAL_SOUND_TRIGGER_HIT, this );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
