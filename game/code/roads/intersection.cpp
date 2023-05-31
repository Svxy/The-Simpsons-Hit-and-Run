/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Intersection

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/

#include <roads/intersection.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/lane.h>
#include <roads/trafficcontrol.h>

#include <meta/triggerlocator.h>
#include <memory/srrmemory.h>

// Math includes.
//
#ifdef TOOLS
#include <choreo/utility.hpp>
#else
#include <choreo/utility.hpp>
#endif


Intersection::Intersection( void )
:
mBigIntersection( 0 ),
mIndex( -1 ),
mpTrafficControl( NULL ),
mnRoadsIn( 0 ),
mnRoadsOut( 0 ),
mfRotation( 0.0f ),
mfRadius( 15.0f )
{
    mpAnimEntityList.Allocate(2);
	int i;

	for ( i = 0; i < MAX_ROADS; i++ )
	{
        mRoadListIn[ i ] = 0;
        mRoadListOut[ i ] = 0;
    }

    mLightControl.SetIntersection( this );
    mNWayControl.SetIntersection( this );
}

Intersection::Intersection( TriggerLocator* pLocator )
:
mBigIntersection( 0 ),
mIndex( -1 ),
mpTrafficControl( NULL ),
mnRoadsIn( 0 ),
mnRoadsOut( 0 ),
mfRotation( 0.0f ),
mfRadius( 15.0f )
{
    mpAnimEntityList.Allocate(2);

	int i;

	for ( i = 0; i < MAX_ROADS; i++ )
	{
        mRoadListIn[ i ] = 0;
        mRoadListOut[ i ] = 0;
    }
    mLightControl.SetIntersection( this );
    mNWayControl.SetIntersection( this );
}

Intersection::~Intersection( void )
{
    mWaitingRoads.Clear();
    mShortestRoadsToAdjacentIntersectionsWithMultiplier.Clear();
    mShortestRoadsToAdjacentIntersectionsNoMultiplier.Clear();
    mOutgoingShortcuts.Clear();
    if( mBigIntersection )
    {
        mBigIntersection->routesWithMultiplier.Clear();
        mBigIntersection->routesNoMultiplier.Clear();
        delete mBigIntersection;
    }
}
// Simulate the intersection when there are cars nearby.
/*
==============================================================================
Intersection::Update
==============================================================================
Description:    Comment

Parameters:     ( unsigned int dt )

Return:         void 

=============================================================================
*/
void Intersection::Update( unsigned int dt )
{
    if( mpTrafficControl != NULL )
    {
        mpTrafficControl->Update( dt );
    }
}

// advance the traffic in roads 'RoadMask' according to light state.
/*
==============================================================================
Intersection::AdvanceTraffic
==============================================================================
Description:    Comment

Parameters:     ( unsigned int RoadMask, unsigned int state )

Return:         void 

=============================================================================
*/
void Intersection::AdvanceTraffic( unsigned int RoadMask, unsigned int state ) const
{
	int i;

	for ( i = 0; i < MAX_ROADS; i++ )
	{
		unsigned int mask = 1 << i;
		if ( RoadMask & mask )
		{
            if ( mRoadListIn[ i ] )
            {
			    unsigned int j;
			    for ( j = 0; j < mRoadListIn[i]->GetNumLanes(); j++ )
			    {
                    Lane* pLane = mRoadListIn[i]->GetLane( j );
                    if ( pLane )
                    {
				        pLane->NotifyWaitingTraffic( state );
			        }
                }
            }
		}
	}
}


void Intersection::AdvanceNextWaitingRoad()
{
    if( mWaitingRoads.mUseSize > 0 )
    {
        Road* road = mWaitingRoads[0];
        for( unsigned int i=0; i<road->GetNumLanes(); i++ )
        {
            road->GetLane(i)->NotifyWaitingTraffic( TrafficControl::GREEN );
        }
        mWaitingRoads.RemoveKeepOrder( 0 );
    }
}

// Is the intersection clear of all cars.
/*
==============================================================================
Intersection::IsIntersectionClear
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
bool Intersection::IsIntersectionClear( void ) const
{
	// TODO: Implement this properly.
	return true;
}

// add a road to the in list.
/*
==============================================================================
Intersection::AddRoadIn
==============================================================================
Description:    Comment

Parameters:     ( Road *pRoad )

Return:         void 

=============================================================================
*/
void Intersection::AddRoadIn( Road *pRoad )
{
	mRoadListIn[ mnRoadsIn ] = pRoad;
	mnRoadsIn++;
}
// add a road to the out list.
/*
==============================================================================
Intersection::AddRoadOut
==============================================================================
Description:    Comment

Parameters:     ( Road *pRoad )

Return:         void 

=============================================================================
*/
void Intersection::AddRoadOut( Road *pRoad )
{
	mRoadListOut[ mnRoadsOut ] = pRoad;
	mnRoadsOut++;
}
// Find the road pointer in the road list.
/*
==============================================================================
Intersection::FindRoadIn
==============================================================================
Description:    Comment

Parameters:     ( const Road* pRoad )

Return:         int 

=============================================================================
*/
int Intersection::FindRoadIn( const Road* pRoad ) const
{
	unsigned int i;
	for ( i = 0; i < mnRoadsIn; i++ )
	{
		if ( pRoad == mRoadListIn[ i ] )
		{
			// found it.
			return i;
		}
	}
	// not found
	return -1;
}

