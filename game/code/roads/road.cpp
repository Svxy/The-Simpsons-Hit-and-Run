/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Road

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/

#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/lane.h>
#include <roads/geometry.h>
#include <raddebug.hpp>

#ifndef TOOLS
#include <memory/srrmemory.h>
#else
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP()
#endif

/*
==============================================================================
Road::Road
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         na    
 
=============================================================================
*/   
Road::Road( void )
:
mpSourceIntersection( 0 ),
mpDestinationIntersection( 0 ),
mLaneList( 0 ),
mnLanes( 0 ),
mppRoadSegmentArray( 0 ),
mnMaxRoadSegments( 0 ),
mnRoadSegments( 0 ),
mSpeed( 0 ),
mDensity( 0 ),
mDifficulty( 0 ),
mIsShortCut( false )
{
}

/*
==============================================================================
Road::~Road
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         na
 
=============================================================================
*/            
Road::~Road( void )
{
	if ( mLaneList )
	{
		delete [] mLaneList;
		mLaneList = 0;
	}

    if (  mppRoadSegmentArray )
    {
        delete[] mppRoadSegmentArray;
        mppRoadSegmentArray = NULL;
    }
}

/*
==============================================================================
Road::AllocateSegments
==============================================================================
Description:    Comment

Parameters:     ( unsigned int numSegments )

Return:         void 

=============================================================================
*/
void Road::AllocateSegments( unsigned int numSegments )
{
MEMTRACK_PUSH_GROUP( "Road" );
#ifndef TOOLS
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
#endif
#endif

    rAssert( numSegments > 0 );
    rAssert( 0 == mppRoadSegmentArray );
    rAssert( 0 == mnRoadSegments );

    mnMaxRoadSegments = numSegments;
    mppRoadSegmentArray = new RoadSegment*[ mnMaxRoadSegments ];

    unsigned int i = 0;
    for ( i = 0; i < mnMaxRoadSegments; i++ )
    {
        mppRoadSegmentArray[ i ] = 0;
    }

#ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    #endif
#endif
MEMTRACK_POP_GROUP( "Road" );
}

/*
==============================================================================
Road::AddRoadSegment
==============================================================================
Description:    Add a road segment.

Parameters:     ( RoadSegment* pRoadSegment )

Return:         void 

=============================================================================
*/
void Road::AddRoadSegment( RoadSegment* pRoadSegment )
{
    mppRoadSegmentArray[ mnRoadSegments ] = pRoadSegment;

    pRoadSegment->SetSegmentIndex( mnRoadSegments );

    mnRoadSegments++;

    unsigned int numVertices = 4;
    rmt::Vector vertex;
    unsigned int i = 0;
    for ( i = 0; i < numVertices; i++ )
    {
        pRoadSegment->GetCorner( i, vertex );
        if ( 1 == mnRoadSegments && 0 == i )
        {
            // This is the first time.
            // Initialize to some value.
            //
            mBox.low = mBox.high = vertex;
        }
        else
        {
            if ( mBox.low.x > vertex.x )
            {
                mBox.low.x = vertex.x;
            }
            if ( mBox.low.y > vertex.y )
            {
                mBox.low.y = vertex.y;
            }
            if ( mBox.low.z > vertex.z )
            {
                mBox.low.z = vertex.z;
            }

            if ( mBox.high.x < vertex.x )
            {
                mBox.high.x = vertex.x;
            }
            if ( mBox.high.y < vertex.y )
            {
                mBox.high.y = vertex.y;
            }
            if ( mBox.high.z < vertex.z )
            {
                mBox.high.z = vertex.z;
            }
        }
    }
    // now we should have a bounding box.
    //
    // compute the bounding sphere.
    //
    // first the centre.
    //
    rmt::Vector vectorBetween;
    vectorBetween.Sub( mBox.high, mBox.low );
    vectorBetween.Scale( 0.5f );
    mSphere.centre.Add( mBox.low, vectorBetween );
    // Then the radius

    mSphere.radius = vectorBetween.Magnitude( );
}

