/*===========================================================================
   rootmatrixdriver.cpp

   created Jan 24, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/


#include <poser/pose.hpp>
#include <poser/poseengine.hpp>
#include <poser/posedriver.hpp>
#include <poser/joint.hpp>

#include <worldsim/redbrick/rootmatrixdriver.h>

//------------------------------------------------------------------------
void RootMatrixDriver::Update(poser::Pose* pose)
{
    poser::Joint* j = pose->GetJoint(0);
    //rmt::Matrix m = j->GetObjectMatrix();
    //m.Mult(*mRootMatrix);
    j->SetWorldMatrix(*mRootMatrix);    // recall, mRootMatrix is a pointer to mTransform of the physics vehicle
}

