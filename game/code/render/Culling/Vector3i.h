#ifndef __VECTOR_3I_H__
#define __VECTOR_3I_H__

#include <render/culling/FloatFuncs.h>

#include <render/culling/Vector3f.h>

class Vector3i
{
public:
   Vector3i(){}
   ~Vector3i(){}

   void Set( int iX, int iY, int iZ )
   {
      mX = iX;
      mY = iY;
      mZ = iZ;
   }

   void Set( int* iCoords )
   {
      mX = iCoords[0];
      mY = iCoords[1];
      mZ = iCoords[2];
   }

   void Set( Vector3i& irVector )
   {
      mX = irVector.mX;
      mY = irVector.mY;
      mZ = irVector.mZ;
   }

   void SetToSpan( Vector3f& irMin, Vector3f& irMax )
   {
      mX = sUpperInt(irMax.x) - sLowerInt(irMin.x);
      mY = sUpperInt(irMax.y) - sLowerInt(irMin.y);
      mZ = sUpperInt(irMax.z) - sLowerInt(irMin.z);
   }

   void SetToSpan( Vector3i& irMin, Vector3i& irMax )
   {
      mX = irMax.mX - irMin.mX;
      mY = irMax.mY - irMin.mY;
      mZ = irMax.mZ - irMin.mZ;
   }

   int& operator[]( int iIndex )
   {
      return ((int*)this)[iIndex];
   }

   int MaxElem()
   {
      if( mX >= mY )
      {
         if( mX >= mZ )
         {
            return mX;
         }
         else
         {
            return mZ;
         }
      }
      else
      {
         if( mY >= mZ )
         {
            return mY;
         }
         else
         {
            return mZ;
         }
      }
   }

   int MaxIndex()
   {
      if( mX >= mY )
      {
         if( mX >= mZ )
         {
            return 0;
         }
         else
         {
            return 2;
         }
      }
      else
      {
         if( mY >= mZ )
         {
            return 1;
         }
         else
         {
            return 2;
         }
      }
   }

   int MinElem()
   {
      if( mX <= mY )
      {
         if( mX <= mZ )
         {
            return mX;
         }
         else
         {
            return mZ;
         }
      }
      else
      {
         if( mY <= mZ )
         {
            return mY;
         }
         else
         {
            return mZ;
         }
      }
   }

   int MinIndex()
   {
      if( mX <= mY )
      {
         if( mX <= mZ )
         {
            return 0;
         }
         else
         {
            return 2;
         }
      }
      else
      {
         if( mY <= mZ )
         {
            return 1;
         }
         else
         {
            return 2;
         }
      }
   }
   int mX,mY,mZ;
protected:
};
#endif