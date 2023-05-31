//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundrenderercallback.cpp
//
// Description: Implement SoundRenderingPlayerCallback
//
// History:     06/07/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/soundrenderercallback.h>

#include <sound/simpsonssoundplayer.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Initialially the list is empty
//
SoundRenderingPlayerCallback* radLinkedClass< SoundRenderingPlayerCallback >::s_pLinkedClassHead = NULL;
SoundRenderingPlayerCallback* radLinkedClass< SoundRenderingPlayerCallback >::s_pLinkedClassTail = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundRenderingPlayerCallback::SoundRenderingPlayerCallback
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundRenderingPlayerCallback::SoundRenderingPlayerCallback( SimpsonsSoundPlayer& playerObj,
                                                            SimpsonsSoundPlayerCallback* callbackObj ) :
    m_callbackObj( callbackObj ),
    m_playerObj( &playerObj )
{
}

//==============================================================================
// SoundRenderingPlayerCallback::~SoundRenderingPlayerCallback
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundRenderingPlayerCallback::~SoundRenderingPlayerCallback()
{
}

//=============================================================================
// SoundRenderingPlayerCallback::CancelGameCallbackAndRelease
//=============================================================================
// Description: Called from the model layer when we're no longer interested
//              in callbacks, probably because the sound player is stopping.
//              Empty out the callback member and release.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundRenderingPlayerCallback::CancelGameCallbackAndRelease()
{
    m_callbackObj = NULL;
    m_playerObj = NULL;
    Release();
}

//=============================================================================
// SoundRenderingPlayerCallback::OnSoundReady
//=============================================================================
// Description: Implements function for IDaSoundSoundState interface.  Called
//              when the sound renderer player has a sound cued and ready to
//              play.
//
// Parameters:  pData - user data
//
// Return:      void 
//
// Return:      void 
//
//=============================================================================
void SoundRenderingPlayerCallback::OnSoundReady( void* pData )
{
    //
    // Let the client know that the sound is ready for immediate playback
    //
    if( m_callbackObj != NULL )
    {
        m_callbackObj->OnSoundReady();
    }
}

//=============================================================================
// SoundRenderingPlayerCallback::OnSoundDone
//=============================================================================
// Description: Implements function for IDaSoundSoundState interface.  Called
//              when the sound renderer player is finished playing something.
//
// Parameters:  pData - user data
//
// Return:      void 
//
//=============================================================================
void SoundRenderingPlayerCallback::OnSoundDone( void* pData )
{
    SimpsonsSoundPlayerCallback* callbackObj = m_callbackObj;

    //
    // AddRef so that none of the callback stuff below can wipe us out until
    // we're done
    //
    AddRef();

    //
    // Trigger the player callback first so that it can clean itself up and
    // make itself available for the client receiving the callback.  Save
    // a copy of the client callback, since the player might ask us to
    // wipe it out
    //
    if( m_playerObj != NULL )
    {
        m_playerObj->OnPlaybackComplete();
    }

    //
    // Now the client, who can reuse the same player now
    //
    if( callbackObj != NULL )
    {
        callbackObj->OnPlaybackComplete();
    }

    Release();
}

//=============================================================================
// SoundRenderingPlayerCallback::CompletionCheck
//=============================================================================
// Description: A sanity check, usable by anyone who wants to ensure that all
//              sound players are closed and nobody is waiting around for
//              callbacks anymore.  If any SoundRenderingPlayerCallback objects
//              are still kicking around, we fail.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundRenderingPlayerCallback::CompletionCheck()
{
    rAssertMsg( GetLinkedClassHead() == NULL, "GAAAAAK!!  Sound renderer players are still running when they're not supposed to be!\n" );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
