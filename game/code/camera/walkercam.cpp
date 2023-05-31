//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        walkercam.cpp
//
// Description: Implement WalkerCam
//
// History:     25/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
//These are included in the precompiled headers on XBOX and GC
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <p3d/pointcamera.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <camera/WalkerCam.h>
#include <camera/WalkerCamData.h>
#include <camera/WalkerCamDataChunk.h>
#include <camera/isupercamtarget.h>
#include <camera/supercamcentral.h>
#include <camera/supercamcontroller.h>
#include <camera/supercammanager.h>

#include <memory/srrmemory.h>

#include <events/eventmanager.h>
#include <events/eventenum.h>

#include <main/game.h>

#include <worldsim/character/character.h>
#include <worldsim/character/charactertarget.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/ped/pedestrianmanager.h>

#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/animatedicon.h>

#include <ai/actor/intersectionlist.h>

#include <choreo/utility.hpp>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifdef DEBUGWATCH
float SLIDE_INTERVAL = 0.06f;
float AVERAGE_LAG = 1.0f;
float PED_MAX_DIST = 5.0f;
float PED_MIN_DIST = 0.0f;
float PED_ZOOM_OFFSET = 0.49f;
float PED_FOV_TEST = 0.567424f;  //~33 deg
unsigned int MIN_PED_ACCUM = 1500;
static float CREEPY_TWIST_WALKER = 0.176f;

rmt::Vector gLookFromR;
rmt::Vector gLookFromL;
rmt::Vector gLookToR;
rmt::Vector gLookToL;
rmt::Vector gLookFromU;
rmt::Vector gLookFromD;
rmt::Vector gLookToU;
rmt::Vector gLookToD;

