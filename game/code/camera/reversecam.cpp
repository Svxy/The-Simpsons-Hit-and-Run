//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ReverseCam.cpp
//
// Description: Implement ReverseCam
//
// History:     11/14/2002 + Created -- Cary Brisebois
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
#include <camera/ReverseCam.h>
#include <camera/isupercamtarget.h>
#include <camera/SuperCamController.h>
#include <camera/supercamconstants.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <worldsim/redbrick/vehicle.h>

#include <mission/gameplaymanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifdef DEBUGWATCH
static float CREEPY_TWIST_REVERSE = 6.107f;
#else
static const float CREEPY_TWIST_REVERSE = 6.107f;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ReverseCam::ReverseCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ReverseCam::ReverseCam() :
    mTarget( NULL ),
    mYHeight( 0.0f ),
    mMagnitude( 4.0f ),
    mLag( 0.05f ),
    mTargetLag( 0.5f ),
    mCollisionOffset( NULL ),
    mCollisionRadius( 2.0f ),
    mNumCollisions( 0 ),
    mOtherY( 0.0f )
{
}

//==============================================================================
// ReverseCam::~ReverseCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ReverseCam::~ReverseCam()
{
}


//=============================================================================
// ReverseCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void ReverseCam::Update( unsigned int milliseconds )
{
    //This is to adjust interpolation when we're running substeps.
    float timeMod = (float)milliseconds / EXPECTED_FRAME_RATE;

    if ( GetFlag( (Flag)FIRST_TIME ) )
    {
        GetPosition( &mPosition );
        mPositionDelta.Set( 0.0f, 0.0f, 0.0f );

        mTarget->GetPosition( &mTargetPosition );
        mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );

        rmt::Vector camToTarg;
        camToTarg.Sub( mPosition, mTargetPosition );
        mYHeight = camToTarg.y;

        mMagnitude = camToTarg.Magnitude();

        rmt::Vector normal, pointInPlane;
        GetCameraUp( &normal );
        GetPosition( &pointInPlane );
        float D = -(normal.DotProduct( pointInPlane ));
        rmt::Plane collisionPlane( normal, D );

        rmt::Vector intersection;
        if ( collisionPlane.Intersect( mTargetPosition, rmt::Vector(0.0f, 1.0f, 0.0f), &intersection ) )
        {
            mOtherY = intersection.y - mTargetPosition.y;
        }

        mTargetPosition.y += mOtherY;

        SetFlag( (Flag)FIRST_TIME, false );
    }

    rmt::Vector desiredPosition, desiredTarget;

    GetPosition( &desiredPosition );
    mTarget->GetPosition( &desiredTarget );

    rmt::Vector targToCam;
    targToCam.Sub( desiredPosition, desiredTarget );
   
    if ( targToCam.MagnitudeSqr() > mMagnitude * mMagnitude )
    {
        targToCam.NormalizeSafe();
        targToCam.Scale( mMagnitude );

        desiredPosition.Add( desiredTarget, targToCam );
        //Override the y height.
        desiredPosition.y = desiredTarget.y + mYHeight;

        mMagnitude = mMagnitude; 
    }
    else if ( targToCam.MagnitudeSqr() < 49.0f )
    {
        rmt::Vector velocity;
        mTarget->GetVelocity( &velocity );

        //Do I go left or right?
        rmt::Vector heading, vup;
        mTarget->GetHeading( &heading );
        mTarget->GetVUP( &vup );

        if ( velocity.DotProduct( targToCam ) > 0.0f && heading.DotProduct( targToCam ) < 0.0f )
        {
            rmt::Vector left;
            left.CrossProduct( heading, vup );
            left.y = 0.0f;

            left.NormalizeSafe();

            if ( left.DotProduct(targToCam) < 0.0f )
            {
                left.Scale( -mMagnitude / 2.0f );
            }
            else
            {
                left.Scale( mMagnitude / 2.0f );
            }

            float lag = mLag * timeMod;
            CLAMP_TO_ONE( lag );
            left.Scale( lag );

            desiredPosition.Add( left );
        }
    }

    rmt::Vector velocity;
    mTarget->GetVelocity( &velocity );
    float velMod = velocity.MagnitudeSqr() / ( 20.0f * 20.0f );
    CLAMP_TO_ONE( velMod );

    float lag = (mLag + velMod) * timeMod;
    CLAMP_TO_ONE( lag );
    MotionCubic( &mPosition.x, &mPositionDelta.x, desiredPosition.x, lag );
    MotionCubic( &mPosition.y, &mPositionDelta.y, desiredPosition.y, lag );
    MotionCubic( &mPosition.z, &mPositionDelta.z, desiredPosition.z, lag );

    //Add the UP offset to make the camera "stay" looking at the right point.
    desiredTarget.y += mOtherY;

    lag = mTargetLag * timeMod;
    CLAMP_TO_ONE( lag );
    MotionCubic( &mTargetPosition.x, &mTargetPositionDelta.x, desiredTarget.x, lag );
    MotionCubic( &mTargetPosition.y, &mTargetPositionDelta.y, desiredTarget.y, lag );
    MotionCubic( &mTargetPosition.z, &mTargetPositionDelta.z, desiredTarget.z, lag );

