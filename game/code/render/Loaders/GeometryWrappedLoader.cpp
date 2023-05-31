//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        GeometryWrappedLoader.cpp
//
// Description: Implementation for RenderManager class.
//
// History:     Implemented	                         --Devin [4/22/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <raddebug.hpp>
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/DSG/IEntityDSG.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : GeometryWrappedLoader Interface
//
//************************************************************************

//========================================================================
// GeometryWrappedLoader::GeometryWrappedLoader 
//========================================================================
//
// Description: Init members to NULL 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
GeometryWrappedLoader::GeometryWrappedLoader()
{
   mpListenerCB  = NULL;
   mUserData     = -1;
}

//========================================================================
// GeometryWrappedLoader::SetRegdListener
//========================================================================
//
// Description: Register a new listener/caretaker, notify old listener of 
//              severed connection.
//
// Parameters:  pListenerCB: Callback to call OnChunkLoaded
//              pUserData:   Data to pass along for filtering, etc
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void GeometryWrappedLoader::SetRegdListener
(
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
   //
   // Follow protocol; notify old Listener, that it has been 
   // "disconnected".
   //
   if( mpListenerCB != NULL )
   {
      mpListenerCB->OnChunkLoaded( NULL, iUserData, 0 );
   }
   mpListenerCB  = pListenerCB;
   mUserData     = iUserData;
}

//========================================================================
// GeometryWrappedLoader::ModRegdListener
//========================================================================
//
// Description: Just fuck with the current pUserData
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void GeometryWrappedLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "GeometryWrappedLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}

//************************************************************************
//
// Protected Member Functions : GeometryWrappedLoader 
//
//************************************************************************

//========================================================================
// GeometryWrappedLoader::LoadObject
//========================================================================
//
// Description: LoadObject, call the old LoadObject, wrapping the call 
//              with a Listener Notification.
//
// Parameters:  file: ChunkFile handle
//              store: where you put the damn thing
//
// Return:      tEntity; the new thing you create on parse.
//
// Constraints: None.
//
//========================================================================
tEntity* GeometryWrappedLoader::LoadObject
(
   tChunkFile* file, 
   tEntityStore* store
)
{
   IEntityDSG::msDeletionsSafe=true;
   tEntity* pEntity = tGeometryLoader::LoadObject( file, store );

   //
   // _id is from SimpleChunkHandler; it is the chunk id
   //
   mpListenerCB->OnChunkLoaded( pEntity, mUserData, _id );

   IEntityDSG::msDeletionsSafe=false;
   return pEntity;
}

//************************************************************************
//
// Private Member Functions : GeometryWrappedLoader 
//
//************************************************************************
