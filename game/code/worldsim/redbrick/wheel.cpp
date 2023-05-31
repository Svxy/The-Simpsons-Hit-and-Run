/*===========================================================================
   wheel.cpp

   created Jan 29, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#include <worldsim/redbrick/wheel.h>
#include <radmath/radmath.hpp>
#include <worldsim/worldphysicsmanager.h>


//------------------------------------------------------------------------
Wheel::Wheel()
{
    mRadius = 0.0f;    
    mObjectSpaceYOffsetFromCurrentPosition = 0.0f;

    mWheelNum = -1;     // unset flag
    mYOffset = 0.0f;

    mLimit = 0.0f;
    mk = 0.0f;   
    mc = 0.0f;   

    mWheelInCollision = false;  // TODO - is this method adequate!?
    mWheelBottomedOutThisFrame = false;

    mSpringRelaxRate = 0.5f;    // second to move from limit to 0 when not in collision

    mWheelTurnAngle = 0.0f;
    
    mRotAngle = 0.0f;
    mCumulativeRot = 0.0f;
    mTotalTime = 0.0f;

    mWheelRenderSpeedRadPerSecond = 0.0f;
    //mSpinUpRate = 10.0f;    //? - I think this is in (rad/s)/s

    //mRenderingSpinUpRateBase = 0.1f;    // this is actually in units of rad/s - 
                           // it's a direct setting of the increment amount, for now

    mRenderingSpinUpRateBase = 100.0f;    // back to rad/s

    mRenderingSpinUpRate = 0.0f; // this one is scaled by the gas input

    // % of top speed at which mode goes back to NORMAL, if we were slipping 'cause we floored it initially
    mDpRenderingSpinUpSpeedThresholdPercentage = 0.3f;
    //mDpRenderingSpinUpSpeedThresholdPercentage = 0.2f;


    mSteerWheel = false;
    mDriveWheel = false;

   
    mVehicle = 0;

    //----------------
    // designer values
    //----------------

    mWheelState = WS_NORMAL;


    //---------
    // redbrick     normal
    //---------
 
    // NOTE: these are all overwritten shortly after init
    // these are just default values that are ok for the ferrari
    
    mDpSuspensionLimit = 0.4f;  // try this for ferrari
    
    mDpSpringk = 0.5f; // prett good value for redbrick
    mDpDamperc = 0.2f;  // scale on critical damping            temp for debugging

      
}
    

//------------------------------------------------------------------------
Wheel::~Wheel()
{
    //
}


//------------------------------------------------------------------------
void Wheel::Init(Vehicle* vehicle, int wheelNum, float radius, bool steer, bool drive)
{
    mVehicle = vehicle;

    mWheelNum = wheelNum;
    mRadius = radius;  

    mSteerWheel = steer;
    mDriveWheel = drive;

    //
    // calculate 'real' values from designer params
    //

    mLimit = mDpSuspensionLimit * mRadius;

    // TODO
    // temp hack for lack of phizsim
    //float gravity_y = 9.81f;

    const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();// this or CGS ?

    //float gravity_y = 9.81f;
    float gravity_y = -1.0f * gravity.y;

    //rmt::Vector gravity = SG::phizSim.mSimEnvironment->Gravity();
    //float force = mVehicle->mDesignerParams.mDpMass * rmt::Fabs(gravity.y);
    float force = mVehicle->mDesignerParams.mDpMass * gravity_y;
    mk = force / mLimit;
    mk *= mDpSpringk;

    //mqk = rmt::Sqrt(mk) * 2.0f; // first guess at quad spring
    mqk = mk * 5.0f; // first guess at quad spring
    //mqk = mk * 10.0f; // first guess at quad spring

    float criticalDamping = 2.0f * (float)rmt::Sqrt(mk * mVehicle->mDesignerParams.mDpMass);
    mc = criticalDamping * mDpDamperc;
    

    // these will get recalculated wheh SetDesignParams is called

}

//------------------------------------------------------------------------
void Wheel::SetDesignerParams(Vehicle::DesignerParams* dp)
{

    mDpSuspensionLimit = dp->mDpSuspensionLimit;
    
    mDpSpringk = dp->mDpSpringk;
    mDpDamperc = dp->mDpDamperc;

    
    //----------

    mLimit = mDpSuspensionLimit * mRadius;

    // TODO
    // temp hack for lack of phizsim
    //float gravity_y = 9.81f;

    const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();// this or CGS ?

    //float gravity_y = 9.81f;
    float gravity_y = -1.0f * gravity.y;

    //rmt::Vector gravity = SG::phizSim.mSimEnvironment->Gravity();
    //float force = mPhysicsVehicleOwner->mDpMass * rmt::Fabs(gravity.y);
    float force = mVehicle->mDesignerParams.mDpMass * gravity_y;
    mk = force / mLimit;
    mk *= mDpSpringk;

    //mqk = rmt::Sqrt(mk) * 2.0f; // first guess at quad spring
    mqk = mk * 5.0f; // first guess at quad spring
    //mqk = mk * 10.0f; // first guess at quad spring


    float criticalDamping = 2.0f * (float)rmt::Sqrt(mk * mVehicle->mDesignerParams.mDpMass);
    mc = criticalDamping * mDpDamperc;


    CalculateRenderingSpinUpRateBase(dp->mDpTopSpeedKmh);

}

//------------------------------------------------------------------------
float Wheel::CalculateSuspensionForce(float suspensionPointYVelocity, float dt)
{
    
    rAssert(0);

    float force = 0.0f;

    if(mWheelInCollision)
    {

        float springForce = mk * mYOffset;

        // apply extra force if we're close to bottoming out
        //
        // TODO
        // hmmmm..... revisit this...
        // replace with quadratic

        //
        // or... topping out!
        //
        if((mLimit - rmt::Fabs(mYOffset)) < mLimit * 0.25f)        
        {            
            springForce *= 3.0f;
        }

        // for now, only add damping if chassis is trying to compress suspension - ie. y velocity is down, -ve
        float damperForce = 0.0f;

        if(suspensionPointYVelocity < 0.0f)
        {
            damperForce = mc * -1.0f * suspensionPointYVelocity;
        }
        else
        {
            // like the havok guys, only let damper pull down at about 1/3 of push up
            damperForce = mc * -1.0f * suspensionPointYVelocity * 0.3f;

        }
 
        force = springForce + damperForce;
    }
    else
    {
        // need to relax spring somehow
        float relaxDistance = mLimit / mSpringRelaxRate * dt;
        if(mYOffset >= relaxDistance)
        {
            mYOffset -= relaxDistance;
        }
        else if(mYOffset <= -relaxDistance)
        {
            mYOffset += relaxDistance;
        }

    }
    return force;


}



//------------------------------------------------------------------------------------------------
void Wheel::CalculateRenderingSpinUpRateBase(float topSpeedKmh)
{
    // mDpRenderingSpinUpSpeedThreshold;    
    
    // at what rad/s is the wheel going % of top speed

    float topspeedmps = topSpeedKmh / 3.6f;

    float thresholdmps = topspeedmps * mDpRenderingSpinUpSpeedThresholdPercentage;

    float linearDistancePerRev = rmt::PI * mRadius * 2.0f;

    // rev/s = rev/dist * dist/s

    // ! fuck you idiot
    // we need rads not revs
    float linearDistancePerRad = linearDistancePerRev / (2.0f * rmt::PI);
    

    mRenderingSpinUpRateBase = (1.0f / linearDistancePerRad) * thresholdmps;



}


//------------------------------------------------------------------------
void Wheel::CalculateRotAngle(float dt) 
{
    if(mVehicle->mEBrake > 0.0f && mWheelInCollision)
    {    
        // now, if no gas, don't rotate
        if(mVehicle->mGas == 0.0f && this->mDriveWheel == true)
        {
            mRotAngle = 0.0f;     
            return;
        }
                
    }


    float speedAlongFacing;

    rmt::Vector wheelDirection = mVehicle->mVehicleFacing;

    if(mSteerWheel)
    {        
        rmt::Matrix steeringMatrix; // TODO - make this a class member?
        steeringMatrix.Identity();
        steeringMatrix.FillRotateY(mWheelTurnAngle);

        wheelDirection.Transform(steeringMatrix);
    }

    speedAlongFacing = wheelDirection.DotProduct(mVehicle->mSuspensionPointVelocities[mWheelNum]);

    //float linearDistance = speedAlongFacing * dt;

    float linearDistancePerRev = rmt::PI * mRadius * 2.0f;

    float revPerTime = speedAlongFacing / linearDistancePerRev;

    float radiansPerTime = revPerTime * 2.0f * rmt::PI;

    // ?
    // see if vehicle's doin' a burnout
    if(mVehicle->mBurnoutLevel > 0.0f && mDriveWheel)
    {
        if(mVehicle->mGas > mVehicle->mBrake)
        {
            radiansPerTime = mRenderingSpinUpRateBase;
        }
        else
        {
           radiansPerTime = -1.0f * mRenderingSpinUpRateBase;
        }
    }


    /*
    if(mWheelState == WS_LOCOMOTIVE_SLIDE || mWheelState == WS_LOCOMOTIVE_FREE_SPIN)
    {
        if(radiansPerTime >= mRenderingSpinUpRate)
        {
            // kick out of thid mode and use the calculated value
            //
            // TODO - how to then easily tune the mRenderingSpinUpRateBase?
            //
            // should be able to calculate based on some % of top speed
            // yeah - no fucking timer needed!

            mWheelState = WS_NORMAL;

        }
        else
        {
            //mRotAngle *= 1.5f;
            //otAngle = mRenderingSpinUpRate;
            radiansPerTime = mRenderingSpinUpRate;
        }
        
    }
    */

    mRotAngle = radiansPerTime * dt;

    //if(rmt::Fabs(radiansPerTime - (2.0f * rmt::PI)) < 0.1f)
    //{
    //    int stophere = 1;
   // }


    // fucking hack to (temporarily?) fix wheel rot on ps2
    /*
    mTotalTime += dt;

    if(mTotalTime > 30.0f)  // value is in seconds
    {
        mTotalTime = 0.0f;
        mCumulativeRot = 0.0f;
    }
    */

    mCumulativeRot += mRotAngle;
   
    if(mCumulativeRot > rmt::PI_2)
    {
        mCumulativeRot -= rmt::PI_2;

        //safeguard - higly unlikely we'd get here...
        if(mCumulativeRot > rmt::PI_2)
        {
            // still?
            // something strange must have happened
            mCumulativeRot = 0.0f;
        }
        
    }
   



    // TODO 
    // what we might want to try here is always calculate the wheel rotation to render based on desired forces
    // not the actual forces we apply.
    //
    // add booleans to the Cap methods?
    //
    // also need to throw in some consideration for braking - draw wheels locked.
    //
    // furthermore, this should be done before the vehicle PostUpdate where the pose drivers actually change the joint matrices


}