// Find the road pointer in the road list.
/*
==============================================================================
Intersection::FindRoadOut
==============================================================================
Description:    Comment

Parameters:     ( const Road* pRoad )

Return:         int 

=============================================================================
*/
int Intersection::FindRoadOut( const Road* pRoad ) const
{
	unsigned int i;
	for ( i = 0; i < mnRoadsOut; i++ )
	{
		if ( pRoad == mRoadListOut[ i ] )
		{
			// found it.
			return i;
		}
	}
	// not found
	return -1;
}

/*
==============================================================================
Intersection::GetLocation
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         const 

=============================================================================
*/

void Intersection::GetLocation( rmt::Vector& location ) const
{
    location = mLocation;
}




void Intersection::PopulateShortestRoads( SwapArray<RoadManager::ShortestRoad>& roadsToAdjacentInts, bool useMultiplier )
{
    ////////////
    // First search through the IN roads, tallying up the shortest, unique IN roads 
    // that get us to distinct adjacent intersections

    for( int i = 0; i< static_cast<int>(mnRoadsIn); i++ )
    {
        Road* road = mRoadListIn[i];
        const Intersection* in = road->GetSourceIntersection();

        // if the road loops back onto same intersection, forget it
        if( in == this ) 
        {
            continue;
        }

        // if this is a shortcut road, forget it
        if( road->GetShortCut() )
        {
            continue;
        }

        RoadManager::ShortestRoad sr;
        sr.isOutRoad = false;
        sr.road = road;

        sr.cost = road->GetRoadLength();
        sr.cost *= useMultiplier ? RoadManager::AGAINST_TRAFFIC_COST_MULTIPLIER : 1.0f;

        // see if we already have this intersection listed
        // If not, add it.
        // If found, check if this road is shorter than road listed
        RoadManager::ShortestRoad* testRoad = NULL;
        const Intersection* testInt = NULL;

        bool found = false;
        for( int j=0; j<roadsToAdjacentInts.mUseSize; j++ )
        {
            testRoad = &(roadsToAdjacentInts[j]);
            float testCost = testRoad->road->GetRoadLength();

            if( testRoad->isOutRoad )
            {
                testInt = testRoad->road->GetDestinationIntersection();
            }
            else
            {
                testInt = testRoad->road->GetSourceIntersection();
                testCost *= useMultiplier ? RoadManager::AGAINST_TRAFFIC_COST_MULTIPLIER : 1.0f;
            }
            if( testInt == in )
            {
                // ok, found "in" in our list... 
                // if the length of road is shorter, store this road
                // instead of testRoad
                found = true;

                if( sr.cost < testCost )
                {
                    roadsToAdjacentInts.Remove( j );
                    roadsToAdjacentInts.Add( sr );
                }
                // we need to break here since we modified the usesize.
                break;
            }
        }
        // if we never found it in any of our lists, add it to the list of IN roads
        if( !found )
        {
            roadsToAdjacentInts.Add( sr );
        }
    }


    /////////////////////
    // Now do the same for the OUT roads

    for( int i = 0; i<static_cast<int>(mnRoadsOut); i++ )
    {
        Road* road = mRoadListOut[i];
        const Intersection* in = road->GetDestinationIntersection();

        // if the road loops back onto same intersection, forget it
        if( in == this ) 
        {
            continue;
        }

        // if this is a shortcut road, add it to the shortcut list, 
        // but skip adding it to adjacency list
        if( road->GetShortCut() )
        {
            continue;
        }


        RoadManager::ShortestRoad sr;
        sr.isOutRoad = true;
        sr.road = road;
        sr.cost = road->GetRoadLength();

        // see if we already have this intersection listed
        // If not, add it.
        // If found, check if this road is shorter than road listed
        RoadManager::ShortestRoad* testRoad = NULL;
        const Intersection* testInt = NULL;

        bool found = false;
        for( int j=0; j<roadsToAdjacentInts.mUseSize; j++ )
        {
            testRoad = &(roadsToAdjacentInts[j]);
            float testCost = testRoad->road->GetRoadLength();

            if( testRoad->isOutRoad )
            {
                testInt = testRoad->road->GetDestinationIntersection();
            }
            else
            {
                testInt = testRoad->road->GetSourceIntersection();
                testCost *= useMultiplier ? RoadManager::AGAINST_TRAFFIC_COST_MULTIPLIER : 1.0f;
            }
            if( testInt == in )
            {
                // ok, "in" already exists in our list... 
                // if the length of road is shorter, store this road
                // instead of testRoad
                found = true;
                if( sr.cost < testCost )
                {
                    roadsToAdjacentInts.Remove( j );
                    roadsToAdjacentInts.Add( sr );
                }
                // we need to break here since we modified the usesize.
                break;
            }
        }
        // if we never found it in any of our lists, add it to the list of IN roads
        if( !found )
        {
            roadsToAdjacentInts.Add( sr );
        }
    }
}





