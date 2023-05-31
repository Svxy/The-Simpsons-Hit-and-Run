//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        splinelocator.h
//
// Description: Blahblahblah
//
// History:     05/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SPLINELOCATOR_H
#define SPLINELOCATOR_H

//========================================
// Nested Includes
//========================================

#include <meta/triggerlocator.h>

//========================================
// Forward References
//========================================

class RailCam;
class SuperCam;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SplineLocator : public TriggerLocator
{
public:
    SplineLocator();
    virtual ~SplineLocator();

    virtual LocatorType::Type GetDataType() const;

    RailCam* GetRailCam() const;
    void SetRailCam( RailCam* spline );
    void SetCarOnly( bool carOnly );
    void SetOnFootOnly( bool onFootOnly );
    void SetCutInOut( bool cut );
    void SetReset( bool reset );

    bool TriggerAllowed( int playerID );

private:
    RailCam* mRailCam;
    int      mRailNum;
    unsigned int mTriggerCount;
    SuperCam* mLastSuperCam;
    bool mCarOnly;
    bool mOnFootOnly;
    bool mCutInOut;
    bool mReset;

    virtual void OnTrigger( unsigned int playerID );

    //Prevent wasteful constructor creation.
    SplineLocator( const SplineLocator& splinelocator );
    SplineLocator& operator=( const SplineLocator& splinelocator );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// SplineLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type SplineLocator::GetDataType() const
{
    return( LocatorType::SPLINE );
}
//=============================================================================
// SplineLocator::GetRailCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline RailCam* SplineLocator::GetRailCam() const
{
    return mRailCam;
}

//=============================================================================
// SplineLocator::SetCarOnly
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool carOnly )
//
// Return:      void 
//
//=============================================================================
inline void SplineLocator::SetCarOnly( bool carOnly )
{
    mCarOnly = carOnly;
}

//=============================================================================
// SplineLocator::SetOnFootOnly
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool onFootOnly )
//
// Return:      void 
//
//=============================================================================
inline void SplineLocator::SetOnFootOnly( bool onFootOnly )
{
    if ( onFootOnly == true )
    {
        rAssert( mCarOnly != true );
    }

    mOnFootOnly = onFootOnly;
}

//=============================================================================
// SplineLocator::SetCutInOut
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool cut )
//
// Return:      void 
//
//=============================================================================
inline void SplineLocator::SetCutInOut( bool cut )
{
    mCutInOut = cut;
}

//=============================================================================
// SplineLocator::SetReset
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool reset )
//
// Return:      void 
//
//=============================================================================
inline void SplineLocator::SetReset( bool reset )
{
    mReset = reset;
}

#endif //SPLINELOCATOR_H
