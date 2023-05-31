//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     22/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef CAMERAPLAYER_H
#define CAMERAPLAYER_H

//========================================
// Nested Includes
//========================================

#include <presentation/simpleanimationplayer.h>

//========================================
// Forward References
//========================================

class tAnimation;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class CameraPlayer : public SimpleAnimationPlayer
{
    public:
        CameraPlayer();
        virtual ~CameraPlayer();

        virtual void ClearData();
        
    protected:
        virtual void DoLoaded();

    private:

        //Prevent wasteful constructor creation.
        CameraPlayer( const CameraPlayer& cameraPlayer );
        CameraPlayer& operator=( const CameraPlayer& cameraPlayer );

        tAnimation* mpAnimation;
};


#endif //CAMERAPLAYER_H