/*
    //Are we on level 7?
    if ( GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L7 )
    {
        rmt::Vector velocity;
        mTarget->GetVelocity( &velocity );
        if ( velocity.MagnitudeSqr() > 0.1f )
        {
            SetTwist( 0.0f );
        }
        else
        {
            SetTwist( CREEPY_TWIST_REVERSE );
        }
    }
*/
    SetCameraValues( milliseconds, mPosition, mTargetPosition );
}

//=============================================================================
// ReverseCam::UpdateForPhysics
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void ReverseCam::UpdateForPhysics( unsigned int milliseconds )
{
    rmt::Vector currentPosition = mPosition;

    bool collision = false;

    unsigned int i;
    for ( i = 0; i < mNumCollisions; ++i )
    {
        rmt::Vector camHeading;
        GetHeadingNormalized( &camHeading );
        if ( camHeading.DotProduct( mCollisionOffset[ i ] ) >= -0.5f )
        {
            currentPosition.Add( mCollisionOffset[ i ] );
            collision = true;
        }
    }

    //Hmmm.  Offset the campos at the start by the ground plane collision offset.
    if ( mGroundOffset.MagnitudeSqr() > 0.1f )
    {
        currentPosition.Add( mGroundOffset );
        collision = true;
    }

    if ( collision )
    {
        mPositionDelta.Set( 0.0f, 0.0f, 0.0f );
    }

    rmt::Vector lookFrom = mTargetPosition;

    rmt::Vector lookTo = currentPosition;

    IntersectionList& iList = GetSuperCamManager()->GetSCC( GetPlayerID() )->GetIntersectionList();

    if ( !iList.LineOfSight( lookFrom, lookTo, 0.1f, true ) )
    {
        currentPosition = mTargetPosition;
        currentPosition.Add( rmt::Vector( 0.0f, 6.0f, 0.0f ) );  //BAD!
        mMagnitude = 6.0f;

        mPositionDelta.Set( 0.0f, 0.0f, 0.0f );
    }


    SetCameraValues( 0, currentPosition, mTargetPosition ); //No extra transition
}

//=============================================================================
// ReverseCam::EnableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ReverseCam::EnableShake()
{
    SetShaker( &mSineCosShaker );

    SuperCam::EnableShake();
}

//=============================================================================
// ReverseCam::DisableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ReverseCam::DisableShake()
{
    SuperCam::DisableShake();
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// ReverseCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ReverseCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Reverse", GetPlayerID() );

    radDbgWatchAddFloat( &mYHeight, "Y Height", nameSpace, NULL, NULL, 0.0f, 5.0f );
    radDbgWatchAddFloat( &mMagnitude, "Magnitude", nameSpace, NULL, NULL, 1.0f, 10.0f );
    radDbgWatchAddFloat( &mLag, "Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mTargetLag, "Target Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );

    radDbgWatchAddFloat( &CREEPY_TWIST_REVERSE, "Twist", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
#endif
}

//=============================================================================
// ReverseCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ReverseCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mYHeight );
    radDbgWatchDelete( &mMagnitude );
    radDbgWatchDelete( &mLag );
    radDbgWatchDelete( &mTargetLag );

    radDbgWatchDelete( &CREEPY_TWIST_REVERSE );
#endif
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// ReverseCam::CanSwitch
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool ReverseCam::CanSwitch() 
{
    rAssert( mTarget );

    bool canSwitch = true;

    if ( GetFlag( (Flag)CUT ) )
    {
        GetSuperCamManager()->GetSCC( GetPlayerID() )->SelectSuperCam( SuperCam::FOLLOW_CAM, SuperCamCentral::CUT | SuperCamCentral::FORCE | SuperCamCentral::NO_TRANS, 0 );
    }
    else if ( mTarget )
    {
        if ( mTarget->IsInReverse() )
        {
            canSwitch =  false;
        }
        else if ( static_cast<Vehicle*>(mTarget)->mGas < 0.5f )  //I hate this.
        {
            canSwitch = false;
        }
//        else if ( mNumCollisions > 0 )
//        {
//            canSwitch = false;
//        }
    }

    if ( canSwitch )
    {
        //Hmmmm..  HACK
        SetFlag( (Flag)END_TRANSITION, true );
    }

    return canSwitch;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
