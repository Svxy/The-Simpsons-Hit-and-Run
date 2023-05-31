//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        debugcam.cpp
//
// Description: Implement DebugCam
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
//These are included in the precompiled headers on XBOX and GC
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <camera/DebugCam.h>
#include <camera/supercamconstants.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>



//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
const float MAGNITUDE = 1.0f;

#ifndef DEBUGWATCH
const float INCREMENT = 0.0174f; //One degree
const float DIST = 1.0f;
#else
float INCREMENT = 0.0174f;
float DIST = 1.0f;
#endif

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// DebugCam::DebugCam
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
DebugCam::DebugCam() :
    mRotationAngleXZ( 0.0f ),
    mRotationAngleY( 2.13f )
{
    mIm = InputManager::GetInstance();
}

//=============================================================================
// DebugCam::~DebugCam
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
DebugCam::~DebugCam()
{
}

//=============================================================================
// DebugCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds  )
//
// Return:      void 
//
//=============================================================================
void DebugCam::Update( unsigned int milliseconds  )
{
    if ( GetFlag( (Flag)CUT ) )
    {
        //Reset the FOV.
        SetFOV( SUPERCAM_FOV );//DEFAULT_FOV );

        SetFlag( (Flag)CUT, false );
    }

    //This is to adjust interpolation when we're running substeps.
    float timeMod = 1.0f;
    
    timeMod = (float)milliseconds / (float)16;
       
    bool cameraRelative = false;

#ifdef RAD_WIN32
    float left = mIm->GetValue( 0, InputManager::CameraLeft );
    float right = mIm->GetValue( 0, InputManager::CameraRight );
    float up = mIm->GetValue( 0, InputManager::CameraMoveIn );
    float down = mIm->GetValue( 0, InputManager::CameraMoveOut );
    
    float xAxis = ( right > left ) ? right : -left;
    float yAxis = ( up > down ) ? up : -down;
    float zAxis = yAxis;
    float zToggle = mIm->GetValue( 0, InputManager::CameraLookUp );
    float rightTrigger = mIm->GetValue( 0, InputManager::CameraZoom );
#else
    float xAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickX );
    float yAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickY );
    float zAxis = mIm->GetValue( s_secondaryControllerID, InputManager::LeftStickY );
    float zToggle = mIm->GetValue( s_secondaryControllerID, InputManager::L3 );
    float rightTrigger = mIm->GetValue( s_secondaryControllerID, InputManager::AnalogR1 );
#endif

    if ( rmt::Fabs( zToggle ) > STICK_DEAD_ZONE && rmt::Fabs( zToggle ) <= 1.0f )
    {
        //zToggled
        yAxis = 0.0f;
    }
    else
    {
        zAxis = 0.0f;
    }

    if ( rmt::Fabs( rightTrigger ) > STICK_DEAD_ZONE && rmt::Fabs( rightTrigger ) <= 1.0f )
    {
        //TODO: Make camera relative movement work.
        cameraRelative = true;
    }

    mRotationAngleXZ -= ( xAxis * INCREMENT * timeMod );
    mRotationAngleY -= ( yAxis * INCREMENT * timeMod );

    float x, y, z;
    rmt::SphericalToCartesian( MAGNITUDE, mRotationAngleXZ, mRotationAngleY,
                               &x, &z, &y );

    rmt::Vector camPos, camTarget, diff;
    GetPosition( &camPos );
    
    camTarget.Add( camPos, rmt::Vector( x, y, z ) );

    diff.Sub( camTarget, camPos );
    diff.NormalizeSafe();

    diff.Scale( DIST * zAxis * timeMod );

    camPos.Add( diff );
    camTarget.Add( diff );

    SetCameraValues( milliseconds, camPos, camTarget );
}

//=============================================================================
// DebugCam::EnableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DebugCam::EnableShake()
{
    SetShaker( &mSineCosShaker );

    SuperCam::EnableShake();
}

//=============================================================================
// DebugCam::DisableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DebugCam::DisableShake()
{
    SuperCam::DisableShake();
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// DebugCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DebugCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Debug", GetPlayerID() );
    
    radDbgWatchAddFloat( &mRotationAngleXZ, "Rotation Angle XZ", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &mRotationAngleY, "Rotation Angle Y", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &INCREMENT, "Rotation Increment", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &DIST, "Move Distance", nameSpace, NULL, NULL, 0.0f, 100.0f );
#endif
}

//=============================================================================
// DebugCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DebugCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mRotationAngleXZ );
    radDbgWatchDelete( &mRotationAngleY );
    radDbgWatchDelete( &INCREMENT );
    radDbgWatchDelete( &DIST );
#endif
}
