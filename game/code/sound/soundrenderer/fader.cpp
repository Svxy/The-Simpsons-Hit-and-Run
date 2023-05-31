//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fader.cpp
//
// Description: Implementation for Fader class which brings down volume on
//              associated multi-input knob.  Replaces IRadSoundFade
//              objects from radsound/util.
//
// History:     13/08/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <float.h>
#include <radmath/radmath.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundrenderer/fader.h>

#include <sound/soundrenderer/playermanager.h>
#include <sound/soundrenderer/idasoundtuner.h>

using namespace Sound;

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

Fader* Fader::s_faderUpdateList = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Fader::Fader
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Fader::Fader( globalSettings* duckSettings, 
              DuckSituations situation,
              daSoundPlayerManager& playerMgr,
              IDaSoundTuner& tuner ) :
    m_duckSituation( situation ),
    m_playerManager( playerMgr ),
    m_tuner( tuner )
{
    m_Time = 750;
    m_In = true;
    m_State = FadedIn;
    m_callback = NULL;
    m_nextUpdatableFader = NULL;

    ReinitializeFader( duckSettings );
}

//==============================================================================
// Fader::~Fader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Fader::~Fader()
{
    if( s_faderUpdateList != NULL )
    {
        removeFromUpdateList();
    }
}

//========================================================================
// Fader::SetTime
//========================================================================
    
void Fader::SetTime( unsigned int milliseconds )
{
    m_Time = milliseconds;
}

//========================================================================
// Fader::GetTime
//========================================================================

unsigned int Fader::GetTime( void )
{
    return m_Time;
}

//========================================================================
// Fader::BroadCast
//========================================================================

void Fader::BroadCast( void )
{
    m_playerManager.PlayerFaderVolumeChange( SOUND_EFFECTS, m_currentVolumes.duckVolume[DUCK_SFX] );
    m_playerManager.PlayerFaderVolumeChange( CARSOUND, m_currentVolumes.duckVolume[DUCK_CAR] );
    m_playerManager.PlayerFaderVolumeChange( DIALOGUE, m_currentVolumes.duckVolume[DUCK_DIALOG] );
    m_playerManager.PlayerFaderVolumeChange( MUSIC, m_currentVolumes.duckVolume[DUCK_MUSIC] );
    m_playerManager.PlayerFaderVolumeChange( AMBIENCE, m_currentVolumes.duckVolume[DUCK_AMBIENCE] );

    m_tuner.SetFaderGroupTrim( DUCK_SFX, m_currentVolumes.duckVolume[DUCK_SFX] );
    m_tuner.SetFaderGroupTrim( DUCK_CAR, m_currentVolumes.duckVolume[DUCK_CAR] );
    m_tuner.SetFaderGroupTrim( DUCK_DIALOG, m_currentVolumes.duckVolume[DUCK_DIALOG] );
    m_tuner.SetFaderGroupTrim( DUCK_MUSIC, m_currentVolumes.duckVolume[DUCK_MUSIC] );
    m_tuner.SetFaderGroupTrim( DUCK_AMBIENCE, m_currentVolumes.duckVolume[DUCK_AMBIENCE] );
}

//========================================================================
// Fader::Fade
//========================================================================

void Fader::Fade( bool in, DuckVolumeSet* initialVolumes, DuckVolumeSet* targetVolumes )
{
    unsigned int i;

    m_In = in;

    //
    // Set current values to whatever we're fading from and target values
    // to whatever we're fading to
    //
    if( initialVolumes != NULL )
    {
        for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
        {
            m_currentVolumes.duckVolume[i] = initialVolumes->duckVolume[i];
        }
    }
    else
    {
        for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
        {
            if( m_In )
            {
                m_currentVolumes.duckVolume[i] = m_globalDuckSettings.duckVolume[i];
            }
            else
            {
                m_currentVolumes.duckVolume[i] = 1.0f;
            }
        }
    }

    if( targetVolumes != NULL )
    {
        for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
        {
            m_targetVolumes.duckVolume[i] = targetVolumes->duckVolume[i];
        }
    }
    else
    {
        for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
        {
            if( m_In )
            {
                m_targetVolumes.duckVolume[i] = 1.0f;
            }
            else
            {
                m_targetVolumes.duckVolume[i] = m_globalDuckSettings.duckVolume[i];
            }
        }
    }
    
    //
    // Now calculate the time steps
    //
    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        if ( m_Time == 0 )
        {
            // avoid divide by zero

            m_stepValues[i] = FLT_MAX / 10.0f; // Some arbitrarily large number
        }
        else
        {		
            m_stepValues[i] = ( rmt::Fabs( m_currentVolumes.duckVolume[i] - m_targetVolumes.duckVolume[i] ) ) / ( m_Time );
        }
    }

    setState( );

    addToUpdateList( );    
}

//========================================================================
// Fader::RegisterStateCallback
//========================================================================

void Fader::RegisterStateCallback( FaderStateChangeCallback* callback )
{
    //
    // I'm assuming only one callback is set at a time
    //
    rAssert( m_callback == NULL );
    m_callback = callback;
}

//========================================================================
// Fader::UnRegisterStateCallback
//========================================================================

void Fader::UnRegisterStateCallback( FaderStateChangeCallback* callback )
{
    //
    // Accept the callback as a parameter to test my assumption that
    // we only set one at a time
    //
    rAssert( m_callback == callback );

    m_callback = NULL;
}

