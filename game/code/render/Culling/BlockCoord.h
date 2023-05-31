#ifndef __BLOCK_COORD_H__
#define __BLOCK_COORD_H__

#include <render/culling/Vector3f.h>
#include <render/culling/Bounds.h>
#include <render/culling/FloatFuncs.h>

class BlockCoord
{
public:
   BlockCoord(){}
   ~BlockCoord(){}

   void Init( Bounds3f& irBounds, Vector3f& irGranularities )
   {
      mOffset = irBounds.mMin; 

      for( int i=0; i<3; i++ )
      {
         mExtents[i] = sUpperInt( (irBounds.mMax[i] - irBounds.mMin[i])/irGranularities[i] );
         
         rAssert( mExtents[i] > -1 );

         if( mExtents[i] == 0 )
            mExtents[i] = 1;
      }
   }
   
   Vector3f mOffset;
   int      mExtents[3];
protected:

};
#endif