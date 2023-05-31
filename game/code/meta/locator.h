//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        locator.h
//
// Description: Blahblahblah
//
// History:     04/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef LOCATOR_H
#define LOCATOR_H

//========================================
// Nested Includes
//========================================
#include <p3d/entity.hpp>
#include <radmath/radmath.hpp>

#include <presentation/gui/utility/hudmap.h>

#ifndef WORLD_BUILDER
#include <meta/locatortypes.h>
#else
#include "locatortypes.h"
#endif

//========================================
// Forward References
//========================================


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class Locator : public tEntity, public IHudMapIconLocator
{
public:
    enum Flag
    {
        ACTIVE,
        DRAWN,
        USED,
        INTERIOR
    };

    Locator();
    virtual ~Locator();

    void SetLocation( const rmt::Vector& loc );
    void GetLocation( rmt::Vector* loc ) const;

    void            SetID( unsigned int id );
    unsigned int    GetID() const;

    void            SetData( unsigned int data );
    unsigned int    GetData() const;

    virtual LocatorType::Type GetDataType() const;  //MUST OVERRIDE THIS!

    void SetFlag( Flag flag, bool on );
    bool GetFlag( Flag flag ) const ;  

    virtual void            SetNumTriggers( unsigned int i, int allocID ) {};   //This makes loading simpler.
    virtual unsigned int    GetNumTriggers() { return 0; };                     //This makes things simpler.
    virtual void            SetMatrix( const rmt::Matrix& mat ) {};             //Again, for simplicity.

    //For IHudMapIconLocator
    virtual void GetPosition( rmt::Vector* currentLoc );
    virtual void GetHeading( rmt::Vector* heading );

    virtual bool TriggerAllowed( int playerID ) { return true; }; //This is different from the ACTIVE flag.

private:
    unsigned int    mID;       // Used to number-id locators.
    unsigned int    mData;     // Here is the data in the locator
    unsigned int    mFlags;    // Flags, we don't need no... oh wait, maybe we do.
    
    rmt::Vector mLocation;  //Where am I?
    
    //Prevent wasteful constructor creation.
    Locator( const Locator& locator );
    Locator& operator=( const Locator& locator );
};


//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************


//=============================================================================
// Locator::SetLocation
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& loc )
//
// Return:      void 
//
//=============================================================================
inline void Locator::SetLocation( const rmt::Vector& loc )
{
    mLocation = loc;
}

//=============================================================================
// Locator::GetLocation
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* loc )
//
// Return:      void 
//
//=============================================================================
inline void Locator::GetLocation( rmt::Vector* loc ) const
{
    *loc = mLocation;
}

//=============================================================================
// Locator::SetID
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int id )
//
// Return:      void 
//
//=============================================================================
inline void Locator::SetID( unsigned int id )
{
    mID = id;
}

//=============================================================================
// Locator::GetID
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int Locator::GetID() const
{
    return mID;
}

//=============================================================================
// Locator::SetData
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int data )
//
// Return:      void 
//
//=============================================================================
inline void Locator::SetData( unsigned int data )
{
    mData = data;
}

//=============================================================================
// Locator::GetData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int Locator::GetData() const
{
    return mData;
}

//=============================================================================
// Locator::SetFlag
//=============================================================================
// Description: Comment
//
// Parameters:  ( Flag flag, bool on )
//
// Return:      void 
//
//=============================================================================
inline void Locator::SetFlag( Flag flag, bool on )
{
    on ? mFlags |= ( 1 << flag ) : mFlags &= ~( 1 << flag ); 
}

//=============================================================================
// Locator::GetFlag
//=============================================================================
// Description: Comment
//
// Parameters:  ( Flag flag )
//
// Return:      bool 
//
//=============================================================================
inline bool Locator::GetFlag( Flag flag ) const
{
    return ( (mFlags & ( 1 << flag )) > 0 );
}
    
//=============================================================================
// ::Type GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type Locator::GetDataType() const
{
    return( LocatorType::GENERIC );
}

#endif //LOCATOR_H
