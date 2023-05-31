#ifndef __TreeDSGLoader_H__
#define __TreeDSGLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   TreeDSGLoader
//
// Description: The TreeDSGLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/06/10]
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
// Synopsis:   The TreeDSGLoader; Synopsis by Inspection.
//
//========================================================================
class TreeDSGLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   TreeDSGLoader();

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
};

#endif
