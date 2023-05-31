//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        icamerashaker.h
//
// Description: Blahblahblah
//
// History:     02/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ICAMERASHAKER_H
#define ICAMERASHAKER_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================
class tPointCamera;

struct ShakeEventData;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ICameraShaker
{
public:
    ICameraShaker() {};
    virtual ~ICameraShaker() {};

    virtual void Reset() = 0;
    virtual void ShakeCamera( rmt::Vector* pos, rmt::Vector* targ, unsigned int milliseconds ) = 0;
    virtual void SetSpeed( float speed ) = 0; //This should be between 0 and 1.
    virtual void SetTime( unsigned int milliseconds ) = 0; //How long does the shake go?
    virtual void SetCamera( tPointCamera* camera ) = 0; //What is the camera for screen-relative shake
    virtual void SetCameraRelative( bool cameraRelative ) = 0;
    virtual void SetDirection( const rmt::Vector& dir ) = 0;
    virtual void SetLooping( bool loop ) = 0;
    virtual bool DoneShaking() = 0;

    virtual const char* const GetName() = 0;

    virtual void RegisterDebugInfo() = 0;
    virtual void UnregisterDebugInfo() = 0;

    virtual void SetShakeData( const ShakeEventData* data ) = 0;
private:

    //Prevent wasteful constructor creation.
    ICameraShaker( const ICameraShaker& icamerashaker );
    ICameraShaker& operator=( const ICameraShaker& icamerashaker );
};


#endif //ICAMERASHAKER_H
