#ifndef __WorldRenderLayer_H__
#define __WorldRenderLayer_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   WorldRenderLayer
//
// Description: The WorldRenderLayer does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/04/23]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/RenderManager/RenderLayer.h>
#include <render/Culling/WorldScene.h>
#include <render/DSG/DynaLoadListDSG.h>
#include <raddebugwatch.hpp>

//class tShadowGenerator;    // VolShadows

class ZoneAnimationController;

//========================================================================
//
// Synopsis:   The WorldRenderLayer; Synopsis by Inspection.
//
//========================================================================
class WorldRenderLayer : public RenderLayer
{
public:
   WorldRenderLayer();
   ~WorldRenderLayer();

   // Render Interface
   virtual void Render();


   // Resource Interface
   virtual void   AddGuts( IntersectDSG* ipIntersectDSG );
   virtual void   AddGuts( StaticEntityDSG* ipStaticEntityDSG );
   virtual void   AddGuts( StaticPhysDSG* ipStaticPhysDSG );
   virtual void   AddGuts( FenceEntityDSG* ipFenceEntityDSG );
   virtual void   AddGuts( SpatialTree* ipSpatialTree );
   virtual void   AddGuts( AnimCollisionEntityDSG* ipAnimCollDSG );
   virtual void   AddGuts( AnimEntityDSG* ipAnimDSG );
   virtual void   AddGuts( DynaPhysDSG* ipDynaPhysDSG );
   virtual void   AddGuts( TriggerVolume* ipTriggerVolume );
   virtual void   AddGuts( WorldSphereDSG* ipWorldSphere );
   virtual void   AddGuts( RoadSegment* ipRoadSegment );
   virtual void   AddGuts( PathSegment* ipPathSegment );
   virtual void	  RemoveGuts( IEntityDSG* ipEDSG );


   void ActivateWS(tUID iUID);
   void DeactivateWS(tUID iUID);
      
   //virtual void   AddGuts( tDrawable* ipDrawable );
   virtual void   SetUpGuts();
   virtual void   NullifyGuts();

   // Load Related interfaces
   virtual void   DoPreStaticLoad();
   virtual void   DoPostStaticLoad();
   virtual void   DumpAllDynaLoads(  unsigned int start, SwapArray<tRefCounted*>& irEntityDeletionList);
   virtual void   DumpDynaLoad(tName& irGiveItAFuckinName, SwapArray<tRefCounted*>& irEntityDeletionList);
   virtual bool   DoPreDynaLoad(tName& irGiveItAFuckinName);
   virtual void   DoPostDynaLoad();

   WorldScene* pWorldScene();

   tName& GetCurSectionName();

   enum DynaLoadState
   {
       msPreLoads,
       msNoLoad,
       msLoad,
       msIgnoreLoad
   };

   DynaLoadState GetCurrentState() const { return mDynaLoadState; };


   void SetMirror(bool enable, rmt::Matrix* matrix) { mMirror = enable; if(matrix) mMirrorMatrix = *matrix; }

protected:

   void   DumpAllDynaLoads();
   void   DumpDynaLoad(tName& irGiveItAFuckinName);
   virtual bool IsGutsSetup();
   //Called by constructor
   void OnWorldRenderLayerInit();

   WorldScene* mpWorldScene;

   ///////////////////////////////////////////////////////////////////////
   // Dynamic Loading Control Stuctures
   ///////////////////////////////////////////////////////////////////////
   SwapArray<WorldSphereDSG*>  mWorldSpheres;
   SwapArray<DynaLoadListDSG>  mStaticLoadLists;
   SwapArray<DynaLoadListDSG*> mLoadLists;
   int                         mnLoadListRefs;
   int                         mCurLoadIndex;
   
#ifdef DEBUGWATCH
   unsigned int mDebugRenderTime, mDebugInnerRenderTime, mDebugGutsTime;
#endif

   DynaLoadState mDynaLoadState;
   tUID          mCurLoadUID;
   
   //tShadowGenerator* mpShadowGenerator;    // VolShadows


   //These will handle deletions during a load
   bool mQdDump;
   unsigned int mQdDeletionStart;
   SwapArray<tRefCounted*>* mpQdDeletionList;

   bool mMirror;
   rmt::Matrix mMirrorMatrix;
};

#endif
