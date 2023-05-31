//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        physicslocomotioncontrollerforces.cpp
//
// Description: moved some methods into a different cpp file 'cause it was getting 
//              too big
//
// History:     May 13, 2001 + Created -- gmayer
//
//=============================================================================


//========================================
// System Includes
//========================================

#include <simcommon/simstate.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simcommon/simulatedobject.hpp>
#include <simphysics/articulatedphysicsobject.hpp>
#include <simphysics/physicsobject.hpp>
#include <simcommon/simenvironment.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>

#include <raddebug.hpp>

#include <gameflow/gameflow.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/avatarmanager.h>


#include <cheats/cheatinputsystem.h>

#include <poser/pose.hpp>
#include <poser/poseengine.hpp>
#include <poser/posedriver.hpp>


//========================================
// Project Includes
//========================================

#include <worldsim/redbrick/physicslocomotion.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/wheel.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#ifdef WORKING_ON_GREG_PHIZTEST_RIGHT_NOW
#include "../../../../users/greg/phiztest/code/worldsim/redbrick/phizsim.h" // just for a temp hack using world up
#else
#include <worldsim/worldphysicsmanager.h>
#endif

#include <render/IntersectManager/IntersectManager.h>

using namespace sim;


// test of terrain friction
                                // enum value
const float TF_Road = 1.0f;     // 0
const float TF_Grass = 0.7f;    // 1
const float TF_Sand = 0.6f;     // 2
const float TF_Gravel = 0.6f;   // 3
const float TF_Water = 0.6f;    // 4
const float TF_Wood = 1.0f;     // 5
const float TF_Metal = 1.0f;    // 6
const float TF_Dirt = 0.6f;     // 7






//=============================================================================
// PhysicsLocomotion::ApplyControllerForces2
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::ApplyControllerForces2(float dt)
{
    // for each wheel, calculate the total requested force, and then 
    // chop it based on the force circle?

    mVehicle->mBurnoutLevel = 0.0f;

    int i;
    rmt::Vector totalForce[4];
    for(i = 0; i < 4; i++)
    {
        totalForce[i].Clear();


        if(/*mVehicle->mWheels[i]->mWheelInCollision &&*/ !(mVehicle->mWheels[i]->mWheelBottomedOutThisFrame))
        {

            // TODO
            // only do loop if wheel in collision?
            // else reset some shit like the wheel state

            // TODO
            // how about wheel bottomed out this frame?
            // only affects steering? or all?

            //rmt::Vector totalForce(0.0f, 0.0f, 0.0f);         

            GasForce2(totalForce[i], i, dt);
            // TODO - make sure you set skid level and renderspinuprate and shit like that.

            BrakeForce2(totalForce[i], i, dt);
            // TODO - somewhere else add code to bring car to a standstill

            ReverseForce2(totalForce[i], i);
        
            EBrakeEffect(totalForce[i], i);

            SteeringForce2(totalForce[i], i, dt);

    

        }
        //else
        {
            // reset wheel state?
        }

    }
    
    //  DoughnutTest();     I think we need a state for this.


    // test if we should change vehicle state
    // TODO - test for airborn etc.. before calling this?
    
    TestControllerForces(totalForce);   // <- mVehicleState only changed in here!

            // new - Mar 12, 2003
            //
            // use state to set "target" steering force.
            //
            // move towards target based on time rate
            // 
            // 
            
            // next step - make let-off gas get you out of slip state
            
            // how does gasreducinggrip thing fit into all of this?
            
            // next step - make skid level 0 to 1 based on have we gotten to our target level yet?
            
            //UpdateSteeringForce(dt);


    // not doing this anymore - only step is really to set good skid level
    


    SetSkidLevel();

    // TODO - actually limit locomotive and other forces??????

    // cheap and easy test of terrain friction:
    //ApplyTerrainFriction(totalForce);
    

    for(i = 0; i < 4; i++)
    {        
        ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->AddForce(totalForce[i], &(mForceApplicationPoints[i]));
    }
}



//=============================================================================
// PhysicsLocomotion::UpdateSteeringForce
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
/*

    // I don't think plum wants this now

void PhysicsLocomotion::UpdateSteeringForce(float dt)
{
    // default changerate: 50 units / second
    //static float changeRate = 10.0f;
    //static float changeRate = 5.0f;
    static float changeRate = 1.0f;
    
    float changethisframe = changeRate * dt;

    float target = 80.0f;   // just some default - should never use


    // if we are not VT_USER, switch immediately.


    if(mVehicle->mVehicleState == VS_NORMAL)
    {
        // "target" - mVehicle->mDesignerParams.mDpTireLateralResistanceNormal;
        
        target = mVehicle->mDesignerParams.mDpTireLateralResistanceNormal;
            
    }
    else if(mVehicle->mVehicleState == VS_EBRAKE_SLIP)
    {
        // "target" - mVehicle->mDesignerParams.mDpTireLateralResistanceSlip
    
        target = mVehicle->mDesignerParams.mDpTireLateralResistanceSlip;
        
    }
    else
    {
        // regular slip
        // "target" - mVehicle->mDesignerParams.mDpTireLateralResistanceSlipNoEBrake
        
        target = mVehicle->mDesignerParams.mDpTireLateralResistanceSlipNoEBrake;
        
    }

    if(mCurrentSteeringForce > target)
    {
        if( (mCurrentSteeringForce - changethisframe) > target)
        {
            mCurrentSteeringForce -= changethisframe;
        }
        else
        {
            mCurrentSteeringForce = target;        
        }
    }
    else if(mCurrentSteeringForce < target)
    {
        if((mCurrentSteeringForce + changethisframe) < target)
        {
            mCurrentSteeringForce += changethisframe;
        }
        else
        {
            mCurrentSteeringForce = target;
        }    
    }
    
    
    

}

*/