#else
const float SLIDE_INTERVAL = 0.06f;
const float AVERAGE_LAG = 1.0f;
const float PED_MAX_DIST = 5.0f;
const float PED_MIN_DIST = 0.0f;
const float PED_ZOOM_OFFSET = 0.49f;
const float PED_FOV_TEST = 0.567424f;  //~33 deg
const unsigned int MIN_PED_ACCUM = 1500;
static const float CREEPY_TWIST_WALKER = 0.176f;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// WalkerCam::WalkerCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WalkerCam::WalkerCam() :
    mFOVDelta( 0.0f ),
    mMagnitude( 0.0f ),
    mMagnitudeDelta( 0.0f ),
    mElevation( 0.0f ),
    mElevationDelta( 0.0f ),
    mIsAirborn( false ),
    mLandingCountdown( 0 ),
    mCameraHeight( 0.0f ),
    mNumCollisions( 0 ),
    mLastCollisionFrame( 0 ),
    mLastCharacter( 0 ),
    mPedTimeAccum( 0 ),
    mXAxis( 0.0f ),
    mOldMagnitude( 0.0f ),
    mCollectible( NULL )
{
    mTarget = NULL;

    mTargetPos.Set(0.0f, 0.0f, 0.0f);
    mTargetPosDelta.Set(0.0f, 0.0f, 0.0f);
    mPosition.Set( 0.0f, 0.0f, 0.0f );
    mDesiredPositionDelta.Set( 0.0f, 0.0f, 0.0f );

    mGroundOffset.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// WalkerCam::~WalkerCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WalkerCam::~WalkerCam()
{
}

//=============================================================================
// WalkerCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::Update( unsigned int milliseconds )
{
    if ( mTarget->IsUnstable() )
    {
        return;
    }

    //This is to adjust interpolation when we're running substeps.
    float timeMod = 1.0f;
    timeMod = (float)milliseconds / 16.0f;

    UpdatePositionNormal( milliseconds, timeMod );

    //---------  Adjust where we look

    rmt::Vector targetPos;
    rmt::Vector desiredTargetPos;

    GetTargetPosition( &targetPos );

#ifdef RAD_DEBUG
    rAssert( !rmt::IsNan( targetPos.x ) );
    rAssert( !rmt::IsNan( targetPos.y ) );
    rAssert( !rmt::IsNan( targetPos.z ) );
#endif

    desiredTargetPos = targetPos;

    desiredTargetPos.y = mTargetPos.y;

    //Look up
    rmt::Vector centre = targetPos;
    
    centre.y += mData.GetUpAngle();
    
    float lookUp = mController->GetValue( SuperCamController::lookToggle );

    if ( !GetCamera()->SphereVisible( centre, 1.0f ) && lookUp <= 0.5f )
    {
        desiredTargetPos = targetPos;
    }
    else
    {
        //Look down
        
        centre = targetPos;
        centre.y -= mData.GetUpAngle();

        if ( !GetCamera()->SphereVisible( centre, 1.0f ) && lookUp <= 0.5f  )
        {
            desiredTargetPos = targetPos;
        }
        else
        {
            //Look normal
            if ( !mTarget->IsCar() )
            {
                if ( lookUp > 0.5f )
                {
                    const float howFarUp = 3.0f;
                    float addY = howFarUp * lookUp;
                    targetPos.y += addY;
                    desiredTargetPos = targetPos;
                }
            }

            rmt::Vector camPos;
            GetPosition( &camPos );

            rmt::Vector camToTarg;
            camToTarg.Sub( targetPos, camPos );

            float mag, theta, phi;
            rmt::CartesianToSpherical( camToTarg.x, camToTarg.z, camToTarg.y, &mag, &theta, &phi );

            if ( phi > 1.65f )
            {
                desiredTargetPos = targetPos;
            }
        }
    }


    rmt::Vector targetVelocity;
    mTarget->GetVelocity( &targetVelocity );

    float targLag = mData.GetTargetLag() * timeMod;
    if ( mTarget->IsCar() || targetVelocity.y < -1.0f )
    {
        targLag *= 2.0f;
    }

    CLAMP_TO_ONE(targLag);

    MotionCubic( &mTargetPos.x, &mTargetPosDelta.x, desiredTargetPos.x, targLag );
    MotionCubic( &mTargetPos.y, &mTargetPosDelta.y, desiredTargetPos.y, targLag );
    MotionCubic( &mTargetPos.z, &mTargetPosDelta.z, desiredTargetPos.z, targLag );

    //---------  Goofin' with the FOV

    if ( !mTarget->IsCar() )
    {
        float zoom = mController->GetValue( SuperCamController::zToggle );

        float FOV = GetFOV();
        float pedPCT = IsTargetNearPed( milliseconds );
        float offset = PED_ZOOM_OFFSET * pedPCT;

        if ( GetFlag((Flag)CUT ) )
        {
            FilterFov( zoom, mData.GetMinFOV(), mData.GetMaxFOV(), FOV, mFOVDelta, 1.0f, timeMod, offset );
        }
        else
        {
            FilterFov( zoom, mData.GetMinFOV(), mData.GetMaxFOV(), FOV, mFOVDelta, mData.GetFOVLag(), timeMod, offset );
        }

        SetFOV( FOV );

        //---------  Goofin' with the twist
        //Are we on level 7?
        if ( GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L7 && pedPCT > 0.0f)
        {
            rmt::Vector velocity;
            mTarget->GetVelocity( &velocity );
            float minCharSpeed = CharacterTune::sfMaxSpeed / 3.0f;
            minCharSpeed *= minCharSpeed;

            if ( velocity.MagnitudeSqr() < minCharSpeed )
            {
                SetTwist( CREEPY_TWIST_WALKER * pedPCT );
            }
            else
            {
                SetTwist( 0.0f );
            }
        }
        else
        {
            SetTwist( 0.0f );
        }
    }

    //---------  Set values.

#ifdef RAD_DEBUG
    rAssert( !rmt::IsNan( mPosition.x ) );
    rAssert( !rmt::IsNan( mPosition.y ) );
    rAssert( !rmt::IsNan( mPosition.z ) );

    rAssert( !rmt::IsNan( mTargetPos.x ) );
    rAssert( !rmt::IsNan( mTargetPos.y ) );
    rAssert( !rmt::IsNan( mTargetPos.z ) );
#endif

    SetCameraValues( milliseconds, mPosition, mTargetPos );
}

//=============================================================================
// WalkerCam::UpdateForPhysics
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::UpdateForPhysics( unsigned int milliseconds )
{
    //If we're in collision OR we're just coming out of collision.
    bool hasGroundOffset = !rmt::Epsilon( mGroundOffset.MagnitudeSqr(), 0.0f, 0.01f);
    if ( mNumCollisions || hasGroundOffset || GetFlag( (Flag)CUT ) )
    {
        float timeMod = 1.0f;
        timeMod = (float)milliseconds / 16.0f;

        if ( mNumCollisions )
        {
            //Adjust the mPosition and the mMagnitude if the camera is colliding.
            UpdatePositionInCollision( milliseconds, timeMod );
        }

        if ( hasGroundOffset )
        {
            mPosition.Add( mGroundOffset );

            rmt::Vector posToTarg;
            posToTarg.Sub( mTargetPos, mPosition );
            mMagnitude = posToTarg.Magnitude();
        }

        //This should test that the magnitude of the rod isn't too long.  If it 
        //is, we ray cast backwards and move there.
        if ( mMagnitude > (mData.GetMaxMagnitude() * 1.2f) ||
             mTargetPos.y - mPosition.y > mData.GetMinMagnitude() ||
             GetFlag( (Flag)CUT ) )
        {
            rmt::Vector lookFrom;
            GetTargetPosition( &lookFrom );

            rmt::Vector lookTo;
            GetPosition( &lookTo );

            rmt::Vector fromTo;
            fromTo.Sub( lookTo, lookFrom );
            fromTo.NormalizeSafe();
            fromTo.Scale( mData.GetMinMagnitude() );

            lookTo.Add( lookFrom, fromTo );

            IntersectionList& iList = GetSuperCamManager()->GetSCC( GetPlayerID() )->GetIntersectionList();

            if ( iList.LineOfSight( lookFrom, lookTo, 0.1f, true ) )
            {
                //Go here!
                mPosition = lookTo;
                mMagnitude = mData.GetMinMagnitude();
            }
            else
            {
                rmt::Vector intersection( 0.0f, 0.0f, 0.0f );
                iList.TestIntersectionStatics( lookFrom, lookTo, &intersection );

                rmt::Vector fromToIntersection;
                fromToIntersection.Sub( intersection, lookFrom );
                if ( rmt::Epsilon( intersection.MagnitudeSqr(), 0.0f, 0.01f ) ||
                     fromToIntersection.MagnitudeSqr() < (mData.GetMinMagnitude() * mData.GetMinMagnitude() ) )
                {
                    //Trouble.
                    GetTargetPosition( &mPosition );
                    mPosition.y += 2.0f;
                    mMagnitude = 2.0f;
                }
                else
                {
                    mPosition = intersection;
                    mMagnitude = fromToIntersection.Magnitude();
                }
            }
        }

        SetCameraValues( 0, mPosition, mTargetPos ); //No extra transition
    }
    else if ( GetFlag( (Flag)IN_COLLISION ) && !IsPushingStick() ) 
    {
        SetFlag( (Flag)IN_COLLISION, false );

        //Get the old position.
        GetPosition( &mOldPos );
        mOldMagnitude = mMagnitude;
    }

    SetFlag( (Flag)CUT, false );
}

//=============================================================================
// WalkerCam::LoadSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned char* settings )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::LoadSettings( unsigned char* settings )
{
}

//=============================================================================
// WalkerCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::SetTarget( ISuperCamTarget* target )
{
    rAssert( target );

    mTarget = target;

    //Load the camera data for this guy since he is the primary
    //dood.

    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( SuperCamCentral::CAMERA_INVENTORY_SECTION );
    HeapMgr()->PushHeap( GMA_TEMP );
    tInventory::Iterator<WalkerCamDataChunk> it;
    HeapMgr()->PopHeap( GMA_TEMP );
    
    WalkerCamDataChunk* wcD = it.First();

    while( wcD )
    {
        if ( wcD->mID == target->GetID() )
        {
            //Load the data.
            mData.SetMaxMagnitude( wcD->mMaxMagnitude);
            mData.SetMaxMagnitude( wcD->mMaxMagnitude);
            mData.SetElevation( wcD->mElevation );

            p3d::inventory->Remove( wcD );

            p3d::inventory->PopSection();
            return;
        }

        wcD = it.Next();
    }

    p3d::inventory->PopSection();
    rDebugString( "There should have been camera data loaded!\n" );
}

//=============================================================================
// WalkerCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::AddTarget( ISuperCamTarget* target )
{
    //We don't care.
    return;
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// WalkerCam::OnDisplay
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WalkerCam::OnDisplay() const
{
#ifdef DEBUGWATCH
    if ( mNumCollisions == 1 && mPlaneIntersectPoint.MagnitudeSqr() != 0.0f )
    {
        rmt::Vector camPos;
        rmt::Vector targPos;

        GetPosition( &camPos );
        GetTargetPosition( &targPos );

        rmt::Vector normal = mCollisionOffset[ 0 ];

        rmt::Vector pointInPlane;
        pointInPlane.Add( mPlaneIntersectPoint, normal );
        float D = -(normal.DotProduct( pointInPlane ));
        rmt::Plane collisionPlane( normal, D );

        normal.NormalizeSafe();

        rmt::Vector left;
        left.CrossProduct( rmt::Vector( 0.0f, 1.0f, 0.0f ), normal );

        rmt::Vector line;

        line.Add( mPlaneIntersectPoint, normal );

        pddiPrimStream* stream;
    
        stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINES, PDDI_V_C, 6);

        line.Add( mPlaneIntersectPoint, normal );
        stream->Colour( tColour( 0, 255, 0 ) );
        stream->Coord(line.x, line.y, line.z);

        normal.Scale( -1.0f );
        line.Add( mPlaneIntersectPoint, normal );
        stream->Colour( tColour( 0, 255, 0 ) );
        stream->Coord(line.x, line.y, line.z);

        line.Add( mPlaneIntersectPoint, left );
        stream->Colour( tColour( 0, 255, 0 ) );
        stream->Coord(line.x, line.y, line.z);

        left.Scale( -1.0f );
        line.Add( mPlaneIntersectPoint, left );
        stream->Colour( tColour( 0, 255, 0 ) );
        stream->Coord(line.x, line.y, line.z);

        stream->Colour( tColour( 0, 0, 255 ) );
        stream->Coord(camPos.x, camPos.y, camPos.z);

        stream->Colour( tColour( 0, 0, 255 ) );
        stream->Coord(targPos.x, targPos.y, targPos.z);
        p3d::pddi->EndPrims(stream);
    }

    pddiPrimStream* stream;
    stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINES, PDDI_V_C, 2);
    stream->Colour( tColour( 0, 255, 0 ) );
    stream->Coord( gLookFromR.x, gLookFromR.y, gLookFromR.z );
    stream->Coord( gLookToR.x, gLookToR.y, gLookToR.z );
    p3d::pddi->EndPrims(stream);
    stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINES, PDDI_V_C, 2);
    stream->Colour( tColour( 0, 255, 0 ) );
    stream->Coord( gLookFromL.x, gLookFromL.y, gLookFromL.z );
    stream->Coord( gLookToL.x, gLookToL.y, gLookToL.z );
    p3d::pddi->EndPrims(stream);
    stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINES, PDDI_V_C, 2);
    stream->Colour( tColour( 0, 255, 0 ) );
    stream->Coord( gLookFromU.x, gLookFromU.y, gLookFromU.z );
    stream->Coord( gLookToU.x, gLookToU.y, gLookToU.z );
    p3d::pddi->EndPrims(stream);
    stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINES, PDDI_V_C, 2);
    stream->Colour( tColour( 0, 255, 0 ) );
    stream->Coord( gLookFromD.x, gLookFromD.y, gLookFromD.z );
    stream->Coord( gLookToD.x, gLookToD.y, gLookToD.z );
    p3d::pddi->EndPrims(stream);
