//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Transevent.cpp
//
// Description: Implement TransEvent
//
// History:     23/05/2002 + Created -- NAME
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

#include <presentation/presentation.h>
#include <presentation/transitionplayer.h>
#include <presentation/presevents/transevent.h>

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

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// TransitionEvent::GetPlayer
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      AnimationPlayer
//
//=============================================================================
AnimationPlayer* TransitionEvent::GetPlayer()
{
    return( GetPresentationManager()->GetTransPlayer() );
}

//=============================================================================
// TransitionEvent::Start
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TransitionEvent::Start()
{
    TransitionPlayer* player = GetPresentationManager()->GetTransPlayer();

    player->SetTransition( &transInfo );

    player->Play();
}

