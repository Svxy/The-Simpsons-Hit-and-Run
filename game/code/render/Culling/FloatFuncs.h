#ifndef __FLOAT_FUNCS_H__
#define __FLOAT_FUNCS_H__

namespace FloatFuncs
{
   const float MaxRemainder = 0.999999f;
   const float MinRemainder = 0.000001f;
}

//
// TODO - Some Functions won't work in the negative domain for now
//
extern "C"
{
   inline float sRoundUp( float iFloat )
   {
      return iFloat + FloatFuncs::MaxRemainder;
   }

   inline int sUpperInt( float iFloat )
   {
      return (int)(sRoundUp(iFloat));      
   }

   inline float sRoundDown( float iFloat )
   {
      return iFloat - FloatFuncs::MaxRemainder;
   }

   inline int sLowerInt( float iFloat )
   {
      return (int)(iFloat);      
   }

   inline float sTruncate( float iFloat )
   {
      return  (float) ((int)iFloat);
   }
}
#endif