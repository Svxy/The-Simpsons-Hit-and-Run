//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        isupercamtarget.h
//
// Description: This is the interface to a camera target.
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ISUPERCAMTARGET_H
#define ISUPERCAMTARGET_H

//========================================
// Nested Includes
//========================================
#ifndef WORLD_BUILDER
namespace RadicalMathLibrary
{
    class Vector;
}
namespace rmt = RadicalMathLibrary;
#else
#include <radmath/radmath.hpp>
#endif

//========================================
// Forward References
//========================================
//=============================================================================
//
// Synopsis:    Inherit this if you want to be a target of the supercam.
//
//=============================================================================

class ISuperCamTarget
{
public:
    ISuperCamTarget() {};
    virtual ~ISuperCamTarget() {};

    virtual void GetPosition( rmt::Vector* position ) = 0;
    virtual void GetHeading( rmt::Vector* heading ) = 0;
    virtual void GetVUP( rmt::Vector* vup ) = 0;
    virtual void GetVelocity( rmt::Vector* velocity ) = 0;
    virtual unsigned int GetID() = 0;
    virtual bool IsCar() const = 0;
    virtual bool IsAirborn() = 0;
    virtual bool IsUnstable() = 0; 
    virtual bool IsQuickTurn() = 0;
    virtual bool IsInReverse() = 0;
    virtual void GetFirstPersonPosition( rmt::Vector* position ) = 0;
    virtual void GetTerrainIntersect( rmt::Vector& pos, rmt::Vector& normal ) const = 0;

    //This is only for debugging, so in the implementation go ahead and 
    //make this return NULL in release.
    virtual const char* const GetName() = 0;

private:

    //Prevent wasteful constructor creation.
    ISuperCamTarget( const ISuperCamTarget& isupercamtarget );
    ISuperCamTarget& operator=( const ISuperCamTarget& isupercamtarget );
};


#endif //ISUPERCAMTARGET_H
