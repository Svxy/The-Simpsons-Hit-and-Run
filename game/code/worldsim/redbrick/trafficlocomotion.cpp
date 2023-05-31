//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficlocomotion.cpp
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Locomote through intersections -- Dusit Eakkachaichanvet
//              04/24/2002 + Created -- Greg Mayer
//
//=============================================================================
#include <poser/poseengine.hpp>
#include <raddebugwatch.hpp>
#include <radtime.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <worldsim/redbrick/trafficlocomotion.h>
#include <worldsim/redbrick/vehicle.h>

#include <debug/profiler.h>
#include <worldsim/worldphysicsmanager.h>

#include <roads/roadmanager.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/roadsegmentdata.h>
#include <roads/lane.h>
#include <roads/intersection.h>
#include <roads/geometry.h>

const float TrafficLocomotion::SECONDS_BETW_HISTORY_UPDATES = 0.005f;
static const float SECONDS_BETW_CHECKS_FOR_FREE_LANE = 5.0f;
static const float TRAFFIC_WHEEL_GAP_HACK = 0.1f;

#define FACING_HISTORY
// Need to use pos history to smooth out motion along curve 
// Fortunately, this also causes the actual (apparent) position 
// to be located further back from the calculated position, so 
// turning looks better.
// Unfortunately, averaging means we can't stop on a dime, 
// like we want to ... 
#define POS_HISTORY 

//------------------------------------------------------------------------
TrafficLocomotion::TrafficLocomotion(Vehicle* vehicle) :
    VehicleLocomotion(vehicle),
    mMyAI( NULL )
{

    mMyAI = new TrafficAI( vehicle );
    mMyAI->AddRef();

#ifdef DEBUGWATCH
    if( vehicle && vehicle->mVehicleType == VT_TRAFFIC )
    {
        mMyAI->RegisterAI();
    }
#endif


    mVehicle = vehicle;
    mIsInIntersection = false;
    mCurrWay = -1;
    mCurrPathLength = 0.0f;
    mCurrPathLocation = 0.0f;
    mActualSpeed = 0.0f;
    mWays = NULL;
    mNumWays = 0;
    mSecondsTillCheckForFreeLane = SECONDS_BETW_CHECKS_FOR_FREE_LANE;

    ////////////////////////////////////////////////
    // HISTORY stuff
    ////////////////////////////////////////////////
#ifdef FACING_HISTORY
    rmt::Vector heading( 0.0f, 0.0f, 0.0f );
    /*
    if( mVehicle != NULL )
    {
        mVehicle->GetHeading( &heading );
    }
    */
    mFacingHistory.Init( heading );
#endif

#ifdef POS_HISTORY
    rmt::Vector pos( 0.0f, 0.0f, 0.0f );
    mPosHistory.Init( pos );
#endif
    ////////////////////////////////////////////////

    mLaneChangeProgress = 0.0f; 
    mLaneChangeDist = 0.0f;
    mLaneChangingFrom = 0; 
    mOutLaneT = 0.0f;

    mSecondsSinceLastAddToHistory = 0.0f;
}


//------------------------------------------------------------------------
TrafficLocomotion::~TrafficLocomotion()
{
    mMyAI->ReleaseVerified();
}

void TrafficLocomotion::InitPos( const rmt::Vector& pos )
{
#ifdef POS_HISTORY
    mPosHistory.Init( pos );
#endif

    mSecondsSinceLastAddToHistory = 0.0f;

    /////////////////////////////////////////////
    // Adjust ground height (unfortunately we need to do this
    // because we use pos averaging (so our y value is off)

    mPrevPos = pos;

    rmt::Vector groundPosition, outnorm;
    bool bFoundPlane = false;

    groundPosition = pos;
    outnorm.Set( 0.0f, 1.0f, 0.0f );

    GetIntersectManager()->FindIntersection(
        groundPosition,   // IN
        bFoundPlane,      // OUT
        outnorm,          // OUT
        groundPosition    // OUT
        );  

    if( bFoundPlane )
    {
        mPrevPos.y = groundPosition.y;
    }
    ///////////////////////////////////////////////

}


void TrafficLocomotion::InitFacing( const rmt::Vector& facing )
{
    rAssert( rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) );
#ifdef FACING_HISTORY
    mFacingHistory.Init( facing );
#endif
    mSecondsSinceLastAddToHistory = 0.0f;

}
//=============================================================================
// TrafficLocomotion::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::Init()
{
    mMyAI->Init();
}

//=============================================================================
// TrafficLocomotion::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ( TrafficAI::Behaviour behaviour, 
//                unsigned int behaviourModifiers, 
//                Vehicle* vehicle, 
//                Lane* lane,
//                unsigned int laneIndex,
//                RoadSegment* segment,
//                unsigned int segmentIndex,
//                float t, 
//                float kmh  )
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::Init( Vehicle* vehicle, 
                              Lane* lane,
                              unsigned int laneIndex,
                              RoadSegment* segment,
                              unsigned int segmentIndex,
                              float t, 
                              float mps )
{
    mMyAI->Init( mVehicle, lane, laneIndex, segment, segmentIndex, t, mps );
}

//=============================================================================
// TrafficLocomotion::InitVehicleAI
//=============================================================================
// Description: Comment
//
// Parameters:  ( Behaviour behaviour, unsigned int behaviourModifiers, Vehicle* vehicle )
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::InitVehicleAI( Vehicle* vehicle )
{
    mMyAI->SetVehicle( vehicle );
}

