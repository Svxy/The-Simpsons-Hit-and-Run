//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        firstpersoncam.cpp
//
// Description: Implement FirstPersonCam
//
// History:     2/21/2003 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radmath/radmath.hpp>

//========================================
// Project Includes
//========================================
#include <camera/firstpersoncam.h>
#include <camera/isupercamtarget.h>
#include <camera/supercamcontroller.h>
#include <camera/supercamconstants.h>
#include <camera/supercammanager.h>

#include <input/inputmanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
#define DEFAULT_MAGNITUDE 5.0f
#define DEFAULT_ROTATION rmt::PI_BY2
#define DEFAULT_ELEVATION rmt::PI_BY2

#define MAX_ROT_ANGLE rmt::DegToRadian( 80.0f )
#define MAX_ELEV_ANGLE rmt::DegToRadian( 80.0f )

#ifdef DEBUGWATCH
float FIRST_PERSON_CAM_MIN_FOV = SUPERCAM_DEFAULT_MIN_FOV;
float FIRST_PERSON_CAM_MAX_FOV = SUPERCAM_DEFAULT_MAX_FOV;
float FIRST_PERSON_CAM_LOOK_LAG = 0.05f;
#else
const float FIRST_PERSON_CAM_MIN_FOV = SUPERCAM_DEFAULT_MIN_FOV;
const float FIRST_PERSON_CAM_MAX_FOV = SUPERCAM_DEFAULT_MAX_FOV;
const float FIRST_PERSON_CAM_LOOK_LAG = SUPERCAM_DEFAULT_FOV_LAG;
#endif


//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// FirstPersonCam::FirstPersonCam
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
FirstPersonCam::FirstPersonCam() :
    mTarget( NULL ),
    mTargetDirty( false ),
    mRotation( DEFAULT_ROTATION ),
    mElevation( DEFAULT_ELEVATION ),
    mRotationDelta( 0.0f ),
    mElevationDelta( 0.0f ),
    mFOVDelta( 0.0f ),
    mCollisionOffset( NULL ),
    mNumCollisions( 0 )
{
    mTargetPositionOffset.Set( 0.0f, 0.0f, 0.0f );
}

//=============================================================================
// FirstPersonCam::~FirstPersonCam
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
FirstPersonCam::~FirstPersonCam()
{
}

//=============================================================================
// FirstPersonCam::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::OnInit()
{
    InitMyController();
}

//=============================================================================
// FirstPersonCam::OnShutdown
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::OnShutdown()
{
    //Reset the controller state if we're not in a new state since we went in.
    //Switching to pause when in first person will cause this to fail.
    if ( GetInputManager()->GetGameState() == Input::ACTIVE_FIRST_PERSON )
    {
        GetInputManager()->SetGameState( Input::ACTIVE_GAMEPLAY );
    }
}

