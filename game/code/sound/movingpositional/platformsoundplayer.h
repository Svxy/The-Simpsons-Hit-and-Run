//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        platformsoundplayer.h
//
// Description: Plays sound for moving platforms
//
// History:     5/29/2003 + Created -- Esan (two days to beta, yay!)
//
//=============================================================================

#ifndef PLATFORMSOUNDPLAYER_H
#define PLATFORMSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <sound/positionalsoundplayer.h>
#include <p3d/anim/pose.hpp>

//========================================
// Forward References
//========================================
struct AnimSoundData;
struct radSoundVector;

namespace ActionButton
{
    class AnimSwitch;
};

//=============================================================================
//
// Synopsis:    PlatformSoundPlayer
//
//=============================================================================

class PlatformSoundPlayer : public PositionCarrier
{
    public:
        PlatformSoundPlayer();
        virtual ~PlatformSoundPlayer();

        bool IsActive() { return( m_identity != NULL ); }
        bool UsesObject( ActionButton::AnimSwitch* soundObject ) { return( soundObject == m_identity ); }
        void Activate( AnimSoundData* soundData );
        void Deactivate();

        void ServiceOncePerFrame();

        //
        // PositionCarrier functions
        //
        void GetPosition( radSoundVector& position );
        void GetVelocity( radSoundVector& velocity );

    private:
        //Prevent wasteful constructor creation.
        PlatformSoundPlayer( const PlatformSoundPlayer& platformsoundplayer );
        PlatformSoundPlayer& operator=( const PlatformSoundPlayer& platformsoundplayer );

        tPose::Joint* m_joint;
        ActionButton::AnimSwitch* m_identity;

        PositionalSoundPlayer m_player;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //PLATFORMSOUNDPLAYER_H
