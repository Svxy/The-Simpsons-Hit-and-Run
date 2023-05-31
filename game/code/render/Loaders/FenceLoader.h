#ifndef __FenceLoader_H__
#define __FenceLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   FenceLoader
//
// Description: The FenceLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/28]
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
// Synopsis:   The FenceLoader; Synopsis by Inspection.
//
//========================================================================
class FenceLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   FenceLoader();

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
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:

	static unsigned int mFenceCount;
};

#endif
