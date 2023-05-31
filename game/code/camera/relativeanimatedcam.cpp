//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        RelativeAnimatedCam.cpp
//
// Description: Implement RelativeAnimatedCam
//
// History:     24/04/2002 + Created -- Ian Gipson
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <p3d/anim/cameraanimation.hpp>
#include <camera/relativeanimatedcam.h>
#include <camera/supercammanager.h>

//========================================
// Definitions
//========================================

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// RelativeAnimatedCam::RelativeAnimatedCam():
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
RelativeAnimatedCam::RelativeAnimatedCam():
    gCameraAnimationController( NULL )
{
    mOffsetMatrix.Identity();
}

//==============================================================================
// RelativeAnimatedCam::CheckPendingCameraSwitch
//==============================================================================
// Description: checks if we've queued up a camera switch to the animated camera
//              because the camera system isn't always set up in time.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void RelativeAnimatedCam::CheckPendingCameraSwitch()
{
    if( CameraSwitchPending() )
    {
        LookupCamera();
        LookupMulticontroller();
        SuperCamManager* scm = GetSuperCamManager();
        SuperCamCentral* scc = scm->GetSCC( 0 );
        SuperCam*        sc  = scc->GetActiveSuperCam();
        if( sc == NULL )
        {
            AnimatedCam::SupressNextLetterbox();
            GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( ANIMATED_CAM );
        }
        else
        {
            SuperCam::Type type  = sc->GetType();
            if( type != RELATIVE_ANIMATED_CAM )
            {
                AnimatedCam::SupressNextLetterbox();
                GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( ANIMATED_CAM );
            }
        }
        SetCameraSwitchPending( false );
    }
}

//=============================================================================
// RelativeAnimatedCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
const char* const RelativeAnimatedCam::GetName() const
{
    return "RELATIVE_ANIMATED_CAM";
}

//=============================================================================
// RelativeAnimatedCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
SuperCam::Type RelativeAnimatedCam::GetType()
{
    return RELATIVE_ANIMATED_CAM;
}

//=============================================================================
// RelativeAnimatedCam::LetterBoxStart
//=============================================================================
// Description: start the letterbox for this camera - does nothing
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
void RelativeAnimatedCam::LetterBoxStart()
{
    //nothing
}

//=============================================================================
// RelativeAnimatedCam::LetterBoxStop
//=============================================================================
// Description: stop the letterbox for this camera - does nothing
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
void RelativeAnimatedCam::LetterBoxStop()
{
    //nothing
}

//=============================================================================
// RelativeAnimatedCam::SetOffsetMatrix
//=============================================================================
// Description: sets the matrix offset of the camera
//
// Parameters:	m - the matrix offset
//
// Return:      N/A.
//
//=============================================================================
void RelativeAnimatedCam::SetCameraAnimationController( tCameraAnimationController* controller )
{
    gCameraAnimationController = controller;
    if( gCameraAnimationController )
    {
        gCameraAnimationController->SetOffsetMatrix( mOffsetMatrix );
    }
}

//=============================================================================
// RelativeAnimatedCam::SetOffsetMatrix
//=============================================================================
// Description: sets the matrix offset of the camera
//
// Parameters:	m - the matrix offset
//
// Return:      N/A.
//
//=============================================================================
void RelativeAnimatedCam::SetOffsetMatrix( const rmt::Matrix& m )
{
    mOffsetMatrix = m;
    if( gCameraAnimationController != NULL )
    {
        gCameraAnimationController->SetOffsetMatrix( m );
    }
}

//=============================================================================
// RelativeAnimatedCam::Update
//=============================================================================
// Description: updates the camera.
//
// Parameters:	milliseconds - the nubmer of miliseconds elapsed
//
// Return:      N/A.
//
//=============================================================================
void RelativeAnimatedCam::Update( unsigned int milliseconds )
{
    AnimatedCam::Update( milliseconds );
}