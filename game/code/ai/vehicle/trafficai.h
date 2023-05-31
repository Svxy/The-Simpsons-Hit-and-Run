//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficai.h
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Accel/Decel behavior -- Dusit Eakkachaichanvet
//              06/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRAFFICAI_H
#define TRAFFICAI_H

//========================================
// Nested Includes
//========================================
#include <roads/intersection.h>
#include <roads/lane.h>
#include <roads/road.h>
#include <worldsim/redbrick/vehiclecontroller/aivehiclecontroller.h>

//========================================
// Forward References
//========================================
class Vehicle;

class TrafficAI : public AiVehicleController
{
public: // METHODS

    static const float SECONDS_LOOKAHEAD;
    static const float LOOKAHEAD_MIN;

    //What is the traffic AI up to?
    enum State
    {
        DEAD,
        DRIVING,
        WAITING_AT_INTERSECTION,
        WAITING_FOR_FREE_LANE,
        LANE_CHANGING,
        SPLINING,
        SWERVING,

        NUM_STATES
    };

    //Which way does he want to turn?
    enum Direction
    {
        LEFT,
        RIGHT,
        STRAIGHT,

        NUM_DIRECTIONS
    };

    //static const float LANE_CHANGE_DIST;

    TrafficAI( Vehicle* vehicle );
    virtual ~TrafficAI();

    void Init();

    void Init( Vehicle* vehicle, 
               Lane* lane,
               unsigned int laneIndex,
               RoadSegment* segment,
               unsigned int segmentIndex,
               float t, 
               float mps );

    void        Update( float seconds );
    
    State       GetState() const;
    void        SetState(State state);

    void SetLane( Lane* lane );
    Lane* GetLane();

    void SetLaneIndex( unsigned int index );
    unsigned int GetLaneIndex() const;

    float GetLaneLength() const;

    void SetSegment( RoadSegment* segment );
    RoadSegment* GetSegment() const;

    void SetSegmentIndex( unsigned int index );
    unsigned int GetSegmentIndex() const;

    void SetLanePosition( float t );
    float GetLanePosition();

    void SetAISpeed( float mps );
    float GetAISpeed() const;

    Direction DecideTurn();

    void RegisterDebugInfo();
    void UnregisterDebugInfo();
    void RegisterAI();
    void UnregisterAI();

    void StartSwerving( bool swerveRight );

    enum ObstacleType 
    {
        OT_NOTHING              = 0,
        OT_NONPLAYERVEHICLE     = 1,
        OT_NONPLAYERCHARACTER   = 2,
        OT_PLAYERCHARACTER      = 3,
        OT_PLAYERVEHICLE        = 4,
        OT_ENDOFROAD            = 99
    };
    void CheckForObstacles( ObstacleType& objID, float& distFromObjSqr, void*& obj, bool& objOnMyRight );


public: // MEMBERS 

    // Breaking architecture a bit. TrafficLocomotion is the one that 
    // needs to detect when we're inside/outside an intersection. So we allow it
    // to set a TrafficAI flag in this instance. But how to ensure that only
    // TrafficLocomotion is allowed to call this function? Hence a small 
    // breakage.
    void SetIsInIntersection( bool value );
    Lane* mPrevLane; // for when we're in an intersection, it's the IN lane
    bool mIsActive           : 1;
    bool mNeedToSuddenlyStop : 1;
    rmt::Vector mLookAheadPt;
    

private: // MEMBERS

    State       mState;
    State       mPrevState;
    Direction   mDirection;

    Lane*           mLane; // curr lane when not in intersection, OUT lane when in intersection
    unsigned int    mLaneIndex;
    float           mLaneLength;

    RoadSegment*    mSegment;
    unsigned int    mSegmentIndex;

    float           mT;         //t
    float           mAISpeed;     //mps

    float mSecondsDriving;
    const Intersection* mPrevIntersection;
    float mStopForSomethingDecel;

    int mRenderHandle;

    float mSecondsSinceLastTriggeredImpedence;
    void* mLastThingThatImpededMe;

    float mSecondsSinceLaneChange;

    float mSecondsSwerving; 
    float mOriginalMaxWheelTurnAngle;
    bool mIsInIntersection : 1;
    bool mSwervingLeft     : 1;
    bool mSwerveHighBeamOn : 1;
    float mOriginalHeadlightScale;
    float mSecondsSwerveHighBeam;

    // used for waiting at intersection state
    rmt::Vector mEndOfRoadPos;

private: // METHODS

    // returns triangular vertices of frustrum
    void GetFrustrum( const rmt::Vector& pos, 
                      const rmt::Vector& dir, 
                      rmt::Vector& leftFrustrumVertex, 
                      rmt::Vector& rightFrustrumVertex );

    float GetGoSpeedMps();


    bool AttemptLaneChange( ObstacleType foundSOMETHING, float distFromSOMETHINGSqr, void* SOMETHING );
    void MaintainSpeed( float seconds );
    void StopForSomething( float seconds, ObstacleType ID, float distSqr, void* obj );

    void PerhapsTriggerImpedence( ObstacleType foundSOMETHING, float distSqr, void* SOMETHING );

    void StopSwerving();
    void Swerve();

    float GetLookAheadDistance();

    //Prevent wasteful constructor creation.
    TrafficAI( const TrafficAI& trafficai );
    TrafficAI& operator=( const TrafficAI& trafficai );
};


//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

inline void TrafficAI::SetIsInIntersection( bool value )
{
    mIsInIntersection = value;
}
inline TrafficAI::State TrafficAI::GetState() const
{
    return mState;
}

inline void TrafficAI::SetLane( Lane* lane )
{
    mPrevLane = mLane;
    mLane = lane;
}
inline Lane* TrafficAI::GetLane()
{
    return mLane;
}
inline float TrafficAI::GetLaneLength() const
{
    return mLaneLength;
}
inline void TrafficAI::SetLaneIndex( unsigned int index )
{
    mLaneIndex = index;
}
inline unsigned int TrafficAI::GetLaneIndex() const
{
    return mLaneIndex;
}
inline void TrafficAI::SetSegment( RoadSegment* segment )
{
    mSegment = segment;
}
inline RoadSegment* TrafficAI::GetSegment() const
{
    return mSegment;
}
inline unsigned int TrafficAI::GetSegmentIndex() const
{
    return mSegmentIndex;
}
inline void TrafficAI::SetLanePosition( float t )
{
    mT = t;
}
inline float TrafficAI::GetLanePosition()
{
    return mT;
}
inline void TrafficAI::SetState(State state)
{
    mPrevState = mState;
    mState = state;
    //rDebugPrintf( "TrafficAI: Statechange: %d -> %d\n", mPrevState, mState );
}

#endif //TRAFFICAI_H
