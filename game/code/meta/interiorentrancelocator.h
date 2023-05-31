//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        interiorentrancelocator.h
//
// Description: Blahblahblah
//
// History:     30/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef INTERIORENTRANCELOCATOR_H
#define INTERIORENTRANCELOCATOR_H

//========================================
// Nested Includes
//========================================
#include <meta/locatortypes.h>
#include <meta/locatorevents.h>
#include <meta/eventlocator.h>

#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================
namespace ActionButton
{
    class EnterInterior;
};
//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class InteriorEntranceLocator : public EventLocator
{
public:
    InteriorEntranceLocator();
    virtual ~InteriorEntranceLocator();

    virtual LocatorType::Type GetDataType() const;

    void SetInteriorFileName( const char* fileName );
    const char* GetInteriorFileName() const;

    void SetInteriorFileNameSize( unsigned int size ); //This is to prevent fragmentation.
    unsigned int GetInteriorFileNameSize() const;

    void SetTransform( const rmt::Matrix& transform );
    const rmt::Matrix& GetTransform() const;

private:
    virtual void OnTrigger( unsigned int playerID );
    
    char* mInteriorFileName;
    unsigned int mInteriorFileNameSize;
    rmt::Matrix mTransform;
    ActionButton::EnterInterior* mpEnterInteriorAction;

    //Prevent wasteful constructor creation.
    InteriorEntranceLocator( const InteriorEntranceLocator& interiorentrancelocator );
    InteriorEntranceLocator& operator=( const InteriorEntranceLocator& interiorentrancelocator );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// InteriorEntranceLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type InteriorEntranceLocator::GetDataType() const
{
    return( LocatorType::INTERIOR_ENTRANCE );
}

//=============================================================================
// InteriorEntranceLocator::GetInteriorFileName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* InteriorEntranceLocator::GetInteriorFileName() const
{
    return mInteriorFileName;
}

//=============================================================================
// InteriorEntranceLocator::GetInteriorFileNameSize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int InteriorEntranceLocator::GetInteriorFileNameSize() const
{
    return mInteriorFileNameSize;
}

//=============================================================================
// InteriorEntranceLocator::SetTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Matrix& transform )
//
// Return:      void 
//
//=============================================================================
inline void InteriorEntranceLocator::SetTransform( const rmt::Matrix& transform )
{
    mTransform = transform;
}

//=============================================================================
// InteriorEntranceLocator::GetTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      rmt
//
//=============================================================================
inline const rmt::Matrix& InteriorEntranceLocator::GetTransform() const
{
    return mTransform;
}
#endif //INTERIORENTRANCELOCATOR_H
