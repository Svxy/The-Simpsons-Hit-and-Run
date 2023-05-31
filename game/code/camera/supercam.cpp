//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supercam.cpp
//
// Description: Implement SuperCam
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
//These are included in the precompiled headers on XBOX and GC
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <radmath/radmath.hpp>

#include <camera/supercamcontroller.h>
#include <input/inputmanager.h>

#ifdef WORLD_BUILDER
#include "main/toolhack.h"
#endif

#include <p3d/pointcamera.hpp>

//========================================
// Project Includes
//========================================
#ifndef WORLD_BUILDER
#include <camera/SuperCam.h>
#include <camera/icamerashaker.h>
#include <camera/supercamconstants.h>

#include <memory/srrmemory.h>
#else
#include "SuperCam.h"
#include "icamerashaker.h"
#include "supercamconstants.h"
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP(x)
#endif

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

// DEFAULTS
//const float SUPERCAM_FOV = 1.57079f;        //90 Degrees
//const float SUPERCAM_ASPECT = 4.0f / 3.0f;  //Standard aspect ratio
//const float SUPERCAM_NEAR = 0.1f;           
//const float SUPERCAM_FAR = 20000.0f;

const float CAMERA_DEAD_ZONE = 0.02f;       //This is a controller dead zone
const float CAMERA_ROTATE_SPEED = 0.01f;    //How fast to rotate camera

#ifdef DEBUGWATCH
unsigned int TRANSITION_TIME_LIMIT = 0; //This is in miliseconds
float FOV_TRANSITION_RATE = 0.122f;
float MAX_MODIFIER = 30.0f;
#else 
//NOTE!!!!
//The values for TRANSITION_RATE and TARGET_TRANSITION RATE should be very close!!!
//How close?  Within 0.001 or you will get some strange effects when transitioning!
const unsigned int TRANSITION_TIME_LIMIT = 0; //This is in miliseconds
const float FOV_TRANSITION_RATE = 0.122f;
const float MAX_MODIFIER = 30.0f;
#endif

const float TRANSITION_RATE = 0.02f;
const float TARGET_TRANSITION_RATE = 0.02f;

unsigned int SuperCam::s_secondaryControllerID = 1;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperCam::SuperCam
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
SuperCam::SuperCam() : 
    mFlags( 0 ),
    mController( NULL ),
    mControllerHandle( -1 ),
    mCamera( NULL ),
    mSCFOV( SUPERCAM_FOV ),
    mSCAspect( SUPERCAM_ASPECT ),
    mSCNearPlane( SUPERCAM_NEAR ),
    mSCFarPlane( SUPERCAM_FAR ),
    mCameraVirtualFOV( 0.0f ),
    mCameraVirtualFOVDelta( 0.0f ),
    mTransitionTime( 0 ),
    mTransitionTimeLimit( TRANSITION_TIME_LIMIT ),
    mTransitionPositionRate( TRANSITION_RATE ),
    mTransitionTargetRate( TARGET_TRANSITION_RATE ),
    mFOVTransitionRate( FOV_TRANSITION_RATE ),
    mShaker( NULL ),
    mPlayerID( -1 ),
    mSCBackupFOV( SUPERCAM_FOV ),
    mSCTwist( 0.0f ),
    mOldTwist( 0.0f ),
    mTwistDelta( 0.0f ),
    mTwistLag( 0.01f )
{
    //This get set automatically for all camera's.  It's up to the camera
    //to care if this is set or not. And to clear it.
    SetFlag( FIRST_TIME, true );
    SetFlag( DIRTY_HEADING, true );

    mVelocity.Set(0.0f,0.0f,0.0f); 
    mCameraVirtualPosition.Set(0.0f,0.0f,0.0f);
    mCameraVirtualTarget.Set(0.0f,0.0f,0.0f);
    mCameraVirtualPositionDelta.Set(0.0f,0.0f,0.0f);
    mCameraVirtualTargetDelta.Set(0.0f,0.0f,0.0f);
    mNormalizedHeading.Set(0.0f, 0.0f, 1.0f);

#ifndef WORLD_BUILDER
    if(p3d::display->IsWidescreen())
    {
        mSCAspect = ( 16.0f / 9.0f );
    }
#endif
}

//=============================================================================
// SuperCam::~SuperCam
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
SuperCam::~SuperCam()
{
    if ( mCamera )
    {
        mCamera->Release();
    }

    if ( mController )
    {
        InputManager* im = InputManager::GetInstance();
        rAssert( im );

        int controllerID = GetInputManager()->GetControllerIDforPlayer( this->GetPlayerID() );
        im->UnregisterMappable( static_cast<unsigned int>( controllerID ), mControllerHandle );
        mController->Release();
    }
}

