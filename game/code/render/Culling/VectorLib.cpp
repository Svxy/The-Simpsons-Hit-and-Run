#include <render/culling/VectorLib.h>

VectorLib& theVectorLib()
{
   static VectorLib tVL;

   return tVL;
}


VectorLib::VectorLib()
{
}
   
VectorLib::~VectorLib()
{
}

void VectorLib::FindBounds( Bounds3f& orBounds, FixedArray<Vector3f>& irPoints )
{
   int i;
   orBounds.mMin = irPoints[0];
   orBounds.mMax = irPoints[0];

   for( i=1; i<irPoints.mSize; i++ )
   {
      orBounds.Accumulate( irPoints[i] );
   }
}


