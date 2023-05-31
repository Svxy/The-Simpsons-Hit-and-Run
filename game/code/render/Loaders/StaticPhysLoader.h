#ifndef __StaticPhysLoader_H__
#define __StaticPhysLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StaticPhysLoader
//
// Description: The StaticPhysLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/28]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <simcollision/collisionobject.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/IWrappedLoader.h>

//========================================================================
//
// Synopsis:   The StaticPhysLoader; Synopsis by Inspection.
//
//========================================================================
class StaticPhysLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   StaticPhysLoader();
   virtual ~StaticPhysLoader();
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