//=============================================================================
// SuperCam::RegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::RegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Base", GetPlayerID() );

    radDbgWatchAddFloat( &mSCFOV, "FOV", nameSpace, NULL, NULL, 0, rmt::PI_2 );
    radDbgWatchAddFloat( &mSCAspect, "Aspect Ratio", nameSpace, NULL, NULL, 0, 5.0f );
    radDbgWatchAddFloat( &mSCNearPlane, "Near Plane", nameSpace, NULL, NULL, -1.0f, 30.0f );
    radDbgWatchAddFloat( &mSCFarPlane, "Far Plane", nameSpace, NULL, NULL, 0, 20000.0f );

    radDbgWatchAddUnsignedInt( &TRANSITION_TIME_LIMIT, "Transition Time", nameSpace, NULL, NULL, 0, 10000 );
    radDbgWatchAddFloat( &mTransitionPositionRate, "Transition Rate", nameSpace, NULL, NULL, 0, 1.0f );
    radDbgWatchAddFloat( &mTransitionTargetRate, "Target Transition Rate", nameSpace, NULL, NULL, 0, 1.0f );
    radDbgWatchAddFloat( &FOV_TRANSITION_RATE, "FOV Transition Rate", nameSpace, NULL, NULL, 0, 1.0f );

    radDbgWatchAddFloat( &MAX_MODIFIER, "Static|Rail max tran speed", nameSpace, NULL, NULL, 10.0f, 100.0f );

    radDbgWatchAddFloat( &mSCTwist, "Camera Twist", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &mTwistLag, "Camera Twist Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
#endif

    OnRegisterDebugControls();
}

//=============================================================================
// SuperCam::UnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::UnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mSCFOV );
    radDbgWatchDelete( &mSCAspect );
    radDbgWatchDelete( &mSCNearPlane );
    radDbgWatchDelete( &mSCFarPlane );

    radDbgWatchDelete( &TRANSITION_TIME_LIMIT );
    radDbgWatchDelete( &mTransitionPositionRate );
    radDbgWatchDelete( &mTransitionTargetRate );
    radDbgWatchDelete( &FOV_TRANSITION_RATE );

    radDbgWatchDelete( &MAX_MODIFIER );

    radDbgWatchDelete( &mSCTwist );
    radDbgWatchDelete( &mTwistLag );
#endif

    OnUnregisterDebugControls();
}

//=============================================================================
// SuperCam::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position )
//
// Return:      void 
//
//=============================================================================
void SuperCam::GetPosition( rmt::Vector* position ) const
{
    if( mCamera )
    {
        if ( GetFlag( (Flag)SHAKE ) || GetFlag( (Flag)TRANSITION ) )
        {
            *position = mCameraVirtualPosition;
        }
        else
        {
            mCamera->GetPosition( position );
        }
    }
    else
    {
        position->Set( 0.0f, 0.0f, 0.0f );
    }
}

//=============================================================================
// SuperCam::GetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* target )
//
// Return:      void 
//
//=============================================================================
void SuperCam::GetTarget( rmt::Vector* target ) const
{
    if( mCamera )
    {
        if ( GetFlag( (Flag)SHAKE ) || GetFlag( (Flag)TRANSITION ) )
        {
            *target = mCameraVirtualTarget;
        }
        else
        {
            mCamera->GetTarget( target );
        }
    }
    else
    {
        target->Set(0,0,0);
    }
}

//=============================================================================
// SuperCam::GetHeading
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* heading )
//
// Return:      void 
//
//=============================================================================
void SuperCam::GetHeading( rmt::Vector* heading ) const
{
    if ( mCamera )
    {
        rmt::Vector pos, targ;
        if ( GetFlag((Flag)SHAKE ) || GetFlag( (Flag)TRANSITION ) )
        {
            pos = mCameraVirtualPosition;
            targ = mCameraVirtualTarget;
        }
        else
        {
            mCamera->GetPosition( &pos );
            mCamera->GetTarget( &targ );
        }

        heading->Sub( targ, pos );
    }
    else
    {
        heading->Set(0,0,0);
    }
}

//=============================================================================
// SuperCam::GetHeadingNormalized
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* heading )
//
// Return:      void 
//
//=============================================================================
void SuperCam::GetHeadingNormalized( rmt::Vector* heading )
{
    rAssert( mCamera );

    if ( GetFlag((Flag)DIRTY_HEADING) )
    {
        GetHeading( &mNormalizedHeading );
        mNormalizedHeading.Normalize();
        SetFlag((Flag)DIRTY_HEADING, false);
    }

    rAssert(mNormalizedHeading.MagnitudeSqr() != 0.0f);

    *heading = mNormalizedHeading;
}

//=============================================================================
// SuperCam::GetCameraUp
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* up )
//
// Return:      void 
//
//=============================================================================
void SuperCam::GetCameraUp( rmt::Vector* up ) const
{
    if ( mCamera )
    {
        //This shouldn't change during the SHAKE or TRANSITION
        mCamera->GetVUp( up );
    }
    else
    {
        up->Set(0,0,0);
    }
}

//=============================================================================
// SuperCam::SetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float FOVinRadians )
//
// Return:      void 
//
//=============================================================================
void SuperCam::SetFOV( float FOVinRadians )
{
    if ( GetFlag((Flag)OVERRIDE_FOV ) )
    {
        //We' are overriding, this will be the regular FOV
        //when we're done
        mSCBackupFOV = FOVinRadians;
    }
    else if ( GetFlag( (Flag)SHAKE ) || GetFlag( (Flag)TRANSITION ) )
    {
        //Middle of transition.
        mCameraVirtualFOV = FOVinRadians;
        mCameraVirtualFOVDelta = 0.0f;
    }   
    else
    {
        //The is the regular FOV of the camera.
        mSCFOV = FOVinRadians;
    }
}

