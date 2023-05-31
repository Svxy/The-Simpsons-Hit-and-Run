//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        followcam.cpp
//
// Description: Implement FollowCam
//              This is the main target following camera mode.  It maintains a rod
//              that orients itself in 3D according to rules on the 3-axis.
//              This mode can handle one main target and possibly 2-others in a
//              limited fashion.
//
// History:     22/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
//These are included in the precompiled headers on XBOX and GC
#include <raddebug.hpp>
#include <radmath/radmath.hpp>
#include <raddebugwatch.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <camera/FollowCam.h>
#include <camera/FollowCamDataChunk.h>
#include <camera/isupercamtarget.h>
#include <camera/supercamcentral.h>
#include <camera/supercamcontroller.h>
#include <camera/supercammanager.h>

#include <main/game.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <cheats/cheatinputsystem.h>
#include <ai/actor/intersectionlist.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

void SetDirtyCB( void* userData )
{
    FollowCam* fc = (FollowCam*)userData;

    fc->SetDirty();
}

static bool reverseTest = false;

#ifdef DEBUGWATCH
float MIN_REVERSE_VELOCITY = 45.0f;
static float CREEPY_TWIST = 6.107f;
float LOOK_OFFSET_DIST = 5.0f;
float LOOK_OFFSET_HEIGHT = 1.0f;
float LOOK_OFFSET_BACK = 0.0f;
float LOOK_ROT = rmt::PI_BY2;
#else
const float MIN_REVERSE_VELOCITY = 45.0f;
static const float CREEPY_TWIST = 6.107f;
const float LOOK_OFFSET_DIST = 5.0f;
const float LOOK_OFFSET_HEIGHT = 1.0f;
const float LOOK_OFFSET_BACK = 0.0f;
const float LOOK_ROT = rmt::PI_BY2;
#endif

const unsigned int FOLLOW_ID_OFFSET = 256;

//Only uncomment ONE of these.
//#define TURN_LOOK
//#define CUT_LOOK
#define EXTRA_ROT

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// FollowCam::FollowCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
FollowCam::FollowCam( FollowType type ) :
    mNumTargets( 0 ),
    mActiveTarget( 0 ),
    mUnstableDelayTimeLeft( 0 ),
    mQuickTurnTimeLeft( 0 ),
    mFollowType( type ),
    mXAxis( 0.0f )
{
    unsigned int i;
    for ( i = 0; i < MAX_TARGETS; ++i )
    {
        mTargets[ i ] = NULL;
    }    

    mUnstablePosition.Set( 0.0f, 0.0f, 0.0f );
    mUnstableTarget.Set( 0.0f, 0.0f, 0.0f );

    mRotationAngleXZ = mData.GetRotationXZ();
    mRotationAngleY = mData.GetRotationY();
    mRotationAngleXZDelta = 0.0f;
    mRotationAngleYDelta = 0.0f;
    mMagnitude = mData.GetMagnitude();
    mMagnitudeDelta = 0.0f;

    mTargetPosition.Set( 0.0f, 0.0f, 0.0f );
    mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );

    mGroundOffset.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// FollowCam::~FollowCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
FollowCam::~FollowCam()
{
}

