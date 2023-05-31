//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        bumpercamdata.h
//
// Description: Blahblahblah
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BUMPERCAMDATA_H
#define BUMPERCAMDATA_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class BumperCamData
{
public:
    BumperCamData();
    virtual ~BumperCamData() {};

    void GetFrontPosition( rmt::Vector* front );
    void SetFrontPosition( rmt::Vector front );
    void GetFrontTarget( rmt::Vector* front );
    void SetFrontTarget( rmt::Vector front );

    void GetBackPosition( rmt::Vector* back );
    void SetBackPosition( rmt::Vector back );
    void GetBackTarget( rmt::Vector* back );
    void SetBackTarget( rmt::Vector back );

    float GetFOV() const;
    void SetFOV( float fov );

    rmt::Vector mFrontPos;
    rmt::Vector mFrontTarg;
    rmt::Vector mBackPos;
    rmt::Vector mBackTarg;

    float mFOV;

private:
    //Prevent wasteful constructor creation.
    BumperCamData( const BumperCamData& bumpercamdata );
    BumperCamData& operator=( const BumperCamData& bumpercamdata );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// BumperCamData::BumperCamData
//=============================================================================
// Description: Constructor
//
// Parameters:  ()
//
// Return:      BumperCamData
//
//=============================================================================
inline BumperCamData::BumperCamData() :
    mFOV( 1.5707f )
{
    mFrontPos.Set( 0.0f, 0.0f, 2.3f );    
    mFrontTarg.Set( 0.0f, 0.0f, 2.8f );    
    mBackPos.Set( 0.0f, 0.0f, -2.22f );
    mBackTarg.Set( 0.0f, 0.0f, -2.8f );
}

//=============================================================================
// BumperCamData::GetFrontPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* front )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::GetFrontPosition( rmt::Vector* front )
{
    *front = mFrontPos;
}

//=============================================================================
// BumperCamData::SetFrontPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector front )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::SetFrontPosition( rmt::Vector front )
{
    mFrontPos = front;
}

//=============================================================================
// BumperCamData::GetFrontTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* front )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::GetFrontTarget( rmt::Vector* front )
{
    *front = mFrontTarg;
}

//=============================================================================
// BumperCamData::SetFrontTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector front )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::SetFrontTarget( rmt::Vector front )
{
    mFrontTarg = front;
}

//=============================================================================
// BumperCamData::GetBackPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* back )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::GetBackPosition( rmt::Vector* back )
{
    *back = mBackPos;
}

//=============================================================================
// BumperCamData::SetBackPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector back )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::SetBackPosition( rmt::Vector back )
{
    mBackPos = back;
}

//=============================================================================
// BumperCamData::GetBackTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* back )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::GetBackTarget( rmt::Vector* back )
{
    *back = mBackTarg;
}

//=============================================================================
// BumperCamData::SetBackTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector back )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::SetBackTarget( rmt::Vector back )
{
    mBackTarg = back;
}

//=============================================================================
// BumperCamData::GetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float BumperCamData::GetFOV() const
{
    return mFOV;
}

//=============================================================================
// BumperCamData::SetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float fov )
//
// Return:      void 
//
//=============================================================================
inline void BumperCamData::SetFOV( float fov )
{
    mFOV = fov;
}

#endif //BUMPERCAMDATA_H