#endif
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// WalkerCam::UpdatePositionNormal
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds, float timeMod )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::UpdatePositionNormal( unsigned int milliseconds, float timeMod )
{
    rmt::Vector targetPos, camPos;
    rmt::Vector desiredPosition;
    rmt::Vector desiredTargetPos;
    rmt::Vector rod;

    GetTargetPosition( &targetPos );
    GetPosition( &camPos );

    if ( mTarget->IsAirborn() && !mIsAirborn )
    {
        //Going Airborn!
        mIsAirborn = true;

        mCameraHeight = camPos.y;        
    }
    else if ( !mTarget->IsAirborn() && mIsAirborn )
    {
        //Come down!
        mIsAirborn = false;
    }

    desiredPosition = targetPos;
    desiredTargetPos = targetPos;

    float desiredMagnitude, rotation, elevation;
        
    desiredMagnitude = mMagnitude;

    if ( GetFlag( (Flag)FIRST_TIME ) )
    {
        mTargetPos = targetPos;
        mMagnitude = mData.GetMagnitude();
        mPosition = camPos;
        mTargetPosDelta.Set( 0.0f, 0.0f, 0.0f );
        mDesiredPositionDelta.Set( 0.0f, 0.0f, 0.0f );
    }

    bool topRayNotBlocked = false;
    bool bottomRayNotBlocked = false;

    if ( GetFlag( (Flag)CUT ) )
    {
        //Reset the FOV.
        SetFOV( mData.GetMinFOV() + (mData.GetMaxFOV() - mData.GetMinFOV()) );

        mMagnitude = mData.GetMagnitude();
        rotation = mData.GetRotation();
        mElevation = mData.GetElevation();
        mElevationDelta = 0.0f;
 
        mTargetPos = targetPos;
        mTargetPosDelta.Set( 0.0f, 0.0f, 0.0f );
        mDesiredPositionDelta.Set( 0.0f, 0.0f, 0.0f );

        //Reset the deltas
        mFOVDelta = 0.0f;
        mMagnitudeDelta = 0.0f;
    }
    else
    {
        //Try to look at the collectible
        bool lookAtCollectible = false;

        if ( mCollectible )
        {
            rmt::Vector collectiblePos;
            mCollectible->GetPosition( collectiblePos );

            rmt::Vector targToCollect;
            targToCollect.Sub( collectiblePos, targetPos );
            if ( targToCollect.MagnitudeSqr() < 100.0f )
            {
                //We should look at the collectible
 
                lookAtCollectible = true;
                rod = targToCollect;
                rod.NormalizeSafe();
                rod.Scale( mData.GetMagnitude() );
                rod.x *= -1.0f;
                rod.z *= -1.0f;
            }
        }
        
        if ( !lookAtCollectible )
        {
            rod.Sub( camPos, targetPos );
        }

        float magWaste;
        rmt::CartesianToSpherical( rod.x, rod.z, rod.y, &magWaste, &rotation, &elevation );

        if ( GetFlag( (Flag)FIRST_TIME ) )
        {
            mElevation = elevation;
        }
    }

    SetFlag( (Flag)FIRST_TIME, false );

    //---------  Adjust the camera according to user input...

    if ( !GetFlag((Flag)CUT ) )
    {
        float xAxis = mController->GetAxisValue( SuperCamController::stickX );
        float zAxis = mController->GetAxisValue( SuperCamController::stickY );

        if ( GetSuperCamManager()->GetSCC( GetPlayerID() )->IsInvertedCameraEnabled() )
        {
            xAxis *= -1.0f;  //Invert
        }

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
        if ( mController->IsWheel() )
        {
            //This is a wheel.  No left right on wheels.
            xAxis *= 3.0f;
            rmt::Clamp( xAxis, -1.0f, 1.0f );
        }
#endif

        if ( GetFlag( (Flag)IN_COLLISION ) && IsPushingStick() )
        {
            xAxis = 0.0f;
        }


        //Auto avoid visual impediments
        if ( !mTarget->IsCar() &&
             ( IsStickStill() ||
               ( GetFlag( (Flag)IN_COLLISION ) && IsPushingStick() ) ) )
        {
            float nearPlane = GetNearPlane();
            float radius = GetCollisionRadius() * 0.8f;

            rmt::Matrix camMat;
            camMat.IdentityTranslation();

            rmt::Vector heading;
            GetHeading( &heading );
            rmt::Vector vup;
            GetCameraUp( &vup );
            camMat.FillHeading( heading, vup );  //I do this because the tCamera may not be the same as I think it is.

            //Test the sides and rotate if there's a problem.
            rmt::Vector lookFrom( radius, 0.0f, nearPlane );
            lookFrom.Transform( camMat );
            lookFrom.Add( camPos );

            rmt::Vector lookTo( 0.5f, 0.0f, -0.5f );
            lookTo.Transform( camMat );
            lookTo.Add( targetPos );

#ifdef DEBUGWATCH
            gLookFromR = lookFrom;
            gLookToR = lookTo;
#endif

            IntersectionList& iList = GetSuperCamManager()->GetSCC( GetPlayerID() )->GetIntersectionList();
            if ( !iList.LineOfSight( lookFrom, lookTo, 0.1f ) )
            {
                xAxis += -0.25f;
            }

            lookFrom.Set( -radius, 0.0f, nearPlane );
            lookFrom.Transform( camMat );
            lookFrom.Add( camPos );

            lookTo.Set( -0.5f, 0.0f, -0.5f );
            lookTo.Transform( camMat );
            lookTo.Add( targetPos );

#ifdef DEBUGWATCH
            gLookFromL = lookFrom;
            gLookToL = lookTo;
#endif
            if ( !iList.LineOfSight( lookFrom, lookTo, 0.1f ) )
            {
                xAxis += 0.25f;
            }

            //Look to see if we can go up.
            lookFrom.Set( 0.0f, radius, nearPlane );
            lookFrom.Transform( camMat );
            lookFrom.Add( camPos );

            lookTo.Set( 0.0f, 0.5f, -0.5f );
            lookTo.Transform( camMat );
            lookTo.Add( targetPos );

#ifdef DEBUGWATCH
            gLookFromU = lookFrom;
            gLookToU = lookTo;
#endif
            topRayNotBlocked = iList.LineOfSight( lookFrom, lookTo, 0.1f );

            //Do this one the other way since we want the ray to maintain it's length.
            lookTo = targetPos;
            lookTo.y = targetPos.y + 0.5f;

            lookFrom.Set( 0.0f, 0.0, -0.2f );
            lookFrom.Transform( camMat );
            
            lookFrom.x += camPos.x;
            lookFrom.y = lookTo.y;
            lookFrom.z += camPos.z;

            rmt::Vector toFrom;
            toFrom.Sub( lookFrom, lookTo );
            toFrom.Normalize();
            toFrom.Scale( mMagnitude + 0.5f );

            lookFrom.Add( lookTo, toFrom );

#ifdef DEBUGWATCH
            gLookFromD = lookFrom;
            gLookToD = lookTo;
#endif
            bottomRayNotBlocked = iList.LineOfSight( lookFrom, lookTo, 0.1f );
        }

        rmt::Clamp( xAxis, -1.0f, 1.0f );

        rotation += ( xAxis * mData.GetRotationIncrement() * timeMod );

        const unsigned int frameTest = 0;
        float lookUp = mController->GetValue( SuperCamController::lookToggle );

        if ( rmt::Fabs( zAxis ) == 1.0f && mLastCollisionFrame == 0 || 
            ( mLastCollisionFrame + frameTest < GetGame()->GetFrameCount() ) )
        {
            float halfMag = (mData.GetMaxMagnitude() - mData.GetMinMagnitude()) / 2.0f;

            desiredMagnitude -= ( zAxis * halfMag );

            if ( desiredMagnitude < mData.GetMinMagnitude() )
            {
                desiredMagnitude = mData.GetMinMagnitude();
            }
            else if ( desiredMagnitude > mData.GetMaxMagnitude() ) 
            {
                desiredMagnitude = mData.GetMaxMagnitude();
            }

            float lag = mData.GetLag() * timeMod;
            CLAMP_TO_ONE(lag);

            MotionCubic( &mMagnitude, &mMagnitudeDelta, desiredMagnitude, lag );
            mLastCollisionFrame = 0;
        }
    }
   
    //---------  Adjust the camera according to where the target is...

    if ( GetFlag( (Flag)CUT ) )
    {
        //Transform the rod to have the angle rmt::PI be behind the target...
        rmt::Matrix mat;
        rmt::Vector heading, vup;
        mTarget->GetHeading( &heading );

        // Dusit: 
        // assume vup of target is always 0,1,0 because
        // the code won't work otherwise, especially now 
        // that the player character can transit into simulation control
        // and tumble about in physics.
        //mTarget->GetVUP( &vup );
        vup.Set( 0.0f, 1.0f, 0.0f );

        mat.Identity();
        mat.FillHeading( heading, vup );

        mElevation = mData.GetElevation();
        rmt::SphericalToCartesian( mMagnitude, rotation, mElevation, &rod.x, &rod.z, &rod.y );
        mElevationDelta = 0.0f;

        rod.Transform( mat );
        
        desiredPosition.Add( rod );

        mPosition = desiredPosition;
    }
    else
    {
        float elevation = mData.GetElevation();

        rmt::Vector groundNormal, pos;
        mTarget->GetTerrainIntersect( pos, groundNormal );

        if ( groundNormal.y < 0.97f )
        {
            elevation -= 0.3f; 
        }
        else if ( topRayNotBlocked && !bottomRayNotBlocked )
        {
            elevation -= 0.2f;
        }

        float elevlag = SLIDE_INTERVAL * timeMod;
        CLAMP_TO_ONE(elevlag);

        MotionCubic( &mElevation, &mElevationDelta, elevation, elevlag );

        rmt::SphericalToCartesian( mMagnitude, rotation, mElevation, &rod.x, &rod.z, &rod.y );

        if ( mMagnitude < GetNearPlane() + 1.5f )
        {
            rod.x *= -1.0f;
            rod.z *= -1.0f;

            desiredPosition.Add( rod );

            mPosition = desiredPosition;
            mPosition.Add( mGroundOffset );  //Add the ground correction.

            mMagnitude = mData.GetMinMagnitude();
            mMagnitudeDelta = 0.0f;

            mDesiredPositionDelta.Set( 0.0f, 0.0f, 0.0f );

            //Tell the character controller that we're cutting to a new position.
            GetEventManager()->TriggerEvent( EVENT_CAMERA_CHANGE, this );
        }
        else
        {
            float lag = mData.GetLag() * timeMod;
            CLAMP_TO_ONE(lag);

            desiredPosition.Add( rod );

            if ( mIsAirborn )
            {
                desiredPosition.y = mCameraHeight;
            }

            MotionCubic( &mPosition.x, &mDesiredPositionDelta.x, desiredPosition.x, lag );
            MotionCubic( &mPosition.y, &mDesiredPositionDelta.y, desiredPosition.y, lag );
            MotionCubic( &mPosition.z, &mDesiredPositionDelta.z, desiredPosition.z, lag );
        }
    }
}

