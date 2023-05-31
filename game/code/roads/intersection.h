/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Intersection

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/

#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include <radmath/radmath.hpp>
#include <p3d/p3dtypes.hpp>
#include <p3d/entity.hpp>
#include <roads/TrafficControl.h>
#include <roads/lane.h>
#include <roads/geometry.h>
#include <roads/roadmanager.h>
#include <render/culling/swaparray.h>

class Road;
class CNavVertex;
class TriggerLocator;
class AnimEntityDSG;

// somewhat arbitrary limit of MAX_ROADS way intersection.
const int MAX_ROADS = 8;
/*
	AI is going to want to know these things about moving along a road:
	1.  Am I at an intersection?
	2.  What is my position and orientation?
	3.	Do I want to change lanes?

	AI is going to want to know these things when it is at an intersection:
	1.  Can I move through this intersection.

	AI is going to want to know these things about moving through intersection.
	1.  Can I turn left from this lane?
	2.  Can I turn right from this lane?
	3.  Can I go straight in this lane?

*/
#ifdef TOOLS
#define IS_ENTITY : public tEntity
#else
#define IS_ENTITY
#endif

class Intersection IS_ENTITY
{
public:
	Intersection(  );
    Intersection( TriggerLocator* pLocator );
	~Intersection( void );
    
    enum Type
    {
        NO_STOP,
        N_WAY
    };    
    
    Type GetType() const;
    void SetType( Type type );
	// Get the count of Roads into the intersection.
	//unsigned int SizeRoadsIn( void ) const { return mnRoadsIn; }

	// Get the count of Roads out of the intersection.
	//unsigned int SizeRoadsOut( void ) const { return mnRoadsOut; }

	// Get the count of Roads into and out of the intersection.
	//unsigned int SizeRoads( void ) const { return mnRoadsIn + mnRoadsOut; }

    // add a road to the in list.
    void AddRoadIn( Road *pRoad );

    // add a road to the out list.
    void AddRoadOut( Road *pRoad );

    void FindGoodTrafficLane( const Road& road, 
                              unsigned int preferredLaneIndex, 
                              Lane*& outLane, 
                              unsigned int& outLaneIndex );

    void GetLeftTurnForTraffic( const Road& inRoad, 
                                unsigned int preferredLaneIndex,
                                Road*& outRoad, 
                                Lane*& outLane, 
                                unsigned int& outLaneIndex );

    void GetRightTurnForTraffic( const Road& inRoad, 
                                 unsigned int preferredLaneIndex,
                                 Road*& outRoad, 
                                 Lane*& outLane, 
                                 unsigned int& outLaneIndex );

    void GetStraightForTraffic( const Road& inRoad, 
                                unsigned int preferredLaneIndex,
                                Road*& outRoad, 
                                Lane*& outLane, 
                                unsigned int& outLaneIndex );

    // So easy, it's almost free!
    //
    const Road* GetRoadIn( unsigned int index ) const;
    const Road* GetRoadOut( unsigned int index ) const;
    unsigned int GetNumRoadsIn() const;
    unsigned int GetNumRoadsOut() const;

	// Is the intersection clear of all cars.
	//
	bool IsIntersectionClear( void ) const;

	// Simulate the intersection when there are cars nearby.
	//
	void Update( unsigned int dt );

	// advance the traffic in roads 'RoadMask' according to light state.
	//
	void AdvanceTraffic( unsigned int RoadMask, unsigned int state ) const;

	// returns the Intersection location in world space.
	//
    void GetLocation( rmt::Vector& location ) const;
    void SetLocation( rmt::Vector& location );

    // Temp method to display.
    //
    void Render( void ) const;

    void SetRadius( float radius );
    float GetRadius( void ) const
    { return mfRadius; }

    // Sorts the roads into clockwise order.
    //
    void SortRoads( void );

    void SetName( const char* name );
    tUID GetNameUID() const;

    bool IsPointInIntersection( rmt::Vector& point ) const;

    TrafficControl* GetTrafficControl();
    void AdvanceNextWaitingRoad();

