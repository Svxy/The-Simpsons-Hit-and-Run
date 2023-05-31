#ifndef __PLANE_3F_H__
#define __PLANE_3F_H__

#include <render/culling/Vector3f.h>

class AAPlane3f
{
public:
   AAPlane3f(){}
   ~AAPlane3f(){}

   void Set( char iAxis, float iPosn )
   {
      mAxis = iAxis;
      mPosn = iPosn;
   }

   char  mAxis;
   float mPosn;
};

class AANormPlane3f
{
public:
   AANormPlane3f(){}
   ~AANormPlane3f(){}

   char  mAxis;
   float mPosn;
   char  mNorm; //Norm == 0, Normal Points toward the Origin; != 0, away
};

class NormPlane3f
{
public:
   NormPlane3f(){}
   ~NormPlane3f(){}

   void Normalise()
   {
       float Mag = (float)rmt::Sqrt( mNorm.x*mNorm.x + mNorm.y*mNorm.y + mNorm.z*mNorm.z + mOffset*mOffset );
      mNorm.x /= Mag;
      mNorm.y /= Mag;
      mNorm.z /= Mag;
      mOffset /= Mag;
   }

   Vector3f mNorm;
   float    mOffset;
};

#endif