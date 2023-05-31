//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        StaticCam.cpp
//
// Description: Implement StaticCam
//
// History:     9/18/2002 + Created -- Cary Brisebois
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
#ifndef WORLD_BUILDER
#include <camera/StaticCam.h>
#include <camera/isupercamtarget.h>
#include <camera/supercamcontroller.h>
#include <camera/supercamconstants.h>
#include <p3d/pointcamera.hpp>
//TODO: I only really need the tuner variables...  Break this file up.
#include <worldsim/character/character.h>
#else
#include "StaticCam.h"
#include "isupercamtarget.h"
#include "supercamcontroller.h"
#include "supercamconstants.h"
class tPointCamera
{
public:
    void GetFOV( float* fov, float* aspect ) { *fov = 1.5707f; *aspect = 4.0f / 3.0f; };
};

namespace CharacterTune
{
    static float sfMaxSpeed;
    static float sfDashBurstMax;
};
#endif


#ifdef DEBUGWATCH
float STATIC_CAM_MIN_FOV = SUPERCAM_DEFAULT_MIN_FOV;
#else
const float STATIC_CAM_MIN_FOV = SUPERCAM_DEFAULT_MIN_FOV;
#endif

const float STATIC_CAM_FOV_LAG = 0.022f;

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
// StaticCam::StaticCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
StaticCam::StaticCam() :
    mTarget( NULL ),
    mTargetLag( 1.0f ),
    mTracking( false ),
    mFOVDelta( 0.0f ),
    mMaxFOV( 1.5707f ),
    mFOVLag( STATIC_CAM_FOV_LAG )
{
    mTargetOffset.Set( 0.0f, 0.0f, 0.0f );
    mTargetPosition.Set( 0.0f, 0.0f, 0.0f );
    mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// StaticCam::~StaticCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
StaticCam::~StaticCam()
{
}

//=============================================================================
// StaticCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void StaticCam::Update( unsigned int milliseconds )
{
    //This is to adjust interpolation when we're running substeps.
    float timeMod = (float)milliseconds / EXPECTED_FRAME_RATE;

    rmt::Vector targ;

    if ( mTracking )
    {
        mTarget->GetPosition( &targ );

        if( mTargetOffset.x != 0.0f ||
            mTargetOffset.y != 0.0f ||
            mTargetOffset.z != 0.0f )
        {
            rmt::Matrix mat;
            mat.Identity();
        
            rmt::Vector heading, vup;
            mTarget->GetHeading( &heading );
            mTarget->GetVUP( &vup );

            mat.FillHeading( heading, vup );

            rmt::Vector offset = mTargetOffset;
            offset.Transform( mat );

            targ.Add( offset );
        }
    }
    else
    {
        //The offset is actually a world coordinate position to look at.
        targ = mTargetOffset;
    }

    rmt::Vector desiredTarget = targ;

    bool cut, firstTime;
    cut = GetFlag( (Flag)CUT );
    firstTime = GetFlag( (Flag)FIRST_TIME );

    if ( cut || firstTime )
    {
        //Cutting camera.
        mTargetPosition = desiredTarget;
        mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );

        //SetFOV( mMaxFOV );
        mFOVDelta = 0.0f;

        if ( cut )
        {
            if ( GetFlag( (Flag)START_TRANSITION ) )
            {
                SetFlag( (Flag)START_TRANSITION, false );
            }
            else if ( GetFlag( (Flag)TRANSITION ) )
            {
                SetFlag( (Flag)END_TRANSITION, true );
            }

            SetFlag( (Flag)CUT, false );
        }
        else
        {
            //Not cutting, so let's try this.
            float fov, aspect = 0.0f;
            GetCamera()->GetFOV( &fov, &aspect );
            SetFOV( fov );
        }

        SetFlag( (Flag)FIRST_TIME, false );
    }

    if ( mTracking )
    {
        float lag = mTargetLag * timeMod;
        CLAMP_TO_ONE( lag );
        MotionCubic( &mTargetPosition.x, &mTargetPositionDelta.x, desiredTarget.x, lag );
        MotionCubic( &mTargetPosition.y, &mTargetPositionDelta.y, desiredTarget.y, lag );
        MotionCubic( &mTargetPosition.z, &mTargetPositionDelta.z, desiredTarget.z, lag );
    }
    else
    {
        mTargetPosition = desiredTarget;
    }

    //---------  Goofin' with the FOV

    if ( ( GetFlag( (Flag)TRANSITION ) ||
         ( !GetFlag( SuperCam::WRECKLESS_ZOOM ) && mTarget->IsCar() ) ) )
    {
        SetFOV( mMaxFOV );

        //Reset this.
        mFOVDelta = 0.0f;
    }
    else if ( GetFlag( SuperCam::WRECKLESS_ZOOM ) )
    {
        rmt::Vector posToTarg;
        posToTarg.Sub( mTargetPosition, mPosition );
        float dist = posToTarg.Magnitude(); //Square root!
        float FOV = GetFOV();
        float FOVDelta = 0.0f;
        DoWrecklessZoom( dist, 10.0f, 25.0f, STATIC_CAM_MIN_FOV, mMaxFOV, FOV, FOVDelta, mFOVLag, timeMod );

        SetFOV( FOV );
    }
    else
    {
#ifndef WORLD_BUILDER
        float zoom = mController->GetValue( SuperCamController::zToggle );
#else
        float zoom = 0.0f;
#endif
        float FOV = GetFOV();
        FilterFov( zoom, STATIC_CAM_MIN_FOV, mMaxFOV, FOV, mFOVDelta, mFOVLag, timeMod );

        SetFOV( FOV );
    }


    //---------  Set values.

    SetCameraValues( milliseconds, mPosition, mTargetPosition );
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// StaticCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void StaticCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Static", GetPlayerID() );

    radDbgWatchAddFloat( &mFOVLag, "FOV Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &STATIC_CAM_MIN_FOV, "FOV Min", nameSpace, NULL, NULL, 0.0f, rmt::PI_BY2 );
#endif
}

//=============================================================================
// StaticCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void StaticCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mFOVLag );
    radDbgWatchDelete( &STATIC_CAM_MIN_FOV );
#endif
}

//=============================================================================
// StaticCam::GetTargetSpeedModifier
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float StaticCam::GetTargetSpeedModifier()
{
    rmt::Vector vel;
    mTarget->GetVelocity( &vel );
    float speed = vel.Magnitude();  //Square root!

    //TODO:I wish this was a const.
    if ( speed < CharacterTune::sfMaxSpeed || rmt::Epsilon( speed, CharacterTune::sfMaxSpeed, 0.01f ) )
    {
        return 1.0f;
    }

    float maxMod = CharacterTune::sfMaxSpeed + CharacterTune::sfDashBurstMax / CharacterTune::sfMaxSpeed;
    float modifier = (speed - CharacterTune::sfMaxSpeed) / CharacterTune::sfDashBurstMax * maxMod;  
    rAssert( modifier > 0.01f );

    return modifier;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
