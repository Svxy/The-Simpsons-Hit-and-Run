//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        nissoundplayer.cpp
//
// Description: Implement NISSoundPlayer, which interacts with the dialog
//              system to provide NIS sounds.
//
// History:     10/5/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech

//========================================
// Project Includes
//========================================
#include <sound/nis/nissoundplayer.h>

#include <sound/soundmanager.h>

#include <events/eventmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Table of NIS resources.
//

NISPlayerGroup::NISPlayerGroup() :
    m_soundID( 0 ),
    m_loadCallback( NULL ),
    m_playCallback( NULL ),
    m_soundQueued( false )
{
}

NISPlayerGroup::~NISPlayerGroup()
{
}

void NISPlayerGroup::LoadSound( radKey32 soundID, NISSoundLoadedCallback* callback )
{
    //
    // For NIS playback, don't use buffered data sources.  Hopefully we
    // won't be play these in situations where skipping is likely.  If it
    // happens, then we can experiment with short buffers, I suppose.
    // Stinky limited IOP memory.
    //
    m_soundQueued = m_player.QueueSound( soundID, this );

    if( m_soundQueued )
    {
        m_loadCallback = callback;
        m_soundID = soundID;
    }
}

void NISPlayerGroup::PlaySound( rmt::Box3D* box, NISSoundPlaybackCompleteCallback* callback )
{
    radSoundVector position;
    rmt::Vector midpoint;

    if( m_soundQueued )
    {
        //
        // Calculate position to play at
        //
        rAssert( box != NULL );
        midpoint = box->Mid();
        position.SetElements( midpoint.x, midpoint.y, midpoint.z );

        m_playCallback = callback;
        m_player.PlayQueuedSound( position, this );
    }
}

void NISPlayerGroup::StopAndDumpSound()
{
    // Just in case.
    m_loadCallback = NULL;
    m_playCallback = NULL;
    m_soundQueued = false;
    m_soundID = 0;

    m_player.Stop();
}

bool NISPlayerGroup::IsSoundIDLoaded( radKey32 soundID )
{
    return( m_soundID == soundID );
}

void NISPlayerGroup::Continue()
{
    if( m_player.IsPaused() )
    {
        m_player.Continue();
    }
}

//=============================================================================
// NISPlayerGroup::OnSoundReady
//=============================================================================
// Description: Called from m_player when sound is cued.  Notify the
//              callback object
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void NISPlayerGroup::OnSoundReady()
{
    if( m_loadCallback )
    {
        m_loadCallback->NISSoundLoaded();
        m_loadCallback = NULL;
    }
}

//=============================================================================
// NISPlayerGroup::OnPlaybackComplete
//=============================================================================
// Description: Called from m_player when playback is done.  Notify the
//              callback object
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void NISPlayerGroup::OnPlaybackComplete()
{
    if( m_playCallback )
    {
        m_playCallback->NISSoundPlaybackComplete();
        m_playCallback = NULL;
    }

    m_soundID = 0;
}

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// NISSoundPlayer::NISSoundPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
NISSoundPlayer::NISSoundPlayer() :
    m_currentFEGag( 0 )
{
    //
    // Register as an event listener
    //
    GetEventManager()->AddListener( this, EVENT_FE_GAG_INIT );
    GetEventManager()->AddListener( this, EVENT_FE_GAG_START );
    GetEventManager()->AddListener( this, EVENT_FE_GAG_STOP );
}

//==============================================================================
// NISSoundPlayer::~NISSoundPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
NISSoundPlayer::~NISSoundPlayer()
{
}

//=============================================================================
// NISSoundPlayer::LoadNISSound
//=============================================================================
// Description: Cue an NIS sound for playback
//
// Parameters:  NISSound - sound to load
//              callback - object to notify when loading is complete
//
// Return:      void 
//
//=============================================================================
void NISSoundPlayer::LoadNISSound( radKey32 NISSoundID, NISSoundLoadedCallback* callback )
{
    unsigned int i;

    for( i = 0; i < NUM_NIS_PLAYERS; i++ )
    {
        if( m_NISPlayers[i].IsFree() )
        {
            m_NISPlayers[i].LoadSound( NISSoundID, callback );
            break;
        }
    }

    rAssert( i < NUM_NIS_PLAYERS );
}

