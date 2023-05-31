//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InstStatPhysLoader.cpp
//
// Description: Implementation for InstStatPhysLoader class.
//
// History:     Implemented	                         --Devin [5/27/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <simcollision/collisionobject.hpp>
#include <p3d/chunkfile.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <render/Loaders/InstStatPhysLoader.h>
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/Loaders/AllWrappers.h>
#include <render/DSG/InstStatPhysDSG.h>
#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>
#include <memory/srrmemory.h>
#include <atc/atcmanager.h>
#include <constants/blobshadownames.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : InstStatPhysLoader Interface
//
//************************************************************************
//========================================================================
// InstStatPhysLoader::
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
InstStatPhysLoader::InstStatPhysLoader() :
tSimpleChunkHandler(SRR2::ChunkID::INSTA_STATIC_PHYS_DSG)
{
    mpCollObjLoader = new(GMA_PERSISTENT) sim::CollisionObjectLoader();
    mpCollObjLoader->AddRef();

    mpListenerCB  = NULL;
    mUserData     = -1;
}
//========================================================================
// InstStatPhysLoader::
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
InstStatPhysLoader::~InstStatPhysLoader()
{
    mpCollObjLoader->Release();
}

///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatPhysLoader::
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
tEntity* InstStatPhysLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char name[128];
    char objName[128];
    f->GetPString(objName);

    int version = f->GetLong();
    int HasAlpha = f->GetLong();

	// Lets see if theis object has a shadow associated with it
	const char* pShadowName = BlobbyShadowNames::FindShadowName( objName );
	tDrawable* pShadow;
	if ( pShadowName != NULL )
	{
		pShadow = p3d::find< tDrawable > ( pShadowName );
	}
	else
	{
		pShadow = NULL;
	}

    sim::CollisionObject*   pCollObj    = NULL;
    tGeometry*              pGeo        = NULL;
    sim::SimState*          pSimState   = NULL;

    sim::PhysicsObject*     pPhysObj    = NULL;

    InstStatPhysDSG* pCurStatPhysDSG = static_cast<InstStatPhysDSG*>(GetAllWrappers()->GetGlobalEntity(tName::MakeUID(objName)));

    CollisionAttributes*    pCollAttr   = NULL;

    //new InstDynaPhysDSG;

    bool foundInstances = false;

    while(f->ChunksRemaining())
    {      
        f->BeginChunk();
        switch(f->GetCurrentID())
        {
        case SRR2::ChunkID::INSTANCES:
            {
                foundInstances = true;
                //Instances >> Scenegraph
                f->BeginChunk();
                //Scenegraph >> ScenegraphRoot
                f->BeginChunk();
                //ScenegraphRoot >> ScenegraphBranch
                f->BeginChunk();
                //ScenegraphBranch >> ScenegraphTransform
                f->BeginChunk();

                //ScenegraphTransform >> real ScenegraphTransform
                //f->BeginChunk();

                for(;f->ChunksRemaining();)
                {
                    f->BeginChunk();
                    
                    f->GetPString(name);
                    int numChild = f->GetLong();

                    rmt::Matrix matrix;
                    f->GetData(&matrix,16,tFile::DWORD);

                    if( pCurStatPhysDSG )
                    {
                        // We are dealing with a global entity.
                        InstStatPhysDSG* clone = pCurStatPhysDSG->Clone(name, matrix);
                        mpListenerCB->OnChunkLoaded( clone, mUserData, _id );
                    }
                    else
                    {
                        pCurStatPhysDSG = new InstStatPhysDSG();
                        rAssert(pCurStatPhysDSG);
                        pCurStatPhysDSG->SetName(name);

                        if(HasAlpha)
                        {
                            pCurStatPhysDSG->mTranslucent = true;
                        }
                        pSimState = sim::SimState::CreateStaticSimState(pCollObj);
                        pSimState->SetControl(sim::simAICtrl);
                        pSimState->GetCollisionObject()->SetIsStatic(false);
                        pSimState->GetCollisionObject()->SetManualUpdate(false);
                        pSimState->SetTransform(matrix); 
                        pSimState->GetCollisionObject()->Update();      
                        pSimState->GetCollisionObject()->SetIsStatic(true);
                        pSimState->GetCollisionObject()->SetManualUpdate(true);
                        pCurStatPhysDSG->LoadSetUp(pSimState,pCollAttr,matrix,pGeo);
                        if ( pShadow != NULL )
                        {
                            pCurStatPhysDSG->SetShadow( pShadow );
                        }
                        mpListenerCB->OnChunkLoaded( pCurStatPhysDSG, mUserData, _id );
                        pCurStatPhysDSG = 0;
                    }
                    f->EndChunk();
                }
                //ScenegraphBranch >> ScenegraphTransform
                f->EndChunk();
                //ScenegraphRoot >> ScenegraphBranch
                f->EndChunk();
                //Scenegraph >> ScenegraphRoot
                f->EndChunk();
                //Instances >> Scenegraph
                f->EndChunk();
            }
            break;

        case Pure3D::Mesh::MESH:
            {
                if(!pCurStatPhysDSG)
                {
                    tRefCounted::Assign(pGeo,(tGeometry*)((GeometryWrappedLoader*)GetAllWrappers()->mpLoader(AllWrappers::msGeometry))->LoadObject(f,store));
                }
            }
            break;


        case SRR2::ChunkID::OBJECT_ATTRIBUTES:
            {
                if(!pCurStatPhysDSG)
                {
                    int classType = f->GetLong();
                    int physPropID = f->GetLong();
                    char tempsound [64];

                    f->GetString(tempsound);

                    // Michael R. Volume set to zero. I only need the information
                    // from physPropID to break particles and breakables
                    // will Greg need this later?
                    tRefCounted::Assign(pCollAttr,GetATCManager()->CreateCollisionAttributes(classType, physPropID, 0.0f));
                    pCollAttr->SetSound(tempsound);
                }
            }
            break;

        case Simulation::Collision::OBJECT:
            {
                if(!pCurStatPhysDSG)
                {
                    tRefCounted::Assign(pCollObj,(sim::CollisionObject*)mpCollObjLoader->LoadObject(f, store));
                    // TBJ [7/9/2002]
                    // Added this to store collision objects in the inventory.
                    // A normal loader stores the top level chunk, in this case the StaticPhysDSG.
                    // Since we want the Collision Object in the inventory, we have to store it here.
                    //
                    if ( pCollObj )
                    {
                        if( store->TestCollision( pCollObj->GetUID(), pCollObj ) )
                        {
                            HandleCollision( pCollObj );
                            // TBJ [7/9/2002]
                            // Don't know what to do with this code?  Doesn't seem right to assign to NULL if
                            // the collObj does in fact exist.
                            // I'll take it out for now.
                            //
                            //pCollObj = NULL;
                            //return LOAD_ERROR;
                        }
                        else
                        {
                            store->Store(pCollObj);
                        }                
        
                        // do this so that we can move the simstate around with ai
                        //pCollObj->SetManualUpdate(true); 

                    }
                    //pSimState = sim::SimState::CreateSimState(pCollObj);
                    //pStaticPhysDSG->SetSimState(sim::SimState::CreateStaticSimState(pCollObj));
                }
            }
            break;

        default:
            break;
        } // switch
        f->EndChunk();
    } // while

    if(!foundInstances)
    {
        rmt::Matrix matrix;
        matrix.Identity();
        // This is an instanced object without an instance chunks so we assume
        //it's a global asset which we will hold only one copy of.
        pCurStatPhysDSG = new InstStatPhysDSG();
        rAssert(pCurStatPhysDSG);
        pCurStatPhysDSG->SetName(objName);
        pCurStatPhysDSG->mTranslucent = HasAlpha != 0;
        pSimState = sim::SimState::CreateStaticSimState(pCollObj);
        pCurStatPhysDSG->SetShadow(pShadow);
        pSimState->SetControl(sim::simAICtrl);
        pSimState->GetCollisionObject()->SetIsStatic(false);
        pSimState->GetCollisionObject()->SetManualUpdate(false);
        pSimState->SetTransform(matrix); 
        pSimState->GetCollisionObject()->Update();      
        pSimState->GetCollisionObject()->SetIsStatic(true);
        pSimState->GetCollisionObject()->SetManualUpdate(true);
        pCurStatPhysDSG->LoadSetUp(pSimState,pCollAttr,matrix,pGeo);
        GetAllWrappers()->AddGlobalEntity(pCurStatPhysDSG);
    }

    //
    // Spin Pure3D async loading.
    //
    //p3d::loadManager->SwitchTask();

    tRefCounted::Release(pGeo);
    tRefCounted::Release(pCollAttr);
    tRefCounted::Release(pCollObj);

    IEntityDSG::msDeletionsSafe=false;
    return NULL;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatPhysLoader::SetRegdListener
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
void InstStatPhysLoader::SetRegdListener
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
// InstStatPhysLoader::ModRegdListener
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
void InstStatPhysLoader::ModRegdListener
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
// Protected Member Functions : InstStatPhysLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : InstStatPhysLoader 
//
//************************************************************************
