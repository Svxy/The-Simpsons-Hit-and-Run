#ifndef __BOUNDS_H__
#define __BOUNDS_H__

#include <render/culling/Vector3f.h>
#include <render/culling/Vector3i.h>

class Bounds3f
{
public:
   Bounds3f(){}
   ~Bounds3f(){}

   void Accumulate( float iX, float iY, float iZ )
   {
      if( iX < mMin[0])
      {
         mMin[0] = iX;
      }
      else
      {
         if( iX > mMax[0])
         {
            mMax[0] = iX;
         }
      }

      if( iY < mMin[1])
      {
         mMin[1] = iY;
      }
      else
      {
         if( iY > mMax[1])
         {
            mMax[1] = iY;
         }
      }

      if( iZ < mMin[2])
      {
         mMin[2] = iZ;
      }
      else
      {
         if( iZ > mMax[2])
         {
            mMax[2] = iZ;
         }
      }
 }


   void Accumulate( float* ipPoint )
   {
      for( int i=0; i<3; i++ )
      {
         if( ipPoint[i] < mMin[i])
         {
            mMin[i] = ipPoint[i];
         }
         else
         {
            if( ipPoint[i] > mMax[i])
            {
               mMax[i] = ipPoint[i];
            }
         }
      }
   }

   void Accumulate( Vector3f& irPoint )
   {
      for( int i=0; i<3; i++ )
      {
         if( irPoint[i] < mMin[i])
         {
            mMin[i] = irPoint[i];
         }
         else
         {
            if( irPoint[i] > mMax[i])
            {
               mMax[i] = irPoint[i];
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   // Taken & Tailored 
   // from http://www.gamasutra.com/features/19991018/Gomez_4.htm
   //////////////////////////////////////////////////////////////////////////
   bool IntersectsSphere( Vector3f& irCenter, float iRadius )
   {
        float s, d = 0; 

        //find the square of the distance
        //from the sphere to the box
        for( int i=0 ; i<3 ; i++ )
        {
            if( irCenter[i] < mMin[i] )
            {
                s = irCenter[i] - mMin[i];
                d += s*s; 
            }
            else if( irCenter[i] > mMax[i] )
            {
                s = irCenter[i] - mMax[i];
                d += s*s; 
            }
        }
        return d <= iRadius*iRadius;
   }

   Vector3f mMin, mMax;
};

class MinBound : public Vector3f
{
public:
   MinBound(){}
   ~MinBound(){}

   void Accumulate( Vector3f& irPoint )
   {
      if( irPoint.x < x)
      {
         x = irPoint.x;
      }
      if( irPoint.y < y)
      {
         y = irPoint.y;
      }
      if( irPoint.z < z)
      {
         z = irPoint.z;
      }
   }

};


class Bounds3i
{
public:
   Bounds3i(){}
   ~Bounds3i(){}

   void Accumulate( Vector3i& irPoint )
   {
      for( int i=0; i<3; i++ )
      {
         if( irPoint[i] < mMin[i])
         {
            mMin[i] = irPoint[i];
         }
         else
         {
            if( irPoint[i] > mMax[i])
            {
               mMax[i] = irPoint[i];
            }
         }
      }
   }

   Vector3i mMin, mMax;
};


#endif