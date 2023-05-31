//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement PotentialField
//
// History:     22/07/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================

#include <ai/vehicle/potentialfield.h>
#include <ai/vehicle/potentials.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

static const float Z_ADJUST = 5.0f;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// PotentialField::PotentialField
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PotentialField::PotentialField()
{
}

//==============================================================================
// PotentialField::~PotentialField
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PotentialField::~PotentialField()
{
}

//=============================================================================
// PotentialField::Clear
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PotentialField::Clear( const float value )
{
    for( int i = 0; i < MAX_FIELD_POTENTIALS; i++ )
    {
        mPotentials[ i ].Clear( value );
    }
}

//=============================================================================
// PotentialField::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ( const float xdim, const float ydim, const rmt::Vector& pos, const rmt::Vector& heading )
//
// Return:      void 
//
//=============================================================================
void PotentialField::Initialize( const float xdim, const float ydim, const rmt::Vector& pos, const rmt::Vector& heading )
{
    mXscale = xdim / Potentials::MAX_POTENTIALS;
    mZscale = ydim / MAX_FIELD_POTENTIALS;
    
    mZaxis = heading;
    mXaxis.CrossProduct( mZaxis, rmt::Vector( 0.0f, 1.0f, 0.0f ) );//, mZaxis );

    //
    // Adjust the origin by half the length of the xdim to move the
    // origin from the bottom-centre of the field to the bottom-left
    //
    rmt::Vector tmp = mXaxis;
    tmp.Scale( static_cast<float>( -xdim / 2 ));

    mOrigin = pos;
    mOrigin.Add( tmp );

    //
    // Move the origin forward on the z axis
    //
    tmp = mZaxis;
    tmp.Scale( Z_ADJUST, Z_ADJUST, Z_ADJUST );
    mOrigin.Add( tmp );
}

//=============================================================================
// PotentialField::IndexToPos
//=============================================================================
// Description: Comment
//
// Parameters:  ( const int x, const int y, rmt::Vector& pos )
//
// Return:      void 
//
//=============================================================================
void PotentialField::IndexToPos( const int x, const int y, rmt::Vector& pos )
{
    float xpos = static_cast<float>( x ) * mXscale;
    float zpos = static_cast<float>( y ) * mZscale;

    pos = mOrigin;

    rmt::Vector tmp = mXaxis;
    tmp.Scale( xpos, xpos, xpos );
    pos.Add( tmp );

    tmp = mZaxis;
    tmp.Scale( zpos, zpos, zpos );
    pos.Add( tmp );
}

//=============================================================================
// PotentialField::PosToIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector& pos, int& x, int& y )
//
// Return:      void 
//
//=============================================================================
void PotentialField::PosToIndex( rmt::Vector& pos, int& x, int& y )
{
    rmt::Vector relpos;
    relpos.Sub( pos, mOrigin );

    float xpos = mXaxis.DotProduct( relpos ) / mXscale;
    float zpos = mZaxis.DotProduct( relpos ) / mZscale;

    x = static_cast<int>( xpos );
    y = static_cast<int>( zpos );
}

//=============================================================================
// PotentialField::SetPotential
//=============================================================================
// Description: Comment
//
// Parameters:  ( const int x, const int y, const float value )
//
// Return:      void 
//
//=============================================================================
void PotentialField::SetPotential( const int x, const int y, const float value )
{
    rAssert( y < MAX_FIELD_POTENTIALS );
    mPotentials[ y ].SetPotential( x, value );
}

//=============================================================================
// PotentialField::GetPotential
//=============================================================================
// Description: Comment
//
// Parameters:  ( const int x, const int y )
//
// Return:      float 
//
//=============================================================================
float PotentialField::GetPotential( const int x, const int y ) const
{
    rAssert( y < MAX_FIELD_POTENTIALS );
    return mPotentials[ y ].GetPotential( x );
}

//=============================================================================
// PotentialField::AddPotential
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector& pos, const float value, const float falloff )
//
// Return:      void 
//
//=============================================================================
bool PotentialField::AddPotential( rmt::Vector& pos, const float value, const float radius, const float falloff )
{
    int xpos;
    int zpos;

    PosToIndex( pos, xpos, zpos );

    //
    // Calc the distance is metres until the potential has no influence
    //
    float disttozero = radius + rmt::Fabs( value / falloff );
    float disttozerosqr = disttozero * disttozero;

    int xsize = 2 * static_cast<int>( disttozero / mXscale );
    int zsize = 2 * static_cast<int>( disttozero / mZscale );

    int xstart = xpos - xsize / 2;
    int zstart = zpos - zsize / 2;

    if( xstart < 0 )
    {
        //
        // xstart is -ve, so by adding it to xsize we decrease 
        // xsize by the right amount
        //
        xsize += xstart;
        xstart = 0;
    }
    if( xstart + xsize >= Potentials::MAX_POTENTIALS )
    {
        xsize = Potentials::MAX_POTENTIALS - xstart - 1;
    }

    if( zstart < 0 )
    {
        zsize -= zstart;
        zstart = 0;
    }
    if( zstart + zsize >= MAX_FIELD_POTENTIALS )
    {
        zsize = Potentials::MAX_POTENTIALS - zstart - 1;
    }

    if( xsize <= 0 || zsize <= 0 
        || xstart >= Potentials::MAX_POTENTIALS 
        || zstart >= MAX_FIELD_POTENTIALS )
    {
        return false;
    }

    //
    // Sign the falloff to make for easier math down below
    //
    float signedfalloff;
    if( value < 0.0f )
    {
        signedfalloff = -falloff;
    }
    else
    {
        signedfalloff = falloff;
    }

    rmt::Vector newpos;
    newpos.y = 0.0f; 

    for( int z = zstart; z < zstart + zsize; z++ )
    {
        for( int x = xstart; x < xstart + xsize; x++ )
        {
            //
            // Convert the location in field-space to worldspace
            //
            newpos.x = ( x - xpos ) * mXscale;
            newpos.z = ( z - zpos ) * mZscale;

            //
            // Only do the sqrt when necessary
            //
            float dist = newpos.MagnitudeSqr();

            if( dist <= disttozerosqr )
            {
                dist = rmt::Sqrt( dist );

                float newvalue;
                if( dist > radius )
                {
                    newvalue = value - (dist - radius) * signedfalloff;
                }
                else
                {
                    newvalue = value;
                }

                //
                // Accumulate potential, or else new potentials cancel out the old
                // ones
                //
                float oldvalue = GetPotential( x, z );

                SetPotential( x, z, oldvalue + newvalue );
            }
        }
    }

    return true;
}

//=============================================================================
// PotentialField::FindBestPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector& pos )
//
// Return:      bool 
//
//=============================================================================
bool PotentialField::FindBestPosition( rmt::Vector& pos, const float targetdist )
{
    bool bFound = false;    
    float bestvalue = -100.0f;
    int bestx = 0;
    int besty = 0;

    rmt::Vector tmp = mZaxis;
    float d = targetdist - Z_ADJUST;
    if( d < 0.0f )
    {
        d = 0.0f;
    }
    tmp.Scale( d, d, d );
    tmp.Add( mOrigin );

    int tx, y;
    PosToIndex( tmp, tx, y );

    for( int x = 0; x < Potentials::MAX_POTENTIALS; x++ )
    {
        float value = mPotentials[ y ].GetPotential( x );
        if( value > bestvalue )
        {
            bFound = true;

            bestvalue = value;
            bestx = x;
            besty = y;
        }
    }

    if( bFound )
    {
        IndexToPos( bestx, besty, pos );
    }

    return bFound;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
