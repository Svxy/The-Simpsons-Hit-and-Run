/*===========================================================================
   physicslocomotion.cpp

   created April 24, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#include <simcommon/simstatearticulated.hpp>
#include <simphysics/articulatedphysicsobject.hpp>
#include <worldsim/redbrick/physicslocomotion.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/wheel.h>
#include <worldsim/worldphysicsmanager.h>


#include <poser/pose.hpp>
#include <poser/poseengine.hpp>
#include <poser/posedriver.hpp>

#include <render/IntersectManager/IntersectManager.h>

#include <cheats/cheatinputsystem.h>

using namespace sim;

/*
    data that should move from Vehicle to here...

    struct TerrainIntersectCache
    {
        rmt::Vector closestTriPosn;
        rmt::Vector closestTriNormal;
        rmt::Vector planePosn;
        rmt::Vector planeNorm;
    };

    TerrainIntersectCache mTerrainIntersectCache[4];    // is this wasting too much memory?


???
    float mRollingFrictionForce;
    float mTireLateralResistance;
    float mSlipGasModifier;
???





*/


//------------------------------------------------------------------------
PhysicsLocomotion::PhysicsLocomotion(Vehicle* vehicle) : VehicleLocomotion(vehicle)
{
    mVehicle = vehicle;
    //
    int i;
    for(i = 0; i < 4; i++)
    {
        mForceApplicationPoints[i].Clear();
        mSuspensionPointVelocities[i].Clear();  

        mCachedSuspensionForceResults[i] = 0.0f;

        // TODO - initialize better?
        // everytime control is switched to VL_PHYSICS
        //mWheelTerrainCollisionFixDepth[i] = 0.0f;
       
        //mWheelTerrainCollisionNormals[i].x = 0.0f;
        //mWheelTerrainCollisionNormals[i].y = 1.0f;
        //mWheelTerrainCollisionNormals[i].z = 0.0f;

        //mWheelTerrainCollisionPoints[i].Clear();

        //mGoodWheelTerrainCollisionValue[i] = false;
    }
    
    mCurrentSteeringForce = mVehicle->mDesignerParams.mDpTireLateralResistanceNormal;

}



//------------------------------------------------------------------------
PhysicsLocomotion::~PhysicsLocomotion()
{
    //
}



//=============================================================================
// PhysicsLocomotion::SetTerrainIntersectCachePointsForNewTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::SetTerrainIntersectCachePointsForNewTransform()
{
    rmt::Vector tempVec;

    int i;
    for(i = 0; i < 4; i++)
    {
        tempVec = mVehicle->mSuspensionWorldSpacePoints[i];
        tempVec.y -= 2.0f * mVehicle->mWheels[i]->mRadius;

        mTerrainIntersectCache[i].closestTriPosn = tempVec;
        mTerrainIntersectCache[i].closestTriNormal.Set(0.0f, 1.0f, 0.0f);

        mTerrainIntersectCache[i].planePosn = tempVec;
        mTerrainIntersectCache[i].planeNorm.Set(0.0f, 1.0f, 0.0f);
        
        mTerrainIntersectCache[i].mTerrainType = TT_Road;
        mTerrainIntersectCache[i].mInteriorTerrain = false;
        
        mIntersectNormalUsed[i].Set(0.0f, 1.0f, 0.0f);
    }

}


//=============================================================================
// PhysicsLocomotion::MoveWheelsToBottomOfSuspension
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::MoveWheelsToBottomOfSuspension()
{
      
    poser::Pose* pose = mVehicle->mPoseEngine->GetPose();
   
    // want to call false here because we want the movement of the hierarchy based on animation to also be available for collision
    // TODO - make sure this is done in the traffic locomotion case as well.
    mVehicle->mPoseEngine->Begin(false);  //Cary Here - I've moved it.
    //mVehicle->mPoseEngine->Begin(true);  //Cary Here - I've moved it.
    // TODO - even need to do this?


    // TODO
    // note:
    // are we making the assumption that the animation doesn't move the suspension points around?
    // for now, yes.


    // TODO - only do this if wheelInCollision?
  

    int i;
    for(i = 0; i < 4; i++)
    {
        Wheel* wheel = mVehicle->mWheels[i];

        poser::Joint* joint = pose->GetJoint(mVehicle->mWheelToJointIndexMapping[i]);
        rmt::Vector trans = joint->GetObjectTranslation();

        //trans.y -= mVehicle->mWheels[i]->mLimit;                                          
        // TODO - verify that the -= is the thing to do here
        trans.y -= wheel->mLimit;

        trans.y += mVehicle->mDesignerParams.mDpSuspensionYOffset;

        joint->SetObjectTranslation(trans);

    

        // TODO - if this method actually works, make nicer interface with wheel to do this

        //
        // remember, mYOffset is the offset (upwards) from the bottom of the suspension limit, to fix wheel out of collision (or just barely in collision)
        wheel->mYOffset = -1.0f * wheel->mLimit;
        
        wheel->mWheelInCollision = false;
        wheel->mWheelBottomedOutThisFrame = false;
                

    }

    // just in case   
    //
    //  TODO - review why the hell you're doing this? - this one's pretty important I think
    CollisionObject* collObj = mVehicle->mSimStateArticulated->GetCollisionObject();
    collObj->SetHasMoved(true);

   
}


//=============================================================================
// PhysicsLocomotion::UpdateVehicleGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::UpdateVehicleGroundPlane()
{
    // create a new transform for the ground plane sim state and set it.

    rmt::Vector p, n;
    p.Set(0.0f, 0.0f, 0.0f);
    n.Set(0.0f, 0.0f, 0.0f);

    /*
    int i;
    int count = 0;
    for(i = 0; i < 4; i++)
    {
        //if(mFoundPlane[i])

        // try doing this with whatever's in there
        // old or new
        //
        // TODO - is this safe?

        {
            p.Add(mVehicle->mTerrainIntersectCache[i].planePosn);
            n.Add(mVehicle->mTerrainIntersectCache[i].planeNorm);
        }
        

    }
  
    p.Scale(0.25f);
    n.Scale(0.25f);
    */

    // new idea for ground plane
    // pick the most upright normal, and the lowest height point
    // ??

    p = mTerrainIntersectCache[0].planePosn;
    n = mTerrainIntersectCache[0].planeNorm;

    int i;    
    for(i = 1; i < 4; i++)
    {   
        if(mTerrainIntersectCache[i].planePosn.y < p.y)
        {
            p = mTerrainIntersectCache[i].planePosn;
        }

        if( GetWorldPhysicsManager()->mWorldUp.DotProduct(mTerrainIntersectCache[i].planeNorm) >
            GetWorldPhysicsManager()->mWorldUp.DotProduct(n) )
        {
            n = mTerrainIntersectCache[i].planeNorm;
        }     

    }
  

    // new approach with manual sim state
    
    mVehicle->mGroundPlaneWallVolume->mPosition = p;
    mVehicle->mGroundPlaneWallVolume->mNormal = n;
    
    
    sim::CollisionObject* co = mVehicle->mGroundPlaneSimState->GetCollisionObject();
    co->PostManualUpdate();
    
    
    
    
    /*
    rmt::Matrix groundPlaneTransform;
    groundPlaneTransform.Identity();

    // TODO - optimize this call since we know what the heading is all the time   
    //rmt::Vector heading(0.0f, 0.0f, 1.0f);
    rmt::Vector up(0.0f, 0.0f, 1.0f);

    // note:
    //!!!
    //
    // new approach - use our n as heading

    //groundPlaneTransform.FillHeading(heading, n);  
    groundPlaneTransform.FillHeading(n, up);  

    groundPlaneTransform.FillTranslate(p);

    //PushSimState(bool inReset=true) = 0;  // better name for this would be Sync
    // ? mSimStateArticulated->SetControl(simAICtrl);

    mVehicle->mGroundPlaneSimState->SetTransform(groundPlaneTransform);


    // ? mSimStateArticulated->SetControl(simSimulationCtrl);  
    // TODO - need to do this?
    // double check with martin!!
    //mVehicle->mGroundPlaneSimState->GetCollisionObject()->Update(); - don't seem to need this?
    */
    
}

