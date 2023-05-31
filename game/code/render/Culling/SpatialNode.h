#ifndef __SPATIAL_NODE_H__
#define __SPATIAL_NODE_H__

#include <render/culling/OctTreeNode.h>
#include <render/culling/Plane3f.h>
#include <render/culling/ContiguousBinNode.h>
#include <render/culling/SwapArray.h>
#include <render/culling/BoxPts.h>

//#include <render/DSG/StaticEntityDSG.h>
//#include <render/DSG/StaticPhysDSG.h>
//#include <render/DSG/IntersectDSG.h>

class StaticEntityDSG;
class StaticPhysDSG;
class IntersectDSG;
class DynaPhysDSG;
class FenceEntityDSG;
class AnimCollisionEntityDSG;
class TriggerVolume;
class RoadSegment;
class PathSegment;
class AnimEntityDSG;

//////////////////////////////////////////////////////////////////////////
//
// A SpatialNode is data 
//    usually attached to a ContiguousBinNode 
// with Spatial Capabilities encompassing the following:
//
// -A Subdivision Plane representing the plane which subdivides
//    the Space contained at the current node into two discrete
//    volumes contained in the child nodes
//

class SpatialNode 
{
public:
   SpatialNode();
   ~SpatialNode();

   //This plane should be move the the contig bin node, to improve cache hits
   AAPlane3f         mSubDivPlane;

   NodeSwapArray<StaticEntityDSG*>          mSEntityElems; //mSpatialElems;
   SwapArray<StaticPhysDSG*>                mSPhysElems;   //mDynamicElems;
   SwapArray<IntersectDSG*>                 mIntersectElems;
   NodeSwapArray<DynaPhysDSG*>              mDPhysElems;
   SwapArray<FenceEntityDSG*>               mFenceElems;
   NodeSwapArray<AnimCollisionEntityDSG*>   mAnimCollElems;
   NodeSwapArray<AnimEntityDSG*>            mAnimElems;
   SwapArray<TriggerVolume*>                mTrigVolElems;
   SwapArray<RoadSegment*>                  mRoadSegmentElems;
   SwapArray<PathSegment*>				    mPathSegmentElems;
   //Debug
   BoxPts   mBBox;

protected:
};


inline SpatialNode::SpatialNode(){}

inline SpatialNode::~SpatialNode(){}


#endif