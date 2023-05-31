//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        simpsonssoundplayer.cpp
//
// Description: Implement SimpsonsSoundPlayer class, which interacts with
//              the sound renderer to play sounds, and tracks the player
//              resources in use.
//
// History:     29/06/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/simpsonssoundplayer.h>

#include <sound/soundmanager.h>
#include <sound/soundloader.h>
#include <sound/soundrenderercallback.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundresourcemanager.h>
#include <sound/soundrenderer/soundallocatedresource.h>
#include <sound/soundrenderer/playermanager.h>
#include <sound/soundrenderer/idasoundresource.h>

#include <memory/srrmemory.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

unsigned int SimpsonsSoundPlayer::s_playersCreated = 0;
unsigned int SimpsonsSoundPlayer::s_clipPlayersInUse = 0;
unsigned int SimpsonsSoundPlayer::s_streamPlayersInUse = 0;
Sound::daSoundResourceManager* SimpsonsSoundPlayer::s_resourceManager = NULL;
Sound::daSoundPlayerManager* SimpsonsSoundPlayer::s_playerManager = NULL;
SoundLoader* SimpsonsSoundPlayer::s_soundLoader = NULL;

//
// Limits on the number of players that can be playing clips/streams at once.
// Numbers are arbitrary and subject to experimentation
//
static const unsigned int s_maxActiveClipPlayersAllowed = 25;
static const unsigned int s_maxActiveStreamPlayersAllowed = 8;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SimpsonsSoundPlayer::SimpsonsSoundPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SimpsonsSoundPlayer::SimpsonsSoundPlayer() :
    m_playa( NULL ),
    m_callback( NULL )
    
{
    m_Type = Type_NonPositional;
    //
    // Get resource manager ptr and sound loader ptr if that hasn't been done yet
    //
    if( s_resourceManager == NULL )
    {
        s_resourceManager = Sound::daSoundRenderingManagerGet()->GetResourceManager();
    }

    if( s_playerManager == NULL )
    {
        s_playerManager = Sound::daSoundRenderingManagerGet()->GetPlayerManager();
    }

    if( s_soundLoader == NULL )
    {
        s_soundLoader = SoundManager::GetInstance()->GetSoundLoader();
    }

    //
    // Update statistics
    //
    ++s_playersCreated;
}

//==============================================================================
// SimpsonsSoundPlayer::~SimpsonsSoundPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SimpsonsSoundPlayer::~SimpsonsSoundPlayer()
{
    //
    // Update statistics
    //
    --s_playersCreated;

    if( m_callback != NULL )
    {
        m_callback->CancelGameCallbackAndRelease();
    }

    rAssert( m_playa == NULL );
}

//=============================================================================
// SimpsonsSoundPlayer::PlaySound
//=============================================================================
// Description: Play the sound resource with the given name
//
// Parameters:  resourceName - string with name of resource to play
//              callback - optional callback on playback completion
//
// Return:      true if sound could be played, false otherwise
//
//=============================================================================
bool SimpsonsSoundPlayer::PlaySound( const char* resourceName,
                                     SimpsonsSoundPlayerCallback* callback )
{
    return( PlaySound( ::radMakeKey32( resourceName ), callback ) );
}


//=============================================================================
// SimpsonsSoundPlayer::PlayResource
//=============================================================================
// Description: Play the sound resource
//
// Parameters:  resource - resource to play
//              callback - object to notify when we're done
//
// Return:      true if sound could be played, false otherwise 
//
//=============================================================================
bool SimpsonsSoundPlayer::PlayResource( IDaSoundResource* resource, 
                                        SimpsonsSoundPlayerCallback* callback /* = NULL  */ )
{
    bool canPlay;

    canPlay = QueueSound( resource, callback );
    if( canPlay )
    {
        PlayQueuedSound();
    }

    return( canPlay );
}