//------------------------------------------------------------------------
void PhysicsLocomotion::PreCollisionPrep(bool firstSubstep)
{

    BEGIN_PROFILE("MoveWheelsToBottomOfSuspension")
    MoveWheelsToBottomOfSuspension();
    END_PROFILE("MoveWheelsToBottomOfSuspension")

    // this is in substep, so comment out if we don't want it to happen
    if(firstSubstep)    // only do this once per update, but it must be inside the loop the first time
    {
    
         if(mVehicle->mSimStateArticulated->GetControl() != sim::simAICtrl)
         {
    
            BEGIN_PROFILE("FetchWheelTerrainCollisionInfo")
            FetchWheelTerrainCollisionInfo();
            END_PROFILE("FetchWheelTerrainCollisionInfo")
         }
    }
    else
    {
        int stophere = 1;
    }

    BEGIN_PROFILE("UpdateVehicleGroundPlane")
    UpdateVehicleGroundPlane();
    END_PROFILE("UpdateVehicleGroundPlane")

}


//=============================================================================
// PhysicsLocomotion::CompareNormalAndHeight
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& normalPointingAtCar, rmt::Vector& groundContactPoint)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::CompareNormalAndHeight(int index, rmt::Vector& normalPointingAtCar, rmt::Vector& groundContactPoint)
{

    // important!
    //
    // assume this is being called from the collision solver agent _after_ we've already fetched new wheel terrain collision info

    // so....
    // how to decide?

    // first try?
    // y value of contact points?
    rAssert(index >= 0 && index < 4);
    
    
    if(mFoundPlane[index])
    { 

        if(groundContactPoint.y > mTerrainIntersectCache[index].planePosn.y)
        {
        
            //mTerrainIntersectCache[index].planePosn = groundContactPoint;
            //mTerrainIntersectCache[index].planeNorm = normalPointingAtCar;
            
            // note this used to be in here??    
            mIntersectNormalUsed[index] = normalPointingAtCar;

        }
    }
    else
    {  
    
        // take the more uprigth normal
        if(GetWorldPhysicsManager()->mWorldUp.DotProduct(normalPointingAtCar) > 
        GetWorldPhysicsManager()->mWorldUp.DotProduct(mIntersectNormalUsed[index]) )
           
        {
            mIntersectNormalUsed[index] = normalPointingAtCar;
            
            
        }
        
    }
/*
  struct TerrainIntersectCache
    {
        rmt::Vector closestTriPosn;
        rmt::Vector closestTriNormal;
        rmt::Vector planePosn;
        rmt::Vector planeNorm;
    };

    TerrainIntersectCache mTerrainIntersectCache[4];    // is this wasting too much memory?

    // need this because if we're driving on a static collision volume the terrain normal could be way off!
    rmt::Vector mIntersectNormalUsed[4];
*/


}

//=============================================================================
// PhysicsLocomotion::PreSubstepUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  (Vehicle* vehicle)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::PreSubstepUpdate()
{
 
    //MoveWheelsToBottomOfSuspension();

    //FetchWheelTerrainCollisionInfo();

    //UpdateVehicleGroundPlane();

}


//------------------------------------------------------------------------
void PhysicsLocomotion::SetForceApplicationPoints()
{
    int i;
    for(i = 0; i < 4; i++)
    {
        // TODO - clean up
        // for force application points should also add in wheel - nope, tried that, not so good

        // TODO 
        // why do I have copies of thsi stuff here
        mForceApplicationPoints[i] = mVehicle->mSuspensionWorldSpacePoints[i];

        // try something new:

        if(mVehicle->mVehicleType == VT_USER && (mVehicle->mVehicleState == VS_SLIP || mVehicle->mVehicleState == VS_EBRAKE_SLIP))
        {

            rmt::Vector tireFix = mVehicle->mVehicleUp;


            // new test - decrease depending on % of top speed
            //tireFix.Scale(mVehicle->mWheels[i]->mRadius * -1.0f * (1.0f - (mVehicle->mPercentOfTopSpeed * 0.5f)));

            //tireFix.Scale(mVehicle->mWheels[i]->mRadius * -1.0f * (1.0f - (mVehicle->mPercentOfTopSpeed * 0.75f)));


            //tireFix.Scale(mVehicle->mWheels[i]->mRadius * -1.0f * (1.0f - (mVehicle->mPercentOfTopSpeed * mVehicle->mPercentOfTopSpeed)));
            tireFix.Scale(mVehicle->mWheels[i]->mRadius * mVehicle->mPercentOfTopSpeed);// * 0.5f);


            //mForceApplicationPoints[i].y -= mVehicle->mWheels[i]->mRadius;


            mForceApplicationPoints[i].Add(tireFix);
        }

        mSuspensionPointVelocities[i] = mVehicle->mSuspensionPointVelocities[i];        

    }

}


