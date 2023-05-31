/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Traffic Control

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/

#ifndef TRAFFICCONTROL_HPP_
#define TRAFFICCONTROL_HPP_

class Intersection;

// An Object that controls the flow of traffic through an intersection.
class TrafficControl
{
public:
	enum 	
    {
        RED,
        YELLOW,
		GREEN,
		ADVANCE_GREEN,
		NUM_STATES
	};
	enum
	{
		ROAD_ZERO = 1 << 0,
		ROAD_ONE = 1 << 1,
		ROAD_TWO = 1 << 2,
		ROAD_THREE = 1 << 3
	};
	// constructor.
	TrafficControl() : m_pIntersection( 0 ) {}
	// destructor.
	virtual ~TrafficControl() {}
	// returns which intersection this traffic control belongs to.
	const Intersection *GetIntersection( void ) const { return m_pIntersection; }
	// sets which intersection this traffic control belongs to.
	void SetIntersection( const Intersection *pIntersection ) { m_pIntersection = pIntersection; }

	// update the traffic control state.  Each one follows different logic.
	virtual void Update( unsigned int dt ) = 0;
protected:
	// the intersection this traffic control belongs to.
	const Intersection *m_pIntersection;
	// the state of the traffic light in the green direction.  All other directions STOP.
	unsigned int m_tState;
	// a bitmask representing which roads the traffic control advance commands applies to.
	unsigned int m_tRoadsToGo;

};

// Allows flow of traffic through a 4 way intersection in 2 opposing, non crossing roads at a time.
class TrafficLight
:
public TrafficControl
{
public:
	TrafficLight() : TrafficControl(), m_uiElapsedTime( 0 ), m_uiSwitchTime( 0 ) {}
	void Update( unsigned int dt );
private:
	void SwitchControl( void );
	enum eTrafficLightSwitchTime
	{
		TrafficLightSwitchTime = 10000
	};
	// the elapsed time.
	unsigned int m_uiElapsedTime;
	// the control signal should switch when the m_uiElapsedTime reaches this value.
	unsigned int m_uiSwitchTime;
};

// Allows flow of traffic through an N way intersection, 2 lanes at a time in a CW order.
class NWayStop
:
public TrafficControl
{
public:
    NWayStop() : TrafficControl(), mTurnTimeLeft( NWAY_TURN_MILLISECONDS ) {}
	void Update( unsigned int dt );
    enum eTrafficTurnTime
    {
        NWAY_TURN_MILLISECONDS = 3500
    };
private:
    int mTurnTimeLeft;
};

// advances cars when intersection is clear. stopped cars are advanced in a CW order.
class CourtesyStop
:
public TrafficControl
{
public:
	void Update( unsigned int dt );
};

#endif