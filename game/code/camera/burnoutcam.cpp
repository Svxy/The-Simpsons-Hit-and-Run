//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        BurnoutCam.cpp
//
// Description: Implement BurnoutCam
//
// History:     10/7/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <radmath/radmath.hpp>


//========================================
// Project Includes
//========================================
#include <camera/BurnoutCam.h>
#include <camera/supercamconstants.h>
#include <input/inputmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifndef DEBUGWATCH
const float BURNOUT_CAM_INCREMENT = 0.0174f; //One degree
//const float BURNOUT_CAM_DIST = 1.0f;
const float BURNOUT_CAM_DIST = 0.1f;
#else
float BURNOUT_CAM_INCREMENT = 0.0174f;
//float BURNOUT_CAM_DIST = 1.0f;
float BURNOUT_CAM_DIST = 0.1f;
#endif


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// BurnoutCam::BurnoutCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BurnoutCam::BurnoutCam() :
    mRotation( -1.061069f ), //Along the side
    mElevation( 1.29794f ),
    mMagnitude( 5.7266f )
{
    mIm = InputManager::GetInstance();

    mTargetOffset.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// BurnoutCam::~BurnoutCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BurnoutCam::~BurnoutCam()
{
}

//=============================================================================
// BurnoutCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void BurnoutCam::Update( unsigned int milliseconds )
{
    rAssert( mTarget );

    if ( GetFlag( (Flag)CUT ) )
    {
        //Reset the FOV.
        SetFOV( SUPERCAM_FOV );//DEFAULT_FOV );
        SetFlag( (Flag)CUT, false );
    }
  
    //This is to adjust interpolation when we're running substeps.
    float timeMod = 1.0f;
    
    timeMod = (float)milliseconds / 16.0f;
 
    float xAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickX );
    float yAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickY );
    float zAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickY );
    float zToggle = mIm->GetValue( s_secondaryControllerID, InputManager::AnalogR1 );

    if ( rmt::Fabs( zToggle ) > STICK_DEAD_ZONE && rmt::Fabs( zToggle ) <= 1.0f )
    {
        //zToggled
        yAxis = 0.0f;
    }
    else
    {
        zAxis = 0.0f;
    }


    mRotation += ( xAxis * BURNOUT_CAM_INCREMENT * timeMod );
    mElevation -= ( yAxis * BURNOUT_CAM_INCREMENT * timeMod );
    mMagnitude -= ( zAxis * BURNOUT_CAM_DIST * timeMod );

    if ( mElevation < 0.001f )
    {
        mElevation = 0.001f;
    }
    else if ( mElevation > rmt::PI - 0.05f )
    {
        mElevation = rmt::PI - 0.05f;
    }
    
    if ( mMagnitude < 2.0f )
    {
        mMagnitude = 2.0f;
    }

    //This positions itself always relative to the target.
    rmt::Vector rod;
    rmt::SphericalToCartesian( mMagnitude, mRotation, mElevation, &rod.x, &rod.z, &rod.y );

    //This is the position of the target.
    rmt::Vector targetPos;
    mTarget->GetPosition( &targetPos );

    rmt::Vector desiredPos, desiredTarget;

    //Put this local to the targets heading.
    rmt::Vector heading;
    mTarget->GetHeading( &heading );

    rmt::Vector vup;
    mTarget->GetVUP( &vup );

    rmt::Matrix mat;
    mat.Identity();

    mat.FillHeading( heading, vup );

    rod.Transform( mat );

    desiredPos.Add( rod, targetPos );

    //Calculate the target of the camera.
    if ( GetFlag( (Flag)FIRST_TIME ) )
    {
        //Okay, if this is the first time we're going in, let's figure out where the camera target is
        //relative to the position of the target object.
        rmt::Vector camTargetPos;
        GetTarget( &camTargetPos );

        mTargetOffset.Sub( camTargetPos, targetPos );

        rmt::Matrix invMat;
        invMat = mat;
        invMat.Invert();

        mTargetOffset.Transform( invMat );

        SetFlag( (Flag)FIRST_TIME, false );
    }

    desiredTarget = mTargetOffset;
    desiredTarget.Transform( mat );
    desiredTarget.Add( targetPos );

    SetCameraValues( milliseconds, desiredPos, desiredTarget );    
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// BurnoutCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BurnoutCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Burnout", GetPlayerID() );

    radDbgWatchAddFloat( &mElevation, "Elevation", nameSpace, NULL, NULL, 0.001f, rmt::PI - 0.05f );
    radDbgWatchAddFloat( &mRotation, "Rotation", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &mMagnitude, "Magnitude", nameSpace, NULL, NULL, 1.0f, 10.0f );
#endif
}

//=============================================================================
// BurnoutCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BurnoutCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mElevation );
    radDbgWatchDelete( &mRotation );
    radDbgWatchDelete( &mMagnitude );
#endif
}