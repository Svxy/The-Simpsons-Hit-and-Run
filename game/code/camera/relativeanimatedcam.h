//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        relativeanimatedcam.h
//
// Description: camers used for animated camera transitions that are relative
//              to a fixed frame in the world
//
// History:     21/01/2003 + Created -- Ian Gipson
//
//=============================================================================

#ifndef RELATIVEANIMATEDCAMERA_H
#define RELATIVEANIMATEDCAMERA_H

//========================================
// Nested Includes
//========================================
#include <camera/animatedcam.h>
#include <camera/supercam.h>
#include <events/eventlistener.h>
#include <input/mappable.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;
class tCameraAnimationController;
class tMultiController;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RelativeAnimatedCam : 
    public AnimatedCam
{
public:
    RelativeAnimatedCam();
    static void       CheckPendingCameraSwitch();
    const char* const GetName() const;
    SuperCam::Type GetType();
    void SetCameraAnimationController( tCameraAnimationController* controller );
    void SetOffsetMatrix( const rmt::Matrix& m );
    void Update( unsigned int milliseconds );

protected:
    virtual void      LetterBoxStart();
    virtual void      LetterBoxStop();

    //Prevent wasteful constructor creation.
    RelativeAnimatedCam( const AnimatedCam& AnimatedCam );
    RelativeAnimatedCam& operator=( const AnimatedCam& AnimatedCam );
    rmt::Matrix mOffsetMatrix;
    tCameraAnimationController* gCameraAnimationController;
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

#endif //ANIMATEDCAMERA_H
