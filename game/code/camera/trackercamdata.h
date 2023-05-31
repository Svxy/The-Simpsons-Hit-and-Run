//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trackercamdata.h
//
// Description: Blahblahblah
//
// History:     08/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRACKERCAMDATA_H
#define TRACKERCAMDATA_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TrackerCamData
{
public:
    TrackerCamData();
    virtual ~TrackerCamData() {};

    void SetMinFOV( float min );
    float GetMinFOV() const;

    void SetMaxFOV( float max );
    float GetMaxFOV() const;

    void SetFOVLag( float lag );
    float GetFOVLag() const;

    float mMinFOV;
    float mMaxFOV;
    float mFOVLag;

private:

    //Prevent wasteful constructor creation.
    TrackerCamData( const TrackerCamData& trackercamdata );
    TrackerCamData& operator=( const TrackerCamData& trackercamdata );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// TrackerCamData::TrackerCamData
//=============================================================================
// Description: Constructor
//
// Parameters:  ()
//
// Return:      TrackerCamData
//
//=============================================================================
inline TrackerCamData::TrackerCamData() :
    mMinFOV( 0.3487f ), //Fudge...
    mMaxFOV( 1.75079f ), //Fudge...
    mFOVLag( 0.05f )
{
}

//=============================================================================
// TrackerCamData::SetMinFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float min )
//
// Return:      void 
//
//=============================================================================
inline void TrackerCamData::SetMinFOV( float min )
{
    mMinFOV = min;
}

//=============================================================================
// TrackerCamData::GetMinFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float TrackerCamData::GetMinFOV() const
{
    return mMinFOV;
}

//=============================================================================
// TrackerCamData::SetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float max )
//
// Return:      void 
//
//=============================================================================
inline void TrackerCamData::SetMaxFOV( float max )
{
    mMaxFOV = max;
}

//=============================================================================
// TrackerCamData::GetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float TrackerCamData::GetMaxFOV() const
{
    return mMaxFOV;
}

//=============================================================================
// TrackerCamData::SetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void TrackerCamData::SetFOVLag( float lag )
{
    mFOVLag = lag;
}

//=============================================================================
// TrackerCamData::GetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float TrackerCamData::GetFOVLag() const
{
    return mFOVLag;
}

#endif //TRACKERCAMDATA_H
