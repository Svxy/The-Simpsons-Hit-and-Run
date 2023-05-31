//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pccam.cpp
//
// Description: Implement PCCam
//
// History:     7/25/2003 + Created -- Cary Brisebois
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
#include <camera/pccam.h>
#include <camera/isupercamtarget.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercamconstants.h>
#include <camera/supercamcontroller.h>

#include <worldsim/character/character.h>
#include <worldsim/character/charactercontroller.h>
#include <worldsim/character/charactermappable.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>

#include <ai/actor/intersectionlist.h>
#include <choreo/utility.hpp>

#include <input/inputmanager.h>
#include <input/usercontrollerwin32.h>
#include <input/mouse.h>
#include <input/realcontroller.h>

#include <roads/geometry.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
const float DEFAULT_LENGTH = 5.0f;
const float gMaxRodLength = DEFAULT_LENGTH;
const float gMinRodLength = 1.0f;

const rmt::Vector gOffset( 0.0f, 2.0f, 0.0f );

static float gDefaultElevation = 1.57079f;
const float gTopElevation = 3.0;
const float gBottomElevation = 1.57079f;
const float gElevationLag = 0.05f;
const float gElevationIncrement = 0.1f;

const float gMagAdjust = 0.0f;
const float gMagIncrement = 0.1f;

const float gMaxLookUp = 2.0f;

static float gDefaultPitch = 0.0f;
static float gPitchIncrement = 0.02f;
static float gPitchLag = 0.05f;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// PCCam::PCCam
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
PCCam::PCCam() :
    mTarget( NULL ),
    m_fAngularSpeed(0.0f),
    m_fYawVelocity(0.0f),
    m_fPitchVelocity(0.0f),
    mFOVDelta( 0.0f ),
    mLastPCCamFacing( 0 ),
    m_fInterpolationSpeed( 0.1f )
{
    mGroundOffset.Set( 0.0f, 0.0f, 0.0f );
}

//=============================================================================
// PCCam::~PCCam
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
PCCam::~PCCam()
{
}

