/*===========================================================================
 Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.

 Component:   Lane

 Description:


 Authors:     Travis Brown-John

 Revisions    Date            Author      Revision
              2001/02/02      Tbrown-John Created

===========================================================================*/

#include <roads/lane.h>
#include <roads/road.h>
#include <roads/trafficcontrol.h>


#ifndef TOOLS
#include <memory/srrmemory.h>
#else
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP()
#endif

#ifndef TOOLS
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/trafficlocomotion.h>
#endif

#include <raddebug.hpp>


//=============================================================================
// Lane::Lane
//=============================================================================
// Description: Constructor
//
// Parameters: None
//
//=============================================================================
Lane::Lane() : 
#ifndef TOOLS
    mWaitingVehicle( NULL ),
#endif
#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    mDesiredDensity( 5 ),
    mPoints( NULL ),
    mNumPoints( 0 )
#else
    mDesiredDensity( 5 )
#endif
{
}

Lane::~Lane()
{
#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    if ( mPoints )
    {
        delete[] mPoints;
        mPoints = NULL;
    }
#endif
    mTrafficVehicles.Clear();
}

void Lane::Create( int density, Road* parentRoad )
{
    // DUSIT [Oct21,2002]
    // Took out this assert cuz we need to be able to set density to 0 to
    // prevent traffic cars from going onto that lane. Why TBJ put this 
    // assert in there in the first place
    //rAssertMsg( fDensity > 0.0f, "Desired Density must be greater than 0.0f" );

    mDesiredDensity = density;
    mpParentRoad = parentRoad;
    if( density > 0 )
    {
        mTrafficVehicles.Allocate( density );
    }
}

#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    void Lane::GetStart( rmt::Vector &point ) const
    {
	    GetPoint( 0, &point );
    }
    void Lane::GetEnd( rmt::Vector &point ) const
    {
	    GetPoint( GetNumPoints() - 1, &point );
    }
#endif


    
// Notify waiting traffic that traffic control signal has changed.
void Lane::NotifyWaitingTraffic( unsigned int state )
{
#ifndef TOOLS
    if( TrafficControl::GREEN == state )
    {
        if( mWaitingVehicle != NULL )
        {
            Vehicle* v = mWaitingVehicle->GetVehicle();
            rAssert( v != NULL );

            // If the vehicle was removed from traffic, don't 
            // bother with notification. This can happen if 
            // player moves away from intersection.
            // Also, if the vehicle is no longer in the intersection, 
            // we shouldn't just transit it back to DRIVING (it 
            // could be in any other state by now!)
            if( mWaitingVehicle->GetIsActive() && v->mVehicleType == VT_TRAFFIC )
            {
                if( this == v->mTrafficLocomotion->GetAILane() &&
                    v->mTrafficLocomotion->GetAI()->GetState() == 
                      TrafficAI::WAITING_AT_INTERSECTION )
                {
                    v->mTrafficLocomotion->SetAIState( TrafficAI::DRIVING );
                }
            }
            mWaitingVehicle = NULL;
        }
    }
    /*
	if ( TrafficLight::ADVANCE_GREEN == state && CanTurnLeft() )
	{
		// Notify all waiting traffic.
		//
	}
	else if ( TrafficLight::GREEN == state && !CanTurnLeft() )
	{
		// Notify all waiting traffic.
		//
	}
    */
#endif
}

#if defined(RAD_DEBUG) || defined(RAD_TUNE)

    void Lane::AllocatePoints( unsigned int numPoints )
    {
        MEMTRACK_PUSH_GROUP( "Lane" );
        rAssert( mPoints == NULL );   

        //TODO: GET RID OF THIS.
    #ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    #endif
    #endif
        mPoints = new rmt::Vector[ numPoints ];
        rAssert( mPoints );

        mNumPoints = numPoints;

    #ifndef TOOLS
        #ifdef RAD_GAMECUBE
            HeapMgr()->PopHeap( GMA_GC_VMM );
        #else
            HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
        #endif
    #endif

        MEMTRACK_POP_GROUP( "Lane" );
    }

    unsigned int Lane::GetNumPoints() const
    {
        return mNumPoints;
    }

    void Lane::SetPoint( unsigned int index, const rmt::Vector& point )
    {
        rAssert( index < mNumPoints );

        mPoints[index] = point;
    }

#endif