//=============================================================================
// FollowCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void FollowCam::Update( unsigned int milliseconds )
{
    BEGIN_PROFILE("FollowCam::Update")
    rAssert( mTargets[ mActiveTarget ] );

    //This is to adjust interpolation when we're running substeps.
    float timeMod = (float)milliseconds / EXPECTED_FRAME_RATE;

    rmt::Vector rod;    
    ISuperCamTarget* target = mTargets[ mActiveTarget ];

#ifdef CUT_LOOK
#if defined(RAD_GAMECUBE) || defined(RAD_XBOX)
    float leftRight = mController->GetValue( SuperCamController::stickX );
#elif defined(RAD_WIN32)
    float left = mController->GetValue( SuperCamController::carLookLeft );
    float right = mController->GetValue( SuperCamController::carLookRight );
    float leftRight = ( right > left ) ? right : -left;
#else //This is PS2
    float leftRight = mController->GetValue( SuperCamController::r2 ) - mController->GetValue( SuperCamController::l2 );
#endif

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
    if ( mController->IsWheel() )
    {
        //This is a wheel.  No left right on wheels.
        leftRight = 0.0f;
    }
#endif

    if ( rmt::Fabs(leftRight) > 0.2f && !GetFlag( (Flag)UNSTABLE ) )
    {
        SetFlag( (Flag)LOOKING_SIDEWAYS, true );
        SetFlag( (Flag)CUT, true );
    } 
    else if ( rmt::Fabs(leftRight) <= 0.2f && GetFlag( (Flag)LOOKING_SIDEWAYS ) )
    {
        SetFlag( (Flag)LOOKING_SIDEWAYS, false );
        SetFlag( (Flag)CUT, true );
    }
    else
#endif

    if ( GetFlag( (Flag)LOOK_BACK ) && 
         !GetFlag( (Flag)LOOKING_BACK ) &&
         !GetFlag( (Flag)UNSTABLE ) )
    {
        //Turn on
        SetFlag( (Flag)LOOKING_BACK, true );
        SetFlag( (Flag)CUT, true );
    }
    else if ( GetFlag( (Flag)LOOKING_BACK ) &&
              !GetFlag( (Flag)LOOK_BACK ) &&
              !GetFlag( (Flag)UNSTABLE ) )
    {
        //Turn off.
        SetFlag( (Flag)LOOKING_BACK, false );
        SetFlag( (Flag)CUT, true );
    }

    if ( GetFlag( (Flag)CUT ) )
    {
        DoCameraCut();
    }

    if ( GetFlag( (Flag)UNSTABLE ) )
    {
        UpdateUnstable( milliseconds );
    }

    //---------  Test to see if the target is still unstable

    if ( !GetFlag((Flag)FIRST_TIME) &&
         !GetFlag((Flag)CUT) && 
         ( target->IsUnstable() || target->IsQuickTurn() || target->IsAirborn() ) &&
         !GetSuperCamManager()->GetSCC( GetPlayerID() )->IsCutCam()
       )
    {
        if ( target->IsQuickTurn() )
        {
            //Say that going unstable is due to quick turn, 
            //so when we come out of unstable, speed up interpolation.
            SetFlag( (Flag)QUICK_TURN_ALERT, true );
        }

        if ( mUnstableDelayTimeLeft == 0 )
        {
            //Going unstable anew
            InitUnstable();
        }

        //Reset the unstable clock.
        mUnstableDelayTimeLeft = mData.GetUnstableDelay();
        SetFlag((Flag)UNSTABLE, true );
    }

    BEGIN_PROFILE("Cam::Rod")
    //---------  Figure out the new rod thing.
    if ( GetFlag( (Flag)UNSTABLE ) )
    {
        //Get the unstable rod.
        rod = mUnstablePosition;

        //Deal with collision 
        //rAssert( mNumCollisions == 0 );
    }
    else
    {
        CalculateRod( &rod, milliseconds, timeMod );
    }
    END_PROFILE("Cam::Rod")

    rmt::Vector desiredPosition = rod;

    //---------  Get the position in space of the target to apply to desired values.
    rmt::Vector targetPos;
    target->GetPosition( &targetPos );

    //---------  Set the desired position of the camera
    desiredPosition.Add( targetPos );
   
    BEGIN_PROFILE("FollowCam::Target")
    //---------  Set the desired position of the camera target
    rmt::Vector desiredTarget;
    GetTargetPosition( &desiredTarget );

    if ( GetFlag( (Flag)UNSTABLE ) )
    {
        desiredTarget.Add( targetPos, mUnstableTarget );
    }
    else
    {
        CalculateTarget( &desiredTarget, milliseconds, timeMod );
    }
    
    //TODO:  Connect this camera to the physics system.
    END_PROFILE("FollowCam::Target")

    if ( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_SPEED_CAM ) )
    {
        SetFOV( 1.608495f );
    }
    else
    {
        SetFOV( mData.GetFOV() );
    }

    bool stabilizingToggle = false;

    rmt::Vector lookFrom = targetPos;
    rmt::Vector lookTo = desiredPosition;

    IntersectionList& iList = GetSuperCamManager()->GetSCC( GetPlayerID() )->GetIntersectionList();

    if ( !iList.LineOfSight( lookFrom, lookTo, 0.01f, true ) )
    {
        bool badSituation = false;

        if ( GetFlag( (Flag)LOOK_BACK ) )
        {
            //Go somewhere else.
            rmt::Vector intersection;
            if ( iList.TestIntersectionStatics( lookFrom, lookTo, &intersection ) )
            {
                desiredPosition = intersection;

                rmt::Vector localPos = desiredPosition;
                localPos.Sub( targetPos );

                rmt::CartesianToSpherical( localPos.x, localPos.z, localPos.y,
                    &mMagnitude, &mRotationAngleXZ, &mRotationAngleY );

                mRotationAngleXZDelta = 0.0f;
                mRotationAngleYDelta = 0.0f;
                mMagnitudeDelta = 0.0f; 

                lookFrom = targetPos;
                lookTo = desiredPosition;

                if ( !iList.LineOfSight( lookFrom, lookTo, 0.01f, true ) )
                {
                    badSituation = true;
                }
            }
        }
        else
        {
            SetFlag( (Flag)CUT, true );
            DoCameraCut();
            CalculateRod( &rod, milliseconds, timeMod );
            desiredPosition = rod;
            desiredPosition.Add( targetPos );

            lookFrom = targetPos;
            lookTo = desiredPosition;

            if ( !iList.LineOfSight( lookFrom, lookTo, 0.01f, true ) )
            {
                badSituation = true;
            }

            SetFlag( (Flag)LOS_CORRECTED, true );
        }

        if ( badSituation )
        {
            iList.TestIntersectionStatics( lookTo, lookFrom, &desiredPosition );

            rmt::Vector distVec;
            distVec.Sub( targetPos, desiredPosition );
            float minDist = mData.GetMagnitude() - 2.0f; //Arbitrary?
            if ( distVec.MagnitudeSqr() < ( minDist * minDist ) )
            {
                desiredPosition = targetPos;
                desiredPosition.Add( rmt::Vector( 0.0f, 6.0f, 0.0f ) );  //BAD!

/*                rmt::Vector heading;
                mTargets[ 0 ]->GetHeading( &heading );

                if ( GetFlag( (Flag)LOOKING_BACK ) )
                {
                    heading.x *= -1.0f;
                    heading.z *= -1.0f;
                }

                heading.Scale( 2.0f );

                desiredTarget.Add( desiredPosition, heading );
*/
            }
        }

        //Better clear the unstable flag, just in case.
        //Clear the unstable flag.
        SetFlag( (Flag)UNSTABLE, false );
        stabilizingToggle = true;
        mUnstableDelayTimeLeft = 0;
    }

    //---------  Set the new camera values
    BEGIN_PROFILE("FollowCam::SetCameraValues")
    SetCameraValues( milliseconds, desiredPosition, desiredTarget );
    END_PROFILE("FollowCam::SetCameraValues")

    SetFlag( (Flag)FIRST_TIME, false );
    SetFlag( (Flag)LOOK_BACK, false );
    SetFlag( (Flag)STABILIZING, stabilizingToggle );
    SetFlag( (Flag)CUT, stabilizingToggle );
    //SetFlag( (Flag)LOOKING_SIDEWAYS, false );
    END_PROFILE("FollowCam::Update")

    //Save the old position of the car in case we go unstable next update.
    GetTargetPosition( &mOldTargetPos, false );
}

