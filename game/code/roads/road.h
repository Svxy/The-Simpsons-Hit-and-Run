/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Road

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/
#ifndef ROAD_HPP_
#define ROAD_HPP_

#include <radmath/radmath.hpp>
#include <p3d/p3dtypes.hpp>
#include <string.h>

#ifdef TOOLS
#include <p3d/entity.hpp>
#endif

// forward declarations
class Intersection;
class Lane;
class RoadSegment;

// 3 lanes in one direction maximum.
const unsigned int MAX_LANES = 3;


#ifdef TOOLS
#define IS_ENTITY : public tEntity
#else
#define IS_ENTITY
#endif

class Road IS_ENTITY
{
public:  // methods

	Road( void );
    ~Road( void );

    void SetNumLanes( int count );
	unsigned int GetNumLanes( void ) const;

    // Allocate memory for numSegments roadSegments.
    //
    void AllocateSegments( unsigned int numSegments );

	// Return Lane at position 'laneId'.  return NULL if lane doesn't exist.
	//
	Lane *GetLane( unsigned int laneId ) const;

	// Add a road segment.
	//
	void AddRoadSegment( RoadSegment* pRoadSegment );

	// Creates lanes based on RoadSegment list.
	//
	void CreateLanes( void );

	// return the intersection at the beginning of this road.
	//
	const Intersection *GetSourceIntersection( void ) const
		{ return mpSourceIntersection; }

	// return the intersection at the end of this road.
	//
	const Intersection *GetDestinationIntersection( void ) const
		{ return mpDestinationIntersection; }

	// set the intersection at the beginning of this road.
	//
	void SetSourceIntersection( Intersection *pIntersection )
		{ mpSourceIntersection = pIntersection; }

	// set the intersection at the end of this road.
	//
	void SetDestinationIntersection( Intersection *pIntersection )
		{ mpDestinationIntersection = pIntersection; }

    // Return the point where the road attaches to the incoming intersection.
    //
    void GetSourceIntersectionJoinPoint( rmt::Vector& out );
    
    // Return the point where the road attaches to the outgoing intersection.
    //
    void GetDestinationIntersectionJoinPoint( rmt::Vector& out );

    unsigned int GetNumRoadSegments( void ) const
        { return mnRoadSegments; }

    unsigned int GetMaxRoadSegments( void ) const
        { return mnMaxRoadSegments; }

    RoadSegment* GetRoadSegment( unsigned int index ) const;

    int GetRoadSegmentAtPoint( const rmt::Vector& point, RoadSegment** ppOutRoadSegment, float& in, float& lateral, int hint ) const;

    bool IsPointInRoadSegment( int index, const rmt::Vector& point, float& in, float& lateral ) const;

    void Render( const tColour& colour );

    void SetSpeed( unsigned int speed );
    unsigned int GetSpeed() const;

    void SetDifficulty( unsigned int diff );
    unsigned int GetDifficulty() const;

    void SetShortCut( bool is );
    bool GetShortCut() const;

    void SetDensity( unsigned int density );
    unsigned int GetDensity() const;

    void FindRoadSegmentAtDist( float iDist, RoadSegment** ippRoadSegment );
    //---------------Helpful methods
    // returns true if got to maxdist on the current road
    // false if got to the end of the road first
    bool FindSegmentAhead( float& dist, 
                           const float maxDist, 
                           const int currentIndex,
                           RoadSegment** segment ) const;
    bool FindSegmentBehind( float& dist, 
                            const float maxDist, 
                            const int currentIndex,
                            RoadSegment** segment ) const;

#if !defined( RAD_RELEASE ) && !defined( TOOLS )
    void SetName( const char* name ) { strcpy( mName, name ); };
    const char* GetName() { return mName; }
#endif

    const rmt::Sphere& GetBoundingSphere() const { return mSphere; };

    float GetRoadLength();
    void SetRoadLength( float len );

private:  // data
	// Intersection at the start of the road.
	//
	const Intersection *mpSourceIntersection;

	// Intersection at the end of the road.
	//
	const Intersection *mpDestinationIntersection;

	// List of Lanes in a left to right order.
	//
	Lane *mLaneList;

	// how many lanes on this road.
	//
	unsigned int mnLanes;

	// The pointer to the head of the RoadSegment Array.
	//
	RoadSegment** mppRoadSegmentArray;

    // How many have we allocated.
    //
    unsigned int mnMaxRoadSegments;

    // A count of road segments.
    //
    unsigned int mnRoadSegments;

    /////////////////////////
    // TODO: Get rid of this
    // A bounding box in world space for the road.
    //
    rmt::Box3D mBox;

    // A bounding sphere in world space for the road.
    //
    rmt::Sphere mSphere;

    unsigned int mSpeed;
    unsigned int mDensity;
    unsigned int mDifficulty;
    bool mIsShortCut;
    
#ifndef RAD_RELEASE
    char mName[256];
#endif

    float mLength;
};



inline void Road::SetSpeed( unsigned int speed )
{
    mSpeed = speed;
}
inline unsigned int Road::GetSpeed() const
{
    return mSpeed;
}
inline void Road::SetDifficulty( unsigned int diff )
{
    mDifficulty = diff;
}
inline unsigned int Road::GetDifficulty() const
{
    return mDifficulty;
}
inline void Road::SetShortCut( bool is )
{
    mIsShortCut = is;
}
inline bool Road::GetShortCut() const
{
    return mIsShortCut;
}
inline unsigned int Road::GetDensity() const
{
    return mDensity;
}
inline float Road::GetRoadLength()
{
    return mLength;
}
inline void Road::SetRoadLength( float len )
{
    rAssert( len >= 0.0f );
    mLength = len;
}
inline void Road::SetNumLanes( int count )
{
    mnLanes = count;
}
inline unsigned int Road::GetNumLanes( void ) const
{ 
    return mnLanes; 
}
/*
inline float Road::GetWidth() const
{
    return mnLanes * mfLaneWidth;
}
*/

#endif
