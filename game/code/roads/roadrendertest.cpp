//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        RoadRenderTest.cpp
//
// Description: Implement RoadRenderTest
//
// History:     27/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef RAD_RELEASE

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radmath/radmath.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <roads/RoadRenderTest.h>
#include <roads/roadmanager.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/intersection.h>
#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/culling/spatialtree.h>
#include <render/culling/worldscene.h>
#include <render/dsg/intersectdsg.h>
#include <contexts/bootupcontext.h>
#include <debug/profiler.h>

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
// RoadRenderTest::RoadRenderTest
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadRenderTest::RoadRenderTest() :
    mDisplay( false ),
    mDisplaySpawnSegments( false ),
    mDisplayTerrainTypes( false )
{
    radDbgWatchAddBoolean( &mDisplay, "Display", "Roads" );
    radDbgWatchAddBoolean( &mDisplaySpawnSegments, "Display Spawn Segments", "Roads" );
    radDbgWatchAddBoolean( &mDisplayTerrainTypes, "Display Terrain Types", "Roads" );
}

//==============================================================================
// RoadRenderTest::~RoadRenderTest
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadRenderTest::~RoadRenderTest()
{
    radDbgWatchDelete( &mDisplay );
    radDbgWatchDelete( &mDisplaySpawnSegments );
    radDbgWatchDelete( &mDisplayTerrainTypes );
}

//=============================================================================
// RoadRenderTest::Display
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RoadRenderTest::Display()
{
    DisplaySpawnSegments();
    DisplayTerrainType();
    if ( !mDisplay )
    {
        return;
    }
    
    BEGIN_PROFILE("RoadRenderTest");

    Avatar* a = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( a );

    rmt::Vector position;
    a->GetPosition( position );
    
    RoadManager* rm = RoadManager::GetInstance();

    const Road* road = NULL;
    RoadSegment* irs = NULL;
    int index = -1;
    float in = 0;
    float lateral = 0;

    if ( rm->FindRoad( position, &road, &irs, index, in, lateral, true ) )
    {
        irs->Render( tColour( 255, 255, 255 ) );
    }
    else
    {
        Intersection* intersection = rm->FindIntersection( position );
        if ( intersection )
        {
            intersection->Render();
        }
    }

    END_PROFILE("RoadRenderTest");
}

void RoadRenderTest::DisplaySpawnSegments()
{
    if ( !mDisplaySpawnSegments )
    {
        return;
    }

    if ( !mSegments.IsSetUp() )
    {
        return;
    }

    BEGIN_PROFILE("RoadRenderTest");


    int i;
    for( i=0; i<mSegments.mUseSize; i++ )
    {
        RoadSegment* segment;

        segment = mSegments.mpData[i];
        rAssert( segment != NULL );

        segment->RenderAnywhere( tColour( 255, 255, 255 ) );
    }


    END_PROFILE("RoadRenderTest");
}

