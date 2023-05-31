#ifndef __InstStatPhysLoader_H__
#define __InstStatPhysLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstStatPhysLoader
//
// Description: The InstStatPhysLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/28]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <simcollision/collisionobject.hpp>
#include <simphysics/physicsobject.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/IWrappedLoader.h>

//========================================================================
//
// Synopsis:   The InstStatPhysLoader; Synopsis by Inspection.
//
//========================================================================
class InstStatPhysLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   InstStatPhysLoader();
   virtual ~InstStatPhysLoader();
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
   sim::CollisionObjectLoader* mpCollObjLoader;

   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:
};

#endif
