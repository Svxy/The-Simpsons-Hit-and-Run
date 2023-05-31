#ifndef __AnimCollLoader_H__
#define __AnimCollLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AnimCollLoader
//
// Description: The AnimCollLoader does STUFF
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
namespace sim
{
    class CollisionObjectLoader;
}
class tFrameControllerLoader;
class tCompositeDrawableLoader;
class tMultiControllerLoader;
//========================================================================
//
// Synopsis:   The AnimCollLoader; Synopsis by Inspection.
//
//========================================================================
class AnimCollLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   AnimCollLoader();
   virtual ~AnimCollLoader();
 
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
   tCompositeDrawableLoader*    mpCompDLoader;
   tFrameControllerLoader*      mpFCLoader;
   tMultiControllerLoader*      mpMultiControllerLoader;
   sim::CollisionObjectLoader* mpCollObjLoader;
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:
};


#endif