//=============================================================================
// PCCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void PCCam::Update( unsigned int milliseconds )
{
    float timeMod = milliseconds / 16.0f;

    rAssert( mTarget );

    if ( GetFlag( (Flag)CUT ) )
    {
        mFOVDelta = 0.0f;
    }

    // Update the angular speed modifier.
    static float decelerationSpeed = 0.7f;
    m_fAngularSpeed = 1.0f * timeMod;
    m_fYawVelocity *= decelerationSpeed;
    m_fPitchVelocity *= decelerationSpeed;

    rmt::Vector targetPos;
    mTarget->GetPosition( &targetPos );

    rmt::Vector rod( 0.0f, 0.0f, 0.0f );

    float elevation = gDefaultElevation;

    //Calculate the length and the elevation based on the mouse values.
    if ( mController )
    {
        //---- Do the Elevation calculation

        float mouseUp = mController->GetValue( SuperCamController::mouseLookUp );
        float mouseDown = mController->GetAxisValue( SuperCamController::mouseLookDown );
        float yAxis = ( mouseUp > mouseDown ) ? -mouseUp : mouseDown;

        m_fYawVelocity += yAxis;

        //rDebugPrintf( " yAxis = %g              xAxis = %g \n", yAxis, xAxis );

        static float Y_SENSE_MOD = 0.1f;
        float mouseSense = static_cast<Mouse*>(GetInputManager()->GetController( 0 )->GetRealController( MOUSE ))->getSensitivityY();

        gDefaultElevation += gElevationIncrement * m_fYawVelocity * m_fAngularSpeed * mouseSense * Y_SENSE_MOD;

        //rDebugPrintf( "Angular Velocity = %g \n", m_fAngularSpeed );

        if ( gDefaultElevation > gTopElevation )
        {
            gDefaultElevation = gTopElevation;
            elevation = gTopElevation;
        }
        
        if ( gDefaultElevation < gBottomElevation )
        {
            elevation = gBottomElevation;
        }
    }

    //---- Do the Pitch calculation
    // If target is walking towards camera, let them... 
    Character* player = GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();

    float pitch = 0.0f;
    if ( player->mPCCamFacing == 2 )
    {
        if ( mLastPCCamFacing != 2 )
        {
            //Reset the pitch to be the direction we're already facing.
            rmt::Vector camHeading;
            GetHeading( &camHeading );
            camHeading.y = 0.0f;
            camHeading *= -1.0f;

            gDefaultPitch = choreo::GetWorldAngle( camHeading.x, camHeading.z );
        }

        float mouseSenseX = static_cast<Mouse*>(GetInputManager()->GetController( 0 )->GetRealController( MOUSE ))->getSensitivityX();

        gDefaultPitch += gPitchIncrement * m_fPitchVelocity * m_fAngularSpeed * mouseSenseX;

        pitch = gDefaultPitch;
    }

    rmt::SphericalToCartesian( -gMaxRodLength, pitch, elevation, &rod.z, &rod.x, &rod.y );


    ///////////////
    // Do camera rotation as necessary

    if( player->mPCCamFacing == 0 || player->mPCCamFacing == 1 )
    {
        static float ROT_DIR = 4.0f;
        static float X_SENSE_MOD = 0.01f;
        float mouseSense = static_cast<Mouse*>(GetInputManager()->GetController( 0 )->GetRealController( MOUSE ))->getSensitivityX();

        float fScaleFactor = 1.0f;
        if( player->IsTurbo() ) 
        {
            rmt::Vector playerVel;
            player->GetVelocity( playerVel );
            fScaleFactor *= playerVel.Magnitude();
        }
        
        rmt::Vector camHeading;
        GetHeading( &camHeading );

        float currAngle = choreo::GetWorldAngle( camHeading.x, camHeading.z );
        currAngle += ROT_DIR * mouseSense * X_SENSE_MOD * GetAngularSpeed() * GetPitchVelocity();

        player->SetDesiredDir( currAngle );

        rmt::Vector newCamHeading;
        newCamHeading = choreo::DEFAULT_FACING_VECTOR;
        choreo::RotateYVector( currAngle, newCamHeading );

        rmt::Matrix mat;
        mat.Identity();
        mat.FillHeading( newCamHeading, rmt::Vector( 0.0f, 1.0f, 0.0f ) );
        rod.Transform( mat );
    }
    /*
    rmt::Vector targetHeading;
    mTarget->GetHeading( &targetHeading );
    player->GetParentTransform().RotateVector( targetHeading, &targetHeading );

    rmt::Vector camHeading;
    GetHeading( &camHeading );

    // Compensate targetHeading so PC_CAM doesn't rotate behind the character
    // when they're walking towards the camera or walking to the left or
    // walking to the right.
    switch( player->mPCCamFacing )
    {
    case 1: // character facing backwards
        targetHeading *= -1.0f;
    case 0: // character facing in cam's direction
        {
            //Orient according to the target.
            rmt::Matrix mat;
            mat.Identity();
            mat.FillHeading( targetHeading, rmt::Vector( 0.0f, 1.0f, 0.0f ) );
            rod.Transform( mat );
        }
        break;
    case 2: // character somewhere else
        break;
    }
    */

    rod.Add( gOffset );
    rod.Add( targetPos );

    rmt::Vector lookTo = targetPos;
    lookTo.Add( gOffset );

    if ( gDefaultElevation < gBottomElevation )
    {
        if ( gDefaultElevation < 0.0f )
        {
            gDefaultElevation = 0.0f;
        }

        float lookUpMod = ( rmt::PI_BY2 - gDefaultElevation ) / rmt::PI_BY2;
        rmt::Vector lookUpOffset;
        lookUpOffset.Set( 0.0f, gMaxLookUp * lookUpMod, 0.0f  );
        lookTo.Add( lookUpOffset );
    }

    //---------  Goofin' with the FOV

    float zoom = mController->GetValue( SuperCamController::zToggle );

    float FOV = GetFOV();

    if ( GetFlag((Flag)CUT ) )
    {
        FilterFov( zoom, SUPERCAM_DEFAULT_MIN_FOV, SUPERCAM_DEFAULT_MAX_FOV, FOV, mFOVDelta, 1.0f, timeMod );
    }
    else
    {
        FilterFov( zoom, SUPERCAM_DEFAULT_MIN_FOV, SUPERCAM_DEFAULT_MAX_FOV, FOV, mFOVDelta, SUPERCAM_DEFAULT_FOV_LAG, timeMod );
    }

    SetFOV( FOV );

    SetCameraValues( milliseconds, rod, lookTo );  

    mLastPCCamFacing = player->mPCCamFacing;
}

