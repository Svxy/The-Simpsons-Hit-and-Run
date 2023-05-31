//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        platformsoundplayer.cpp
//
// Description: Plays sound for moving platforms
//
// History:     5/29/2003 + Created -- Esan (two days to beta, yay!)
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>
#include <p3d/anim/pose.hpp>

//========================================
// Project Includes
//========================================
#include <sound/movingpositional/platformsoundplayer.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundallocatedresource.h>

#include <events/eventdata.h>
#include <ai/actionbuttonhandler.h>

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
// PlatformSoundPlayer::PlatformSoundPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
PlatformSoundPlayer::PlatformSoundPlayer() :
    m_joint( NULL ),
    m_identity( NULL )
{
}

//=============================================================================
// PlatformSoundPlayer::~PlatformSoundPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
PlatformSoundPlayer::~PlatformSoundPlayer()
{
    if( m_identity != NULL )
    {
        //m_identity->Release();
    }
}

//=============================================================================
// PlatformSoundPlayer::Activate
//=============================================================================
// Description: Start playing a sound for a particular platform
//
// Parameters:  soundData - position and identify info for platform
//
// Return:      void 
//
//=============================================================================
void PlatformSoundPlayer::Activate( AnimSoundData* soundData )
{
    rmt::Vector posn;
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    positionalSoundSettings* parameters;

    //
    // Get the positionalSoundSettings object for the platform sound
    //
    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    nameSpaceObj = nameSpace->GetInstance( ::radMakeKey32( soundData->positionalSettingName ) );
    if( nameSpaceObj != NULL )
    {
        parameters = reinterpret_cast<positionalSoundSettings*>( nameSpaceObj );

        m_joint = soundData->animJoint;
        rAssert( m_joint != NULL );

        m_identity = soundData->soundObject;
        rAssert( m_identity != NULL );
        //m_identity->AddRef();

        m_player.SetPositionCarrier( *this );
        m_player.SetParameters( parameters );

        //
        // Get world position of the platform through this joint
        //
        posn = m_joint->worldMatrix.Row( 3 );
        m_player.SetPosition( posn.x, posn.y, posn.z );

        //
        // Don't buffer, to save IOP
        //

        m_player.PlaySound( soundData->soundName, NULL );
    }
    else
    {
        rDebugString( "Couldn't play platform sound, no matching settings found" );
    }
}

//=============================================================================
// PlatformSoundPlayer::Deactivate
//=============================================================================
// Description: Stop playing platform sound
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void PlatformSoundPlayer::Deactivate()
{
    m_player.Stop();

    if( m_identity != NULL )
    {
        //m_identity->Release();
        m_identity = NULL;
    }

    m_joint = NULL;
}

//=============================================================================
// PlatformSoundPlayer::ServiceOncePerFrame
//=============================================================================
// Description: Service the sound player
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void PlatformSoundPlayer::ServiceOncePerFrame()
{
    m_player.ServiceOncePerFrame();
}

//=============================================================================
// PlatformSoundPlayer::GetPosition
//=============================================================================
// Description: Get the platform's current position
//
// Parameters:  position - filled in with platform position
//
// Return:      void 
//
//=============================================================================
void PlatformSoundPlayer::GetPosition( radSoundVector& position )
{
    rmt::Vector posn;

    rAssert( m_joint != NULL );
    posn = m_joint->worldMatrix.Row( 3 );
    position.SetElements( posn.x, posn.y, posn.z );
}

//=============================================================================
// PlatformSoundPlayer::GetVelocity
//=============================================================================
// Description: Comment
//
// Parameters:  ( radSoundVector& velocity )
//
// Return:      void 
//
//=============================================================================
void PlatformSoundPlayer::GetVelocity( radSoundVector& velocity )
{
    //
    // Doppler would be a big waste on those platforms anyway
    //
    velocity.SetElements( 0.0f, 0.0f, 0.0f );
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
