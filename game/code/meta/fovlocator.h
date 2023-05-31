//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fovlocator.h
//
// Description: Blahblahblah
//
// History:     03/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FOVLOCATOR_H
#define FOVLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <p3d/entity.hpp>
#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================
#include <meta/triggerlocator.h>
#include <meta/locatortypes.h>

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FOVLocator : public TriggerLocator
{
public:
    FOVLocator();
    virtual ~FOVLocator();

    virtual LocatorType::Type GetDataType() const;

    void SetFOV( float fovInRadians );
    float GetFOV() const;

    void SetTime( float seconds );
    float GetTime() const;

    void SetRate( float rate );
    float GetRate() const;
    
    void RegisterDebugData();
    void UnRegisterDebugData();

private:
    float           mFOV;
    float           mTime;
    float           mRate;

    virtual void OnTrigger( unsigned int playerID );

    //Prevent wasteful constructor creation.
    FOVLocator( const FOVLocator& fovlocator );
    FOVLocator& operator=( const FOVLocator& fovlocator );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// FOVLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorType ::Type
//
//=============================================================================
inline LocatorType::Type FOVLocator::GetDataType() const
{
    return( LocatorType::FOV );
}

//=============================================================================
// FOVLocator::SetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float fovInRadians )
//
// Return:      void 
//
//=============================================================================
inline void FOVLocator::SetFOV( float fovInRadians )
{
    mFOV = fovInRadians;
}

//=============================================================================
// FOVLocator::GetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FOVLocator::GetFOV() const
{
    return mFOV;
}

//=============================================================================
// FOVLocator::SetTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( float seconds )
//
// Return:      void 
//
//=============================================================================
inline void FOVLocator::SetTime( float seconds )
{
    mTime = seconds;
}

//=============================================================================
// FOVLocator::GetTime
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FOVLocator::GetTime() const
{
    return mTime;
}

//=============================================================================
// FOVLocator::SetRate
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rate )
//
// Return:      void 
//
//=============================================================================
inline void FOVLocator::SetRate( float rate )
{
    mRate = rate;
}

//=============================================================================
// FOVLocator::GetRate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float FOVLocator::GetRate() const
{
    return mRate;
}

#endif //FOVLOCATOR_H
