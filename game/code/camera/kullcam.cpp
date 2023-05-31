//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        KullCam.cpp
//
// Description: Implement KullCam
//
// History:     06/05/2002 + Created -- Cary Brisebois
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
#include <camera/KullCam.h>
#include <camera/supercamconstants.h>
#include <input/inputmanager.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifndef DEBUGWATCH
const float KULL_CAM_INCREMENT = 0.0174f; //One degree
//const float KULL_CAM_DIST = 1.0f;
const float KULL_CAM_DIST = 0.1f;
#else
float KULL_CAM_INCREMENT = 0.0174f;
//float KULL_CAM_DIST = 1.0f;
float KULL_CAM_DIST = 0.1f;
#endif


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// KullCam::KullCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
KullCam::KullCam() :
    mRotation( 0.0f ), //Along the side
    mElevation( 1.5707f ),
    mMagnitude( 5.0f ),
    mIgnoreDebugController( false )
{
    mIm = InputManager::GetInstance();
}

//==============================================================================
// KullCam::~KullCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
KullCam::~KullCam()
{
}

//=============================================================================
// KullCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void KullCam::Update( unsigned int milliseconds )
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
 
    if( !mIgnoreDebugController )
    {
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


        mRotation += ( xAxis * KULL_CAM_INCREMENT * timeMod );
        mElevation -= ( yAxis * KULL_CAM_INCREMENT * timeMod );
        mMagnitude -= ( zAxis * KULL_CAM_DIST * timeMod );
    }

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

    rmt::Vector targetPos;
    mTarget->GetPosition( &targetPos );

    rmt::Vector desiredPos, desiredTarget;
    desiredPos.Add( rod, targetPos );
    desiredTarget = targetPos;

    SetCameraValues( milliseconds, desiredPos, desiredTarget );    
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// KullCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void KullCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
#endif
}

//=============================================================================
// KullCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void KullCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
#endif
}