void RoadRenderTest::DisplayTerrainType( void )
{
    if( !mDisplayTerrainTypes )
    {
        return;
    }

    tColour terrainColours[ 9 ];
    terrainColours[ 0 ].Set( 128, 128, 128 );   // Road.
    terrainColours[ 1 ].Set( 0, 255, 0 );       // Grass.
    terrainColours[ 2 ].Set( 255, 255, 0 );     // Sand.
    terrainColours[ 3 ].Set( 32, 32, 32 );      // Gravel.
    terrainColours[ 4 ].Set( 0, 0, 255 );       // Water.
    terrainColours[ 5 ].Set( 250, 200, 150 );   // Wood.
    terrainColours[ 6 ].Set( 200, 225, 250 );   // Metal.
    terrainColours[ 7 ].Set( 64, 48, 32 );      // Dirt.
    terrainColours[ 8 ].Set( 255, 0, 255 );     // Unknown.

    Avatar* a = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( a );

    rmt::Vector position;
    a->GetPosition( position );
    SpatialNode& rCurrentLeaf = GetRenderManager()->pWorldScene()->mStaticTreeWalker.rSeekLeaf( (Vector3f&)position );

    pddiShader* testShader = BootupContext::GetInstance()->GetSharedShader();
    testShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_NONE );
    testShader->SetInt( PDDI_SP_ISLIT, 0 );
    testShader->SetInt( PDDI_SP_ALPHATEST, 0 );
    testShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT ); //PDDI_SHADE_GOURAUD
    testShader->SetInt( PDDI_SP_TWOSIDED, 1 );

    for( int i = rCurrentLeaf.mIntersectElems.mUseSize - 1; i > -1; --i )
    {
        for( int j = rCurrentLeaf.mIntersectElems[ i ]->nTris() - 1; j > -1; --j )
        {
            //rmt::Vector tmpVect, tmpVect2;
            //float DistToPlane, ClosestDistToPlane = 20000.0f;
            rmt::Vector triPts[ 3 ];
            rmt::Vector triNorm;
            rmt::Vector triCtr;
            //float triRadius;
            int terrainType;
            bool interior;
            //triRadius = rCurrentLeaf.mIntersectElems[ i ]->mTri( j, triPts, triNorm, triCtr, &terrainType);
			terrainType = rCurrentLeaf.mIntersectElems[ i ]->mTri( j, triPts, triNorm );
            interior = ( terrainType & 0x80 ) != 0;
			terrainType &= ~0x80;
            int colourIndex = rmt::Clamp( terrainType, 0, 8 );
            tColour lineColour( terrainColours[ colourIndex ] );
            if( interior )
            {
                lineColour.Set( lineColour.Red() >> 1, lineColour.Green() >> 1, lineColour.Blue() >> 1 );
            }
            tColour fillColour;
            fillColour.Set( lineColour.Red() >> 1, lineColour.Green() >> 1, lineColour.Blue() >> 1 );

            rmt::Vector center( triPts[ 0 ] );
            center.Add( triPts[ 1 ] );
            center.Add( triPts[ 2 ] );
            center.Scale( 1.0f / 3.0f );
            rmt::Vector toCenter;
            triNorm.Scale( 0.1f );

            for( int k = 0; k < 3; ++k )
            {
                toCenter.Sub( center, triPts[ k ] );
                toCenter.Normalize();
                toCenter.Scale( 0.1f );
                triPts[ k ].Add( toCenter );
                triPts[ k ].Add( triNorm );
            }

            pddiPrimStream* test = p3d::pddi->BeginPrims( testShader, PDDI_PRIM_TRIANGLES, PDDI_V_C, 3 );
            test->Colour( fillColour );
            test->Coord( triPts[ 0 ].x, triPts[ 0 ].y, triPts[ 0 ].z );
            test->Colour( fillColour );
            test->Coord( triPts[ 1 ].x, triPts[ 1 ].y, triPts[ 1 ].z );
            test->Colour( fillColour );
            test->Coord( triPts[ 2 ].x, triPts[ 2 ].y, triPts[ 2 ].z );
            p3d::pddi->EndPrims( test );
            test = p3d::pddi->BeginPrims( testShader, PDDI_PRIM_LINESTRIP, PDDI_V_C, 4 );
            test->Colour( lineColour );
            test->Coord( triPts[ 0 ].x, triPts[ 0 ].y, triPts[ 0 ].z );
            test->Colour( lineColour );
            test->Coord( triPts[ 1 ].x, triPts[ 1 ].y, triPts[ 1 ].z );
            test->Colour( lineColour );
            test->Coord( triPts[ 2 ].x, triPts[ 2 ].y, triPts[ 2 ].z );
            test->Colour( lineColour );
            test->Coord( triPts[ 0 ].x, triPts[ 0 ].y, triPts[ 0 ].z );
            p3d::pddi->EndPrims( test );
        }
    }
    testShader->SetInt( PDDI_SP_TWOSIDED, 0 );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

#endif