//=============================================================================
// TrafficLocomotion::InitLane
//=============================================================================
// Description: Comment
//
// Parameters:  ( Lane* lane, unsigned int laneIndex, float mps )
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::InitLane( Lane* lane, unsigned int laneIndex, float mps )
{
    mMyAI->SetLane( lane );
    mMyAI->SetLaneIndex( laneIndex );
    mMyAI->SetAISpeed( mps );
}

//=============================================================================
// TrafficLocomotion::InitSegment
//=============================================================================
// Description: Comment
//
// Parameters:  ( RoadSegment* segment, unsigned int segmentIndex, float t )
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::InitSegment( RoadSegment* segment, unsigned int segmentIndex, float t )
{
    mMyAI->SetSegment( segment );
    mMyAI->SetSegmentIndex( segmentIndex );
    mMyAI->SetLanePosition( t );      
    mMyAI->SetIsInIntersection( false );
    mIsInIntersection = false;
}


//=============================================================================
// TrafficLocomotion::UpdateVehicleGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::UpdateVehicleGroundPlane()
{
    rmt::Vector p, n;
    p.Set(0.0f, 0.0f, 0.0f);
    n.Set(0.0f, 1.0f, 0.0f);
    
    p = mVehicle->GetPosition();
    p.y -= 5.0f;     // just to be safe
    
      
    mVehicle->mGroundPlaneWallVolume->mPosition = p;
    mVehicle->mGroundPlaneWallVolume->mNormal = n;
    
    
    sim::CollisionObject* co = mVehicle->mGroundPlaneSimState->GetCollisionObject();
    co->PostManualUpdate();
    

}



//------------------------------------------------------------------------
void TrafficLocomotion::PreCollisionPrep(bool firstSubstep)
{
    UpdateVehicleGroundPlane();
}

//=============================================================================
// TrafficLocomotion::PreSubstepUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  (mVehicle* mVehicle)
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::PreSubstepUpdate()
{
    if( !mMyAI->mIsActive )
    {
        return;
    }

    // perform whatever update here
}


//=============================================================================
// TrafficLocomotion::PreUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::PreUpdate()
{
    poser::Pose* pose = mVehicle->mPoseEngine->GetPose();
   
    mVehicle->mPoseEngine->Begin( false );
    //mVehicle->mPoseEngine->Begin(true);
   
     
    int i;
    for(i = 0; i < 4; i++)
    {
        Wheel* wheel = mVehicle->mWheels[i];

        poser::Joint* joint = pose->GetJoint(mVehicle->mWheelToJointIndexMapping[i]);
        rmt::Vector trans = joint->GetObjectTranslation();

        //trans.y -= mVehicle->mWheels[i]->mLimit;                                          
        // TODO - verify that the -= is the thing to do here
        //trans.y -= wheel->mLimit;
        trans.y += TRAFFIC_WHEEL_GAP_HACK;

        joint->SetObjectTranslation(trans);                

    }
    

}


//=============================================================================
// TrafficLocomotion::UpdateAI
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::UpdateAI(unsigned int ms)
{
    // TrafficAI::Update takes timestep in seconds
    mMyAI->Update(float(ms)/1000.0f);
}



void TrafficLocomotion::StopSuddenly( rmt::Vector& pos, rmt::Vector& facing )
{
#ifdef POS_HISTORY
    // re-init history with pos
    mPosHistory.GetAverage( pos );
    mPosHistory.Init( pos );
#else
    pos = mPrevPos;
#endif

#ifdef FACING_HISTORY
    mFacingHistory.GetNormalizedAverage( facing );
    mFacingHistory.Init( facing );
#endif

    mSecondsSinceLastAddToHistory = 0.0f;

    // TODO:
    // When we stop suddenly, our t value is slightly ahead of 
    // us (because we've clobbered our pos history with the
    // vehicle's current position). When we accelerate again, 
    // we'll dump the t position into the history, causing 
    // the vehicle to lurch forward inertialessly, throwing off
    // the average.
    // 
    // So we reset our t value at this point to where we are.
    // This is rather tricky to do: we could be anywhere...
    // - If we're on a road segment, that's perfect. 
    // - If we're not on a road segment, but we're in 
    //    an intersection, there's no way to guarantee that
    //    our t value is still on the spline (we could already 
    //    be in the OUT lane). We need to create a new spline
    //    that still takes us from one road to another, but
    //    we didn't store all this info!! Arrgh...
    // 
    // A BETTER design (from the ground up), would have 
    // just stored splines from the beginning (even if you're
    // traversing roads), making sure that this spline is 
    // long enough to contain our vehicle's actual position.
    // As our t value crawls along a roadsegment, we add it to 
    // our spline. This way when we need to recompute t, we 
    // can just search on our spline for the closest segment
    // to our current position. The other plus side to this
    // is that we don't traverse segments differently from 
    // intersections. Everything gets stored in our spline.
    //       

}