//=============================================================================
// SuperCam::GetFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float (in radians)
//
//=============================================================================
float SuperCam::GetFOV() const
{
    if ( GetFlag((Flag)SHAKE ) || GetFlag( (Flag)TRANSITION ) )
    {
        return mCameraVirtualFOV;
    }
    else
    {
        return mSCFOV;
    }
}

//=============================================================================
// SuperCam::Display
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::Display() const
{
#ifdef SUPERCAM_DEBUG
    //We could display the name of the camera or something.

    OnDisplay();
#endif
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//==============================================================================
// SuperCam::SetCameraValues
//==============================================================================
// Description: This is the best way to change the position and target of the 
//              camera.  If you do not pass in a VUP vector one will be 
//              calculated for you.
//              Also, if DEBUG is defined then the values of the camera matrix 
//              will be checked.
//
// Parameters:  ( unsigned int milliseconds, 
//                rmt::Vector pos, 
//                rmt::Vector targ, 
//                const rmt::Vector* vup )
//
// Return:      void 
//
//==============================================================================
void SuperCam::SetCameraValues( unsigned int milliseconds,
                                rmt::Vector pos, 
                                rmt::Vector targ, 
                                const rmt::Vector* vup )
{
#ifdef PROFILER_ENABLED
        char sCname[256];
        sprintf( sCname, "SC: %d SetCameraValues", mPlayerID );
        BEGIN_PROFILE( sCname )
#endif

    rAssert( !rmt::Epsilon( pos.MagnitudeSqr(), 0.0f, 0.01f ) );
    
    CorrectDist( pos, targ );

    rAssertMsg( mPlayerID >= 0, "You need to set a player ID before anyting else!" );
    rAssertMsg( !(GetFlag( (Flag)SHAKE ) && GetFlag( (Flag)TRANSITION )), "Do not shake the camera while transitioning!" );

    if ( GetFlag( (Flag)START_SHAKE ) )
    {
        SetupShake();
    }
    else if ( GetFlag( (Flag)END_SHAKE ) )
    {
        EndShake();
    }
    else if ( GetFlag( (Flag)START_TRANSITION ) )
    {
        SetupTransition();
    }
    else if ( GetFlag( (Flag)END_TRANSITION ) )
    {
        EndTransition();
    }

        //Calculate the velocity of the camera
    rmt::Vector oldPos;
    GetPosition( &oldPos );
    
    if ( GetFlag( (Flag)SHAKE ) || GetFlag( (Flag)TRANSITION ) )
    {
        //If we're doing an ease in, we don't do this...
        if ( !GetFlag( (Flag)EASE_IN ) )
        {
            //Store the position that the super cam THINKS it's at...
            mCameraVirtualPosition = pos;
            mCameraVirtualTarget = targ;
        }

        if ( GetFlag( (Flag)SHAKE ) )
        {
            mShaker->ShakeCamera( &pos, &targ, milliseconds );
        }
        else
        {

            if ( GetFlag( (Flag)EASE_IN ) )
            {
                if( mTransitionPositionRate >= 1.0f )
                {
                    //Hackish
                    //Emulate a cut...  Clean this up sometime eh?
                    mTransitionTime = mTransitionTimeLimit - 1;
                }
                else
                {
                    float timeMod = rmt::LtoF(milliseconds) / EXPECTED_FRAME_RATE;
                    float timeEffect = MAX_MODIFIER * mTransitionPositionRate;

                    //Also take into account the target's velocity.
                    //This allows us to transition faster when the character is 
                    //moving quickly.
                    timeEffect *= GetTargetSpeedModifier();

                    rDebugPrintf( "timeEffect: %.4f\n", timeEffect );

                    mTransitionTime += rmt::FtoL((rmt::LtoF(milliseconds) * timeEffect));
                }
            }
            else
            {
                mTransitionTime += milliseconds;
            }


            rmt::Vector distPos, distTarg;
            rmt::Vector actPos, actTarg;
            mCamera->GetPosition( &actPos );
            mCamera->GetTarget( &actTarg );

            distPos.Sub( pos, actPos );
            distTarg.Sub( targ, actTarg );

            if ( mTransitionTime >= mTransitionTimeLimit ||
                 (rmt::Epsilon( distPos.MagnitudeSqr(), 0.0f,  0.001f ) &&
                  rmt::Epsilon( distTarg.MagnitudeSqr(), 0.0f, 0.001f ) &&
                  rmt::Epsilon( mSCFOV, mCameraVirtualFOV, 0.001f ) )
               )
            {
                //Turn it off now.
                SetFlag( (Flag)END_TRANSITION, true );
                EndTransition();
            }
            else
            {
                if ( GetFlag( (Flag)EASE_IN ) )
                {
                    //Do the Ease in
                    EaseIn( rmt::LtoF(mTransitionTime) / rmt::LtoF(mTransitionTimeLimit), &pos, &targ, milliseconds );
                    TransitionFOV( rmt::LtoF(mTransitionTime) / rmt::LtoF(mTransitionTimeLimit), &mSCFOV, milliseconds );
                }
                else
                {
                    //Do the Transition
                    TransitionCamera( rmt::LtoF(mTransitionTime) / rmt::LtoF(mTransitionTimeLimit), &pos, &targ, &mSCFOV, milliseconds, GetFlag( (Flag)QUICK_TRANSITION ) );
                }
            }        
        }
    }

    //Better place for the velocity calculation.
    mVelocity.Sub( pos, oldPos );    

    //Update the position of the camera and set the VUP
    mCamera->SetPosition( pos );
    mCamera->SetTarget( targ );

    SetFlag((Flag)DIRTY_HEADING, true);

    if ( vup )
    {
        mCamera->SetVUp( *vup );
    }
    else
    {
        mCamera->SetVUp( UpdateVUP( pos, targ ) );
    }

    //This will be cool in the halloween level
    float oldTwist = mCamera->GetTwist();
    if ( !rmt::Epsilon( mSCTwist, oldTwist, 0.001f ) )
    {
        AdjustAngles( &mSCTwist, &oldTwist, &mTwistDelta );
        MotionCubic( &oldTwist, &mTwistDelta, mSCTwist, mTwistLag );
    }

    mCamera->SetTwist( oldTwist );

    //Write the overriding FOV, aspect and near and far planes.
    mCamera->SetFOV( mSCFOV, mSCAspect );
    mCamera->SetNearPlane( mSCNearPlane );
    mCamera->SetFarPlane( mSCFarPlane );

#ifdef RAD_DEBUG
    TestCameraMatrix();
#endif
    
#ifdef PROFILER_ENABLED
        END_PROFILE( sCname )
#endif

//    char out[256];
//    sprintf( out, "pos:  %.2f, %.2f, %.2f targ: %.2f, %.2f, %.2f\n", pos.x, pos.y, pos.z, targ.x, targ.y, targ.z );
//    rDebugString( out );
}

//=============================================================================
// SuperCam::OverrideFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool enable, float time )
//
// Return:      void 
//
//=============================================================================
void SuperCam::OverrideFOV( bool enable, float time, float rate )
{
    if ( enable != GetFlag( (Flag)OVERRIDE_FOV ) )
    {
        //Toggling

        if ( enable )
        {
            //Turning on.
            if ( GetFlag( (Flag)TRANSITION ) )
            {
                //Switch the stored override value for the current regular value
                //which is stored in the virtual fov since we're tansitioning.
                float tempFOV = mSCBackupFOV;
                mSCBackupFOV = mCameraVirtualFOV;
                mCameraVirtualFOV = tempFOV;
            }
            else
            {
                //Swap the regular fov for the stored override FOV
                float tempFOV = mSCBackupFOV;
                mSCBackupFOV = mSCFOV;
                mSCFOV = tempFOV;
            }
        }
        else
        {
            //Bring the regular FOV value back.
            if ( GetFlag( (Flag)TRANSITION ) )
            {
                mCameraVirtualFOV = mSCBackupFOV;
            }
            else
            {
                mSCFOV = mSCBackupFOV;
            }

            //Clear out the stored FOV.
            mSCBackupFOV = 0.0f;
        }

        //Use the transition system to transition the 
        //fov..
        mTransitionTimeLimit = rmt::FtoL(time * 1000.0f);
        mFOVTransitionRate = rate;

        SetFlag( (Flag)OVERRIDE_FOV, enable );
    
        if ( !GetFlag((Flag)TRANSITION ) )
        {
            SetupTransition( true );
        }
    }
}

//=============================================================================
// SuperCam::DisableOverride
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::DisableOverride()
{
    if ( GetFlag( (Flag)OVERRIDE_FOV ) )
    {
        if ( GetFlag( (Flag)TRANSITION ) )
        {
            mCameraVirtualFOV = mSCBackupFOV;
        }
        else
        {
            mSCFOV = mSCBackupFOV;
        }

        mSCBackupFOV = 0.0f;

        SetFlag( (Flag)OVERRIDE_FOV, false );
    }
}
    

//=============================================================================
// SuperCam::SetFOVOverride
//=============================================================================
// Description: Comment
//
// Parameters:  ( float newFOV )
//
// Return:      void 
//
//=============================================================================
void SuperCam::SetFOVOverride( float newFOV )
{
    if ( GetFlag( (Flag)OVERRIDE_FOV) )
    {
        if ( GetFlag( (Flag)TRANSITION ) )
        {
            //Middle of a transition, set the virtual FOV...
            mCameraVirtualFOV = newFOV;
            mCameraVirtualFOVDelta = 0.0f;  //Must reset when we change this!
        }
        else
        {
            //Already overriding, need to set a new number
            mSCFOV = newFOV;
        }
    }
    else
    {
        //Hang on to the override until we need it.
        mSCBackupFOV = newFOV;
    }
}

//=============================================================================
// SuperCam::SetCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( tPointCamera* cam )
//
// Return:      void 
//
//=============================================================================
void SuperCam::SetCamera( tPointCamera* cam )
{
    if ( mCamera )
    {
        mCamera->Release();
    }

    mCamera = cam;
    mCamera->AddRef();
}

//=============================================================================
// SuperCam::ClampAngle
//=============================================================================
// Description: Comment
//
// Parameters:  ( float* angle, bool* clamped )
//
// Return:      int (This number of times PI_2 was added
//
//=============================================================================
int SuperCam::ClampAngle( float* angle ) const
{
    int total = 0;

    if ( *angle < 0.0f )
    {
        do
        {
            //Add 360 until the number is positive
            *angle += rmt::PI_2;
            ++total;
        }
        while ( *angle < 0.0f );
    }
    else if ( *angle > rmt::PI_2 )
    {
        do
        {
            //Subtract 360 until number is <= 360
            *angle -= rmt::PI_2;
            --total;
        }
        while ( *angle > rmt::PI_2 );
    }
   
    return total;
}

//=============================================================================
// SuperCam::AdjustAngles
//=============================================================================
// Description: Comment
//
// Parameters:  ( float* desiredAngle, 
//                float* currentAngle, 
//                float* curentAngleDelta )
//
// Return:      void 
//
//=============================================================================
void SuperCam::AdjustAngles( float* desiredAngle, 
                             float* currentAngle, 
                             float* currentAngleDelta ) const
{
    //First we should get all values within 0 360 ( 0 - 2 PI )
    ClampAngle( desiredAngle );    
    ClampAngle( currentAngle );
  
    //We only want to interpolate to a rotation via the fewest number of degrees.
    if ( rmt::Fabs( *desiredAngle - *currentAngle ) > rmt::PI ) //More than 180deg
    {
        //They are too far apart, which way to rotate?
        if ( *desiredAngle - *currentAngle < 0.0f )
        {
            //desiredAngle is bigger than currentAngle by >= 180 deg
            //rotate towards 0
            //Add 360 deg 
            *desiredAngle += rmt::PI_2;
        }
        else
        {
            //desiredAngle is smaller than currentAngle by >= 180 deg
            //rotate away from 0
            //Subtract 360 deg
            *desiredAngle -= rmt::PI_2;
        }
    }
}

//=============================================================================
// SuperCam::Shutdown
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::Shutdown()
{
    DisableOverride();
    EndTransition( true );
    ShutDownMyController();
    OnShutdown(); //Tell the child..
}

//=============================================================================
// SuperCam::CorrectDist
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& pos, rmt::Vector& targ )
//
// Return:      void 
//
//=============================================================================
void SuperCam::CorrectDist( const rmt::Vector& pos, rmt::Vector& targ )
{
    //Make sure pos and target aren't too close!
    rmt::Vector pos2targ;
    pos2targ.Sub( targ, pos );
    if ( pos2targ.MagnitudeSqr() < 1.0f )
    {
        pos2targ.NormalizeSafe();
        pos2targ.Scale( 1.0f );

        targ.Add( pos, pos2targ );

        // rDebugString( "\n\nWARNING! Adjusting camera targ as it is too close to the position!\n\n" );
    }
}