//=============================================================================
// Wheel::SetYOffsetFromCurrentPosition
//=============================================================================
// Description: 
//
// Parameters:  (float yoffset)
//
// Return:      float 
//
//      note -  if we've bottomed out, this will return the overflow
//              amount that the suspension could not absorb
//              if we didn't bottom out it will return 0.0f;
//
//
//=============================================================================
float Wheel::SetYOffsetFromCurrentPosition(float yoffset)
{

    // the parameter mObjectSpaceYOffsetFromCurrentPosition is a bit verbose at this
    // point since before doing wheel collision we set the wheels to the bottom of the 
    // suspension limit, so at this poitn the currentposition is always the same

    if(yoffset > mObjectSpaceYOffsetFromCurrentPosition)
    {
        mObjectSpaceYOffsetFromCurrentPosition = yoffset;
    }
    // TODO - make sure this gets reset to 0.0 at the right place


    // this call got propagated to us from the collision solver, so I think this is the place to set the flag
    mWheelInCollision = true;    

    // temp
    // TODO - remove
    //return 0.0f;


    if(mObjectSpaceYOffsetFromCurrentPosition > 2.0f * mLimit)
    {
        // suspension has bottomed out
        //
        //  TODO - rename this method?
        //
        // TODO - double check this

        float overflow = mObjectSpaceYOffsetFromCurrentPosition - (2.0f * mLimit);
        mObjectSpaceYOffsetFromCurrentPosition = 2.0f * mLimit;

        return overflow;
        //return true;
    }

    // ??
    // TODO
    // is this ok?
    // should be if we always started out at the bottom of the suspension limit
    if(mObjectSpaceYOffsetFromCurrentPosition < 0.0f)
    {
        rAssert(0);
        mObjectSpaceYOffsetFromCurrentPosition = 0.0f;
    }

    return 0.0f;

}


