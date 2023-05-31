#ifndef LENSFLARELOADER_H
#define LENSFLARELOADER_H

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   LensFlareLoader
//
// Description: Loads a lensflaredsg chunk
//
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/IWrappedLoader.h>


class tCompositeDrawableLoader;
class tBillboardQuadGroupLoader;


class LensFlareLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   LensFlareLoader();
   virtual ~LensFlareLoader();
 
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
   tBillboardQuadGroupLoader*   mpBillBoardQuadLoader;

   // Iterates through the compdrawable, setting the first billboardquadgroup's
   // occlusion flag to 2
   // all subsequent ones to 1
   void SetOcclusionFlags( tCompositeDrawable* compDraw );

   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:
};


#endif