//=============================================================================
// WalkerCam::UpdatePositionInCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds, float timeMod )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::UpdatePositionInCollision( unsigned int milliseconds, float timeMod )
{
    //Store the new collision and the num of collisions;
    mLastCollisionFrame = GetGame()->GetFrameCount();

    rmt::Vector camPos;
    GetPosition( &camPos );

    if ( mNumCollisions == 1 )
    {
        UpdatePositionOneCollsion( milliseconds, timeMod );
    }
    else
    {
        UpdatePositionMultipleCollision( milliseconds, timeMod );
    }

    SetFlag( (Flag)IN_COLLISION, true );
    mXAxis = mController->GetAxisValue( SuperCamController::stickX );
}

//=============================================================================
// WalkerCam::UpdatePositionOneCollsion
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds, float timeMod, unsigned int collisionIndex )
//
// Return:      bool 
//
//=============================================================================
bool WalkerCam::UpdatePositionOneCollsion( unsigned int milliseconds, float timeMod, unsigned int collisionIndex )
{

    rmt::Vector camPos;
    GetPosition( &camPos );
    rmt::Vector targetPos;
    mTarget->GetPosition( &targetPos );

    rmt::Vector newPos;
    mPlaneIntersectPoint.Set( 0.0f, 0.0f, 0.0f );
    newPos.Add( camPos, mCollisionOffset[ collisionIndex ] );

    mPosition = newPos;
    mDesiredPositionDelta.Set( 0.0f, 0.0f, 0.0f );

    rmt::Vector camToTarg;
    camToTarg.Sub( targetPos, mPosition );

    mMagnitude = camToTarg.Magnitude();
    mMagnitudeDelta = 0.0f;

/*
    bool speedUp = false;

    float xAxis = mController->GetValue( SuperCamController::stickX );

    if ( GetFlag( (Flag)IN_COLLISION ) )
    {
        if ( IsPushingStick() )
        {
            return false;
        }
        else if ( !IsStickStill() )
        {
            speedUp = true;
        }
    }
    else
    {
        if ( !IsStickStill() )
        {
            mPosition = mOldPos;
            mMagnitude = mOldMagnitude;
            return false;
        }
    }

    rmt::Vector camPos, targetPos;
    GetPosition( &camPos );
    GetTargetPosition( &targetPos );

    //Let's use the plane equation Ax+By+Cz+D = 0
    //Where x,y,z are points in 3D and A,B,C are the plane normal
    //D is the distance to the origin.  YEAH!
    rmt::Vector normal = mCollisionOffset[ collisionIndex ];
    rmt::Vector normalScaled = normal;
    normalScaled.NormalizeSafe();
    normalScaled.Scale( -GetNearPlane() * 1.2f );

    rmt::Vector pointInPlane;
    pointInPlane.Add( camPos, normal );
    pointInPlane.Add( normalScaled );

    float D = -(normal.DotProduct( pointInPlane ));
    rmt::Plane collisionPlane( normal, D );

    rmt::Vector camToTargDir;
    camToTargDir.Sub( targetPos, camPos );
    camToTargDir.NormalizeSafe();

    rmt::Vector newPos;
    mPlaneIntersectPoint.Set( 0.0f, 0.0f, 0.0f );
    newPos.Add( camPos, mCollisionOffset[ collisionIndex ] );

    if ( collisionPlane.Intersect( camPos, camToTargDir, &mPlaneIntersectPoint ) )
    {
        rmt::Vector camToIntersect;
        camToIntersect.Sub( mPlaneIntersectPoint, camPos );
        
        //Test to ignore intersects that are on the wrong side of the plane.
        if ( camToIntersect.DotProduct( camToTargDir ) > 0.0f && camToIntersect.MagnitudeSqr() < 16.0f )
        {
            rmt::Vector intersectPointOffset = mPlaneIntersectPoint;
            intersectPointOffset.Sub( normalScaled );

            newPos = intersectPointOffset;
        }
        else
        {
            mPlaneIntersectPoint.Set( 0.0f, 0.0f, 0.0f );
        }
    }
    else
    {
        mPlaneIntersectPoint.Set( 0.0f, 0.0f, 0.0f );
    }

    float lag = mData.GetCollisionLag() * timeMod;
    CLAMP_TO_ONE( lag );

//    if ( speedUp )
//    {
//        lag = 1.0f;
//    }

    mDesiredPositionDelta.Set( 0.0f, 0.0f, 0.0f );
    MotionCubic( &mPosition.x, &mDesiredPositionDelta.x, newPos.x, lag );
    MotionCubic( &mPosition.y, &mDesiredPositionDelta.y, newPos.y, lag );
    MotionCubic( &mPosition.z, &mDesiredPositionDelta.z, newPos.z, lag );

    rmt::Vector newDir;
    newDir.Sub( mPosition, targetPos );
    float mag = newDir.Magnitude();

    bool panic = false;
   
    if ( mag > mData.GetMaxMagnitude() )
    {
        //Build a new rod.
        newDir.NormalizeSafe();
        newDir.Scale( mData.GetMagnitude() );

        mPosition.Add( targetPos, newDir );
        mag = mData.GetMagnitude();

    }

    if ( mag < GetNearPlane() + 1.5f )
    {
        MotionCubic( &mPosition.x, &mDesiredPositionDelta.x, mOldPos.x, lag );
        MotionCubic( &mPosition.y, &mDesiredPositionDelta.y, mOldPos.y, lag );
        MotionCubic( &mPosition.z, &mDesiredPositionDelta.z, mOldPos.z, lag );

        rmt::Vector targToPos;
        targToPos.Sub( mPosition, targetPos );
        mag = targToPos.Magnitude();
    }

    mMagnitude = mag;
    mMagnitudeDelta = 0.0f;
*/
    return false;
}

