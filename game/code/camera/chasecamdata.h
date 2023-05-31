//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        chasecamdata.h
//
// Description: Blahblahblah
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef CHASECAMDATA_H
#define CHASECAMDATA_H

//========================================
// Nested Includes
//========================================
//These are included in the precompiled headers on XBOX and GC
#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ChaseCamData
{
public:
    ChaseCamData();
    virtual ~ChaseCamData() {};

    void SetRod( rmt::Vector rod );
    void GetRod( rmt::Vector* rod ) const;    

    void SetPositionLag( float lag );
    float GetPositionLag() const;

    void SetTargetLag( float lag );
    float GetTargetLag() const;

    void SetMinFOV( float min );
    float GetMinFOV() const;

    void SetMaxFOV( float max );
    float GetMaxFOV() const;

    void SetMaxSpeed( float speed );
    float GetMaxSpeed() const;

    void SetFOVLag( float lag );
    float GetFOVLag() const;

    rmt::Vector mRod;
    float mPositionLag;
    float mTargetLag;
    float mMinFOV;
    float mMaxFOV;
    float mMaxSpeed;
    float mFOVLag;

private:
    //Prevent wasteful constructor creation.
    ChaseCamData( const ChaseCamData& chasecamdata );
    ChaseCamData& operator=( const ChaseCamData& chasecamdata );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ChaseCamData::ChaseCamData
//=============================================================================
// Description: Constructor
//
// Parameters:  ()
//
// Return:      nothing
//
//=============================================================================
inline ChaseCamData::ChaseCamData() :
    mPositionLag( 0.01f ),
    mTargetLag( 0.089f ),
    mMinFOV( 0.3487f ), //Fudge...
    mMaxFOV( 1.75079f ), //Fudge...
    mMaxSpeed( 0.233f ),
    mFOVLag( 0.05f )
{
    mRod.Set( 0.0f, 50.0f, -15.0f );
}

//=============================================================================
// ChaseCamData::SetRod
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector rod )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::SetRod( rmt::Vector rod )
{
    mRod = rod;
}

//=============================================================================
// ChaseCamData::GetRod
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* rod )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::GetRod( rmt::Vector* rod ) const
{
    *rod = mRod;
}

//=============================================================================
// ChaseCamData::SetPositionLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::SetPositionLag( float lag )
{
    mPositionLag = lag;
}

//=============================================================================
// ChaseCamData::GetPositionLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float ChaseCamData::GetPositionLag() const
{
    return mPositionLag;
}

//=============================================================================
// ChaseCamData::SetTargetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::SetTargetLag( float lag )
{
    mTargetLag = lag;
}

//=============================================================================
// ChaseCamData::GetTargetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float ChaseCamData::GetTargetLag() const
{
    return mTargetLag;
}

//=============================================================================
// ChaseCamData::SetMinFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float min )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::SetMinFOV( float min )
{
    mMinFOV = min;
}

//=============================================================================
// ChaseCamData::GetMinFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float ChaseCamData::GetMinFOV() const
{
    return mMinFOV;
}

//=============================================================================
// ChaseCamData::SetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float max )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::SetMaxFOV( float max )
{
    mMaxFOV = max;
}

//=============================================================================
// ChaseCamData::GetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float ChaseCamData::GetMaxFOV() const
{
    return mMaxFOV;
}

//=============================================================================
// ChaseCamData::SetMaxSpeed
//=============================================================================
// Description: Comment
//
// Parameters:  ( float speed )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::SetMaxSpeed( float speed )
{
    mMaxSpeed = speed;
}

//=============================================================================
// ChaseCamData::GetMaxSpeed
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float ChaseCamData::GetMaxSpeed() const
{
    return mMaxSpeed;
}

//=============================================================================
// ChaseCamData::SetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCamData::SetFOVLag( float lag )
{
    mFOVLag = lag;
}

//=============================================================================
// ChaseCamData::GetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float ChaseCamData::GetFOVLag() const
{
    return mFOVLag;
}

#endif //CHASECAMDATA_H
