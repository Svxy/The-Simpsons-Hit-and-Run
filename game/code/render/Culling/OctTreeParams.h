#ifndef __OCT_TREE_PARAMS_H__
#define __OCT_TREE_PARAMS_H__

#include <p3d/p3dtypes.hpp>
#include <render/Culling/Bounds.h>

//
// OctTreeParams:
//
// Contains:
//    -All data necessary for which only a single instance is needed 
//       for the persistence of an OctTree
//
class OctTreeParams
{
public:
   OctTreeParams();
   ~OctTreeParams();

   //
   // The lowest possible value boundary at which space can be divided 
   //
   float mSpaceGranularity;
   //
   // The extents of the world being bound by the OctTree (X,Y,Z)
   //
   rmt::Box3D  mBounds; 
protected:
};

#endif