/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   RoadSegment

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2002/02/25      Tbrown-John Created

===========================================================================*/

#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/roadsegmentdata.h>

RoadSegment::RoadSegment()
:
mRoad( NULL ),
mSegmentIndex( 0 )
{
}

RoadSegment::~RoadSegment( void )
{
}

/*
void RoadSegment::Init( RoadSegmentData* rsd, rmt::Matrix& hierarchy, float scaleAlongFacing )
{
    ////////////////////////////////////////////////////////////////
    // Transform segment data based on given matrix & scale-along-facing
    //
    rmt::Vector vector;

    // First, do the corners and the edgenormals
    for( int i=0; i<4; i++ )
    {
        // transform the corner
        vector = rsd->GetCorner( i );
        vector.z *= scaleAlongFacing;
        vector.Transform( hierarchy );
        mCorners[ i ] = vector;

        // transform the edge normals
        vector = rsd->GetEdgeNormal( i );
        vector.Rotate( hierarchy );
        mEdgeNormals[ i ] = vector;
    }

    // Now, transform the segment normal
    vector = rsd->GetSegmentNormal();
    vector.Rotate( hierarchy );
    mNormal = vector;

    // Now, calculate and store segment length
    rmt::Vector segStart = (mCorners[0] + mCorners[3]) * 0.5f;
    rmt::Vector segEnd = (mCorners[1] + mCorners[2]) * 0.5f;
    mfSegmentLength = (segEnd - segStart).Length(); // *** SQUARE ROOT! *** 

    // Now, calculate and store the bounding sphere
    rmt::Box3D box;
    GetBoundingBox( &box ); // find the box on the fly (based on extents of corners)

    // compute & store the bounding sphere based on bbox
    rmt::Vector vectorBetween;
    vectorBetween = ( box.high - box.low ) * 0.5f;
    mSphere.centre = box.low + vectorBetween;
    mSphere.radius = vectorBetween.Magnitude(); // *** SQUARE ROOT! ***



    //////////////////////////////
    // TODO: 
    // This stuff is dubious. It won't be accurate given that we can no 
    // longer assume interior and exterior edges are parallel.
    //
    // Now, Calculate the width of the leading edge of the segment.
    float fWidth = segStart.Magnitude();
    mfLaneWidth = fWidth / (float)rsd->GetNumLanes();

    // Calculate a turn radius.
    //
    float fCosTheta = mEdgeNormals[0].DotProduct( mEdgeNormals[1] );
    if ( fCosTheta < 0.0f )
    {
        fCosTheta = 0.0f - fCosTheta;
    }
    if ( fCosTheta < 0.001f ) //Clamp me.
    {
        fCosTheta = 0.0f;
    }

    rmt::Vector temp;
    temp.Sub( mCorners[0], mCorners[1] );
    float fInteriorEdgeLength = temp.Magnitude( );
    temp.Sub( mCorners[2], mCorners[3] );
    float fExteriorEdgeLength = temp.Magnitude( );

    if ( fCosTheta != 0.0f )
    {
        // take the shortest length.
        float length = ( fInteriorEdgeLength < fExteriorEdgeLength )? fInteriorEdgeLength : fExteriorEdgeLength;
        length = length / 2.0f;
        mfRadius = length / fCosTheta;
        mfAngle = rmt::PI_BY2 - rmt::ACos( fCosTheta );
        //rmt::RadianToDeg( mfAngle );
    }
    else
    {
        // Not a curved segment.
        //
        mfRadius = 0.0f;
        mfAngle = 0.0f;
    }
}
*/
void RoadSegment::Init( RoadSegmentData* rsd, rmt::Matrix& hierarchy, float scaleAlongFacing )
{
    ////////////////////////////////////////////////////////////////
    // Transform segment data based on given matrix & scale-along-facing
    //
    rmt::Vector vector;

    // store the unmodified values
    for( int i=0; i<4; i++ )
    {
        vector = rsd->GetCorner( i );
        mCorners[ i ] = vector;

        vector = rsd->GetEdgeNormal( i );
        mEdgeNormals[ i ] = vector;
    }


    //////////////////////////////
    // TODO: 
    // This stuff is dubious. It won't be accurate given that we can no 
    // longer assume interior and exterior edges are parallel. AND
    // somehow it's able to work quite accurately from the corner 
    // and edgenormal values that have not yet been transformed. *shudder*
    //
    // Now, Calculate the width of the leading edge of the segment.
    float fWidth = ((mCorners[0] + mCorners[3]) * 0.5f).Magnitude();
    mfLaneWidth = fWidth / (float)rsd->GetNumLanes();

    // Calculate a turn radius.
    //
    float fCosTheta = mEdgeNormals[0].DotProduct( mEdgeNormals[1] );
    if ( fCosTheta < 0.0f )
    {
        fCosTheta = 0.0f - fCosTheta;
    }
    if ( fCosTheta < 0.001f ) //Clamp me.
    {
        fCosTheta = 0.0f;
    }

    rmt::Vector temp;
    temp.Sub( mCorners[0], mCorners[1] );
    float fInteriorEdgeLength = temp.Magnitude( );
    temp.Sub( mCorners[2], mCorners[3] );
    float fExteriorEdgeLength = temp.Magnitude( );

    if ( fCosTheta != 0.0f )
    {
        // take the shortest length.
        float length = ( fInteriorEdgeLength < fExteriorEdgeLength )? fInteriorEdgeLength : fExteriorEdgeLength;
        length = length / 2.0f;
        mfRadius = length / fCosTheta;
        mfAngle = rmt::PI_BY2 - rmt::ACos( fCosTheta );
        //rmt::RadianToDeg( mfAngle );
    }
    else
    {
        // Not a curved segment.
        //
        mfRadius = 0.0f;
        mfAngle = 0.0f;
    }
    ///////////////////////////////////////////



    // Ok, so first, transform the corners and the edgenormals
    for( int i=0; i<4; i++ )
    {
        // transform the corner
        vector = rsd->GetCorner( i );
        vector.z *= scaleAlongFacing;
        vector.Transform( hierarchy );
        mCorners[ i ] = vector;

        // transform the edge normals
        vector = rsd->GetEdgeNormal( i );
        vector.Rotate( hierarchy );
        mEdgeNormals[ i ] = vector;
    }

    // Now, transform the segment normal
    vector = rsd->GetSegmentNormal();
    vector.Rotate( hierarchy );
    mNormal = vector;

    // Now, calculate and store segment length
    rmt::Vector segStart = (mCorners[0] + mCorners[3]) * 0.5f;
    rmt::Vector segEnd = (mCorners[1] + mCorners[2]) * 0.5f;
    mfSegmentLength = (segEnd - segStart).Length(); // *** SQUARE ROOT! *** 

    // Now, calculate and store the bounding sphere
    rmt::Box3D box;
    GetBoundingBox( &box ); // find the box on the fly (based on extents of corners)

    // compute & store the bounding sphere based on bbox
    rmt::Vector vectorBetween;
    vectorBetween = ( box.high - box.low ) * 0.5f;
    mSphere.centre = box.low + vectorBetween;
    mSphere.radius = vectorBetween.Magnitude(); // *** SQUARE ROOT! ***


}