//=============================================================================
// NISSoundPlayer::PlayNISSound
//=============================================================================
// Description: Play the cued NIS sound
//
// Parameters:  callback - object to notify when playback is complete
//
// Return:      void 
//
//=============================================================================
void NISSoundPlayer::PlayNISSound( radKey32 NISSoundID, rmt::Box3D* box, NISSoundPlaybackCompleteCallback* callback )
{
    unsigned int i;

    if( NISSoundID == 0 )
    {
        rDebugString( "Attempting to play NIS sound where no sound file specified. Ignored.\n" );
    }
    else
    {
        for( i = 0; i < NUM_NIS_PLAYERS; i++ )
        {
            if( m_NISPlayers[i].IsSoundIDLoaded( NISSoundID ) )
            {
                m_NISPlayers[i].PlaySound( box, callback );
                break;
            }
        }

        if( i >= NUM_NIS_PLAYERS )
        {
            rTuneString( "Attempting to play NIS sound which wasn't loaded. Tell Cory.\n" );
            //rTuneAssert( false );
        }
    }
}

//=============================================================================
// NISSoundPlayer::StopAndDumpNISSound
//=============================================================================
// Description: Stop the NIS sound if it's playing
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void NISSoundPlayer::StopAndDumpNISSound( radKey32 NISSoundID )
{
    unsigned int i;

    for( i = 0; i < NUM_NIS_PLAYERS; i++ )
    {
        if( m_NISPlayers[i].IsSoundIDLoaded( NISSoundID ) )
        {
            m_NISPlayers[i].StopAndDumpSound();
            break;
        }
    }
}

//=============================================================================
// NISSoundPlayer::PauseAllNISPlayers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NISSoundPlayer::PauseAllNISPlayers()
{
    unsigned int i;

    for( i = 0; i < NUM_NIS_PLAYERS; i++ )
    {
        m_NISPlayers[i].Pause();
    }
}

//=============================================================================
// NISSoundPlayer::ContinueAllNISPlayers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NISSoundPlayer::ContinueAllNISPlayers()
{
    unsigned int i;

    for( i = 0; i < NUM_NIS_PLAYERS; i++ )
    {
        m_NISPlayers[i].Continue();
    }
}

//=============================================================================
// NISSoundPlayer::HandleEvent
//=============================================================================
// Description: Take care of NIS events thrown by the front end.
//
// Parameters:  id - event ID
//              pEventData - FE gag name (radKey32) for EVENT_FE_GAG_INIT, unused for rest
//
// Return:      void 
//
//=============================================================================
void NISSoundPlayer::HandleEvent( EventEnum id, void* pEventData )
{
    rmt::Box3D box;
    rmt::Vector low;
    rmt::Vector high;
    radKey32 gagName = 0;

    switch( id )
    {
        case EVENT_FE_GAG_INIT:
            gagName = reinterpret_cast<radKey32>( pEventData );
            loadFEGag( gagName );
            break;

        case EVENT_FE_GAG_START:
            //
            // Use a hardcoded position here for now.  Not ideal, but the
            // window isn't going anywhere soon.
            //
            low.Set( -2.04f, 1.9f, -0.2f );
            high.Set( -2.0f, 1.96f, -0.1f );
            box.Set( low, high );

            PlayNISSound( m_currentFEGag, &box, NULL );
            break;

        case EVENT_FE_GAG_STOP:
            StopAndDumpNISSound( m_currentFEGag );
            break;

        default:
            rAssertMsg( false, "Huh?  Shouldn't get an event here." );
            break;
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// NISSoundPlayer::loadFEGag
//=============================================================================
// Description: Find the appropriate FE gag and prep it for playback
//
// Parameters:  gagIndex - index for gag, sent from front end
//
// Return:      void 
//
//=============================================================================
void NISSoundPlayer::loadFEGag( radKey32 gagKey )
{
    m_currentFEGag = gagKey;

    LoadNISSound( gagKey, NULL );
}