//=============================================================================
// FollowCam::UpdateForPhysics
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void FollowCam::UpdateForPhysics( unsigned int milliseconds )
{
    bool collision = false;

    //This is to correct for collisions.
    //Adjust for collision;
    rmt::Vector currentPosition, desiredPosition, delta;
    GetPosition( &currentPosition );

    desiredPosition = currentPosition;
    delta.Set( 0.0f, 0.0f, 0.0f );

    unsigned int i;
    for ( i = 0; i < mNumCollisions; ++i )
    {
        rmt::Vector camHeading;
        GetHeading( &camHeading );
        camHeading.NormalizeSafe();
        if ( camHeading.DotProduct( mCollisionOffset[ i ] ) >= -0.7f )
        {
            desiredPosition.Add( mCollisionOffset[ i ] );
        }
        else if ( !collision )
        {
            rmt::Vector fakedCollision = camHeading;
            fakedCollision *= GetNearPlane() * 1.5f;
            desiredPosition.Add( fakedCollision );
        }

        collision = true;
    }
   
    float lag = mData.GetCollisionLag();
    lag *= milliseconds / 16.0f;
    CLAMP_TO_ONE( lag );

    MotionCubic( &currentPosition.x, &delta.x, desiredPosition.x, lag );
    MotionCubic( &currentPosition.y, &delta.y, desiredPosition.y, lag );
    MotionCubic( &currentPosition.z, &delta.z, desiredPosition.z, lag );

    //------- Check to see if we've intersected anything
    if ( mGroundOffset.MagnitudeSqr() > 0.001f )
    {
        currentPosition.Add( mGroundOffset );
        collision = true;
    }

    rmt::Vector targetPos;
    GetTarget( &targetPos );

    SetCameraValues( 0, currentPosition, targetPos );  //0.0f, no extra transition please.

    SetFlag((Flag)COLLIDING, collision );

    if ( collision )
    {
        mXAxis = mController->GetAxisValue( SuperCamController::stickX );
    }
}


//=============================================================================
// FollowCam::LoadSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned char* settings )
//
// Return:      void 
//
//=============================================================================
void FollowCam::LoadSettings( unsigned char* settings )
{
}

//=============================================================================
// FollowCam::CopyToData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::CopyToData()
{
    // dlong: it's fun to misuse other people's code
    mData.SetAspect( GetAspect() );
}

