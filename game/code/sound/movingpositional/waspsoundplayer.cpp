//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        waspsoundplayer.cpp
//
// Description: Implement WaspSoundPlayer
//
// History:     3/10/2003 + Created -- Esan
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/movingpositional/waspsoundplayer.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <events/eventmanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// WaspSoundPlayer::WaspSoundPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
WaspSoundPlayer::WaspSoundPlayer() :
    m_isFadingIn( false ),
    m_attacking( false ),
    m_blowingUp( false )
{
    EventManager* eventMgr = GetEventManager();

    eventMgr->AddListener( this, EVENT_ACTOR_REMOVED );
    eventMgr->AddListener( this, EVENT_WASP_CHARGING );
    eventMgr->AddListener( this, EVENT_WASP_CHARGED );
    eventMgr->AddListener( this, EVENT_WASP_ATTACKING );
    eventMgr->AddListener( this, EVENT_WASP_BLOWED_UP );
}

//=============================================================================
// WaspSoundPlayer::~WaspSoundPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
WaspSoundPlayer::~WaspSoundPlayer()
{
    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// WaspSoundPlayer::Activate
//=============================================================================
// Description: Start playing wasp noises
//
// Parameters:  theActor - wasp object
//              soundName - name of sound resource to start playing for wasp
//
// Return:      void 
//
//=============================================================================
void WaspSoundPlayer::Activate( Actor* theActor )
{
    //
    // Play the fade-in sound
    //
    playWaspSound( "wasp_fade_in", theActor );
    m_isFadingIn = true;
}

//=============================================================================
// WaspSoundPlayer::HandleEvent
//=============================================================================
// Description: Look for actor destruction and disassociate when it happens
//
// Parameters:  id - event ID
//              pEventData - event user data
//
// Return:      void 
//
//=============================================================================
void WaspSoundPlayer::HandleEvent( EventEnum id, void* pEventData )
{
    //
    // Make sure we've got the event for the right wasp
    //
    if( static_cast<Actor*>(pEventData) != m_actor )
    {
        return;
    }

    switch( id )
    {
        case EVENT_ACTOR_REMOVED:
            deactivate();
            break;

        case EVENT_WASP_CHARGING:
            playWaspSound( "wasp_charging", m_actor );
            break;

        case EVENT_WASP_CHARGED:
            playWaspSound( "wasp_charged_idle", m_actor );
            break;

        case EVENT_WASP_ATTACKING:
            playWaspSound( "wasp_attack", m_actor );
            m_attacking = true;
            break;

        case EVENT_WASP_BLOWED_UP:
            playWaspSound( "wasp_destroyed", m_actor );
            m_blowingUp = true;
            break;

        default:
            rAssertMsg( false, "Unexpected event in WaspSoundPlayer::HandleEvent\n" );
            break;
    }
}

//=============================================================================
// WaspSoundPlayer::OnPlaybackComplete
//=============================================================================
// Description: Playback callback, so that we can make sound transitions
//              if necessary
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void WaspSoundPlayer::OnPlaybackComplete()
{
    if( m_isFadingIn || m_attacking )
    {
        m_isFadingIn = false;
        m_attacking = false;

        playWaspSound( "wasp_idle", m_actor );
    }
    else if( m_blowingUp )
    {
        //
        // Wasp destroyed, deactivate player
        //
        m_blowingUp = false;
        deactivate();
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// WaspSoundPlayer::deactivate
//=============================================================================
// Description: Stop sound playback and free player for next wasp
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void WaspSoundPlayer::deactivate()
{
    safeStop();

    ActorPlayer::deactivate();
}

//=============================================================================
// WaspSoundPlayer::safeStop
//=============================================================================
// Description: Stop sound without triggering playback callbacks
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void WaspSoundPlayer::safeStop()
{
    //
    // Kill the callbacks before stopping, so we don't get sounds
    // lingering around after we think we're stopped
    //
    m_isFadingIn = false;
    m_attacking = false;

    m_player.Stop();
}

//=============================================================================
// WaspSoundPlayer::playWaspSound
//=============================================================================
// Description: Play a sound for the wasp
//
// Parameters:  soundName - name of sound resource
//              theActor - wasp actor object
//
// Return:      void 
//
//=============================================================================
void WaspSoundPlayer::playWaspSound( const char* soundName, Actor* theActor )
{
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    positionalSoundSettings* parameters;

    //
    // Kill the old sound, disabling any callback action first.
    //
    safeStop();

    //
    // Get the positionalSoundSettings object for the wasp sound
    //
    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    nameSpaceObj = nameSpace->GetInstance( ::radMakeKey32( "wasp_settings" ) );
    if( nameSpaceObj != NULL )
    {
        parameters = reinterpret_cast<positionalSoundSettings*>( nameSpaceObj );

        playSound( parameters, soundName, theActor );
    }
    else
    {
        rDebugString( "Couldn't play wasp sound, no matching settings found" );
    }
}
