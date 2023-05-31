/*===========================================================================
   rootmatrixdriver.h

   created Jan 24, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#ifndef _ROOTMATRIXDRIVER_H
#define _ROOTMATRIXDRIVER_H


#include <poser/pose.hpp>
#include <poser/poseengine.hpp>
#include <poser/posedriver.hpp>

/*
class PoseDriver: public tEntity
{
public:

   PoseDriver();

   virtual int GetMinimumJointIndex() const
      { return 0; }
   virtual int GetPriority() const
      { return PRIORITY_DEFAULT; }

   bool IsEnabled() const
      { return m_IsEnabled; }
   void SetIsEnabled(bool isEnabled)
      { m_IsEnabled = isEnabled; }

   virtual void Advance(float dt) = 0;
   virtual void Update(Pose* pose) = 0;

protected:

   virtual ~PoseDriver();

private:

   bool m_IsEnabled;
};
*/


class RootMatrixDriver : public poser::PoseDriver
{
public:
    RootMatrixDriver(rmt::Matrix* inRootMatrix) : mRootMatrix(inRootMatrix) {}
    virtual int GetMinimumJointIndex() const { return 0; }
    virtual int GetPriority() const { return 0; }
    virtual void Advance(float dt) {}
    virtual void Update(poser::Pose* pose);

    // move definition to cpp file
    /*
    {
        poser::Joint* j = pose->GetJoint(0);
        rmt::Matrix m = j->GetObjectMatrix();
        m.Mult(*mRootMatrix);
        j->SetWorldMatrix(m);
    }
    */

private:
    rmt::Matrix* mRootMatrix;
};

#endif // #ifndef _ROOTMATRIXDRIVER_H