//=============================================================================
// TrafficLocomotion::Update
//=============================================================================
// Description: Comment
//
// Parameters:  (float seconds)
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::Update(float seconds)
{
    if( !mMyAI->mIsActive )
    {
        return;
    }

    mSecondsSinceLastAddToHistory += seconds;
    
BEGIN_PROFILE( "Traffic Locomotion" );

    //mVehicle->mPoseEngine->Begin( false );

    rmt::Vector pos, facing;

    TrafficAI::State state = mMyAI->GetState();

    switch ( state )
    {
    case TrafficAI::DEAD:
        {
            pos = mPrevPos;
        #ifdef FACING_HISTORY
            mFacingHistory.GetNormalizedAverage( facing );
        #endif
        }
        break;
    case TrafficAI::DRIVING: // fall through
    case TrafficAI::WAITING_AT_INTERSECTION:
        {
            if( mMyAI->mNeedToSuddenlyStop )
            {
                StopSuddenly( pos, facing );
                break;
            }

            bool stayDoggyStay = false;

            //================================================
            // DETERMINE t AND CURRENT WAYPOINT OR ROAD SEGMENT 
            // (whichever's applicable)
            //================================================
            float t = 0.0f;
            float pathLength = 0.0f;

            if( !mIsInIntersection )
            {
                pathLength = mMyAI->GetLaneLength();
                t = mMyAI->GetLanePosition();
            }
            else
            {
                pathLength = mCurrPathLength;
                t = mCurrPathLocation;
            }

            float adjustedDt = (GetAISpeed() * seconds) / pathLength;
            t += adjustedDt;

            while( t > 1.0f )
            {
                t -= 1.0f;

                if( !mIsInIntersection ) // we're not inside an intersection
                {
                    const Road* road = mMyAI->GetLane()->GetRoad();
                    unsigned int segmentIndex = mMyAI->GetSegmentIndex();

                    // If there are more segments on this road
                    if ( segmentIndex < (road->GetNumRoadSegments()-1) )
                    {
                        // we have to move ahead a segment
                        segmentIndex++;
                        mMyAI->SetSegmentIndex( segmentIndex ); 
                        float newLength = mMyAI->GetLaneLength();
                        t *= pathLength / newLength;
                        mMyAI->SetLanePosition(t); 
                        pathLength = newLength;

                        // ========================
                        // We just updated mMyAI:
                        // - segment & segment index updated to look at next segment
                        // - lane & lane index not updated
                        // - lane position updated with the new segment's t
                        // ========================
                    }
                    // Else we must be at an intersection
                    else 
                    {
                        // set up cubic spline
                        bool succeeded = EnterIntersection();
                        // ========================
                        // We just updated mMyAI:
                        // - segment & segment index updated to look at the OUT segment
                        // - lane & lane index updated to look at the OUT lane
                        // - lane position not updated
                        // ========================

                        // Set info of first path on spline
                        if( succeeded )
                        {
                            rAssert( mWays != NULL );

                            mMyAI->SetIsInIntersection( true );
                            mIsInIntersection = true;

                            mCurrWay = 0;
                            mCurrPath.Sub( mWays[mCurrWay+1], mWays[mCurrWay] );
                            mCurrPathLength = mCurrPath.Length();  // *** SQUARE ROOT! ***
                            t *= pathLength / mCurrPathLength;
                            pathLength = mCurrPathLength;
                        }
                        else
                        {
                            stayDoggyStay = true;
                            break;
                        }

                    }
                }
                else // we ARE inside an intersection
                {
                    // Since we never spawn in an intersection, we never
                    // get into this case unless we entered via EnterIntersection(), 
                    // and t is > 1.0f... 
                    // Meaning that at this point, mWays has been populated and
                    // mCurrWay, mCurrPath, and mCurrPathLength are set.

                    rAssert( mCurrWay < mNumWays );
                    rAssert( mCurrWay >= 0 );

                    // if we still got waypoints to traverse in intersection
                    if( mCurrWay < (mNumWays - 2) )
                    {
                        mCurrWay++;

                        rAssert( mWays != NULL );

                        mCurrPath.Sub( mWays[mCurrWay+1], mWays[mCurrWay] );
                        float newLength = mCurrPath.Length();  // *** SQUARE ROOT! ***
                        t *= pathLength / newLength;
                        pathLength = newLength;
                        mCurrPathLength = newLength;
                    }
                    else // Else going out of the intersection now
                    {
                        mIsInIntersection = false;
                        mMyAI->SetIsInIntersection( false ); 

                        // mMyAI information has already been set to look 
                        // at the out lane/segment/etc.
                        // Must move within the lane...
                        float newLength = mMyAI->GetLaneLength();
                        t *= pathLength / newLength;
                        pathLength = newLength;
                    }
                }
            }

            // if t is not a valid number, it means that we encountered
            // an error inside EnterIntersection()
            if( stayDoggyStay )
            {
                pos = mPrevPos;
            #ifdef FACING_HISTORY
                mFacingHistory.GetNormalizedAverage( facing );
            #endif                            
                break; // exit this case
            }


            //================================================
            // KNOWING t, DETERMINE pos AND facing
            //================================================
            rAssert( 0 <= t && t <= 1.0f );

            // Depending on whether we're inside an intersection,
            // we have different methods for determining pos & facing
            if( !mIsInIntersection ) 
            {
                mMyAI->SetLanePosition( t );
                RoadSegment* segment = mMyAI->GetSegment();
                rAssert( segment );

                segment->GetLaneLocation( mMyAI->GetLanePosition(), mMyAI->GetLaneIndex(), pos, facing );
                if( !rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) )
                {
                    facing.NormalizeSafe(); // *** SQUARE ROOT! ***
                }
                rAssert( rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) );
            }
            else
            {
                mCurrPathLocation = t;
                rmt::Vector temp = facing = mCurrPath;
                facing.Scale( 1.0f / mCurrPathLength );
                rAssert( rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) );
                temp.Scale(t);

                rAssert( mWays != NULL );
                pos.Add( mWays[mCurrWay], temp );
            }
        }
        break;

    case TrafficAI::WAITING_FOR_FREE_LANE:
        {
            mSecondsTillCheckForFreeLane -= seconds;
            if( mSecondsTillCheckForFreeLane < seconds )
            {
                mSecondsTillCheckForFreeLane = SECONDS_BETW_CHECKS_FOR_FREE_LANE;

                // Transit back to driving next frame, so we check again for 
                // lane availability
                mMyAI->SetState( TrafficAI::DRIVING );
            }

            // keep old heading & facing for now (complete and instantaneous stop)
            pos = mPrevPos;
        #ifdef FACING_HISTORY
            mFacingHistory.GetNormalizedAverage( facing );
        #endif
        }
        break;

    case TrafficAI::SPLINING: // fall thru
    case TrafficAI::LANE_CHANGING:
        {
            rAssert( !mIsInIntersection );

            // In TrafficAI, if we're SPLINING, we can be told to stop for something.
            // (when LANE_CHANGING we don't stop for anything... why? Not sure...)
            // What happens if we need to suddenly stop here?
            if( mMyAI->mNeedToSuddenlyStop )
            {
                StopSuddenly( pos, facing );
                break;
            }


            //================================================
            // DETERMINE t AND CURRENT WAYPOINT OR ROAD SEGMENT 
            // (whichever's applicable)
            //================================================
            float pathLength = mCurrPathLength;
            float t = mCurrPathLocation;

            float adjustedDt = (GetAISpeed() * seconds) / pathLength;
            t += adjustedDt;

            while( t > 1.0f )
            {
                t -= 1.0f;

                // In case we got fed a long "seconds" value and go WAY over 
                // the lane change spline. Then we're not lane changing 
                // anymore (we're on normal road now)
                if( mMyAI->GetState() != state ) 
                {
                    const Road* road = mMyAI->GetLane()->GetRoad();
                    unsigned int segmentIndex = mMyAI->GetSegmentIndex();

                    if( segmentIndex < (road->GetNumRoadSegments()-1) )
                    {
                        // we have to move ahead a segment
                        segmentIndex++;
                        mMyAI->SetSegmentIndex( segmentIndex ); 
                        float newLength = mMyAI->GetLaneLength();
                        t *= pathLength / newLength;
                        mMyAI->SetLanePosition(t); 
                        pathLength = newLength;

                        // ========================
                        // We just updated mMyAI:
                        // - segment & segment index updated to look at next segment
                        // - lane & lane index not updated
                        // - lane position updated with the new segment's t
                        // ========================
                    }
                    else // if we're out of segments.. we are at an intersection... 
                    {
                        // TODO:
                        // we should include intersection code here and deal with
                        // the t-overflow properly... but I'm thinking that's a lot 
                        // of code repeat... so I'll get around to it later... 
                        t = 0.999f;
                        mMyAI->SetLanePosition(t);
                    }
                }
                else // else we are in middle of lane changing...
                {
                    // At this point, mWays has been populated and
                    // mCurrWay, mCurrPath, and mCurrPathLength are set.

                    rAssert( mCurrWay < mNumWays );
                    rAssert( mCurrWay >= 0 );

                    // if we still got waypoints to traverse...
                    if( mCurrWay < (mNumWays - 2) )
                    {
                        mCurrWay++;

                        rAssert( mWays != NULL );

                        mCurrPath.Sub( mWays[mCurrWay+1], mWays[mCurrWay] );
                        float newLength = mCurrPath.Length();  // *** SQUARE ROOT! ***
                        t *= pathLength / newLength;
                        pathLength = newLength;
                        mCurrPathLength = newLength;
                    }
                    else // Else done lane changing now...
                    {
                        mMyAI->SetState( TrafficAI::DRIVING );

                        // mMyAI information has already been set to look 
                        // at the target lane/segment/etc.
                        // Must move within the lane...
                        float newLength = mMyAI->GetLaneLength();
                        t *= pathLength / newLength;
                        t += mOutLaneT;
                        pathLength = newLength;
                    }
                }
            }

            //================================================
            // KNOWING t, DETERMINE pos AND facing
            //================================================
            rAssert( 0 <= t && t <= 1.0f );

            // Depending on whether we're still lane changing...
            // we have different methods for determining pos & facing
            if( mMyAI->GetState() != state ) 
            {
                mMyAI->SetLanePosition( t );
                RoadSegment* segment = mMyAI->GetSegment();
                rAssert( segment );

                segment->GetLaneLocation( mMyAI->GetLanePosition(), mMyAI->GetLaneIndex(), pos, facing );
                if( !rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) )
                {
                    facing.NormalizeSafe(); // *** SQUARE ROOT! ***
                }
                rAssert( rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) );
            }
            else
            {
                mCurrPathLocation = t;
                rmt::Vector temp = facing = mCurrPath;
                facing.Scale( 1.0f / mCurrPathLength );
                rAssert( rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) );
                temp.Scale(t);

                rAssert( mWays != NULL );
                pos.Add( mWays[mCurrWay], temp );
            }
        }
        break;

    case TrafficAI::SWERVING:
        {
            // we should be updating TrafficLocomotion if we're swerving because
            // we should be in PhysicsLocomotion
            rAssert( false );
        }
    default:
        {
            rAssert( false );
        }
        break;
    }



    //================================================
    // KNOWING pos AND facing, UPDATE VEHICLE
    //================================================

    // only need mPrevPos for the transition into Intersection
    // since we can't rely on Vehicle->GetPosition to return us
    // the RIGHT-ON-THE-GROUND values (Vehicle class does its
    // own adjustments to bring the car up from ground level)
    rAssert( rmt::Epsilon( facing.MagnitudeSqr(), 1.0f, 0.005f ) );

