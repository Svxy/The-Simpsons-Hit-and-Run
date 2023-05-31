/*===========================================================================
   physicslocomotion.h

   created April 24, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#ifndef _PHYSICSLOCOMOTION_H
#define _PHYSICSLOCOMOTION_H

#include <worldsim/redbrick/vehiclelocomotion.h>
#include <render/intersectmanager/intersectmanager.h> // For the terrain type enumeration.
#include <radmath/radmath.hpp>

class Vehicle;
class Wheel;

class PhysicsLocomotion : public VehicleLocomotion
{
public:

    PhysicsLocomotion(Vehicle* vehicle);
    ~PhysicsLocomotion();

    virtual void PreSubstepUpdate();
    virtual void PreCollisionPrep(bool firstSubstep);

    virtual void UpdateVehicleGroundPlane();

    virtual void PreUpdate();
    virtual void Update(float dt);
    virtual void PostUpdate();
    

    // call this from Vehicle::SetTransform
    void SetTerrainIntersectCachePointsForNewTransform();

    virtual void CompareNormalAndHeight(int index, rmt::Vector& normalPointingAtCar, rmt::Vector& groundContactPoint);
  
//private: for the purposes of debug printout

    void MoveWheelsToBottomOfSuspension();
        
    void FetchWheelTerrainCollisionInfo();
    void UseWheelTerrainCollisionInfo(float dt);

    void TipTest();
    void ApplyAngularDrag();
    
    void StuckOnSideTest(float dt);
    
    void DurangoStyleStabilizer(float dt);


    // delete all this crap soon
    
    //float mWheelTerrainCollisionFixDepth[4];
    //rmt::Vector mWheelTerrainCollisionPoints[4];
    //rmt::Vector mWheelTerrainCollisionNormals[4];
    //bool mGoodWheelTerrainCollisionValue[4];

    bool mFoundTri[4];
    bool mFoundPlane[4];


    void CorrectWheelYPositions();  // used to be preupdate

    void SetForceApplicationPoints();
    rmt::Vector mForceApplicationPoints[4];
    rmt::Vector mSuspensionPointVelocities[4];

    void ApplySuspensionForces(float dt, bool atrest);
    float mCachedSuspensionForceResults[4];    
    float CalculateSuspensionForce(Wheel* wheel, float suspensionPointYVelocity, float dt,  float& forceToUse);

    void ApplyControllerForces2(float dt);
    void GasForce2(rmt::Vector& forceResult, int wheelNum, float dt);
    void BrakeForce2(rmt::Vector& forceResult, int wheelNum, float dt);
    void ReverseForce2(rmt::Vector& forceResult, int wheelNum);
    void SteeringForce2(rmt::Vector& forceResult, int wheelNum, float dt);
    //void LimitTireForce(rmt::Vector& totalForce, int wheelNum);
    void TestControllerForces(rmt::Vector* totalForce);
    void SetSkidLevel();

    void UpdateSteeringForce(float dt);
    float mCurrentSteeringForce;

    void ApplyTerrainFriction(rmt::Vector* totalForce);
    

    void TestSteeringForce(rmt::Vector& force, int index);

    void EBrakeEffect(rmt::Vector& forceResult, int wheelNum);
    void DoughnutTest();

    void ApplyDragForce();
    void Weeble();
    
    void HighSpeedInstabilityTest();
    
    void ApplyRollingFriction();

    void LowSpeedTest();

    // just for convenience in internal methods
    Vehicle* mVehicle;

    struct TerrainIntersectCache
    {
        rmt::Vector closestTriPosn;
        rmt::Vector closestTriNormal;
        rmt::Vector planePosn;
        rmt::Vector planeNorm;
        eTerrainType mTerrainType;
        bool mInteriorTerrain;
    };

    TerrainIntersectCache mTerrainIntersectCache[4];    // is this wasting too much memory?

    // need this because if we're driving on a static collision volume the terrain normal could be way off!
    rmt::Vector mIntersectNormalUsed[4];
    
    friend class Vehicle;

 
   
};


#endif // _PHYSICSLOCOMOTION_H