//=============================================================================
// PhysicsLocomotion::ApplyTerrainFriction
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector* totalForce)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::ApplyTerrainFriction(rmt::Vector* totalForce)
{
    int i;
    for(i = 0; i < 4; i++)
    {
        switch (mTerrainIntersectCache[i].mTerrainType)
        {

            case TT_Road:
                totalForce[i].Scale(TF_Road);
                break;
                
            case TT_Grass:	
                totalForce[i].Scale(TF_Grass);
                break;
                
            case TT_Dirt:  
                totalForce[i].Scale(TF_Dirt);
                break;
                
            case TT_Water:	
                totalForce[i].Scale(TF_Water);
                break;
                
            case TT_Gravel:
                totalForce[i].Scale(TF_Gravel);
                break;
                
            case TT_Wood:
                totalForce[i].Scale(TF_Wood);
                break;
                
            case TT_Sand:
                totalForce[i].Scale(TF_Sand);
                break;
                
            case TT_Metal:
                totalForce[i].Scale(TF_Metal);
                break;
                
            default: 
                break;
                
        }       
    
    }
}


//=============================================================================
// PhysicsLocomotion::GasForce2
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& forceResult, float dt, int wheelNum)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::GasForce2(rmt::Vector& forceResult, int wheelNum, float dt)
{
    if(mVehicle->mWheels[wheelNum]->mDriveWheel)
    {
        float desiredForceMag = 0.0f;

        // burnout check
        //
        //  TODO
        //  only high powered vehicles should do this? or do for longer?
        //if(mVehicle->mDesignerParams.mDpGasScale > 

        // maybe the % topspeed for which you do it is greater for high powered vehicles

        // try mapping 5.0 (average value) to %20 topspeed
        //static float burnouttunebullshit = 0.04f;
        const float burnouttunebullshit = 0.03f;

        //static float gasdecrease = 0.5f;
        const float gasdecrease = 0.75f;

        //static float burnoutsound = 0.7f;
        const float gasvalue = 0.99f;

        //float burnoutRange = mVehicle->mDesignerParams.mDpGasScale * burnouttunebullshit;

        float burnoutRange = mVehicle->mDesignerParams.mDpBurnoutRange;

        // need new designer param for this percent of top speed
        //
        // use same value for doing doughnut?

        // !! TODO - get hard/soft press on ps2!
        //
        // this is not gonna happen
        // if you put the ps2 button into analog mode, you gotta mash it to get _anything_!

        if(mVehicle->mPercentOfTopSpeed < burnoutRange && mVehicle->mGas > gasvalue  && mVehicle->mVehicleType == VT_USER)        
        {
            // burnout
            //
            // skid, smoke, spin and non loco

            // the regular calculation would give this:
            //desiredForceMag = mVehicle->mDesignerParams.mDpGasScale * mVehicle->mSlipGasModifier * mVehicle->mGas * mVehicle->mDesignerParams.mDpMass;

            const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();
            float gravFactor = gravity.y * -1.0f / 9.81f;

            //desiredForceMag = mVehicle->mDesignerParams.mDpGasScale * mVehicle->mSlipGasModifier * /*mVehicle->mGas*/ 1.0f * mVehicle->mDesignerParams.mDpMass;// * gravFactor;
            desiredForceMag = mVehicle->mDesignerParams.mDpGasScale * mVehicle->mSlipGasModifier * mVehicle->mGas * mVehicle->mDesignerParams.mDpMass;// * gravFactor;

            desiredForceMag *= gasdecrease;

            //mVehicle->mBurnoutLevel = burnoutsound;
            mVehicle->mBurnoutLevel = (1.0f - (0.5f * (mVehicle->mPercentOfTopSpeed / burnoutRange)));
            if(mVehicle->mBurnoutLevel < 0.0f)
            {
                mVehicle->mBurnoutLevel = 0.0f;
            }



            // additional test for donut?
            // here?
            if(rmt::Fabs(mVehicle->mUnmodifiedInputWheelTurnAngle) > 0.6)
            {
                //const float magicshit = 4.0f;
                //float magicshit = 9.0f;
                float magicshit = mVehicle->mDesignerParams.mDpDonutTorque;

                rmt::Vector etorque = mVehicle->mVehicleUp;
                float dir = 0.0f;
                if(mVehicle->mWheelTurnAngle > 0.0f)
                {
                    dir = 1.0f;
                }
                if(mVehicle->mWheelTurnAngle < 0.0f)
                {
                    dir = -1.0f;
                }

                //float speedeffect = mVehicle->mPercentOfTopSpeed * 2.0f;
                //if(speedeffect > 1.0f)
                //{
                //    speedeffect = 1.0f;
                //}

                float speedmodifier = 1.0f - mVehicle->mPercentOfTopSpeed;

                etorque.Scale(magicshit * dir * mVehicle->mDesignerParams.mDpMass * speedmodifier);

                //mVehicle->mPhObj->AddTorque(etorque);
                
                ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->AddTorque(etorque);

            }
            else
            {
                // speed burst build up here....

                if(mVehicle->mEBrake > 0.5f)
                {
                    mVehicle->mSpeedBurstTimer += dt;                    
                    mVehicle->mBuildingUpSpeedBurst = true;
                    mVehicle->mDoSpeedBurst = false;

                    //if(mVehicle->mSpeedBurstTimer > 3.0f)
                    if(mVehicle->mSpeedBurstTimer > mVehicle->mDesignerParams.mDpMaxSpeedBurstTime)
                    {
                        mVehicle->mSpeedBurstTimer = mVehicle->mDesignerParams.mDpMaxSpeedBurstTime;
                    }
                }
                else
                {
                    if(mVehicle->mBuildingUpSpeedBurst)
                    {       
                        rAssert(mVehicle->mDoSpeedBurst == false);
                        // just let off the button
                        mVehicle->mBuildingUpSpeedBurst = false;
                        mVehicle->mDoSpeedBurst = true;

                        SuperCamCentral* scc = GetSuperCamManager()->GetSCC(0);
                        SuperCam* sc = scc->GetActiveSuperCam();
            
                        mVehicle->mFOVToRestore = sc->GetFOV();

                        mVehicle->mSpeedBurstTimerHalf = mVehicle->mSpeedBurstTimer * 0.5f;


                    }
                }

            }


        }
        else
        {

            //!
            // TODO
            //
            // take analog gas usage out of here?
            
            if(mVehicle->mGas > 0.0f)
            {


                const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();
                float gravFactor = gravity.y * -1.0f / 9.81f;


                if(mVehicle->mVehicleState == VS_NORMAL)
                {
                    if(mVehicle->mVehicleType == VT_USER && mVehicle->mPercentOfTopSpeed > mVehicle->mDesignerParams.mDpGasScaleSpeedThreshold)
                    {
                        // use high speed gas scale
                        desiredForceMag = mVehicle->mDesignerParams.mDpHighSpeedGasScale * mVehicle->mSlipGasModifier * mVehicle->mGas * mVehicle->mDesignerParams.mDpMass;// * gravFactor;
                        
                    }
                    else
                    {
                        desiredForceMag = mVehicle->mDesignerParams.mDpGasScale * mVehicle->mSlipGasModifier * mVehicle->mGas * mVehicle->mDesignerParams.mDpMass;// * gravFactor;
        
                    }
                
                }
                else
                {
                    // slip

                    desiredForceMag = mVehicle->mDesignerParams.mDpSlipGasScale * mVehicle->mSlipGasModifier * mVehicle->mGas* mVehicle->mDesignerParams.mDpMass;// * gravFactor;

                }
            }
            
            // if we get out of speedburst range we should probably null out
            
                       
            //mVehicle->mBuildingUpSpeedBurst = false;
            //mVehicle->mSpeedBurstTimer = 0.0f;
            //mVehicle->mDoSpeedBurst = false;
            
        }


        // regardless of burnout or normal
        // test for wheelie down here
        if(mVehicle->mPercentOfTopSpeed < mVehicle->mDesignerParams.mDpWheelieRange && mVehicle->mGas > gasvalue  && mVehicle->mVehicleType == VT_USER)        
        {
            // set wheelie offsets

            if(!(mVehicle->mDoingWheelie) && !(mVehicle->mWeebleOn))
            {                
                rmt::Vector wheelie = mVehicle->mCMOffset;

                wheelie.y += mVehicle->mDesignerParams.mDpWheelieYOffset;
                wheelie.z += mVehicle->mDesignerParams.mDpWheelieZOffset;

                //mVehicle->mPhObj->SetExternalCMOffset(wheelie);
                ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(wheelie);
                
                mVehicle->mDoingWheelie = true;
            }


        }
        else if(mVehicle->mDoingWheelie)
        {
            //mVehicle->mPhObj->SetExternalCMOffset(mVehicle->mCMOffset);
            ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->SetExternalCMOffset(mVehicle->mCMOffset);
            mVehicle->mDoingWheelie = false;
        }



        rmt::Vector force = mVehicle->mVehicleTransverse;
        //force.CrossProduct(mTerrainIntersectCache[wheelNum].planeNorm);
        force.CrossProduct(mIntersectNormalUsed[wheelNum]);

        // just in case
        force.NormalizeSafe();

        
        if(mVehicle->mDoSpeedBurst)
        {
            desiredForceMag *= 2.0f;
            mVehicle->mSpeedBurstTimer -= dt;

            SuperCamCentral* scc = GetSuperCamManager()->GetSCC(0);
            SuperCam* sc = scc->GetActiveSuperCam();         


            if(mVehicle->mSpeedBurstTimer <= 0.0f)
            {
                mVehicle->mSpeedBurstTimer = 0.0f;
                mVehicle->mDoSpeedBurst = false;

                //sc->SetFOV(mVehicle->mFOVToRestore);  - this shoudl all be moved to super cam
            }
            else
            {
                //static float camhack = 1.0f;
                //static float camhack = 0.015f;
                float camhack = 0.015f;
                float fov;

                if(mVehicle->mSpeedBurstTimer > (mVehicle->mSpeedBurstTimerHalf * 1.5f))
                {
                    fov = sc->GetFOV();
                    fov = fov + camhack;
                    //sc->SetFOV(fov);   this shoudl all be moved to super cam
                }
                else if(mVehicle->mSpeedBurstTimer < (mVehicle->mSpeedBurstTimerHalf * 0.5f))                
                {
                    fov = sc->GetFOV();
                    fov = fov - camhack;
                    //sc->SetFOV(fov);   this shoudl all be moved to super cam
                }
                // else do nothing in the mid-range
            }            
            
        }
        
        if(mVehicle->mDoingJumpBoost)
        {
            // want to boost the jump
            // proportioal to the cars power, and current speed, and should have gas held down
            if(mVehicle->mGas > 0.5f && mVehicle->mPercentOfTopSpeed > 0.3f && mVehicle->GetSpeedKmh() < 140.0f)
            {
                //static float gasscaleboosteffect = 0.1f;
                //const float gasscaleboosteffect = 0.08f;
                float boostEffect = 0.0f;
                if( GetGameFlow()->GetCurrentContext() == CONTEXT_SUPERSPRINT )
                {
                     // do special boost only for AI in supersprint
                    rAssert( mVehicle->mVehicleType == VT_AI );
                    boostEffect = 1.4f;
                }
                else
                {
                    boostEffect = 0.07f;
                }
                const float gasscaleboosteffect = boostEffect;
                
                //float multiplier = 1.0f + /* mVehicle->mPercentOfTopSpeed */ + (mVehicle->mDesignerParams.mDpGasScale * gasscaleboosteffect);
                const float avgGasScale = 7.0f;
                
                float multiplier = 1.0f + /* mVehicle->mPercentOfTopSpeed */ + (avgGasScale * gasscaleboosteffect);
                
                desiredForceMag *= multiplier;
            }
        
        }


        //rmt::Vector force = mVehicle->mVehicleFacing;                

        // just in case
        // you only get speed burst of buildup if you hold down gas
        if(mVehicle->mGas == 0.0f && (mVehicle->mDoSpeedBurst || mVehicle->mSpeedBurstTimer > 0.0f))
        {        
            mVehicle->mSpeedBurstTimer = 0.0f;
            mVehicle->mDoSpeedBurst = false;
        }

        if(GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle() == mVehicle)
        {
            if(GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_HIGH_ACCELERATION))
            {
                desiredForceMag *= 2.0f;
            }
        }
        


        force.Scale(desiredForceMag);
        
        forceResult.Add(force);
    }            

}


