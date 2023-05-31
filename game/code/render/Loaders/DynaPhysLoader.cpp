//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        DynaPhysLoader.cpp
//
// Description: Implementation for DynaPhysLoader class.
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
#include <render/Loaders/DynaPhysLoader.h>
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/Loaders/AllWrappers.h>
#include <render/DSG/InstDynaPhysDSG.h>
#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>
#include <constants/blobshadownames.h>
#include <memory/srrmemory.h>

#include <atc/atcmanager.h>

#ifndef RAD_RELEASE
#include <memory/propstats.h>
#endif


//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************


//************************************************************************
//
// Public Member Functions : DynaPhysLoader Interface
//
//************************************************************************
//========================================================================
// DynaPhysLoader::
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
DynaPhysLoader::DynaPhysLoader() :
tSimpleChunkHandler(SRR2::ChunkID::DYNA_PHYS_DSG)
{
    HeapMgr()->PushHeap( GMA_PERSISTENT );

    mpCollObjLoader = new sim::CollisionObjectLoader();
    mpCollObjLoader->AddRef();

    mpPhysObjLoader = new sim::PhysicsObjectLoader();
    mpPhysObjLoader->AddRef();

    mpCompDLoader = new tCompositeDrawableLoader;
    mpCompDLoader->AddRef();

    HeapMgr()->PopHeap(GMA_PERSISTENT);

    mpListenerCB  = NULL;
    mUserData     = -1;
}
//========================================================================
// DynaPhysLoader::
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
DynaPhysLoader::~DynaPhysLoader()
{
    mpCollObjLoader->Release();
    mpPhysObjLoader->Release();
    mpCompDLoader->Release();
}

///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// DynaPhysLoader::
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
tEntity* DynaPhysLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char objName[128];
    char name[128];
    f->GetPString(objName);

#ifndef RAD_RELEASE
    PropStats::StartTracking( objName );
