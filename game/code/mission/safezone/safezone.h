//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        safezone.h
//
// Description:  Represent an area around a locator that if a player enters it will deactivate AI cars and trigger possibly other events
//
// History:     11/25/2002 + Created -- Chuck C.
//
//=============================================================================

#ifndef SAFEZONE_H
#define SAFEZONE_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>


//========================================
// Forward References
//========================================
class CarStartLocator;

//=============================================================================
//
// Synopsis:    SafeZone Class
//
//=============================================================================


class SafeZone
{
public:
    SafeZone ();
    SafeZone(CarStartLocator* locator,unsigned int radius);
	virtual ~SafeZone();

    bool InsideZone( rmt::Vector vector);
    rmt::Vector GetPosition ();
    unsigned int GetRadius ();
    
private:
    CarStartLocator* mLocator;
    unsigned int mRadius ;   

    
    //Prevent wasteful constructor creation.
	SafeZone( const SafeZone& safezone );
	SafeZone& operator=( const SafeZone& safezone );
};


#endif //SAFEZONE_H