//=============================================================================
// SuperCam::InitMyController
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerID )
//
// Return:      void 
//
//=============================================================================
void SuperCam::InitMyController( int controllerID )
{
MEMTRACK_PUSH_GROUP( "SuperCam" );
    if ( mController == NULL )
    {
        mController = new SuperCamController();
        rAssert( mController );
        mController->AddRef();
    }

    if ( mControllerHandle == -1 )
    {
        InputManager* im = InputManager::GetInstance();
        rAssert( im );

        if ( controllerID == -1 )
        {
            //Get a new one, otherwise use the passed in one.
            controllerID = GetInputManager()->GetControllerIDforPlayer( this->GetPlayerID() );
        }

        if (controllerID != -1 )
        {
            mControllerHandle = im->RegisterMappable( static_cast<unsigned int>( controllerID ), mController );
        }
    }
MEMTRACK_POP_GROUP("SuperCam");
}

//=============================================================================
// SuperCam::ShutDownMyController
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::ShutDownMyController()
{
    if ( mControllerHandle != -1 )
    {
        InputManager* im = InputManager::GetInstance();
        rAssert( im );

        rTuneAssert( mControllerHandle != -1 );
        int controllerID = GetInputManager()->GetControllerIDforPlayer( this->GetPlayerID() );
        im->UnregisterMappable( static_cast<unsigned int>( controllerID ), mController );
        mControllerHandle = -1;
    }
}

