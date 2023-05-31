//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        staticcamlocator.h
//
// Description: Blahblahblah
//
// History:     9/18/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef STATICCAMLOCATOR_H
#define STATICCAMLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/triggerlocator.h>

//========================================
// Forward References
//========================================

class StaticCam;
class SuperCam;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class StaticCamLocator : public TriggerLocator
{
public:
    StaticCamLocator();
	virtual ~StaticCamLocator();

    virtual LocatorType::Type GetDataType() const;

    StaticCam* GetStaticCam() const;
    void SetStaticCam( StaticCam* cam );
    void SetOneShot( bool oneShot );
    void SetCarOnly( bool carOnly );
    void SetOnFootOnly( bool onFootOnly );
    void SetCutInOut( bool cut );

    bool TriggerAllowed( int playerID );

private:
    StaticCam*      mStaticCam;
    int             mCamNum;
    unsigned int    mTriggerCount;
    SuperCam* mLastSuperCam;
    bool mOneShot;
    bool mOneShotted;  //Haha.
    bool mCarOnly;
    bool mOnFootOnly;
    bool mCutInOut;

    virtual void OnTrigger( unsigned int playerID );

    //Prevent wasteful constructor creation.
	StaticCamLocator( const StaticCamLocator& staticcamlocator );
	StaticCamLocator& operator=( const StaticCamLocator& staticcamlocator );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// StaticCamLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type StaticCamLocator::GetDataType() const
{
    return( LocatorType::STATIC_CAMERA );
}

//=============================================================================
// StaticCamLocator::GetRailCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      StaticCamera 
//
//=============================================================================
inline StaticCam* StaticCamLocator::GetStaticCam() const
{
    return mStaticCam;
}

//=============================================================================
// StaticCamLocator::SetOneShot
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool oneShot )
//
// Return:      void 
//
//=============================================================================
inline void StaticCamLocator::SetOneShot( bool oneShot )
{
    mOneShot = oneShot;
}

//=============================================================================
// StaticCamLocator::SetCarOnly
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool carOnly )
//
// Return:      void 
//
//=============================================================================
inline void StaticCamLocator::SetCarOnly( bool carOnly )
{
    mCarOnly = carOnly;
}

//=============================================================================
// StaticCamLocator::SetOnFootOnly
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool onFootOnly )
//
// Return:      void 
//
//=============================================================================
inline void StaticCamLocator::SetOnFootOnly( bool onFootOnly )
{
    if ( onFootOnly == true )
    {
        rAssert( mCarOnly != true );
    }

    mOnFootOnly = onFootOnly;
}

//=============================================================================
// StaticCamLocator::SetCutInOut
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool cut )
//
// Return:      void 
//
//=============================================================================
inline void StaticCamLocator::SetCutInOut( bool cut )
{
    mCutInOut = cut;
}


#endif //STATICCAMLOCATOR_H
