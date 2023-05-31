//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TreeDSGLoader.cpp
//
// Description: Implementation for TreeDSGLoader class.
//
// History:     Implemented	                         --Devin [6/10/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/chunkfile.hpp>

//========================================
// Project Includes
//========================================
#include <render/Culling/Bounds.h>
#include <render/Culling/SpatialTree.h>
#include <render/Loaders/TreeDSGLoader.h>
#include <render/DSG/StaticEntityDSG.h>
#include <render/DSG/StaticPhysDSG.h>
#include <render/DSG/IntersectDSG.h>
#include <constants/srrchunks.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : TreeDSGLoader Interface
//
//************************************************************************
//========================================================================
// TreeDSGLoader::
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
TreeDSGLoader::TreeDSGLoader() :
tSimpleChunkHandler(SRR2::ChunkID::TREE_DSG)
{
   mpListenerCB  = NULL;
   mUserData     = -1;
}
///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// TreeDSGLoader::
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
tEntity* TreeDSGLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
    int nNodes = f->GetLong();
    
    Bounds3f bounds;

    f->GetData(&bounds.mMin, 3, tFile::DWORD);
    f->GetData(&bounds.mMax, 3, tFile::DWORD);

    SpatialTree* pSpatialTree = new SpatialTree;

    pSpatialTree->SetTo(nNodes,bounds);

    ContiguousBinNode< SpatialNode >* pCurNode=pSpatialTree->GetRoot();
    for(int i=0 ; 
        f->ChunksRemaining(); 
        i++, pCurNode++)
    {
        rAssert(i<nNodes);

        f->BeginChunk();
        switch(f->GetCurrentID())
        {
            case SRR2::ChunkID::CONTIGUOUS_BIN_NODE:
            {
                pCurNode->SetSubTreeSize(f->GetLong());
                pCurNode->LinkParent(f->GetLong());
                f->BeginChunk();
                //////////////////////////////////////////////////////////////////////////
                // WET PAINT Do not Touch!! Talk to Devin first.
                // Violation leads to the Tree of Woe
                //////////////////////////////////////////////////////////////////////////                
                    pCurNode->mData.mSubDivPlane.mAxis         = f->GetChar(); 
                    pCurNode->mData.mSubDivPlane.mPosn         = f->GetFloat(); 
		            pCurNode->mData.mSEntityElems.mUseSize     = f->GetLong();	
                    pCurNode->mData.mSPhysElems.mUseSize       = f->GetLong(); 
                    pCurNode->mData.mIntersectElems.mUseSize   = f->GetLong(); 
                    pCurNode->mData.mDPhysElems.mUseSize       = f->GetLong(); 
                    pCurNode->mData.mFenceElems.mUseSize       = f->GetLong(); 
                    pCurNode->mData.mRoadSegmentElems.mUseSize = f->GetLong(); 
					pCurNode->mData.mPathSegmentElems.mUseSize = f->GetLong(); 
                    pCurNode->mData.mAnimElems.mUseSize        = f->GetLong()+1;
                    pCurNode->mData.mAnimCollElems.mUseSize    = 1; 
                f->EndChunk();

                if(pCurNode->IsRoot())
                {
                //////////////////////////////////////////////////////////////////////////
                // WET PAINT Do not Touch!! Talk to Devin first.
                // Violation leads to the Tree of Woe
                //////////////////////////////////////////////////////////////////////////
                    pCurNode->mData.mSEntityElems.mUseSize  += 100;
                    pCurNode->mData.mDPhysElems.mUseSize    += 10;
                    pCurNode->mData.mAnimCollElems.mUseSize += 50;
                    pCurNode->mData.mAnimElems.mUseSize     += 60;
                }

                break;
            }

            default:
                rAssert(false);
                break;
        } // switch
        f->EndChunk();
    } 

    mpListenerCB->OnChunkLoaded( pSpatialTree, mUserData, _id );
    IEntityDSG::msDeletionsSafe=false;
    return NULL; //Screw you inventory! pSpatialTree;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// TreeDSGLoader::SetRegdListener
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
void TreeDSGLoader::SetRegdListener
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
// TreeDSGLoader::ModRegdListener
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
void TreeDSGLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "TreeDSGLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}
//************************************************************************
//
// Protected Member Functions : TreeDSGLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : TreeDSGLoader 
//
//************************************************************************
