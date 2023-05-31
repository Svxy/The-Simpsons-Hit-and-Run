#ifndef __DynaLoadListDSG_H__
#define __DynaLoadListDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   DynaLoadListDSG
//
// Description: The DynaLoadListDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/06/26]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <memory/srrmemory.h>


//=================================================
// Project Includes
//=================================================

//========================================================================
//
// Synopsis:   The DynaLoadListDSG; Synopsis by Inspection.
//
//========================================================================
class DynaLoadListDSG 
{
public:
   DynaLoadListDSG(){}
   ~DynaLoadListDSG(){}

   void ClearAll()
   {
      mWorldSphereElems.Clear();
      mSEntityElems.Clear(); 
      mSPhysElems.Clear();   
      mIntersectElems.Clear();
      mDPhysElems.Clear();
      mFenceElems.Clear();
      mAnimCollElems.Clear();
      mAnimElems.Clear();
      mTrigVolElems.Clear();
      mRoadSegmentElems.Clear();
      mPathSegmentElems.Clear();

   }

   void ClearAllUse()
   {
      mWorldSphereElems.ClearUse();
      mSEntityElems.ClearUse(); 
      mSPhysElems.ClearUse();   
      mIntersectElems.ClearUse();
      mDPhysElems.ClearUse();
      mFenceElems.ClearUse();
      mAnimCollElems.ClearUse();
      mAnimElems.ClearUse();
      mTrigVolElems.ClearUse();
      mRoadSegmentElems.ClearUse();
      mPathSegmentElems.ClearUse();

   }

   void AllocateAll(int inSize)
   {
      MEMTRACK_PUSH_GROUP( "Devin's world" );       
      mWorldSphereElems.Allocate(2);
      mSEntityElems.Allocate(inSize); 
      mSPhysElems.Allocate(inSize/2);   
      mIntersectElems.Allocate(inSize/4);
      mDPhysElems.Allocate(inSize);
      mFenceElems.Allocate(inSize);
      mAnimCollElems.Allocate(inSize/4);
      mAnimElems.Allocate(inSize/4);
      mTrigVolElems.Allocate(inSize/4);
      mRoadSegmentElems.Allocate(1250);
      mPathSegmentElems.Allocate(inSize);
      MEMTRACK_POP_GROUP( "Devin's world" );
   }

   tName mGiveItAFuckinName;
   
   SwapArray<WorldSphereDSG*>            mWorldSphereElems; 
   SwapArray<StaticEntityDSG*>           mSEntityElems; 
   SwapArray<StaticPhysDSG*>             mSPhysElems;   
   SwapArray<IntersectDSG*>              mIntersectElems;
   SwapArray<DynaPhysDSG*>               mDPhysElems;
   SwapArray<FenceEntityDSG*>            mFenceElems;
   SwapArray<AnimCollisionEntityDSG*>    mAnimCollElems;
   SwapArray<AnimEntityDSG*>             mAnimElems;
   SwapArray<TriggerVolume*>             mTrigVolElems;
   SwapArray<RoadSegment*>               mRoadSegmentElems;
   SwapArray<PathSegment*>               mPathSegmentElems;

private:
};

#endif
