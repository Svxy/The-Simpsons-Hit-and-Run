//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        CameraDataLoader.cpp
//
// Description: Implement CameraDataLoader
//
// History:     17/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/chunkfile.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <loading/CameraDataLoader.h>

#include <camera/followcamdatachunk.h>
#include <camera/walkercamdatachunk.h>
#include <camera/supercamcentral.h>

#include <memory/srrmemory.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// CameraDataLoader::CameraDataLoader
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
CameraDataLoader::CameraDataLoader()
{
}

//==============================================================================
// CameraDataLoader::~CameraDataLoader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
CameraDataLoader::~CameraDataLoader()
{
}

//=============================================================================
// CameraDataLoader::Load
//=============================================================================
// Description: Comment
//
// Parameters:  (tChunkFile* f, tEntityStore* store)
//
// Return:      tLoadStatus 
//
//=============================================================================
tLoadStatus CameraDataLoader::Load(tChunkFile* f, tEntityStore* store)
{
MEMTRACK_PUSH_GROUP( "Camera Data Loading" );

    //This loads two types of chunks.  Follow and walker cams.

    unsigned int id = f->GetUInt();

    char name[256];
    sprintf( name, "CameraData%d", id );

    HeapMgr()->PushHeap (GMA_LEVEL_OTHER);

    if ( f->GetCurrentID() == SRR2::ChunkID::FOLLOWCAM )
    {
        if ( !SuperCamCentral::FindFCD( id ) )
        {
            //Load and create a FOLLOWCAM data chunk
            FollowCamDataChunk& fcD = SuperCamCentral::GetNewFollowCamDataChunk();

            fcD.SetName( name );

            fcD.mID = id;
            fcD.mRotation = f->GetFloat();
            fcD.mElevation = f->GetFloat();
            fcD.mMagnitude = f->GetFloat();

            fcD.mTargetOffset.x = f->GetFloat();
            fcD.mTargetOffset.y = f->GetFloat();
            fcD.mTargetOffset.z = f->GetFloat();
        }
    }
    else if ( f->GetCurrentID() == SRR2::ChunkID::WALKERCAM )
    {
        //Load and create a WALKERCAM data chunk
        WalkerCamDataChunk* wcD = new WalkerCamDataChunk();

        rAssert( wcD );

        wcD->SetName( name );

        wcD->mID = id;
        wcD->mMinMagnitude = f->GetFloat();
        wcD->mMaxMagnitude = f->GetFloat();
        wcD->mElevation = f->GetFloat();

        wcD->mTargetOffset.x = f->GetFloat();
        wcD->mTargetOffset.y = f->GetFloat();
        wcD->mTargetOffset.z = f->GetFloat();

        //store->Store( wcD );
        tEntity* camData = NULL;
        camData = wcD;

        if ( camData )
        {
            p3d::inventory->PushSection();
            p3d::inventory->SelectSection( SuperCamCentral::CAMERA_INVENTORY_SECTION );
            //const tName& name = camData->GetNameObject();
            //p3d::inventory->find( name );
            bool collision = p3d::inventory->TestCollision( camData );
            if( !collision )
            {
                p3d::inventory->Store( camData );
            }
            else
            {
                camData->AddRef();
                camData->Release();
            }
            p3d::inventory->PopSection();
        }
    }


    HeapMgr()->PopHeap (GMA_LEVEL_OTHER);

MEMTRACK_POP_GROUP("Camera Data Loading");

    return LOAD_OK;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
