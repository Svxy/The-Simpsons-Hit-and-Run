//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfxfrontendlogic.cpp
//
// Description: Implements the SoundFXFrontEndLogic class, which handles
//              the translation of events into sound effects for the front
//              end.
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/soundfx/soundfxfrontendlogic.h>

#include <sound/soundmanager.h>

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
// SoundFXFrontEndLogic::SoundFXFrontEndLogic
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFXFrontEndLogic::SoundFXFrontEndLogic()
{
}

//==============================================================================
// SoundFXFrontEndLogic::~SoundFXFrontEndLogic
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFXFrontEndLogic::~SoundFXFrontEndLogic()
{
}

//=============================================================================
// SoundFXFrontEndLogic::RegisterEventListeners
//=============================================================================
// Description: Register as listener of sound effect events with Event Manager
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXFrontEndLogic::RegisterEventListeners()
{
    GetEventManager()->AddListener( this, EVENT_FE_MENU_SELECT );
    GetEventManager()->AddListener( this, EVENT_FE_MENU_BACK );
    GetEventManager()->AddListener( this, EVENT_FE_MENU_UPORDOWN );
    GetEventManager()->AddListener( this, EVENT_FE_CHEAT_SUCCESS );
    GetEventManager()->AddListener( this, EVENT_FE_CHEAT_FAILURE );
    GetEventManager()->AddListener( this, EVENT_FE_CREDITS_NEW_LINE );
}

//=============================================================================
// SoundFXFrontEndLogic::HandleEvent
//=============================================================================
// Description: Play sound effects in response to events
//
// Parameters:  id - Sound effect event identifier
//              pEventData - Currently unused
//
// Return:      void 
//
//=============================================================================
void SoundFXFrontEndLogic::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_FE_MENU_SELECT:
            playSFXSound( "accept", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_MENU_BACK:
            playSFXSound( "back", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_MENU_UPORDOWN:
            playSFXSound( "scroll", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_CHEAT_SUCCESS:
            playSFXSound( "cheat_success", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_CHEAT_FAILURE:
            playSFXSound( "cheat_fail", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_CREDITS_NEW_LINE:
            playCreditLine( reinterpret_cast<int>(pEventData) );
            break;
            
        default:
            rAssertMsg( false, "Huh?  Unexpected sound FX event\n" );
            break;
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

