#ifndef __StaticEntityLoader_H__
#define __StaticEntityLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StaticEntityLoader
//
// Description: The StaticEntityLoader does STUFF
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

//========================================================================
//
// Synopsis:   The StaticEntityLoader; Synopsis by Inspection.
//
//========================================================================
class StaticEntityLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   StaticEntityLoader();

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
};


#endif
