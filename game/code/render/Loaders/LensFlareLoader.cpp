//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        LensFlareLoader.cpp
//
// Description: Implementation for LensFlareLoader class.
//
//========================================================================

//========================================
// System Includes
//========================================
#include <constants/chunks.h>
#include <constants/chunkids.hpp>
#include <p3d/chunkfile.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/billboardobject.hpp>
#include <p3d/inventory.hpp>
//========================================
// Project Includes
//========================================
#include <render/Loaders/LensFlareLoader.h>
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/DSG/LensFlareDSG.h>


#include <constants/srrchunks.h>
#include <memory/srrmemory.h>

#include <render/Loaders/AllWrappers.h>
#include <render/Loaders/BillboardWrappedLoader.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

const char* VIS_TEST_NAME = "visibility_test";

//************************************************************************
//
// Public Member Functions : LensFlareLoader Interface
//
//************************************************************************
//========================================================================
// LensFlareLoader::
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
LensFlareLoader::LensFlareLoader() :
tSimpleChunkHandler(SRR2::ChunkID::LENS_FLARE_DSG)
{

	mpCompDLoader = new(GMA_PERSISTENT) tCompositeDrawableLoader;
	mpCompDLoader->AddRef();

    mpBillBoardQuadLoader = new (GMA_PERSISTENT) tBillboardQuadGroupLoader;
	mpBillBoardQuadLoader->AddRef();

    mpListenerCB  = NULL;
    mUserData     = -1;
}
//========================================================================
// LensFlareLoader::
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
LensFlareLoader::~LensFlareLoader()
{
	mpCompDLoader->Release();
	mpBillBoardQuadLoader->Release();

}
///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// LensFlareLoader::
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
tEntity* LensFlareLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char name[255];
    f->GetPString(name);

    int version = f->GetLong();

    LensFlareDSG *pLensFlareDSG = new LensFlareDSG;
	pLensFlareDSG->SetNumBillBoardQuadGroups( f->GetLong() );

    // Composite drawable, hold onto a pointer so that we can
    // search through it after loading and pick out the bbqs
    tCompositeDrawable* pCompDraw = NULL;

    while(f->ChunksRemaining())
    {      
        f->BeginChunk();
		int id = f->GetCurrentID();
        switch(f->GetCurrentID())
        {

			case P3D_COMPOSITE_DRAWABLE:
			{
				pCompDraw = static_cast<tCompositeDrawable*>( mpCompDLoader->LoadObject( f, store ) );
				pLensFlareDSG->SetCompositeDrawable( pCompDraw );	
				store->Store( pCompDraw );				
			}
			break;
			case Pure3D::Mesh::MESH:
				{
		            GeometryWrappedLoader* pGeoLoader = (GeometryWrappedLoader*)AllWrappers::GetInstance()->mpLoader(AllWrappers::msGeometry);
					tGeometry* pGeo = static_cast<tGeometry*>(pGeoLoader->LoadObject( f, store) );
					store->Store( pGeo );
				}
				break;

			case Pure3D::BillboardObject::QUAD_GROUP:
			{

				BillboardWrappedLoader::OverrideLoader( true );
                BillboardWrappedLoader* pBBQLoader = static_cast<BillboardWrappedLoader*>(AllWrappers::GetInstance()->mpLoader(AllWrappers::msBillboard));

                tBillboardQuadGroup* pGroup = static_cast<tBillboardQuadGroup*>( pBBQLoader->LoadObject(f, store) );
				rAssert( pGroup != NULL );
				pLensFlareDSG->AddBillBoardQuadGroup( pGroup );
				store->Store( pGroup );	
				BillboardWrappedLoader::OverrideLoader( false );
				break;
			}

            default:
                break;
        } // switch
        f->EndChunk();
    } // while


    SetOcclusionFlags( pCompDraw );


    if ( mpListenerCB != NULL )
    {
       // mpListenerCB->OnChunkLoaded( pLensFlareDSG, mUserData, _id );
    }
    IEntityDSG::msDeletionsSafe=false;
    return pLensFlareDSG;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// LensFlareLoader::SetRegdListener
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
void LensFlareLoader::SetRegdListener
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
// LensFlareLoader::ModRegdListener
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
void LensFlareLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "LensFlareLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}
//************************************************************************
//
// Protected Member Functions : LensFlareLoader 
//
//************************************************************************


void LensFlareLoader::SetOcclusionFlags( tCompositeDrawable* compDraw )
{
    rAssert( compDraw != NULL );
    // Iterate through the drawable elements, looking for billboard quad groups
    // to set the occlusion flags on
    int currentBBQ = 0;
    float highestPriority = -FLT_MAX;
    int highestPriorityElement = -1;
    for ( int i = 0 ; i < compDraw->GetNumDrawableElement() ; i++ )
    {
        tCompositeDrawable::DrawableElement* element = compDraw->GetDrawableElement(i);
        if( element )
        {
            tBillboardQuadGroup* bbq = dynamic_cast< tBillboardQuadGroup* >( element->GetDrawable() );
            if ( bbq )
            {  
                // Set to 1, occlusion test enabled on this one             
                bbq->SetOcclusion( 1 );
                if ( element->SortOrder() > highestPriority )
                {
                    highestPriorityElement = i;
                    highestPriority = element->SortOrder();
                    currentBBQ++;
                }
            }
        }
    }
    // Find the one that will be drawn first ( highest sort order ) and set its occlusion to 2
    if ( highestPriorityElement != -1 )
    {
        tCompositeDrawable::DrawableElement* element = compDraw->GetDrawableElement(highestPriorityElement);
        tBillboardQuadGroup* bbq = dynamic_cast< tBillboardQuadGroup* >( element->GetDrawable() );
        rAssert( bbq != NULL );
        if ( bbq )
        {
            bbq->SetOcclusion( 2 );
        }
    }
}


//************************************************************************
//
// Private Member Functions : LensFlareLoader 
//
//************************************************************************
