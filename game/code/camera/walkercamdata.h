//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        walkercamdata.h
//
// Description: Blahblahblah
//
// History:     25/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef WALKERCAMDATA_H
#define WALKERCAMDATA_H

//========================================
// Nested Includes
//========================================
//These are included in the precompiled headers on XBOX and GC
#include <radmath/radmath.hpp>
#include <camera/supercamconstants.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WalkerCamData
{
public:
    WalkerCamData();
    virtual ~WalkerCamData() {};

    void SetLag( float lag );
    float GetLag() const;

    void SetMinFOV( float min );
    float GetMinFOV() const;

    void SetMaxFOV( float max );
    float GetMaxFOV() const;

    void SetFOVLag( float lag );
    float GetFOVLag() const;

    void SetMinMagnitude( float mag );
    float GetMinMagnitude() const;

    void SetMaxMagnitude( float mag );
    float GetMaxMagnitude() const;

    void SetElevation( float elev );
    float GetElevation() const;

    void SetRotation( float rot );
    float GetRotation() const;

    void SetMagnitude( float mag );
    float GetMagnitude() const;

    void SetRotationIncrement( float inc );
    float GetRotationIncrement() const;

    //This is the offset from the target
    void GetTargetOffset( rmt::Vector* offset ) const;
    void SetTargetOffset( rmt::Vector offset );

    float GetTargetLag() const;
    void  SetTargetLag( float lag );

    float GetTargetJumpLag() const;
    void  SetTargetJumpLag( float lag );

    unsigned int    GetLandingTransitionTime() const;
    void            SetLandingTransitionTime( unsigned int time );

    float GetUpAngle() const;
    void  SetUpAngle( float angle );
    
    float GetCollisionLag() const;
    void SetCollisionLag( float lag );

    float mLag;

    float mMinFOV;
    float mMaxFOV;
    float mFOVLag;

    float mMinMagnitude;
    float mMaxMagnitude;

    float mElevation;
    float mRotation;
    float mMagnitude;

    float mRotationIncrement;

    //This is the target offset.
    rmt::Vector mTargetOffset;

    float mTargetLag;
    float mJumpLag;

    float mUpAngle;

    unsigned int mLandingTransitionTime;

    float mCollisionLag;

private:
    //Prevent wasteful constructor creation.
    WalkerCamData( const WalkerCamData& walkercamdata );
    WalkerCamData& operator=( const WalkerCamData& walkercamdata );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// WalkerCamData::WalkerCamData
//=============================================================================
// Description: Constructor
//
// Parameters:  ()
//
// Return:      nothing
//
//=============================================================================
inline WalkerCamData::WalkerCamData() :
    mLag( 0.08f ),
    mMinFOV( SUPERCAM_DEFAULT_MIN_FOV ), //Fudge...
    mMaxFOV( SUPERCAM_DEFAULT_MAX_FOV ), //Fudge...
    mFOVLag( SUPERCAM_DEFAULT_FOV_LAG ),
    mMinMagnitude( 4.26f ),
    mMaxMagnitude( 9.0f ),
    mElevation( 1.36f ),
    mRotation( rmt::PI_BY2 ),
    mMagnitude( ((mMaxMagnitude - mMinMagnitude) / 2.0f) + mMinMagnitude ),
    mRotationIncrement( 0.4f ),
    mTargetLag( 0.044f ),
    mJumpLag( 0.3f ),
    mUpAngle( 2.44f ),
    mLandingTransitionTime( 2000 ),
    mCollisionLag( 0.15f )
{
    mTargetOffset.Set( 0.0f, 1.0f, 0.0f );
}

//=============================================================================
// WalkerCamData::SetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetLag( float lag )
{
    mLag = lag;
}

//=============================================================================
// WalkerCamData::GetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetLag() const
{
    return mLag;
}

//=============================================================================
// WalkerCamData::SetMinFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float min )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetMinFOV( float min )
{
    mMinFOV = min;
}

//=============================================================================
// WalkerCamData::GetMinFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetMinFOV() const
{
    return mMinFOV;
}

//=============================================================================
// WalkerCamData::SetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float max )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetMaxFOV( float max )
{
    mMaxFOV = max;
}

//=============================================================================
// WalkerCamData::GetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetMaxFOV() const
{
    return mMaxFOV;
}

//=============================================================================
// WalkerCamData::SetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetFOVLag( float lag )
{
    mFOVLag = lag;
}

//=============================================================================
// WalkerCamData::GetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetFOVLag() const
{
    return mFOVLag;
}

//=============================================================================
// WalkerCamData::SetMinMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ( float mag )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetMinMagnitude( float mag )
{
    mMinMagnitude = mag;
}

//=============================================================================
// WalkerCamData::GetMinMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetMinMagnitude() const
{
    return mMinMagnitude;
}

//=============================================================================
// WalkerCamData::SetMaxMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ( float mag )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetMaxMagnitude( float mag )
{
    mMaxMagnitude = mag;
}

//=============================================================================
// WalkerCamData::GetMaxMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetMaxMagnitude() const
{
    return mMaxMagnitude;
}

//=============================================================================
// WalkerCamData::SetElevation
//=============================================================================
// Description: Comment
//
// Parameters:  ( float elev )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetElevation( float elev )
{
    mElevation = elev;
}

//=============================================================================
// WalkerCamData::GetElevation
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetElevation() const
{
    return mElevation;
}

//=============================================================================
// WalkerCamData::SetRotation
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rot )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetRotation( float rot )
{
    mRotation = rot;
}