//=============================================================================
// SimpsonsSoundPlayer::PlaySound
//=============================================================================
// Description: Comment
//
// Parameters:  resourceKey - hashed value of the name of the sound resource 
//                            to play
//
// Return:      true if sound could be played, false otherwise 
//
//=============================================================================
bool SimpsonsSoundPlayer::PlaySound( Sound::daResourceKey resourceKey,
                                     SimpsonsSoundPlayerCallback* callback )
{
    IDaSoundResource* resource;
    bool retVal;

    resource = s_resourceManager->FindResource( resourceKey );

    if( resource != NULL )
    {
        retVal = PlayResource( resource, callback );
    }
    else
    {
        rDebugString( "Tried to play missing sound resource\n" );
        retVal = false;
    }

    return( retVal );
}

//=============================================================================
// SimpsonsSoundPlayer::QueueSound
//=============================================================================
// Description: Queue up a sound for playback, but don't play it.  Useful
//              for streamed dialog
//
// Parameters:  resourceName - name of sound resource to queue
//              callback - playback completion callback, unused if NULL
//              playUnbuffered - if streamer, don't use buffered data source
//
// Return:      true if sound could be played, false otherwise 
//
//=============================================================================
bool SimpsonsSoundPlayer::QueueSound( radKey32 resourceID, 
                                      SimpsonsSoundPlayerCallback* callback )
{
    IDaSoundResource* resource = s_resourceManager->FindResource( resourceID );

    if( resource != NULL )
    {
        return( QueueSound( resource, callback ) );
    }
    else
    {
        rDebugPrintf( "Couldn't play sound resource ID %d\n", resourceID );
        return( false );
    }
}

//=============================================================================
// SimpsonsSoundPlayer::QueueSound
//=============================================================================
// Description: Queue up a sound for playback, but don't play it.  Useful
//              for streamed dialog
//
// Parameters:  resource - sound resource to queue
//              callback - playback completion callback, unused if NULL
//              playUnbuffered - if streamer, don't use buffered data source
//
// Return:      true if sound could be played, false otherwise 
//
//=============================================================================
bool SimpsonsSoundPlayer::QueueSound( IDaSoundResource* resource,
                                      SimpsonsSoundPlayerCallback* callback )
{
    if( m_playa != NULL )
    {
        rDebugString( "Dropped sound, player busy\n" );
        return( false );
    }

    //
    // Make sure we haven't maxed our limit on playback of this type of
    // resource
    //
    if( resource->GetType() == IDaSoundResource::CLIP )
    {
        if( s_clipPlayersInUse < s_maxActiveClipPlayersAllowed )
        {
            ++s_clipPlayersInUse;
        }
        else
        {
            rAssertMsg( false, "Reached maximum allowable number of clip players\n" );
            return( false );
        }
    }
    else
    {
        rAssert( resource->GetType() == IDaSoundResource::STREAM );

        if( s_streamPlayersInUse < s_maxActiveStreamPlayersAllowed )
        {
            ++s_streamPlayersInUse;
        }
        else
        {
            rAssertMsg( false, "Reached maximum allowable number of stream players\n" );
            return( false );
        }
    }

    s_playerManager->CaptureFreePlayer( &m_playa, 
                                        resource,
                                        Type_Positional == m_Type );
    rAssert( m_playa != NULL );

    //
    // Reset trim, just to be safe
    //
    m_playa->SetExternalTrim( 1.0f );

    //
    // Create a callback object and point it toward whoever wants the callback
    //
    if( ( callback != NULL ) && ( m_callback == NULL ) )
    {
        m_callback = new(GMA_TEMP) SoundRenderingPlayerCallback( *this, callback );
        m_playa->RegisterSoundPlayerStateCallback( m_callback, NULL );
    }

    return( true );
}

//=============================================================================
// SimpsonsSoundPlayer::PlayQueuedSound
//=============================================================================
// Description: Play the sound we've queued for playback
//
// Parameters:  callback - callback to trigger on playback completion, ignored
//                         if NULL
//
// Return:      void 
//
//=============================================================================
void SimpsonsSoundPlayer::PlayQueuedSound( SimpsonsSoundPlayerCallback* callback )
{
    rAssert( m_playa != NULL );

    //
    // Play should not get stuck on pauses.  If this causes problems,
    // then I'll have to figure out how to make 100% sure that all
    // paused streams (e.g. NIS) get unpaused in a proper fashion
    //
    if( m_playa->IsPaused() )
    {
        m_playa->UberContinue();
    }

    m_playa->Play();

    //
    // If requested, create a callback object and point it toward whoever 
    // wants the callback
    //
    if( m_callback == NULL )
    {
        m_callback = new(GMA_TEMP) SoundRenderingPlayerCallback( *this, callback );
        m_playa->RegisterSoundPlayerStateCallback( m_callback, NULL );
    }
}