//------------------------------------------------------------------------
void PhysicsLocomotion::ApplySuspensionForces(float dt, bool atrest)
{ 

    int i;
    for(i = 0; i < 4; i++)
    {     
        //rmt::Vector force = mVehicle->mVehicleUp;
        //rmt::Vector force = mTerrainIntersectCache[i].planeNorm;
        rmt::Vector force;
        if(atrest)
        {
            force = mVehicle->mVehicleUp;
        }
        else
        {
            force = mIntersectNormalUsed[i];
        }
        
        //??
        /*
            // not helping
        if( GetWorldPhysicsManager()->mWorldUp.DotProduct(mVehicle->mVehicleUp) >
            GetWorldPhysicsManager()->mWorldUp.DotProduct(force) && mVehicle->mDoingJumpBoost)
            
        {
        
            force = mVehicle->mVehicleUp;
        }
        */
        
        // debug
        //float cos30 = 0.866f;
        //float cos40 = 0.766f;
        //if(GetWorldPhysicsManager()->mWorldUp.DotProduct(force) < cos30)
        

        float yVelocityAlongSuspensionAxis = force.DotProduct(mSuspensionPointVelocities[i]);

        //mCachedSuspensionForceResults[i] = mWheels[i]->CalculateSuspensionForce(mSuspensionPointVelocities[i].y, dt);       
        Wheel* wheel = mVehicle->mWheels[i];

        float forceToUse = 0.0f;
        mCachedSuspensionForceResults[i] = CalculateSuspensionForce(wheel, yVelocityAlongSuspensionAxis, dt, forceToUse);       
        
        
        force.Scale(forceToUse);

        ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->AddForce(force, &(mForceApplicationPoints[i]));
    }


}

//=============================================================================
// PhysicsLocomotion::CalculateSuspensionForce
//=============================================================================
// Description: Comment
//
// note - moving thsi functionality from Wheel class to here because wheel
// shouldn't do physics calculations, and this class shouldn't hold the
// info about wheels - like k and c
//
//
// Parameters:  (Wheel* wheel, float suspensionPointYVelocity, float dt)
//
// Return:      float 
//
//=============================================================================
float PhysicsLocomotion::CalculateSuspensionForce(Wheel* wheel, float suspensionPointYVelocity, float dt, float& forceToUse)
{

    // crazy shit goin' on here!
    // the value we return we'll cachce for slip calculations and stuff
    //
    // the value we stuff in forceToUse is the one to make the car bank.
    //
    // the one we cache should not include the quadSpringForce


    float force = 0.0f;


    rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
    float tip = mVehicle->mVehicleUp.DotProduct(up);

    float cos85 = 0.087f; 
    float cos80 = 0.17365f;
    float cos65 = 0.4226f;

    if(wheel->mWheelInCollision && tip > cos65) // minor TODO - should this number match any others?
    {
        // test
        //
        // make the spring also only push up
        
        //float springForce = wheel->mk * wheel->mYOffset;          
        float springForce = 0.0f;
        //if(wheel->mYOffset > 0.0f)
        
        if(!(mVehicle->mDoingJumpBoost && wheel->mYOffset < 0.0f))        
        {
            springForce = wheel->mk * wheel->mYOffset;          
        }
        
        
                  
        float quadSpringForce = 0.0f;    
        if(wheel->mYOffset > 0.0f)
        {
            quadSpringForce = wheel->mqk * wheel->mYOffset * wheel->mYOffset;
            //springForce += quadSpringForce;
        }


        //
        // or... topping out!
        //
        if((wheel->mLimit - rmt::Fabs(wheel->mYOffset)) < wheel->mLimit * 0.25f)        
        {            
            //springForce *= 3.0f;
        }

        // for now, only add damping if chassis is trying to compress suspension - ie. y velocity is down, -ve
        float damperForce = 0.0f;
       
        
        if(suspensionPointYVelocity > 0.0f)// this would make the dampe pull down
        {
            if((mVehicle->mDamperShouldNotPullDown[wheel->mWheelNum]) || mVehicle->mDoingJumpBoost)
            {
                // no suspension force down                
            }
            else
            {
                // regular
                damperForce = wheel->mc * -1.0f * suspensionPointYVelocity;
            }                      
            
        }
        else
        {
            // no worries
            damperForce = wheel->mc * -1.0f * suspensionPointYVelocity;
        }
           
           
 
        force = springForce + damperForce;
        forceToUse = springForce + damperForce + quadSpringForce;
    }
    else
    {
        // need to relax spring somehow
        
        // this is essentially useless....
        
        float relaxDistance = wheel->mLimit / wheel->mSpringRelaxRate * dt;
        if(wheel->mYOffset >= relaxDistance)
        {
            wheel->mYOffset -= relaxDistance;
        }
        else if(wheel->mYOffset <= -relaxDistance)
        {
            wheel->mYOffset += relaxDistance;
        }

    }

    return force;

}


//=============================================================================
// PhysicsLocomotion::ApplyDragForce
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::ApplyDragForce()
{
    if(mVehicle->mSpeed > 1.0f)
    {
        rmt::Vector force = mVehicle->mVelocityCM;

        // new test - 
        //force.y = 0.0f;

        force.NormalizeSafe();
    
        float mag = mVehicle->mSpeed * mVehicle->mSpeed * 0.5f * mVehicle->mDragCoeff;
    
        // new
        // car slows down too much when you let off gas
        if(mVehicle->mGas == 0.0f && mVehicle->mSpeedKmh > 50.0f && mVehicle->mBrake == 0.0f)
        {
            mag *= 0.25f;
        }
    
    
        force.Scale(-1.0f * mag);
    
        ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->AddForce(force);   
    }

}


//=============================================================================
// PhysicsLocomotion::LowSpeedTest
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::LowSpeedTest()
{

    //static float test = 0.8f;
    static float test = 0.8f;

    static float linearWhittle = 0.6f;
    static float angularWhittle = 0.1f;

    //const float lowspeed = 1.0f;
    //static float lowspeed = 2.0f;
    float lowspeed = 2.0f;
    float reallylowspeed = 0.05f;

    // only apply if at least 2 wheels in contact with the ground:

    int i;
    int count = 0;    
    for(i = 0; i < 4; i++)
    {
        if(mVehicle->mWheels[i]->mWheelInCollision)
        {
            count++;
        }
    }
    
    if(count > 1)
    {

        if(mVehicle->mSpeed < lowspeed && mVehicle->mGas < 0.1f /*&& mVehicle->mReverse < 0.1f*/ && mVehicle->mBrake < 0.1f)  // new... brake
        {
            // whittle away speed, or just reset?
            //mVehicle->mSimStateArticulated->ResetVelocities();
            // I knew this was a bad idea
            
            if(mVehicle->mSpeed < reallylowspeed)
            {
                mVehicle->mSimStateArticulated->ResetVelocities();
            }
            else
            {

                rmt::Vector& linearVel = mVehicle->mSimStateArticulated->GetLinearVelocity();
                rmt::Vector& angularVel = mVehicle->mSimStateArticulated->GetAngularVelocity();

                linearVel.Scale(linearWhittle);
                angularVel.Scale(angularWhittle);
            }
        }
    }
    

}



