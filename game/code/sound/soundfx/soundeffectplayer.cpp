//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundeffectplayer.cpp
//
// Description: Implement SoundEffectPlayer, which switches between
//              the objects that direct the soundfx logic for various game
//              states
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
#include <sound/soundfx/soundeffectplayer.h>

#include <sound/soundfx/soundfxfrontendlogic.h>
#include <sound/soundfx/soundfxgameplaylogic.h>
#include <sound/soundfx/soundfxpauselogic.h>

#ifdef RAD_XBOX
#include <sound/soundfx/xboxreverbcontroller.h>
#elif RAD_WIN32
#include <sound/soundfx/win32reverbcontroller.h>
#elif RAD_PS2
#include <sound/soundfx/ps2reverbcontroller.h>
#else
#include <sound/soundfx/gcreverbcontroller.h>
#endif

#include <memory/srrmemory.h>

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
// SoundEffectPlayer::SoundEffectPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundEffectPlayer::SoundEffectPlayer() :
    m_reverbController( NULL ),
    m_currentState( FXSTATE_INVALID )
{
    unsigned int i;

    for( i = 0; i < FXSTATE_MAX_STATES; i++ )
    {
        m_logicObjects[i] = NULL;
    }

    initialize();
}

//==============================================================================
// SoundEffectPlayer::~SoundEffectPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundEffectPlayer::~SoundEffectPlayer()
{
    unsigned int i;

    for( i = 0; i < FXSTATE_MAX_STATES; i++ )
    {
        if( m_logicObjects[i] != NULL )
        {
            delete m_logicObjects[i];
        }
    }

    delete m_reverbController;
}

//=============================================================================
// SoundEffectPlayer::ServiceOncePerFrame
//=============================================================================
// Description: Positional sound servicing
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundEffectPlayer::ServiceOncePerFrame( unsigned int elapsedTime )
{
    if( m_currentState != FXSTATE_INVALID )
    {
        m_logicObjects[m_currentState]->ServiceOncePerFrame( elapsedTime );
    }

    m_reverbController->ServiceOncePerFrame( elapsedTime );
}

void SoundEffectPlayer::OnPauseStart()
{
    m_reverbController->PauseReverb();
}

void SoundEffectPlayer::OnPauseEnd()
{
    m_reverbController->UnpauseReverb(); 
}

//=============================================================================
// SoundEffectPlayer::PlayCarOptionStinger
//=============================================================================
// Description: The following four functions play stinger sounds for the
//              level settings in the options menu
//
// Parameters:  trim - trim setting to play it at
//
// Return:      void 
//
//=============================================================================
void SoundEffectPlayer::PlayCarOptionStinger( float trim )
{
    playStinger( "car_stinger", trim );
}

void SoundEffectPlayer::PlayDialogOptionStinger( float trim )
{
    playStinger( "dialog_stinger", trim );
}

void SoundEffectPlayer::PlayMusicOptionStinger( float trim )
{
    playStinger( "music_stinger", trim );
}

void SoundEffectPlayer::PlaySfxOptionStinger( float trim )
{
    playStinger( "sfx_stinger", trim );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SoundEffectPlayer::initialize
//=============================================================================
// Description: Create the sound FX logic objects during initialization
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundEffectPlayer::initialize()
{
    m_logicObjects[FXSTATE_FRONTEND] = new(GMA_PERSISTENT) SoundFXFrontEndLogic();
    m_logicObjects[FXSTATE_GAMEPLAY] = new(GMA_PERSISTENT) SoundFXGameplayLogic();
    m_logicObjects[FXSTATE_PAUSED] = new(GMA_PERSISTENT) SoundFXPauseLogic();

#ifdef RAD_PS2
    m_reverbController = new(GMA_PERSISTENT) PS2ReverbController();
#elif RAD_XBOX
    m_reverbController = new(GMA_PERSISTENT) XboxReverbController();
#elif RAD_WIN32
    m_reverbController = new(GMA_PERSISTENT) Win32ReverbController();
#else
    m_reverbController = new(GMA_PERSISTENT) GCReverbController();
#endif
}

//=============================================================================
// SoundEffectPlayer::setSFXState
//=============================================================================
// Description: Switch to a new sound FX state
//
// Parameters:  newState - state to be switched to
//
// Return:      void 
//
//=============================================================================
void SoundEffectPlayer::setSFXState( SFXState newState )
{
    rAssert( newState < FXSTATE_MAX_STATES );
    
    if( m_currentState != FXSTATE_INVALID )
    {
        m_logicObjects[m_currentState]->UnregisterEventListeners();
    }

    m_currentState = newState;
    m_logicObjects[m_currentState]->RegisterEventListeners();
}

//=============================================================================
// SoundEffectPlayer::doCleanup
//=============================================================================
// Description: Shut down anything that might be playing
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundEffectPlayer::doCleanup()
{
    m_logicObjects[m_currentState]->Cleanup();

    m_reverbController->SetReverbOff();
}

//=============================================================================
// SoundEffectPlayer::playStinger
//=============================================================================
// Description: Play the given stinger resource at the specified trim
//
// Parameters:  stingerName - name of sound resource for stinger
//              trim - volume to play it at
//
// Return:      void 
//
//=============================================================================
void SoundEffectPlayer::playStinger( const char* stingerName, float trim )
{
    m_stingerPlayer.PlaySound( stingerName );
    m_stingerPlayer.SetTrim( trim );
}