//=============================================================================
// SuperCam::GetCameraNonConst
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      tPointCamera
//
//=============================================================================
tPointCamera* SuperCam::GetCameraNonConst()
{
    return mCamera;
}

//=============================================================================
// SuperCam::SphericalMotion
//=============================================================================
// Description: Comment
//
// Parameters:  (  const rmt::Vector& target, 
//                 rmt::Vector& currentPos, 
//                 rmt::Vector& desiredPos, 
//                 rmt::Vector& desiredPosDelta, 
//                 float rate, 
//                 float timeMod )
//
// Return:      void 
//
//=============================================================================
void SuperCam::SphericalMotion(  const rmt::Vector& target, 
                               rmt::Vector& currentPos, 
                               const rmt::Vector& desiredPos, 
                               rmt::Vector& desiredPosDelta, 
                               const float rate ) const
{
#ifdef PROFILER_ENABLED
    char sCname[256];
    sprintf( sCname, "SC: %d Spherical Motion", mPlayerID );
    BEGIN_PROFILE( sCname )
#endif

        rmt::Vector virtTargToPos;
    rmt::Vector virtTargToVirtPos;

    virtTargToPos.Sub( currentPos, target );
    virtTargToVirtPos.Sub( desiredPos, target );

    float actRot, actElev, actMag;
    float virtRot, virtElev, virtMag;

    rmt::CartesianToSpherical( virtTargToPos.x, virtTargToPos.z, virtTargToPos.y, &actMag, &actRot, &actElev );
    rmt::CartesianToSpherical( virtTargToVirtPos.x, virtTargToVirtPos.z, virtTargToVirtPos.y, &virtMag, &virtRot, &virtElev );

    //Clean up the angles.
    AdjustAngles( &virtRot, &actRot, &desiredPosDelta.x );
    AdjustAngles( &actElev, &actElev, &desiredPosDelta.y );

    MotionCubic( &actRot, &desiredPosDelta.x, virtRot, rate );
    MotionCubic( &actElev, &desiredPosDelta.y, virtElev, rate );
    MotionCubic( &actMag, &desiredPosDelta.z, virtMag, rate );

    rmt::Vector newPos;
    rmt::SphericalToCartesian( actMag, actRot, actElev, &newPos.x, &newPos.z, &newPos.y );

    currentPos = newPos;

#ifdef PROFILER_ENABLED
    END_PROFILE( sCname )
#endif
}