/*
==============================================================================
Intersection::SortRoads
==============================================================================
Description:    Sorts the roads into clockwise order.
                Pick a road to start, add it to the head of the sorted list.
                Find the first road to the right of that, add it to the sorted list.
                Repeat until all roads have been added to the sorted list.

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void Intersection::SortRoads( void )
{
    struct roadOrientation
    {
        Road* pRoad;
        float fAngle;
    };

	// a copy of the list of the roads leading in to this intersection.
	//
	roadOrientation roadListIn[ MAX_ROADS ];

    // a copy of the list of the roads leading out of this intersection.
	//
	roadOrientation roadListOut[ MAX_ROADS ];
    
    for ( int i = 0; i < MAX_ROADS; i++ )
    {
        roadListIn[ i ].pRoad = mRoadListIn[ i ];
        if ( mRoadListIn[ i ] )
        {
            rmt::Vector to, from;
            // Get the facing vector by finding the two points that connect the road.
            //
            roadListIn[ i ].pRoad->GetSourceIntersection()->GetLocation( from );
            this->GetLocation( to );
            // Get the vector between and flip it.
            // Choreo considers 0 0 -1 = 0 degrees.
            //
            to.Sub( from );
            to.Scale( -1.0f );

            roadListIn[ i ].fAngle = choreo::GetWorldAngle( to.x, to.z );
        } 

        roadListOut[ i ].pRoad = mRoadListOut[ i ];
        if ( mRoadListOut[ i ] )
        {
            rmt::Vector to, from;    
            // Get the facing vector by finding the two points that connect the road.
            //
            roadListOut[ i ].pRoad->GetDestinationIntersection()->GetLocation( from );
            this->GetLocation( to );
            // Get the vector between and flip it.
            // Choreo considers 0 0 -1 = 0 degrees.
            //
            to.Sub( from );
            to.Scale( -1.0f );

            roadListOut[ i ].fAngle = choreo::GetWorldAngle( to.x, to.z );
        }
    }

    for ( int i = 0; i < MAX_ROADS; i++ )
    {
        roadOrientation* pRoadOrientation = 0;
        // A larger angle than would ever be stored.
        //
        float fMinAngle = rmt::PI * 4; 
        
        for ( int j = 0; j < MAX_ROADS; j++ )
        {
            // Take the smallest remaining angle each time through.
            //
            if ( roadListIn[ j ].pRoad && roadListIn[ j ].fAngle < fMinAngle )
            {
                // This is the best candidate so far.
                //
                fMinAngle = roadListIn[ j ].fAngle;
                // So store it for later.
                //
                pRoadOrientation = &roadListIn[ j ];
            }
        }
        if ( pRoadOrientation )
        {
            // Store this road in sorted order.
            //
            mRoadListIn[ i ] = pRoadOrientation->pRoad;
            // Remove the road from consideration.
            //
            pRoadOrientation->pRoad = 0;
        }
        else
        {
            mRoadListIn[ i ] = 0;
        }
    }

    for ( int i = 0; i < MAX_ROADS; i++ )
    {
        roadOrientation* pRoadOrientation = 0;
        // A larger angle than would ever be stored.
        //
        float fMinAngle = rmt::PI * 4; 
        
        for ( int j = 0; j < MAX_ROADS; j++ )
        {
            // Take the smallest remaining angle each time through.
            //
            if ( roadListOut[ j ].pRoad && roadListOut[ j ].fAngle < fMinAngle )
            {
                // This is the best candidate so far.
                //
                fMinAngle = roadListOut[ j ].fAngle;
                // So store it for later.
                //
                pRoadOrientation = &roadListOut[ j ];
            }
        }
        if ( pRoadOrientation )
        {
            // Store this road in sorted order.
            //
            mRoadListOut[ i ] = pRoadOrientation->pRoad;
            // Remove the road from consideration.
            //
            pRoadOrientation->pRoad = 0;
        }
        else
        {
            mRoadListOut[ i ] = 0;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // store shortcut roads going OUT of this intersection..
    // Note, shortcuts can only be taken one way (e.g. a jump), so we 
    // only consider the OUTGOING shortcut roads...

    SwapArray<Road*> outShortcutRoads;

    HeapMgr()->PushHeap(GMA_TEMP);
    outShortcutRoads.Allocate( MAX_ROADS );
    HeapMgr()->PopHeap(GMA_TEMP);

    for( int i = 0; i<static_cast<int>(mnRoadsOut); i++ )
    {
        Road* road = mRoadListOut[i];
        const Intersection* in = road->GetDestinationIntersection();

        // if the road loops back onto same intersection, forget it
        if( in == this ) 
        {
            continue;
        }

        // if this is a shortcut road, add it to the shortcut list, 
        // but skip adding it to adjacency list
        if( road->GetShortCut() )
        {
            outShortcutRoads.Add( road );
            continue;
        }
    }

    // also, store away the list of shortcut roads
    if( outShortcutRoads.mUseSize > 0 )
    {
        mOutgoingShortcuts.Allocate( outShortcutRoads.mUseSize );
        for( int i=0; i<outShortcutRoads.mUseSize; i++ )
        {
            mOutgoingShortcuts.Add( outShortcutRoads[i] );
        }
    }
    outShortcutRoads.Clear();

    ////////////////////////////
    // Determine shortest roads
    // Store two versions: the one that uses AGAINST_TRAFFIC_COST_MULTIPLIER 
    // and the one that doesn't

    SwapArray<RoadManager::ShortestRoad> roadsToAdjacentInts;

    HeapMgr()->PushHeap(GMA_TEMP);
    roadsToAdjacentInts.Allocate( MAX_ROADS*2 );
    HeapMgr()->PopHeap(GMA_TEMP);

    PopulateShortestRoads( roadsToAdjacentInts, true );
    if( roadsToAdjacentInts.mUseSize > 0 )
    {
        mShortestRoadsToAdjacentIntersectionsWithMultiplier.Allocate( roadsToAdjacentInts.mUseSize );
        for( int i=0; i<roadsToAdjacentInts.mUseSize; i++ )
        {
            mShortestRoadsToAdjacentIntersectionsWithMultiplier.Add( roadsToAdjacentInts[i] );
        }
    }
    roadsToAdjacentInts.ClearUse();

    PopulateShortestRoads( roadsToAdjacentInts, false );
    if( roadsToAdjacentInts.mUseSize > 0 )
    {
        mShortestRoadsToAdjacentIntersectionsNoMultiplier.Allocate( roadsToAdjacentInts.mUseSize );
        for( int i=0; i<roadsToAdjacentInts.mUseSize; i++ )
        {
            mShortestRoadsToAdjacentIntersectionsNoMultiplier.Add( roadsToAdjacentInts[i] );
        }
    }
    roadsToAdjacentInts.Clear();

    // the choices of roads maybe different with or without multiplier, but
    // the sizes of the two arrays should be the same because the number
    // of reachable, neighboring intersections is the same
    rAssert( mShortestRoadsToAdjacentIntersectionsWithMultiplier.mUseSize ==
        mShortestRoadsToAdjacentIntersectionsNoMultiplier.mUseSize );

    // see if we are a big intersection, doesn't matter which size we use here.
    int numAdjacentInts = mShortestRoadsToAdjacentIntersectionsWithMultiplier.mUseSize;
    if( numAdjacentInts > 2 )
    {
        mBigIntersection = new RoadManager::BigIntersection;
        mBigIntersection->in = this;
    }

    ////////////////////////////////////////////////////////////////////////////
    // allocate space for waiting roads... only need to do it for IN roads
    // because they're the ones approaching the intersection
    if( mType != Intersection::NO_STOP )
    {
        if( mnRoadsIn > 0 )
        {
            mWaitingRoads.Allocate( mnRoadsIn );
        }
    }



    ////////////////////////////////////////////////////////////////////////////
    // Do some checking here, shall we?
    // Every intersection must have at least 1 neighbor (must be attached to rest of world)

#if( RAD_TUNE || RAD_DEBUG )
    if( numAdjacentInts <= 0 )
    {
        char msg[256];
        sprintf( msg, 
            "Intersection at (%0.2f,%0.2f,%0.2f) is not joined to any other\n"
            "    intersection by any road!\n",
            mLocation.x, mLocation.y, -mLocation.z );
        rTuneAssertMsg( false, msg );
    }
#endif

#ifdef RAD_DEBUG
    for( int i = 0; i<mShortestRoadsToAdjacentIntersectionsWithMultiplier.mUseSize; i++ )
    {
        RoadManager::ShortestRoad* shortestRoad = 
            &(mShortestRoadsToAdjacentIntersectionsWithMultiplier[i]);

        Intersection* testInt = NULL;
        if( shortestRoad->isOutRoad )
        {
            testInt = (Intersection*) 
                shortestRoad->road->GetSourceIntersection();
        }
        else
        {
            testInt = (Intersection*) 
                shortestRoad->road->GetDestinationIntersection();
        }
        rAssert( testInt == this );
    }
    for( int i = 0; i<mShortestRoadsToAdjacentIntersectionsNoMultiplier.mUseSize; i++ )
    {
        RoadManager::ShortestRoad* shortestRoad = 
            &(mShortestRoadsToAdjacentIntersectionsNoMultiplier[i]);

        Intersection* testInt = NULL;
        if( shortestRoad->isOutRoad )
        {
            testInt = (Intersection*) 
                shortestRoad->road->GetSourceIntersection();
        }
        else
        {
            testInt = (Intersection*) 
                shortestRoad->road->GetDestinationIntersection();
        }
        rAssert( testInt == this );
    }
#endif

#ifdef TOOLS
//    char baseMsg [1000];
//    char fullMsg [1100];

    /*
    // Every intersection should have at minimum 1 OUT road
    if( mnRoadsOut <= 0 )
    {
        sprintf( fullMsg, 
            "You are doomed. Intersection at (%f,%f,%f)\n"
            "    does not have an out road.\n", 
            mLocation.x, mLocation.y, -1*mLocation.z );
        rTuneAssertMsg( false, fullMsg );
    }
    */

    /*
    // Road segments coming in and going out of this intersection MUST have same values 
    // of y where they hit the intersection. If this is not so, it is due to BAD ROAD DATA. 
    // For the sake of traffic, the intersection must be FLAT (so Traffic only computes
    // a spline in 2D rather than 3D).
    //
    // If the y values of any two segments do not match up, cars will "hop" when 
    // they transit through the intersection between these segments.
    // 
    // Please notify Sheik to check the road data around nearby intersections.
    //
    sprintf( baseMsg, 
        "\nMismatching y-values at intersection (%f,%f,%f).\n"
        "    Check if y values are same for all IN & OUT road segments attached\n"
        "    to this intersection. Check hypergraph to see if the roadnodes leading\n"
        "    IN and OUT of this intersection contain all the proper roadsegments.\n"
        "    If you skip this, Traffic cars will \"hop\" when they transit through\n"
        "    the intersection between the road segments with mismatching y values.\n"
        "    Better to report error to me (Dusit) or Sheik.\n\n", 
        mLocation.x, 
        mLocation.y,
        -1*mLocation.z );

    float ep = 0.001f;
    for( unsigned int m=0; m<mnRoadsIn; m++ )
    {
        // grab the last segment on the in road
        RoadSegment* inSeg = mRoadListIn[m]->GetRoadSegment
            ( mRoadListIn[m]->GetNumRoadSegments()-1 );
        rTuneAssert( inSeg != NULL );

        rmt::Vector p1, p2;
        inSeg->GetCorner( 1, p1 );
        inSeg->GetCorner( 2, p2 );

        sprintf( fullMsg, "%s    Offending road: IN \"%s\"\n", 
            baseMsg, mRoadListIn[m]->GetName() );
        rTuneAssertMsg( rmt::Epsilon( p1.y, p2.y, ep ), fullMsg );

        for( unsigned int n=0; n<mnRoadsOut; n++ )
        {
            // grab the first segment on the out road
            RoadSegment* outSeg = mRoadListOut[n]->GetRoadSegment( 0 );
            rTuneAssert( outSeg != NULL );

            rmt::Vector q0, q3;
            outSeg->GetCorner( 0, q0 );
            outSeg->GetCorner( 3, q3 );

            sprintf( fullMsg, "%s    Offending roads: IN \"%s\", OUT \"%s\"\n", 
                baseMsg, mRoadListIn[m]->GetName(), mRoadListOut[n]->GetName() );
            rTuneAssertMsg( rmt::Epsilon( p1.y, q0.y, ep ) &&
                            rmt::Epsilon( p1.y, q3.y, ep ), fullMsg );
        }
    }
    */
