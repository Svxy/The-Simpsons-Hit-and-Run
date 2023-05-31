//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        animobjsoundplayer.cpp
//
// Description: Plays sound for AnimCollisionEntityDSG objects
//
// History:     6/5/2003 + Created -- Esan (post-beta, yay!)
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
#include <sound/movingpositional/animobjsoundplayer.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundallocatedresource.h>

#include <events/eventdata.h>
#include <render/DSG/animcollisionentitydsg.h>

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
// AnimObjSoundPlayer::AnimObjSoundPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
AnimObjSoundPlayer::AnimObjSoundPlayer() :
    m_joint( NULL ),
    m_identity( NULL )
{
}

//=============================================================================
// AnimObjSoundPlayer::~AnimObjSoundPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
AnimObjSoundPlayer::~AnimObjSoundPlayer()
{
    if( m_identity != NULL )
    {
        m_identity->Release();
    }
}

//=============================================================================
// AnimObjSoundPlayer::Activate
//=============================================================================
// Description: Start playing a sound for a particular platform
//
// Parameters:  soundData - position and identify info for platform
//
// Return:      void 
//
//=============================================================================
void AnimObjSoundPlayer::Activate( AnimSoundDSGData* soundData )
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

        m_identity = soundData->soundObject;
        rAssert( m_identity != NULL );

        m_joint = soundData->animJoint;
        rAssert( m_joint != NULL );

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
        rDebugString( "Couldn't play anim DSG platform sound, no matching settings found" );
    }
}

//=============================================================================
// AnimObjSoundPlayer::Deactivate
//=============================================================================
// Description: Stop playing platform sound
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void AnimObjSoundPlayer::Deactivate()
{
    m_player.Stop();

    m_identity = NULL;
}

//=============================================================================
// AnimObjSoundPlayer::ServiceOncePerFrame
//=============================================================================
// Description: Service the sound player
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void AnimObjSoundPlayer::ServiceOncePerFrame()
{
    m_player.ServiceOncePerFrame();
}

//=============================================================================
// AnimObjSoundPlayer::GetPosition
//=============================================================================
// Description: Get the platform's current position
//
// Parameters:  position - filled in with platform position
//
// Return:      void 
//
//=============================================================================
void AnimObjSoundPlayer::GetPosition( radSoundVector& position )
{
    rmt::Vector posn;

    rAssert( m_joint != NULL );
    posn = m_joint->worldMatrix.Row( 3 );
    position.SetElements( posn.x, posn.y, posn.z );
}

//=============================================================================
// AnimObjSoundPlayer::GetVelocity
//=============================================================================
// Description: Comment
//
// Parameters:  ( radSoundVector& velocity )
//
// Return:      void 
//
//=============================================================================
void AnimObjSoundPlayer::GetVelocity( radSoundVector& velocity )
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