//=============================================================================
// PhysicsLocomotion::ApplyAngularDrag
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::ApplyAngularDrag()
{
    if(mVehicle->mPercentOfTopSpeed > 0.01f)
    {
        const float magicshit = 3.0f;

        rmt::Vector angularDragForce = mVehicle->mSimStateArticulated->GetAngularVelocity();
        angularDragForce.Scale(-1.0f * magicshit * mVehicle->mDesignerParams.mDpMass);
        
        sim::PhysicsObject* phobj = (sim::PhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject());
        rAssert(phobj);
        
        phobj->AddTorque(angularDragForce);   
    }    

}

//=============================================================================
// PhysicsLocomotion::ApplyRollingFriction
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::ApplyRollingFriction()
{
    // to start with simplest possible model - constant?
    //static float test = 1.0f;
    //static float test2 = 2000.0f;

    //static float test = 0.9f;
    //static float test = 0.8f;
    static float test = 0.8f;

    //const float lowspeed = 1.0f;
    //static float lowspeed = 2.0f;
    static float lowspeed = 2.0f;
    static float reallylowspeed = 0.05f;

    static float torquemod = 0.001f;

    // only apply if at least 2 wheels in contact with the ground:

    int i;
    int count = 0;    
    for(i = 0; i < 4; i++)
    {
        if(mVehicle->mWheels[i]->mWheelInCollision)
        {
            count++;
        }
    }
    
    if(count > 1)
    {

        if(mVehicle->mSpeed > reallylowspeed && mVehicle->mGas == 0.0f)
        {
            rmt::Vector force = mVehicle->mVelocityCM;
            force.Normalize();
    
            force.Scale(-1.0f * mVehicle->mRollingFrictionForce * mVehicle->mDesignerParams.mDpMass);

            ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->AddForce(force);     
            

        }

        else if(0)//mVehicle->mGas < 0.1f && mVehicle->mReverse < 0.1f && mVehicle->mBrake < 0.1f)  // new... brake
        {
            // whittle away speed, or just reset?
            //mVehicle->mSimStateArticulated->ResetVelocities();
            // I knew this was a bad idea
            
            //if(1)//mVehicle->mSpeed < reallylowspeed)
            /*
            if(mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->IsAtRest())
            {

                mVehicle->mSimStateArticulated->ResetVelocities();
                mVehicle->mSimStateArticulated->SetControl(sim::simAICtrl);

                //mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->ResetAppliedForces();

            }
            else
            {
                //mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->ResetAppliedForces();

                
                //rmt::Vector& linearVel = mVehicle->mSimStateArticulated->GetLinearVelocity();
                //rmt::Vector& angularVel = mVehicle->mSimStateArticulated->GetAngularVelocity();

                //linearVel.Scale(test);
                //angularVel.Scale(test);
            }
            */

        }
    }

}


//=============================================================================
// PhysicsLocomotion::HighSpeedInstabilityTest
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::HighSpeedInstabilityTest()
{
    const float threshold = 0.5f;
    
    // from threshold to 100%, increase y cm offset
    
    if(mVehicle->mPercentOfTopSpeed > threshold)
    {
        // should do ...
        if(mVehicle->mInstabilityOffsetOn)
        {
            // already set
            //
            // nothing to do here
        }
        else
        {
            // set it

            rmt::Vector unstableAdjust = mVehicle->mCMOffset;

            //static float weeblemagic = 1.0f;
            /*static*/ float magic = 1.0f;

            // scale by point in range
            // set range of 0 to 1 for start to limit
            /*
                float range = 1.0f - ((tip - end) / (start - end));

                if(range < 0.0f)
                {
                    range = 0.0f;
                    //rAssert(0);
                }
                if(range > 1.0f)
                {
                    range = 1.0f;
                }

                float modifier = weeblemagic * range;
            */
            //weeble.y -= modifier;

            unstableAdjust.y += magic;
            
            //weeble.y -= 3.0f;

            ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(unstableAdjust);

            mVehicle->mInstabilityOffsetOn = true;
        }    
    }
    else
    {
        // if it's on, shut it off
        if(mVehicle->mInstabilityOffsetOn)
        {
            ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(mVehicle->mCMOffset);
        
            mVehicle->mInstabilityOffsetOn = false;            
        }
    }

}


//=============================================================================
// PhysicsLocomotion::Weeble
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::Weeble()
{
    bool doit = false;
    
    // only do this for sideways tipping

    rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
    float tip = mVehicle->mVehicleUp.DotProduct(up);

    float cos35 = 0.8192f; 
    float cos45 = 0.7071f; 
    float cos55 = 0.5736f;
    float cos40 = 0.766f;

    float start = cos40;
    float end = cos55;


    // new test - 
    // if we are completely airborn - set cmoffset to 0,0,0
    
    //if(mVehicle->mAirBorn)
    //{
    //    if(!(mVehicle->mCMOffsetSetToOriginal))
    //    {
    //        //mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(mVehicle->mCMOffset);
    //        mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(mVehicle->mOriginalCMOffset);
    //        mVehicle->mCMOffsetSetToOriginal = true;
    //    }
    //}
    //else
    {
        /*
        if(mVehicle->mCMOffsetSetToOriginal)
        {
            //mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(mVehicle->mOriginalCMOffset);
            mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(mVehicle->mCMOffset);
            mVehicle->mCMOffsetSetToOriginal = false;
        }
        */

        //if(!mVehicle->mAirBorn && tip < start)
        if(1)//tip < start)
        {      
            bool slowdowntipping = false;
            if( !(mVehicle->mWheels[0]->mWheelInCollision) && !(mVehicle->mWheels[3]->mWheelInCollision) )
            {
                doit = true;
                
                if(mVehicle->mWheels[1]->mWheelInCollision && mVehicle->mWheels[2]->mWheelInCollision)
                {
                    slowdowntipping = true;         
                }
            }
        
        
            if( !(mVehicle->mWheels[1]->mWheelInCollision) && !(mVehicle->mWheels[2]->mWheelInCollision) )
            {
                doit = true;
                
                if(mVehicle->mWheels[0]->mWheelInCollision && mVehicle->mWheels[3]->mWheelInCollision)
                {
                    slowdowntipping = true;         
                }
            }
            
            if(0)//(slowdowntipping)           
            {
                // only resist tipping UP!
                
            
            
                //static float magicshit = 3.0f;
                const float magicshit = 2.5f;
                
                float facingAngVel = (mVehicle->mSimStateArticulated->GetAngularVelocity()).DotProduct(mVehicle->mVehicleFacing);
                
                // a positive value means tipping left.
                //
                // so.......
                // we want to apply the torque if the transverse.dot.worldup is positive
                
                // or... if tipping right and transverse.dot.worldup is negative
                
                if( (facingAngVel > 0.0f && mVehicle->mVehicleTransverse.DotProduct(up) > 0.0f) ||
                    (facingAngVel < 0.0f && mVehicle->mVehicleTransverse.DotProduct(up) < 0.0f) )
                {                
                    rmt::Vector torque = mVehicle->mVehicleFacing;
                    
                    
                    torque.Scale(-1.0f * magicshit * mVehicle->mDesignerParams.mDpMass * facingAngVel);    
          
                    sim::PhysicsObject* phobj = (sim::PhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject());
                    rAssert(phobj);
                        
                    phobj->AddTorque(torque);                   
                }        
            }
            
        }
        if(mVehicle->mAirBorn)
        {
            doit = true;
        }


        if(doit)
        {
            
            if(!(mVehicle->mWeebleOn))
            {

                // weeble
                rmt::Vector weeble = mVehicle->mCMOffset;

                //static float weeblemagic = 1.0f;
                float weeblemagic = 1.0f;

                // scale by point in range
                // set range of 0 to 1 for start to limit
                /*
                    float range = 1.0f - ((tip - end) / (start - end));

                    if(range < 0.0f)
                    {
                        range = 0.0f;
                        //rAssert(0);
                    }
                    if(range > 1.0f)
                    {
                        range = 1.0f;
                    }

                    float modifier = weeblemagic * range;
                */
                //weeble.y -= modifier;

                //weeble.y -= weeblemagic;
                weeble.y += mVehicle->mDesignerParams.mDpWeebleOffset;

                //weeble.y -= 3.0f;

                rmt::Vector current = ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->GetExternalCMOffset();
            
                if(current.Equals(weeble))
                {
                    int stophere = 1;
                }
                else
                {
                    ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(weeble);
                }

                mVehicle->mWeebleOn = true;
            }
            
        }
        else
        {
            if(mVehicle->mWeebleOn)
            {
                ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(mVehicle->mCMOffset);
            }
            mVehicle->mWeebleOn = false;
        }

    }

}


