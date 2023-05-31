//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SnapshotCam.cpp
//
// Description: Implement SnapshotCam
//
// History:     11/28/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <p3d/utility.hpp>
#include <p3d/vectorcamera.hpp>
#include <camera/SnapshotCam.h>
#include <camera/supercamcontroller.h>

#include <memory/srrmemory.h>

#include <loading/loadingmanager.h>

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
// SnapshotCam::SnapshotCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SnapshotCam::SnapshotCam() :
    mNumCameras( 0 ),
    mCurrentCamera( 0 ),
    mToggling( false )
{
}

//==============================================================================
// SnapshotCam::~SnapshotCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SnapshotCam::~SnapshotCam()
{
    unsigned int i;
    for ( i = 0; i < mNumCameras; ++i )
    {
        mCameras[ i ]->Release();
        mCameras[ i ] = NULL;
    }
}

//=============================================================================
// SnapshotCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SnapshotCam::Update( unsigned int milliseconds )
{
    if ( GetFlag( (Flag)FIRST_TIME ) )
    {
        unsigned int i;
        for ( i = 0; i < mNumCameras; ++i )
        {
            mCameras[ i ]->Release();
            mCameras[ i ] = NULL;
        }

        mNumCameras = 0;

        //Load the file.
        GetLoadingManager()->LoadSync( FILEHANDLER_PURE3D, "snapshot.p3d" );

        //Find all the tCameras.
        p3d::inventory->SelectSection("Default");
        HeapMgr()->PushHeap( GMA_TEMP );
        tInventory::Iterator<tVectorCamera> it;
        HeapMgr()->PopHeap( GMA_TEMP );
    
        tVectorCamera* cam = it.First();

        while( cam )
        {
            mCameras[ mNumCameras ] = cam;
            mCameras[ mNumCameras ]->AddRef();
            ++mNumCameras;

            cam = it.Next();        
        }
        
        for ( i = 0; i < mNumCameras; ++i )
        {
            //Take all the cameras out of the inventory.
            p3d::inventory->Remove( mCameras[ i ] );
        }

        SetFlag( (Flag)FIRST_TIME, false );
    }

    rmt::Vector position, target, vup;
    float fov, aspect;

    if ( mNumCameras > 0 )
    {
        //Select a camera.
        if ( !mToggling && rmt::Fabs(mController->GetAxisValue( SuperCamController::stickX )) == 1.0f )
        {
            mToggling = true;

            mCurrentCamera = (mCurrentCamera + rmt::FtoL(mController->GetAxisValue( SuperCamController::stickX ))) % mNumCameras;

#ifdef RAD_DEBUG
            char str[256];
            sprintf( str, "\n\nSNAPSHOT_CAM: %s\n\n", mCameras[mCurrentCamera]->GetName() );
            rDebugString( str );
#endif
        }
        else if ( mToggling && mController->GetAxisValue( SuperCamController::stickX ) == 0.0f )
        {
            mToggling = false;
        }

        mCameras[mCurrentCamera]->GetPosition( &position );
        mCameras[mCurrentCamera]->GetDirection( &target );
        target.Scale( 3.0f );
        target.Add( position );
        mCameras[mCurrentCamera]->GetUpVector( &vup );
        mCameras[mCurrentCamera]->GetFOV( &fov, &aspect );
    }
    else
    {
        GetPosition( &position );
        GetTarget( &target );
        GetCameraUp( &vup );
        fov = GetFOV();
        aspect = GetAspect();
    }

    SetFOV( fov );
    SetAspect( aspect );
    
    SetCameraValues( milliseconds, position, target, &vup );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SnapshotCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SnapshotCam::OnRegisterDebugControls()
{
}

//=============================================================================
// SnapshotCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SnapshotCam::OnUnregisterDebugControls()
{
}