//=============================================================================
// SuperCam::UpdateVUP
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector position, rmt::Vector target )
//
// Return:      rmt::Vector
//
//=============================================================================
rmt::Vector SuperCam::UpdateVUP( rmt::Vector position, rmt::Vector target )
{
#ifdef PROFILER_ENABLED
    char sCname[256];
    sprintf( sCname, "SC: %d UpdateVUP", mPlayerID );
    BEGIN_PROFILE( sCname )
#endif

        const float epsilon = 0.01f;
    //Set the vUP by projecting the heading into the ZX plane and creating a 
    //crossproduct of a right angle to the projected heading along the X axis 
    //and the heading.
    rmt::Vector CamX, CamY, CamZ;
    CamZ.Sub(target, position);
    CamX.Set(CamZ.z, 0, -CamZ.x);

    if ( rmt::Epsilon( CamX.x, 0, epsilon ) &&
        rmt::Epsilon( CamX.y, 0, epsilon ) &&
        rmt::Epsilon( CamX.z, 0, epsilon ) )
    {
        //Then the camera is looking straight down.
        CamY.Set( 0, 0, 1.0f ); //Up along the Z...
    }
    else
    {
        CamY.CrossProduct(CamZ, CamX);
        CamY.Normalize();
    }

#ifdef PROFILER_ENABLED
    END_PROFILE( sCname )
#endif

        return CamY;
}

//=============================================================================
// SuperCam::EaseMotion
//=============================================================================
// Description: Parabolic ease-in / ease-out
//              The default case of no ease-in/ease-out would produce a velocity
//              curve that is a horizontal straight line of vec0 as t goes from 0 
//              to 1. The distance covered would be ease(1) = vec0*1. 
//              Assume constant acceleration and deceleration at the beginning 
//              and end of the motion, and zero acceleration during the middle 
//              of the motion.  t is parametric time ( 0 - 1 ), a and b are times
//              when acceleration ends and deceleration begins.
//
// Parameters:  ( float t, float a, float b )
//
// Return:      float 
//
//=============================================================================
float SuperCam::EaseMotion( float t, float a, float b )
{
    float vec0, d;

    vec0 = 2.0f / ( 1 + b - a );  //Constant velocity attained.

    if ( t < a )
    {
        d = vec0 * t * t / ( 2.0f * a );
    }
    else
    {
        d = vec0 * a / 2.0f;

        if ( t < b )
        {
            d += ( t - a ) / 2.0f;
        }
        else
        {
            d += ( b - a ) * vec0;
            d += ( t - t * t / 2.0f - b + b * b / 2.0f ) * vec0 / ( 1 - b );
        }
    }

    return d;
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperCam::DoCameraTransition
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool quick, unsigned int timems )
//
// Return:      void 
//
//=============================================================================
void SuperCam::DoCameraTransition( bool quick, unsigned int timems )
{
    if ( GetFlag( (Flag)CUT ) )
    {
        //Let's not transition if we're waiting to cut.
        return;
    }

#ifdef DEBUGWATCH
    if ( TRANSITION_TIME_LIMIT > 0 )
    { 
        //Override the time limit by the watcher value.
        timems = TRANSITION_TIME_LIMIT;
    }
#endif

    mTransitionTimeLimit = timems;

    if ( GetFlag((Flag)SHAKE ) )
    {
        //Hmmmm...  This could be a hack.
        EndShake();
    }

    SetFlag( (Flag)START_TRANSITION, true );

    if ( quick )
    {
        SetFlag( (Flag)QUICK_TRANSITION, true );
    }

    if ( GetType() == STATIC_CAM || GetType() == RAIL_CAM )
    {
        SetFlag( (Flag)EASE_IN, true );
    }
}

