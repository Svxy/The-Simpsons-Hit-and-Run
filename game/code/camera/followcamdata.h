//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        followcamdata.h
//
// Description: Blahblahblah
//
// History:     22/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FOLLOWCAMDATA_H
#define FOLLOWCAMDATA_H

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

class FollowCamData
{
public:
    FollowCamData();
    virtual ~FollowCamData() {};

    //These are the spherical coordinates for the camera position
    float GetRotationXZ() const;
    void  SetRotationXZ( float rotXZ );

    float GetRotationY() const;
    void  SetRotationY( float rotY );

    float GetMagnitude() const;
    void  SetMagnitude( float magnitude );

    //Cartesian version of the above
    void GetRod( rmt::Vector* rod );

    //This is the offset from the target
    void GetTargetOffset( rmt::Vector* offset ) const;
    void SetTargetOffset( rmt::Vector offset );

    //These set the interpolation speed (lag) of the positions.
    float   GetCameraLagXZ() const;
    void    SetCameraLagXZ( float lag );

    float   GetCameraLagY() const;
    void    SetCameraLagY( float lag );

    float   GetTargetLagXZ() const;
    void    SetTargetLagXZ( float lag );

    float   GetTargetLagY() const;
    void    SetTargetLagY( float lag );

    float   GetMagnitudeLag() const;
    void    SetMagnitudeLag( float lag );

    float   GetCollisionLag() const;
    void    SetCollisionLag( float lag );

    unsigned int    GetUnstableDelay() const;
    void            SetUnstableDelay( unsigned int delay );

    unsigned int    GetQuickTurnDelay() const;
    void            SetQuickTurnDelay( unsigned int delay );

    float GetQuickTurnModifier() const;
    void  SetQuickTurnModifier( float modifier );

    float GetAspect() const;
    void SetAspect( float aspect );

    float GetFOV() const;
    void SetFOV( float fov );

    void SetDirty();

    //This is the default camera position.
    float mRotationXZ;
    float mRotationY;
    float mMagnitude;

    //This is the target offset.
    rmt::Vector mTargetOffset;

    //These are the rotation (XZ) and height (Y) damping values for camera 
    //movement
    float       mCameraLagXZ;
    float       mCameraLagY;

    //Target lag could be broken into X, and Y in a 2D projection.
    //or as before.
    float       mTargetLagXZ;
    float       mTargetLagY;

    float       mMagnitudeLag;

    float       mCollisionLag;

    //This is how long the camera will stay unstable without additional 
    //unstable stimulus
    unsigned int mUnstableDelay;

    //This is how long the camera will stay in a quick turn state without\
    //additional stimulus
    unsigned int mQuickTurnDelay;
    float        mQuickTurnModifier;  
    
    float mAspect;
    float mFOV;

private:
    //This is the rod built by the stuff above.  (cartesian version)
    rmt::Vector mRod;

    bool mIsDirty : 1;

    //Prevent wasteful constructor creation.
    FollowCamData( const FollowCamData& followcamdata );
    FollowCamData& operator=( const FollowCamData& followcamdata );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

inline FollowCamData::FollowCamData() :
    mRotationXZ( 4.71238f ),
    mRotationY( 1.2568370f ),
    mMagnitude( 7.8559999f ),
    //mCameraLagXZ( 0.044f ),
    mCameraLagXZ( 0.044f ),
    mCameraLagY( 0.028f ),
    //mTargetLagXZ( 1.0f ),
    mTargetLagXZ( 0.1f ),
    mTargetLagY( 0.1f ),
    mMagnitudeLag( 0.05f ),
    mCollisionLag( 0.2f ),
    mUnstableDelay( 200 ),
    mQuickTurnDelay( 0 ),
    mQuickTurnModifier( 0.0f ),
    mAspect( SUPERCAM_ASPECT ),//4.0f / 3.0f ),
#ifdef RAD_XBOX
    mFOV( 1.45f ),
#else
    mFOV( 1.363451f ),
#endif
    mIsDirty( true )
{
    mTargetOffset.Set( 0.0f, 0.5f, 1.2f );
}

//=============================================================================
// FollowCamData::GetRotationXZ
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
// 
//=============================================================================
inline float FollowCamData::GetRotationXZ() const
{
    return mRotationXZ;
}

//=============================================================================
// FollowCamData::SetRotationXZ
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rotXZ )
//
// Return:      void  
//
//=============================================================================
inline void FollowCamData::SetRotationXZ( float rotXZ )
{
    mRotationXZ = rotXZ;
    mIsDirty = true;
}

//=============================================================================
// FollowCamData::GetRotationY
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetRotationY() const
{
    return mRotationY;
}

//=============================================================================
// FollowCamData::SetRotationY
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rotY )
//
// Return:      void  
//
//=============================================================================
inline void  FollowCamData::SetRotationY( float rotY )
{
    mRotationY = rotY;
    mIsDirty = true;
}