//=============================================================================
// PhysicsLocomotion::DoughnutTest
//=============================================================================
// Description: Comment
//
// Parameters:  (int wheelNum)
//
// Return:      float 
//
//=============================================================================
void PhysicsLocomotion::DoughnutTest()
{
    // already know this is a drive wheel
/*    
    static float test1 = 0.3f;
    static float test2 = 0.8f;
    static float gastest = 0.9f;
    static float amount = 18.0f;

    if( mVehicle->mPercentOfTopSpeed < test1 && rmt::Fabs(mVehicle->mUnmodifiedInputWheelTurnAngle) > test2 && mVehicle->mGas > gastest)        
    {
        rmt::Vector doughnutTorque = mVehicle->mVehicleUp;
        
        // nasty hardcoded logic
        // -ve is left turn 

        if(mVehicle->mUnmodifiedInputWheelTurnAngle < 0.0f)
        {
            doughnutTorque.Scale(-amount * mVehicle->mDesignerParams.mDpMass);
        }
        else
        {
            doughnutTorque.Scale(amount* mVehicle->mDesignerParams.mDpMass);
        }

        mVehicle->mPhObj->AddTorque(doughnutTorque);

    }
*/
}   

//=============================================================================
// PhysicsLocomotion::BrakeForce2
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& forceResult, int wheelNum)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::BrakeForce2(rmt::Vector& forceResult, int wheelNum, float dt)
{
    
    //mBrakeTimer = 0.0f;
    //mBrakeActingAsReverse = false;

    // this is exactly reverse force
    
    if(mVehicle->mWheels[wheelNum]->mDriveWheel)
    {

        const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();
        float gravFactor = gravity.y * -1.0f / 9.81f;  

        // if we're coming to a stop we want to use brakescale
        // if we're going backwards want to use gasscale
        
        float desiredForceMag = 0.0f;
        
        
        float proj = mVehicle->mVelocityCM.DotProduct(mVehicle->mVehicleFacing);

        // don't want just any slight backwards motion to trigger
        const float cos120 = -0.5f;

        //if(proj < cos120)
        if(proj < 0.0f)
        {
            // going backwards
            
            // same accel and top speed reverse as forward
            
            if(mVehicle->mVehicleType == VT_USER && mVehicle->mPercentOfTopSpeed > mVehicle->mDesignerParams.mDpGasScaleSpeedThreshold)
            {
                desiredForceMag = mVehicle->mDesignerParams.mDpHighSpeedGasScale * mVehicle->mBrake * mVehicle->mDesignerParams.mDpMass;
            }
            else
            {
                desiredForceMag = mVehicle->mDesignerParams.mDpGasScale * mVehicle->mBrake * mVehicle->mDesignerParams.mDpMass;
            }
            
            if(mVehicle->mSpeedKmh < 50.0f)
            {
                mVehicle->mBurnoutLevel = mVehicle->mBrake;
            }
        }
        else
        {
            if(mVehicle->mGas > 0.1f)
            {
                desiredForceMag = (mVehicle->mDesignerParams.mDpGasScale) * mVehicle->mBrake * mVehicle->mDesignerParams.mDpMass * 0.75f;// magic number is gasdecrease when doing burnout
            }
            else
            {
                desiredForceMag = (mVehicle->mDesignerParams.mDpBrakeScale + 3.0f) * mVehicle->mBrake * mVehicle->mDesignerParams.mDpMass;// * gravFactor;            
            }
        }









        rmt::Vector force = mVehicle->mVehicleFacing;                
        force.Scale(-1.0f * desiredForceMag);
        
        // TODO - make this a state?
        // so that we can reduce the grip on the rear wheels and totally up the front?

        forceResult.Add(force);

        // if we are actually going in reverse
        //if(mVehicle->mVehicleFacing.DotProduct(mVehicle->mVelocityCM) < 0.0f)
        //{
        //    mVehicle->mBurnoutLevel = mVehicle->mBrake;
        //}
                
    }               
    


    /*
    //static float test = 0.8f;
    const float test = 0.8f;

    //const float lowspeed = 1.0f;
    const float lowspeed = 1.0f;




    // copy of test from rolling friction - 
    // holding brake down should hold car perfectly still....
    if(mVehicle->mBrakeActingAsReverse)
    {
        if(mVehicle->mSpeed > lowspeed)
        {   
            float desiredForceMag = mVehicle->mBrake * mVehicle->mDesignerParams.mDpBrakeScale * mVehicle->mDesignerParams.mDpMass;

            rmt::Vector force = mSuspensionPointVelocities[wheelNum];
        
            force.NormalizeSafe();
            force.Scale(-1.0f * desiredForceMag);

            forceResult.Add(force);
        }
        else if(mVehicle->mBrake > 0.7f)
        {
            // TODO - this may have to change with brakestands!
        
            // whittle away speed
            rmt::Vector& linearVel = mVehicle->mSimStateArticulated->GetLinearVelocity();
            rmt::Vector& angularVel = mVehicle->mSimStateArticulated->GetAngularVelocity();

            linearVel.Scale(test);
            angularVel.Scale(test);


        }
    }
    */
}