struct OldCamData
{
    rmt::Vector position;
    rmt::Vector positionDelta;
    float elevation;
    float elevationDelta;
    float magnitude;
    float magnitudeDelta;
};

//=============================================================================
// WalkerCam::UpdatePositionMultipleCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds, float timeMod )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::UpdatePositionMultipleCollision( unsigned int milliseconds, float timeMod )
{
    OldCamData origData;

    origData.elevation          = mElevation;
    origData.elevationDelta     = mElevationDelta;
    origData.magnitude          = mMagnitude;
    origData.magnitudeDelta     = mMagnitudeDelta;
    origData.position           = mPosition;
    origData.positionDelta      = mDesiredPositionDelta;

    float desiredElevation  = 0.0f;
    float desiredMagnitude = 0.0f;
    rmt::Vector desiredPosition( 0.0f, 0.0f, 0.0f );

    unsigned int i;
    for ( i = 0; i < mNumCollisions; ++i )
    {
        UpdatePositionOneCollsion( milliseconds, timeMod, i );

        desiredElevation          += mElevation;
        desiredMagnitude          += mMagnitude;
        desiredPosition.Add( mPosition );

        //Reset
        mElevation              = origData.elevation;
        mElevationDelta         = origData.elevationDelta;
        mMagnitude              = origData.magnitude;
        mMagnitudeDelta         = origData.magnitudeDelta;
        mPosition               = origData.position;
        mDesiredPositionDelta   = origData.positionDelta;
    }

    mElevationDelta = 0.0f;
    mDesiredPositionDelta.Set( 0.0f, 0.0f, 0.0f );
    mMagnitudeDelta = 0.0f;

    desiredElevation += origData.elevation;
    desiredMagnitude += origData.magnitude;
    desiredPosition.x += origData.position.x;
    desiredPosition.y += origData.position.y;
    desiredPosition.z += origData.position.z;
    
    desiredElevation /= mNumCollisions + 1;
    desiredMagnitude /= mNumCollisions + 1;
    desiredPosition.x /= mNumCollisions + 1;
    desiredPosition.y /= mNumCollisions + 1;
    desiredPosition.z /= mNumCollisions + 1;

    float newLag = AVERAGE_LAG * timeMod;
    CLAMP_TO_ONE(newLag);

    MotionCubic( &mElevation, &mElevationDelta, desiredElevation, newLag );

    MotionCubic( &mPosition.x, &mDesiredPositionDelta.x, desiredPosition.x, newLag );
    MotionCubic( &mPosition.y, &mDesiredPositionDelta.y, desiredPosition.y, newLag );
    MotionCubic( &mPosition.z, &mDesiredPositionDelta.z, desiredPosition.z, newLag );

    MotionCubic( &mMagnitude, &mMagnitudeDelta, desiredMagnitude, newLag );
}

