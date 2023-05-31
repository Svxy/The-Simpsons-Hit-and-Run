#ifndef __BillboardWrappedLoader_H__
#define __BillboardWrappedLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   BillboardWrappedLoader
//
// Description: The BillboardWrappedLoader does STUFF
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
#include <p3d/billboardobject.hpp>

//========================================================================
//
// Synopsis:   The BillboardWrappedLoader; Synopsis by Inspection.
//
//========================================================================
class BillboardWrappedLoader 
: public tBillboardQuadGroupLoader,
  public IWrappedLoader 
{
public:
   BillboardWrappedLoader();
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   void SetRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

   void ModRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );


   ///////////////////////////////////////////////////////////////////////
   // tBillboardLoader
   ///////////////////////////////////////////////////////////////////////
   virtual tEntity* LoadObject(tChunkFile* file, tEntityStore* store);

   static void OverrideLoader( bool override ) { mOverrideWrapper = override; };

protected:

   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;
private:

    static bool mOverrideWrapper;
};

#endif