/*
==============================================================================
Road::CreateLanes
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Road::CreateLanes( void )
{
MEMTRACK_PUSH_GROUP( "Road" );
	// Allocate the lanes.
	//
    //TODO: REMOVE!
    #ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    #endif
    #endif

	mLaneList = new Lane[ mnLanes ];

	// Allocate the memory for the lane points.
	//
    unsigned int totalDensity = GetDensity();
    unsigned int laneDensity = totalDensity / mnLanes;

	unsigned int i = 0;
	for ( i = 0; i < mnLanes; i++ )
	{
        // Do this first.
        //
        // if on last lane, just add whatever's left of totalDensity
        unsigned int density = 0;
        if( i == mnLanes - 1 )
        {
            density = totalDensity;
        }
        else
        {
            density = laneDensity;
        }
        mLaneList[ i ].Create( (int)density, this );
        totalDensity -= laneDensity;
        mLaneList[ i ].SetSpeedLimit( (float)GetSpeed() * KPH_2_MPS ); //convert from kph to mps

#if defined(RAD_DEBUG) || defined(RAD_TUNE)
		// For each lane, allocate enough points to store all the roadSegment data plus one.
		mLaneList[ i ].AllocatePoints( this->GetNumRoadSegments( ) + 1 );
#endif
        
	}
	// Iterate through the segments.
	//
	for ( i = 0; i < mnRoadSegments; i++ )
    {
        RoadSegment* pSeg = mppRoadSegmentArray[ i ];
        unsigned int j = 0;
		for ( j = 0; j < mnLanes; j++ )
		{
			// For each lane in each segment, add a point to the lane.
			//
			rmt::Vector start, facing;
			pSeg->GetLaneLocation( 0.0f, j, start, facing );

        #if defined(RAD_TUNE) || defined(RAD_DEBUG)
			mLaneList[ j ].SetPoint( i, start );
        #endif
		}
	}
    // This is attached to the out intersection.
    //
    RoadSegment* pSeg = mppRoadSegmentArray[ mnRoadSegments - 1 ];
	// Now add the last point.
	//
	for ( i = 0; i < mnLanes; i++ )
	{
		rmt::Vector end, facing;
		pSeg->GetLaneLocation( 1.0f, i, end, facing );
    #if defined(RAD_TUNE) || defined(RAD_DEBUG)
		mLaneList[ i ].SetPoint( mnRoadSegments,  end );
    #endif
	}

    //TODO: REMOVE!
    #ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    #endif
    #endif

MEMTRACK_POP_GROUP( "Road" );
}

/*
==============================================================================
Road::GetLane
==============================================================================
Description:    Comment

Parameters:     ( unsigned int LaneId )

Return:         Lane*

=============================================================================
*/
Lane* Road::GetLane( unsigned int LaneId ) const
{
    if ( LaneId < mnLanes )
	{
		return &mLaneList[ LaneId ];
	}
	else
	{
		return 0;
	}
}
RoadSegment* Road::GetRoadSegment( unsigned int index ) const
{
    rAssert( index < mnRoadSegments );
    
    return mppRoadSegmentArray[ index ];
}

// 
//
/*
==============================================================================
Road::GetDestinationIntersectionJoinPoint
==============================================================================
Description:    Return the point where the road attaches to the incoming intersection.

Parameters:     ( rmt::Vector& out )

Return:         void 

=============================================================================
*/
void Road::GetDestinationIntersectionJoinPoint( rmt::Vector& out )
{
    RoadSegment* pRoadSegment = this->GetRoadSegment( 0 );

    pRoadSegment->GetCorner( 0, out );
}
/*
==============================================================================
Road::GetSourceIntersectionJoinPoint
==============================================================================
Description:    Return the point where the road attaches to the outgoing intersection.

Parameters:     ( rmt::Vector& out )

Return:         void 

=============================================================================
*/
void Road::GetSourceIntersectionJoinPoint( rmt::Vector& out )
{
    RoadSegment* pRoadSegment = this->GetRoadSegment( this->GetNumRoadSegments( ) - 1 );

    pRoadSegment->GetCorner( 1, out );
}

