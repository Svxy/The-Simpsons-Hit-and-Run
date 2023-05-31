//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        StaticPhysLoader.cpp
//
// Description: Implementation for StaticPhysLoader class.
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
#include <render/Loaders/StaticPhysLoader.h>
#include <render/DSG/StaticPhysDSG.h>
#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>
#include <memory/srrmemory.h>
#include <atc/atcmanager.h>
#include <constants/blobshadownames.h>

#include <radtime.hpp>
//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : StaticPhysLoader Interface
//
//************************************************************************
//========================================================================
// StaticPhysLoader::
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
StaticPhysLoader::StaticPhysLoader() :
tSimpleChunkHandler(SRR2::ChunkID::STATIC_PHYS_DSG)
{
    mpCollObjLoader = new(GMA_PERSISTENT) sim::CollisionObjectLoader();
    mpCollObjLoader->AddRef();

    mpListenerCB  = NULL;
    mUserData     = -1;
}
//========================================================================
// StaticPhysLoader::
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
StaticPhysLoader::~StaticPhysLoader()
{
    mpCollObjLoader->Release();
}

///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// StaticPhysLoader::
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
tEntity* StaticPhysLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char name[255];
    f->GetPString(name);
	// Lets see if theis object has a shadow associated with it
	const char* pShadowName = BlobbyShadowNames::FindShadowName( name );
	tDrawable* pShadow;
	if ( pShadowName != NULL )
	{
		pShadow = p3d::find< tDrawable > ( pShadowName );
	}
	else
	{
		pShadow = NULL;
	}

    int version = f->GetLong();

    StaticPhysDSG* pStaticPhysDSG = new StaticPhysDSG;
    pStaticPhysDSG->SetName(name);
    CollisionAttributes* pCollAttr   = NULL;

    while(f->ChunksRemaining())
    {      
        f->BeginChunk();
        switch(f->GetCurrentID())
        {
            case Simulation::Collision::OBJECT:
            {
                sim::CollisionObject* pCollObj = (sim::CollisionObject*)mpCollObjLoader->LoadObject(f, store);
                // TBJ [7/9/2002]
                // Added this to store collision objects in the inventory.
                // A normal loader stores the top level chunk, in this case the StaticPhysDSG.
                // Since we want the Collision Object in the inventory, we have to store it here.
                //
                if ( pCollObj )
                {
                    if( store && store->TestCollision( pCollObj->GetUID(), pCollObj ) )
                    {
                        HandleCollision(pCollObj);
                    }
                    else
                    {
                        store->Store(pCollObj);
                    }                
                }
                pStaticPhysDSG->SetSimState(sim::SimState::CreateStaticSimState(pCollObj));
                
                pStaticPhysDSG->SetShadow( pShadow );

                pStaticPhysDSG->GetSimState()->GetCollisionObject()->GetCollisionVolume()->GenerateHierarchy();

                break;
            }
			case SRR2::ChunkID::OBJECT_ATTRIBUTES:
            {
                    
                int classType = f->GetLong();
                int physPropID = f->GetLong();
				char tempsound [64];
				f->GetString(tempsound);
            
				pCollAttr = GetATCManager()->CreateCollisionAttributes(classType, physPropID, 0.0f);
				pCollAttr->SetSound(tempsound);
				pStaticPhysDSG->SetCollisionAttributes( pCollAttr );
			}
            break;

				break;
            default:
                break;
        } // switch
        f->EndChunk();
    } // while


    mpListenerCB->OnChunkLoaded( pStaticPhysDSG, mUserData, _id );
    //
    // Spin Pure3D async loading.
    //
    //p3d::loadManager->SwitchTask();
    IEntityDSG::msDeletionsSafe=false;
    return pStaticPhysDSG;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// StaticPhysLoader::SetRegdListener
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
void StaticPhysLoader::SetRegdListener
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
// StaticPhysLoader::ModRegdListener
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
void StaticPhysLoader::ModRegdListener
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
// Protected Member Functions : StaticPhysLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : StaticPhysLoader 
//
//************************************************************************