//=============================================================================
// FollowCam::SetTarget
//=============================================================================
// Description: This selects the current active target.
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void FollowCam::SetTarget( ISuperCamTarget* target )
{
    rAssert( target );

    mTargets[ 0 ] = target;
    mNumTargets = 1;

    mActiveTarget = 0;//Set it to the first one.
    
    FollowCamDataChunk* fcD = SuperCamCentral::FindFCD( target->GetID() + static_cast<unsigned int>(mFollowType) * FOLLOW_ID_OFFSET );
    
    if ( fcD )
    {
        //Load the data.
        //mData.SetRotationXZ( fcD->mRotation );
        mData.SetRotationY( fcD->mElevation );
        mData.SetMagnitude( fcD->mMagnitude );
        mData.SetTargetOffset( fcD->mTargetOffset );

        return;
    }
     
    rDebugString( "There should have been camera data loaded!" );
}

//=============================================================================
// FollowCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void FollowCam::AddTarget( ISuperCamTarget* target )
{
    rAssert( mNumTargets < MAX_TARGETS );
    rAssert( target );

    if ( mNumTargets < MAX_TARGETS )
    {
        //Add the target
        mTargets[ mNumTargets ] = target;

        ++mNumTargets;
    }
}

//=============================================================================
// FollowCam::EnableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::EnableShake()
{
    SetShaker( &mSineCosShaker );
    SuperCam::EnableShake();
}

//=============================================================================
// FollowCam::DisableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::DisableShake()
{
    SuperCam::DisableShake();
}

//=============================================================================
// FollowCam::ShouldReverse
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool FollowCam::ShouldReverse() const
{
    rAssert( mTargets[0] != NULL );
    rmt::Vector v;
    mTargets[0]->GetVelocity( &v );

    return !mTargets[0]->IsAirborn() &&
           !mTargets[0]->IsUnstable() &&
           mTargets[0]->IsInReverse() && 
           ((v.MagnitudeSqr() > MIN_REVERSE_VELOCITY ) || mNumCollisions > 0 );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// FollowCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Follow", GetPlayerID() );

    radDbgWatchAddFloat( &mData.mRotationXZ, "Rotation XZ", nameSpace, &SetDirtyCB, this, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &mData.mRotationY, "Rotation Y", nameSpace, &SetDirtyCB, this, 0.001f, rmt::PI_BY2 );
    radDbgWatchAddFloat( &mData.mMagnitude, "Magnitude", nameSpace, &SetDirtyCB, this, 2.0f, 50.0f );

    radDbgWatchAddFloat( &mData.mCameraLagXZ, "Camera Lag XZ", nameSpace, &SetDirtyCB, this, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mCameraLagY, "Camera Lag Y", nameSpace, &SetDirtyCB, this, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mMagnitudeLag, "Magnitude Lag", nameSpace, &SetDirtyCB, this, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mCollisionLag, "Collision Lag", nameSpace, &SetDirtyCB, this, 0.0f, 1.0f );

    radDbgWatchAddVector( &mData.mTargetOffset.x, "Target Offset", nameSpace, &SetDirtyCB, this, 0.0f, 100.0f );

    radDbgWatchAddFloat( &mData.mTargetLagXZ, "Target Lag XZ", nameSpace, &SetDirtyCB, this, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mTargetLagY, "Target Lag Y", nameSpace, &SetDirtyCB, this, 0.0f, 1.0f );

    radDbgWatchAddUnsignedInt( &mData.mUnstableDelay, "Unstable Delay", nameSpace, &SetDirtyCB, this, 0, 1000 );

    radDbgWatchAddUnsignedInt( &mData.mQuickTurnDelay, "Quick-turn Delay", nameSpace, &SetDirtyCB, this, 0, 1000 );
    radDbgWatchAddFloat( &mData.mQuickTurnModifier, "Quick-turn Modifier", nameSpace, &SetDirtyCB, this, 0.0f, 1.0f );

    radDbgWatchAddFloat( &MIN_REVERSE_VELOCITY, "Min reverse v", nameSpace, NULL, this, 10.0f, 300.0f );

    radDbgWatchAddFloat( &CREEPY_TWIST, "Twist", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );

#if defined(LOOK_TURN) || defined(CUT_LOOK)
    radDbgWatchAddFloat( &LOOK_OFFSET_DIST, "Side/Up Look Dist", nameSpace, NULL, NULL, 0.0f, 20.0f );
    radDbgWatchAddFloat( &LOOK_OFFSET_HEIGHT, "Side/Up Look height", nameSpace, NULL, NULL, 0.0f, 5.0f );
    radDbgWatchAddFloat( &LOOK_OFFSET_BACK, "Side/Up Look pos Z", nameSpace, NULL, NULL, 0.0f, -5.0f );
#endif

#ifdef EXTRA_ROT
    radDbgWatchAddFloat( &LOOK_ROT, "Side Look Rot", nameSpace, NULL, NULL, 0.01f, rmt::PI_BY2 );
#endif

#endif
}