#if defined(FACING_HISTORY) || defined(POS_HISTORY)

    // we need to add this many entries
    int maxCount = (int)(mSecondsSinceLastAddToHistory / SECONDS_BETW_HISTORY_UPDATES);

    if( maxCount > 0 )
    {
        float secondsConsumed = (float)(maxCount)*SECONDS_BETW_HISTORY_UPDATES;
        float tmpT = (secondsConsumed/mSecondsSinceLastAddToHistory);

        // figure out the starting and ending elements to interpolate between
    #ifdef POS_HISTORY
        rmt::Vector posStart = mPosHistory.GetLastEntry();
        rmt::Vector posDir = pos - posStart;
        rmt::Vector posEnd = posStart + posDir * tmpT;
        posDir = posEnd - posStart;
    #endif
    #ifdef FACING_HISTORY
        rmt::Vector faceStart = mFacingHistory.GetLastEntry();
        rmt::Vector faceDir = facing - faceStart;
        rmt::Vector faceEnd = faceStart + faceDir * tmpT;
        faceDir = faceEnd - faceStart;
    #endif


        float tIncrement = 1.0f / (float)(maxCount);
        tmpT = 0.0f;
        for( int count = 1; count <= maxCount; count++ )
        {
            tmpT += tIncrement;
        #ifdef POS_HISTORY
            mPosHistory.UpdateHistory( posStart + posDir * tmpT );
        #endif
        #ifdef FACING_HISTORY
            mFacingHistory.UpdateHistory( faceStart + faceDir * tmpT );
        #endif

        }
        mSecondsSinceLastAddToHistory -= secondsConsumed;

    }