void RoadSegment::GetBoundingBox(rmt::Box3D* box)
{
    // get axis-aligned bounding box from vertices...
    unsigned int numVertices = 4;
    rmt::Vector vertex;
    unsigned int i = 0;
    for ( i = 0; i < numVertices; i++ )
    {
        vertex = mCorners[i];
        if ( 0 == i )
        {
            // This is the first time.
            // Initialize to some value.
            //
            box->low = box->high = vertex;
        }
        else
        {
            if ( box->low.x > vertex.x )
            {
                box->low.x = vertex.x;
            }
            if ( box->low.y > vertex.y )
            {
                box->low.y = vertex.y;
            }
            if ( box->low.z > vertex.z )
            {
                box->low.z = vertex.z;
            }

            if ( box->high.x < vertex.x )
            {
                box->high.x = vertex.x;
            }
            if ( box->high.y < vertex.y )
            {
                box->high.y = vertex.y;
            }
            if ( box->high.z < vertex.z )
            {
                box->high.z = vertex.z;
            }
        }
    }
}    

void RoadSegment::GetBoundingSphere(rmt::Sphere* sphere)
{
    *sphere = mSphere;
}




/*
==============================================================================
RoadSegment::CalculateUnitDistIntoRoadSegment
==============================================================================
Description:    Adapted from GameGems Article by Steven Ranck. pp412-pp420.
                Implements a fast and simple algm for determing where a point
                in between the edges of a 2D quad (RoadSegment).  The result 
                is a unit floting point number, where 0 indicates that the point
                lies on the leading edge, and where 1 indicates that the point
                lies on the opposite edge.  The RoadSegment may be any 4 sided
                2D convex shape.

Constraints:    The RoadSegment must be convex and have 4 sides.
                The RoadSegment must have a non zero area.
                The point must lie within the sector. *****  What if it doesn't?

Parameters:     ( float fPointX, float fPointZ )

Return:         A scalar from 0 to 1.
                0 if point lies on the leading edge.
                1 if point lies on the trailing edge.
                Smoothly interpolated value for all points in between.

=============================================================================
*/
float RoadSegment::CalculateUnitDistIntoRoadSegment( float fPointX, float fPointZ )
{
	rmt::Vector VLP, VTP;
	float fDotL, fDotT;

    // Get and cache the leading edge top corner
    // and the trailing edge bottom corner.
    //
    rmt::Vector vertices[ 2 ];
    GetCorner( 0, vertices[ 0 ] );
    GetCorner( 2, vertices[ 1 ] );

    // Get and cache the leading edge normal
    // and the trailing edge normal.
    //
    rmt::Vector unitNormals[ 2 ];
    GetEdgeNormal( 0, unitNormals[ 0 ] );
    GetEdgeNormal( 2, unitNormals[ 1 ] );

    //for this to work, the normals must both point into
    //the volume...
    //so, I need to reverse the second edge normal
    unitNormals[1] *= -1;

	// Compute vector from point on Leading Edge to P:
	//
	VLP.x = fPointX - vertices[0].x;
	VLP.y = 0.0f;
	VLP.z = fPointZ - vertices[0].z;

	// Compute vector from point on Trailing Edge to P:
	//
	VTP.x = fPointX - vertices[1].x;
	VTP.y = 0.0f;
	VTP.z = fPointZ - vertices[1].z;

	// Compute (VLP dot Leading Edge Normal):
	//
	fDotL = VLP.x*unitNormals[0].x + VLP.z*unitNormals[0].z;

	// Compute (VTP dot Trailing Edge Normal):
	//
	fDotT = VTP.x*unitNormals[1].x + VTP.z*unitNormals[1].z;

	// Compute unit distance into sector and return it:
	//
	return ( fDotL / (fDotL + fDotT) );
}