#endif
}

//==============================================================================
// Intersection::GetType
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Intersection::Type
//
//==============================================================================
Intersection::Type Intersection::GetType() const
{
    return mType;
}


//==============================================================================
// Intersection::SetType
//==============================================================================
// Description: Comment
//
// Parameters:  ( Type type )
//
// Return:      void 
//
//==============================================================================
void Intersection::SetType( Type type )
{
    mType = type;
    switch( mType )
    {
    case NO_STOP:
        {
            mpTrafficControl = NULL;
        }
        break;
    case N_WAY:
        {
            mpTrafficControl = &mNWayControl;
        }
        break;
    default:
        {
            mpTrafficControl = NULL;
        }
        break;
    }
}

//=============================================================================
// Intersection::IsPointInIntersection
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector& point )
//
// Return:      bool 
//
//=============================================================================
bool Intersection::IsPointInIntersection( rmt::Vector& point ) const
{
    rmt::Vector vectorBetween;
    
    vectorBetween.x = point.x - mLocation.x;
    vectorBetween.y = 0.0f;
    vectorBetween.z = point.z - mLocation.z;
    vectorBetween.x *= vectorBetween.x;
    vectorBetween.z *= vectorBetween.z;
    vectorBetween.x += vectorBetween.z;
    
    return ( vectorBetween.x <= rmt::Sqr( mfRadius ) );    
}


