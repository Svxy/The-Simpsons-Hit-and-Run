//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        frustrumdrawable.h
//
// Description: Blahblahblah
//
// History:     01/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FRUSTRUMDRAWABLE_H
#define FRUSTRUMDRAWABLE_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>
#include <p3d/debugdraw.hpp>
#include <p3d/drawable.hpp>
#include <p3d/shader.hpp>

#include <camera/supercam.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FrustrumDrawable : public tDrawable
{
public:
    FrustrumDrawable() : mEnable( false ), mSphereScale( 1.0f ), mSuperCam( NULL ) {};
    virtual ~FrustrumDrawable() {};

    void Display();

    void SetPoints( rmt::Vector collPos,
                    rmt::Vector position, 
                    rmt::Vector p0, 
                    rmt::Vector p1, 
                    rmt::Vector p2, 
                    rmt::Vector p3 );

    void SetColour( tColour colour );

    void Enable() { mEnable = true; };
    void Disable() { mEnable = false; };

    void SetScale( float scale ) { mSphereScale = scale; };

    void SetSuperCam( const SuperCam* cam ) { mSuperCam = cam; };

private:
    rmt::Vector mColPos;
    rmt::Vector mPosition;
    rmt::Vector mPoints[ 4 ];

    tColour mColour;

    bool mEnable : 1;

    float mSphereScale;

    const SuperCam* mSuperCam;

    //Prevent wasteful constructor creation.
    FrustrumDrawable( const FrustrumDrawable& frustrumdrawable );
    FrustrumDrawable& operator=( const FrustrumDrawable& frustrumdrawable );
};


//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// FrustrumDrawable::Display
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline void FrustrumDrawable::Display()
{
    if ( mEnable )
    {
        pddiPrimStream* stream;
    
        unsigned int i;
        for ( i = 0; i < 4; ++i )
        {
            stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINES, PDDI_V_C, 2);
            stream->Colour(mColour);
            stream->Coord(mPosition.x, mPosition.y, mPosition.z);
            stream->Colour(mColour);
            stream->Coord(mPoints[i].x, mPoints[i].y, mPoints[i].z);
            p3d::pddi->EndPrims(stream);
        }

        rmt::Vector circlePos = mColPos;

        tShader* shader = new tShader("simple");
        shader->AddRef();

        #ifndef RAD_RELEASE
            P3DDrawSphere( mSphereScale, circlePos, *shader, tColour( 255, 0, 0 ) );
        #endif

        if ( mSuperCam )
        {
            mSuperCam->Display();
        }

        shader->Release();
    }
}

//=============================================================================
// FrustrumDrawable::SetPoints
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector collPos,
//                rmt::Vector position, 
//                rmt::Vector p0, 
//                rmt::Vector p1, 
//                rmt::Vector p2, 
//                rmt::Vector p3 )
//
// Return:      void 
//
//=============================================================================
inline void FrustrumDrawable::SetPoints( rmt::Vector collPos,
                                         rmt::Vector position, 
                                         rmt::Vector p0, 
                                         rmt::Vector p1, 
                                         rmt::Vector p2, 
                                         rmt::Vector p3 )
{
    mColPos = collPos;
    mPosition = position;

    mPoints[ 0 ] = p0;
    mPoints[ 1 ] = p1;
    mPoints[ 2 ] = p2;
    mPoints[ 3 ] = p3;
}

//=============================================================================
// FrustrumDrawable::SetColour
//=============================================================================
// Description: Comment
//
// Parameters:  ( tColour colour )
//
// Return:      void 
//
//=============================================================================
inline void FrustrumDrawable::SetColour( tColour colour )
{
    mColour = colour;
}

#endif //FRUSTRUMDRAWABLE_H
