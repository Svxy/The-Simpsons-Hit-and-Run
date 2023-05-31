//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        tuidunaligned.cpp
//
// Description: tuids won't cause your class to get overly padded 
//
// History:     
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <main/tuidunaligned.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// tUidUnaligned::tUidUnaligned
//==============================================================================
// Description: constructor
//
// Parameters:	none
//
// Return:      none
//
// Constraints: none
//
//==============================================================================
tUidUnaligned::tUidUnaligned():
    u0( 0 ),
    u1( 1 )
{
}

//==============================================================================
// tUidUnaligned::tUidUnaligned
//==============================================================================
// Description: copy constructor
//
// Parameters:	right - the object we're copying from
//
// Return:      none
//
// Constraints: none
//
//==============================================================================
tUidUnaligned::tUidUnaligned( const tUidUnaligned& right ):
    u0( right.u0 ),
    u1( right.u1 )
{
}

//==============================================================================
// tUidUnaligned::tUidUnaligned
//==============================================================================
// Description: conversion constructor from 64 bit integers - these are meant
//              to be interchangable
//
// Parameters:	right - the object we're copying from
//
// Return:      none
//
// Constraints: none
//
//==============================================================================
//tUidUnaligned::tUidUnaligned( const radInt64 right )
//{
//    const unsigned int* r = reinterpret_cast< const unsigned int* >( &right );
//    u0 = r[ 0 ];
//    u1 = r[ 1 ];
//}

//=============================================================================
// tUidUnaligned::operator radInt64
//=============================================================================
// Description: conversion operator to 64 bit ints
//
// Parameters:	none
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
//tUidUnaligned::operator radInt64()
//{
//    radInt64 returnMe;
//    unsigned int* r = reinterpret_cast< unsigned int* >( &returnMe );
//    r[ 0 ] = u0;
//    r[ 1 ] = u1;
//    return returnMe;
//}

//=============================================================================
// tUidUnaligned::operator !=
//=============================================================================
// Description: inequality operator
//
// Parameters:	right - are we equal to this?
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
bool tUidUnaligned::operator !=( const tUidUnaligned  right ) const
{
    return ! operator==( right );
}

//=============================================================================
// tUidUnaligned::operator !=
//=============================================================================
// Description: equality operator
//
// Parameters:	right - are we equal to this?
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
bool tUidUnaligned::operator ==( const tUidUnaligned  right ) const
{
    bool returnMe = ( ( u0 == right.u0 ) && ( u1 == right.u1 ) );
    return returnMe;
}

//=============================================================================
// tUidUnaligned::operator <
//=============================================================================
// Description: less than operator
//
// Parameters:	right - are we less than this?
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
bool tUidUnaligned::operator <( const tUidUnaligned  right ) const
{
    radInt64 thisOne;
    unsigned int* t = reinterpret_cast< unsigned int* >( &thisOne );
    t[ 0 ] = u0;
    t[ 1 ] = u1;
    radInt64 rightOne;
    unsigned int* r = reinterpret_cast< unsigned int* >( &rightOne );
    r[ 0 ] = right.u0;
    r[ 1 ] = right.u1;
    return thisOne < rightOne;
}

//=============================================================================
// tUidUnaligned::operator =
//=============================================================================
// Description: assignment operator
//
// Parameters:	assign the two objects
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
//tUidUnaligned& tUidUnaligned::operator=( const tUidUnaligned&  right )
//{
//}

//=============================================================================
// tUidUnaligned::operator^
//=============================================================================
// Description: less than operator
//
// Parameters:	right - are we less than this?
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
tUidUnaligned tUidUnaligned::operator^( const tUidUnaligned  right ) const
{
    radInt64 thisOne;
    unsigned int* t = reinterpret_cast< unsigned int* >( &thisOne );
    t[ 0 ] = u0;
    t[ 1 ] = u1;
    radInt64 rightOne;
    unsigned int* r = reinterpret_cast< unsigned int* >( &rightOne );
    r[ 0 ] = right.u0;
    r[ 1 ] = right.u1;
    return thisOne ^ rightOne;
}

//=============================================================================
// tUidUnaligned::operator *=
//=============================================================================
// Description: in place multiplication
//
// Parameters:	right 8 what are we multiplying by
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
tUidUnaligned tUidUnaligned::operator*=( const radInt64 right )
{
    radInt64 thisOne;
    unsigned int* t = reinterpret_cast< unsigned int* >( &thisOne );
    t[ 0 ] = u0;
    t[ 1 ] = u1;
    thisOne *= right;
    ( *this ) = thisOne;
    return *this;
}

//=============================================================================
// tUidUnaligned::operator &
//=============================================================================
// Description: and operator
//
// Parameters:	right - what are we anding with
//
// Return:      none
//
// Constraints: none
//
//=============================================================================
tUidUnaligned tUidUnaligned::operator&( const tUidUnaligned  right ) const
{
    unsigned int r0 = u0 & right.u0;
    unsigned int r1 = u1 & right.u1;
    tUidUnaligned returnMe;
    returnMe.u0 = r0;
    returnMe.u1 = r1;
    return returnMe;
}


tUidUnaligned tUidUnaligned::operator >>( const int bits ) const
{
    radInt64 returnMe;
    unsigned int* t = reinterpret_cast< unsigned int* >( &returnMe );
    t[ 0 ] = u0;
    t[ 1 ] = u1;
    returnMe = returnMe >> bits;   
    return returnMe;
}