//=============================================================================
// FollowCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mData.mRotationXZ );
    radDbgWatchDelete( &mData.mRotationY );
    radDbgWatchDelete( &mData.mMagnitude );

    radDbgWatchDelete( &mData.mCameraLagXZ );
    radDbgWatchDelete( &mData.mCameraLagY );
    radDbgWatchDelete( &mData.mMagnitudeLag );
    radDbgWatchDelete( &mData.mCollisionLag );

    radDbgWatchDelete( &mData.mTargetOffset.x );

    radDbgWatchDelete( &mData.mTargetLagXZ );
    radDbgWatchDelete( &mData.mTargetLagY );

    radDbgWatchDelete( &mData.mUnstableDelay );

    radDbgWatchDelete( &mData.mQuickTurnDelay );
    radDbgWatchDelete( &mData.mQuickTurnModifier );

    radDbgWatchDelete( &MIN_REVERSE_VELOCITY );

    radDbgWatchDelete( &CREEPY_TWIST );

#if defined(LOOK_TURN) || defined(CUT_LOOK)
    radDbgWatchDelete( &LOOK_OFFSET_DIST );
    radDbgWatchDelete( &LOOK_OFFSET_HEIGHT );
    radDbgWatchDelete( &LOOK_OFFSET_BACK );
#endif

#ifdef EXTRA_ROT
    radDbgWatchDelete( &LOOK_ROT );
#endif
#endif
}

//=============================================================================
// FollowCam::DoCameraCut
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::DoCameraCut()
{
    SetFlag( (Flag)CUT, true );
    
    //We're doing a camera cut so let's set the position to the default.
    rmt::Matrix mat;
    rmt::Vector targetHeading, targetVUP;
    mTargets[ mActiveTarget ]->GetHeading( &targetHeading );
    mTargets[ mActiveTarget ]->GetVUP( &targetVUP );
    mat.Identity();
    mat.FillHeading( targetHeading, targetVUP );

    rmt::Vector desiredRod;
    GetDesiredRod( &desiredRod );

    //Get the rod into the target's space
    desiredRod.Transform( mat );

    rmt::CartesianToSpherical( desiredRod.x, desiredRod.z, desiredRod.y,
                               &mMagnitude, &mRotationAngleXZ, &mRotationAngleY );

    //Also, reset the deltas.
    mRotationAngleXZDelta = 0.0f;
    mRotationAngleYDelta = 0.0f;
    mMagnitudeDelta = 0.0f;

#ifdef CUT_LOOK
    if ( GetFlag( (Flag)LOOKING_SIDEWAYS ) )
    {
        GetTargetPosition( &mTargetPosition, false );
        mTargetPosition.Add( rmt::Vector( 0.0f, LOOK_OFFSET_HEIGHT, 0.0f ) );
    }
    else
    {
        //mTargetPosition.Set( 0.0f, 0.0f, 0.0f );
        GetTargetPosition( &mTargetPosition );
    }
#else
    //mTargetPosition.Set( 0.0f, 0.0f, 0.0f );
    GetTargetPosition( &mTargetPosition );
#endif   

    mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );

    //Reset the FOV and aspect ratio for this camera.
    SetFOV( mData.GetFOV() );
    SetAspect( mData.GetAspect() );

    SetFlag( (Flag)UNSTABLE, false );
}

//=============================================================================
// FollowCam::InitUnstable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::InitUnstable()
{
    //First time unstable since recovered.
    //Set up the unstable rod
    
    rmt::Vector targetPosition;
    //GetTargetPosition( &targetPosition, false );
    targetPosition = mOldTargetPos;
    
    rmt::Vector camPosition;
    GetPosition( &camPosition );

    mUnstablePosition.Sub( camPosition, targetPosition );

    rmt::Vector camTarget;
    GetTarget( &camTarget );

    mUnstableTarget.Sub( camTarget, targetPosition );

    //Set the unstable flag
    SetFlag( (Flag)UNSTABLE, true );
}

