//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: this file contains some operators for colors *, +, -, etc
//
// History:     29/01/2003 + Created -- Ian Gipson
//
//=============================================================================

#ifndef _COLOURUTILITY_H_
#define _COLOURUTILITY_H_

#include <p3d/p3dtypes.hpp>

//==============================================================================
// operator+
//==============================================================================
// Description: addition of two colors
//
// Parameters:	a, b, the two colors to add
//
// Return:      the added version of the two colors
//
//==============================================================================
inline tColour operator+( const tColour a, const tColour b )
{
    int rr = a.Red()   + b.Red();
    int gg = a.Green() + b.Green();
    int bb = a.Blue()  + b.Blue();
    int aa = a.Alpha() + b.Alpha();
    return( tColour( rr, gg, bb, aa ) );
}

//==============================================================================
// operator*
//==============================================================================
// Description: multiplication of a color by a constant
//
// Parameters:	a, the color
//              f, the float
//
// Return:      the scaled version of the colour
//
//==============================================================================
inline tColour operator*( const tColour a, const float f )
{
    int rr = static_cast< int >( a.Red()   * f );
    int gg = static_cast< int >( a.Green() * f );
    int bb = static_cast< int >( a.Blue()  * f );
    int aa = static_cast< int >( a.Alpha() * f );
    return tColour( rr, gg, bb, aa );
}

inline tColour operator*( const float f, const tColour a )
{
    return operator*( a, f);
}

#endif


