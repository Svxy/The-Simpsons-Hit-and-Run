#ifndef __VECTOR_3F_H__
#define __VECTOR_3F_H__

#include <radmath/radmath.hpp>
#include <raddebug.hpp>

class Vector3f : public rmt::Vector
{
public:
   Vector3f()                                 : Vector() {}
   Vector3f( float iX, float iY, float iZ )   : Vector( iX, iY, iZ ) {}
   Vector3f( float* ipVector )                : Vector( ipVector[0], ipVector[1], ipVector[2] ) {}

   Vector& SetTo( const Vector& vect )
   {
      x = vect.x;
      y = vect.y;
      z = vect.z;
      return *this;
   }

   Vector& Div(const Vector& vect1, const Vector& vect2)
   {
      x = vect1.x / vect2.x;
      y = vect1.y / vect2.y;
      z = vect1.z / vect2.z;
      return *this;
   }

   Vector& Div(const Vector& vect1, const float iMag )
   {
      x = vect1.x / iMag;
      y = vect1.y / iMag;
      z = vect1.z / iMag;
      return *this;
   }

   Vector& Mult(const Vector& vect1, const float iMag)
   {
      x = vect1.x * iMag;
      y = vect1.y * iMag;
      z = vect1.z * iMag;
      return *this;
   }

   Vector& Mult( const float iMag)
   {
      x *= iMag;
      y *= iMag;
      z *= iMag;
      return *this;
   }

   void SetFP( float* ipVector )
   {
      x = ipVector[0];
      y = ipVector[1];
      z = ipVector[2];
   }

   Vector& operator-=( const float& right )
   {
      x -= right;
      y -= right;
      z -= right;
      return *this;
   }
   
   Vector& operator+=( const float& right )
   {
      x += right;
      y += right;
      z += right;
      return *this;
   }
   
protected:
};

#endif