#endif

    // Histories are updated, now we get the average out of them.

#ifdef FACING_HISTORY
    mFacingHistory.GetNormalizedAverage( facing );
#endif
#ifdef POS_HISTORY
    mPosHistory.GetAverage( pos );
#endif

    /////////////////////////////////////////////
    // Adjust ground height (unfortunately we need to do this
    // because we use pos averaging (so our y value is off)
    rmt::Vector groundPosition, outnorm;
    bool bFoundPlane = false;

    groundPosition = pos;
    outnorm.Set( 0.0f, 1.0f, 0.0f );

    GetIntersectManager()->FindIntersection(
        groundPosition,   // IN
        bFoundPlane,      // OUT
        outnorm,          // OUT
        groundPosition    // OUT
        );  

    if( bFoundPlane )
    {
        pos.y = groundPosition.y;
    }
    ///////////////////////////////////////////////

    // compute actual speed
    rmt::Vector oldPos;
    oldPos = mPrevPos;
    mActualSpeed = (pos - oldPos).Magnitude() / seconds;

    // Update vehicle transform...
    rmt::Matrix newTransform;
    newTransform.Identity();

    rmt::Vector target;
    target = pos;
    target.Add( facing );
    rmt::Vector up = UpdateVUP( pos, target );
    newTransform.FillTranslate(pos);
    newTransform.FillHeading(facing, up);
    mVehicle->TrafficSetTransform(newTransform);

    // Pivot the front wheels...
    PivotFrontWheels( facing );

    mPrevPos = pos;

END_PROFILE( "Traffic Locomotion" );
}

//=============================================================================
// TrafficLocomotion::PostUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficLocomotion::PostUpdate()
{
    if( !mMyAI->mIsActive )
    {
        return;
    }
    // make sure values are set for wheel rendering info...

    // set 'artificial suspension point velocities
    int i;
    for(i = 0; i < 4; i++)
    {
        rmt::Vector velocity = mVehicle->mVehicleFacing;
        velocity.Scale( mActualSpeed );
        mVehicle->mSuspensionPointVelocities[i] = velocity;
    }
    mVehicle->mVelocityCM = mVehicle->mVehicleFacing;
    mVehicle->mVelocityCM.Scale( mActualSpeed );

    // update mSimStateArticulated speed here?
    // hmmm...
    rmt::Vector& linearVelocity = mVehicle->mSimStateArticulated->GetLinearVelocity();
    linearVelocity = mVehicle->mVelocityCM;

    mVehicle->mSpeed = mActualSpeed;

    
}
 
//////////////////////////////////////////////////////////////////////
//////////////////////////// PRIVATES ////////////////////////////////

void TrafficLocomotion::FindOutLane (
    const Lane* inLane, 
    unsigned int inLaneIndex,
    Lane*& outLane, 
    unsigned int& outLaneIndex )
{
    rAssert( inLane != NULL );
    
    const Road* inRoad = inLane->GetRoad();
    rAssert( inRoad != NULL );

    Intersection* intersection = 
        (Intersection*) inRoad->GetDestinationIntersection();
    rAssert( intersection );

    Road* outRoad = NULL;
    outLane = NULL;
    outLaneIndex = 0;

    switch( mMyAI->DecideTurn() )
    {
    case TrafficAI::LEFT:
        {
            intersection->GetLeftTurnForTraffic( 
                *inRoad, inLaneIndex, outRoad, outLane, outLaneIndex );
        }
        break;
    case TrafficAI::RIGHT:
        {
            intersection->GetRightTurnForTraffic( 
                *inRoad, inLaneIndex, outRoad, outLane, outLaneIndex );
        }
        break;
    case TrafficAI::STRAIGHT:
        {
            intersection->GetStraightForTraffic( 
                *inRoad, inLaneIndex, outRoad, outLane, outLaneIndex );
        }
        break;
    default:
        {
            rAssert( false );
        }
    }
}


