#include <roads/intersection.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/lane.h>
#include <p3d/camera.hpp>
#include <p3d/shader.hpp>
#include <p3d/utility.hpp>
#include <p3d/view.hpp>


//#include "../../debugtools/debuginfo.h"

// TODO: remove ugly hack for shader.
//
#include <roads/roadmanager.h>

tShader* gpShader = NULL; //new tShader("simple");

tShader* GetShader()
{
    if ( !gpShader )
    {
        gpShader = new tShader("simple");
    }

    return gpShader;
}


void DrawCircle( float radius, const rmt::Vector& center, int subDivisions, const tColour& colour )
{
    rmt::Vector a,b;
    int i;
    int n = subDivisions;
    float r = radius;
    pddiPrimStream* stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, n * 2 );
    if ( stream )
    {
        for( i = 0; i < n; i++ )
        {
            a.x = center.x + r * rmt::Cos( i * rmt::PI_2 / n );
            a.y = center.y;
            a.z = center.z + r * rmt::Sin( i * rmt::PI_2 / n );
            b.x = center.x + r * rmt::Cos( ( i + 1 ) * rmt::PI_2 / n );
            b.y = center.y;
            b.z = center.z + r * rmt::Sin( ( i + 1 ) * rmt::PI_2 / n );

            stream->Vertex( ( ( pddiVector* )( &( a ) ) ),  colour );
            stream->Vertex( ( ( pddiVector* )( &( b ) ) ),  colour );
        }
    }
    p3d::pddi->EndPrims( stream );
    
}

void DrawBox( rmt::Box3D& box, const tColour& colour )
{
    // Render bounding box.
    //
    pddiPrimStream* stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, 2 );
    if ( stream )
    {
        rmt::Vector start;
        rmt::Vector end;
        start.y = 0.0f;
        end.y = 0.0f;

        start.x = box.high.x;
        start.z = box.high.z;
        end.x = box.high.x;
        end.z = box.low.z;
        stream->Vertex( ( ( pddiVector* )( &( start ) ) ),  colour );
        stream->Vertex( ( ( pddiVector* )( &( end ) ) ),  colour ); 

        start = end;
        end.x = box.low.x;
        end.z = box.low.z;
        stream->Vertex( ( ( pddiVector* )( &( start ) ) ),  colour );
        stream->Vertex( ( ( pddiVector* )( &( end ) ) ),  colour ); 

        start = end;
        end.x = box.low.x;
        end.z = box.high.z;
        stream->Vertex( ( ( pddiVector* )( &( start ) ) ),  colour );
        stream->Vertex( ( ( pddiVector* )( &( end ) ) ),  colour ); 

        start = end;
        end.x = box.high.x;
        end.z = box.high.z;

        stream->Vertex( ( ( pddiVector* )( &( start ) ) ),  colour );
        stream->Vertex( ( ( pddiVector* )( &( end ) ) ),  colour ); 
    }
    p3d::pddi->EndPrims( stream );
}


