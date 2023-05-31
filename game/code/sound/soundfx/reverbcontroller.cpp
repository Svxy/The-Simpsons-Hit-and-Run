//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        reverbcontroller.cpp
//
// Description: Implementation for ReverbController, which turns reverb on
//              and off and sets control values.
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radsound_hal.hpp>
#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/reverbcontroller.h>

#include <sound/soundfx/reverbsettings.h>
#include <sound/soundrenderer/soundrenderingmanager.h>

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
// ReverbController::ReverbController
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ReverbController::ReverbController() :
    m_targetGain( 0.0f ),
    m_currentGain( 0.0f ),
    m_fadeInMultiplier( 1.0f ),
    m_fadeOutMultiplier( 1.0f ),
    m_lastReverb( NULL ),
    m_queuedReverb( NULL )
{
    unsigned int event;
    EventManager* eventMgr = GetEventManager();

    rAssert( eventMgr != NULL );

    //
    // Register all of the ambience events
    //
    for( event = EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_CITY;
            event < EVENT_LOCATOR + LocatorEvent::PARKED_BIRDS;
            ++event )
    {
        eventMgr->AddListener( this, static_cast<EventEnum>(event) );
    }
	eventMgr->AddListener(this, EVENT_MISSION_RESET);
	eventMgr->AddListener(this, EVENT_MISSION_CHARACTER_RESET);

    // bmc -- add registering of extra ambient sound events (placeholder and otherwise)
    for( event = EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_COUNTRY_NIGHT;
            event <= EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PLACEHOLDER9;
            ++event )
    {
        eventMgr->AddListener( this, static_cast<EventEnum>(event) );
    }

    for( event = EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_SEASIDE_NIGHT;
            event <= EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PLACEHOLDER16;
            ++event )
    {
        eventMgr->AddListener( this, static_cast<EventEnum>(event) );
    }
    // bmc


}