float RoadSegment::CalculateUnitHeightInRoadSegment( float fPointX, float fPointZ )
{
	rmt::Vector VLP, VTP;
	float fDotL, fDotT;

    // Get and cache the leading edge top corner
    // and the trailing edge bottom corner.
    //
    rmt::Vector vertices[ 2 ];
    GetCorner( 0, vertices[ 0 ] );
    GetCorner( 2, vertices[ 1 ] );

    // Get and cache the leading edge normal
    // and the trailing edge normal.
    //
    rmt::Vector unitNormals[ 2 ];
    GetEdgeNormal( 1, unitNormals[ 0 ] );
    GetEdgeNormal( 3, unitNormals[ 1 ] );

	// Compute vector from point on Leading Edge to P:
	//
	VLP.x = fPointX - vertices[0].x;
	VLP.y = 0.0f;
	VLP.z = fPointZ - vertices[0].z;

	// Compute vector from point on Trailing Edge to P:
	//
	VTP.x = fPointX - vertices[1].x;
	VTP.y = 0.0f;
	VTP.z = fPointZ - vertices[1].z;

	// Compute (VLP dot Leading Edge Normal):
	//
	fDotL = VLP.x*unitNormals[0].x + VLP.z*unitNormals[0].z;

	// Compute (VTP dot Trailing Edge Normal):
	//
	fDotT = VTP.x*unitNormals[1].x + VTP.z*unitNormals[1].z;

	// Compute unit distance into sector and return it:
	//
	return ( fDotL / (fDotL + fDotT) );
}


/*
float RoadSegment::CalculateYHeight( float fPointX, float fPointZ )
{
    // Get and cache the leading edge top corner
    // and the trailing edge bottom corner.
    //
    rmt::Vector vertices[ 2 ];
    GetCorner( 0, vertices[ 0 ] );
    GetCorner( 2, vertices[ 1 ] );

	float fDistance = CalculateUnitDistIntoRoadSegment( fPointX, fPointZ );
	float y = LERP( fDistance, vertices[ 0 ].y, vertices[ 1 ].y );
	return y;
}
*/