//=============================================================================
// PhysicsLocomotion::ReverseForce2
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& forceResult, int wheelNum)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::ReverseForce2(rmt::Vector& forceResult, int wheelNum)
{
    if(mVehicle->mWheels[wheelNum]->mDriveWheel)
    {
        float desiredForceMag = mVehicle->mDesignerParams.mDpGasScale * mVehicle->mReverse * mVehicle->mDesignerParams.mDpMass;

        rmt::Vector force = mVehicle->mVehicleFacing;                
        force.Scale(-1.0f * desiredForceMag);
        
        // TODO - make this a state?
        // so that we can reduce the grip on the rear wheels and totally up the front?

        forceResult.Add(force);
    }               
    
}


//=============================================================================
// PhysicsLocomotion::EBrakeEffect
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::EBrakeEffect(rmt::Vector& forceResult, int wheelNum)
{
    // in addition to reducing the rear grip, we want to apply (weak?) brakes too?

    // since people are using the handbrake like a parking brake, I guess I better acknowledge...
    static const float reallylowspeed = 0.5f;
    if(mVehicle->mSpeed < reallylowspeed && mVehicle->mEBrake == 1.0f)
    {
        // someone's trying to use it to hold the car in place, so just let the rest test do it's thing
        return;
    }
    
    if(mVehicle->mWheels[wheelNum]->mDriveWheel && mVehicle->mWheels[wheelNum]->mWheelTurnAngle == 0.0f)
    {
        // just lock up steer/drive wheels?????

        const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();
        float gravFactor = gravity.y * -1.0f / 9.81f;

        //float desiredForceMag = mVehicle->mEBrake * mVehicle->mDesignerParams.mDpBrakeScale * mVehicle->mDesignerParams.mDpMass;// * gravFactor;
        const float defaultebrakebrakescalethatplumishappywith = 3.0f;
        float desiredForceMag = mVehicle->mEBrake * defaultebrakebrakescalethatplumishappywith /*mVehicle->mDesignerParams.mDpBrakeScale*/ * mVehicle->mDesignerParams.mDpMass;// * gravFactor;

        rmt::Vector force = mSuspensionPointVelocities[wheelNum];
        
        force.NormalizeSafe();
        force.Scale(-1.0f * desiredForceMag);

        forceResult.Add(force);

    }




    //mVehicle->mVehicleState = VS_SLIP;
    
    // instead of adding another state variable, just put this calculation right in steering code?
    //mVehicle->mTireLateralResistance = mVehicle->mDesignerParams.mDpTireLateralResistanceSlip * (1.0f - mVehicle->mEBrake);

}

