//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        positionalsoundplayer.cpp
//
// Description: Implement PositionalSoundPlayer
//
// History:     12/18/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <sound/soundrenderer/soundplayer.h>
//========================================
// Project Includes
//========================================
#include <sound/positionalsoundplayer.h>

#include <sound/soundfx/positionalsoundsettings.h>

#include <memory/srrmemory.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

static const float POSITIONAL_PAUSE_BUFFER_DIST = 25.0f;

//=============================================================================
// PositionCarrier::PositionCarrier
//=============================================================================
// Description: Constructor.
//
// Parameters:  None
//
// Return:      N/A
//
//=============================================================================
PositionCarrier::PositionCarrier()
{
}

//=============================================================================
// PositionCarrier::~PositionCarrier
//=============================================================================
// Description: Destructor
//
// Parameters:  None
//
// Return:      N/A
//
//=============================================================================
PositionCarrier::~PositionCarrier()
{
}

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// PositionalSoundPlayer::PositionalSoundPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
PositionalSoundPlayer::PositionalSoundPlayer( ) :
    m_positionCarrier( NULL ),
    m_positionalSettings( NULL ),
    m_minDist( 3.0f ),
    m_maxDist( 100.f ),
    m_position( 0.0f, 0.0f, 0.0f ),
    m_outOfRange( false )
{
    m_Type = Type_Positional;
}

//=============================================================================
// PositionalSoundPlayer::~PositionalSoundPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
PositionalSoundPlayer::~PositionalSoundPlayer()
{
    delete m_positionCarrier;

    if( m_positionalSettings != NULL )
    {
        m_positionalSettings->Release();
        m_positionalSettings = NULL;
    }
}

bool PositionalSoundPlayer::PlayResource( IDaSoundResource* resource,
                                          SimpsonsSoundPlayerCallback* callback /* = NULL  */)
{
    bool canPlay;

    canPlay = QueueSound( resource, callback );
    if( canPlay )
    {
        //
        // m_position should have been set in an earlier call to SetPosition.
        // If not, then it should be a moving sound and should therefore
        // get set later by the service function.
        //
        PlayQueuedSound( m_position, callback );
    }

    return( canPlay );
}

//=============================================================================
// PositionalSoundPlayer::PlayQueuedSound
//=============================================================================
// Description: Sets the position for the sound, then calls PlayQueuedSound
//              in the SimpsonsSoundPlayer base class
//
// Parameters:  position - position that sound is to be played at
//              callback - object to inform when playback complete
//
// Return:      void 
//
//=============================================================================
void PositionalSoundPlayer::PlayQueuedSound( radSoundVector& position, 
                                             SimpsonsSoundPlayerCallback* callback )
{
    //
    // Before playing the sound, create a positional group and set the position.
    // The group is reference counted, so the player will release it when
    // the sound resource is released.
    //
    
    if ( m_playa )
    {
        radSoundVector velocity( 0.0f, 0.0f, 0.0f );
        
        m_playa->SetPositionAndVelocity( & position, & velocity );
        m_playa->SetMinMaxDistance( m_minDist, m_maxDist );
    }

    SimpsonsSoundPlayer::PlayQueuedSound( callback );
}

void PositionalSoundPlayer::ServiceOncePerFrame()
{
    radSoundVector position;
    radSoundVector velocity;

    radSoundVector distanceVector;
    radSoundVector soundPosition;
    radSoundVector listenerPosition;
    float positionalMax;
    float distance;

    //
    // Update the positional group with new vehicle position
    //
    if( ( m_positionCarrier != NULL ) )
    {
        m_positionCarrier->GetPosition( position );
        m_positionCarrier->GetVelocity( velocity );

        if ( m_playa )
        {
            m_playa->SetPositionAndVelocity( & position, & velocity );
        }
    }

    //
    // Do unpause/pause on players when they get in and out of range
    //
    if( ( m_playa != NULL ) && ( m_positionalSettings != NULL ) )
    {
        ::radSoundHalListenerGet()->GetPosition( &listenerPosition );
        m_playa->GetPositionalGroup()->GetPosition( &soundPosition );
        distanceVector = listenerPosition - soundPosition;
        distance = distanceVector.GetLength();

        positionalMax = m_positionalSettings->GetMaxDistance();

        if( m_outOfRange && ( distance <= ( positionalMax + POSITIONAL_PAUSE_BUFFER_DIST ) ) )
        {
            if( IsPaused() )
            {
                Continue();
            }
            m_outOfRange = false;
        }
        else if( ( !m_outOfRange ) && ( distance > ( positionalMax + POSITIONAL_PAUSE_BUFFER_DIST ) ) )
        {
            Pause();
            m_outOfRange = true;
        }
    }
}

void PositionalSoundPlayer::SetPositionCarrier( PositionCarrier& movingSound )
{
    m_positionCarrier = &movingSound;
}

//=============================================================================
// PositionalSoundPlayer::SetParameters
//=============================================================================
// Description: Set the min/max and whatever else from the tunable
//              sound settings object
//
// Parameters:  settings - object containing sound settings
//
// Return:      void 
//
//=============================================================================
void PositionalSoundPlayer::SetParameters( positionalSoundSettings* settings )
{
    m_minDist = settings->GetMinDistance();
    m_maxDist = settings->GetMaxDistance();

    m_positionalSettings = settings;
    m_positionalSettings->AddRef();

    if( m_playa != NULL )
    {
        m_playa->SetMinMaxDistance( m_minDist, m_maxDist );
    }
}

//=============================================================================
// PositionalSoundPlayer::SetPosition
//=============================================================================
// Description: Set the position for the sound
//
// Parameters:  x,y,z - position
//
// Return:      void 
//
//=============================================================================
void PositionalSoundPlayer::SetPosition( float x, float y, float z )
{
    m_position.SetElements( x, y, z );
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

void PositionalSoundPlayer::dumpSoundPlayer()
{
    //
    // Get rid of the positional group
    //

    if( m_positionalSettings != NULL )
    {
        m_positionalSettings->Release();
        m_positionalSettings = NULL;
    }

    m_positionCarrier = NULL;

    //
    // Let the parent clean up now
    //
    SimpsonsSoundPlayer::dumpSoundPlayer();
}