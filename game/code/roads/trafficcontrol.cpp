/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Traffic Control

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/

#include <roads/trafficcontrol.h>
#include <roads/intersection.h>

void TrafficLight::Update( unsigned int dt )
{
	m_uiElapsedTime += dt;

	if ( m_uiElapsedTime > m_uiSwitchTime )
	{
		m_uiSwitchTime = m_uiElapsedTime + TrafficLightSwitchTime;
		SwitchControl();
	}
}

void TrafficLight::SwitchControl( void )
{
	m_tState++;

	m_tState = m_tState % NUM_STATES;

	if ( ADVANCE_GREEN == m_tState )
	{
		// we must have rolled through all the other states, so change the direction.
		// do a bitwise not.
		m_tRoadsToGo = ~m_tRoadsToGo;
		// mask out the high bits, for clarity.
		m_tRoadsToGo = m_tRoadsToGo & 0x0000000F;
	}
	// notify the intersection of the control change.
	m_pIntersection->AdvanceTraffic( m_tRoadsToGo, m_tState );
}




void NWayStop::Update( unsigned int dt )
{
    //
    // After elapsed time, tell next road to go. The road should tell all 
    // of its lanes to go.
    //
    Intersection* intersection = (Intersection*)m_pIntersection;
    if( intersection->mWaitingRoads.mUseSize > 0 )
    {
        if( mTurnTimeLeft < 0 )
        {
            mTurnTimeLeft = NWAY_TURN_MILLISECONDS;
            
            // tell the road to go...
            //rAssert( m_pIntersection->GetType() == Intersection::N_WAY );

            // damn constants
    	    ((Intersection*)m_pIntersection)->AdvanceNextWaitingRoad();
        }
        else
        {
            mTurnTimeLeft -= dt;
        }
    }
    else
    {
        mTurnTimeLeft = NWAY_TURN_MILLISECONDS;
    }
    /*
	if ( m_pIntersection->IsIntersectionClear() )
	{
		m_tState++;

		m_tState = m_tState % NUM_STATES;

		// do a bitwise not.
		m_tRoadsToGo = ~m_tRoadsToGo;
		// mask out the high bits, for clarity.
		m_tRoadsToGo &= 0x0000000F;

		m_pIntersection->AdvanceTraffic( m_tRoadsToGo, m_tState );
	}
    */
}

void CourtesyStop::Update( unsigned int dt )
{
}