//=============================================================================
// PhysicsLocomotion::PreUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::PreUpdate()
{
    //mVehicle->CalculateSuspensionLocationAndVelocity();

}


//=============================================================================
// PhysicsLocomotion::Update
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::Update(float dt)
{


    // recall 
    // this is the set of function calls to physicsvehicle from vehicle in the old 
    // system

    UseWheelTerrainCollisionInfo(dt);

    CorrectWheelYPositions();   // old name: PreUpdate(dt);   

    mVehicle->CalculateSuspensionLocationAndVelocity();



    //mVehicle->mSimStateArticulated->SetControl(sim::simSimulationCtrl); -- move this into SelfRestTest


    //bool rest = mVehicle->SelfRestTest();


    bool rest = false;
    rest = mVehicle->SelfRestTest();


    if(mVehicle->mSimStateArticulated->GetControl() != sim::simAICtrl)
    {


            // TODO - this call, and UpdateJointState, should be in the same place
        //    mVehicle->mSimStateArticulated->StoreJointState(dt);  
    

        // the update guts from physicsvehicle
        //
        // kind of a fucking mess
        //mVehicle->CalculateSuspensionLocationAndVelocity();
        
        SetForceApplicationPoints();
 

        ApplySuspensionForces(dt, rest);
    
        if(!(mVehicle->mAirBorn) && !rest)
        {
            ApplyControllerForces2(dt);
        }
        else
        {
            // at least do this
            mVehicle->mBurnoutLevel = 0.0f;

        }

        if(GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_NO_TOP_SPEED) && mVehicle->mVehicleType == VT_USER)
        {
            // no drag for you!
        }
        else
        {
            if(!(mVehicle->mSteeringWheelsOutOfContact) && !(mVehicle->mDoSpeedBurst) && !(mVehicle->mDoingJumpBoost))
            {
                ApplyDragForce();
            }
        }
        
        if(!(mVehicle->mDoingJumpBoost))
        {
            ApplyRollingFriction(); // move low speed portion of this into a separate function to follow update...
        }
    
            //TipTest();
    
        if(mVehicle->mAirBorn && mVehicle->mVehicleType == VT_USER )
        {
            ApplyAngularDrag();
        }

        Weeble();
        
        if(mVehicle->mVehicleType == VT_USER)
        {
            //HighSpeedInstabilityTest();
        }

        if(mVehicle->mVehicleType == VT_USER)
        {
            StuckOnSideTest(dt);
        }


        if(mVehicle->mAirBorn)// && mVehicle->mVehicleType == VT_USER)
        {
            DurangoStyleStabilizer(dt);
        }



        //
        // ****
        // 
        // THE Update
        //
        // ****

        ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->Update(dt);

            //LowSpeedTest();
            
        //rest = mVehicle->SelfRestTest();
    }

    

    //Update(dt);    
    //PostUpdate(dt);

    // maybe for first pass implementation, implement methods with the same name?


    // ???????
    //
    // do the pose driver updates here!!!! or back in vehicle <- for some reason
    // the second way seems much nicer to me.


    
    // the net effect on Vehicle result when this function returns is to have 
    // modified the joint-space y values in the wheel joints to correct 
    // the wheel positions, and to have applied forces and torques to the
    // simstate and had it update itself so there is a new world space transform
    // AND make sure the world space transform of joint 0 is in sync with this

    // also - make sure wheels have enough info for suspensionjointdriver to 
    // do it's thing
    //
    // ie - cumulative rot
    // wheel turn angle
    // that's about it?

    // are we gonna have to make physicslocomotion a friend of the Vehicle?
    // is that the correct thing to do - make all the locomotions friends??
    


}



//=============================================================================
// PhysicsLocomotion::DurangoStyleStabilizer
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::DurangoStyleStabilizer(float dt)
{
    // assume airborn
    
    rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
    float tip = mVehicle->mVehicleUp.DotProduct(up);

    // we want a fix torque proportional to 1.0 - tip.
    
    // 'direction' of the toruqe is vehicleup crossed into world up
    float cos10 = 0.9848f;
    if(tip < cos10)
    {
        
        
        rmt::Vector fixTorque;
        fixTorque.CrossProduct(mVehicle->mVehicleUp, up);
     
        // need this?   
        fixTorque.NormalizeSafe();
        
        const float hackmagicshit = 200.0f;
        
        float fixscale = 1.0f - tip;
        
        fixTorque.Scale(hackmagicshit * mVehicle->mDesignerParams.mDpMass * fixscale);
        
        sim::PhysicsObject* phobj = (sim::PhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject());
        rAssert(phobj);
            
        phobj->AddTorque(fixTorque);                   
        
    
        // try some damping too    
        const float dampingShit = 7.0f;

        rmt::Vector angularDragForce = mVehicle->mSimStateArticulated->GetAngularVelocity();
        angularDragForce.Scale(-1.0f * dampingShit * mVehicle->mDesignerParams.mDpMass);
        
        phobj->AddTorque(angularDragForce);   
        

    }

}