void RoadSegment::GetPosition( float t, float w, rmt::Vector* pos )
{
    // Get and cache the corners.
    //
    rmt::Vector vertices[ 4 ];
    GetCorner( 0, vertices[ 0 ] );
    GetCorner( 1, vertices[ 1 ] );
    GetCorner( 2, vertices[ 2 ] );
    GetCorner( 3, vertices[ 3 ] );

    rmt::Vector position;

    // Interpolate the Normal vector across the Segment.
    //
    rmt::Vector leadingEdge;
    leadingEdge.Sub( vertices[ 3 ], vertices[ 0 ] );
    rmt::Vector leadingPoint = leadingEdge;
    leadingPoint.Scale( w );
    leadingPoint.Add( vertices[ 0 ] );

    rmt::Vector trailingEdge;
    trailingEdge.Sub( vertices[ 2 ], vertices[ 1 ] );
    rmt::Vector trailingPoint = trailingEdge;
    trailingPoint.Scale( w );
    trailingPoint.Add( vertices[ 1 ] );

    position.Sub( trailingPoint, leadingPoint );
    position.Scale( t );
    position.Add( leadingPoint );
    *pos = position;
}


void RoadSegment::GetLaneLocation( float t, int index, rmt::Vector& position, rmt::Vector& facing )
{
    //
    // Get the world space point and facing at time 't'.
    //
    // Interpolate the facing.
    //
    rmt::Vector facingNormals[ 2 ];
    GetEdgeNormal( 0, facingNormals[ 0 ] );
    GetEdgeNormal( 2, facingNormals[ 1 ] );
    facing.x = LERP( t, facingNormals[ 0 ].x, facingNormals[ 1 ].x );
    facing.y = LERP( t, facingNormals[ 0 ].y, facingNormals[ 1 ].y );
    facing.z = LERP( t, facingNormals[ 0 ].z, facingNormals[ 1 ].z );

    // [Dusit: July 6th, 2003]
    // NOTE: 
    // This is the CORRECT way to produce the lane length value when
    // the width of the roadsegment isn't guaranteed across its length.
    // The only thing that it assumes (and is always correct) is that
    // each lane is as wide  as the other lanes at any given point along 
    // the length of the segment
    //
    float edgeT = ((float)(index<<1) + 1.0f) / ((float)(GetNumLanes()<<1));

    // find start & end points of the lane
    rmt::Vector vec0, vec1, vec2, vec3;

    GetCorner( 0, vec0 );
    GetCorner( 1, vec1 );
    GetCorner( 2, vec2 );
    GetCorner( 3, vec3 );

    // lane indices go from 0 to n, right to left ( n <=== 0 )
    rmt::Vector bottomEdgeDir = vec0 - vec3; // points frm 3 to 0
    rmt::Vector topEdgeDir = vec1 - vec2; // points frm 2 to 1

    // now we figure out the starting point and ending point of the 
    // lane segment
    rmt::Vector start = vec3 + bottomEdgeDir * edgeT;
    rmt::Vector end = vec2 + topEdgeDir * edgeT;

    // now find the t position along the lane
    rmt::Vector laneDir = end - start;
    position = start + laneDir * t;

    /*

    // Get and cache the corners.
    //
    rmt::Vector vertices[ 4 ];
    GetCorner( 0, vertices[ 0 ] );
    GetCorner( 1, vertices[ 1 ] );
    GetCorner( 2, vertices[ 2 ] );
    GetCorner( 3, vertices[ 3 ] );

    //There is an assumption here that the road does not get wider or thinner 
    //across its length...
    // Scale unnormalized vector by normalized center of desired lane.
    //  ( ( index * fLaneWidth ) + ( fLaneWidth / 2.0f ) ) / roadWidth;
    //
    float fCentreOfLane = ( index * mfLaneWidth ) + ( mfLaneWidth / 2.0f );

    //I call this parametric variable w;
    float w = fCentreOfLane / GetRoadWidth();

    // Interpolate the Normal vector across the Segment.
    //
    rmt::Vector leadingEdge;
    leadingEdge.Sub( vertices[ 0 ], vertices[ 3 ] );
    rmt::Vector leadingPoint = leadingEdge;
    leadingPoint.Scale( w );
    leadingPoint.Add( vertices[ 3 ] );


    rmt::Vector trailingEdge;
    trailingEdge.Sub( vertices[ 1 ], vertices[ 2 ] );
    rmt::Vector trailingPoint = trailingEdge;
    trailingPoint.Scale( w );
    trailingPoint.Add( vertices[ 2 ] );

    //This gives the point between the leading and trailing point by parameter t. 
    position.x = LERP( t, leadingPoint.x, trailingPoint.x );
    position.y = LERP( t, leadingPoint.y, trailingPoint.y );
    position.z = LERP( t, leadingPoint.z, trailingPoint.z );
    */


} 