//=============================================================================
// FollowCam::UpdateUnstable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::UpdateUnstable( unsigned int milliseconds )
{
    mUnstableDelayTimeLeft -= milliseconds;

    if ( mUnstableDelayTimeLeft <= 0 )
    {
        //Finished being unstable.  Let the camera drift back to it's normal
        //position.  If this is a speed up camera situation, we should 
        //accelerate the rotation.

        mUnstableDelayTimeLeft = 0;

        mUnstablePosition.Set( 0.0f, 0.0f, 0.0f );
        mUnstableTarget.Set( 0.0f, 0.0f, 0.0f );

        mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );

        //Clear the unstable flag.
        SetFlag( (Flag)UNSTABLE, false );

        if ( GetFlag( (Flag)QUICK_TURN_ALERT ) )
        {
            SetFlag( (Flag)QUICK_TURN_ALERT, false );
            SetFlag( (Flag)QUICK_TURN, true );

            InitQuickTurn();
        }

        //This is to internally reset the state of the follow cam.  By doing
        //The unstable stuff we've hosed the "memory" of the rotations.
        SetFlag( (Flag)STABILIZING, true );
    }
}

//=============================================================================
// FollowCam::InitQuickTurn
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCam::InitQuickTurn()
{
    mQuickTurnTimeLeft = mData.GetQuickTurnDelay();
}

//=============================================================================
// FollowCam::UpdateQuickTurn
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void FollowCam::UpdateQuickTurn( unsigned int milliseconds )
{
    mQuickTurnTimeLeft -= milliseconds;

    if ( mQuickTurnTimeLeft <= 0 )
    {
        //Done the quick turn, back to normal speed now.
        SetFlag( (Flag)QUICK_TURN, false );
        mQuickTurnTimeLeft = 0;
    }
}

//=============================================================================
// FollowCam::GetTargetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position, bool withOffset )
//
// Return:      void 
//
//=============================================================================
void FollowCam::GetTargetPosition( rmt::Vector* position, 
                                          bool withOffset )
{
    mTargets[ mActiveTarget ]->GetPosition( position );

    if ( withOffset )
    {
        rmt::Vector offset;
        mData.GetTargetOffset( &offset );

        if ( GetFlag( (Flag)LOOK_BACK ) )
        {
            offset.x *= -1.0f;
            offset.z *= -1.0f;
        }

        //Add offset according to controller input.
        float lookUp = 0.0f;

#ifdef TURN_LOOK
        float lookLeftRight = 0.0f;

#if defined(RAD_GAMECUBE) || defined(RAD_XBOX)
        lookUp = mController->GetValue( SuperCamController::stickY );
        lookLeftRight = mController->GetValue( SuperCamController::stickX );
#elif defined(RAD_WIN32)
        lookUp = mController->GetValue( SuperCamController::carLookUp );
        float left = mController->GetValue( SuperCamController::carLookLeft );
        float right = mController->GetValue( SuperCamController::carLookRight );
        lookLeftRight = ( right > left ) ? right : -left;
#else //This is PS2
        lookUp = mController->GetValue( SuperCamController::lookToggle );
        lookLeftRight = mController->GetValue( SuperCamController::r2 ) - mController->GetValue( SuperCamController::l2 );
#endif
#else
#if defined(RAD_GAMECUBE) || defined(RAD_XBOX)
        lookUp = mController->GetValue( SuperCamController::stickY );
#elif defined(RAD_WIN32)
        lookUp = mController->GetValue( SuperCamController::carLookUp );
#else //This is PS2
        lookUp = mController->GetValue( SuperCamController::lookToggle );
#endif
#endif

#ifdef TURN_LOOK       
        if ( GetCharacterSheetManager()->QueryInvertedCameraSetting() )
        {
            //Invert this!
            lookLeftRight *= -1.0f;
        }

        offset.x += ( LOOK_OFFSET_DIST * lookLeftRight );
#endif

        if ( lookUp > 0.2f )
        {
            offset.y += ( LOOK_OFFSET_DIST * 1.0f ); //Make it digital.
        }

        //Now put the offset in the target's space
        rmt::Matrix mat;
        rmt::Vector targetHeading, targetVUP;
        mTargets[ mActiveTarget ]->GetHeading( &targetHeading );
        mTargets[ mActiveTarget ]->GetVUP( &targetVUP );
        mat.Identity();
        mat.FillHeading( targetHeading, targetVUP );

        offset.Transform( mat );        

        (*position).Add( offset );
    }

}


