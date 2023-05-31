//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        safezone.cpp
//
// Description:  Represent an area around a locator that if a player enters it will deactivate AI cars and trigger possibly other events
//
// History:     11/25/2002 + Created -- Chuck C.
//
//=============================================================================


//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>


//========================================
// Project Includes
//========================================
#include <meta/carstartlocator.h>
#include <mission/safezone/safezone.h>


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================



//Default 
SafeZone::SafeZone ( )
{
    mLocator = NULL;
    mRadius = 1;
};

SafeZone::~SafeZone ()
{

};

//Use this Constructor 
SafeZone::SafeZone(CarStartLocator* locator,unsigned int radius)
{
    mLocator = locator;
    mRadius = radius;
};


//pass in  a vector and it will return true if its in the safzones radius, else false
bool SafeZone::InsideZone( rmt::Vector vector)
{
    rmt::Vector temp;

    mLocator->GetLocation( &temp );
    
    if ((rmt::Sqr(vector.x -temp.x) + rmt::Sqr(vector.z- temp.z) )<= rmt::Sqr (mRadius) )
    {
        return true;
    }
    else
    {
        return false;
    }


};


//returns a position of the safezone
rmt::Vector SafeZone::GetPosition ()
{

        rmt::Vector temp;
        mLocator->GetLocation( &temp );
        return temp;
};


//returns the radius of the safezone
unsigned int SafeZone::GetRadius()
{
    return mRadius;
};


