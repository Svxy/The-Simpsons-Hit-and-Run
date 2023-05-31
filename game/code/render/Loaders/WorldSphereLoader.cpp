//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        WorldSphereLoader.cpp
//
// Description: Implementation for WorldSphereLoader class.
//
// History:     Implemented	                         --Devin [5/27/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/chunkfile.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/billboardobject.hpp>
#include <p3d/inventory.hpp>
#include <p3d/anim/skeleton.hpp>
#include <p3d/anim/animate.hpp>
#include <constants/chunks.h>
#include <constants/chunkids.hpp>
//========================================
// Project Includes
//========================================
#include <render/Loaders/WorldSphereLoader.h>
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/DSG/WorldSphereDSG.h>
#include <render/Loaders/LensFlareLoader.h>

#include <constants/srrchunks.h>
#include <memory/srrmemory.h>

#include <render/Loaders/AllWrappers.h>
#include <render/Loaders/BillboardWrappedLoader.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : WorldSphereLoader Interface
//
//************************************************************************
//========================================================================
// WorldSphereLoader::
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
WorldSphereLoader::WorldSphereLoader() :
tSimpleChunkHandler(SRR2::ChunkID::WORLD_SPHERE_DSG)
{

	mpCompDLoader = new(GMA_PERSISTENT) tCompositeDrawableLoader;
	mpCompDLoader->AddRef();

    mpMCLoader    = new(GMA_PERSISTENT) tMultiControllerLoader;
    mpMCLoader->AddRef();

    mpBillBoardQuadLoader = new (GMA_PERSISTENT) tBillboardQuadGroupLoader;
	mpBillBoardQuadLoader->AddRef();

	mpAnimLoader = new (GMA_PERSISTENT) tAnimationLoader;
	mpAnimLoader->AddRef();

	mpSkelLoader = new (GMA_PERSISTENT) tSkeletonLoader;
	mpSkelLoader->AddRef();

	mpFCLoader = new (GMA_PERSISTENT) tFrameControllerLoader;
	mpFCLoader->AddRef();

	mpLensFlareLoader = new (GMA_PERSISTENT) LensFlareLoader;
	mpLensFlareLoader->AddRef();


    mpListenerCB  = NULL;
    mUserData     = -1;
}
//========================================================================
// WorldSphereLoader::
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
WorldSphereLoader::~WorldSphereLoader()
{
	mpCompDLoader->Release();
    mpMCLoader->Release();
	mpBillBoardQuadLoader->Release();

	mpAnimLoader->Release();
	mpSkelLoader->Release();
	mpFCLoader->Release();
	mpLensFlareLoader->Release();
}
///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// WorldSphereLoader::
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
tEntity* WorldSphereLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char name[255];
    f->GetPString(name);

    int version = f->GetLong();

    WorldSphereDSG *pWorldSphereDSG = new WorldSphereDSG;
    pWorldSphereDSG->SetName(name);

    tMultiController* pMC = NULL;

    pWorldSphereDSG->SetNumMeshes(f->GetLong());

	pWorldSphereDSG->SetNumBillBoardQuadGroups( f->GetLong() );

    while(f->ChunksRemaining())
    {      
        f->BeginChunk();
		int id = f->GetCurrentID();
        switch(f->GetCurrentID())
        {
            case Pure3D::Mesh::MESH:
            {
                GeometryWrappedLoader* pGeoLoader = (GeometryWrappedLoader*)AllWrappers::GetInstance()->mpLoader(AllWrappers::msGeometry) ;
                tGeometry* pGeo = (tGeometry*)pGeoLoader->LoadObject(f, store);
				store->Store( pGeo );
                pWorldSphereDSG->AddMesh(pGeo);
                break;
            }
			case P3D_COMPOSITE_DRAWABLE:
			{
				tCompositeDrawable* pCompDraw = static_cast<tCompositeDrawable*>( mpCompDLoader->LoadObject( f, store ) );
				pWorldSphereDSG->SetCompositeDrawable( pCompDraw );	
				store->Store( pCompDraw );				
			}
			break;
			case P3D_SKELETON:
				{
				tSkeleton* pSkeleton = static_cast<tSkeleton*>(mpSkelLoader->LoadObject( f, store ));
				rAssert( pSkeleton != NULL );
				store->Store( pSkeleton );

				break;
				}     
			case SRR2::ChunkID::LENS_FLARE_DSG:
				{
				LensFlareDSG* pLensFlare = static_cast<LensFlareDSG*>(mpLensFlareLoader->LoadObject( f, store ));
				pWorldSphereDSG->SetFlare( pLensFlare );
				store->Store( pLensFlare );
				}
				break;
			case Pure3D::Animation::AnimationData::ANIMATION:
				{
				tAnimation* pAnimation = static_cast<tAnimation*>(mpAnimLoader->LoadObject( f, store ));
				rAssert( pAnimation != NULL );
				store->Store( pAnimation );				
				break;
				}
			case Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER:
				{
				tFrameController* pFC = static_cast<tFrameController*>(mpFCLoader->LoadObject( f, store ) );
				pFC->SetCycleMode( FORCE_CYCLIC );
				rAssert( pFC != NULL );
				store->Store( pFC );
				}
				break;
            case P3D_MULTI_CONTROLLER:
            {
                pMC = static_cast<tMultiController*>(mpMCLoader->LoadObject(f,store));
				rAssert( pMC != NULL );
				pWorldSphereDSG->SetMultiController(pMC);

				store->Store( pMC );	
                break;
            }
			case Pure3D::BillboardObject::QUAD_GROUP:
			{

				BillboardWrappedLoader::OverrideLoader( true );
                BillboardWrappedLoader* pBBQLoader = static_cast<BillboardWrappedLoader*>(AllWrappers::GetInstance()->mpLoader(AllWrappers::msBillboard));

                tBillboardQuadGroup* pGroup = static_cast<tBillboardQuadGroup*>( pBBQLoader->LoadObject(f, store) );
				rAssert( pGroup != NULL );
				pWorldSphereDSG->AddBillBoardQuadGroup( pGroup );
				store->Store( pGroup );	
				BillboardWrappedLoader::OverrideLoader( false );
				
				break;
			}

            default:
                break;
        } // switch
        f->EndChunk();
    } // while

	LensFlareDSG* pFlare = NULL;


    mpListenerCB->OnChunkLoaded( pWorldSphereDSG, mUserData, _id );
    IEntityDSG::msDeletionsSafe=false;
    return pWorldSphereDSG;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// WorldSphereLoader::SetRegdListener
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
void WorldSphereLoader::SetRegdListener
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
// WorldSphereLoader::ModRegdListener
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
void WorldSphereLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "WorldSphereLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}
//************************************************************************
//
// Protected Member Functions : WorldSphereLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : WorldSphereLoader 
//
//************************************************************************
