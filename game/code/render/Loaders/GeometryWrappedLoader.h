#ifndef __GeometryWrappedLoader_H__
#define __GeometryWrappedLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GeometryWrappedLoader
//
// Description: The GeometryWrappedLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/04/21]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/ChunkListenerCallback.h>
#include <render/Loaders/IWrappedLoader.h>
#include <p3d/geometry.hpp>

//========================================================================
//
// Synopsis:   The GeometryWrappedLoader; Synopsis by Inspection.
//
//========================================================================
class GeometryWrappedLoader 
: public tGeometryLoader,
  public IWrappedLoader 
{
public:
   GeometryWrappedLoader();
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   void SetRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

   void ModRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );


   ///////////////////////////////////////////////////////////////////////
   // tGeometryLoader
   ///////////////////////////////////////////////////////////////////////
   virtual tEntity* LoadObject(tChunkFile* file, tEntityStore* store);

protected:

   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:
};

#endif