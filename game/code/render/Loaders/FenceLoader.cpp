//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        FenceLoader.cpp
//
// Description: Implementation for FenceLoader class.
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
#include <render/Culling/BoxPts.h>
#include <render/Culling/WorldScene.h>
#include <render/RenderManager/RenderManager.h>
#include <render/Loaders/FenceLoader.h>
#include <render/DSG/FenceEntityDSG.h>
#include <constants/srrchunks.h>
#include <memory/srrmemory.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

unsigned int FenceLoader::mFenceCount = 0;

//************************************************************************
//
// Public Member Functions : FenceLoader Interface
//
//************************************************************************
//========================================================================
// FenceLoader::
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
FenceLoader::FenceLoader() :
tSimpleChunkHandler(SRR2::ChunkID::FENCE_DSG)
{
   mpListenerCB  = NULL;
   mUserData     = -1;
}
///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// FenceLoader::
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
tEntity* FenceLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_ZONE );
    #endif

    IEntityDSG::msDeletionsSafe = true;
    FenceEntityDSG* pFenceDSG = new FenceEntityDSG;

    while(f->ChunksRemaining())
    {      
        f->BeginChunk();
        switch(f->GetCurrentID())
        {
            case SRR2::ChunkID::WALL:
            {
                f->GetData(&(pFenceDSG->mStartPoint),   3, tFile::DWORD);
                f->GetData(&(pFenceDSG->mEndPoint),     3, tFile::DWORD);
                f->GetData(&(pFenceDSG->mNormal),       3, tFile::DWORD);

            
                BoxPts WorldBBox = GetRenderManager()->pWorldScene()->mStaticTreeWalker.rIthNode(0).mBBox;
                pFenceDSG->mStartPoint.y  = WorldBBox.mBounds.mMin.y;
                pFenceDSG->mEndPoint.y    = WorldBBox.mBounds.mMax.y;
                // debuggin'
                //if(pFenceDSG->mStartPoint

                break;
            }

            default:
                break;
        } // switch
        f->EndChunk();
    } // while

    mpListenerCB->OnChunkLoaded( pFenceDSG, mUserData, _id );
    
    char name[64];
    sprintf(name, "FenceDSG%d", mFenceCount );
    ++mFenceCount;
    pFenceDSG->SetName(name);
    
    IEntityDSG::msDeletionsSafe=false;

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_ZONE );
    #endif


    return pFenceDSG;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// FenceLoader::SetRegdListener
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
void FenceLoader::SetRegdListener
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
// FenceLoader::ModRegdListener
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
void FenceLoader::ModRegdListener
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
// Protected Member Functions : FenceLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : FenceLoader 
//
//************************************************************************
