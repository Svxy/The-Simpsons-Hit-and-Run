//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        MayaCamera.cpp
//
// Description: Implement MayaCamera
//
// History:     22/07/2002 + Created -- Cary Brisebois
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
#include "MayaCamera.h"
#include "main/constants.h"
#include "utility/mext.h"


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
const MString WB_CAM_NAME( "WorldBuilderCam" );

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// MayaCamera::MayaCamera
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
MayaCamera::MayaCamera()
{
    EnsureCamExists();
}

//==============================================================================
// MayaCamera::~MayaCamera
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
MayaCamera::~MayaCamera()
{
}

//=============================================================================
// MayaCamera::EnsureCamExists
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MayaCamera::EnsureCamExists()
{
    MStatus status;
    MFnCamera fnCamera;

    MDagPath path;

    if ( !MExt::FindDagNodeByName( &path, WB_CAM_NAME ) )
    {
        fnCamera.create( &status );
        assert( status );

        fnCamera.setName( WB_CAM_NAME + MString( "Camera" ) );

        MFnDependencyNode transform( fnCamera.parent( 0 ) );

        MString transName = WB_CAM_NAME;
        transform.setName( transName );
    }        
}

//=============================================================================
// MayaCamera::Set
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& position, const rmt::Vector& target, const rmt::Vector& vup, float fov )
//
// Return:      void 
//
//=============================================================================
void MayaCamera::Set( const rmt::Vector& position, const rmt::Vector& target, const rmt::Vector& vup, float fov )
{
    MStatus status;

    EnsureCamExists();

    MDagPath path;
    bool found = MExt::FindDagNodeByName( &path, WB_CAM_NAME );
    assert( found );

    MFnCamera fnCamera( path, &status );
    assert( status );
    
    
    //Scale it so you can see it.
    MFnTransform fnTransform( fnCamera.parent( 0 ) );
    const double scale[3] = { 200, 200, 200 } ;
    fnTransform.setScale( scale );

    MPoint eyePosition;
    eyePosition.x = position.x * WBConstants::Scale;
    eyePosition.y = position.y * WBConstants::Scale;
    eyePosition.z = -position.z * WBConstants::Scale;

    MVector viewDirection;
    viewDirection.x = (target.x - position.x) * WBConstants::Scale;
    viewDirection.y = (target.y - position.y) * WBConstants::Scale;
    viewDirection.z = -(target.z - position.z) * WBConstants::Scale;

    MVector viewUp;
    viewUp.x = vup.x;
    viewUp.y = vup.y;
    viewUp.z = -vup.z;

    status = fnCamera.set( eyePosition, viewDirection, viewUp, rmt::DegToRadian(fov), 4/3 );
    assert( status );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
