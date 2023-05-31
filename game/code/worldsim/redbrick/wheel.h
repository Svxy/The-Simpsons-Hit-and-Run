/*===========================================================================
   wheel.h

   created Jan 29, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#ifndef _WHEEL_H
#define _WHEEL_H

#include <radmath/radmath.hpp>

#include <worldsim/redbrick/vehicle.h>

class SuspensionJointDriver;

enum WheelState{ WS_NORMAL, WS_SLIP, WS_LOCOMOTIVE_SLIDE, WS_LOCOMOTIVE_FREE_SPIN };

class Wheel
{
private:

    friend class SuspensionJointDriver;
    friend class Vehicle;   
    friend class PhysicsLocomotion;
    friend class GeometryVehicle;       // everyone's friends with the Wheel :)
    friend class RedBrickCollisionSolverAgent;
     
    
    Wheel();
    ~Wheel();

    Vehicle* mVehicle;

    // TODO - initialize k and c from vehicle too...easier to tune?
    void Init(Vehicle* vehicle, int wheelNum, float radius, bool steer, bool drive);
    void Reset();

    void SetDesignerParams(Vehicle::DesignerParams* dp);

    // this will check the one we're passing in against the one stored
    float SetYOffsetFromCurrentPosition(float yoffset);
    void ResolveOffset();   // modify mYOffset by the final thing left in mObjectSpaceYOffsetFromCurrentPosition
    
    float GetYCorrectionValue();  

    float CalculateSuspensionForce(float suspensionPointYVelocity, float dt);
    void CalculateRenderingSpinUpRateBase(float topSpeedKmh);

    bool mSteerWheel;
    bool mDriveWheel;

    int mWheelNum;
    float mRadius;
    float mObjectSpaceYOffsetFromCurrentPosition;    // set frame to frame by collision agent


    float mYOffset; // object space offset from suspension rest point

    void CalculateRotAngle(float dt);
    float mRotAngle;
    float mCumulativeRot;
    float mTotalTime;

    float mLimit;
    float mk;   // spring constant
    float mc;   // damper constant

    float mqk;  // new test - quadratic spring

    bool mWheelInCollision;
    bool mWheelBottomedOutThisFrame;

    float mSpringRelaxRate;

    float mWheelTurnAngle;

    

    WheelState mWheelState;

    float mWheelRenderSpeedRadPerSecond;

    float mRenderingSpinUpRate;
    float mRenderingSpinUpRateBase;
    
    
    //---------------------
    // designer parameters:
    //---------------------

    float mDpSuspensionLimit;    // multiplier on radius
    float mDpSpringk;
    float mDpDamperc;            // normalized values

    float mDpRenderingSpinUpSpeedThresholdPercentage;


};


#endif // _WHEEL_H