//=============================================================================
// WalkerCamData::GetRotation
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetRotation() const
{
    return mRotation;
}

//=============================================================================
// WalkerCamData::SetMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ( float mag )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetMagnitude( float mag )
{
    mMagnitude = mag;
}

//=============================================================================
// WalkerCamData::GetMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetMagnitude() const
{
    return mMagnitude;
}

//=============================================================================
// WalkerCamData::SetRotationIncrement
//=============================================================================
// Description: Comment
//
// Parameters:  ( float inc )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetRotationIncrement( float inc )
{
    mRotationIncrement = inc;
}

//=============================================================================
// WalkerCamData::GetRotationIncrement
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetRotationIncrement() const
{
    return mRotationIncrement;
}


//=============================================================================
// WalkerCamData::GetTargetOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* offset )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::GetTargetOffset( rmt::Vector* offset ) const
{
    *offset = mTargetOffset;   
}

//=============================================================================
// WalkerCamData::SetTargetOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector offset )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetTargetOffset( rmt::Vector offset )
{
    mTargetOffset = offset;
}

//=============================================================================
// WalkerCamData::GetTargetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetTargetLag() const
{
    return mTargetLag;
}

//=============================================================================
// WalkerCamData::SetTargetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetTargetLag( float lag )
{
    mTargetLag = lag;
}

//=============================================================================
// WalkerCamData::GetTargetJumpLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetTargetJumpLag() const
{
    return mJumpLag;
}

//=============================================================================
// WalkerCamData::SetTargetJumpLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void  
//
//=============================================================================
inline void  WalkerCamData::SetTargetJumpLag( float lag )
{
    mJumpLag = lag;
}

//=============================================================================
// WalkerCamData::GetLandingTransitionTime
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned int 
//
//=============================================================================
inline unsigned int WalkerCamData::GetLandingTransitionTime() const
{
    return mLandingTransitionTime;
}

//=============================================================================
// WalkerCamData::SetLandingTransitionTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int time )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetLandingTransitionTime( unsigned int time )
{
    mLandingTransitionTime = time;
}

//=============================================================================
// WalkerCamData::GetUpAngle
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetUpAngle() const
{
    return mUpAngle;
}

//=============================================================================
// WalkerCamData::SetUpAngle
//=============================================================================
// Description: Comment
//
// Parameters:  ( float angle )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetUpAngle( float angle )
{
    mUpAngle = angle;
}

//=============================================================================
// WalkerCamData::GetCollisionLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float WalkerCamData::GetCollisionLag() const
{
    return mCollisionLag;
}

//=============================================================================
// WalkerCamData::SetCollisionLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCamData::SetCollisionLag( float lag )
{
    mCollisionLag = lag;
}


#endif //WALKERCAMDATA_H