//=============================================================================
// PhysicsLocomotion::StuckOnSideTest
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::StuckOnSideTest(float dt)
{
    // only for user vehicle in physics locomotion

    const float lowspeed = 1.0f;
    if(mVehicle->mSpeed < lowspeed)
    {
    
        rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
        float tip = mVehicle->mVehicleUp.DotProduct(up);

        //float cos85 = 0.0872f;
        float test = 0.2f;
        if(tip < test)
        {            
            //static float magicshit = 1.0f;
            const float magicshit = 130.0f;
        
            float side = up.DotProduct(mVehicle->mVehicleTransverse);
            if(side > 0.0f)
            {
                // lying on it's left side - right side pointint to sky
                //if(mVehicle->mUnmodifiedInputWheelTurnAngle > 0.0f)
                if(mVehicle->mUnmodifiedInputWheelTurnAngle < 0.0f)
                {
                    // apply some torque
                    rmt::Vector torque = mVehicle->mVehicleFacing;
                    //torque.Scale(mVehicle->mUnmodifiedInputWheelTurnAngle * -1.0f * mVehicle->mDesignerParams.mDpMass * magicshit);                                                      
                    torque.Scale(mVehicle->mUnmodifiedInputWheelTurnAngle * 1.0f * mVehicle->mDesignerParams.mDpMass * magicshit);                                                      
                    
                    sim::PhysicsObject* phobj = (sim::PhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject());
                    rAssert(phobj);
                        
                    phobj->AddTorque(torque);                   
          
                }
        
            
            }
            else
            {
                // lying on it's right side - left side pointint to sky
                //if(mVehicle->mUnmodifiedInputWheelTurnAngle < 0.0f)
                if(mVehicle->mUnmodifiedInputWheelTurnAngle > 0.0f)
                {
                    // apply some torque
                    rmt::Vector torque = mVehicle->mVehicleFacing;
                    //torque.Scale(mVehicle->mUnmodifiedInputWheelTurnAngle * -1.0f * mVehicle->mDesignerParams.mDpMass * magicshit);                   
                    torque.Scale(mVehicle->mUnmodifiedInputWheelTurnAngle * 1.0f * mVehicle->mDesignerParams.mDpMass * magicshit);                   
                    
                    sim::PhysicsObject* phobj = (sim::PhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject());
                    rAssert(phobj);
                        
                    phobj->AddTorque(torque);                   
          
                }
        
                
            }
        
        }
        
    }
    
}

//=============================================================================
// PhysicsLocomotion::TipTest
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::TipTest()
{
    

    rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
    float tip = mVehicle->mVehicleUp.DotProduct(up);

    float cos45 = 0.7071f;
    float cos80 = 0.17365f;
    float cos65 = 0.4226f;
    // need to figure out if the angular velocity is trying to tip us over more, or right us

    float start = cos45;
    float end = cos65;

    //static float tipRecoverPercentage = 75.0f;
    float tipRecoverPercentage = 75.0f;
    
    if(tip < start && tip > end)
    {
        // set range of 0 to 1 for start to limit
        float range = 1.0f - ((tip - end) / (start - end));


        float lean = mVehicle->mVehicleTransverse.DotProduct(up);

        if(lean > cos45)
        {
            // tipping left
        
            rmt::Vector& angularVel = mVehicle->mSimStateArticulated->GetAngularVelocity();

            float proj = angularVel.DotProduct(mVehicle->mVehicleFacing);
            
            //if(proj > 0.0f)
            if(proj > 1.0f)
            {
                // the velocity is trying to tip us more

                //rmt::Vector recover = mVehicle->mVehicleFacing;
                //recover.Scale(-1.0f * proj * tipRecoverPercentage);

                //angularVel.Add(recover);

                //angularVel.Scale(tipRecoverPercentage);
                //angularVel.Scale(1.0f - range);
                angularVel.Scale(0.0f);

                //rmt::Vector recover = mVehicle->mVehicleFacing;
                //recover.Scale(-1.0f * proj * tipRecoverPercentage * mVehicle->mDesignerParams.mDpMass * range);

                //mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->AddTorque(recover);
                    


            }
        }
        else if(lean < -cos45)
        {
        
            //rmt::Vector& linearVel = mVehicle->mSimStateArticulated->GetLinearVelocity();
            rmt::Vector& angularVel = mVehicle->mSimStateArticulated->GetAngularVelocity();

            float proj = angularVel.DotProduct(mVehicle->mVehicleFacing);


            // tipping over right
            //if(proj < 0.0f)
            if(proj < -1.0f)
            {

                //rmt::Vector recover = mVehicle->mVehicleFacing;
                //recover.Scale(-1.0f * proj * tipRecoverPercentage);

                //angularVel.Add(recover);

                //angularVel.Scale(tipRecoverPercentage);
                //angularVel.Scale(1.0f - range);
                angularVel.Scale(0.0f);
                
                //rmt::Vector recover = mVehicle->mVehicleFacing;
                //recover.Scale(-1.0f * proj * tipRecoverPercentage * mVehicle->mDesignerParams.mDpMass * range);

                //mVehicle->((ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1)))->AddTorque(recover);

            }

        }

        // else don't do shit






        // what is our tendency to hold and recover from tipping?


    }

}

//=============================================================================
// PhysicsLocomotion::PostUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::PostUpdate()
{
    // this whole method is kind of annyoing
    //
    // TODO
    //??? shoudl be in Vehicle

    mVehicle->mTransform = mVehicle->mSimStateArticulated->GetTransform(-1);


    mVehicle->mVelocityCM = mVehicle->mSimStateArticulated->GetLinearVelocity();
    mVehicle->mSpeed = mVehicle->mVelocityCM.Magnitude();
    mVehicle->mPercentOfTopSpeed = mVehicle->mSpeed / (mVehicle->mDesignerParams.mDpTopSpeedKmh / 3.6f);
    if(mVehicle->mPercentOfTopSpeed > 1.0f)
    {
        mVehicle->mPercentOfTopSpeed = 1.0f;
    }

    // Roll up the terrain type and interior flag;
    // As soon as we get two tires or more of a terrain type, that's the type we'll consider the car on.
    // This isn't very exact since the rear two tires will have precidence over the other tires.
    int terrainCounts = 0;
    int interiorCount = 0;
    eTerrainType newType = TT_Road;
    for( int i = 0; i < 4; i++ )
    {
        int index = i ^ 2; // We do this to change i from 0, 1, 2, 3 into 2, 3, 0, 1 so that the rear wheels are processed last.
        interiorCount += mTerrainIntersectCache[ index ].mInteriorTerrain ? 1 : 0;
//? Do we want to mask out roads?        int mask = ( 1 << (int)mTerrainIntersectCache[ index ].mTerrainType ) & ~(int)TT_Road; // Mask out roads so everything else takes precidence over it.
        int mask = 1 << (int)mTerrainIntersectCache[ index ].mTerrainType;
        if( ( terrainCounts & mask ) )
        {
            // We've already got one, so this is number two.
            newType = mTerrainIntersectCache[ index ].mTerrainType;
        }
        else
        {
            terrainCounts |= mask;
        }
    }
    mVehicle->mTerrainType = newType;
    mVehicle->mInterior = ( interiorCount >= 2 );

}

