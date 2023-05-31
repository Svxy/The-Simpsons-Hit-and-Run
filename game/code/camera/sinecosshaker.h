//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        sinecosshaker.h
//
// Description: Blahblahblah
//
// History:     02/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SINECOSSHAKER_H
#define SINECOSSHAKER_H

//========================================
// Nested Includes
//========================================

#ifndef WORLD_BUILDER
//This is the non-tool includes
#include <camera/icamerashaker.h>
#else
#include "icamerashaker.h"
#endif

//========================================
// Forward References
//========================================
class tPointCamera;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SineCosShaker : public ICameraShaker
{
public:
    SineCosShaker();
    virtual ~SineCosShaker();

    void Reset();
    void ShakeCamera( rmt::Vector* pos, rmt::Vector* targ, unsigned int milliseconds );
    void SetSpeed( float speed );
    void SetTime( unsigned int milliseconds );
    void SetCamera( tPointCamera* camera );
    void SetCameraRelative( bool cameraRelative );
    void SetDirection( const rmt::Vector& dir );

    void SetLooping( bool loop );
    bool DoneShaking();
    
    const char* const GetName();

    void RegisterDebugInfo();
    void UnregisterDebugInfo();

    void SetShakeData( const ShakeEventData* data );

    void SetAmpYIncrement( float inc );
    void SetAmpMaxScale( float max );
    

private:

    unsigned int mTime;
    unsigned int mCurrentTime;
    float mSpeed;

    //This camera is screen-relative
    tPointCamera* mCamera;
    bool mIsCameraRelative;
    
    float mAmpYIncrement;   //rate of amplitude
    float mAmplitudeY;      //aplitude stored

    float mAmpScale;        //scale the amplitude
    float mAmpScaleMax;     //scale the amplitude MAX

    bool mLooping;

    rmt::Vector mDirection;



    //Prevent wasteful constructor creation.
    SineCosShaker( const SineCosShaker& sinecosshaker );
    SineCosShaker& operator=( const SineCosShaker& sinecosshaker );
};


//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SineCosShaker::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::Reset()
{
    //mTime = 0;
    mCurrentTime = 0;
    mLooping = false;
    mAmpScale = 0.0f;
}

//=============================================================================
// SineCosShaker::SetSpeed
//=============================================================================
// Description: Comment
//
// Parameters:  ( float speed )
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::SetSpeed( float speed )
{
    mSpeed = speed;
}

//=============================================================================
// SineCosShaker::SetTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( float milliseconds )
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::SetTime( unsigned int milliseconds )
{
    mTime = milliseconds;
}

//=============================================================================
// SineCosShaker::SetCameraRelative
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool cameraRelative )
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::SetCameraRelative( bool cameraRelative )
{
    mIsCameraRelative = cameraRelative;
}

//=============================================================================
// SineCosShaker::SetDirection
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& dir )
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::SetDirection( const rmt::Vector& dir )
{
    mDirection = dir;
}

//=============================================================================
// SineCosShaker::SetLooping
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool loop )
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::SetLooping( bool loop )
{
    mLooping = loop;
}

//=============================================================================
// SineCosShaker::DoneShaking
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool SineCosShaker::DoneShaking()
{
    return (mCurrentTime >= mTime);
}

//=============================================================================
// SineCosShaker::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* const SineCosShaker::GetName()
{
    return "SineCos Shaker";
}

//=============================================================================
// SineCosShaker::SetAmpYIncrement
//=============================================================================
// Description: Comment
//
// Parameters:  ( float inc )
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::SetAmpYIncrement( float inc )
{
    mAmpYIncrement = inc;
}

//=============================================================================
// SineCosShaker::SetAmpMaxScale
//=============================================================================
// Description: Comment
//
// Parameters:  ( float max )
//
// Return:      void 
//
//=============================================================================
inline void SineCosShaker::SetAmpMaxScale( float max )
{
    mAmpScaleMax = max;
}

#endif //SINECOSSHAKER_H
