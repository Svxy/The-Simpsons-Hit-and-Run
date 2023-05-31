//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        potentialfield.h
//
// Description: Blahblahblah
//
// History:     22/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef POTENTIALFIELD_H
#define POTENTIALFIELD_H

//========================================
// Nested Includes
//========================================

#include <ai/vehicle/potentials.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class PotentialField
{
public:
    PotentialField();
    virtual ~PotentialField();

    static const int MAX_FIELD_POTENTIALS = 50;

    void Clear( const float value = -1.0f );

    //
    // xdim is the distance in metres to the left and right of the field
    // ydim is the distance along the heading vector of the field
    // pos is the position of the vehicle (becomes the origin)
    // heading is the direction of the vehicle (forms the z axis of the field)
    //
    void Initialize( const float xdim, const float ydim, const rmt::Vector& pos, const rmt::Vector& heading );

    void IndexToPos( const int x, const int y, rmt::Vector& pos );
    void PosToIndex( rmt::Vector& pos, int& x, int& y );

    void SetPotential( const int x, const int y, const float value );
    float GetPotential( const int x, const int y ) const;
    
    //
    // pos is the location in world space of the potential
    // value is the potential to be added
    // falloff is how much, per metre, the potential fades
    //
    bool AddPotential( rmt::Vector& pos, const float value, 
        const float radius, const float falloff = 0.1f );

    //
    // returns in pos the location in world space of the highest potential 
    // at distance targetdist.
    // targetdist is the distance along the field's z axis (the heading vector)
    // at which the highest potential will be found
    //
    bool FindBestPosition( rmt::Vector& pos, const float targetdist );

private:

    //Prevent wasteful constructor creation.
    PotentialField( const PotentialField& potentialfield );
    PotentialField& operator=( const PotentialField& potentialfield );

    Potentials mPotentials[ MAX_FIELD_POTENTIALS ];

    float mXdim;
    float mZdim;
    float mXscale;
    float mZscale;
    rmt::Vector mXaxis;
    rmt::Vector mZaxis;
    rmt::Vector mOrigin;
}; 


#endif //POTENTIALFIELD_H
