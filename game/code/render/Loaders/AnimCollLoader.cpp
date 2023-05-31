//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        AnimCollLoader.cpp
//
// Description: Implementation for AnimCollLoader class.
//
// History:     Implemented	                         --Devin [5/27/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/chunkfile.hpp>
#include <p3d/anim/animate.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/inventory.hpp>
#include <simcollision/collisionobject.hpp>
#include <render/Loaders/BillboardWrappedLoader.h>
//========================================
// Project Includes
//========================================
#include <render/Loaders/AnimCollLoader.h>
#include <render/DSG/AnimCollisionEntityDSG.h>

#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>
#include <constants/chunks.h>
#include <memory/srrmemory.h>

#include <render/Loaders/AllWrappers.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : AnimCollLoader Interface
//
//************************************************************************
//========================================================================
// AnimCollLoader::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
AnimCollLoader::AnimCollLoader() :
tSimpleChunkHandler(SRR2::ChunkID::ANIM_COLL_DSG)
{
MEMTRACK_PUSH_GROUP( "AnimCollLoader" );
    mpCompDLoader = new(GMA_PERSISTENT) tCompositeDrawableLoader;
    mpCompDLoader->AddRef();

    mpFCLoader    = new(GMA_PERSISTENT) tFrameControllerLoader;
    mpFCLoader->AddRef();
    
    mpCollObjLoader = new (GMA_PERSISTENT) sim::CollisionObjectLoader;
    mpCollObjLoader->AddRef();

    mpMultiControllerLoader = new (GMA_PERSISTENT) tMultiControllerLoader;
    mpMultiControllerLoader->AddRef();
    mpListenerCB  = NULL;
    mUserData     = -1;
MEMTRACK_POP_GROUP( "AnimCollLoader" );
}
//========================================================================
// AnimCollLoader::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
AnimCollLoader::~AnimCollLoader()
{
    mpCompDLoader->Release();
    mpFCLoader->Release();
    mpCollObjLoader->Release();
    tRefCounted::Release( mpMultiControllerLoader );
}
///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// AnimCollLoader::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
tEntity* AnimCollLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char name[255];
    f->GetPString(name);

    int version = f->GetLong();
    int HasAlpha = f->GetLong();

    AnimCollisionEntityDSG *pAnimCollDSG = new AnimCollisionEntityDSG;
    pAnimCollDSG->SetName(name);

    if(HasAlpha)
    {
        pAnimCollDSG->mTranslucent = true;
    }
    
    tCompositeDrawable* pCompD = NULL;
    tMultiController* pAnimFC = NULL;
    sim::CollisionObject* pCollObject = NULL;
    while(f->ChunksRemaining())
    {    
        // Use this for tEntity's that have dependencies with other objects.
        // ie, they need to be in the inventory.
        //
        tEntity* pStoreMe = 0;
        f->BeginChunk();
        switch(f->GetCurrentID())
        {
            case P3D_COMPOSITE_DRAWABLE:
            {
                pCompD = (tCompositeDrawable*)mpCompDLoader->LoadObject(f,store);
                pStoreMe = pCompD;
                break;
            }
			case Pure3D::BillboardObject::QUAD_GROUP:
			{

				BillboardWrappedLoader::OverrideLoader( true );
                BillboardWrappedLoader* pBBQLoader = static_cast<BillboardWrappedLoader*>(AllWrappers::GetInstance()->mpLoader(AllWrappers::msBillboard));
                tBillboardQuadGroup* pGroup = static_cast<tBillboardQuadGroup*>( pBBQLoader->LoadObject(f, store) );
				rAssert( pGroup != NULL );
				store->Store( pGroup );	
				BillboardWrappedLoader::OverrideLoader( false );
				break;
			}
            case P3D_MULTI_CONTROLLER:
            {
                pAnimFC = (tMultiController*)mpMultiControllerLoader->LoadObject(f,store);
                break;
            }
            case Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER:
            {
                pStoreMe = mpFCLoader->LoadObject(f,store);
                break;
            }
            case Simulation::Collision::OBJECT:
                {
                    pCollObject = (sim::CollisionObject*)mpCollObjLoader->LoadObject(f, store );
                    break;
                }
            default:
                break;
        } // switch
        // This tEntity needs to be found in the inventory for another object.
        //
        if ( pStoreMe != 0 )
        {
            if( store->TestCollision( pStoreMe->GetUID(), pStoreMe ) )
            {
                HandleCollision( pStoreMe );
            }
            else
            {
                store->Store( pStoreMe );
            }
        }
        f->EndChunk();
    } // while

    pAnimCollDSG->LoadSetUp(pCompD,pAnimFC,pCollObject,store);

    mpListenerCB->OnChunkLoaded( pAnimCollDSG, mUserData, _id );

    IEntityDSG::msDeletionsSafe=false;
    //
    // Spin Pure3D async loading.
    //
    //p3d::loadManager->SwitchTask();
    return pAnimCollDSG;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// AnimCollLoader::SetRegdListener
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
void AnimCollLoader::SetRegdListener
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
// AnimCollLoader::ModRegdListener
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
void AnimCollLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "AnimCollLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}
//************************************************************************
//
// Protected Member Functions : AnimCollLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : AnimCollLoader 
//
//************************************************************************