//========================================================================
// Fader::GetState
//========================================================================

Fader::State Fader::GetState( void )
{
    unsigned int i;
    float targetValue;
    State currentState = m_In ? FadedIn : FadedOut;

    //
    // Test each of the fading volumes.  If one of them hasn't hit
    // the target yet, we're still fading
    //
    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        targetValue = m_In ? 1.0f : m_globalDuckSettings.duckVolume[i];

        if( m_currentVolumes.duckVolume[i] != targetValue )
        {
            currentState = m_In ? FadingIn : FadingOut;
            break;
        }
    }

    return( currentState );
}

//========================================================================
// Fader::OnTimerDone
//========================================================================

void Fader::Update( unsigned int elapsed )
{
    unsigned int i;
    float stepValue;
    bool allTargetsHit = true;

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        stepValue = m_stepValues[i] * elapsed; // adjust for game chug.
        
        if ( stepValue >= radSoundVolumeChangeThreshold )
        {
            stepValue = radSoundVolumeChangeThreshold;
        }

        if ( m_currentVolumes.duckVolume[i] < m_targetVolumes.duckVolume[i] )
        {
            m_currentVolumes.duckVolume[i] += stepValue;

            if ( m_currentVolumes.duckVolume[i] >= m_targetVolumes.duckVolume[i] )
            {
                m_currentVolumes.duckVolume[i] = m_targetVolumes.duckVolume[i];
            }
            else
            {
                allTargetsHit = false;
            }
        }
        else if ( m_currentVolumes.duckVolume[i] > m_targetVolumes.duckVolume[i] )
        {
            m_currentVolumes.duckVolume[i] -= stepValue;

            if ( m_currentVolumes.duckVolume[i] <= m_targetVolumes.duckVolume[i] )
            {
                m_currentVolumes.duckVolume[i] = m_targetVolumes.duckVolume[i];
            }
            else
            {
                allTargetsHit = false;
            }
        }

    }

    BroadCast();

    if ( allTargetsHit )
    {
        removeFromUpdateList( );

        setState( );
    }
}

//=============================================================================
// Fader::UpdateAllFaders
//=============================================================================
// Description: Run through the fader list and update everything
//
// Parameters:  elapsedTime - number of elapsed msecs since last call
//
// Return:      void 
//
//=============================================================================
void Fader::UpdateAllFaders( unsigned int elapsedTime )
{
    Fader* currFader;

    currFader = s_faderUpdateList;
    while( currFader != NULL )
    {
        currFader->Update( elapsedTime );
        currFader = currFader->m_nextUpdatableFader;
    }
}

//=============================================================================
// Fader::ReinitializeFader
//=============================================================================
// Description: Get the ducking parameters from the global settings object
//
// Parameters:  ( globalSettings* settingObj )
//
// Return:      void 
//
//=============================================================================
void Fader::ReinitializeFader( globalSettings* settingObj )
{
    unsigned int i;

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        if( settingObj == NULL )
        {
            m_globalDuckSettings.duckVolume[i] = 0.0f;
        }
        else
        {
            m_globalDuckSettings.duckVolume[i] = 
                settingObj->GetDuckVolume( m_duckSituation, static_cast<Sound::DuckVolumes>(i) );
        }
    }
}

//=============================================================================
// Fader::Stop
//=============================================================================
// Description: Fader is being interrupted, get it out of the update list
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void Fader::Stop()
{
    removeFromUpdateList();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//========================================================================
// Fader::setState
//========================================================================

void Fader::setState( void )
{
    
    if ( GetState( ) != m_State )
    {
        m_State = GetState( );

        if( m_callback != NULL )
        {
            m_callback->OnStateChange( m_State );
        }
    }
}

//========================================================================
// Fader::addToUpdateList
//========================================================================
void Fader::addToUpdateList()
{
    if( !faderInUpdateList() )
    {
        //
        // Order doesn't matter, add it to the head of the list
        //
        m_nextUpdatableFader = s_faderUpdateList;
        s_faderUpdateList = this;
    }
}

//========================================================================
// Fader::removeFromUpdateList
//========================================================================
void Fader::removeFromUpdateList()
{
    Fader* currentFader;
    
    //
    // Search for position in list.  The list is usually one or two faders,
    // I think, so don't bother with double-linked lists.
    //
    if( s_faderUpdateList == this )
    {
        s_faderUpdateList = m_nextUpdatableFader;
    }
    else
    {
        currentFader = s_faderUpdateList;
        while( ( currentFader != NULL )
               && ( currentFader->m_nextUpdatableFader != this ) )
        {
            currentFader = currentFader->m_nextUpdatableFader;
        }

        if( currentFader != NULL )
        {
            currentFader->m_nextUpdatableFader = m_nextUpdatableFader;
        }
    }

    m_nextUpdatableFader = NULL;
}

//=============================================================================
// Fader::faderInUpdateList
//=============================================================================
// Description: Indicate whether this fader is currently in the update list
//
// Parameters:  None
//
// Return:      true if in list, false otherwise 
//
//=============================================================================
bool Fader::faderInUpdateList()
{
    Fader* currFader;

    currFader = s_faderUpdateList;
    while( currFader != NULL )
    {
        if( currFader == this )
        {
            return( true );
        }

        currFader = currFader->m_nextUpdatableFader;
    }

    return( false );
}