//=============================================================================
// SuperCam::TestCameraMatrix
//=============================================================================
// Description: This is the only way to tell when the camera gets a bad 
//              setting that will blow up the game.
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::TestCameraMatrix()
{
#ifdef PROFILER_ENABLED
        char sCname[256];
        sprintf( sCname, "SC: %d TestCameraMatrix", mPlayerID );
        BEGIN_PROFILE( sCname )
#endif
    const rmt::Matrix c2w = mCamera->GetWorldToCameraMatrix();

    int i, j;
    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            rAssertMsg( !rmt::IsNan(c2w.m[i][j]), "This is a bad matrix for the super cam!");
        }
    }
#ifdef PROFILER_ENABLED
        END_PROFILE( sCname )
#endif

}

//=============================================================================
// SuperCam::SetupShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::SetupShake()
{
    if ( !GetFlag( (Flag)SHAKE ) && mShaker )
    {
        rmt::Vector oldPos;
        GetPosition( &oldPos );
    
        mCameraVirtualPosition = oldPos;

        SetFlag( (Flag)START_SHAKE, false );
        SetFlag( (Flag)SHAKE, true );

        rAssert( mShaker );

        mShaker->SetCamera( mCamera );
        mShaker->SetSpeed( 1.0f );
        mShaker->RegisterDebugInfo();
    }
}


//=============================================================================
// SuperCam::EndShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCam::EndShake()
{
    if ( GetFlag( (Flag)SHAKE ) )
    {
        mCamera->SetPosition( mCameraVirtualPosition );
        mCamera->SetTarget( mCameraVirtualTarget );

        mCameraVirtualPosition.Set(0.0f, 0.0f, 0.0f);
        mCameraVirtualTarget.Set(0.0f, 0.0f, 0.0f);

        SetFlag( (Flag)START_SHAKE, false );
        SetFlag( (Flag)SHAKE, false );
        SetFlag( (Flag)END_SHAKE, false );
        
        mShaker->UnregisterDebugInfo();
        mShaker = NULL;
    }
}

//=============================================================================
// SuperCam::SetupTransition
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool swap )
//
// Return:      void 
//
//=============================================================================
void SuperCam::SetupTransition( bool swap )
{
    if ( !GetFlag( (Flag)TRANSITION ) )
    {
        rmt::Vector oldPos, oldTarget;
        GetPosition( &oldPos );
   
        mCameraVirtualPosition = oldPos;

        GetTarget( &oldTarget );
        mCameraVirtualTarget = oldTarget;

        if ( swap )
        {
            float temp = mCameraVirtualFOV;
            mCameraVirtualFOV = mSCFOV;
            mSCFOV = temp;
        }
        else
        {
            mCameraVirtualFOV = mSCFOV;
            
            float waste = 0.0f;
            mCamera->GetFOV( &mSCFOV, &waste );
        }

        SetFlag( (Flag)TRANSITION, true );
    }

    mTransitionTime = 0;
    mCameraVirtualPositionDelta.Set(0.0f, 0.0f, 0.0f);
    mCameraVirtualTargetDelta.Set(0.0f, 0.0f, 0.0f);
    mCameraVirtualFOVDelta = 0.0f;

    SetFlag( (Flag)START_TRANSITION, false );
}

//=============================================================================
// SuperCam::EndTransition
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool abort )
//
// Return:      void 
//
//=============================================================================
void SuperCam::EndTransition( bool abort )
{
    if ( GetFlag( (Flag)TRANSITION ) )
    {
        if ( !abort )
        {
            mCamera->SetPosition( mCameraVirtualPosition );
            mCamera->SetTarget( mCameraVirtualTarget );
            mSCFOV = mCameraVirtualFOV;
        }

        mCameraVirtualPosition.Set(0.0f, 0.0f, 0.0f);
        mCameraVirtualTarget.Set(0.0f, 0.0f, 0.0f);
        mCameraVirtualPositionDelta.Set(0.0f, 0.0f, 0.0f);
        mCameraVirtualTargetDelta.Set(0.0f, 0.0f, 0.0f);
        mCameraVirtualFOVDelta = 0.0f;

        mTransitionTimeLimit = TRANSITION_TIME_LIMIT;
        mFOVTransitionRate = FOV_TRANSITION_RATE;

        SetFlag( (Flag)TRANSITION, false );
        SetFlag( (Flag)END_TRANSITION, false );
        SetFlag( (Flag)QUICK_TRANSITION, false );
    }

    SetFlag( (Flag)START_TRANSITION, false );
}

//=============================================================================
// SuperCam::ShakeCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* pos, rmt::Vector* targ, unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SuperCam::ShakeCamera( rmt::Vector* pos, 
                            rmt::Vector* targ,
                            unsigned int milliseconds )
{
    if ( mShaker )
    {
        if ( !mShaker->DoneShaking() )
        {
            mShaker->ShakeCamera( pos, targ, milliseconds );

            if ( mShaker->DoneShaking() )
            {
                DisableShake();
            }
        }
    }
}