//=============================================================================
// SimpsonsSoundPlayer::OnPlaybackComplete
//=============================================================================
// Description: Callback from the sound renderer callback object when the
//              clip stops playback (not called for looping clips)
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SimpsonsSoundPlayer::OnPlaybackComplete()
{
    dumpSoundPlayer();
}

//=============================================================================
// SimpsonsSoundPlayer::Stop
//=============================================================================
// Description: Stop playing sound.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SimpsonsSoundPlayer::Stop()
{
    if( m_playa != NULL )
    {
        //
        // Stop() doesn't seem to play well with paused players.  They tend
        // to remain paused when you reuse them later.
        //
        if( m_playa->IsPaused() )
        {
            m_playa->Continue();
        }

        m_playa->Stop();

        if( m_playa != NULL )
        {
            //
            // The player didn't get released because we don't have
            // a callback set (presumably), so we do it ourselves
            //
            dumpSoundPlayer();
        }
        
        rAssert( m_playa == NULL );
    }
}

//=============================================================================
// SimpsonsSoundPlayer::Pause
//=============================================================================
// Description: Pause the sound player
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SimpsonsSoundPlayer::Pause()
{
    if( m_playa != NULL )
    {
        m_playa->Pause();
    }
}

//=============================================================================
// SimpsonsSoundPlayer::Continue
//=============================================================================
// Description: Unpause the sound player
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SimpsonsSoundPlayer::Continue()
{
    if( m_playa != NULL )
    {
        m_playa->Continue();
    }
}

//=============================================================================
// SimpsonsSoundPlayer::IsPaused
//=============================================================================
// Description: Indicate whether the player is paused
//
// Parameters:  None
//
// Return:      true if paused, false if not or player unused 
//
//=============================================================================
bool SimpsonsSoundPlayer::IsPaused()
{
    if( m_playa != NULL )
    {
        return( m_playa->IsPaused() );
    }
    else
    {
        return( false );
    }
}

//=============================================================================
// SimpsonsSoundPlayer::SetPitch
//=============================================================================
// Description: Set the pitch of the currently playing clip
//
// Parameters:  pitch - pitch setting to apply
//
// Return:      void 
//
//=============================================================================
void SimpsonsSoundPlayer::SetPitch( float pitch )
{
    if( m_playa != NULL )
    {
        m_playa->SetPitch( pitch );
    }
    else
    {
        rDebugString( "Can't set pitch without associated player\n" );
    }
}

//=============================================================================
// SimpsonsSoundPlayer::SetTrim
//=============================================================================
// Description: Set the trim of the currently playing clip
//
// Parameters:  trim - trim setting to apply
//
// Return:      void 
//
//=============================================================================
void SimpsonsSoundPlayer::SetTrim( float trim )
{
    if( m_playa != NULL )
    {
        m_playa->SetExternalTrim( trim );
    }
    else
    {
        rDebugString( "Can't set trim without associated player\n" );
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SimpsonsSoundPlayer::dumpSoundPlayer
//=============================================================================
// Description: To be called when we're done with the sound renderer's player
//
// Parameters:  None
//
// Return:      void  
//
//=============================================================================
void SimpsonsSoundPlayer::dumpSoundPlayer()
{
    if( m_callback )
    {
        m_callback->CancelGameCallbackAndRelease();
        m_playa->UnregisterSoundPlayerStateCallback( m_callback, NULL );
        m_callback = NULL;
    }

    if( m_playa->GetPlayerType() == IDaSoundResource::CLIP )
    {
        --s_clipPlayersInUse;
    }
    else
    {
        rAssert( m_playa->GetPlayerType() != IDaSoundResource::UNKNOWN );
        --s_streamPlayersInUse;
    }
    
    if( m_playa->IsCaptured() )
    {
        m_playa->UnCapture();
    }
    m_playa = NULL;
}