//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        chasecam.cpp
//
// Description: Implement ChaseCam
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <camera/ChaseCam.h>
#include <camera/isupercamtarget.h>
#include <camera/supercamconstants.h>

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
// ChaseCam::ChaseCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ChaseCam::ChaseCam() :
    mTarget( NULL ),
    mFOVDelta( 0.0f )
{
    mPosition.Set( 0.0f, 0.0f, 0.0f );
    mPositionDelta.Set( 0.0f, 0.0f, 0.0f );
    mTargetPos.Set( 0.0f, 0.0f, 0.0f );
    mTargetDelta.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// ChaseCam::~ChaseCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ChaseCam::~ChaseCam()
{
}

//=============================================================================
// ChaseCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void ChaseCam::Update( unsigned int milliseconds )
{
    rAssertMsg( mTarget, "The ChaseCam needs a target!" );
    
    //This is to adjust interpolation when we're running substeps.
    float timeMod = (float)milliseconds / EXPECTED_FRAME_RATE;
    
    if ( GetFlag( (Flag)CUT ) )
    {
        DoCameraCut();
    }

    rmt::Vector oldCamPos;
    GetPosition( &oldCamPos );

    //---------  Buid a rod for the camera

    rmt::Vector rod;

    mData.GetRod( &rod );

    //This makes the camera act more like a hellicopter camera
    rmt::Matrix mat;
    rmt::Vector targetHeading;
    mTarget->GetHeading( &targetHeading );
    mat.Identity();
    mat.FillHeadingXZ( targetHeading );

    rod.Transform( mat );

    rmt::Vector targetPosition;
    mTarget->GetPosition( &targetPosition );

    rod.Add( targetPosition );


    //---------  Set the position and target of the camera
    
    rmt::Vector desiredPosition = rod;
    rmt::Vector desiredTarget = targetPosition;

    float posLag = mData.GetPositionLag() * timeMod;
    CLAMP_TO_ONE(posLag);

    MotionCubic( &mPosition.x, &mPositionDelta.x, desiredPosition.x, posLag );
    MotionCubic( &mPosition.y, &mPositionDelta.y, desiredPosition.y, posLag );
    MotionCubic( &mPosition.z, &mPositionDelta.z, desiredPosition.z, posLag );

    float targLag = mData.GetTargetLag() * timeMod;
    CLAMP_TO_ONE(targLag);

    MotionCubic( &mTargetPos.x, &mTargetDelta.x, desiredTarget.x, targLag );
    MotionCubic( &mTargetPos.y, &mTargetDelta.y, desiredTarget.y, targLag );
    MotionCubic( &mTargetPos.z, &mTargetDelta.z, desiredTarget.z, targLag );

    rmt::Vector diff;
    diff.Sub( mPosition, oldCamPos );
    float speed = diff.Magnitude();

    //Let's goof with the field of view.
    float diffFOV = mData.GetMaxFOV() - mData.GetMinFOV();
    float maxSpeed = mData.GetMaxSpeed();

    if ( speed > maxSpeed )
    {
        speed = maxSpeed;
    }

    //The closer we get to the max speed, the wider the FOV.
    float desiredFOV = mData.GetMinFOV() + (diffFOV * speed / maxSpeed);

    float FOV = GetFOV();
        
    float fovLag = mData.GetFOVLag() * timeMod;
    CLAMP_TO_ONE(fovLag);

    MotionCubic( &FOV, &mFOVDelta, desiredFOV, fovLag );

    SetFOV( FOV );

    SetCameraValues( milliseconds, mPosition, mTargetPos );
}

//=============================================================================
// ChaseCam::LoadSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned char* settings )
//
// Return:      void 
//
//=============================================================================
void ChaseCam::LoadSettings( unsigned char* settings )
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// ChaseCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ChaseCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Chase", GetPlayerID() );

    radDbgWatchAddVector( &mData.mRod.x, "Rod", nameSpace, NULL, NULL, -100.0f, 100.0f );
    radDbgWatchAddFloat( &mData.mPositionLag, "Position Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mTargetLag, "Target Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mMinFOV, "Min FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mData.mMaxFOV, "Max FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mData.mMaxSpeed, "Max Speed", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mFOVLag, "FOV Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
#endif
}

//=============================================================================
// ChaseCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ChaseCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mData.mRod.x );
    radDbgWatchDelete( &mData.mPositionLag );
    radDbgWatchDelete( &mData.mTargetLag );
    radDbgWatchDelete( &mData.mMinFOV );
    radDbgWatchDelete( &mData.mMaxFOV );
    radDbgWatchDelete( &mData.mMaxSpeed );
    radDbgWatchDelete( &mData.mFOVLag );
#endif
}

//=============================================================================
// ChaseCam::DoCameraCut
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ChaseCam::DoCameraCut()
{
    rmt::Vector rod;

    mData.GetRod( &rod );

    rmt::Vector targetPosition;
    mTarget->GetPosition( &targetPosition );

    rmt::Matrix mat;
    rmt::Vector targetHeading;
    mTarget->GetHeading( &targetHeading );
    mat.Identity();
    mat.FillHeadingXZ( targetHeading );

    rod.Transform( mat );       

    mPosition.Add( rod, targetPosition );
    mPositionDelta = mPosition;

    SetFOV( SUPERCAM_FOV );//DEFAULT_FOV );
    mFOVDelta = 0.0f;

    SetFlag( (Flag)CUT, false );
}
