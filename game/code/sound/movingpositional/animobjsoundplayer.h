//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        animobjsoundplayer.h
//
// Description: Plays sound for moving anim objs
//
// History:     6/5/2003 + Created -- Esan (post-beta, yay!)
//
//=============================================================================

#ifndef ANIMOBJSOUNDPLAYER_H
#define ANIMOBJSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <sound/positionalsoundplayer.h>
#include <p3d/anim/pose.hpp>

//========================================
// Forward References
//========================================
struct AnimSoundDSGData;
struct radSoundVector;
class AnimCollisionEntityDSG;

//=============================================================================
//
// Synopsis:    AnimObjSoundPlayer
//
//=============================================================================

class AnimObjSoundPlayer : public PositionCarrier
{
    public:
        AnimObjSoundPlayer();
        virtual ~AnimObjSoundPlayer();

        bool IsActive() { return( m_identity != NULL ); }
        bool UsesObject( AnimCollisionEntityDSG* soundObject ) { return( soundObject == m_identity ); }
        void Activate( AnimSoundDSGData* soundData );
        void Deactivate();

        void ServiceOncePerFrame();

        //
        // PositionCarrier functions
        //
        void GetPosition( radSoundVector& position );
        void GetVelocity( radSoundVector& velocity );

    private:
        //Prevent wasteful constructor creation.
        AnimObjSoundPlayer( const AnimObjSoundPlayer& animobjsoundplayer );
        AnimObjSoundPlayer& operator=( const AnimObjSoundPlayer& animobjsoundplayer );

        tPose::Joint* m_joint;
        AnimCollisionEntityDSG* m_identity;

        PositionalSoundPlayer m_player;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //ANIMOBJSOUNDPLAYER_H
