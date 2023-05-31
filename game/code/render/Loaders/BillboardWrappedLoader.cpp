//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        BillboardWrappedLoader.cpp
//
// Description: Implementation for RenderManager class.
//
// History:     Implemented	                         --Devin [4/22/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/inventory.hpp>

//========================================
// Project Includes
//========================================
#include <raddebug.hpp>
#include <render/Loaders/BillboardWrappedLoader.h>
#include <render/DSG/StaticEntityDSG.h>
#include <constants/srrchunks.h>
#include <memory/srrmemory.h>
//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

bool BillboardWrappedLoader::mOverrideWrapper = false;

//************************************************************************
//
// Public Member Functions : BillboardWrappedLoader Interface
//
//************************************************************************

//========================================================================
// BillboardWrappedLoader::BillboardWrappedLoader
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
BillboardWrappedLoader::BillboardWrappedLoader()
{
   mpListenerCB  = NULL;
   mUserData     = -1;
}

//========================================================================
// BillboardWrappedLoader::SetRegdListener
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
void BillboardWrappedLoader::SetRegdListener
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
// BillboardWrappedLoader::ModRegdListener
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
void BillboardWrappedLoader::ModRegdListener
(
   ChunkListenerCallback* pListenerCB,
   int iUserData
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "BillboardWrappedLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}

//************************************************************************
//
// Protected Member Functions : BillboardWrappedLoader
//
//************************************************************************

//========================================================================
// BillboardWrappedLoader::LoadObject
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
tEntity* BillboardWrappedLoader::LoadObject
(
	tChunkFile* file,
	tEntityStore* store
)
{
    IEntityDSG::msDeletionsSafe=true;
	tBillboardQuadGroup* pBQGroup = (tBillboardQuadGroup*)tBillboardQuadGroupLoader::LoadObject( file, store );

	rAssert(pBQGroup);

	if ( mOverrideWrapper )
	{
	   return pBQGroup;
	}

	// TBJ: Hack until Devin gets back.
	// Just store the tBillboardQuadGroup in the inventory (ie, make it act like a regular
	// loader).  The problem is that we will also create a StaticEntityDSG (at the origin?)
	// but that is not a huge cost, and it will work for now.
	//
	tEntity* t = pBQGroup;

	if(!t)
		return 0;
  
	if( store->TestCollision( t->GetUID(), t ) )
	{
		HandleCollision(t);
	}
	else
	{
		store->Store(t);
	}

	// End hack.
	//
	StaticEntityDSG* pStaticEntity = new StaticEntityDSG;
	pStaticEntity->SetName( pBQGroup->GetName() );
	const char* name = pStaticEntity->GetName();

	// Billboards anre't always translucent
    // This can happen when they are using alpha test to carve out the opaque sections
    // check the shader
    tShader* shader = pBQGroup->GetShader();
    if ( shader->mTranslucent )
    {
        pStaticEntity->mTranslucent = true;
    }
    else
    {
        pStaticEntity->mTranslucent = false;
    }

	pStaticEntity->SetDrawable(pBQGroup);

	//
	// _id is from SimpleChunkHandler; it is the chunk id;
	// however, this should be treated like any other StaticEntity,
	// so we change its return type _id
	//
	mpListenerCB->OnChunkLoaded( pStaticEntity, mUserData, SRR2::ChunkID::ENTITY_DSG );//_id );

    IEntityDSG::msDeletionsSafe=false;
	return pStaticEntity;
}

//************************************************************************
//
// Private Member Functions : BillboardWrappedLoader
//
//************************************************************************