/*
//==============================================================================
//RoadSegment::GetJoinPoint
//==============================================================================
//Description:    RoadSegmentData pieces are always joined at the left corner
//                of the trailing edge.  The normal of the leading edge of the
//                new piece is always the inverse normal of the trailing edge
//                of the previous piece.
//
//                This function returns the join vertex and facing in world space.
//
//Parameters:     ( rmt::Vector& position, rmt::Vector& facing )
//
//Return:         void 
//
//=============================================================================
void RoadSegment::GetJoinPoint( rmt::Vector& position, rmt::Vector& facing )
{
    // All segments are joined at the left top corner.
    //
    position = GetRoadSegmentData( )->GetCorner( 1 );

    facing = GetRoadSegmentData( )->GetEdgeNormal( 2 );
    facing.Scale( -1.0f );
}
*/

void RoadSegment::GetCorner( int index, rmt::Vector& out )
{
    rAssert( 0 <= index && index < 4 );
    out = mCorners[ index ];
}
void RoadSegment::GetEdgeNormal( int index, rmt::Vector& out )
{
    rAssert( 0 <= index && index < 4 );
    out = mEdgeNormals[ index ];
}
void RoadSegment::GetSegmentNormal( rmt::Vector& out )
{
    out = mNormal;
}
unsigned int RoadSegment::GetNumLanes( void )
{
    return GetRoad()->GetNumLanes();
}
float RoadSegment::GetLaneLength( unsigned int lane )
{
    // [Dusit: July 6th, 2003]
    // NOTE: 
    // This is the CORRECT way to produce the lane length value when
    // the width of the roadsegment isn't guaranteed across its length.
    // The only thing that it assumes (and is always correct) is that
    // each lane is as wide  as the other lanes at any given point along 
    // the length of the segment
    //
    // The problem with using this is that it requires a Sqrt (because
    // it's too late for us to rearrange the data so that we can avoid 
    // this). So we continue using the old way because we're never off 
    // by more than 5 centimeters anyway.
    // 
    float edgeT = ((float)(lane<<1) + 1.0f) / ((float)(GetNumLanes()<<1));

    // find start & end points of the lane
    rmt::Vector vec0, vec1, vec2, vec3;

    GetCorner( 0, vec0 );
    GetCorner( 1, vec1 );
    GetCorner( 2, vec2 );
    GetCorner( 3, vec3 );

    // lane indices go from 0 to n, right to left ( n <=== 0 )
    rmt::Vector bottomEdgeDir = vec0 - vec3; // points frm 3 to 0
    rmt::Vector topEdgeDir = vec1 - vec2; // points frm 2 to 1

    rmt::Vector start = vec3 + bottomEdgeDir * edgeT;
    rmt::Vector end = vec2 + topEdgeDir * edgeT;

    float expectedLength = (end - start).Magnitude();
    return expectedLength;

    /*
    float computedLength = 0.0f;

    // TODO:
    // Because we can't assume that a lane is constant-width, this
    // code is totally bogus... 
    if ( mfAngle > 0.0f )
    {
        // 2*PI*r for a circle.
        // Theta*r for arc of theta degrees.
        //
        float fLaneOffset = mfLaneWidth * lane + mfLaneWidth;
        computedLength = 2.0f * mfAngle * ( mfRadius + fLaneOffset );
    }
    else
    {
        // it's not a curve, both edge lengths will be equal.
        //
        computedLength = mfSegmentLength;
    }

    //rAssert( rmt::Epsilon( computedLength, expectedLength, 0.01f ) );
    return computedLength;
    */
}

float RoadSegment::GetRoadWidth()
{
    return mRoad->GetNumLanes() * mfLaneWidth;
}


