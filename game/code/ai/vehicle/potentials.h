//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        potentials.h
//
// Description: Blahblahblah
//
// History:     22/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef POTENTIALS_H
#define POTENTIALS_H

//========================================
// Nested Includes
//========================================

#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class Potentials
{
public:
    Potentials();
    virtual ~Potentials();

    static const int MAX_POTENTIALS = 11;

    void SetPotential( const int index, float value );
    float GetPotential( const int index ) const;

    void Clear( const float value );

private:

    //Prevent wasteful constructor creation.
    Potentials( const Potentials& potentials );
    Potentials& operator=( const Potentials& potentials );

    float mValues[ MAX_POTENTIALS ];
};

//=============================================================================
// Potentials::SetPotential
//=============================================================================
// Description: Comment
//
// Parameters:  ( const int index )
//
// Return:      void 
//
//=============================================================================
inline void Potentials::SetPotential( const int index, float value )
{
    rAssert( index >= 0 && index < MAX_POTENTIALS - 1 );

/*    if( value > 1.0f )
    {
        value = 1.0f;
    }
    if( value < -1.0f )
    {
        value = -1.0f;
    }*/
    mValues[ index ] = value;
}

//=============================================================================
// Potentials::GetPotential
//=============================================================================
// Description: Comment
//
// Parameters:  ( const int index )
//
// Return:      float 
//
//=============================================================================
inline float Potentials::GetPotential( const int index ) const
{
    rAssert( index >= 0 && index < MAX_POTENTIALS );
    return mValues[ index ];
}

#endif //POTENTIALS_H
