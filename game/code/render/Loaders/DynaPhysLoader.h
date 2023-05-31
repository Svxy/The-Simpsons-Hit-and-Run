#ifndef __DynaPhysLoader_H__
#define __DynaPhysLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   DynaPhysLoader
//
// Description: The DynaPhysLoader does STUFF
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
// Synopsis:   The DynaPhysLoader; Synopsis by Inspection.
//
//========================================================================
class DynaPhysLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   DynaPhysLoader();
   virtual ~DynaPhysLoader();
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
   sim::PhysicsObjectLoader*   mpPhysObjLoader;
   tCompositeDrawableLoader*   mpCompDLoader;
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:
};

#endif