void Intersection::FindGoodTrafficLane( const Road& road, unsigned int preferredLaneIndex, Lane*& outLane, unsigned int& outLaneIndex )
{
    Lane* lane = road.GetLane(preferredLaneIndex);
    int numTrafficVehicles = 0;
    if( lane != NULL )
    {
        numTrafficVehicles = lane->mTrafficVehicles.mUseSize;
        rAssert( numTrafficVehicles >= 0 );
        
        if( numTrafficVehicles < lane->GetDensity() )
        {
            outLane = lane;
            outLaneIndex = preferredLaneIndex;
            return;
        }
    }

    // if can't use the preferred lane, gotta find the first lane that can support my vehicle...
    for( unsigned int i=0; i<road.GetNumLanes(); i++ )
    {
        lane = road.GetLane(i);
        rAssert( lane != NULL );

        numTrafficVehicles = lane->mTrafficVehicles.mUseSize;
        rAssert( numTrafficVehicles >= 0 );

        if( numTrafficVehicles < lane->GetDensity() )
        {
            outLane = lane;
            outLaneIndex = i;
            return;
        }
    }

    // finally, can't find any lane, return NULL...
    outLane = NULL;
    outLaneIndex = 0;
    return;
}


void Intersection::GetLeftTurnForTraffic( const Road& inRoad, 
                                          unsigned int preferredLaneIndex,
                                          Road*& outRoad, 
                                          Lane*& outLane, 
                                          unsigned int& outLaneIndex )
{
    outRoad = NULL;
    outLane = NULL;
    outLaneIndex = 0;

	int index = FindRoadIn( &inRoad );
    rAssert( index != -1 );
    if( index == -1 )
    {
        return;
    }

    if( mnRoadsOut <= 0 )
    {
        return;
    }

    if( mnRoadsOut == 1 )
    {
        //No roads out.  Or only a straight out.
        rDebugString( "Intersection has only one OUT road!\n" );

        outRoad = mRoadListOut[0];

        // Pick an outLane & outLaneIndex
        FindGoodTrafficLane( *outRoad, preferredLaneIndex, outLane, outLaneIndex );

        if( outLane == NULL )
        {
            return;
        }
    }
    else 
    {
        // To find which turn will give a closest approximation to a "LEFT"
        // turn, we turn our inDir by 90 degrees (to the left!) and 
        // iterate through the outDirs for one that's STRAIGHTEST along it.
        //
        rmt::Vector origInDir;
        rmt::Vector inDir, outDir;

        // Grab normal of trailing edge of the last segment of IN road (for comparison)
        RoadSegment* segment;
        unsigned int nSegments = inRoad.GetNumRoadSegments();
        assert( nSegments > 0 );
        segment = inRoad.GetRoadSegment( nSegments - 1 );

        // TODO:
        // Don't use Edge Normals, use Direction of Segment's LANE (but we'll have to
        // pass in lane info)
        segment->GetEdgeNormal( 2, origInDir );

        // *** NOTE ***
        // We rely on the assumption that the intersection is HORIZONTALLY FLAT
        // Else, we'll have to project the In & Out vectors onto the plane of 
        // the intersection.
        origInDir.y = 0.0f;
        origInDir.Normalize(); // *** SQUARE ROOT! ***

        // copy origInDir... we'll be modifying inDir to suit our needs
        inDir = origInDir;
        inDir.Scale(-1.0f);

        // turn inDir CCW 90 degrees about y axis (the ideal left turn
        // will be 180 angle from this vector)
        rmt::Vector temp;
        temp.Set( -1 * inDir.z, inDir.y, inDir.x );
        inDir = temp;

        float cosAlpha = 0.0f;
    
        // Let bestCosAlpha be, initially, the WORST value possible 
        // cosAlpha of 1.0 = alphaBetwIn&OutRoads of 0 = 180 degree turn! BAD!
        // As we iterate through the OUT roads, we want to find one with most 
        // alpha, or smallest cosAlpha
        //
        float bestCosAlpha = 1.0f; 
        bool stillLookingForFirstRoad = true;

        outRoad = NULL;
        outLane = NULL;
        outLaneIndex = 0;

        unsigned int i = 0;
        for( i ; i<mnRoadsOut ; ++i )
        {
            // Grab normal of leading edge of the first segment of OUT road
            segment = mRoadListOut[i]->GetRoadSegment(0);
            segment->GetEdgeNormal(0, outDir); 

            // *** NOTE ***
            // We rely on the assumption that the intersection is HORIZONTALLY FLAT
            // Else, we'll have to project the In & Out vectors onto the plane of 
            // the intersection.
            outDir.y = 0.0f;
            outDir.Normalize();  // *** SQUARE ROOT! *** 

            // Skip this OUT road if it just goes back the way we came
            // This prevents U-turn at an intersection when there are 
            // other roads to take, even if they don't turn the way
            // we want. The decision is really up to traffic control or
            // some other entity; for now, we hardcode it in the Intersection
            if( outDir.Equals( origInDir * -1, 0.05f ) )
            {
                continue;
            }

            cosAlpha = inDir.Dot(outDir);
        
            // *** NOTE ***
            // If contention arises between a left and a right turn that 
            // both have bestCosAlpha, this inequality will favor the last 
            // best match. 
            if( stillLookingForFirstRoad || (cosAlpha <= bestCosAlpha) )
            {
                Lane* laneCandidate = NULL;
                unsigned int laneIndexCandidate = 0;
                FindGoodTrafficLane( *mRoadListOut[i], preferredLaneIndex, laneCandidate, laneIndexCandidate );

                if( laneCandidate != NULL )
                {
                    stillLookingForFirstRoad = false;
                    bestCosAlpha = cosAlpha; 
                    outRoad = mRoadListOut[i];
                    outLane = laneCandidate;
                    outLaneIndex = laneIndexCandidate;
                }
            }
        }// end of FOR LOOP

    }// end of ELSE

}



