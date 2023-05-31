#ifndef __IntersectLoader_H__
#define __IntersectLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   IntersectLoader
//
// Description: The IntersectLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/07/19]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/IWrappedLoader.h>

//========================================================================
//
// Synopsis:   The IntersectLoader; Synopsis by Inspection.
//
//========================================================================
class IntersectLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   IntersectLoader();

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

private:
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
};

#endif

