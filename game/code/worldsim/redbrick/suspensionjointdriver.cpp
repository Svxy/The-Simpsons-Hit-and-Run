/*===========================================================================
   suspensionjointdriver.cpp

   created Feb 1, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#include <poser/pose.hpp>
#include <poser/poseengine.hpp>
#include <poser/posedriver.hpp>
#include <poser/joint.hpp>

#include <worldsim/redbrick/suspensionjointdriver.h>
#include <worldsim/redbrick/wheel.h>



//------------------------------------------------------------------------
SuspensionJointDriver::SuspensionJointDriver(Wheel* wheel, int jointIndex)
{
    mWheel = wheel;
    mJointIndex = jointIndex;

}


//------------------------------------------------------------------------
SuspensionJointDriver::~SuspensionJointDriver()
{
    //
}



//------------------------------------------------------------------------
void SuspensionJointDriver::Update(poser::Pose* pose)
{
    // process results of last time's collision detection and solving...


    //mWheel->ResolveOffset();    // I think this call is redundant - bug

    // now just use mYOffset
    //float yOffset = mWheel->mYOffset;   // need to reapply this because we called PoseEngine::Begin and overwrote the suspension results...


    //!
    // BUT
    // 
    // this mYOffset is from the bottomed out location set in precollisionprep, not the neutral point
    //
    // try not calling PoseEngine::Begin in PostUpdate







    poser::Joint* joint = pose->GetJoint(mJointIndex);

    // TODO - is this call inefficient?
    // I'm gonna have to make a copy of the matrix at some point so I can change some values....
    rmt::Matrix matrix = joint->GetObjectMatrix();




//Matrix& FillRotateXYZ(float anglex, float angley, float anglez);

    /*


    rmt::Matrix rot;
    rot.Identity();
    rot.FillRotateX(mWheel->mRotAngle * 100.0f);

    matrix.Mult(rot);


    */

    
    //matrix.m[3][1] += yOffset;

    // !! TODO - safe to just apply this?
    //matrix.m[3][1] += mWheel->mPhysicsVehicleOwner->mGravitySettleYCorrection;  // this is insignificant

    if(mWheel->mSteerWheel)
    {
        // this is one of the two front wheels, so turn it
        float angle = mWheel->mWheelTurnAngle;

        // should this be done before the addition of the yOffset?
        // shouldn't matter too much....
        matrix.FillRotateXYZ(mWheel->mCumulativeRot, mWheel->mWheelTurnAngle, 0.0f);

    }
    else
    {
        matrix.FillRotateX(mWheel->mCumulativeRot);
    }

    joint->SetObjectMatrix(matrix);

    /*

    rmt::Vector trans = joint->GetObjectTranslation();

    trans.y += yOffset;
    
    joint->SetObjectTranslation(trans);

    if(mWheel->GetNum() < 1)
    {
        // this is one of the two front wheels, so rotate it

           const rmt::Matrix& GetObjectMatrix() const
        { return GetObjectTransform().GetMatrix(); }
    void SetObjectMatrix(const rmt::Matrix& matrix);
  

    }
    */
}
