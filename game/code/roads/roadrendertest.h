//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        roadrendertest.h
//
// Description: Blahblahblah
//
// History:     27/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ROADRENDERTEST_H
#define ROADRENDERTEST_H

#ifndef RAD_RELEASE
//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>
#include <roads/roadsegment.h>
#include <render/Culling/ReserveArray.h>

#include <p3d/drawable.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RoadRenderTest : public tDrawable
{
public:
    RoadRenderTest();
    virtual ~RoadRenderTest();

    void Display();
    void DisplaySpawnSegments();
    void DisplayTerrainType( void );

    ReserveArray<RoadSegment*> mSegments;

private:

    bool mDisplay;
    bool mDisplaySpawnSegments;
    bool mDisplayTerrainTypes;

    //Prevent wasteful constructor creation.
    RoadRenderTest( const RoadRenderTest& roadrendertest );
    RoadRenderTest& operator=( const RoadRenderTest& roadrendertest );
};

#endif

#endif //ROADRENDERTEST_H
