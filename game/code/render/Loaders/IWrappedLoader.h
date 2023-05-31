#ifndef __IWrappedLoader_H__
#define __IWrappedLoader_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   IWrappedLoader
//
// Description: The IWrappedLoader does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/04/24]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/Loaders/ChunkListenerCallback.h>
#include <p3d/loadmanager.hpp>

//========================================================================
//
// Synopsis:   The IWrappedLoader; Synopsis by Inspection.
//
//========================================================================
class IWrappedLoader //: public tSimpleChunkHandler
{
public:
   IWrappedLoader(){}
   virtual ~IWrappedLoader(){}

   virtual void SetRegdListener( ChunkListenerCallback* pListenerCB,
                                 int   iUserData ) = 0;

   virtual void ModRegdListener( ChunkListenerCallback* pListenerCB,
                                 int   iUserData ) = 0;


protected:
   ChunkListenerCallback*  mpListenerCB;
   int mUserData;

};

#endif