//=============================================================================
// SuperCam::TransitionCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeLeftPct, rmt::Vector* pos, rmt::Vector* targ, float* fov, unsigned int milliseconds, bool quick )
//
// Return:      void 
//
//=============================================================================
void SuperCam::TransitionCamera( float timeLeftPct, 
                                 rmt::Vector* pos, 
                                 rmt::Vector* targ,
                                 float* fov,
                                 unsigned int milliseconds,
                                 bool quick )
{
    //This is to adjust interpolation when we're running substeps.
    float timeMod = (float)milliseconds / EXPECTED_FRAME_RATE;

    float posInterval = ( ( ( 1.0f - mTransitionPositionRate ) * timeLeftPct ) + mTransitionPositionRate ) * timeMod;
    CLAMP_TO_ONE(posInterval);

    float targInterval = ( ( ( 1.0f - mTransitionTargetRate ) * timeLeftPct ) + mTransitionTargetRate )* timeMod;
    CLAMP_TO_ONE(targInterval);

    rmt::Vector actPos, actTarg;
    mCamera->GetPosition( &actPos );
    mCamera->GetTarget( &actTarg );

    if ( !quick )
    {
        //Calculate the new position
        SphericalMotion( mCameraVirtualTarget, actPos, mCameraVirtualPosition, mCameraVirtualPositionDelta, posInterval );

        actPos.Add( actPos, mCameraVirtualTarget );
    }
    else
    {
        //Do it quick.
        MotionCubic( &actPos.x, &mCameraVirtualPositionDelta.x, mCameraVirtualPosition.x, posInterval );
        MotionCubic( &actPos.y, &mCameraVirtualPositionDelta.y, mCameraVirtualPosition.y, posInterval );
        MotionCubic( &actPos.z, &mCameraVirtualPositionDelta.z, mCameraVirtualPosition.z, posInterval );
    }

    //Calculate the new targetPosition
    MotionCubic( &actTarg.x, &mCameraVirtualTargetDelta.x, mCameraVirtualTarget.x, targInterval );
    MotionCubic( &actTarg.y, &mCameraVirtualTargetDelta.y, mCameraVirtualTarget.y, targInterval );
    MotionCubic( &actTarg.z, &mCameraVirtualTargetDelta.z, mCameraVirtualTarget.z, targInterval );

    float actFOV, actAspect;
    GetCamera()->GetFOV( &actFOV, &actAspect );
    TransitionFOV( timeLeftPct, &actFOV, milliseconds );

    *pos = actPos;
    *targ = actTarg;
    *fov = actFOV;
}

//=============================================================================
// SuperCam::TransitionFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeLeftPct, float* fov, unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SuperCam::TransitionFOV( float timeLeftPct, float* fov, unsigned int milliseconds )
{
    //This is to adjust interpolation when we're running substeps.
    float timeMod = (float)milliseconds / EXPECTED_FRAME_RATE;

    float fovInterval = ( ( ( 1.0f - mFOVTransitionRate ) * timeLeftPct ) + mFOVTransitionRate ) * timeMod;
    CLAMP_TO_ONE(fovInterval);

    //Calculate the new FOV
    MotionCubic( fov, &mCameraVirtualFOVDelta, mCameraVirtualFOV, fovInterval );
}


//=============================================================================
// SuperCam::EaseIn
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeLeftPct, rmt::Vector* pos, rmt::Vector* targ, unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SuperCam::EaseIn( float timeLeftPct, rmt::Vector* pos, rmt::Vector* targ, unsigned int milliseconds )
{
    float d = EaseMotion( timeLeftPct, 0.0f, 0.0f );
//    char out[256];
//    sprintf( out,  "T = %.2f, D = %.2f\n", timeLeftPct, d );
//    rDebugString( out );

    pos->x = ( pos->x - mCameraVirtualPosition.x ) * d + mCameraVirtualPosition.x;
    pos->y = ( pos->y - mCameraVirtualPosition.y ) * d + mCameraVirtualPosition.y;
    pos->z = ( pos->z - mCameraVirtualPosition.z ) * d + mCameraVirtualPosition.z;

    targ->x = ( targ->x - mCameraVirtualTarget.x ) * d + mCameraVirtualTarget.x;
    targ->y = ( targ->y - mCameraVirtualTarget.y ) * d + mCameraVirtualTarget.y;
    targ->z = ( targ->z - mCameraVirtualTarget.z ) * d + mCameraVirtualTarget.z;
   
}

//=============================================================================
// SuperCam::SetCameraShakerData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const ShakeEventData* data )
//
// Return:      void 
//
//=============================================================================
void SuperCam::SetCameraShakerData( const ShakeEventData* data )
{
    if ( mShaker )
    {
        mShaker->Reset();
        mShaker->SetShakeData( data );
    }
}

//=============================================================================
// SuperCam::GetWatcherName
//=============================================================================
// Description: char for identifying this object in the watcher
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
#ifdef DEBUGWATCH
const char* SuperCam::GetWatcherName() const
{
    return "No Name";
}

void SuperCam::PrintClassName() const
{
    const char* watcherName = GetWatcherName();
    rDebugPrintf( "Camera - %s\n", watcherName );
}
#endif //DEBUGWATCH