    // Works only for non-big intersections
    //
    // Given an intersection connected to myself, I'll search through
    // my list of shortest roads to other intersections to return the 
    // other intersection adjacent to me.
    //
    // The "useMultiplier" flag tells us to use the list of shortestroads 
    // whose "shortest" descriptive is defined using the road cost augmented 
    // by AGAINST_TRAFFIC_COST_MULTIPLIER
    //
    void GetOtherIntersection( 
        bool useMultiplier, 
        Intersection* knownIntersection, 
        Intersection*& otherIntersection,
        RoadManager::ShortestRoad*& road );

    void LinkAnimEntity( AnimEntityDSG* ipAnimEntity, int iIndex )
    {
        int i = iIndex - mpAnimEntityList.mUseSize + 1;
        if(i>0)  mpAnimEntityList.AddUse(i);

        mpAnimEntityList[iIndex] = ipAnimEntity;
    }

    AnimEntityDSG* AnimEntity(int iIndex)
    {
        return mpAnimEntityList[iIndex];
    }

public:
    SwapArray<Road*> mWaitingRoads; // list of roads waiting for a turn to go
    SwapArray<Road*> mOutgoingShortcuts;
    SwapArray<RoadManager::ShortestRoad> mShortestRoadsToAdjacentIntersectionsWithMultiplier;
    SwapArray<RoadManager::ShortestRoad> mShortestRoadsToAdjacentIntersectionsNoMultiplier;
    RoadManager::BigIntersection* mBigIntersection;
    int mIndex; // index to RoadManager::mIntersections[pool==0] list

private:
    SwapArray<AnimEntityDSG*> mpAnimEntityList;
	// methods
	//

	// Find the road pointer in the road list.
	//
	int FindRoadIn( const Road* pRoad ) const;

	// Find the road pointer in the road list.
	//
	int FindRoadOut( const Road* pRoad ) const;

    void PopulateShortestRoads( SwapArray<RoadManager::ShortestRoad>& roadsToAdjacentInts, bool useMultiplier );

private:  // data
	// Roads are Clockwise ordered.
	//
	//		0
	//		|
	//		|
	//3 ____|____ 1
	//		|
	//		|
	//		|
	//		2
	//
	// a list of the roads leading in to this intersection.
	//
	Road* mRoadListIn[ MAX_ROADS ];

	// a list of the roads leading out of this intersection.
	//
	Road* mRoadListOut[ MAX_ROADS ];

    //
	// A pointer to the traffic control object for this intersection.
	// & the different controls this intersection might have
    //
	TrafficControl* mpTrafficControl;
    TrafficLight mLightControl;
    NWayStop mNWayControl;


	// the actual number of roads in. <= MAX_ROADS.
	//
	unsigned int mnRoadsIn;

	// the actual number of roads out. <= MAX_ROADS.
	//
	unsigned int mnRoadsOut;

	// the location of the intersection in world space.
	//
	rmt::Vector mLocation;

	// the rotation around the y vector of the intersection.
	//
	float mfRotation;

    float mfRadius;

    Type mType;

    tUID mNameUID;
};



inline void Intersection::SetName( const char* name )
{
#ifdef TOOLS
    tEntity::SetName( name );
#endif
    mNameUID = tEntity::MakeUID( name );
}
inline tUID Intersection::GetNameUID() const
{
    return mNameUID;
}
inline void Intersection::SetLocation( rmt::Vector& location )
{
    mLocation = location;
}
inline void Intersection::SetRadius( float radius )
{
    mfRadius = radius;
}
inline const Road* Intersection::GetRoadIn( unsigned int index ) const
{
    if(index >= mnRoadsIn)
    {
        return NULL;
    }

    return mRoadListIn[ index ];
}
inline const Road* Intersection::GetRoadOut( unsigned int index ) const
{
    if(index >= mnRoadsOut)
    {
        return NULL;
    }

    return mRoadListOut[ index ];
}
inline unsigned int Intersection::GetNumRoadsIn() const
{
    return mnRoadsIn;
}
inline unsigned int Intersection::GetNumRoadsOut() const
{
    return mnRoadsOut;
}
inline TrafficControl* Intersection::GetTrafficControl() 
{
    return mpTrafficControl;
}




#endif