/*

/////////////////////////////////////////////////////////////////
// TransformRoadSegment
/////////////////////////////////////////////////////////////////
// 
TransformRoadSegment::TransformRoadSegment() :
    RoadSegment( NULL )
{
    mTransform.Identity();
    mfScaleAlongFacing = 0.0f;
}

TransformRoadSegment::TransformRoadSegment( 
    const RoadSegmentData* pRoadSegmentData, 
    rmt::Matrix& transform ) 
    :
    RoadSegment( pRoadSegmentData )
{
    mTransform = transform;
}

TransformRoadSegment::TransformRoadSegment( 
    const RoadSegmentData* pRoadSegmentData, 
    rmt::Matrix& transform,
    float fScaleAlongFacing ) 
    :
    RoadSegment( pRoadSegmentData ),
    mfScaleAlongFacing( fScaleAlongFacing )
{
    mTransform = transform;
}

TransformRoadSegment::TransformRoadSegment( 
    const RoadSegmentData* pRoadSegmentData, 
    const rmt::Vector& facing, 
    const rmt::Vector& position )
    :
    RoadSegment( pRoadSegmentData )
{
	rmt::Vector sApproximateUp( 0.0f, 1.0f, 0.0f );

    mTransform.Identity( );
    mTransform.FillHeading( facing, sApproximateUp );
    mTransform.FillTranslate( position );
}

TransformRoadSegment::TransformRoadSegment( 
    const RoadSegmentData* pRoadSegmentData, 
    const rmt::Vector& facing, 
    const rmt::Vector& position, 
    float fScaleAlongFacing )
    :
    RoadSegment( pRoadSegmentData ),
    mfScaleAlongFacing( fScaleAlongFacing )
{
	rmt::Vector sApproximateUp( 0.0f, 1.0f, 0.0f );

    mTransform.Identity( );
    mTransform.FillHeading( facing, sApproximateUp );
    mTransform.FillTranslate( position );
}
void TransformRoadSegment::GetCorner( int index, rmt::Vector& out ) const
{
    out = GetRoadSegmentData( )->GetCorner( index );
    out.z *= mfScaleAlongFacing;
    out.Transform( mTransform );
}
void TransformRoadSegment::GetEdgeNormal( int index, rmt::Vector& out ) const
{
    out = GetRoadSegmentData( )->GetEdgeNormal( index );
    out.Rotate( mTransform );
}
void TransformRoadSegment::GetSegmentNormal( rmt::Vector& out ) const
{
    out = GetRoadSegmentData( )->GetSegmentNormal( );
    out.Rotate( mTransform );
}

//==============================================================================
//TransformRoadSegment::GetJoinPoint
//==============================================================================
//Description:    RoadSegmentData pieces are always joined at the left corner
//                of the trailing edge.  The normal of the leading edge of the
//                new piece is always the inverse normal of the trailing edge
//                of the previous piece.
//
//                This function returns the join vertex and facing in world space.
//
//Parameters:     ( rmt::Vector& position, rmt::Vector& facing )
//
//Return:         void 
//
//=============================================================================
void TransformRoadSegment::GetJoinPoint( rmt::Vector& position, rmt::Vector& facing ) const
{
    // All segments are joined at the left top corner.
    //
    facing = GetRoadSegmentData( )->GetEdgeNormal( 2 );
    facing.Scale( -1.0f );
    facing.Rotate( mTransform );
    
    position = GetRoadSegmentData( )->GetCorner( 1 );
    position.z *= mfScaleAlongFacing;
    position.Transform( mTransform );
}
void TransformRoadSegment::GetBoundingBox(rmt::Box3D* box)
{
    GetRoadSegmentData()->GetBoundingBox( *box );
    (*box).high.z *= mfScaleAlongFacing;
    (*box).high.Transform( mTransform );
    (*box).low.Transform( mTransform );
}
void TransformRoadSegment::GetBoundingSphere(rmt::Sphere* sphere)
{
    GetRoadSegmentData( )->GetBoundingSphere( *sphere );
    (*sphere).radius *= mfScaleAlongFacing;
    (*sphere).centre.Transform( mTransform );
}

void TransformRoadSegment::GetBoundingBox( rmt::Box3D& out ) const
{
    GetRoadSegmentData( )->GetBoundingBox( out );
    out.high.z *= mfScaleAlongFacing;
    out.high.Transform( mTransform );
    out.low.Transform( mTransform );
}

void TransformRoadSegment::GetBoundingSphere( rmt::Sphere& out ) const
{
    out = GetRoadSegmentData( )->GetBoundingSphere( );
    out.radius *= mfScaleAlongFacing;
    out.centre.Transform( mTransform );
}

void TransformRoadSegment::GetTransform( rmt::Matrix &out ) const
{
    out = mTransform;
}

*/