//=============================================================================
// FollowCam::CalculateRod
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* rod, unsigned int milliseconds, float timeMod )
//
// Return:      void 
//
//=============================================================================
void FollowCam::CalculateRod( rmt::Vector* rod, 
                              unsigned int milliseconds, 
                              float timeMod )
{  
    //Where is the camera in spherical coordinates?

    //This is the targets matrix and inverse matrix for rotation.
    rmt::Matrix mat;
    rmt::Vector targetHeading, targetVUP;
    mTargets[ mActiveTarget ]->GetHeading( &targetHeading );
    mTargets[ mActiveTarget ]->GetVUP( &targetVUP );

    //Never go upside down.
    targetVUP.y = rmt::Fabs( targetVUP.y );

    mat.Identity();
    mat.FillHeading( targetHeading, targetVUP );

    rmt::Vector desiredRod;

    GetDesiredRod( &desiredRod );
    desiredRod.Transform( mat );

    //---------  Do the look back
    if( GetFlag( (Flag)LOOK_BACK ) )
    {
        //reverse the rod.
        desiredRod.x = -desiredRod.x;
        desiredRod.z = -desiredRod.z;
    }


    //This puts us in spherical space, also note the z and y switch.
    float desiredRotXZ, desiredRotY, desiredMag;
    rmt::CartesianToSpherical( desiredRod.x, desiredRod.z, desiredRod.y, 
                               &desiredMag, &desiredRotXZ, &desiredRotY );


#ifdef EXTRA_ROT
    float invertMod = GetSuperCamManager()->GetSCC( GetPlayerID() )->IsInvertedCameraEnabled() ? -1.0f : 1.0f;
#if defined(RAD_GAMECUBE) || defined(RAD_XBOX)
    float leftRight = -mController->GetAxisValue( SuperCamController::stickX );
#elif defined(RAD_WIN32)
    float left = mController->GetValue( SuperCamController::carLookLeft );
    float right = mController->GetValue( SuperCamController::carLookRight );
    float leftRight = ( right > left ) ? -right : left;
#else //This is PS2
    float leftRight = mController->GetValue( SuperCamController::l2 ) - mController->GetValue( SuperCamController::r2 );
#endif

    if ( GetFlag( (Flag)LOS_CORRECTED ) && IsPushingStick() ) 
    {
        leftRight = 0.0f;
    }
    else
    {
        SetFlag( (Flag)LOS_CORRECTED, false );
    }

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
    if ( mController->IsWheel() )
    {
        //This is a wheel.  No left right on wheels.
        //Or we are stopping the user from continuing to force the stick into a wall.
        leftRight = 0.0f;
    }
#endif

    if ( !GetFlag( (Flag)LOOK_BACK ) )
    {
#ifdef RAD_WIN32 // this retarded move is thanks to vs.net optimization.
        desiredRotXZ += ( invertMod * leftRight * rmt::PI_BY2 );
#else
        desiredRotXZ += ( invertMod * leftRight * LOOK_ROT );
#endif
    }
#endif


    //---------  Setup the desired angle and the current angle.

    if ( !GetFlag((Flag)CUT) &&
         (GetFlag( (Flag)FIRST_TIME ) || GetFlag( (Flag)STABILIZING ) || GetFlag((Flag)COLLIDING )) )
    {
        //Let's try to interpolate from it's current position.
        rmt::Vector position;
        GetPosition( &position );

        rmt::Vector target;
        if ( GetFlag( (Flag)FIRST_TIME ) )
        {
            mTargets[ mActiveTarget]->GetPosition( &target );
        }
        else
        {
            //Use the old position since we're stabilizing from badness.
            target = mOldTargetPos;
        }

        rmt::Vector targToPos;
        targToPos.Sub( position, target );

        rmt::CartesianToSpherical( targToPos.x, targToPos.z, targToPos.y, &mMagnitude, &mRotationAngleXZ, &mRotationAngleY );
        mMagnitudeDelta = 0.0f;
        mRotationAngleXZDelta = 0.0f;
        mRotationAngleYDelta = 0.0f;
    }

    //We only want to interpolate to a rotation via the fewest number of degrees.
    AdjustAngles( &desiredRotXZ, &mRotationAngleXZ, &mRotationAngleXZDelta );
    AdjustAngles( &desiredRotY, &mRotationAngleY, &mRotationAngleYDelta );       

    rmt::Clamp( desiredRotY, rmt::PI + rmt::PI_BY2, rmt::PI_BY2 );

    //---------  Interpolate to the desired position and target

    //This is the normal interpolation stage.
    float cameraLagXZ   = mData.GetCameraLagXZ() * timeMod;
    float cameraLagY    = mData.GetCameraLagY() * timeMod;
    float magnitudeLag  = mData.GetMagnitudeLag() * timeMod;

    UpdateQuickTurn( milliseconds );

    if ( GetFlag( (Flag)QUICK_TURN ) )
    {
        //Change the speed of interpolation to the adjusted value.
        cameraLagXZ *= mData.GetQuickTurnModifier();
        CLAMP_TO_ONE(cameraLagXZ);
    }

    if ( GetFlag( (Flag)CUT ) )
    {
        cameraLagXZ = 1.0f;
        cameraLagY = 1.0f;
        magnitudeLag = 1.0f;
    }
    else
    {
        CLAMP_TO_ONE( cameraLagXZ );
        CLAMP_TO_ONE( cameraLagY );
        CLAMP_TO_ONE( magnitudeLag );
    }

    MotionCubic( &mRotationAngleXZ, &mRotationAngleXZDelta, desiredRotXZ, cameraLagXZ );
    MotionCubic( &mRotationAngleY, &mRotationAngleYDelta, desiredRotY, cameraLagY );
    MotionCubic( &mMagnitude, &mMagnitudeDelta, desiredMag, magnitudeLag );

    float x, y, z;
    rmt::SphericalToCartesian( mMagnitude, mRotationAngleXZ, mRotationAngleY,
                               &x, &z, &y );
    rod->Set( x, y, z );
}

