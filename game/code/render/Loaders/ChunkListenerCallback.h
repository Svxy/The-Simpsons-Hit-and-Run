#ifndef __ChunkListenerCallback_H__
#define __ChunkListenerCallback_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ChunkListenerCallback
//
// Description: The ChunkListenerCallback does STUFF
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
#include <p3d/entity.hpp>

//========================================================================
//
// Synopsis:   The ChunkListenerCallback; Synopsis by Inspection.
//
//========================================================================

class ChunkListenerCallback
{
public:
   //
   // Protocol: OnChunkLoaded is called as normal, EXCEPT when the 
   //           Listener's Registration is being cancelled.
   //           In this case OnChunkLoaded( NULL, pNewUserData, ipCUID ) 
   //           is called.
   //
   virtual void OnChunkLoaded(   tEntity* ipEntity, 
                                 int      iUserData,
                                 unsigned ipChunkID    ) = 0;        
};

#endif
