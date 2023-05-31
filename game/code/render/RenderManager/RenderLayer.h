#ifndef __RENDER_LAYER_H__
#define __RENDER_LAYER_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   RenderLayer 
//
// Description: The RenderLayer Model provides all interfaces 
//              neccessary for interaction with the render resources
//              associated with a given Layer (to be rendered in a sorted 
//              order and composited in said order)     
//
// History:     + Implemented Initial interfaces      -- Devin [4/18/2002]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/Culling/UseArray.h>
#include <render/Culling/SwapArray.h>
#include <constants/maxplayers.h>
//#include <render/DSG/IntersectDSG.h>
//#include <render/DSG/StaticEntityDSG.h>
//#include <render/DSG/StaticPhysDSG.h>
//#include <render/Culling/SpatialTree.h>

class IEntityDSG;
class IntersectDSG;
class StaticEntityDSG;
class StaticPhysDSG;
class SpatialTree;
class FenceEntityDSG;
class AnimCollisionEntityDSG;
class AnimEntityDSG;
class DynaPhysDSG;
class TriggerVolume;
class WorldSphereDSG;
class RoadSegment;
class PathSegment;
class tCamera;
class tDrawable;
class tGeometry;
class tMultiController;
class tName;
class tView;

namespace Scrooby
{
    class App;
}

//=================================================
// TODOs: Re-encapsulate (into other file(s)) the
//        data-sets below:
//=================================================

//========================================================================
//
// Synopsis:   The RenderLayer ; 
//                   -A simple datastructure for collecting a set of Layer
//                   resources in a single container for ease of 
//                   management                       --Devin  [4/18/2002]
//
//========================================================================
class RenderLayer 
{
public:
   RenderLayer();
   ~RenderLayer();

   ///////////////////////////////////////////////////////////////////////
   // Render Interface
   ///////////////////////////////////////////////////////////////////////
   virtual void Render();

   ///////////////////////////////////////////////////////////////////////
   // Resource Interfaces
   ///////////////////////////////////////////////////////////////////////
   // Setup all components to their default initial states
   void DoAllSetups();
   // Guts; Renderable Type Things
   virtual void   AddGuts( tDrawable* ipDrawable );
   virtual void   AddGuts( tGeometry* ipGeometry );
   virtual void   AddGuts( IntersectDSG* ipIntersectDSG );
   virtual void   AddGuts( StaticEntityDSG* ipStaticEntityDSG );
   virtual void   AddGuts( StaticPhysDSG* ipStaticPhysDSG );
   virtual void   AddGuts( FenceEntityDSG* ipFenceEntityDSG );
   virtual void   AddGuts( Scrooby::App* ipScroobyApp );
   virtual void   AddGuts( SpatialTree* ipSpatialTree );
   virtual void   AddGuts( AnimCollisionEntityDSG* ipAnimCollDSG );
   virtual void   AddGuts( AnimEntityDSG* ipAnimDSG );
   virtual void   AddGuts( DynaPhysDSG* ipDynaPhysDSG );
   virtual void   AddGuts( TriggerVolume* ipTriggerVolume );
   virtual void   AddGuts( WorldSphereDSG* ipWorldSphere );
   virtual void   AddGuts( RoadSegment* ipRoadSegment );
   virtual void   AddGuts( PathSegment* ipPathSegment );
   virtual void   RemoveGuts( tDrawable* ipDrawable );
   virtual void   RemoveGuts( IEntityDSG* ipEDSG );
   virtual void	  RemoveGuts( tMultiController* ipZoneController );

   virtual void   SetUpGuts();
   virtual void   NullifyGuts();

   bool HasGuts( tDrawable* guts );

   // Other; Other Layer Resources of interest
   // NOTE: if you want the Cam, you can & should get it through the 
   // SuperCam system, unless you're Cary.
   tCamera*       pCam( unsigned int viewIndex );
   tView*         pView( unsigned int viewIndex );
   float&         rAlpha();
   void           SetUpViewCam();
   void           NullifyViewCam();
   // Load Related interfaces
   virtual void   DoPreStaticLoad();
   virtual void   DoPostStaticLoad();
   virtual void   DumpAllDynaLoads(unsigned int start, SwapArray<tRefCounted*>& irEntityDeletionList);   
   virtual void   DumpDynaLoad(tName& irGiveItAFuckinName, SwapArray<tRefCounted*>& irEntityDeletionList);
   virtual bool   DoPreDynaLoad(tName& irGiveItAFuckinName);
   virtual void   DoPostDynaLoad();

   ///////////////////////////////////////////////////////////////////////
   //Exported Class/State Manipulators
   ///////////////////////////////////////////////////////////////////////
   void           Kill();        // RenderReady||Frozen >> Dead
   void		      Resurrect();   // Dead                >> RenderReady
   void           FreezeCorpse();// Dead                >> Frozen 
   void           Freeze();      // RenderReady         >> Frozen 
   void		      Thaw();        // Frozen              >> RenderReady
   void           Chill( void ); // To frozen, but remembers old state.
   void           Warm( void );  // Restores state remember by Chill.

   ///////////////////////////////////////////////////////////////////////
   //Exported Class/State
   ///////////////////////////////////////////////////////////////////////
   enum eExportedState
   {
      msDead,
      msFrozen,
      msRenderReady
   };
   eExportedState mExportedState;
   eExportedState mPreviousState;
   bool IsDead();
   bool IsFrozen();
   bool IsRenderReady();

   void SetNumViews( unsigned int numViews ) { mNumViews = numViews; }
   unsigned int GetNumViews() { return( mNumViews ); }
   void SetBeginView( bool BeginView ) { mIsBeginView = BeginView; }
   bool IsBeginView( void ) const { return mIsBeginView; }

protected:
   virtual bool IsGutsSetup();
   virtual bool IsViewCamSetup( unsigned int viewIndex );
   //Called by constructor
   void OnRenderLayerInit();

   //Static Data
   enum
   {
      msMaxGuts=10
   };

   //Member data
   tView*        mpView[ MAX_PLAYERS ];
   float         mAlpha;

   SwapArray<tDrawable*> mpGuts;

   bool mIsBeginView : 1;
    unsigned int mNumViews;
};

#endif