//=============================================================================
// PCCam::UpdateForPhysics
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void PCCam::UpdateForPhysics( unsigned int milliseconds )
{
   /* float nearPlane = GetNearPlane();
    float radius = GetCollisionRadius()*2.0f;

    rmt::Matrix camMat;
    camMat.IdentityTranslation();

    rmt::Vector heading;
    GetHeading( &heading );
    rmt::Vector vup;
    GetCameraUp( &vup );
    camMat.FillHeading( heading, vup );  //I do this because the tCamera may not be the same as I think it is.

    //Test the sides and rotate if there's a problem.
    rmt::Vector camPos;
    GetPosition( &camPos );
    rmt::Vector camTargPos;
    mTarget->GetPosition( &camTargPos );
    rmt::Vector lookFrom( radius, 0.0f, nearPlane );
    lookFrom.Transform( camMat );
    lookFrom.Add( camTargPos );
    lookFrom.Add( gOffset );

    //rmt::Vector lookTo;
    rmt::Vector lookTo( 0.5f, 0.0f, -0.5f );
    lookTo.Transform( camMat );
    lookTo.Add( camPos );
    //GetPosition( &lookTo );*/
    
    rmt::Vector lookFrom;
    mTarget->GetPosition( &lookFrom );
    lookFrom.Add( gOffset );

    rmt::Vector lookTo;
    GetPosition( &lookTo );

    static float NEARCLIP_OFFSET = GetNearPlane()*0.8f;

    // Offset the lookFrom to compensate for the near clip plane. (Thanks for the math help Ian)
    // lookFrom = (Normalize(LT-LF) * NEARCLIP_OFFSET) + LF
    //rmt::Vector offsetVector = (((lookTo-lookFrom)/(lookTo-lookFrom).Magnitude()) * NEARCLIP_OFFSET)+lookFrom;
    //rmt::Vector offsetVector = (((lookFrom-lookTo)/(lookFrom-lookTo).Magnitude()) * NEARCLIP_OFFSET)+lookTo;

    rmt::Vector newPos;
    newPos = lookTo;
    newPos.Add( mGroundOffset );
    

/*
    if ( mNumCollisions )
    {
        //Deal with collisions too;
        unsigned int i;
        unsigned int collCount = 0;
        rmt::Vector collisionOffset( 0.0f, 0.0f, 0.0f );
        for ( i = 0; i < mNumCollisions; ++i )
        {
            if ( mCollisionOffset[ i ].y >= 0.0f )
            {
                collisionOffset.Add( mCollisionOffset[ i ] );
                ++collCount;
            }
        }

        if ( collCount )
        {
            collisionOffset.x /= collCount;
            collisionOffset.y /= collCount;
            collisionOffset.z /= collCount;

            newPos.Add( collisionOffset );
        }
    }
*/
    IntersectionList& iList = GetSuperCamManager()->GetSCC( GetPlayerID() )->GetIntersectionList();

    static float ZOOMIN_FACTOR = 0.5f;
    static float ZOOMOUT_FACTOR = 0.9f;

    rmt::Vector intersection( 0.0f, 0.0f, 0.0f );
    if ( !iList.LineOfSight( lookFrom, lookTo, 0.01f, true ) )
    {
        if ( iList.TestIntersection( lookFrom, lookTo, &intersection, 1.0f ) )
        {
            // Offset the intersection to compensate for the near clip plane.
            // F = D + [( D-S )/ (|| D-S ||)] * NEARCLIP_OFFSET

            rmt::Vector offSettedDest = intersection + ((intersection-newPos)/((intersection-newPos).Magnitude())*(NEARCLIP_OFFSET));
            //offSettedDest = lookFrom-((lookFrom-offSettedDest)/((lookFrom-offSettedDest).Magnitude())*(NEARCLIP_OFFSET*2.0f));

            if( (lookFrom-offSettedDest).Magnitude() < 5.0f )
            {
                offSettedDest.Add( gOffset );    
            }

            newPos = offSettedDest;            

            //newPos = intersection;
            // Now interpolate between the current position and the intersection,
            // to achieve a smoother translation towards the character.
            //newPos.Interpolate( offSettedDest, m_fInterpolationSpeed );

            //interpolation not used at the moment.
            m_fInterpolationSpeed /= ZOOMIN_FACTOR;
            if( m_fInterpolationSpeed >= 1.0f ) m_fInterpolationSpeed = 1.0f;
        }
    }
    else
    {
        if( iList.TestIntersectionAnimPhys( lookFrom, lookTo, &intersection ) )
        {
            // Offset the intersection to compensate for the near clip plane.
            // F = D + [( D-S )/ (|| D-S ||)] * NEARCLIP_OFFSET

            rmt::Vector offSettedDest = intersection + ((intersection-newPos)/((intersection-newPos).Magnitude())*(NEARCLIP_OFFSET));

            newPos = intersection;
            // Now interpolate between the current position and the intersection,
            // to achieve a smoother translation towards the character.
            //newPos.Interpolate( offSettedDest, m_fInterpolationSpeed );

            m_fInterpolationSpeed /= ZOOMIN_FACTOR;
            if( m_fInterpolationSpeed >= 1.0f ) m_fInterpolationSpeed = 1.0f;
        }
        else
        {
            m_fInterpolationSpeed *= ZOOMOUT_FACTOR;
            if( m_fInterpolationSpeed <= 0.1f ) m_fInterpolationSpeed = 0.1f;
        }
    }

//    rmt::Vector newToTarg;
//    newToTarg.Sub( lookFrom, newPos );
//    float mag = newToTarg.Magnitude();
//    mag -= 0.5f; //This is for the size of the character.
//    SetNearPlane( rmt::Clamp( mag, 0.1f, SUPERCAM_NEAR ) );

    SetNearPlane( 0.1f );

    rmt::Vector camTarg;
    GetTarget( &camTarg );
    SetCameraValues( 0, newPos, camTarg );
    //rDebugPrintf( "mNumCollisions = %d \n", mNumCollisions );
//    rDebugPrintf( "m_fInterpolationSpeed = %g \n", m_fInterpolationSpeed );
}

//=============================================================================
// PCCam::SetCollisionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
//
// Return:      void 
//
//=============================================================================
void PCCam::SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
{
    mCollisionOffset = offset;
    mNumCollisions = numCollisions;
    mGroundOffset = groundOffset;
}

//=============================================================================
// PCCam::GetIntersectionRadius
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float PCCam::GetIntersectionRadius() const
{
    return gMaxRodLength;
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