//=============================================================================
// PhysicsLocomotion::SteeringForce2
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& forceResult, int wheelNum)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::SteeringForce2(rmt::Vector& forceResult, int wheelNum, float dt)
{
    rmt::Vector lateralDirection = mVehicle->mVehicleTransverse;


    if(mVehicle->mWheels[wheelNum]->mSteerWheel)
    {        
        rmt::Matrix steeringMatrix; // TODO - make this a class member?
        steeringMatrix.Identity();
        steeringMatrix.FillRotateY(mVehicle->mWheels[wheelNum]->mWheelTurnAngle);

        lateralDirection.Transform(steeringMatrix);
    }

    //float lateralVelocityProjection = lateralDirection.DotProduct(mSuspensionPointVelocities[wheelNum]);

    

    rmt::Vector temp = mSuspensionPointVelocities[wheelNum];
    
    //rmt::Vector temp = mVehicle->mVelocityCM;


    float desiredLateralResistanceForce = 0.0f;

    //const float threshold = 0.15f;
    //static float threshold = 0.15f;
    const float threshold = 0.15f;
    if(mVehicle->mPercentOfTopSpeed > threshold)
    {
        //-------
        // normal
        //-------

        // this is the normal thing to do for regular and high-speed driving
        temp.NormalizeSafe();

        float lateralVelocityProjection = lateralDirection.DotProduct(temp);
        // the higher this is the more we're asking this tire to do for us!


        const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();
        float gravFactor = gravity.y * -1.0f / 9.81f;

        if(mVehicle->mVehicleState == VS_NORMAL)
        {
            desiredLateralResistanceForce = (mVehicle->mDesignerParams.mDpMass * lateralVelocityProjection * 
                                             mVehicle->mDesignerParams.mDpTireLateralResistanceNormal) /*mCurrentSteeringForce)*/ * 0.25f;
        }
        else if(mVehicle->mVehicleState == VS_EBRAKE_SLIP)
        {
            // slip
            {
                
                desiredLateralResistanceForce = (mVehicle->mDesignerParams.mDpMass * lateralVelocityProjection * 
                                                 mVehicle->mDesignerParams.mDpTireLateralResistanceSlip) /*mCurrentSteeringForce)*/ * 0.25f;

  

            }
        }
        else    // just VS_SLIP
        {
            // slip without ebrake!
            desiredLateralResistanceForce = (mVehicle->mDesignerParams.mDpMass * lateralVelocityProjection * 
                                             mVehicle->mDesignerParams.mDpTireLateralResistanceSlipNoEBrake /*mCurrentSteeringForce*/) * 0.25f;

                     
            if(mVehicle->mWheels[wheelNum]->mSteerWheel)
            {                    
                desiredLateralResistanceForce *= (1.0f + mVehicle->mDesignerParams.mDpSlipEffectNoEBrake);

            }
            else
            {                 
                desiredLateralResistanceForce *= (1.0f - mVehicle->mDesignerParams.mDpSlipEffectNoEBrake);
            }
        
        }


/*
    
        // put ebrake effect outside of any state

        if(mVehicle->mEBrake > 0.1) // note: we can be in this state without always holding the button down
        {
            if(mVehicle->mWheels[wheelNum]->mSteerWheel)
            {
                // increase resistance on steer wheels
                //desiredLateralResistanceForce *= (1.0f + mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);
                desiredLateralResistanceForce *= (1.0f + mVehicle->mDesignerParams.mDpEBrakeEffect);

            }
            else
            {
                // decrease resistance on rear
                //desiredLateralResistanceForce *= (1.0f - mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);
                desiredLateralResistanceForce *= (1.0f - mVehicle->mDesignerParams.mDpEBrakeEffect);
            }
        
        }            

*/



        // rockford...?


        // this is the condition for beginning a rockford
        if( mVehicle->mVehicleFacing.DotProduct(mVehicle->mVelocityCM) < 0.0f && mVehicle->mEBrake < 0.1 && rmt::Fabs(mVehicle->mUnmodifiedInputWheelTurnAngle) > 0.5f && mVehicle->mGas == 0.0f )//||
            //mVehicle->mDoingRockford )
        {
            
            mVehicle->mDoingRockford = true;
            // rockford in proportion to ebrake setting

            // note - the + and - are inverted from regular ebrake effect


            //static float magicshit = 20.0f;
            //static float mDpRockfordEffect = 0.5f;
            float mDpRockfordEffect = 0.5f;


        
            if(mVehicle->mWheels[wheelNum]->mSteerWheel)
            {
                // increase resistance on steer wheels
                //desiredLateralResistanceForce *= (1.0f + mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);
                desiredLateralResistanceForce *= (1.0f - mDpRockfordEffect);
            }
            else
            {
                // decrease resistance on rear
                //desiredLateralResistanceForce *= (1.0f - mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);
                desiredLateralResistanceForce *= (1.0f + mDpRockfordEffect);
            }
        
            
            float hackbullshit = 10.0f;
            float magicshit = hackbullshit * mDpRockfordEffect;

            rmt::Vector rockfordtorque = mVehicle->mVehicleUp;
            float dir = 0.0f;
            if(mVehicle->mWheelTurnAngle > 0.0f)
            {
                dir = -1.0f;
            }
            if(mVehicle->mWheelTurnAngle < 0.0f)
            {
                dir = 1.0f;
            }

            
            float speedeffect = mVehicle->mPercentOfTopSpeed;
            
            rockfordtorque.Scale(magicshit * dir * mVehicle->mDesignerParams.mDpMass * speedeffect);

            //mVehicle->mPhObj->AddTorque(rockfordtorque);
            ((ArticulatedPhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject(-1)))->AddTorque(rockfordtorque);


            mVehicle->mBurnoutLevel = 1.0f;




            //hmmm....
            //static float resdrop = 0.001f;
            float resdrop = 0.001f;
            //desiredLateralResistanceForce *= resdrop;

            // new place to set this
            mVehicle->mCollisionLateralResistanceDropFactor = resdrop;
            
        }

        
        // new - reduce traction on drive wheels if givin 'er gas
        const float gasmodifierthreshold = 0.7f;
		const float absoluteSpeedThreshold = 90.0f;
        if( mVehicle->mWheels[wheelNum]->mDriveWheel && mVehicle->mVehicleType == VT_USER && mVehicle->mGas > 0.0f && 
            mVehicle->mPercentOfTopSpeed > gasmodifierthreshold && rmt::Fabs(mVehicle->mWheelTurnAngleInputValue) > 0.1f &&
			mVehicle->mSpeedKmh > absoluteSpeedThreshold)
        {
            const float maxPenalty = 0.2f;
            float modifier = 1.0f - (maxPenalty * mVehicle->mGas);   // ? also include speed??
            
            desiredLateralResistanceForce *= modifier;          
            
            mVehicle->mDrawWireFrame = true;
            mVehicle->mLosingTractionDueToAccel = true; // for plum
            //mVehicle->mBurnoutLevel = 1.0f;

        
        }
        


        
    }
    else
    {
        //----------
        // low speed
        //----------
                
        temp.NormalizeSafe();

        float lateralVelocityProjection = lateralDirection.DotProduct(temp);
    
        // at low speeds we want to account for the (small) magnitude of the velocity        
        lateralVelocityProjection *= (mVehicle->mPercentOfTopSpeed / threshold);


        const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();
        float gravFactor = gravity.y * -1.0f / 9.81f;

        if(mVehicle->mVehicleState == VS_NORMAL)
        {
            desiredLateralResistanceForce = (mVehicle->mDesignerParams.mDpMass * lateralVelocityProjection * mVehicle->mDesignerParams.mDpTireLateralResistanceNormal) * 0.25f;// * gravFactor;
        }
        else
        {
            // slip
            desiredLateralResistanceForce = (mVehicle->mDesignerParams.mDpMass * lateralVelocityProjection * mVehicle->mDesignerParams.mDpTireLateralResistanceSlip) * 0.25f;// * gravFactor;

            /*
            if(mVehicle->mEBrake > 0.0)
            {
                if(mVehicle->mWheels[wheelNum]->mSteerWheel)
                {
                    // increase resistance on steer wheels
                    desiredLateralResistanceForce *= (1.0f + mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);

                }
                else
                {
                    // decrease resistance on rear
                    desiredLateralResistanceForce *= (1.0f - mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);

                }        

            }
            */
           

        }

    }



        // put ebrake effect outside of any state
    if(mVehicle->mPercentOfTopSpeed > 0.02f)
    {
        if(mVehicle->mEBrake > 0.1) // note: we can be in this state without always holding the button down
        {
            if(mVehicle->mWheels[wheelNum]->mSteerWheel)
            {
                // increase resistance on steer wheels
                //desiredLateralResistanceForce *= (1.0f + mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);
                desiredLateralResistanceForce *= (1.0f + mVehicle->mDesignerParams.mDpEBrakeEffect);

            }
            else
            {
                // decrease resistance on rear
                //desiredLateralResistanceForce *= (1.0f - mVehicle->mEBrake * mVehicle->mDesignerParams.mDpEBrakeEffect);
                desiredLateralResistanceForce *= (1.0f - mVehicle->mDesignerParams.mDpEBrakeEffect);
            }
        
        }            
    }


    //rmt::Vector force = lateralDirection;
    
    // assumign these are good?

    //mTerrainIntersectCache[i].planePosn = planePosn;
    //mTerrainIntersectCache[i].planeNorm = planeNormal;


    //rmt::Vector force = mTerrainIntersectCache[wheelNum].planeNorm;
    
    rmt::Vector force = mIntersectNormalUsed[wheelNum];
    force.CrossProduct(mVehicle->mVehicleFacing);

    // just in case
    force.NormalizeSafe();
    
    //rmt::Vector force = mVehicle->mVehicleTransverse;       // I think this is better!





    /// new quick terrain based friction test
    // only affect steering
    
    switch (mTerrainIntersectCache[wheelNum].mTerrainType)
    {
        case TT_Road:
            desiredLateralResistanceForce *= TF_Road;
            break;
            
        case TT_Grass:	
            desiredLateralResistanceForce *= TF_Grass;
            break;
            
        case TT_Dirt:  
            desiredLateralResistanceForce *= TF_Dirt;
            break;
            
        case TT_Water:	
            desiredLateralResistanceForce *= TF_Water;
            break;
            
        case TT_Gravel:
            desiredLateralResistanceForce *= TF_Gravel;
            break;
            
        case TT_Wood:
            desiredLateralResistanceForce *= TF_Wood;
            break;
            
        case TT_Sand:
            desiredLateralResistanceForce *= TF_Sand;
            break;
            
        case TT_Metal:
            desiredLateralResistanceForce *= TF_Metal;
            break;
            
        default: 
            break;
            
    }


    force.Scale(-1.0f * desiredLateralResistanceForce);


    if(1)//mVehicle->mBurnoutLevel == 0.0f)
    {
        // test
        // scale lateral resistance way down if we have just had a collision
        force.Scale(mVehicle->mCollisionLateralResistanceDropFactor);
    }


    // test before or after the above drop?
    //TestSteeringForce(force, wheelNum);   



    forceResult.Add(force);
    
    
    
    // new test
    // faked high speed instability
    /*
    const float highSpeedInstabilityThreshold = 0.6f;
    if(mVehicle->mPercentOfTopSpeed > highSpeedInstabilityThreshold)
    {
        // add some magic torque depending on the input wheel turn angle.
        if(mVehicle->mWheelTurnAngleInputValue != 0.0f && !(mVehicle->mAirBorn))
        {
            static float magicshit = 10.0f;
                        
            rmt::Vector instabilityTorque = mVehicle->mVehicleFacing;
            instabilityTorque.Scale(magicshit * mVehicle->mDesignerParams.mDpMass * mVehicle->mPercentOfTopSpeed * mVehicle->mWheelTurnAngleInputValue);            
           
            sim::PhysicsObject* phobj = (sim::PhysicsObject*)(mVehicle->mSimStateArticulated->GetSimulatedObject());
            rAssert(phobj);
            
            phobj->AddTorque(instabilityTorque);          
            
        
        }
    }
    */
    
}


