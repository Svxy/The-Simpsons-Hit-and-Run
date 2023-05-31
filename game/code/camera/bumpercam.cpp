//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        bumpercam.cpp
//
// Description: Implement BumperCam
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
#include <radmath/radmath.hpp>

//========================================
// Project Includes
//========================================
#include <camera/BumperCam.h>
#include <camera/isupercamtarget.h>

#include <worldsim/redbrick/vehicle.h>

#include <cheats/cheatinputsystem.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//That;
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// BumperCam::BumperCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BumperCam::BumperCam() :
    mTarget( NULL )
{
    mGroundOffset.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// BumperCam::~BumperCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BumperCam::~BumperCam()
{
}


//=============================================================================
// BumperCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void BumperCam::Update( unsigned int milliseconds )
{
    rAssertMsg( mTarget, "The BumperCam needs a target!" );

    if ( !mTarget->IsCar() )
    {
        return;
    }

    if ( GetFlag( (Flag)CUT ) )
    {
        SetFlag( (Flag)CUT, false );
    }

    //Reset the FOV.
    if ( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_SPEED_CAM ) )
    {
        SetFOV( 1.608495f );
    }
    else
    {
        SetFOV( mData.GetFOV() );
    }

    if ( GetFlag((Flag)FIRST_TIME ) )
    {
        rAssert( mTarget->IsCar() );

        rAssert( dynamic_cast< Vehicle* > ( mTarget ) != NULL );
        Vehicle* targV = static_cast<Vehicle*>(mTarget);
        rAssert( targV );
       
        float halfWheelBase = targV->mWheelBase * 0.5f;
        float top = targV->GetExtents().y;
        float front = halfWheelBase - GetNearPlane();

        top += 0.3f;

        mData.SetFrontPosition( rmt::Vector( 0.0f, top, front ) );
        mData.SetFrontTarget( rmt::Vector( 0.0f, top, front + 2.0f ) );

        float back = -halfWheelBase + GetNearPlane();

        top -= 0.3f;

        mData.SetBackPosition( rmt::Vector( 0.0f, top, back ) );
        mData.SetBackTarget( rmt::Vector( 0.0f, top, back - 2.0f ) );

        SetFlag( (Flag)FIRST_TIME, false );
    }
    
    bool lookBack = GetFlag( (Flag)LOOK_BACK );

    if ( lookBack )
    {
        SetFlag( (Flag)LOOK_BACK, false );
    }
 
    //---------  Buid a rod for the camera

    //This is where the camera is looking, not it's position.
    rmt::Vector desiredTarget;

    if ( lookBack )
    {
        //Use the back view.
        mData.GetBackTarget( &desiredTarget );
    }
    else
    {
        //Use the front view.
        mData.GetFrontTarget( &desiredTarget );
    }

    rmt::Matrix mat;
    rmt::Vector targetHeading, targetVUP;
    mTarget->GetHeading( &targetHeading );
    mTarget->GetVUP( &targetVUP );
    mat.Identity();
    mat.FillHeading( targetHeading, targetVUP );

    //Put the desiredTarget in the target space
    desiredTarget.Transform( mat );

    rmt::Vector desiredPosition;
    if ( lookBack )
    {
        mData.GetBackPosition( &desiredPosition );
    }
    else
    {
        mData.GetFrontPosition( &desiredPosition );
    }

    //Put desiredPosition in the space of the target.
    desiredPosition.Transform( mat );

    //---------  Set the position and target of the camera

    rmt::Vector targetPosition;
    mTarget->GetPosition( &targetPosition );

    desiredPosition.Add( targetPosition );
    desiredTarget.Add( targetPosition );

    SetCameraValues( milliseconds, desiredPosition, desiredTarget, &targetVUP );
}

//=============================================================================
// BumperCam::UpdateForPhysics
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void BumperCam::UpdateForPhysics( unsigned int milliseconds )
{
    rmt::Vector position, target, vup;
    GetPosition( &position );
    GetTarget( &target );
    mTarget->GetVUP( &vup );

    // Add ground collision.
    position.Add( mGroundOffset );

    SetCameraValues( 0, position, target, &vup );  //No extra transition
}

//=============================================================================
// BumperCam::LoadSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned char* settings )
//
// Return:      void 
//
//=============================================================================
void BumperCam::LoadSettings( unsigned char* settings )
{
}

//=============================================================================
// BumperCam::SetCollisionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
//
// Return:      void 
//
//=============================================================================
void BumperCam::SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
{
    //Only colliding with ground.
    mGroundOffset.y = groundOffset.y;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// BumperCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BumperCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Bumper", GetPlayerID() );

    radDbgWatchAddVector( &mData.mFrontPos.x, "Front Position", nameSpace, NULL, NULL, -10.0f, 10.0f );
    radDbgWatchAddVector( &mData.mFrontTarg.x, "Front Target", nameSpace, NULL, NULL, -10.0f, 10.0f );
    radDbgWatchAddVector( &mData.mBackPos.x, "Back Position", nameSpace, NULL, NULL, -10.0f, 10.0f );
    radDbgWatchAddVector( &mData.mBackTarg.x, "Back Target", nameSpace, NULL, NULL, -10.0f, 10.0f );
#endif
}

//=============================================================================
// BumperCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BumperCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mData.mFrontPos.x );
    radDbgWatchDelete( &mData.mFrontTarg.x );
    radDbgWatchDelete( &mData.mBackPos.x );
    radDbgWatchDelete( &mData.mBackTarg.x );
#endif
}
