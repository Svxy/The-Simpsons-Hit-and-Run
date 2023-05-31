#ifndef __AllWrappers_H__
#define __AllWrappers_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AllWrappers
//
// Description: The AllWrappers does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/04/24]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/IWrappedLoader.h>

class tDrawable;

//========================================================================
//
// Synopsis:   The AllWrappers; Synopsis by Inspection.
//
//========================================================================
class AllWrappers 
{
public:
   // Static Methods (for creating, destroying and acquiring an instance 
   // of the RenderManager)
   static AllWrappers* CreateInstance();
   static AllWrappers* GetInstance();
   static void  DestroyInstance();

   enum
   {
      msGeometry,
      msStaticEntity,
      msStaticPhys,
      msTreeDSG,
      msFenceEntity,
      msIntersectDSG,
      msAnimCollEntity,
      msAnimEntity,
      msDynaPhys,
      msInstStatEntity,
      msInstStatPhys,
      msLocator,
      msWorldSphere,
      msRoadSegment,
	  msPathSegment,
      msBillboard,
      msInstParticleSystem,
      msBreakableObject,
	  msLensFlare,
	  msAnimDynaPhys,
	  msAnimDynaPhysWrapper,
      msNumWrappers
   };

   IWrappedLoader& mLoader(   int iIndex );
   IWrappedLoader* mpLoader(  int iIndex );

   // Global entities are loaded in the global art file (i.e. L1_TERRA.p3d) and
   //a single instance is created and held here. Whenever one is loaded in the
   //zone/rail files we check here first to see if we already have it. If so we
   //just use the instance chunk to place the object around the world and skip
   //loading the rest of the data. The idea is that we can have a single copy
   //of phonebooths, stop signs, lamp posts, etc instead of one in each on the
   //zone/rail inventories.
   void AddGlobalEntity(tDrawable* Entity);
   tDrawable* GetGlobalEntity(tUID EntityID) const;
   void ClearGlobalEntities(void);

protected:
   void CoupleAllLoaders();

   IWrappedLoader* mpLoaders[msNumWrappers];
   tDrawable** mpGlobalEntities;
   int mNumGlobalEntities;

private:
   AllWrappers();
   ~AllWrappers();

   static AllWrappers* mspInstance;
};

//
// A little syntactic sugar for getting at this singleton.
//
inline AllWrappers* GetAllWrappers() 
{ 
   return( AllWrappers::GetInstance() ); 
}

#endif
