//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fmvevent.cpp
//
// Description: Implement FMVEvent
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

#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/presentation.h>
#include <presentation/presevents/fmvevent.h>
#include <memory/srrmemory.h>
#include <render/rendermanager/rendermanager.h>

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

FMVEvent::FMVEvent () : PresentationEvent ()
{
}


FMVEvent::~FMVEvent ()
{
}



//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// FMVEvent::GetPlayer
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      AnimationPlayer
//
//=============================================================================
AnimationPlayer* FMVEvent::GetPlayer()
{
    return( GetPresentationManager()->GetFMVPlayer() );
}

FMVEvent::FMVEventData::FMVEventData() : AudioIndex( 0 ), Allocator( GMA_LEVEL_MOVIE ), KillMusic( false )
{
}