//------------------------------------------------------------------------
void Wheel::ResolveOffset()
{
    // we assume at this point anyone who wants to has called SetYOffsetFromCurrentPosition
    mYOffset += mObjectSpaceYOffsetFromCurrentPosition;


    // temp
    // TODO - remove
    //mObjectSpaceYOffsetFromCurrentPosition = 0.0f;
    //return;
    
    if(mYOffset > mLimit)
    {
        mYOffset = mLimit;
    }
    if(mYOffset < -mLimit)
    {
        mYOffset = -mLimit;
    }

    // !
    //
    // mYOffset is only of use internally to this class
    //
    // this is NOT the value other shit should use to move suspension points around and what have you.



    mObjectSpaceYOffsetFromCurrentPosition = 0.0f;
    //mWheelInCollision = false;  // TODO - is this method adequate!?

}


//------------------------------------------------------------------------
float Wheel::GetYCorrectionValue()
{
    return mObjectSpaceYOffsetFromCurrentPosition;
}




//------------------------------------------------------------------------
void Wheel::Reset()
{
    mYOffset = 0.0f;
    mObjectSpaceYOffsetFromCurrentPosition = 0.0f;

    mRotAngle = 0.0f;

    mCumulativeRot = 0.0f;
    mTotalTime = 0.0f;

    mWheelInCollision = false;
    mWheelBottomedOutThisFrame = false;

    mWheelState = WS_NORMAL;

    mWheelRenderSpeedRadPerSecond = 0.0f;

}





