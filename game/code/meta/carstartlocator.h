//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        carstartlocator.h
//
// Description: Blahblahblah
//
// History:     19/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef CARSTARTLOCATOR_H
#define CARSTARTLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/locator.h>
#include <meta/locatortypes.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class CarStartLocator : public Locator
{
public:
    CarStartLocator();
    virtual ~CarStartLocator();
    
    virtual LocatorType::Type GetDataType() const;

    float GetRotation() const;
    void SetRotation( float rot );

private:

    float mYRotation;

    //Prevent wasteful constructor creation.
    CarStartLocator( const CarStartLocator& carstartlocator );
    CarStartLocator& operator=( const CarStartLocator& carstartlocator );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// CarStartLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type CarStartLocator::GetDataType() const
{
    return( LocatorType::CAR_START );
}

//=============================================================================
// CarStartLocator::GetRotation
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float CarStartLocator::GetRotation() const
{
    return mYRotation;
}

//=============================================================================
// CarStartLocator::SetRotation
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rot )
//
// Return:      void 
//
//=============================================================================
inline void CarStartLocator::SetRotation( float rot )
{
    mYRotation = rot;
}

#endif //CARSTARTLOCATOR_H