//------------------------------------------------------------------------
void PhysicsLocomotion::CorrectWheelYPositions()
{


    poser::Pose* pose = mVehicle->mPoseEngine->GetPose();


    int i;
    for(i = 0; i < 4; i++)
    {    
        // we don't want yOffset, we want the correction value!!
        
        // now just use mYOffset
        //float yOffset = mWheels[i]->mYOffset; fuck no!!!
        Wheel* wheel = mVehicle->mWheels[i];        

        float yCorrectionValue = wheel->GetYCorrectionValue();
    
        poser::Joint* joint = pose->GetJoint(mVehicle->mWheelToJointIndexMapping[i]);
        rmt::Vector trans = joint->GetObjectTranslation();

        // these trans.y should still be at the bottom of their suspension range
        
        // new test so wheels don't hang so low
        if(wheel->mWheelInCollision)
        {
            trans.y += yCorrectionValue;
        }
        else if(mVehicle->mAirBorn) // add this here to remove the snap/bounce when turning hard and two wheels come out of contact
        {
            trans.y += wheel->mLimit;
        }

        // test
        //trans.y += mVehicle->mDesignerParams.mDpSuspensionYOffset;
    
        joint->SetObjectTranslation(trans);

        wheel->ResolveOffset();

    }


}


//------------------------------------------------------------------------
// temp...
//------------------------------------------------------------------------
inline void TempGetTerrainIntersects(   rmt::Vector& trans,
                                        float radius,
                                        bool& foundtri,
                                        rmt::Vector& closestTriNormal,
                                        rmt::Vector& closestTriPosn,
                                        bool& foundplane,
                                        rmt::Vector& planeNormal,
                                        rmt::Vector& planePosn)
{
    const float y = 3.0f;


    foundtri = false;
    foundplane = true;
    planeNormal.Set(0.0f, 1.0f, 0.0f);
    planePosn.x = trans.x;
    planePosn.y = y;
    planePosn.z = trans.z;


}

//=============================================================================
// PhysicsLocomotion::FetchWheelTerrainCollisionInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::FetchWheelTerrainCollisionInfo()
{
    poser::Pose* pose = mVehicle->mPoseEngine->GetPose();

    int i;
    for(i = 0; i < 4; i++)
    {
        BEGIN_PROFILE("PreInter") 
        Wheel* wheel = mVehicle->mWheels[i];

        // for each wheel, based on the world xz, get a y and a normal, ..... and a ground type

        //? could also do based on suspension points, no?
        // I guess this is more accurate

        poser::Joint* joint = pose->GetJoint(mVehicle->mWheelToJointIndexMapping[i]);
        rmt::Vector trans = joint->GetWorldTranslation();


        rmt::Vector closestTriNormal, closestTriPosn;
        rmt::Vector planeNormal, planePosn;
        int terrainType;
        //bool bFoundTri, bFoundPlane;
        
        mFoundTri[i] = false;
        mFoundPlane[i] = false;
        END_PROFILE("PreInter") 

        BEGIN_PROFILE("GetIntersectManager()->FindIntersection") 
        
        terrainType = GetIntersectManager()->FindIntersection( trans, 
                                                 mFoundPlane[i],
                                                 planeNormal,
                                                 planePosn );
        
        
        /*
        TempGetTerrainIntersects(   trans, 
                                    wheel->mRadius, 
                                    mFoundTri[i],
                                    closestTriNormal,
                                    closestTriPosn, 
                                    mFoundPlane[i],
                                    planeNormal,
                                    planePosn);
        */
                                 
        END_PROFILE("GetIntersectManager()->FindIntersection") 
        
            
        BEGIN_PROFILE("PostInter") 
        // fill in pieces of cache 
        if(mFoundPlane[i])
        {
        
            rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
            if(up.DotProduct(planeNormal) < 0.0f)
            {
                //rAssert(0);
            }
            
        
            mTerrainIntersectCache[i].planePosn = planePosn;
            mTerrainIntersectCache[i].planeNorm = planeNormal;
            
            mIntersectNormalUsed[i] = mTerrainIntersectCache[i].planeNorm;
        }
        else
        {
            // no plane
            //rAssert(0);        
        }
        
                
        

        //Devin says this is not used.  Cleaning up the warnings.
//        if(mFoundTri[i])
//        {
//            mTerrainIntersectCache[i].closestTriPosn = closestTriPosn;
//            mTerrainIntersectCache[i].closestTriNormal = closestTriNormal;            
//        }
        
        mTerrainIntersectCache[ i ].mTerrainType = (eTerrainType)( terrainType & ~0x80 );
        mTerrainIntersectCache[ i ].mInteriorTerrain = ( terrainType & 0x80 ) == 0x80;

        // stuff in value for later comparison
        //mIntersectNormalUsed[i] = mTerrainIntersectCache[i].planeNorm;
        END_PROFILE("PostInter") 
    }

}