//=============================================================================
// PhysicsLocomotion::TestSteeringForce
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& force)
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::TestSteeringForce(rmt::Vector& force, int index)
{
    //mVehicleState not changed here - this is just used to limit the lateral force

    //static float forceCap = 1.0f;


    rAssert(0);
    

/*
    static float rearWheelMult = 1.0f;

    if(mVehicle->mVehicleState == VS_SLIP)
    {

        float suspensionEffect = mCachedSuspensionForceResults[index];        

        float suspensionMinEffect = mVehicle->mSuspensionRestValue;

        if(suspensionEffect < suspensionMinEffect)
        {
            suspensionEffect = suspensionMinEffect;
        }

        float maxTireForce = mVehicle->mDesignerParams.mDpTireLateralStaticGrip * suspensionEffect;


        // cap all forces
        int j;
        for(j = 0; j < 4; j++)
        {
            
            //if(totalForce[j].Magnitude() > forceCap)
            //{
            //    totalForce[j].NormalizeSafe();
            //    totalForce[j].Scale(forceCap);
            //}
                        
            if(j < 2)
            {

                if(force.Magnitude() > maxTireForce * rearWheelMult)
                {
                    force.NormalizeSafe();
                    force.Scale(maxTireForce * rearWheelMult);
                }
            }
            else
            {
                if(force.Magnitude() > maxTireForce)
                {
                    force.NormalizeSafe();
                    force.Scale(maxTireForce);
                }
            
            }        
    
        }

    }

*/
}

