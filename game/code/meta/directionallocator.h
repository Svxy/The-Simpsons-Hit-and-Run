//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        directionallocator.h
//
// Description: Blahblahblah
//
// History:     30/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef DIRECTIONALLOCATOR_H
#define DIRECTIONALLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/locator.h>
#include <meta/locatortypes.h>

#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class DirectionalLocator : public Locator
{
public:
    DirectionalLocator();
    virtual ~DirectionalLocator();

    virtual LocatorType::Type GetDataType() const;

    void SetTransform( const rmt::Matrix& transform );
    const rmt::Matrix& GetTransform() const;

private:

    rmt::Matrix mTransform;

    //Prevent wasteful constructor creation.
    DirectionalLocator( const DirectionalLocator& directionallocator );
    DirectionalLocator& operator=( const DirectionalLocator& directionallocator );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// DirectionalLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorType::Type
//
//=============================================================================
inline LocatorType::Type DirectionalLocator::GetDataType() const
{
    return LocatorType::DIRECTIONAL;
}


//=============================================================================
// DirectionalLocator::SetTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Matrix& transform )
//
// Return:      void 
//
//=============================================================================
inline void DirectionalLocator::SetTransform( const rmt::Matrix& transform )
{
    mTransform = transform;
}

//=============================================================================
// DirectionalLocator::GetTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      rmt
//
//=============================================================================
inline const rmt::Matrix& DirectionalLocator::GetTransform() const
{
    return mTransform;
}

#endif //DIRECTIONALLOCATOR_H
