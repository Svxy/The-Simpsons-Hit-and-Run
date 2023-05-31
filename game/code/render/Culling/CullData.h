#ifndef __CULL_DATA_H__
#define __CULL_DATA_H__

#include <render/culling/ScratchArray.h>

class CullCache
{
public:
   CullCache()
   {
      mIntArray.Allocate( 1024 );
      InUse = false;
   }
   ~CullCache(){}

   bool Acquire( SubArray<int>& orSubArray, int iSize )
   {
      if( !InUse )
      {
         rAssert( iSize <= mIntArray.mSize );
         orSubArray.Init( iSize, mIntArray.mpData );
         return true;
      }
      else
      {
         return false;
      }
   }

   void Release( SubArray<int>& orSubArray )
   {
      // This will be replaced by a scratch array soon
      orSubArray.Invalidate();
   }

protected:

   // This will be replaced by a scratch array soon
   FixedArray<int> mIntArray;
   bool InUse;

};

CullCache& theCullCache();

#endif