void TrafficLocomotion::BuildCurve ( 
    RoadSegment* inSegment, 
    unsigned int inLaneIndex, 
    RoadSegment* outSegment, 
    unsigned int outLaneIndex )
{
    rAssert( inSegment != NULL );
    rAssert( outSegment != NULL );

    // get the start pos & dir
    rmt::Vector startPos, startDir;
    inSegment->GetLaneLocation( 1.0f, inLaneIndex, startPos, startDir );
    //startDir.y = 0.0f;
    if( !rmt::Epsilon( startDir.MagnitudeSqr(), 1.0f, 0.001f ) )
    {
        startDir.Normalize(); // *** SQUARE ROOT! ***
    }

    // get end pos & dir
    rmt::Vector endPos, endDir;
    outSegment->GetLaneLocation( 0.0f, outLaneIndex, endPos, endDir );
    //endDir.y = 0.0f;
    if( !rmt::Epsilon( endDir.MagnitudeSqr(), 1.0f, 0.001f ) )
    {
        endDir.Normalize(); // *** SQUARE ROOT! ***
    }

    // get intermediate points
    rmt::Vector startEndTemp, p2, p4;
    startEndTemp.Sub( endPos, startPos );
    float distFromStartOrEnd = startEndTemp.Length() / 3.0f; // *** SQUARE ROOT! ***
    p2 = startPos + startDir * distFromStartOrEnd;
    p4 = endPos - endDir * distFromStartOrEnd;

    // Now we are ready to gather together our points and send it
    // to our "curve" finder.
    rmt::Vector pts [4];
    pts[0] = startPos;
    pts[1] = p2;
    pts[2] = p4;
    pts[3] = endPos;

    mSplineMaker.SetControlPoint( pts[0], 0 );
    mSplineMaker.SetControlPoint( pts[1], 1 );
    mSplineMaker.SetControlPoint( pts[2], 2 );
    mSplineMaker.SetControlPoint( pts[3], 3 );

/*
#if defined(RAD_TUNE) || defined(RAD_DEBUG)

    const Road* inRoad = inSegment->GetRoad();
    rAssert( inRoad );

    Intersection* intersection = 
        (Intersection*) inRoad->GetDestinationIntersection();
    rAssert( intersection );

    // NOTE:
    // If you hate this assert, you can comment it out locally... FOR NOW... 
    // Put it back in once all y-value mismatch errors are gone. 
    // In other words, once hell freezes over.
    rmt::Vector intersectionLoc;
    intersection->GetLocation( intersectionLoc );
    char baseMsg[1000];
    sprintf( baseMsg, 
        "\nMismatching y-values at intersection (%f,%f,%f).\n"
        "    Check if y values are same for all IN & OUT road segments attached\n"
        "    to this intersection. Check hypergraph to see if the roadnodes leading\n"
        "    IN and OUT of this intersection contain all the proper roadsegments.\n"
        "    If you skip this, Traffic cars will \"hop\" when they transit through\n"
        "    the intersection between the road segments with mismatching y values.\n"
        "    Better to report error to me (Dusit) or Sheik. Preferrably Sheik.\n\n", 
        intersectionLoc.x, 
        intersectionLoc.y,
        -1*intersectionLoc.z );
    rAssert( strlen(baseMsg) < 1000 );

    float ep = 0.001f;
    if( !( rmt::Epsilon( pts[0].y, pts[1].y, ep ) &&
            rmt::Epsilon( pts[0].y, pts[2].y, ep ) &&
            rmt::Epsilon( pts[0].y, pts[3].y, ep ) ) )
    {
        rTunePrintf( baseMsg );
    }
#endif
*/

    mSplineMaker.GetCubicBezierCurve( mWays, mNumWays );
    rAssert( mWays != NULL );
    rAssert( mNumWays == CubicBezier::MAX_CURVE_POINTS );
}



bool TrafficLocomotion::EnterIntersection()
{
    const Lane* inLane = mMyAI->GetLane();
    rAssert( inLane != NULL );
    RoadSegment* inSegment = mMyAI->GetSegment();
    rAssert( inSegment != NULL );
    const unsigned int inLaneIndex = mMyAI->GetLaneIndex();
   
    // ==============================================
    // Find OUT LANE and OUT LANE INDEX 
    // ==============================================

    Lane* outLane = NULL;
    unsigned int outLaneIndex = 0;

    FindOutLane( inLane, inLaneIndex, outLane, outLaneIndex );

    // if absolutely can't find a road, transit to waiting state
    if( outLane == NULL ) 
    {
        // TODO: Should we slow down?
        // Right now we have complete and instantaneous stop,
        // which is needed because we are transiting to a diff state
        // and must maintain old position & facing for when we transit
        // back... 
        mMyAI->SetState( TrafficAI::WAITING_FOR_FREE_LANE );

        return false;
    }

    rAssert( outLane != NULL );


    // ================================================
    // Update IN & OUT lanes' list of traffic vehicles
    // ================================================
    UpdateLanes( mVehicle->mTrafficVehicle, (Lane*)inLane, outLane );


    // ==============================================
    // Build Cubic Spline to navigate intersection
    // ==============================================
    const Road* outRoad = outLane->GetRoad();
    rAssert( outRoad != NULL );
    unsigned int outSegmentIndex = 0;
    RoadSegment* outSegment = outRoad->GetRoadSegment(outSegmentIndex);
    rAssert( outSegment != NULL );

    // create control points, then the spline... store all in mWays
    BuildCurve( inSegment, inLaneIndex, outSegment, outLaneIndex );


    // ==================================================
    // Update mMyAI:
    //  - to look at the OUT segment & segment index
    //  - to look at the OUT lane & lane index
    //  - lane position not updated
    // ===================================================
    mMyAI->SetLane( outLane );
    mMyAI->SetLaneIndex( outLaneIndex );
    mMyAI->SetSegment( outSegment );
    mMyAI->SetSegmentIndex( outSegmentIndex );

    return true;
}