void Intersection::GetStraightForTraffic( const Road& inRoad, 
                                          unsigned int preferredLaneIndex,
                                          Road*& outRoad, 
                                          Lane*& outLane, 
                                          unsigned int& outLaneIndex )
{
    outRoad = NULL;
    outLane = NULL;
    outLaneIndex = 0;

	int index = FindRoadIn( &inRoad );
    rAssert( index != -1 );
    if( index == -1 )
    {
        return;
    }

    if( mnRoadsOut <= 0 )
    {
        return;
    }

    if ( mnRoadsOut == 1 )
    {
        rDebugString( "Intersection has only one OUT road!\n" );

        outRoad = mRoadListOut[0];

        // Pick an outLane & outLaneIndex
        FindGoodTrafficLane( *outRoad, preferredLaneIndex, outLane, outLaneIndex );

        if( outLane == NULL )
        {
            return;
        }
    } 
    else 
    {
        // Iterate through all the OUT roads to find which one gives us the angle closest to 
        // going STRAIGHT (180)
        //
        rmt::Vector origInDir;
        rmt::Vector inDir, outDir;

        // Grab normal of trailing edge of the last segment of IN road (for comparison)
        RoadSegment* segment;
        unsigned int nSegments = inRoad.GetNumRoadSegments();
        assert( nSegments > 0 );
        segment = inRoad.GetRoadSegment( nSegments - 1 );
        segment->GetEdgeNormal( 2, origInDir );

        // *** NOTE ***
        // We rely on the assumption that the intersection is HORIZONTALLY FLAT
        // Else, we'll have to project the In & Out vectors onto the plane of 
        // the intersection.
        origInDir.y = 0.0f;
        origInDir.Normalize(); // *** SQUARE ROOT! ***

        inDir = origInDir;
        inDir.Scale(-1.0f);

        float cosAlpha = 0.0f;
    
        // Let bestCosAlpha be, initially, the WORST value possible 
        // cosAlpha of 1.0 = alphaBetwIn&OutRoads of 0 = 180 degree turn! BAD!
        // As we iterate through the OUT roads, we want to find one with most 
        // alpha, or smallest cosAlpha
        //
        float bestCosAlpha = 1.0f; 
        bool stillLookingForFirstRoad = true;

        unsigned int i = 0;
        for( i ; i<mnRoadsOut ; ++i )
        {
            // Grab normal of leading edge of the first segment of OUT road
            segment = mRoadListOut[i]->GetRoadSegment(0);
            segment->GetEdgeNormal(0, outDir); 

            // *** NOTE ***
            // We rely on the assumption that the intersection is HORIZONTALLY FLAT
            // Else, we'll have to project the In & Out vectors onto the plane of 
            // the intersection.
            outDir.y = 0.0f;
            outDir.Normalize(); // *** SQUARE ROOT! ***

            // Skip this OUT road if it just goes back the way we came
            // This prevents U-turn at an intersection when there are 
            // other roads to take, even if they don't turn the way
            // we want. The decision is really up to traffic control or
            // some other entity; for now, we hardcode it in the Intersection
            if( outDir.Equals( origInDir * -1, 0.05f ) )
            {
                continue;
            }

            cosAlpha = inDir.Dot(outDir);
        
            // *** NOTE ***
            // This inequality will favor the last best match. (If another road
            // has cosAlpha = current bestCosAlpha, we ignore it & go with the
            // most recent one).
            if( stillLookingForFirstRoad || (cosAlpha <= bestCosAlpha) )
            {
                Lane* laneCandidate = NULL;
                unsigned int laneIndexCandidate = 0;
                FindGoodTrafficLane( *mRoadListOut[i], preferredLaneIndex, laneCandidate, laneIndexCandidate );

                if( laneCandidate != NULL )
                {
                    stillLookingForFirstRoad = false;
                    bestCosAlpha = cosAlpha; 
                    outRoad = mRoadListOut[i];
                    outLane = laneCandidate;
                    outLaneIndex = laneIndexCandidate;
                }
            }
        }
    }

}