//=============================================================================
// FollowCamData::GetMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetMagnitude() const
{
    return mMagnitude;
}

//=============================================================================
// FollowCamData::SetMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ( float )
//
// Return:      void  
//
//=============================================================================
inline void FollowCamData::SetMagnitude( float magnitude )
{
    mMagnitude = magnitude;
    mIsDirty = true;
}

//=============================================================================
// FollowCamData::GetRod
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* rod )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::GetRod( rmt::Vector* rod )
{
    if ( mIsDirty )
    {
        //Update the mRod since new values for rotations and / or magnitude
        //have been added.
        
        rmt::SphericalToCartesian( mMagnitude, mRotationXZ, mRotationY,
                                   &mRod.x, &mRod.z, &mRod.y );

        mIsDirty = false;
    }

    *rod = mRod;
}

//=============================================================================
// FollowCamData::GetTargetOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* offset )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::GetTargetOffset( rmt::Vector* offset ) const
{
    *offset = mTargetOffset;   
}

//=============================================================================
// FollowCamData::SetTargetOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector offset )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetTargetOffset( rmt::Vector offset )
{
    mTargetOffset = offset;
}


//=============================================================================
// FollowCamData::GetCameraLagXZ
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetCameraLagXZ() const
{
    return mCameraLagXZ;
}

//=============================================================================
// FollowCamData::SetCameraLagXZ
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetCameraLagXZ( float lag )
{
    mCameraLagXZ = lag;
}

//=============================================================================
// FollowCamData::GetCameraLagY
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetCameraLagY() const
{
    return mCameraLagY;
}

//=============================================================================
// FollowCamData::SetCameraLagY
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetCameraLagY( float lag )
{
    mCameraLagY = lag;
}

//=============================================================================
// FollowCamData::GetTargetLagXZ
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetTargetLagXZ() const
{
    return mTargetLagXZ;
}

//=============================================================================
// FollowCamData::SetTargetLagXZ
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetTargetLagXZ( float lag )
{
    mTargetLagXZ = lag;
}

//=============================================================================
// FollowCamData::GetTargetLagY
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetTargetLagY() const
{
    return mTargetLagY;
}

//=============================================================================
// FollowCamData::SetTargetLagY
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetTargetLagY( float lag )
{
    mTargetLagY = lag;
}

//=============================================================================
// FollowCamData::GetMagnitudeLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetMagnitudeLag() const
{
    return mMagnitudeLag;
}

//=============================================================================
// FollowCamData::SetMagnitudeLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetMagnitudeLag( float lag )
{
    mMagnitudeLag = lag;
}

//=============================================================================
// FollowCamData::GetCollisionLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetCollisionLag() const
{
    return mCollisionLag;
}

//=============================================================================
// FollowCamData::SetCollisionLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag  )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetCollisionLag( float lag  )
{
    mCollisionLag = lag;
}
//=============================================================================
// FollowCamData::GetUnstableDelay
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline unsigned int FollowCamData::GetUnstableDelay() const
{
    return mUnstableDelay;
}

//=============================================================================
// FollowCamData::SetUnstableDelay
//=============================================================================
// Description: Comment
//
// Parameters:  ( float delay )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetUnstableDelay( unsigned int delay )
{
    mUnstableDelay = delay;
}

//=============================================================================
// FollowCamData::GetQuickTurnDelay
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int FollowCamData::GetQuickTurnDelay() const
{
    return mQuickTurnDelay;
}

//=============================================================================
// FollowCamData::SetQuickTurnDelay
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int delay )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetQuickTurnDelay( unsigned int delay )
{
    mQuickTurnDelay = delay;
}

//=============================================================================
// FollowCamData::GetQuickTurnModifier
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetQuickTurnModifier() const
{
    return mQuickTurnModifier;
}

//=============================================================================
// FollowCamData::SetQuickTurnModifier
//=============================================================================
// Description: Comment
//
// Parameters:  ( float modifier )
//
// Return:      void  
//
//=============================================================================
inline void FollowCamData::SetQuickTurnModifier( float modifier )
{
    mQuickTurnModifier = modifier;
}

//=============================================================================
// FollowCamData::GetAspect
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetAspect() const
{
    return mAspect;
}

//=============================================================================
// FollowCamData::SetAspect
//=============================================================================
// Description: Comment
//
// Parameters:  ( float aspect )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetAspect( float aspect )
{
    mAspect = aspect;
}

//=============================================================================
// FollowCamData::GetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FollowCamData::GetFOV() const
{
    return mFOV;
}

//=============================================================================
// FollowCamData::SetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float fov )
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetFOV( float fov )
{
    mFOV = fov;
}

//=============================================================================
// FollowCamData::SetDirty
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void FollowCamData::SetDirty()
{
    mIsDirty = true;
}

#endif //FOLLOWCAMDATA_H
