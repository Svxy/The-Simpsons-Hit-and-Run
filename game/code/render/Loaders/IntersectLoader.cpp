//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        IntersectLoader.cpp
//
// Description: Implementation for IntersectLoader class.
//
// History:     Implemented	                         --Devin [7/19/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/chunkfile.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <render/Loaders/IntersectLoader.h>
#include <render/DSG/IntersectDSG.h>
#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : IntersectLoader Interface
//
//************************************************************************
//========================================================================
// IntersectLoader::
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
IntersectLoader::IntersectLoader() :
tSimpleChunkHandler(SRR2::ChunkID::INTERSECT_DSG)
{
   mpListenerCB  = NULL;
   mUserData     = -1;
}

///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// IntersectLoader::
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
//   ULONG NumIndices;
//   ULONG Indices
//      array = NumIndices;
//   ULONG NumPositions;
//   tlPoint Positions
//      array = NumPositions;
//   ULONG NumNormals;
//   tlPoint Normals
//      array = NumNormals;
//   ULONG NumCentroids;
//   tlPoint Centroids
//      array = NumCentroids;
//   ULONG NumRadii;
//   float Radii
//      array = NumRadii;
//   Chunk tlBBoxChunk;
//   Chunk tlBSphereChunk;
//
//========================================================================
tEntity* IntersectLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
/*    static int profileNumPts        = 0;
    static int profileNumNorms      = 0;
    static int profileNumTerrain    = 0;
    static int profileNumIndicies   = 0;
    static int profileTotalSize     = 0;
*/

    IEntityDSG::msDeletionsSafe=true;
    int i;
    IntersectDSG* pIDSG = new IntersectDSG;

    pIDSG->mTriIndices.Allocate(f->GetLong());  
    pIDSG->mTriIndices.AddUse(pIDSG->mTriIndices.mSize);  

    for(i=0; i<pIDSG->mTriIndices.mSize; i++)
    {
        pIDSG->mTriIndices[i] = (int)(f->GetLong());
    }

    pIDSG->mTriPts.Allocate(f->GetLong());  
    pIDSG->mTriPts.AddUse(pIDSG->mTriPts.mSize);
    
    //for(i=0; i<pIDSG->mTriPts.mSize; i++)
    {
        f->GetData(&pIDSG->mTriPts[0], 3*pIDSG->mTriPts.mSize, tFile::DWORD);
    }

    pIDSG->mTriNorms.Allocate(f->GetLong());  
    pIDSG->mTriNorms.AddUse(pIDSG->mTriNorms.mSize);
    
    //for(i=0; i<pIDSG->mTriNorms.mSize; i++)
    {
        f->GetData(&pIDSG->mTriNorms[0], 3*pIDSG->mTriNorms.mSize, tFile::DWORD);
    }
/*
    pIDSG->mTriCentroids.Allocate(f->GetLong());  
    pIDSG->mTriCentroids.AddUse(pIDSG->mTriCentroids.mSize);
    
    //for(i=0; i<pIDSG->mTriCentroids.mSize; i++)
    {
        f->GetData(&pIDSG->mTriCentroids[0], 3*pIDSG->mTriCentroids.mSize, tFile::DWORD);
    }

    pIDSG->mTriRadius.Allocate(f->GetLong());  
    pIDSG->mTriRadius.AddUse(pIDSG->mTriRadius.mSize);
    
    //for(i=0; i<pIDSG->mTriRadius.mSize; i++)
    {
        f->GetData(&pIDSG->mTriRadius[0], pIDSG->mTriRadius.mSize, tFile::DWORD);
    }
*/	
    for(i=0 ; 
        f->ChunksRemaining(); 
        i++)
    {
        f->BeginChunk();
        switch(f->GetCurrentID())
        {
            case Pure3D::Mesh::BOX:
            {
                float minx = f->GetFloat();
                float miny = f->GetFloat();
                float minz = f->GetFloat();
                float maxx = f->GetFloat();
                float maxy = f->GetFloat();
                float maxz = f->GetFloat();

                pIDSG->SetBoundingBox( minx, miny, minz, maxx, maxy, maxz);
                break;
            }

            case Pure3D::Mesh::SPHERE:
            {
                float cx = f->GetFloat();
                float cy = f->GetFloat();
                float cz = f->GetFloat();
                float r  = f->GetFloat();

                pIDSG->SetBoundingSphere(cx,cy,cz,r);
                break;
            }

            case SRR2::ChunkID::TERRAIN_TYPE:
            {
                long version = f->GetLong();
                rAssert( version == 0 );
                long size = f->GetLong();
                pIDSG->mTerrainType.Allocate( size );
                pIDSG->mTerrainType.AddUse( size );
                //for( i = 0; i < size; ++i )
                //{
                    f->GetData(&pIDSG->mTerrainType[0], pIDSG->mTerrainType.mSize, tFile::BYTE);
                    //f->GetData( &pIDSG->mTerrainType[ i ], 1, tFile::BYTE );
                //}
                break;
            }

            default:
                rAssert(false);
                break;
        } // switch
        f->EndChunk();
    } 
/*
    if(pIDSG->mTriPts.mSize > profileNumPts)            profileNumPts       = pIDSG->mTriPts.mSize;
    if(pIDSG->mTriNorms.mSize > profileNumNorms)        profileNumNorms     = pIDSG->mTriNorms.mSize;
    if(pIDSG->mTerrainType.mSize > profileNumTerrain)   profileNumTerrain   = pIDSG->mTerrainType.mSize;
    if(pIDSG->mTriIndices.mSize > profileNumIndicies)   profileNumIndicies  = pIDSG->mTriIndices.mSize;

    if( 3*4*(pIDSG->mTriPts.mSize + pIDSG->mTriNorms.mSize) + pIDSG->mTriIndices.mSize*4 + pIDSG->mTerrainType.mSize > profileTotalSize )
    {
        profileTotalSize  = 3*4*(pIDSG->mTriPts.mSize + pIDSG->mTriNorms.mSize) + pIDSG->mTriIndices.mSize*4 + pIDSG->mTerrainType.mSize;
        if(profileTotalSize > 8192)
            profileTotalSize = 8192;
    }
*/
    mpListenerCB->OnChunkLoaded( pIDSG, mUserData, _id );
    IEntityDSG::msDeletionsSafe=false;
    return NULL;//pIDSG;
}
///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// IntersectLoader::SetRegdListener
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
void IntersectLoader::SetRegdListener
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
// IntersectLoader::ModRegdListener
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
void IntersectLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "IntersectLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}
//************************************************************************
//
// Protected Member Functions : IntersectLoader 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : IntersectLoader 
//
//************************************************************************