//=============================================================================
// PhysicsLocomotion::TestControllerForces
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector* totalForce) -- points to array of 4
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::TestControllerForces(rmt::Vector* totalForce)
{

    // mVehicleState only changed in here!


    // find wheel with highest down force
    int i;
    int index = 0;
    for(i = 1; i < 4; i++)
    {
        if(mCachedSuspensionForceResults[i] > mCachedSuspensionForceResults[index])
        {
            index = i;
        }
    }

    // TODO - fix this for airborn!
    /*
    float suspensionEffect = mCachedSuspensionForceResults[index];        

    float suspensionMinEffect = mVehicle->mSuspensionRestValue;

    if(suspensionEffect < suspensionMinEffect)
    {
        suspensionEffect = suspensionMinEffect;
    }
    */

    // new test
    float suspensionEffect = mVehicle->mSuspensionRestValue;


    float maxTireForce = mVehicle->mDesignerParams.mDpTireLateralStaticGrip * suspensionEffect;

                    /*
                        // new Mar 12, 2003
                        // affect the maxTireForce calculation by gas
                        const float gasmodifierthreshold = 0.7f;
	                    const float absoluteSpeedThreshold = 90.0f;
                    	
                        if(mVehicle->mVehicleType == VT_USER && mVehicle->mGas > 0.0f && 
                        mVehicle->mPercentOfTopSpeed > gasmodifierthreshold && rmt::Fabs(mVehicle->mWheelTurnAngleInputValue) > 0.1f &&
	                    mVehicle->mSpeedKmh > absoluteSpeedThreshold)
                        {
                            //const float maxPenalty = 0.2f;
                            static float maxPenalty = 0.4f;
                            float modifier = 1.0f - (maxPenalty * mVehicle->mGas);   // ? also include speed??
                            
                            // *** desiredLateralResistanceForce *= modifier;          
                            maxTireForce *= modifier;
                            
                            mVehicle->mDrawWireFrame = true;
                            mVehicle->mLosingTractionDueToAccel = true; // for plum
                            //mVehicle->mBurnoutLevel = 1.0f;

                        
                        }
                    */


    if(mVehicle->mVehicleState == VS_NORMAL)
    {
        // if the wheel with the greatest down force is slipping
        // make whole car slip
      
        if(mVehicle->mEBrake > 0.1f && mVehicle->mPercentOfTopSpeed > 0.05f)
        {
            // change state to controlled skid
            mVehicle->mVehicleState = VS_EBRAKE_SLIP;
        }
        else
        {
            // now if wheel index is slipping, whole fucking car is slipping
            float totalForceMag = totalForce[index].Magnitude();
            //float maxTireForce = mVehicle->mDesignerParams.mDpTireLateralStaticGrip * suspensionEffect;
            if(totalForceMag > maxTireForce)
            {
                // fucker is sliding
                mVehicle->mVehicleState = VS_SLIP;
                //mVehicle->mTireLateralResistance = mVehicle->mDesignerParams.mDpTireLateralResistanceSlip;            
           
            }        
        }        

    }
    else
    {  
        if( !mVehicle->mOutOfControl )
        {
            // need heruistic to go back to normal
            rmt::Vector velDir = mVehicle->mVelocityCM;
            velDir.NormalizeSafe();

            float cos5 = 0.9962f;
            float cos10 = 0.9848f;
            float cos20 = 0.9397f;
            float cos15 = 0.9659f;

            // in addition to the alignment test, should not go into VS_SLIP if you're going very slow,
            // ebrake or not.

            if( (velDir.DotProduct(mVehicle->mVehicleFacing) > cos15 && rmt::Fabs(mVehicle->mWheelTurnAngleInputValue) < 0.1f) || 
                 mVehicle->mSpeed < 1.0f ||
                 mVehicle->mGas == 0.0f)
            {
                mVehicle->mVehicleState = VS_NORMAL;
                //mVehicle->mTireLateralResistance = mVehicle->mDesignerParams.mDpTireLateralResistanceNormal;            
            }  

        /*
            // duplicate this here in case we only got in here cause the ebrake is held down.
            if(mVehicle->mEBrake > 0.1f && mVehicle->mSpeed > 1.0f)
            {
                mVehicle->mVehicleState = VS_SLIP;
                //mVehicle->mTireLateralResistance = mVehicle->mDesignerParams.mDpTireLateralResistanceSlip;
            //    return;
            }
        */    
        }
    }





}


