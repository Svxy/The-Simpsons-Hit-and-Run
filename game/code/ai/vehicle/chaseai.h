//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        chaseai.h
//
// Description: Blahblahblah
//
// History:     10/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef CHASEAI_H
#define CHASEAI_H

//========================================
// Nested Includes
//========================================

#include <ai/vehicle/vehicleai.h>
#include <simcollision/collisionvolume.hpp>
//========================================
// Forward References
//========================================
//class Vehicle;
class Avatar;
class CollisionVolume;


static const float TAIL_BEELINE_DIST = 20.0f;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ChaseAI : public VehicleAI
{
public:
    ChaseAI( Vehicle* pVehicle, float beelineDist=TAIL_BEELINE_DIST );
    virtual ~ChaseAI();

    virtual void Reset();
    virtual void Initialize();
    virtual void Update( float timeins );

protected:
    virtual bool MustRepopulateSegments();

    virtual bool TestReachedTarget( const rmt::Vector& start, const rmt::Vector& end );

    virtual void GetClosestPathElementToTarget( 
        rmt::Vector& targetPos,
        RoadManager::PathElement& elem,
        RoadSegment*& seg,
        float& segT,
        float& roadT );

    virtual void DoCatchUp( float timeins );

    void UpdateTarget();

    void Beeline( float dist2target );

    // All return true if line segment defined by start & end go through the volume colVol
    bool TestIntersectBBox( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol );

    /*** HOPE I DON'T HAVE TO IMPLEMENT THESE JUST YET
    bool TestIntersectOBBox( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol );
    bool TestIntersectSphere( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol );
    bool TestIntersectCylinder( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol );
    bool TestIntersectWall( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol );
    ***/

private:
    virtual int RegisterHudMapIcon();

    Avatar* mpTarget;

    bool mTargetHasMovedToAnotherPathElement : 1;

    RoadManager::PathElement mTargetPathElement;
    RoadSegment* mTargetRoadSegment;
    float mTargetRoadSegmentT;
    float mTargetRoadT;

    float mBeelineDist;

private:
    //Prevent wasteful constructor creation.
    ChaseAI( const ChaseAI& chaseai );
    ChaseAI& operator=( const ChaseAI& chaseai );

};


#endif //CHASEAI_H