#endif

    bool test = tName::MakeUID(objName) == tName::MakeUID("l1_streetlamp_Shape");
    static bool doTestOnce = true;

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

    int instanceCount = 0;

    int version = f->GetLong();
    int HasAlpha = f->GetLong();
    //pDynaPhysDSG->SetName(name);
    
    sim::CollisionObject*   pCollObj    = NULL;
    sim::PhysicsObject*     pPhysObj    = NULL;
    tDrawable*              pDrawable   = NULL;
    sim::SimState*          pSimState   = NULL;
    CollisionAttributes*    pCollAttr   = NULL;
    bool foundInstances = false;

    InstDynaPhysDSG* pCurDynaPhysDSG = static_cast<InstDynaPhysDSG*>(GetAllWrappers()->GetGlobalEntity(tName::MakeUID(objName)));

    //new InstDynaPhysDSG;

    while(f->ChunksRemaining())
    {      
        f->BeginChunk();
        switch(f->GetCurrentID())
        {
        case SRR2::ChunkID::INSTANCES:
            {
                //Instances >> Scenegraph
                f->BeginChunk();
                //Scenegraph >> ScenegraphRoot
                f->BeginChunk();
                //ScenegraphRoot >> ScenegraphBranch
                f->BeginChunk();
                //ScenegraphBranch >> ScenegraphTransform
                f->BeginChunk();

                foundInstances = true;

                //ScenegraphTransform >> real ScenegraphTransform
                //f->BeginChunk();

                for(;f->ChunksRemaining();)
                {

                    instanceCount++;
                    f->BeginChunk();
                    
                    f->GetPString(name);
                    int numChild = f->GetLong();

                    rmt::Matrix matrix;
                    f->GetData(&matrix,16,tFile::DWORD);

                    if(pCurDynaPhysDSG == 0)
                    {
                        pCurDynaPhysDSG = new InstDynaPhysDSG();
                        rAssert(pCurDynaPhysDSG);
                        pCurDynaPhysDSG->SetName(name);
                        pCurDynaPhysDSG->mTranslucent = ( HasAlpha != 0 ) || pShadow;
                        pSimState = sim::SimState::CreateSimState(pCollObj,pPhysObj);

                        // I suppose this could have just as easily gone into LoadSetUp
                        pSimState->SetControl(sim::simAICtrl);
                        pSimState->SetTransform(matrix);

                        pCurDynaPhysDSG->LoadSetUp(pSimState, pCollAttr, matrix, pDrawable, pShadow );

                        mpListenerCB->OnChunkLoaded( pCurDynaPhysDSG, mUserData, _id );
                        pCurDynaPhysDSG = 0;
                    }
                    else
                    {
                        InstDynaPhysDSG* clone = pCurDynaPhysDSG->Clone(name, matrix);
                        mpListenerCB->OnChunkLoaded(clone, mUserData, _id);
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
                if(pCurDynaPhysDSG == 0)
                {
                    tGeometry* pGeo = (tGeometry*)((GeometryWrappedLoader*)GetAllWrappers()->mpLoader(AllWrappers::msGeometry))->LoadObject(f,store);
                    if( pGeo )
                    {
                        tRefCounted::Assign(pDrawable,(tDrawable*)pGeo);
                    }
                }
            }
            break;

        case P3D_COMPOSITE_DRAWABLE:
            {
                if(pCurDynaPhysDSG == 0)
                {
                    tCompositeDrawable* pCompD = static_cast<tCompositeDrawable*>( mpCompDLoader->LoadObject( f, store ) );
                    if( store->TestCollision( pCompD->GetUID(), pCompD ) )
                    {
                        HandleCollision( pCompD );
                        pCompD = NULL;
                    }
                    else
                    {
                        store->Store( pCompD );
                        tRefCounted::Assign(pDrawable,(tDrawable*)pCompD);
                    }
                }
            }
            break;

        case Simulation::Physics::OBJECT:
            {
                if(pCurDynaPhysDSG == 0)
                {
                    tRefCounted::Assign(pPhysObj,(sim::PhysicsObject*)mpPhysObjLoader->LoadObject(f,store));
                }
            }
            break;

        case Simulation::Collision::OBJECT:
            {
                if(pCurDynaPhysDSG == 0)
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
                            HandleCollision(pCollObj);
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
                    }
                }
            }
            break;

        case SRR2::ChunkID::OBJECT_ATTRIBUTES:
            {
                if(pCurDynaPhysDSG == 0)
                {
                    int classType = f->GetLong();
                    int physPropID = f->GetLong();
                    char tempsound [64];
                    f->GetString(tempsound);

                    // MS10 GREG TODO: This isn't even being used and calling it is leaking memory.
                    //

                    // we need to pass in volume for this thing to be able to set the mass properly in the PhysicsProperties
                    rAssert(pPhysObj);
                    float volume = pPhysObj->GetVolume();    

                    tRefCounted::Assign(pCollAttr,GetATCManager()->CreateCollisionAttributes(classType, physPropID, volume));
                    pCollAttr->SetSound(tempsound);
                }
            }
            break;

        default:
            break;
        } // switch
        f->EndChunk();
    } // while

    if( foundInstances == false || (test && doTestOnce) )
    {
        doTestOnce = false;
        //This must be a global entity.
        rmt::Matrix matrix;
        matrix.Identity();
        pCurDynaPhysDSG = new InstDynaPhysDSG();
        rAssert(pCurDynaPhysDSG);
        pCurDynaPhysDSG->SetName(objName);
        pCurDynaPhysDSG->mTranslucent = HasAlpha != 0;
        pSimState = sim::SimState::CreateSimState(pCollObj, pPhysObj);
        pSimState->SetControl(sim::simAICtrl);
        pSimState->SetTransform(matrix);
        pCurDynaPhysDSG->LoadSetUp(pSimState, pCollAttr, matrix, pDrawable, pShadow );
        GetAllWrappers()->AddGlobalEntity(pCurDynaPhysDSG);
        instanceCount++;
    }

    //
    // Spin Pure3D async loading.
    //
    ///p3d::loadManager->SwitchTask();
    tRefCounted::Release(pCollObj);
    tRefCounted::Release(pPhysObj);
    tRefCounted::Release(pDrawable);
    tRefCounted::Release(pCollAttr);

#ifndef RAD_RELEASE
    PropStats::StopTracking( objName, instanceCount );
#endif


    IEntityDSG::msDeletionsSafe=false;
    return NULL;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// DynaPhysLoader::SetRegdListener
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
void DynaPhysLoader::SetRegdListener
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
// DynaPhysLoader::ModRegdListener
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
void DynaPhysLoader::ModRegdListener
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
// Protected Member Functions : DynaPhysLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : DynaPhysLoader 
//
//************************************************************************
