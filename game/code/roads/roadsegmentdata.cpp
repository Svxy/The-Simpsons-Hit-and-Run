/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   RoadSegment

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2002/02/25      Tbrown-John Created

===========================================================================*/
#include <memory/classsizetracker.h>
#include <roads/roadsegmentdata.h>

// System.
//
#include <raddebug.hpp>

/*

  Input parameters:

 O----------> v0
 |          |
 |          |
 |          |
 |          |
 v          v
 v2         v1

  O = ( 0.0f, 0.0f, 0.0f ) LCS origin.

  # of lanes.

  create this:

                       |
                N1    \ / 
       v0_________________________v1----> facing
        |                         |
  -> N0 |           --> direction | -> N2
        |_________________________|
       v3						  v2
                N3    / \ 
                       |
  N is UP
  N0 is  ->

  N1 is  |
        \ /

  N2 is  ->

  N3 is / \
         |
 */
RoadSegmentData::RoadSegmentData() :
    mnLanes( 0 )
{
    CLASSTRACKER_CREATE( RoadSegmentData );
    unsigned int i;
    for ( i = 0; i < 4; ++i )
    {
        mCorners[i].Set(0.0f, 0.0f, 0.0f);
        mEdgeNormals[i].Set(0.0f, 0.0f, 0.0f);
    }

    mNormal.Set(0.0f, 0.0f, 0.0f);
    
}

RoadSegmentData::RoadSegmentData(
        const rmt::Vector& v0, 
        const rmt::Vector& v1, 
        const rmt::Vector& v2,
        unsigned int nLanes )
{
    CLASSTRACKER_CREATE( RoadSegmentData );
    SetData( v0, v1, v2, nLanes );
}

RoadSegmentData::~RoadSegmentData()
{
    CLASSTRACKER_DESTROY( RoadSegmentData );
}

//=============================================================================
// RoadSegmentData::SetData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& v0, const rmt::Vector& v1, const rmt::Vector& v2, unsigned int nLanes, bool hasShoulder )
//
// Return:      void 
//
//=============================================================================
void RoadSegmentData::SetData( const rmt::Vector& v0, 
                               const rmt::Vector& v1, 
                               const rmt::Vector& v2, 
                               unsigned int nLanes )
{
    mnLanes = nLanes;

    // Calculate the up vector for this piece.
    //
    mNormal.CrossProduct( v0, v2 );
    mNormal.Normalize( );

    rmt::Vector origin( 0.0f, 0.0f, 0.0f );

    mCorners[ 0 ] = origin;
    mCorners[ 1 ] = v0;
    mCorners[ 2 ] = v1;
    mCorners[ 3 ] = v2;

    rmt::Vector temp;

    // Calculate the edge normals.
    temp.Sub( mCorners[ 0 ], mCorners[ 3 ] );
    mEdgeNormals[ 0 ].CrossProduct( mNormal, temp );
    mEdgeNormals[ 0 ].Normalize( );
    
    temp.Sub( mCorners[ 1 ], mCorners[ 0 ] );
    mEdgeNormals[ 1 ].CrossProduct( mNormal, temp );
    mEdgeNormals[ 1 ].Normalize( );

    temp.Sub( mCorners[ 1 ], mCorners[ 2 ] );
    mEdgeNormals[ 2 ].CrossProduct( mNormal, temp );
    mEdgeNormals[ 2 ].Normalize( );

    temp.Sub( mCorners[ 3 ], mCorners[ 2 ] );
    mEdgeNormals[ 3 ].CrossProduct( mNormal, temp );
    mEdgeNormals[ 3 ].Normalize( );


}

/*
==============================================================================
RoadSegmentData::GetCorner
==============================================================================
Description:    Comment


Parameters:     ( int index ), which box corner are we interested in.

Return:         const rmt::Vector& - a copy of the vertex location in local space.

=============================================================================
*/
const rmt::Vector& RoadSegmentData::GetCorner( int index ) const
{
    rAssert( index < 4 );
    return mCorners[ index ];
}
/*
==============================================================================
RoadSegmentData::GetEdgeNormal
==============================================================================
Description:    Return the normal to the edge.  Define CW, starting 
                with leading edge.  v4.Sub( v0 ) = leading edge.

Parameters:     ( int index )

Return:         const 

=============================================================================
*/
const rmt::Vector& RoadSegmentData::GetEdgeNormal( int index ) const
{
    rAssert( index < 4 );
    return mEdgeNormals[ index ];
}

/*
==============================================================================
RoadSegmentData::GetSegmentNormal
==============================================================================
Description:    Return the normal to the planar segment surface.

Parameters:     ( void )

Return:         const rmt::Vector&

=============================================================================
*/
const rmt::Vector& RoadSegmentData::GetSegmentNormal( void ) const
{
    return mNormal;
}

/*
==============================================================================
RoadSegmentData::GetNumLanes
==============================================================================
Description:    Return the number of lanes, on the left side if bIsLeft is true.
                Else, the number of lanes on the right side.

Parameters:     ( void )

Return:         unsigned int, the number of lanes.

=============================================================================
*/
unsigned int RoadSegmentData::GetNumLanes( void ) const
{
    return mnLanes;
}