//=============================================================================
// FirstPersonCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::Update( unsigned int milliseconds )
{
    rAssert( mTarget );

    if ( mTargetDirty )
    {
        //Update the position offset since we just got a new target.
        mTarget->GetFirstPersonPosition( &mTargetPositionOffset );
        mTargetDirty = false;
    }

    if ( GetInputManager()->GetGameState() == Input::ACTIVE_GAMEPLAY || GetInputManager()->GetGameState() == Input::ACTIVE_ALL )
    {
        GetInputManager()->SetGameState( Input::ACTIVE_FIRST_PERSON );
    }

    if ( GetFlag((Flag)FIRST_TIME) || GetFlag((Flag)CUT) )
    {
        mRotation = DEFAULT_ROTATION;
        mElevation = DEFAULT_ELEVATION;
        mRotationDelta = 0.0f;
        mElevationDelta = 0.0f;
        mFOVDelta = 0.0f;

        SetFlag( (Flag)FIRST_TIME, false );
        SetFlag( (Flag)CUT, false );
    }

    //---------  Calculate positino and target

    float timeMod = milliseconds / 16.0f;

    //place the target at the position and deal with controller input.

    rmt::Vector position, target;
    mTarget->GetPosition( &position );
    position.Add( mTargetPositionOffset );

    //Take controller values and calculate desired rotation and position.
    float desiredRot, desiredElev;
    desiredRot = mRotation + ( MAX_ROT_ANGLE * -(mController->GetAxisValue( SuperCamController::stickX )) );

    float invert = -1.0f;
    if ( GetSuperCamManager()->GetSCC( GetPlayerID() )->IsInvertedCameraEnabled() )
    {
        invert = 1.0f;
    }

    desiredElev = MAX_ELEV_ANGLE * ( invert * mController->GetAxisValue( SuperCamController::stickY ) ) + DEFAULT_ELEVATION;

    float lag = FIRST_PERSON_CAM_LOOK_LAG * timeMod;
    CLAMP_TO_ONE( lag );

    MotionCubic( &mRotation, &mRotationDelta, desiredRot, lag );
    MotionCubic( &mElevation, &mElevationDelta, desiredElev, lag );

    rmt::SphericalToCartesian( DEFAULT_MAGNITUDE, mRotation, mElevation, &target.x, &target.z, &target.y );

    rmt::Vector targetHeading;
    mTarget->GetHeading( &targetHeading );
    rmt::Vector targetVUP;
    mTarget->GetVUP( &targetVUP );

    rmt::Matrix mat;
    mat.Identity();
    mat.FillHeading( targetHeading, targetVUP );

    target.Transform( mat );
    target.Add( position );

    //---------  Goofin' with the FOV

    float zoom = mController->GetValue( SuperCamController::zToggle );

    float FOV = GetFOV();
    FilterFov( zoom, FIRST_PERSON_CAM_MIN_FOV, FIRST_PERSON_CAM_MAX_FOV, FOV, mFOVDelta, SUPERCAM_DEFAULT_FOV_LAG, timeMod );

    SetFOV( FOV );

    //---------  Set values

    SetCameraValues( milliseconds, position, target );
}

//=============================================================================
// FirstPersonCam::UpdateForPhysics
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::UpdateForPhysics( unsigned int milliseconds )
{
    if ( mNumCollisions )
    {
        rmt::Vector offset( 0.0f, 0.0f, 0.0f );

        unsigned int i;
        for ( i = 0; i < mNumCollisions; ++i )
        {
            offset += mCollisionOffset[ i ];
        }

        //offset.x /= mNumCollisions;
        //offset.y /= mNumCollisions;
        //offset.z /= mNumCollisions;

        rmt::Vector camPos;
        GetPosition( &camPos );

        rmt::Vector camTarg;
        GetTarget( &camTarg );

        camPos += offset;
        camTarg += offset;

        SetCameraValues( 0, camPos, camTarg );
    }

    if ( mTarget->IsUnstable() )
    {
        int controllerID = GetInputManager()->GetControllerIDforPlayer( GetPlayerID() );
        GetSuperCamManager()->GetSCC( 0 )->ToggleFirstPerson( controllerID );
    }
}

//=============================================================================
// FirstPersonCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
    mTargetDirty = true;
}

//=============================================================================
// FirstPersonCam::OverrideOldState
//=============================================================================
// Description: Comment
//
// Parameters:  ( Input::ActiveState state )
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::OverrideOldState( Input::ActiveState state )
{
    mOldState = state;
}


//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// FirstPersonCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\1st Person", GetPlayerID() );

    radDbgWatchAddFloat( &FIRST_PERSON_CAM_MIN_FOV, "Min FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &FIRST_PERSON_CAM_MAX_FOV, "Max FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &FIRST_PERSON_CAM_LOOK_LAG, "Look Lag", nameSpace, NULL, NULL, 0.0f, rmt::PI );
#endif
}

//=============================================================================
// FirstPersonCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FirstPersonCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &FIRST_PERSON_CAM_MIN_FOV );
    radDbgWatchDelete( &FIRST_PERSON_CAM_MAX_FOV );
    radDbgWatchDelete( &FIRST_PERSON_CAM_LOOK_LAG );
#endif
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