//=============================================================================
// FollowCam::CalculateTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* desiredTarget, 
//                unsigned int milliseconds, 
//                float timeMod )
//
// Return:      void 
//
//=============================================================================
void FollowCam::CalculateTarget( rmt::Vector* desiredTarget, 
                                 unsigned int milliseconds, 
                                 float timeMod )
{
#ifdef CUT_LOOK
    if ( GetFlag( (Flag)LOOKING_SIDEWAYS ) )
    {
        GetTargetPosition( &mTargetPosition, false );
        mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );
        *desiredTarget = mTargetPosition;
        desiredTarget->Add( rmt::Vector( 0.0f, LOOK_OFFSET_HEIGHT, 0.0f ) );
        return;
    }
#endif

    if ( !GetFlag((Flag)CUT) &&
         (GetFlag( (Flag)FIRST_TIME ) || 
          (GetFlag( (Flag)STABILIZING ))) )
    {
        GetTarget( &mTargetPosition );
        mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f ); 
    }

    GetTargetPosition( desiredTarget );

    //Here's the target position and interpolation.
    float targetLagXZ   = mData.GetTargetLagXZ() * timeMod;
    CLAMP_TO_ONE(targetLagXZ);

    float targetLagY    = mData.GetTargetLagY() * timeMod;
    CLAMP_TO_ONE(targetLagY);

    MotionCubic( &mTargetPosition.x, &mTargetPositionDelta.x, desiredTarget->x, targetLagXZ );
    MotionCubic( &mTargetPosition.y, &mTargetPositionDelta.y, desiredTarget->y, targetLagY );
    MotionCubic( &mTargetPosition.z, &mTargetPositionDelta.z, desiredTarget->z, targetLagXZ );


    //Set the target position    
    *desiredTarget = mTargetPosition;
}

//=============================================================================
// FollowCam::GetDesiredRod
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* rod)
//
// Return:      bool - this says whether the rod is in world space or not.
//
//=============================================================================
bool FollowCam::GetDesiredRod( rmt::Vector* rod)
{
#ifdef CUT_LOOK

    if ( !GetFlag( (Flag)LOOKING_SIDEWAYS ) )
    {
        //No movement.
        mData.GetRod( rod );
        return false;
    }
    else
    {
        float invertMod = GetSuperCamManager()->GetSCC( GetPlayerID() )->IsInvertedCameraEnabled() ? -1.0f : 1.0f;
#if defined(RAD_GAMECUBE) || defined(RAD_XBOX)
        float leftRight = mController->GetValue( SuperCamController::stickX );
#elif defined(RAD_WIN32)
        float left = mController->GetValue( SuperCamController::carLookLeft );
        float right = mController->GetValue( SuperCamController::carLookRight );
        float leftRight = ( right > left ) ? right : -left;
#else //This is PS2
        float leftRight = mController->GetValue( SuperCamController::r2 ) - mController->GetValue( SuperCamController::l2 );
#endif

        float dir = rmt::Sign( leftRight );
        //Look right so move left (I like it inverted here.)
        rod->Set( dir * invertMod * LOOK_OFFSET_DIST, LOOK_OFFSET_HEIGHT, LOOK_OFFSET_BACK );
        return false;
    }

#endif
    mData.GetRod( rod );
    return false;
}

//=============================================================================
// FollowCam::IsPushingStick
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool FollowCam::IsPushingStick()
{
#if defined(RAD_XBOX) || defined(RAD_GAMECUBE)
    float xAxis = mController->GetValue( SuperCamController::stickX );
#elif defined(RAD_WIN32)
    float left = mController->GetValue( SuperCamController::carLookLeft );
    float right = mController->GetValue( SuperCamController::carLookRight );
    float xAxis = ( right > left ) ? right : -left;
#else
    float xAxis = mController->GetValue( SuperCamController::l2 ) - mController->GetValue( SuperCamController::r2 );
#endif

    return ( !rmt::Epsilon( xAxis, 0.0f, 0.001f ) &&
             rmt::Sign( xAxis ) == rmt::Sign( mXAxis ) );
}

