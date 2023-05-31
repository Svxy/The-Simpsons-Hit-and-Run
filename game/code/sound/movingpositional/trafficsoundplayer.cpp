//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficsoundplayer.cpp
//
// Description: Administers the playing of sound for a traffic vehicle
//
// History:     1/4/2003 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/movingpositional/trafficsoundplayer.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundfx/positionalsoundsettings.h>
#include <sound/avatar/carsoundparameters.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/trafficlocomotion.h>
#include <worldsim/traffic/trafficmanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

IRadTimerList* TrafficSoundPlayer::s_timerList = NULL;

static const unsigned int s_minHonkShortMsecs = 250;
static const unsigned int s_maxHonkShortMsecs = 500;
static const unsigned int s_minHonkLongMsecs = 500;
static const unsigned int s_maxHonkLongMsecs = 1000;
static const unsigned int s_minHonkDelay = 250;
static const unsigned int s_maxHonkDelay = 500;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// TrafficSoundPlayer::TrafficSoundPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
TrafficSoundPlayer::TrafficSoundPlayer( ) :
    m_hornTimer( NULL ),
    m_vehicleParameters( NULL ),
    m_honkCount( 0 ),
    m_pitchMultiplier( 1.0f )
{
}

//=============================================================================
// TrafficSoundPlayer::~TrafficSoundPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
TrafficSoundPlayer::~TrafficSoundPlayer()
{
    if( m_hornTimer != NULL )
    {
        m_hornTimer->UnregisterCallback( this );
        m_hornTimer->Release();
    }
}

//=============================================================================
// TrafficSoundPlayer::InitializeClass
//=============================================================================
// Description: Prep the timer list for tracking horn times
//
// Parameters:  numVehicles - number of cars, and therefore timers we need
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::InitializeClass( unsigned int numVehicles )
{
    if( s_timerList == NULL )
    {
        ::radTimeCreateList( &s_timerList, numVehicles, GMA_AUDIO_PERSISTENT );
    }
}

//=============================================================================
// TrafficSoundPlayer::Activate
//=============================================================================
// Description: Before we start playing any sound, set a random pitch adjustment
//              to give this thing a little variety
//
// Parameters:  soundSettings - positional stuff for traffic
//              resourceName - name of engine sound
//              theCar - pointer to traffic vehicle object
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::Activate( positionalSoundSettings* soundSettings,
                                   const char* resourceName,
                                   Vehicle* theCar )
{
    unsigned int randomNumber;

    //
    // Pick a random pitch multiplier from 0.8 to 1.2.  Arbitrary numbers.
    //
    randomNumber = rand() % 401;
    m_pitchMultiplier = 0.8f + ( static_cast<float>(randomNumber) / 1000.0f );

    VehiclePositionalSoundPlayer::Activate( soundSettings, resourceName, theCar );

    m_player.SetPitch( 0.5f );
}

//=============================================================================
// TrafficSoundPlayer::Deactivate
//=============================================================================
// Description: Stop playing sound.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::Deactivate()
{
    VehiclePositionalSoundPlayer::Deactivate();

    if( m_hornTimer != NULL )
    {
        m_hornTimer->UnregisterCallback( this );
        m_hornTimer->Release();
        m_hornTimer = NULL;
    }

    m_hornPlayer.Stop();

    m_overlayPlayer.Stop();
}

//=============================================================================
// TrafficSoundPlayer::ServiceOncePerFrame
//=============================================================================
// Description: Adjust the pitch for this vehicle
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::ServiceOncePerFrame()
{
    float pitch;

    VehiclePositionalSoundPlayer::ServiceOncePerFrame();

    //
    // Adjust pitch for vehicle speed if desired.  I'll probably need to
    // expose this for designer tuning later.
    //
    if( IsActive() && m_tiePitchToVelocity )
    {
        pitch = 0.5f + ( 0.5f * ( m_vehicle->mTrafficLocomotion->mActualSpeed / TrafficManager::GetInstance()->GetDesiredTrafficSpeed() ) );
        pitch *= m_pitchMultiplier;

        //
        // Arbitrary cap, just in case.  Being paranoid.
        //
        if( pitch > 1.5f )
        {
            pitch = 1.5f;
        }

        m_player.SetPitch( pitch );
    }

    m_hornPlayer.ServiceOncePerFrame();
    m_overlayPlayer.ServiceOncePerFrame();
}

//=============================================================================
// TrafficSoundPlayer::ServiceTimerList
//=============================================================================
// Description: Service the timer list.  Static function, since we only
//              need to service this static member once per frame, not once
//              per object per frame.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::ServiceTimerList()
{
    if( s_timerList != NULL )
    {
        s_timerList->Service();
    }
}

