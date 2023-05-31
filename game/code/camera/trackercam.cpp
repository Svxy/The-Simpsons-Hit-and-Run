//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TrackerCam.cpp
//
// Description: Implement TrackerCam
//
// History:     08/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <p3d/pointcamera.hpp>

//========================================
// Project Includes
//========================================
#include <camera/TrackerCam.h>
#include <camera/ISuperCamTarget.h>
#include <camera/supercamconstants.h>
#include <input/inputmanager.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifndef DEBUGWATCH
const float TRACKER_CAM_INCREMENT = 0.0174f; //One degree
const float TRACKER_CAM_DIST = 1.0f;
const float TRACKER_CAM_MIN_DIST = 10.0f;
const float TRACKER_CAM_MAX_DIST = 25.0f;
#else
float TRACKER_CAM_INCREMENT = 0.0174f;
float TRACKER_CAM_DIST = 1.0f;
float TRACKER_CAM_MIN_DIST = 10.0f;
float TRACKER_CAM_MAX_DIST = 25.0f;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// TrackerCam::TrackerCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TrackerCam::TrackerCam() :
    mTarget( NULL ),
    mFOVDelta( 0.0f )
{
    mIm = InputManager::GetInstance();
}

//==============================================================================
// TrackerCam::~TrackerCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TrackerCam::~TrackerCam()
{
}

//=============================================================================
// TrackerCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void TrackerCam::Update( unsigned int milliseconds )
{
    if ( GetFlag( (Flag)CUT ) )
    {
        //Reset the FOV.
        SetFOV( SUPERCAM_FOV );//DEFAULT_FOV );
        SetFlag( (Flag)CUT, false );
    }

    //This is to adjust interpolation when we're running substeps.
    float timeMod = (float)milliseconds / (float)16;

    float xAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickX );
    float yAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickY );
    float zAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickY );
    float zToggle = mIm->GetValue( s_secondaryControllerID, InputManager::AnalogL1 );
    float zoom = mIm->GetValue( s_secondaryControllerID, InputManager::AnalogR1 );

    if ( rmt::Fabs( zToggle ) > STICK_DEAD_ZONE && rmt::Fabs( zToggle ) <= 1.0f )
    {
        //zToggled
        yAxis = 0.0f;
    }
    else
    {
        zAxis = 0.0f;
    }

    rmt::Vector targetPos, camPos;
    rmt::Vector desiredPosition;

    mTarget->GetPosition( &targetPos );
    GetPosition( &camPos );

    desiredPosition = targetPos;

    //---------  Figure out where to move the camera...
    
    rmt::Vector rod;
    rod.Sub( camPos, targetPos );
    float magnitude, rotation, elevation;

    rmt::CartesianToSpherical( rod.x, rod.z, rod.y, &magnitude, &rotation, &elevation );

    rotation += ( xAxis * TRACKER_CAM_INCREMENT * timeMod );
    elevation -= ( yAxis * TRACKER_CAM_INCREMENT * timeMod );
    magnitude -= ( zAxis * TRACKER_CAM_DIST * timeMod );

    if ( elevation < 0.05f )
    {
        elevation = 0.05f;
    }
    else if ( elevation > rmt::PI_BY2 )
    {
        elevation = rmt::PI_BY2;
    }
    
    if ( magnitude < 2.0f )
    {
        magnitude = 2.0f;
    }

    rmt::SphericalToCartesian( magnitude, rotation, elevation, &rod.x, &rod.z, &rod.y );

    desiredPosition.Add( rod );

    //---------  Goofin' with the FOV

    rmt::Vector posToTarg;
    posToTarg.Sub( targetPos, desiredPosition );
    float dist = posToTarg.Magnitude(); //Square root!
    float FOV = GetFOV();

    DoWrecklessZoom( dist, TRACKER_CAM_MIN_DIST, TRACKER_CAM_MAX_DIST, mData.GetMinFOV(), mData.GetMaxFOV(),  FOV, mFOVDelta, mData.GetFOVLag(), timeMod );

    SetFOV( FOV );

    SetCameraValues( milliseconds, desiredPosition, targetPos );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
//=============================================================================
// TrackerCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrackerCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Tracker", GetPlayerID() );

    radDbgWatchAddFloat( &mData.mMinFOV, "Min FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mData.mMaxFOV, "Max FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mData.mFOVLag, "FOV Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );

    radDbgWatchAddFloat( &TRACKER_CAM_MIN_DIST, "Min Dist", nameSpace, NULL, NULL, 1.0f, 50.0f );
    radDbgWatchAddFloat( &TRACKER_CAM_MAX_DIST, "Max Dist", nameSpace, NULL, NULL, 1.0f, 50.0f );
#endif
}

//=============================================================================
// TrackerCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrackerCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mData.mMinFOV );
    radDbgWatchDelete( &mData.mMaxFOV );
    radDbgWatchDelete( &mData.mFOVLag );

    radDbgWatchDelete( &TRACKER_CAM_MIN_DIST );
    radDbgWatchDelete( &TRACKER_CAM_MAX_DIST );
#endif
}
