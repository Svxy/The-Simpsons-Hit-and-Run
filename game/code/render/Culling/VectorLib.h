#ifndef __VECTOR_LIB_H__
#define __VECTOR_LIB_H__

#include <render/culling/Vector3f.h>
#include <render/culling/FixedArray.h>
#include <render/culling/Bounds.h>

///////////////////////////////////////////////////
// Originally, I was going to make this a bunch 
// of functions, that operate on lists of Vectors.
//
// However, I decided to put them into a class in
// case I need some persistent state further down 
// the road.
///////////////////////////////////////////////////
class VectorLib
{
public:
   VectorLib();
   ~VectorLib();

   void FindBounds( Bounds3f& orBounds, FixedArray<Vector3f>& irPoints );

protected:
};

VectorLib& theVectorLib();

#endif