//=============================================================================
// TrafficSoundPlayer::OnTimerDone
//=============================================================================
// Description: Stop the traffic horn associated with this timer
//
// Parameters:  elapsedTime - unused
//              pUserData - unused
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::OnTimerDone( unsigned int elapsedTime, void * pUserData )
{
    unsigned int timeout;
    rmt::Vector position;
    positionalSoundSettings* settings;
    IRadNameSpace* nameSpace;

    if( m_hornPlayer.IsInUse() )
    {
        m_hornPlayer.Stop();

        if( m_honkCount == 0 )
        {
            //
            // Last honk done
            //
            m_hornTimer->Release();
            m_hornTimer = NULL;
        }
        else
        {
            //
            // Random amount of silence before next honk
            //
            timeout = s_minHonkDelay + ( rand() % ( s_maxHonkDelay - s_minHonkDelay ) );
            m_hornTimer->SetTimeout( timeout );
            m_hornTimer->Start();
        }
    }
    else
    {
        //
        // Silence done, start another honk
        //
        rAssert( m_honkCount > 0 );

        //
        // Find the settings for this positional sound first
        //
        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );
        settings = reinterpret_cast<positionalSoundSettings*>( nameSpace->GetInstance( "traffic_horn" ) );
        rAssert( settings != NULL );

        m_vehicle->GetPosition( &position );
        m_hornPlayer.SetPosition( position.x, position.y, position.z );
        m_hornPlayer.SetPositionCarrier( *this );
        m_hornPlayer.SetParameters( settings );
        m_hornPlayer.PlaySound( "horn" );

        --m_honkCount;

        //
        // Reset the timer
        //
        if( m_honkCount > 0 )
        {
            timeout = s_minHonkShortMsecs + ( rand() % ( s_maxHonkShortMsecs - s_minHonkShortMsecs ) );
        }
        else
        {
            timeout = s_minHonkLongMsecs + ( rand() % ( s_maxHonkLongMsecs - s_minHonkLongMsecs ) );
        }

        m_hornTimer->SetTimeout( timeout );
        m_hornTimer->Start();
    }
}

//=============================================================================
// TrafficSoundPlayer::HonkHorn
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::HonkHorn()
{
    unsigned int hornTime;
    rmt::Vector position;
    positionalSoundSettings* settings;
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    float diceRoll;
    float probability;

    rAssert( m_vehicle != NULL );

    if( m_hornTimer != NULL )
    {
        //
        // Already honking
        //
        return;
    }

    //
    // Find the settings for this positional sound first
    //
    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    nameSpaceObj = nameSpace->GetInstance( "traffic_horn" );
    if( nameSpaceObj != NULL )
    {
        settings = reinterpret_cast<positionalSoundSettings*>( nameSpaceObj );

        probability = settings->GetPlaybackProbability();
        if( probability < 1.0f )
        {
            //
            // Random play
            //
            diceRoll = (static_cast<float>( rand() % 100 )) / 100.0f;
            if( diceRoll >= probability )
            {
                return;
            }
        }

        //
        // Pick a random number of honks from 1 to 3.  Pick a random time for
        // the honk, giving the last one a better chance of being longer.
        //
        m_honkCount = rand() % 3;
        if( m_honkCount > 0 )
        {
            hornTime = s_minHonkShortMsecs + ( rand() % ( s_maxHonkShortMsecs - s_minHonkShortMsecs ) );
        }
        else
        {
            hornTime = s_minHonkLongMsecs + ( rand() % ( s_maxHonkLongMsecs - s_minHonkLongMsecs ) );
        }

        s_timerList->CreateTimer( &m_hornTimer, hornTime, this, NULL, true, 
                                  IRadTimer::ResetModeOneShot );

        //
        // Start honking
        //
        m_vehicle->GetPosition( &position );
        m_hornPlayer.SetPosition( position.x, position.y, position.z );
        m_hornPlayer.SetPositionCarrier( *this );
        m_hornPlayer.SetParameters( settings );
        m_hornPlayer.PlaySound( "horn" );
    }
    else
    {
        rDebugString( "Couldn't find settings for traffic horn\n" );
    }
}

//=============================================================================
// TrafficSoundPlayer::AddOverlayClip
//=============================================================================
// Description: Play a positional clip along with the engine
//
// Parameters:  parameters - car sound description for traffic vehicle
//              posnSettingsName - name of object with positional settings
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::AddOverlayClip( carSoundParameters* parameters, const char* posnSettingsName )
{
    const char* clipName;
    positionalSoundSettings* settings;
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    rmt::Vector position;

    rAssert( parameters != NULL );

    m_vehicleParameters = parameters;
    clipName = parameters->GetOverlayClipName();
    if( clipName != NULL )
    {
        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );
        nameSpaceObj = nameSpace->GetInstance( posnSettingsName );
        if( nameSpaceObj != NULL )
        {
            settings = reinterpret_cast<positionalSoundSettings*>( nameSpaceObj );

            m_vehicle->GetPosition( &position );
            m_overlayPlayer.SetPosition( position.x, position.y, position.z );
            m_overlayPlayer.SetPositionCarrier( *this );
            m_overlayPlayer.SetParameters( settings );
            m_overlayPlayer.PlaySound( clipName );
        }
        else
        {
            rTuneAssertMsg( false, "Huh? Positional settings for overlay clip disappeared" );
        }
    }
}

//=============================================================================
// TrafficSoundPlayer::ToggleOverlayClip
//=============================================================================
// Description: Comment
//
// Parameters:  ( carSoundParameters* parameters, const char* posnSettingsName )
//
// Return:      void 
//
//=============================================================================
void TrafficSoundPlayer::ToggleOverlayClip( carSoundParameters* parameters, const char* posnSettingsName )
{
    if( m_overlayPlayer.IsInUse() )
    {
        m_overlayPlayer.Stop();
    }
    else
    {
        AddOverlayClip( parameters, posnSettingsName );
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
