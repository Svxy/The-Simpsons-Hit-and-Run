/*===========================================================================
   suspensionjointdriver.h

   created Feb 1, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#ifndef _SUSPENSIONJOINTDRIVER_H
#define _SUSPENSIONJOINTDRIVER_H


#include <poser/pose.hpp>
#include <poser/poseengine.hpp>
#include <poser/posedriver.hpp>

class Wheel;

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


class SuspensionJointDriver : public poser::PoseDriver
{
public:
    SuspensionJointDriver(Wheel* wheel, int jointIndex);
    ~SuspensionJointDriver();

    virtual void Advance(float dt) {}
    virtual void Update(poser::Pose* pose);

private:
    Wheel* mWheel;
    int mJointIndex;

};

#endif // #ifndef _SUSPENSIONJOINTDRIVER_H