//==============================================================================
// ReverbController::~ReverbController
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ReverbController::~ReverbController()
{
    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// ReverbController::SetReverbGain
//=============================================================================
// Description: Set the gain on the entire reverb system
//
// Parameters:  gain - 0 to 1 gain value
//
// Return: None
//
//=============================================================================
void ReverbController::SetReverbGain( float gain )
{
    ::radSoundHalSystemGet()->SetAuxGain( REVERB_AUX_EFFECT_NUMBER, gain );
}

//=============================================================================
// ReverbController::HandleEvent
//=============================================================================
// Description: Listen for events that will cause us to switch reverb modes
//              on and off
//
// Parameters:  id - event ID
//              pEventData - user data, unused
//
// Return:      void 
//
//=============================================================================
void ReverbController::HandleEvent( EventEnum id, void* pEventData )
{
    reverbSettings* settingsObj = NULL;
    reverbSettings* oldSettings = m_lastReverb;

    switch( id )
    {
        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PP_ROOM_1:
            settingsObj = getReverbSettings( "pproom1" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PP_ROOM_2:
            settingsObj = getReverbSettings( "pproom2" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PP_ROOM_3:
            settingsObj = getReverbSettings( "pproom3" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PP_TUNNEL_1:
            settingsObj = getReverbSettings( "PP_tunnel_01" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PP_TUNNEL_2:
            settingsObj = getReverbSettings( "PP_tunnel_02" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_BURNS_TUNNEL:
            settingsObj = getReverbSettings( "burns_tunnel" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_MANSION_INTERIOR:
            settingsObj = getReverbSettings( "mansion_interior" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_SEWERS:
            settingsObj = getReverbSettings( "sewers" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_STONE_CUTTER_TUNNEL:
            settingsObj = getReverbSettings( "stonecuttertunnel" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_STONE_CUTTER_HALL:
            settingsObj = getReverbSettings( "stonecutterhall" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_STADIUM_TUNNEL:
            settingsObj = getReverbSettings( "stadiumtunnel" );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_KRUSTYLU_EXTERIOR:
            settingsObj = getReverbSettings( "krustylu" );
            break;

        default:
            //
            // This should be triggered by any ambient sound trigger that
            // isn't mapped to reverb above
            //
            SetReverbOff();
            m_lastReverb = NULL;
            break;
    }

    if( settingsObj != NULL )
    {
        //
        // No sense in switching to something we're already doing.  That only
        // seems to lead to unnecessary clicks on PS2 anyway.
        //
        if( settingsObj != oldSettings )
        {
            if( m_currentGain > 0.0f )
            {
                //
                // We can't do a sudden switch without hearing popping.  Queue up this
                // switch to occur after we fade out
                //
                prepareFadeSettings( 0.0f, settingsObj->GetFadeInTime(), settingsObj->GetFadeOutTime() );
                m_queuedReverb = m_lastReverb;
            }
            else
            {
                SetReverbOn( settingsObj );
            }
        }
    }
}

//=============================================================================
// ReverbController::ServiceOncePerFrame
//=============================================================================
// Description: Handle the fading in and fading out of reverb
//
// Parameters:  elapsedTime - elapsed time since last update
//
// Return:      void 
//
//=============================================================================
void ReverbController::ServiceOncePerFrame( unsigned int elapsedTime )
{
    if( m_currentGain == m_targetGain )
    {
        if( ( m_targetGain == 0.0f )
            && ( m_queuedReverb != NULL ) )
        {
            //
            // We're done fading out, now we can make the switch and fade
            // back in
            //
            SetReverbOn( m_queuedReverb );
            m_queuedReverb = NULL;
        }

        return;
    }
    else if( m_currentGain < m_targetGain )
    {
        m_currentGain += ( elapsedTime * m_fadeInMultiplier );
        if( m_currentGain > m_targetGain )
        {
            m_currentGain = m_targetGain;
        }
    }
    else
    {
        m_currentGain -= ( elapsedTime * m_fadeOutMultiplier );
        if( m_currentGain < m_targetGain )
        {
            m_currentGain = m_targetGain;
        }
    }

    SetReverbGain( m_currentGain );
}

//=============================================================================
// ReverbController::PauseReverb
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ReverbController::PauseReverb()
{
    if( m_lastReverb != NULL )
    {
        SetReverbOff();
    }
}

//=============================================================================
// ReverbController::UnpauseReverb
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ReverbController::UnpauseReverb()
{
    if( m_lastReverb != NULL )
    {
        SetReverbOn( m_lastReverb );
    }
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

void ReverbController::registerReverbEffect( IRadSoundHalEffect* reverbEffect )
{
    ::radSoundHalSystemGet()->SetAuxEffect( REVERB_AUX_EFFECT_NUMBER, reverbEffect );
    ::radSoundHalSystemGet()->SetAuxGain( REVERB_AUX_EFFECT_NUMBER, 0.0f );

    Sound::daSoundRenderingManagerGet()->GetTheListener()->SetEnvironmentAuxSend( REVERB_AUX_EFFECT_NUMBER );
    Sound::daSoundRenderingManagerGet()->GetTheListener()->SetEnvEffectsEnabled( true );
}

//=============================================================================
// ReverbController::prepareFadeSettings
//=============================================================================
// Description: Set up the member values for fading reverb in and out
//
// Parameters:  targetGain - gain value to gradually move to
//              fadeInTime - time in seconds for the fade, assuming we're
//                           going from 0.0 to 1.0
//              fadeOutTime - similar to fadeInTime
//
// Return:      void 
//
//=============================================================================
void ReverbController::prepareFadeSettings( float targetGain, float fadeInTime, float fadeOutTime )
{
    m_targetGain = targetGain;

    m_fadeInMultiplier = fadeInTime;
    if( m_fadeInMultiplier == 0.0f )
    {
        m_fadeInMultiplier = 1.0f;
    }
    else
    {
        m_fadeInMultiplier = 1.0f / m_fadeInMultiplier;
    }
    m_fadeOutMultiplier = fadeOutTime;
    if( m_fadeOutMultiplier == 0.0f )
    {
        m_fadeOutMultiplier = 1.0f;
    }
    else
    {
        m_fadeOutMultiplier = 1.0f / m_fadeInMultiplier;
    }
}

//=============================================================================
// ReverbController::startFadeOut
//=============================================================================
// Description: Set up for the gradual reverb fadeout
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void ReverbController::startFadeOut()
{
    m_targetGain = 0.0f;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// ReverbController::getReverbSettings
//=============================================================================
// Description: Retrieve the object containing a group of reverb settings
//
// Parameters:  objName - name of reverbSettings object to find
//
// Return:      void 
//
//=============================================================================
reverbSettings* ReverbController::getReverbSettings( const char* objName )
{
    IRadNameSpace* nameSpace;

    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    
    if ( objName != NULL )
    {
    	rTunePrintf( "\n\nAUDIO: Reverb Settings: [%s]\n\n\n", objName );
    }

    m_lastReverb = static_cast<reverbSettings*>(nameSpace->GetInstance( objName ));

    return( m_lastReverb );
}