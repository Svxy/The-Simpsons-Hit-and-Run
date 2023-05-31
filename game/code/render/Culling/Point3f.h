#ifndef __VECTOR_3F_H__
#define __VECTOR_3F_H__

#include <radmath.hpp>

class Vector3f : public Vector
{
public:
   Vector3f()                                 : Vector() {}
   Vector3f( float iX, float iY, float iZ )   : Vector( iX, iY, iZ ) {}
   Vector3f( float* ipVector )                : Vector( ipVector[0], ipVector[1], ipVector[2] ) {}

   float& operator[]( int iIndex ) 
   { 
      rAssert( (iIndex>-1) && (iIndex<3) );
      return ((float*)(this))[iIndex];
   }

protected:
};

#endif