/*
==============================================================================
Road::GetRoadSegmentAtPoint
==============================================================================
Description:    returns a road segment, if the point is inside a segment on this road.

Parameters:     ( const rmt::Vector& point, RoadSegment& outRoadSegment, int hint )

Return:         int - index of road segment this point is in.

=============================================================================
*/
int Road::GetRoadSegmentAtPoint( const rmt::Vector& point, RoadSegment** ppOutRoadSegment, float& in, float& lateral, int hint ) const
{
    rmt::Vector vectorBetween;
    // Do the 2D Math Explicitly.
    // My profile test show it is significantly faster.
    //
    vectorBetween.x = point.x - mSphere.centre.x;
    vectorBetween.y = 0.0f;
    vectorBetween.z = point.z - mSphere.centre.z;
    vectorBetween.x *= vectorBetween.x;
    vectorBetween.z *= vectorBetween.z;
    vectorBetween.x += vectorBetween.z;
    
    if ( vectorBetween.x <= rmt::Sqr( mSphere.radius ) )
    {
        // Now test against AABB.
        //
        if ( point.x <= mBox.high.x
            && point.x >= mBox.low.x
            && point.z <= mBox.high.z
            && point.z >= mBox.low.z )
        {
            int i = hint;
            // Check the hint.
            //
            if ( !IsPointInRoadSegment( i, point, in, lateral ) )
            {
                // Check the one ahead.
                //
                if ( !IsPointInRoadSegment( ++i, point, in, lateral ) )
                {
                    // Check the one behind.
                    //
                    if ( !IsPointInRoadSegment( i -= 2, point, in, lateral ) )
                    {
                        // Scan the entire list.
                        //
                        for ( i = 0; i < static_cast< int >( GetNumRoadSegments( ) ); i++ )
                        {
                            // Skip the ones we've looked at.
                            //
                            if ( i != hint && i != hint + 1 && i != hint - 1 )
                            {
                                // Stop when we find one.
                                //
                                if ( IsPointInRoadSegment( i, point, in, lateral ) )
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if ( i < static_cast< int >( GetNumRoadSegments( ) ) )
            {
                // We found one.
                //
                *ppOutRoadSegment = GetRoadSegment( i );
                return i;
            }
        }
    }
    *ppOutRoadSegment = 0;
    return -1;
}

/*
==============================================================================
Road::IsPointInRoadSegment
==============================================================================
Description:    Tests the distance into and the lateral distance in a segment
                if both lateral and in distance are between 0.0f - 1.0f
                we return true.

  Quick and Dirty performance number:
  XBox 02/28/2002
        26.0 ms per 10000 calls point inside BBox
        1.0 ms per 10000 calls test BBox only.

        0.0025 / 10000

        XBox 0.0025 ms in Debug for one call to IsPointInRoadSegment

Parameters:     ( int index )

Return:         bool - true if point is in road segment.

=============================================================================
*/
bool Road::IsPointInRoadSegment( int index, const rmt::Vector& point, float& in, float& lateral ) const
{
    if ( index >= 0 && index < static_cast<int>( GetNumRoadSegments() ) )
    {
        RoadSegment* pRoadSegment = GetRoadSegment( index );
        rmt::Vector vectorBetween;
        rmt::Sphere sphere;
        pRoadSegment->GetBoundingSphere( &sphere );

        // Do the 2D Math Explicitly.
        // My profile test show it is significantly faster.
        //
        vectorBetween.x = point.x - sphere.centre.x;
        vectorBetween.y = 0.0f;
        vectorBetween.z = point.z - sphere.centre.z;
        vectorBetween.x *= vectorBetween.x;
        vectorBetween.z *= vectorBetween.z;
        vectorBetween.x += vectorBetween.z;
    
        if ( vectorBetween.x <= rmt::Sqr( sphere.radius ) )
        {
            in = pRoadSegment->CalculateUnitDistIntoRoadSegment( point.x, point.z );
            if ( in >= 0.0f && in <= 1.0f )
            {
                lateral = pRoadSegment->CalculateUnitHeightInRoadSegment( point.x, point.z );
                if ( lateral >= 0.0f && lateral <= 1.0f )
                {
                    return true;
                }
            }
        }
    }
    return false;
}


//=============================================================================
// RoadManager::FindRoadSegmentAhead
//=============================================================================
// Description: Comment
//
// Parameters:  ( const Road* pRoad, 
//                float& dist, 
//                const float maxDist, 
//                RoadSegment** segment )
//
// Return:        bool 
//
//=============================================================================
bool Road::FindSegmentAhead( float& dist, 
                                      const float maxDist, 
                                      const int currentIndex,
                                      RoadSegment** segment ) const
{
    rmt::Vector backleft;
    rmt::Vector frontleft;
    rmt::Vector frontright;
    rmt::Vector backright;
    rmt::Vector leftdir;
    rmt::Vector rightdir;
    rmt::Vector direction;
    rmt::Vector destination;

    rmt::Vector currentPos;

    RoadSegment* pCurrentSegment = GetRoadSegment( currentIndex );
    
    //
    // Temporarily use pCurrentSegment to get data about where
    // we're starting
    //

    pCurrentSegment->GetCorner( 0, backleft );
    pCurrentSegment->GetCorner( 1, frontleft );
    pCurrentSegment->GetCorner( 2, frontright );
    pCurrentSegment->GetCorner( 3, backright );

    //
    // Find the front midpoint of the segment
    // 
    currentPos.Add( frontleft, frontright );
    currentPos.Scale( 0.5f, 0.5f, 0.5f );

    //
    // Find the average direction of the segment
    //

    leftdir.Sub( frontleft, backleft );
    leftdir.Normalize();
    rightdir.Sub( frontright, backright );
    rightdir.Normalize();

    //
    // Take the average and scale by 0.1
    //
    direction.Add( leftdir, rightdir );
    direction.Scale( 0.05f, 0.05f, 0.05f );

    //
    // This should put us within the next segment
    //
    currentPos.Add( direction );

    RoadSegment* pNextSegment = NULL;
    pCurrentSegment = NULL;
    
    //float in, lateral = 0.0f;
    int segmentIndex = currentIndex;

    float currentDist = dist;
    while( currentDist < maxDist )
    {
        //
        // This search is not optimal
        //
        //segmentIndex = pRoad->GetRoadSegmentAtPoint( currentPos, 
        //    &pNextSegment, in, lateral, segmentIndex + 1 );

        //
        // Much faster when the segments are sorted. Yay!
        //
        segmentIndex += 1;

        if( segmentIndex >= static_cast<int>( GetNumRoadSegments() ) )
        {
            //
            // Get as close as we can to the desired distance.
            // Returns NULL if we didn't find anything at all.
            //
            (*segment) = pCurrentSegment;
            dist = currentDist;
            return false;
        }
        
        pNextSegment = GetRoadSegment( segmentIndex );
        rAssert( pNextSegment != NULL );

        //
        // Find the midpoint of the front edge
        //
        pNextSegment->GetCorner( 1, frontleft );
        pNextSegment->GetCorner( 2, frontright );

        destination.Add( frontleft, frontright );
        destination.Scale( 0.5f, 0.5f, 0.5f );

        //
        // Gives a vector along the centre line assuming currentPos
        // is on the front edge midpoint of the previous segment
        //
        direction.Sub( destination, currentPos );

        float segLength = direction.Magnitude();
        currentDist += segLength;

        //
        // Normalize the direction and scale it by 0.1
        //
        // not necessary for the fast method
        //float invLen = 0.1f / segLength;
        //direction.Scale( invLen, invLen, invLen );

        //currentPos.Add( destination, direction );

        pCurrentSegment = pNextSegment;

    }

    //
    // Got to the distance the person wanted
    //
    (*segment) = pCurrentSegment;
    dist = currentDist;
    return true;
}

//=============================================================================
// RoadManager::FindSegmentBehind
//=============================================================================
// Description: Comment
//
// Parameters:  ( const Road* pRoad, 
//                float& dist, 
//                const float maxDist, 
//                RoadSegment** segment )
//
// Return:        bool 
//
//=============================================================================
bool Road::FindSegmentBehind( float& dist, 
                                      const float maxDist, 
                                      const int currentIndex,
                                      RoadSegment** segment ) const
{
    rmt::Vector backleft;
    rmt::Vector frontleft;
    rmt::Vector frontright;
    rmt::Vector backright;
    rmt::Vector leftdir;
    rmt::Vector rightdir;
    rmt::Vector direction;
    rmt::Vector destination;

    rmt::Vector currentPos;

    RoadSegment* pCurrentSegment = GetRoadSegment( currentIndex );
    
    //
    // Temporarily use pCurrentSegment to get data about where
    // we're starting
    //

    pCurrentSegment->GetCorner( 0, backleft );
    pCurrentSegment->GetCorner( 1, frontleft );
    pCurrentSegment->GetCorner( 2, frontright );
    pCurrentSegment->GetCorner( 3, backright );

    //
    // Find the rear midpoint of the segment
    // 
    currentPos.Add( backleft, backright );
    currentPos.Scale( 0.5f, 0.5f, 0.5f );

    //
    // Find the average direction of the segment
    //

    leftdir.Sub( backleft, frontleft );
    leftdir.Normalize();
    rightdir.Sub( backright, frontright );
    rightdir.Normalize();

    //
    // Take the average and scale by 0.1
    //
    direction.Add( leftdir, rightdir );
    direction.Scale( 0.05f, 0.05f, 0.05f );

    //
    // This should put us within the next segment
    //
    currentPos.Add( direction );

    RoadSegment* pNextSegment = NULL;
    //float in, lateral = 0.0f;
    int segmentIndex = currentIndex;

    float currentDist = dist;
    while( currentDist < maxDist )
    {
        pCurrentSegment = pNextSegment;

        //
        // This search is not optimal
        //
        //segmentIndex = pRoad->GetRoadSegmentAtPoint( currentPos, 
        //    &pNextSegment, in, lateral, segmentIndex + 1 );

        //
        // Much faster when the segments are sorted. Yay!
        //
        segmentIndex -= 1;

        if( segmentIndex < 0 )
        {
            //
            // Get as close as we can to the desired distance.
            // Returns NULL if we didn't find anything at all.
            //
            (*segment) = pCurrentSegment;
            dist = currentDist;
            return false;
        }
        
        pNextSegment = GetRoadSegment( segmentIndex );
        rAssert( pNextSegment != NULL );

        //
        // Find the midpoint of the front edge
        //
        pNextSegment->GetCorner( 0, backleft );
        pNextSegment->GetCorner( 3, backright );

        destination.Add( backleft, backright );
        destination.Scale( 0.5f, 0.5f, 0.5f );

        //
        // Gives a vector along the centre line assuming currentPos
        // is on the front edge midpoint of the previous segment
        //
        direction.Sub( destination, currentPos );

        float segLength = direction.Magnitude();
        currentDist += segLength;

        //
        // Normalize the direction and scale it by 0.1
        //
        // not necessary for the fast method
        //float invLen = 0.1f / segLength;
        //direction.Scale( invLen, invLen, invLen );

        //currentPos.Add( destination, direction );
    }

    //
    // Got to the distance the person wanted
    //
    (*segment) = pCurrentSegment;
    dist = currentDist;
    return true;
}


//=============================================================================
// Road::SetDensity
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int density )
//
// Return:      void 
//
//=============================================================================
void Road::SetDensity( unsigned int density )
{
    mDensity = density;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Road::FindRoadSegmentAtDist( float iDist, RoadSegment** oppRoadSegment )
{
    if(iDist>GetRoadLength())
    {
        *oppRoadSegment = NULL;
        return;
    }

    RoadSegment* pRoadSegment = NULL;

    float distAccum = 0.0f;
	for(int i=mnRoadSegments-1; i>-1 && distAccum<iDist; i--)
    {
        pRoadSegment = mppRoadSegmentArray[i];
        distAccum += pRoadSegment->GetSegmentLength();
    }

    *oppRoadSegment = pRoadSegment;
}

