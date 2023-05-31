//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficlocomotion.h
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Locomote through intersections -- Dusit Eakkachaichanvet
//              07/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRAFFICLOCOMOTION_H
#define TRAFFICLOCOMOTION_H

//========================================
// Nested Includes
//========================================
#include <worldsim/redbrick/vehiclelocomotion.h>
#include <radmath/radmath.hpp>
#include <ai/vehicle/trafficai.h>
#include <roads/geometry.h>

//========================================
// Forward References
//========================================
class Lane;
class RoadSegment;

//
// A lightweight statically allocated, singly-linked list
//

const int MAX_ITEMS = 30;

struct Item 
{
    rmt::Vector p;
    int next;
};

class StaticList
{
public:
    Item mItems[MAX_ITEMS];

    void Clear()
    {
        int i=0;
        for(i; i<MAX_ITEMS; i++)
        {
            mItems[i].next = -1;
        }
        mFirstFree = 0;
    }

    StaticList()
    {
        Clear();
    }

    int GetNumItems()
    {
        return mFirstFree;
    }

    Item* InsertItem( rmt::Vector p, Item* prevItem )
    {
        if( mFirstFree >= MAX_ITEMS )
        {
            return NULL;
        }
        int freeIndex = mFirstFree;
        mItems[freeIndex].p = p;
        mItems[freeIndex].next = (prevItem != NULL)? prevItem->next : -1;
        
        if( prevItem != NULL )
        {
            prevItem->next = freeIndex;
        }
        else if( freeIndex > 0 )
        {
            mItems[freeIndex-1].next = freeIndex;
        }
        mFirstFree++;
        return &(mItems[freeIndex]);
    }

private:
    
    int mFirstFree;
};








//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TrafficLocomotion : public VehicleLocomotion
{
public:

    enum {
        ON_ROAD_HISTORY_SIZE = 66
    };

    static const float SECONDS_BETW_HISTORY_UPDATES;

    TrafficLocomotion(Vehicle* vehicle);
    virtual ~TrafficLocomotion();

    void Init();
    void Init( Vehicle* vehicle, 
               Lane* lane,
               unsigned int laneIndex, //0 is curbside
               RoadSegment* segment,
               unsigned int segmentIndex,
               float t, 
               float kmh );

    void InitPos( const rmt::Vector& pos );
    void InitFacing( const rmt::Vector& facing );
    void InitVehicleAI( Vehicle* vehicle );
    void InitLane( Lane* lane, unsigned int laneIndex, float mps );
    void InitSegment( RoadSegment* segment, unsigned int segmentIndex, float t );
    
    //////////////////// VEHICLELOCOMOTION STUFF ////////////////////
    virtual void PreSubstepUpdate();
    virtual void PreCollisionPrep(bool firstSubstep);
    virtual void UpdateVehicleGroundPlane();
    virtual void PreUpdate();
    virtual void Update(float seconds);
    virtual void PostUpdate();
    virtual void CompareNormalAndHeight(int index, rmt::Vector& normalPointingAtCar, rmt::Vector& groundContactPoint) {};
    ///////////////////////////////////////////////////////////////////

    TrafficAI* GetAI();
    Lane* GetAILane();
    Lane* GetAIPrevLane();
    void SetAISpeed( float mps );
    float GetAISpeed();
    bool IsInIntersection() const;
    void UpdateAI(unsigned int ms);
    void SetActive( bool isActive );
    bool GetActive() const;
    void SetAIState( enum TrafficAI::State state );

    void UpdateLanes( TrafficVehicle* tv, Lane* oldLane, Lane* newLane );

    bool BuildLaneChangeCurve( 
        RoadSegment* oldSegment, 
        const float oldT,
        unsigned int oldLaneIndex,
        unsigned int newLaneIndex,
        const float dist);

    bool BuildArbitraryCurve(
        const rmt::Vector& startPos,
        const rmt::Vector& startDir,
        const rmt::Vector& endPos,
        const rmt::Vector& endDir );

    void GetSplineCurve( rmt::Vector*& ways, int& npts, int& currWay );

public:
    float mLaneChangeProgress; // progress in meters while in lane change state
    float mLaneChangeDist; // total distance over which we want to lane change
    unsigned int mLaneChangingFrom; // index of lane we are lane changing FROM
    float mOutLaneT;

    float mCurrPathLocation;

    float mActualSpeed;

private:
    Vehicle*    mVehicle;
    TrafficAI*   mMyAI;

    ///// For building splines at intersections //////
    CubicBezier mSplineMaker;
    rmt::Vector* mWays;
    int mNumWays;
    //////////////////////////////////////////////////

    rmt::Vector mCurrPath;
    float mCurrPathLength;
    int mCurrWay;

    bool mIsInIntersection;

    rmt::Vector mPrevPos;

    float mSecondsTillCheckForFreeLane; 

    ////////////// History ///////////////
    VectorHistory<ON_ROAD_HISTORY_SIZE> mFacingHistory;
    VectorHistory<ON_ROAD_HISTORY_SIZE> mPosHistory;
    /////////////////////////////////////////////

    float mSecondsSinceLastAddToHistory;

private:
    // Helpers
    void FindOutLane( const Lane* inLane, 
        unsigned int inLaneIndex,
        Lane*& outLane, 
        unsigned int& outLaneIndex );

    void BuildCurve ( RoadSegment* inSegment, 
        unsigned int inLaneIndex, 
        RoadSegment* outSegment, 
        unsigned int outLaneIndex );

    bool EnterIntersection();

    void PivotFrontWheels( rmt::Vector facing );

    void StopSuddenly( rmt::Vector& pos, rmt::Vector& facing );

    ////////////// WASTEFUL CONSTRUCTORS ////////////////////
    TrafficLocomotion();
    TrafficLocomotion( const TrafficLocomotion& trafficlocomotion );
    TrafficLocomotion& operator=( const TrafficLocomotion& trafficlocomotion );
    ////////////////////////////////////////////////////////////
};

inline bool TrafficLocomotion::IsInIntersection() const
{
    return mIsInIntersection;
}
inline bool TrafficLocomotion::GetActive() const
{
    return mMyAI->mIsActive;
}
inline void TrafficLocomotion::SetActive( bool isActive ) 
{
    mMyAI->mIsActive = isActive;
}
inline void TrafficLocomotion::SetAISpeed( float mps )
{
    mMyAI->SetAISpeed( mps );
}
inline float TrafficLocomotion::GetAISpeed()
{
    return mMyAI->GetAISpeed();
    }
inline Lane* TrafficLocomotion::GetAILane()
{
    return mMyAI->GetLane();
}
inline void TrafficLocomotion::SetAIState( enum TrafficAI::State state )
{
    mMyAI->SetState(state);
}
inline TrafficAI* TrafficLocomotion::GetAI()
{
    return mMyAI;
}

#endif //TRAFFICLOCOMOTION_H


