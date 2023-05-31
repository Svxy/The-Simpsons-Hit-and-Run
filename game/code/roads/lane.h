/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Lane

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/

#ifndef LANE_HPP_
#define LANE_HPP_

#include <radmath/radmath.hpp>
#include <vector>
#include <list>
#include <roads/geometry.h>
#include <render/culling/swaparray.h>
#include <worldsim/traffic/trafficvehicle.h>

class Road;
class Lane;

// the read only interface to the lane.
struct ILaneInformation
{
	// get the max allowable speed.
	virtual float GetSpeedLimit( void ) const = 0;
    virtual int GetDensity( void ) const = 0;
	// returns the parent road of this lane.
	virtual const Road *GetRoad( void ) const = 0;

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    // For Rendering 
	// point is assigned the world position of the start of the lane
	virtual void GetStart( rmt::Vector &point ) const = 0;
	// point is assigned the world position of the end of the lane
	virtual void GetEnd( rmt::Vector &point ) const = 0;
    // given a parametric time, updates the 'point.' returns true if at the end of the lane.
    virtual bool GetPoint( float t, rmt::Vector& point ) const = 0;
#endif

};

// the write only interface to the lane.
struct ILaneControl
{
	// set the max allowable speed.
	virtual void SetSpeedLimit( float metrespersecond ) = 0;
	// set the max allowable num traffic cars.
    virtual void SetDensity( int numCars ) = 0;
};

class Lane
:
public ILaneInformation,
public ILaneControl
{
public:

	// ctor
    Lane( void );
    ~Lane();

	// grab the spline associated with this lane.
	void Create( int density, Road* parentRoad );


    //////////////////////////////////////////////////
    // Implementing ILaneInformation
    //
    float GetSpeedLimit( void ) const;
    int GetDensity( void ) const;
    const Road* GetRoad( void ) const;

#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    // Rendering stuff
    void GetStart( rmt::Vector &point ) const;
    void GetEnd( rmt::Vector &point ) const;
    bool GetPoint( float t, rmt::Vector& point ) const;
    void GetPoint( unsigned int index, rmt::Vector* point ) const;
#endif
    //////////////////////////////////////////////////////

    ///////////////////////////////////////////////////
	// Implementing ILaneControl
	//
    void SetSpeedLimit( float metrespersecond );
    void SetDensity( int numCars );
    void NotifyWaitingTraffic( unsigned int state );
    ////////////////////////////////////////////////////
    

    // Temp method to display.
    void Render( void );

#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    void AllocatePoints( unsigned int numPoints );
    unsigned int GetNumPoints( void ) const;
    void SetPoint( unsigned int index, const rmt::Vector& point );
#endif

public:
    // the vehicle at the front of all lane's traffic, waiting at the intersection.
    TrafficVehicle* mWaitingVehicle;

    // Add traffic to the lane's internal list to keep track of how many have been
    // added to the lane (to conform to the lane's desired density).
    SwapArray<TrafficVehicle*> mTrafficVehicles;


private:
    const Road *mpParentRoad;   // the road that owns this lane.
    float mfSpeedLimit;         // the maximum allowable speed for this lane.
    int mDesiredDensity;     // the desired density ( in total cars )

#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    // Rendering stuff
    rmt::Vector* mPoints;
    unsigned int mNumPoints;
#endif 

};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// Lane::SetSpeedLimit
//=============================================================================
// Description: Comment
//
// Parameters:  ( float metrespersecond )
//
// Return:      void 
//
//=============================================================================
inline void Lane::SetSpeedLimit( float metrespersecond )
{
    mfSpeedLimit = metrespersecond;
}


#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    inline bool Lane::GetPoint( float t, rmt::Vector& point ) const
    {
        //t will equal what segment 0 - mpParentRoad->GetNumRoadSegments()
        // *plus* 0-0.999999... for it's position in the road.
        return false;
    }

    inline void Lane::GetPoint( unsigned int index, rmt::Vector* point ) const
    {
        rAssert( index < mNumPoints );
        *point = mPoints[index];
    }
#endif



inline const Road* Lane::GetRoad( void ) const
{
    return mpParentRoad;
}
inline float Lane::GetSpeedLimit() const
{
    return mfSpeedLimit;
}
inline void Lane::SetDensity( int numCars )
{
    mDesiredDensity = numCars;
}
inline int Lane::GetDensity() const
{
    return mDesiredDensity;
}


#endif