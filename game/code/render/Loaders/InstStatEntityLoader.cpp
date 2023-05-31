//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InstStatEntityLoader.cpp
//
// Description: Implementation for InstStatEntityLoader class.
//
// History:     Implemented	                         --Devin [5/27/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/chunkfile.hpp>

//========================================
// Project Includes
//========================================
#include <render/Loaders/InstStatEntityLoader.h>
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/DSG/InstStatEntityDSG.h>

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
// Public Member Functions : InstStatEntityLoader Interface
//
//************************************************************************
//========================================================================
// InstStatEntityLoader::
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
InstStatEntityLoader::InstStatEntityLoader() :
tSimpleChunkHandler(SRR2::ChunkID::INSTA_ENTITY_DSG)
{
   mpListenerCB  = NULL;
   mUserData     = -1;
}
///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatEntityLoader::
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
tEntity* InstStatEntityLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    char name[255];
    f->GetPString(name);

    int version = f->GetLong();
    int HasAlpha = f->GetLong();

    InstStatEntityDSG*      pCurStatEntity  = NULL; 
    tGeometry*              pGeo            = NULL;
    rmt::Matrix*            pMatrix         = NULL;

    bool foundInstances = false;

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

                //ScenegraphTransform >> real ScenegraphTransform
                //f->BeginChunk();

                for(;f->ChunksRemaining();)
                {
                    foundInstances = true;

                    f->BeginChunk();
                    
                    f->GetPString(name);
                    int numChild = f->GetLong();

                    pMatrix = new rmt::Matrix;
                    f->GetData(pMatrix,16,tFile::DWORD);

                    pCurStatEntity = new InstStatEntityDSG;
                    pCurStatEntity->SetName(name);

                    if(HasAlpha)
                    {
                        pCurStatEntity->mTranslucent = true;
                    }

                    f->EndChunk();

                    pCurStatEntity->LoadSetUp(pMatrix,pGeo);

                    mpListenerCB->OnChunkLoaded( pCurStatEntity, mUserData, _id );
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
                GeometryWrappedLoader* pGeoLoader = (GeometryWrappedLoader*)AllWrappers::GetInstance()->mpLoader(AllWrappers::msGeometry) ;
                tRefCounted::Assign(pGeo,(tGeometry*)pGeoLoader->LoadObject(f, store));
                break;
            }

            default:
                break;
        } // switch
        f->EndChunk();
    } // while

    tRefCounted::Release(pGeo);

    if(!foundInstances)
    {
        rDebugPrintf("WARNING : no instances for inststatentity (%s)\n", name);
    }

    IEntityDSG::msDeletionsSafe=false;
    return NULL;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatEntityLoader::SetRegdListener
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
void InstStatEntityLoader::SetRegdListener
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
// InstStatEntityLoader::ModRegdListener
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
void InstStatEntityLoader::ModRegdListener
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
// Protected Member Functions : InstStatEntityLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : InstStatEntityLoader 
//
//************************************************************************