//=============================================================================
// WalkerCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WalkerCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Walker", GetPlayerID() );

    radDbgWatchAddFloat( &mData.mMinFOV, "Min FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mData.mMaxFOV, "Max FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mData.mLag, "Main lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mFOVLag, "FOV lag", nameSpace, NULL, NULL, 0.0f, 1.0f );

    radDbgWatchAddFloat( &mData.mMinMagnitude, "Min Magnitude", nameSpace, NULL, NULL, 1.0f, 100.0f );
    radDbgWatchAddFloat( &mData.mMaxMagnitude, "Max Magnitude", nameSpace, NULL, NULL, 1.0f, 100.0f );

    radDbgWatchAddFloat( &mData.mElevation, "Elevation (M)", nameSpace, NULL, NULL, 0.0f, 5.0 );
    radDbgWatchAddFloat( &mData.mRotation, "Rotation", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &mData.mMagnitude, "Magnitude", nameSpace, NULL, NULL, 0.0f, 100.0f );

    radDbgWatchAddFloat( &mData.mRotationIncrement, "Rotation Increment", nameSpace, NULL, NULL, 0.0f, rmt::PI_BY2 );

    radDbgWatchAddVector( &mData.mTargetOffset.x, "Target Offset (M)", nameSpace, NULL, NULL, 0.0f, 2.0f );

    radDbgWatchAddFloat( &mData.mTargetLag, "Target Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mData.mJumpLag, "Jump Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );

    radDbgWatchAddUnsignedInt( &mData.mLandingTransitionTime, "Landing Transition", nameSpace, NULL, NULL, 0, 10000 );

    radDbgWatchAddFloat( &mData.mUpAngle, "Up Height (M)", nameSpace, NULL, NULL, 0.0f, 5.0f );

    radDbgWatchAddFloat( &mData.mCollisionLag, "Collision Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );

    radDbgWatchAddFloat( &SLIDE_INTERVAL, "Slide Interval", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &AVERAGE_LAG, "Average Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );

    radDbgWatchAddFloat( &PED_MIN_DIST, "Ped Min Dist", nameSpace, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddFloat( &PED_MAX_DIST, "Ped Max Dist", nameSpace, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddFloat( &PED_ZOOM_OFFSET, "Ped Zoom", nameSpace, NULL, NULL, 0.0f, rmt::PI_BY2 );
    radDbgWatchAddFloat( &PED_FOV_TEST, "Ped FOV Test", nameSpace, NULL, NULL, 0.01f, rmt::PI );

    radDbgWatchAddUnsignedInt( &MIN_PED_ACCUM, "Min Ped Time Acc", nameSpace, NULL, NULL, 0, 10000 );

    radDbgWatchAddFloat( &CREEPY_TWIST_WALKER, "Twist", nameSpace, NULL, NULL, 0.0f, rmt::PI_2 );
#endif
}

//=============================================================================
// WalkerCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WalkerCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mData.mMinFOV );
    radDbgWatchDelete( &mData.mMaxFOV );
    radDbgWatchDelete( &mData.mLag );
    radDbgWatchDelete( &mData.mFOVLag );

    radDbgWatchDelete( &mData.mMinMagnitude );
    radDbgWatchDelete( &mData.mMaxMagnitude );

    radDbgWatchDelete( &mData.mElevation );
    radDbgWatchDelete( &mData.mRotation );
    radDbgWatchDelete( &mData.mMagnitude );

    radDbgWatchDelete( &mData.mRotationIncrement );

    radDbgWatchDelete( &mData.mTargetOffset.x );

    radDbgWatchDelete( &mData.mTargetLag );
    radDbgWatchDelete( &mData.mJumpLag );

    radDbgWatchDelete( &mData.mLandingTransitionTime );

    radDbgWatchDelete( &mData.mUpAngle );

    radDbgWatchDelete( &mData.mCollisionLag );

    radDbgWatchDelete( &SLIDE_INTERVAL );
    radDbgWatchDelete( &AVERAGE_LAG );

    radDbgWatchDelete( &PED_MIN_DIST );
    radDbgWatchDelete( &PED_MAX_DIST );
    radDbgWatchDelete( &PED_ZOOM_OFFSET );
    radDbgWatchDelete( &PED_FOV_TEST );

    radDbgWatchDelete( &MIN_PED_ACCUM );

    radDbgWatchDelete( &CREEPY_TWIST_WALKER );
#endif
}

//=============================================================================
// WalkerCam::IsPushingStick
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool WalkerCam::IsPushingStick()
{
    float xAxis = mController->GetAxisValue( SuperCamController::stickX );

    return ( !rmt::Epsilon( xAxis, 0.0f, 0.001f ) &&
             rmt::Sign( xAxis ) == rmt::Sign( mXAxis ) );
}

//=============================================================================
// WalkerCam::IsStickStill
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool WalkerCam::IsStickStill()
{
    float xAxis = mController->GetAxisValue( SuperCamController::stickX );
    return rmt::Epsilon( xAxis, 0.0f, 0.01f );
}

//=============================================================================
// WalkerCam::GetTargetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position, bool withOffset )
//
// Return:      void 
//
//=============================================================================
void WalkerCam::GetTargetPosition( rmt::Vector* position, 
                                          bool withOffset ) const
{
    rAssert( mTarget );

    mTarget->GetPosition( position );

    rAssert( !rmt::IsNan( position->x ) );
    rAssert( !rmt::IsNan( position->y ) );
    rAssert( !rmt::IsNan( position->z ) );

    if ( withOffset )
    {
        rmt::Vector offset;
        mData.GetTargetOffset( &offset );

        //Now put the offset in the target's space
/*        rmt::Matrix mat;
        rmt::Vector targetHeading, targetVUP;
        mTarget->GetHeading( &targetHeading );

#ifdef RAD_DEBUG
        rAssert( !rmt::IsNan( targetHeading.x ) );
        rAssert( !rmt::IsNan( targetHeading.y ) );
        rAssert( !rmt::IsNan( targetHeading.z ) );
#endif

        // Dusit: 
        // assume vup of target is always 0,1,0 because
        // the code won't work otherwise, especially now 
        // that the player character can transit into simulation control
        // and tumble about in physics.
        //mTarget->GetVUP( &targetVUP );
        targetVUP.Set( 0.0f, 1.0f, 0.0f );

        mat.Identity();
        mat.FillHeading( targetHeading, targetVUP );

        offset.Transform( mat );        
*/
        (*position).Add( offset );
    }
}

//=============================================================================
// WalkerCam::IsTargetNearPed
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      float 
//
//=============================================================================
float WalkerCam::IsTargetNearPed( unsigned int milliseconds )
{
    if ( mPedTimeAccum - static_cast<int>(milliseconds) <= 0 )
    {
        mPedTimeAccum = 0;
    }
    else
    {
        mPedTimeAccum -= milliseconds;
    }

    if ( mPedTimeAccum == 0 )
    {
        //Reset the time.
        mPedTimeAccum = MIN_PED_ACCUM;

        //Retest for a new ped
        CharacterManager* cm = GetCharacterManager();
        rmt::Vector targetPos;

        mTarget->GetPosition( &targetPos );

        int i;
        float pedMagSqr = 10000.0f;
        mLastCharacter = 0;

        //Find the closest, visible character.
        float oldFOV, oldAspect;
        GetCamera()->GetFOV( &oldFOV, &oldAspect );
        GetCameraNonConst()->SetFOV( PED_FOV_TEST, oldAspect );

        for ( i = 0; i < cm->GetMaxCharacters(); ++i )
        {
            Character* tempCharacter = cm->GetCharacter( i );
            //If it's valid and not the player.
            if ( tempCharacter != NULL && 
                 static_cast<ISuperCamTarget*>(tempCharacter->GetTarget()) != mTarget &&
                 !PedestrianManager::GetInstance()->IsPed( tempCharacter ) )
            {
                rmt::Vector charPos;
                tempCharacter->GetPosition( &charPos );

                if ( GetCamera()->SphereVisible( charPos, 1.0f ) )
                {
                    rmt::Vector targetToPed;
                    targetToPed.Sub( charPos, targetPos );
                    if ( targetToPed.MagnitudeSqr() < pedMagSqr )
                    {
                        if ( targetToPed.MagnitudeSqr() < PED_MAX_DIST * PED_MAX_DIST )
                        {
                            pedMagSqr = targetToPed.MagnitudeSqr();
                            mLastCharacter = tempCharacter->GetUID();
                        }
                    }
                }
            }
        }

        //Reset the FOV.
        GetCameraNonConst()->SetFOV( oldFOV, oldAspect );
    }

    //If we have a candidate ped...
    if ( mLastCharacter != static_cast< tUID >( 0 ) )
    {
        Character* c = GetCharacterManager()->GetCharacterByName(mLastCharacter);
        if(c)
        {
            rmt::Vector charPos;
            c->GetPosition( &charPos );
            rmt::Vector targetPos;
            mTarget->GetPosition( &targetPos );
            rmt::Vector targetToPed;
            targetToPed.Sub( charPos, targetPos );

            float distNormal = targetToPed.Magnitude() / (PED_MAX_DIST - PED_MIN_DIST);  //Square root!
            
            CLAMP_TO_ONE( distNormal );

            distNormal = 1.0f - distNormal;
            return distNormal;
        }
    }
    return 0.0f;
}

//=============================================================================
// WalkerCam::GetWatcherName
//=============================================================================
// Description: the name of the class for the watcher or other debug purposes
//
// Parameters:  NONE
//
// Return:      const char* - the name of the class 
//
//=============================================================================
#ifdef DEBUGWATCH
const char* WalkerCam::GetWatcherName() const
{
    return "WalkerCam";
}
#endif