//=============================================================================
// PhysicsLocomotion::UseWheelTerrainCollisionInfo
//=============================================================================
// Description: Comment
//
// per-wheel, calculate and call Wheel::SetYOffsetFromCurrentPosition
// 
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::UseWheelTerrainCollisionInfo(float dt)
{

    poser::Pose* pose = mVehicle->mPoseEngine->GetPose();

    // take the highest value for this from all 4 wheels
    float bottomOutFix = 0.0f;  

    int i;
    for(i = 0; i < 4; i++)
    {
    
        // make copies 'cause we may want to modify local result, but not cached value
        bool foundPlane = mFoundPlane[i];
        bool foundTri = mFoundTri[i];

        float fixHeight = 0.0f;
        bool thisWheelInCollision = false;
        rmt::Vector fixHeightNormal(0.0f, 1.0f, 0.0f);

        float fixAlongSuspensionAxis = 0.0f;
        
        Wheel* wheel = mVehicle->mWheels[i];
        poser::Joint* joint = pose->GetJoint(mVehicle->mWheelToJointIndexMapping[i]);
        rmt::Vector trans = joint->GetWorldTranslation();      


        if(!mFoundPlane[i] && !mFoundTri[i])
        {
            // we didn't get any good data back, so we need
            // to use last cached value
            //rAssert(0); // curious to see if we hit this - yes, a lot

            // just use plane y
            foundPlane = true;                        
        }

        if(foundPlane)
        {
            float y = mTerrainIntersectCache[i].planePosn.y;

            // first check
            // the pathologically bad case:
            if(mVehicle->mSuspensionWorldSpacePoints[i].y < y)
            {
                // this means the suspension rest point on the car is below the terrain
                // this is fucking bad.

                // TODO: take this out?
                //rAssert(0);

                // ?? actually need to do anything here?

                // TODO - bump car up?
                        
            }                        

            //else

            {
                // hopefully sane case

                // in planePosn, we only really care about the y
            
                float penetratingDepth = y - (trans.y - wheel->mRadius);

                if(penetratingDepth > 0.0f)
                {
                    thisWheelInCollision = true;
                    fixHeight = penetratingDepth;
            
                    // fixHeight is going pure up
                    fixHeightNormal.Set(0.0f, 1.0f, 0.0f);

                    fixAlongSuspensionAxis = fixHeight / rmt::Fabs( (mVehicle->mVehicleUp).DotProduct(fixHeightNormal));

                }
            }

        }


        // TODO! revisit this!

        if(0)//foundTri)
        {
            // calculate fixAlongSuspensionAxis based on this also, and if it's greater than above, use it instead
            
            rmt::Vector penetrationVector;
            //penetrationVector.Sub(trans, mWheelTerrainCollisionPoints[i]);

            penetrationVector = trans;
            penetrationVector.Sub(mTerrainIntersectCache[i].closestTriPosn);

            // TODO - is this safe?
            float penMag = penetrationVector.Magnitude();

            bool checkBump = false;

            if(trans.y < mTerrainIntersectCache[i].closestTriPosn.y)
            {
                // definately need to fix!
                fixHeight = wheel->mRadius + penMag;
                thisWheelInCollision  = true;              
                fixHeightNormal = mTerrainIntersectCache[i].closestTriNormal;

                float tempFixAlongSuspensionAxis = fixHeight / rmt::Fabs( (mVehicle->mVehicleUp).DotProduct(fixHeightNormal));
                
                if(tempFixAlongSuspensionAxis > fixAlongSuspensionAxis)
                {
                    fixAlongSuspensionAxis = tempFixAlongSuspensionAxis;
                }

                checkBump = true;

            }
            else if(penMag < wheel->mRadius)    // normal case
            {            
                fixHeight = wheel->mRadius - penMag;
                thisWheelInCollision = true;
                fixHeightNormal = mTerrainIntersectCache[i].closestTriNormal;

                float tempFixAlongSuspensionAxis = fixHeight / rmt::Fabs( (mVehicle->mVehicleUp).DotProduct(fixHeightNormal));
                
                if(tempFixAlongSuspensionAxis > fixAlongSuspensionAxis)
                {
                    fixAlongSuspensionAxis = tempFixAlongSuspensionAxis;
                }

                checkBump = true;

            }           

            if(checkBump)
            {
                     
                // if this is too horizontal, we need to deal with it specially
                float sin10 = 0.1736f;
                float sin20 = 0.342f;
                float sin30 = 0.5f;

                // TODO - what angle?
                if(rmt::Fabs(fixHeightNormal.DotProduct(mVehicle->mVehicleUp)) < sin20)// && fixHeight > wheel->mRadius)
                {
                    // just bump the whole car back by penMag?

                    rmt::Vector bump = fixHeightNormal;
                    bump.NormalizeSafe();
                    bump.Scale(fixHeight);

                    // TODO - true or false or tapered down or what?
                    //mVehicle->mSimStateArticulated->DynamicPositionAdjustment(bump, dt, true);
                    //mVehicle->mSimStateArticulated->DynamicPositionAdjustment(bump, dt, false);
                               
                    mVehicle->mBottomedOutThisFrame = true;

                    continue;
            

                }          
            }    
        }


        if(thisWheelInCollision)
        {  
          
            // perhaps this method should be renamed 
            // it sets the wheelInCollision flag to true!
            float bottomedOut = wheel->SetYOffsetFromCurrentPosition(fixAlongSuspensionAxis);

            if(bottomedOut > bottomOutFix)
            {
                bottomOutFix = bottomedOut;
                wheel->mWheelBottomedOutThisFrame = true;   // not sure if we'll use yet.

                //char buffy[128];
                //sprintf(buffy, "wheel %d bottomed out this frame\n", i);
                //rDebugPrintf(buffy);
            }

        }        

    } // end for(i


    if(bottomOutFix > 0.0f)
    {

        // this is the amount the suspension couldn't deal with.
        //
        // what to do?
        // in srr1 we just bumped the whole chassis up by this amount - seemed to work fairly well actually.
        //rmt::Vector inDeltaPos = mVehicle->mVehicleUp;

        // Note!
        // we hav to be careful how to deal with this 'cause we can get some weird ass results that make
        // the car go flying after seemingly minor crashes

        // hack, but might work well - only correct the vehicle straight up!            

        //rmt::Vector inDeltaPos = fixHeightNormal;

        rmt::Vector inDeltaPos;                

        inDeltaPos = GetWorldPhysicsManager()->mWorldUp;


        // sanity test?
        if(bottomOutFix > (mVehicle->mWheels[0]->mRadius * 0.5f))
        {
            bottomOutFix = (mVehicle->mWheels[0]->mRadius * 0.5f);
        }
    
    


        if(inDeltaPos.y > 0.0f) // this test is pointless right now
        {

            inDeltaPos.Scale(bottomOutFix);
   
            // TODO - revisit this
            
            
            
///you are here
//take this out - see what f1 car does
           
// no this was not the problem.

// get kevin to raise the bv

            mVehicle->mSimStateArticulated->DynamicPositionAdjustment(inDeltaPos, dt, false);



            // ?
            // how 'bout just applying some impulse to the sucker?
    
    /*
            if(0)//mVehicle->mOkToCrashLand)
            {
                rmt::Matrix groundPlaneMatrix = mVehicle->mGroundPlaneSimState->GetTransform(-1);      
                rmt::Vector groundPlaneUp = groundPlaneMatrix.Row(2);   // recall - ground plane is oriented weird

                rmt::Vector& linearVel = mVehicle->mSimStateArticulated->GetLinearVelocity();

                static float test = 10.0f;            
                groundPlaneUp.Scale(test);

                linearVel.Add(groundPlaneUp);
        
                mVehicle->mOkToCrashLand = false;

            }
    */

        }   
                    
        mVehicle->mBottomedOutThisFrame = true;
    }



}



