#ifndef __MATRIX_3_F__
#define __MATRIX_3_F__

#include <radmath/radmath.hpp>
#include <raddebug.hpp>

class Matrix3f : public rmt::Matrix
{
public:
   Matrix3f(){}
   ~Matrix3f(){}

   float& operator[](int i)
   {
      return m[i/4][i%4];
   }
protected:
};

#endif