// Given the current road, this should return a ref to the right turn road.
void Intersection::GetRightTurnForTraffic( const Road& inRoad, 
                                          unsigned int preferredLaneIndex,
                                          Road*& outRoad, 
                                          Lane*& outLane, 
                                          unsigned int& outLaneIndex )
{
    outRoad = NULL;
    outLane = NULL;
    outLaneIndex = 0;

	int index = FindRoadIn( &inRoad );
    rAssert( index != -1 );
    if( index == -1 )
    {
        return;
    }
    if( mnRoadsOut <= 0 )
    {
        return;
    }

    if( mnRoadsOut == 1 )
    {
        //No roads out.  Or only a straight out.
        rDebugString( "Intersection has only one OUT road!\n" );

        outRoad = mRoadListOut[0];

        // Pick an outLane & outLaneIndex
        FindGoodTrafficLane( *outRoad, preferredLaneIndex, outLane, outLaneIndex );

        if( outLane == NULL )
        {
            return;
        }
    }
    else 
    {
        // To find which turn will give a closest approximation to a "LEFT"
        // turn, we turn our inDir by 90 degrees (to the left!) and 
        // iterate through the outDirs for one that's STRAIGHTEST along it.
        //
        rmt::Vector origInDir;
        rmt::Vector inDir, outDir;

        // Grab normal of trailing edge of the last segment of IN road (for comparison)
        RoadSegment* segment;
        unsigned int nSegments = inRoad.GetNumRoadSegments();
        assert( nSegments > 0 );
        segment = inRoad.GetRoadSegment( nSegments - 1 );
        segment->GetEdgeNormal( 2, origInDir );

        // *** NOTE ***
        // We rely on the assumption that the intersection is HORIZONTALLY FLAT
        // Else, we'll have to project the In & Out vectors onto the plane of 
        // the intersection.
        origInDir.y = 0.0f;
        origInDir.Normalize(); // *** SQUARE ROOT! ***

        // copy origInDir... we'll be modifying inDir to suit our needs
        inDir = origInDir;
        inDir.Scale(-1.0f);

        // turn inDir CW 90 degrees about y axis (the ideal right turn
        // is at 180 angle from this vector)
        rmt::Vector temp;
        temp.Set( inDir.z, inDir.y, -1 * inDir.x );
        inDir = temp;

        float cosAlpha = 0.0f;
    
        // Let bestCosAlpha be, initially, the WORST value possible 
        // cosAlpha of 1.0 = alphaBetwIn&OutRoads of 0 = 180 degree turn! BAD!
        // As we iterate through the OUT roads, we want to find one with most 
        // alpha, or smallest cosAlpha
        //
        float bestCosAlpha = 1.0f; 
        bool stillLookingForFirstRoad = true;

        unsigned int i = 0;
        for( i ; i<mnRoadsOut ; ++i )
        {
            // Grab normal of leading edge of the first segment of OUT road
            segment = mRoadListOut[i]->GetRoadSegment(0);
            segment->GetEdgeNormal(0, outDir); 

            // *** NOTE ***
            // We rely on the assumption that the intersection is HORIZONTALLY FLAT
            // Else, we'll have to project the In & Out vectors onto the plane of 
            // the intersection.
            outDir.y = 0.0f;
            outDir.Normalize(); // *** SQUARE ROOT! *** 

            // Skip this OUT road if it just goes back the way we came
            // This prevents U-turn at an intersection when there are 
            // other roads to take, even if they don't turn the way
            // we want. The decision is really up to traffic control or
            // some other entity; for now, we hardcode it in the Intersection
            if( outDir.Equals( origInDir * -1, 0.05f ) )
            {
                continue;
            }

            cosAlpha = inDir.Dot(outDir);
        
            // *** NOTE ***
            // If contention arises between a left and a right turn that 
            // both have bestCosAlpha, this inequality will favor the last
            // best match. 
            if( stillLookingForFirstRoad || (cosAlpha <= bestCosAlpha) )
            {
                Lane* laneCandidate = NULL;
                unsigned int laneIndexCandidate = 0;
                FindGoodTrafficLane( *mRoadListOut[i], preferredLaneIndex, laneCandidate, laneIndexCandidate );

                if( laneCandidate != NULL )
                {
                    stillLookingForFirstRoad = false;
                    bestCosAlpha = cosAlpha; 
                    outRoad = mRoadListOut[i];
                    outLane = laneCandidate;
                    outLaneIndex = laneIndexCandidate;
                }
            }
        }
    }
}

