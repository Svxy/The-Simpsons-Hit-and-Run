#ifndef __WorldSphereLoader_H__
#define __WorldSphereLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   WorldSphereLoader
//
// Description: The WorldSphereLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/27]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/IWrappedLoader.h>

class tMultiControllerLoader;
class tCompositeDrawableLoader;
class tBillboardQuadGroupLoader;
class tSkeletonLoader;
class tAnimationLoader;
class tFrameControllerLoader;
class LensFlareLoader;

//========================================================================
//
// Synopsis:   The WorldSphereLoader; Synopsis by Inspection.
//
//========================================================================
class WorldSphereLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   WorldSphereLoader();
   virtual ~WorldSphereLoader();
 
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   void SetRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

   void ModRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

   ///////////////////////////////////////////////////////////////////////
   // tSimpleChunkHandler
   ///////////////////////////////////////////////////////////////////////
   virtual tEntity* LoadObject(tChunkFile* file, tEntityStore* store);

protected:
   tCompositeDrawableLoader*	mpCompDLoader;
   tMultiControllerLoader*      mpMCLoader;
   tFrameControllerLoader*		mpFCLoader;
   tBillboardQuadGroupLoader*   mpBillBoardQuadLoader;
   tAnimationLoader*			mpAnimLoader;
   tSkeletonLoader*				mpSkelLoader;
   LensFlareLoader*				mpLensFlareLoader;
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:
};


#endif
