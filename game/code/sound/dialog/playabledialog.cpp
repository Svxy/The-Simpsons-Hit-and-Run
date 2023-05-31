//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        playabledialog.cpp
//
// Description: Abstract base class for the basic unit of dialog playback.
//              A PlayableDialog object represents a piece of dialog which 
//              is to be played without interruption (unless it gets booted 
//              by a higher-priority PlayableDialog).
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/dialog/playabledialog.h>



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
// PlayableDialog::PlayableDialog
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PlayableDialog::PlayableDialog()
{
}

//==============================================================================
// PlayableDialog::PlayableDialog
//==============================================================================
// Description: Constructor.
//
// Parameters:	mission - mission number for dialog
//              level - level number
//              event - event that this dialog is played in response to
//
// Return:      N/A.
//
//==============================================================================
PlayableDialog::PlayableDialog( unsigned int level, unsigned int mission, EventEnum event ) :
    SelectableDialog( level, mission, event )
{
}

//==============================================================================
// PlayableDialog::~PlayableDialog
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PlayableDialog::~PlayableDialog()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
