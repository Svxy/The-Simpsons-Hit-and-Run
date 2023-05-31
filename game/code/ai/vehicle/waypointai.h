//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        waypointai.h
//
// Description: Blahblahblah
//
// History:     10/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef WAYPOINTAI_H
#define WAYPOINTAI_H

//========================================
// Nested Includes
//========================================

#include <ai/vehicle/vehicleai.h>

//========================================
// Forward References
//========================================

class Locator;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WaypointAI : public VehicleAI
{
public:

    static const float DEFAULT_TRIGGER_RADIUS;

    WaypointAI( 
        Vehicle* pVehicle, 
        bool enableSegmentOptimization=true, 
        float triggerRadius=DEFAULT_TRIGGER_RADIUS,
        bool autoResetOnDestroyed=false );

    virtual ~WaypointAI();

    void ClearWaypoints();
    void AddWaypoint( Locator* loc );

    virtual void Update( float timeins );
    virtual void Initialize();
    virtual void Reset();

    int GetCurrentWayPoint() const { return miCurrentWayPoint; };

    // get/set collectible from mission objective
    void SetCurrentCollectible( int collectible );
    int GetCurrentCollectible() const;

    // get/set current lap 
    void SetCurrentLap( int lap );
    int GetCurrentLap() const;

    // get/set dist to current collectible
    float GetDistToCurrentCollectible() const;
    void SetDistToCurrentCollectible( float dist );

    static const int MAX_WAYPOINTS = 32;

    enum WaypointAIType
    {
        RACE,
        EVADE,
        TARGET
    };

    void SetAIType( WaypointAIType type );

    void UseTurbo();

protected:
    void FollowWaypoints();
    void SetCurrentWayPoint( int index );
    virtual bool MustRepopulateSegments();
    virtual bool TestReachedTarget( const rmt::Vector& start, const rmt::Vector& end );

    virtual void GetClosestPathElementToTarget( 
        rmt::Vector& targetPos,
        RoadManager::PathElement& elem,
        RoadSegment*& seg,
        float& segT,
        float& roadT );

    virtual void DoCatchUp( float timeins );

    void UpdateNeedsResetOnSpot( float timeins );

    void UpdateNeedToWaitForPlayer( float timeins );

    void PossiblyUseTurbo();

private:
    virtual int RegisterHudMapIcon();

    bool TestWaypoint( int waypoint );

    //Prevent wasteful constructor creation.
    WaypointAI( const WaypointAI& waypointai );
    WaypointAI& operator=( const WaypointAI& waypointai );


private:
    struct WayPoint
    {
        Locator* loc;
        RoadManager::PathElement elem;
        float segT;
        RoadSegment* seg; 
        float roadT;
    };
    WayPoint mpWayPoints[ MAX_WAYPOINTS ];
    int miNumWayPoints;
    int miCurrentWayPoint;
    int miNextWayPoint;

    // distance at and closer than which we consider a waypoint reached
    // (and move along to next waypoint)
    float mTriggerRadius;


    ///////////////// RACE DATA ///////////////////
    // we keep pieces of the race data in this class
    // because catch-up logic will need to use them
    float mDistToCurrentCollectible;
    int miCurrentCollectible;
    int miNumLapsCompleted;

    bool mCurrWayPointHasMoved : 1;
    ///////////////// Auto-resetting stuff ///////////
    bool mNeedsResetOnSpot     : 1;
    bool mAutoResetOnDestroyed : 1;
    float mSecondsTillResetOnSpot;
    WaypointAIType mWaypointAIType;

    ////////////// Turbo logic stuff ///////////////
    float mSecondsSinceTurboUse;


    float mSecondsWaitingForPlayer;

};

inline void WaypointAI::SetCurrentCollectible( int collectible )
{
    miCurrentCollectible = collectible;
}
inline int WaypointAI::GetCurrentCollectible() const
{
    return miCurrentCollectible;
}
inline void WaypointAI::SetCurrentLap( int lap )
{
    miNumLapsCompleted = lap;
}
inline int WaypointAI::GetCurrentLap() const
{
    return miNumLapsCompleted;
}
inline void WaypointAI::SetDistToCurrentCollectible( float dist )
{
    rAssert( dist >= 0.0f );
    mDistToCurrentCollectible = dist;
}
inline float WaypointAI::GetDistToCurrentCollectible() const
{
    return mDistToCurrentCollectible;
}
inline void WaypointAI::SetAIType( WaypointAIType type )
{
    mWaypointAIType = type;
}

#endif //WAYPOINTAI_H