void Intersection::GetOtherIntersection
( 
    bool useMultiplier,
    Intersection* knownIntersection, 
    Intersection*& otherIntersection,
    RoadManager::ShortestRoad*& road 
)
{
    rAssert( knownIntersection != NULL );

    // works only for non-big intersections
    if( mBigIntersection != NULL )
    {
        rAssert( false );
        otherIntersection = NULL;
        road = NULL;
        return;
    }

    SwapArray<RoadManager::ShortestRoad>* array = useMultiplier ?
        &mShortestRoadsToAdjacentIntersectionsWithMultiplier :
        &mShortestRoadsToAdjacentIntersectionsNoMultiplier ;

    // I'm a non-big intersection if I'm connected to either 1 or 2 other 
    // intersections
    rAssert( 1 <= array->mUseSize && array->mUseSize <= 2 );

    for( int i=0; i<array->mUseSize; i++ )
    {
        RoadManager::ShortestRoad* shortRoad = &((*array)[i]);
        rAssert( shortRoad );

        Intersection* otherIn = NULL;
        if( shortRoad->isOutRoad )
        {
            otherIn = (Intersection*) shortRoad->road->GetDestinationIntersection();
        }
        else
        {
            otherIn = (Intersection*) shortRoad->road->GetSourceIntersection();
        }
        if( otherIn != knownIntersection )
        {
            road = shortRoad;
            otherIntersection = otherIn;
            return;
        }
    }
}