void TrafficLocomotion::PivotFrontWheels( rmt::Vector facing )
{
    float cosAlpha = 0.0f;
    rmt::Vector outPos, outFacing;
    if( mIsInIntersection )
    {
        // when we're in the intersection, the lane is the OUTLANE, so get the t=0.0f
        mMyAI->GetSegment()->GetLaneLocation( 0.0f, (int)mMyAI->GetLaneIndex(), outPos, outFacing );
        outFacing.y = 0.0f;
        outFacing.Normalize(); // *** SQUARE ROOT! ***
        facing.y = 0.0f;
        facing.Normalize(); // *** SQUARE ROOT! ***
        cosAlpha = facing.Dot( outFacing );
    }
    else
    {
        // fake wheel turning for lane change...
        if( mMyAI->GetState() == TrafficAI::LANE_CHANGING )
        {
            float progress = mLaneChangeProgress / mLaneChangeDist;
            if( progress < 0.65f )
            {
                cosAlpha = 0.9396926f; // cos20
            }
            else
            {
                cosAlpha = -0.9396926f; // cos20
            }
        }
        else
        {
            // when we're not in the intersection, the lane is the 
            // current lane, so get t=1.0f
            mMyAI->GetSegment()->GetLaneLocation( 1.0f, 
                (int)mMyAI->GetLaneIndex(), outPos, outFacing );
            outFacing.y = 0.0f;
            outFacing.Normalize(); // *** SQUARE ROOT! ***
            facing.y = 0.0f;
            facing.Normalize(); // *** SQUARE ROOT! ***
            cosAlpha = facing.Dot( outFacing );

        }
    }

    // ensure we are in bounds
    if( cosAlpha < -1.0f )
    {
        cosAlpha = -1.0f;
    }
    else if( cosAlpha > 1.0f )
    {
        cosAlpha = 1.0f;
    }
    float alpha = rmt::ACos( cosAlpha ); // *** COSINE! ***
    rAssert( !rmt::IsNan( alpha ) );

    // pivot left or right 
    if( mMyAI->GetState() != TrafficAI::LANE_CHANGING )
    {
        rmt::Vector rightOfFacing = Get90DegreeRightTurn( facing );
        if( rightOfFacing.Dot( outFacing ) < 0.0f )
        {
            alpha *= -1.0f;
        }
    }
    // Who am I? I'm Spiderman! No, I'm Dusit.
    mVehicle->SetWheelTurnAngleDirectlyInRadiansForDusitOnly( alpha );

}



void TrafficLocomotion::UpdateLanes( TrafficVehicle* tv, Lane* oldLane, Lane* newLane )
{
    rAssert( tv != NULL );
    rAssert( tv->GetIsActive() );
    rAssert( oldLane != NULL );
    rAssert( newLane != NULL );

    // Remove vehicle from IN lane
    bool found = false;
    for( int i=0; i<oldLane->mTrafficVehicles.mUseSize; i++ )
    {
        if( tv == oldLane->mTrafficVehicles[i] )
        {
            oldLane->mTrafficVehicles.Remove( i );
            found = true;
            break;
        }
    }
    //rAssert( found );

    // Add vehicle to OUT lane.
    // What happens if AddLast returns -1 because you can't add
    // another vehicle to that lane? It WON'T! Because in our
    // Intersection::Get<blah>ForTraffic, we make sure we either 
    // return a lane that can take this car or NULL. If NULL, then
    // we shouldn't be at this point in the code.
    //
    rAssert( newLane->mTrafficVehicles.mUseSize < newLane->mTrafficVehicles.mSize );
    newLane->mTrafficVehicles.Add( tv );
    tv->SetLane( newLane );
}

Lane* TrafficLocomotion::GetAIPrevLane()
{
    return mMyAI->mPrevLane;
}