/*
==============================================================================
Intersection::Render
==============================================================================
Description:    Draw a circle representing the Intersection.
                Call the road Render routine for each road.

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Intersection::Render( void ) const
{
    // Draw a circle representing the Intersection.
    //
    tColour intersectionColour;
    if( mType == Intersection::N_WAY )
    {
        intersectionColour.Set( 0, 255, 0 );
    }
    else 
    {
        intersectionColour.Set( 0, 0, 255 );
    }

    rmt::Vector center;
    GetLocation( center );

    if ( p3d::context->GetView()->GetCamera()->SphereVisible( center, 15.0f ) )
    {
        center.y += 1.0f;
        DrawCircle( GetRadius( ), center, 8, intersectionColour ); 
    }

    unsigned int i = 0;
    for ( i = 0; i < this->mnRoadsIn; i++ )
    {
        if ( mRoadListIn[ i ] )
        {
            mRoadListIn[ i ]->Render( tColour(255, 255, 255) );
        }
    }
    for ( i = 0; i < this->mnRoadsOut; i++ )
    {
        if ( mRoadListOut[ i ] )
        {
            mRoadListOut[ i ]->Render( tColour(0, 0, 0) );
        }
    }
}
/*
==============================================================================
Road::Render
==============================================================================
Description:    Draw a line along the inside edge of the road.
                Call render for each roadsegment.
                Call Render for each Lane.

Parameters:     (  const tColour& colour  )

Return:         void 

=============================================================================
*/
void Road::Render(  const tColour& colour  )
{
    static tColour roadColour = colour;

    if ( p3d::context->GetView()->GetCamera()->SphereVisible( mSphere.centre, mSphere.radius ) )
    {
        static bool bDrawRoads = true;
        if ( bDrawRoads )
        {
            rmt::Vector start;
            this->GetDestinationIntersection( )->GetLocation( start );
            rmt::Vector end;
            this->GetSourceIntersection( )->GetLocation( end );

#ifdef TOOLS
            DrawCircle( GetDestinationIntersection( )->GetRadius( ), start, 8, colour ); 
            DrawCircle( GetSourceIntersection( )->GetRadius( ), end, 8, colour ); 

#endif
            // Render the roads.
            //

            // Draw a line from the start to the end.
            //
            pddiPrimStream* stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, 2 );
            if ( stream )
            {
                stream->Vertex( ( ( pddiVector* )( &( start ) ) ),  roadColour );
                stream->Vertex( ( ( pddiVector* )( &( end ) ) ),  roadColour );  
            }
            p3d::pddi->EndPrims( stream );
        }
        static bool bDrawRoadBBox = false;
//        if ( BEGIN_DEBUGINFO_SECTION( "Draw Road BBox" ) )
//        {
//            ::DrawBox( mBox, roadColour );
//        }
//        END_DEBUGINFO_SECTION( "Draw Road BBox" );
        static bool bDrawRoadBSphere = false;
//        if ( BEGIN_DEBUGINFO_SECTION( "Draw Road BSphere" ) )
//        {
//            // Render the roads bounding sphere.
//            //
//            ::DrawCircle( mSphere.radius, mSphere.centre, 8, roadColour );
//        }
//        END_DEBUGINFO_SECTION( "Draw Road BSphere" );
        // Render the road Segments.
        //
        static bool bDrawRoadSegments = true;
        if ( bDrawRoadSegments )
        {
            // Iterate through the segments.
            //
            unsigned int i = 0;

            unsigned int colourStep = 255 / mnRoadSegments;

            for ( i = 0; i < mnRoadSegments; i++ )
            {
                tColour colour = roadColour;
                colour.SetRed( colourStep * i );
                colour.SetGreen( colourStep * i );
                colour.SetBlue( colourStep * i );
                mppRoadSegmentArray[ i ]->Render( colour );
            }
        }
	    
        // Render the lanes.
        //
        static bool bDrawLanes = true;
        if ( bDrawLanes )
        {
            unsigned int i = 0;
            for ( i = 0; i < mnLanes; i++ )
            {
                this->mLaneList[ i ].Render( );
            }
        }
    }
}
/*
==============================================================================
RoadSegment::Render
==============================================================================
Description:    Outline the rectangular road segment.

Parameters:     ( const tColour& colour )

Return:         void 

=============================================================================
*/
void RoadSegment::Render( const tColour& colour )
{
    tColour roadSegmentColour = colour;
   
    rmt::Sphere sphere;
    GetBoundingSphere( &sphere );


    if ( p3d::context->GetView()->GetCamera()->SphereVisible( sphere.centre, sphere.radius ) )
    {       
        const int numPoints = 4;
        rmt::Vector vertices[ numPoints ];
        int i = 0;
        for ( i = 0; i < numPoints; i++ )
        {
            GetCorner( i, vertices[ i ] );

            vertices[i].y += 1.0f;
        }

        pddiPrimStream* stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, numPoints * 2 );
    
  
        for ( i = 0; i < numPoints; i++ )
        {
            if ( stream )
            {
                stream->Vertex( ( ( pddiVector* )( &( vertices[ i ] ) ) ),  roadSegmentColour );
                stream->Vertex( ( ( pddiVector* )( &( vertices[ ( i + 1 ) % numPoints ] ) ) ),  roadSegmentColour );
            }
        }
        p3d::pddi->EndPrims( stream );

        stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, GetNumLanes() * 2 );
      
        unsigned int index;
        for ( index = 0; index < GetNumLanes(); index++ )
        {
            if ( stream )
            {
                rmt::Vector pos, facing;
                GetLaneLocation(0.0f, index, pos, facing);
                pos.y += 1.0f;
                stream->Vertex( ( ( pddiVector* )( &( pos ) ) ), tColour( 255, 0, 0 ) );

                GetLaneLocation(1.0f, index, pos, facing);
                pos.y += 1.0f;
                stream->Vertex( ( ( pddiVector* )( &( pos ) ) ), tColour( 255, 0, 0 ) );
            }
        }
        p3d::pddi->EndPrims( stream );
    }
}


void RoadSegment::RenderAnywhere( const tColour& colour )
{
    tColour roadSegmentColour = colour;
   
    rmt::Sphere sphere;
    GetBoundingSphere( &sphere );

    const int numPoints = 4;
    rmt::Vector vertices[ numPoints ];
    int i = 0;
    for ( i = 0; i < numPoints; i++ )
    {
        GetCorner( i, vertices[ i ] );

        vertices[i].y += 1.0f;
    }

    pddiPrimStream* stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, numPoints * 2 );


    for ( i = 0; i < numPoints; i++ )
    {
        if ( stream )
        {
            stream->Vertex( ( ( pddiVector* )( &( vertices[ i ] ) ) ),  roadSegmentColour );
            stream->Vertex( ( ( pddiVector* )( &( vertices[ ( i + 1 ) % numPoints ] ) ) ),  roadSegmentColour );
        }
    }
    p3d::pddi->EndPrims( stream );

    stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINES, PDDI_V_C, GetNumLanes() * 2 );
  
    unsigned int index;
    for ( index = 0; index < GetNumLanes(); index++ )
    {
        if ( stream )
        {
            rmt::Vector pos, facing;
            GetLaneLocation(0.0f, index, pos, facing);
            pos.y += 1.0f;
            stream->Vertex( ( ( pddiVector* )( &( pos ) ) ), tColour( 255, 0, 0 ) );

            GetLaneLocation(1.0f, index, pos, facing);
            pos.y += 1.0f;
            stream->Vertex( ( ( pddiVector* )( &( pos ) ) ), tColour( 255, 0, 0 ) );
        }
    }
    p3d::pddi->EndPrims( stream );

}

/*
==============================================================================
Lane::Render
==============================================================================
Description:    Draw a line connecting the lane points.

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Lane::Render( void )
{
#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    static tColour laneColour( 255, 0, 0 );

    rmt::Vector start, end; 
    GetStart( start );
    GetEnd( end );

    int numPoints = this->GetNumPoints( );
    // Draw a line from the start to the end.
    //
    pddiPrimStream* stream = p3d::pddi->BeginPrims( GetShader()->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C, numPoints );
    
    int i = 0;
    for ( i = 0; i < numPoints; i++ )
    {
        if ( stream )
        {
            rmt::Vector point;
            GetPoint( i, &point );
            point.y += 1.0f;
            stream->Vertex( ( ( pddiVector* )( &( point ) ) ),  laneColour );
        }
    }
    p3d::pddi->EndPrims( stream );
#endif
}