//=============================================================================
// PhysicsLocomotion::SetSkidLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PhysicsLocomotion::SetSkidLevel()
{
    mVehicle->mSkidLevel = 0.0f;
    mVehicle->mSlipGasModifier = 1.0f;

    //static float hack = 0.7f;
    //mVehicle->mSkidLevel = hack;
    

    float skid = 0.0f;
    float speedfactor = mVehicle->mPercentOfTopSpeed;
       


    rmt::Vector velDir = mVehicle->mVelocityCM;
    velDir.NormalizeSafe();

    
    float projectionOnly = rmt::Fabs(velDir.DotProduct(mVehicle->mVehicleTransverse));


    if(mVehicle->mVehicleState == VS_SLIP || mVehicle->mVehicleState == VS_EBRAKE_SLIP)
    {
    //    static float hack = 0.7f;
    //    mVehicle->mSkidLevel = hack;


        if(1)//projectionOnly > 0.7f)
        {
            // this actually works quite well
            skid = projectionOnly;
        }
        else
        {
            skid =  projectionOnly * speedfactor;
        }

        
        // if they're using the ebrake, we want whichever is loudest:
        if(mVehicle->mEBrake * 0.5f > skid)
        {
            skid = mVehicle->mEBrake * 0.5f;
        }
  
    }

    if(mVehicle->mBrake > 0.05f && speedfactor > 0.07f)
    {
        /*
        rmt::Vector velDir = mVehicle->mVelocityCM;
        velDir.NormalizeSafe();

        float proj = velDir.DotProduct(mVehicle->mVehicleFacing);

        // hmm..
        if(mVehicle->mBrake * 0.5f > skid && proj > 0.0f)
        {
            skid = mVehicle->mBrake * 0.5f;
        }
        */
        
        // not sure if values lower than 0.5 are working...
        //
        // 
        float forward = velDir.DotProduct(mVehicle->mVehicleFacing);
        if(forward > 0.0f && speedfactor < 0.5f)
        {
            skid = 1.0f;
        }
        
        
    }

    if(mVehicle->mEBrake > 0.85f && speedfactor > 0.07f)// && mVehicle->mWheelTurnAngle == 0.0f)
    {
        // hmm..
        if(mVehicle->mEBrake * 0.5f > skid)
        {
            skid = mVehicle->mEBrake * 0.5f;
        }
    }

    
    // compare to burnout level
    if(mVehicle->mBurnoutLevel > skid)
    {
        mVehicle->mSkidLevel = mVehicle->mBurnoutLevel;
    }
    else
    {
        mVehicle->mSkidLevel = skid;
    }

    
    // still too quiet
    if(mVehicle->mSkidLevel > 0.0f)
    {
        if(mVehicle->mSkidLevel < 0.5f)
        {
            mVehicle->mSkidLevel = 0.5f;
        }
    }


/*

    // override any of the above shit if wheels are not in contact with ground
    int i;
    if(mBurnoutLevel > 0.0f)
    {
        if(mWheels[0]->mWheelInCollision


        for(i = 0; i < 2; i++)
        {
            if(mWheels[i]->mWheelInCollision)   // will this value still be valid here?
            {
                mGeometryVehicle->SetSkidValues(i, mSkidLevel);
            }
        }
    }
    else
    {
        for(i = 0; i < 4; i++)
        {
            if(mWheels[i]->mWheelInCollision)   // will this value still be valid here?
            {
                mGeometryVehicle->SetSkidValues(i, mSkidLevel);
            }
        }
    }   

    
*/


}