// will return if there's not enough room to lanechange
bool TrafficLocomotion::BuildLaneChangeCurve( 
    RoadSegment* oldSegment, 
    const float oldT,
    unsigned int oldLaneIndex,
    unsigned int newLaneIndex,
    const float dist)
{
    rAssert( oldSegment != NULL );

    // We have to create control points
    rmt::Vector pts[4];

    // First figure out entry point
    rmt::Vector entryFacing;
    oldSegment->GetLaneLocation( oldT, oldLaneIndex, pts[0], entryFacing );
    if( !rmt::Epsilon( entryFacing.MagnitudeSqr(), 1.0f, 0.0001f ) )
    {
        entryFacing.NormalizeSafe(); // *** SQUARE ROOT! ***
    }
    rAssert( rmt::Epsilon( entryFacing.MagnitudeSqr(), 1.0f, 0.0001f ) );

    // Second, figure out the next point in line with facing, 
    // some dist ahead (percentage of lanechange dist)
    float entryOffset = 0.2f * dist;
    pts[1] = pts[0] + entryFacing * entryOffset;

    // Third, figure out the exit point.
    // we'll have to follow the road for the given lanechange distance
    RoadSegment* endSegment = oldSegment;

    float oldLaneLength = oldSegment->GetLaneLength( oldLaneIndex );
    float t = oldT;
    float adjustedDt = dist / oldLaneLength;
    t += adjustedDt;

    unsigned int endSegIndex = endSegment->GetSegmentIndex();
    while( t > 1.0f )
    {
        t -= 1.0f;
        // move on to next segment (there must be one)
        unsigned int numSegs = endSegment->GetRoad()->GetNumRoadSegments();
        endSegIndex++;

        // if we're out of bounds, it means that there weren't enough segments
        // to complete lane-change. Abort!
        if( endSegIndex < 0 || endSegIndex >= numSegs )
        {
            return false;
        }

        endSegment = endSegment->GetRoad()->GetRoadSegment( endSegIndex );
        float nextSegLen = endSegment->GetLaneLength( oldLaneIndex );

        t *= oldLaneLength / nextSegLen;

        oldLaneLength = nextSegLen;
    }

    rAssert( t <= 1.0f );

    // since the last point of spline won't be the START of the 
    // segment when we come out of LANE_CHANGING state, we need to
    // store away the t so we add it later...
    mOutLaneT = t; 

    rmt::Vector exitFacing;
    endSegment->GetLaneLocation( t, newLaneIndex, pts[3], exitFacing );
    if( !rmt::Epsilon( exitFacing.MagnitudeSqr(), 1.0f, 0.0001f ) )
    {
        exitFacing.NormalizeSafe(); // *** SQUARE ROOT! ***
    }
    rAssert( rmt::Epsilon( exitFacing.MagnitudeSqr(), 1.0f, 0.0001f ) );
    exitFacing.Scale( -1.0f );

    // Lastly, figure out the previous point, in line with facing, 
    // some dist ahead (percentage of lanechange dist)
    float exitOffset = dist * 0.5f;//0.3f;
    pts[2] = pts[3] + exitFacing * exitOffset;

    // 
    // update AI's segment info: segment, segment index, lane length
    //
    mMyAI->SetSegmentIndex( endSegment->GetSegmentIndex() );

    mSplineMaker.SetControlPoint( pts[0], 0 );
    mSplineMaker.SetControlPoint( pts[1], 1 );
    mSplineMaker.SetControlPoint( pts[2], 2 );
    mSplineMaker.SetControlPoint( pts[3], 3 );

    // use mWays to store the spline 
    // (since we're not in an intersection anyway)
    mSplineMaker.GetCubicBezierCurve( mWays, mNumWays );
    rAssert( mWays != NULL );
    rAssert( mNumWays == CubicBezier::MAX_CURVE_POINTS );

    // update the currway stuff needed to work with mWays
    mCurrWay = 0;
    mCurrPath.Sub( mWays[mCurrWay+1], mWays[mCurrWay] );
    mCurrPathLength = mCurrPath.Length();  // *** SQUARE ROOT! ***
    mCurrPathLocation = 0.0f;

    return true;
}

bool TrafficLocomotion::BuildArbitraryCurve(
    const rmt::Vector& startPos,
    const rmt::Vector& startDir, // is normalized to 1
    const rmt::Vector& endPos,
    const rmt::Vector& endDir )// is normalized to 1 
{
    rAssert( rmt::Epsilon( startDir.MagnitudeSqr(), 1.0f, 0.001f ) );
    rAssert( rmt::Epsilon( endDir.MagnitudeSqr(), 1.0f, 0.001f ) );

    float distScale = (startPos - endPos).Length(); // *** SQUARE ROOT! ***
    distScale *= 0.3f;

    rmt::Vector pts[4];
    pts[0] = startPos;
    pts[1] = startPos + startDir * distScale;
    pts[2] = endPos + endDir * -1 * distScale;
    pts[3] = endPos;

    mSplineMaker.SetControlPoint( pts[0], 0 );
    mSplineMaker.SetControlPoint( pts[1], 1 );
    mSplineMaker.SetControlPoint( pts[2], 2 );
    mSplineMaker.SetControlPoint( pts[3], 3 );

    // use mWays to store the spline 
    mSplineMaker.GetCubicBezierCurve( mWays, mNumWays );
    rAssert( mWays != NULL );
    rAssert( mNumWays == CubicBezier::MAX_CURVE_POINTS );

    // update the currway stuff needed to work with mWays
    mCurrWay = 0;
    mCurrPath.Sub( mWays[mCurrWay+1], mWays[mCurrWay] );
    mCurrPathLength = mCurrPath.Length();  // *** SQUARE ROOT! ***
    mCurrPathLocation = 0.0f;

    return true;
}

void TrafficLocomotion::GetSplineCurve( rmt::Vector*& ways, int& npts, int& currWay )
{
    if( mWays == NULL )
    {
        ways = NULL;
        npts = 0;
        currWay = -1;
    }
    else
    {
        ways = mWays;
        npts = mNumWays;
        currWay = mCurrWay;
    }
}
