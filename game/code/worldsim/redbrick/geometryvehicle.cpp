/*===========================================================================
   geometryvehicle.cpp

   created Dec 7, 2001
   by Greg Mayer

   Copyright (c) 2001 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/
#include <camera/supercammanager.h>
#include <contexts/bootupcontext.h>
#include <worldsim/character/characterrenderable.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/wheel.h>
#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>
#include <render/RenderManager/RenderManager.h>

#include <p3d/anim/compositedrawable.hpp>
#include <p3d/billboardobject.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/anim/poseanimation.hpp>
#include <p3d/camera.hpp>
#include <p3d/directionallight.hpp>
#include <p3d/light.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/shadow.hpp>
#include <p3d/view.hpp>
#include <p3d/effects/particlesystem.hpp>
#include <pddi/pddi.hpp>
#include <p3d/utility.hpp>
#include <typeinfo>
#include <p3d/anim/visibilityanimation.hpp>

#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <mission/statepropcollectible.h>
#include <memory/srrmemory.h>

#include <mission/gameplaymanager.h>
#include <render/particles/particlemanager.h>
#include <radmath/util.hpp>
#include <debug/debuginfo.h>
#include <worldsim/redbrick/trafficbodydrawable.h>
#include <worldsim/coins/sparkle.h>

//#define SHADOW_EDITTING

const int TYPICAL_NUMBER_OF_FRAME_CONTROLLERS = 16;

// this is by artist design... the value is between 0 and 255
static const int INCAR_ROOF_ALPHA = 100;
// The velocity at which variable emission particle systems
// reach their peak (13.5 meters/second ~ 50 kph)
static const float PARTICLE_SYSTEM_MAX_VELOCITY = 13.5f;
static const float MAX_Y_OFFSET = 0.25f; // Max distance the shadow will raise to avoid Z Chew.

// Listing of special frame controllers
// This list might be best moved into some sort of script file or data chunk
// rather than being hardcoded if many more get added
// Currently its got framecontrollers for the flame effects on the hoverbike
static const GeometryVehicle::VehicleFrameController FRAME_CONTROLLER_DATA[] = 
{
    // These are reversable framecontrollers
    { "BQG_flame4Shape",        NULL, true, eNone, 0, 1.0f, 0, 0 },
    { "BQG_flame3Shape",        NULL, true, eNone, 0, 1.0f, 0, 0 },
    { "BQG_flame2Shape",        NULL, true, eNone, 1.0f, 0, 0, 0 },
    { "BQG_flame1Shape",        NULL, true, eNone, 1.0f, 0, 0, 0 },
    { "BQG_flame5Shape",        NULL, true, eNone, 1.0f, 0, 0, 0 },
    { "BQG_flame6Shape",        NULL, true, eNone, 1.0f, 0, 0, 0 },
    { "EFX_backfireSystShape",  NULL, false, eBackfire, 0,0,0, 0 },
    { "BQG_backfireflashGroupShape", NULL, false, eBackfire, 0,0,0 },
    { "PTRN_ship",              NULL, false, eNone, 0, 0, 1.0f/25.0f, 0 },
    { "PTRN_redbrick",          NULL, false, eNone, 0, 0, 0.08f, 5.0f }
    //,
//    { "PTRN_ship", NULL, false, eNone, 2.0f,-2.0f,0,0 }
};

const rmt::Vector BARRACUDA_COLLECTIBLE_POS( 0, 0.4f, -3.0f  );
const rmt::Vector JEEP_COLLECTIBLE_POS( -0.237f, 0.216f, -2.243f );
const rmt::Vector DUNE_COLLECTIBLE_ROT( rmt::DegToRadian( -90 ), rmt::DegToRadian( 0 ), rmt::DegToRadian( 0 ));
const rmt::Vector DUNE_COLLECTIBLE_POS( 0, 0.714f, 0);

const float BARREL_RADIUS = 0.351f;

const rmt::Vector WITCH_COLLECTIBLE_POS( 0, 0.4f, -1.31f - BARREL_RADIUS  );
const rmt::Vector COFFIN_COLLECTIBLE_POS( 0, 0.4f, -1.59f - BARREL_RADIUS );
const rmt::Vector HALLO_COLLECTIBLE_POS( 0, 0.4f, -3.467f - BARREL_RADIUS  );


const rmt::Matrix DEFAULT_COLLECTIBLE_TRANSFORM( 0.9219f, -0.3875f, 0.0f, 0.0f, 
                                                 0.3797f, 0.9034f, -0.1994f, 0.0f,
                                                 0.0773f, 0.1838f, 0.9799f, 0.0f,
                                                 0.0f, 0.1270f, -2.6610f, 1.0f );


static const int NUM_FRAME_CONTROLLERS = sizeof( FRAME_CONTROLLER_DATA ) / sizeof( FRAME_CONTROLLER_DATA[0] );
static bool sbDrawVehicle = true;
static float refractiveIndex = 10.0f;

//------------------------------------------------------------------------
GeometryVehicle::GeometryVehicle():
    m_Collectible( NULL ),
    m_EnvRef( 0x40 )
{
    mCompositeDrawable = 0;

    mChassisGeometry = 0;


    mAnim = 0;

    mAnimRevPerSecondBase = 1.0f;

    mVehicleOwner = 0;

    //mRevMult = 0.5f;
    //mRevMult = 0.4f;
    mRevMult = 0.1f;    // good for frink - anyone else need this?

 
    mHoodShader = 0;
    mTrunkShader = 0;
    mDoorPShader = 0;
    mDoorDShader = 0;

    mHoodTextureDam = 0;
    mTrunkTextureDam = 0;
    mDoorPTextureDam = 0;
    mDoorDTextureDam = 0;

    mHoodTextureNorm = 0;
    mTrunkTextureNorm = 0;
    mDoorPTextureNorm = 0;
    mDoorDTextureNorm = 0;


    mChassisShader = 0;
    mChassisTextureNorm = 0;
    mChassisTextureDam = 0;

    mParticleEmitter = 0;
    mVariableEmissionParticleSystem = 0;
    mEngineParticleAttr.mType = ParticleEnum::eNull;
    mLeftWheelParticleAttr.mType = ParticleEnum::eNull;
    mRightWheelParticleAttr.mType = ParticleEnum::eNull;
    mTailPipeParticleAttr.mType = ParticleEnum::eNull;

    mSkidMarkGenerator = 0;

	mSpecialEffect = ParticleEnum::eNull;

    mLastPosition = rmt::Vector (0.0f, 0.0f, 0.0f);
    mCurEnvMapRotation = 0.0f;
    
    mBrakeLightJoints[0] = -1;
    mBrakeLightJoints[1] = -1;
    mBrakeLightJoints[2] = -1;
    mBrakeLightJoints[3] = -1;
    
    mReverseLightJoints[0] = -1;
    mReverseLightJoints[1] = -1;
    mReverseLightJoints[2] = -1;
    mReverseLightJoints[3] = -1;

    mTrafficBodyDrawable = NULL;
    mTrafficDoorDrawable = NULL;

    for( int i = 0; i < 4; ++i )
    {
        mShadowPointAdjustments[ i ][ 0 ] = 0.0f;
        mShadowPointAdjustments[ i ][ 1 ] = 0.0f;
    }

    mFadeAlpha = 255;

    for( int i=0; i<NUM_BRAKELIGHT_BBQS; i++ )
    {
        mBrakeLights[i] = NULL;
    }

#ifdef RAD_WIN32
    mFrinkArc = NULL;
#endif

    mUsingTrafficModel = false;

    /*
    for( int i=0; i<NUM_HEADLIGHT_BBQGS; i++ )
    {
        mHeadLights[i] = NULL;
    }
    */

    mHasGhostGlow = false;
    for( int i=0; i<NUM_GHOSTGLOW_BBQGS; i++ )
    {
        mGhostGlows[i] = NULL;
    }

    mHasNukeGlow = false;
    for( int i=0; i<NUM_NUKEGLOW_BBQGS; i++ )
    {
        mNukeGlows[i] = NULL;
    }

    mBrakeLightsOn = false;
    mBrakeLightScale = 0.0f;
    mHeadLightScale = 0.0f;
    mEnableLights = true; // sets visibility on headlights and brakelights
    mLightsOffDueToDamage = false;

    mRoofShader = NULL;
    mRoofAlpha = 255;
    mRoofTargetAlpha = 255; 

    mRoofOpacShape = NULL;
    mRoofAlphaShape = NULL;

    for( int i = 0; i < MAX_REFRACTION_SHADERS; ++i )
    {
        mRefractionShader[ i ] = NULL;
    }
    // Lets make a collectible transform
    // Hard code it for now. Later use car specific ones from Kevin
    m_CollectibleTransform = DEFAULT_COLLECTIBLE_TRANSFORM;

    mFrameControllers.reserve( TYPICAL_NUMBER_OF_FRAME_CONTROLLERS );

    //
    // Add refractive index to the watcher
    //
    static bool refractiveIndexAdded = false;
    if( !refractiveIndexAdded )
    {
        radDbgWatchAddFloat( &refractiveIndex, "RefractionShader", "RefraciveIndex", 0, 0, -20.0f, 20.0f );
        refractiveIndexAdded = true;
    }
}


//------------------------------------------------------------------------
GeometryVehicle::~GeometryVehicle()
{
#ifdef SHADOW_EDITTING
    for( int i = 0; i < 4; ++i )
    {
        radDbgWatchDelete( &(mShadowPointAdjustments[ i ][ 0 ]) );
        radDbgWatchDelete( &(mShadowPointAdjustments[ i ][ 1 ]) );
    }
#endif
    if(mCompositeDrawable)
    {
        GetRenderManager()->mEntityDeletionList.Add((tRefCounted*&)mCompositeDrawable);
        mCompositeDrawable = 0;
    }

    if(mVariableEmissionParticleSystem)
    {
        GetRenderManager()->mEntityDeletionList.Add((tRefCounted*&)mVariableEmissionParticleSystem);
        mVariableEmissionParticleSystem = 0;
    }

    if(mChassisGeometry)
    {
        mChassisGeometry->Release();
        mChassisGeometry = 0;
    }

    if(mAnim)
    {
        mAnim->Release();
        mAnim = 0;
    }



    for( int i = 0; i < MAX_REFRACTION_SHADERS; ++i )
    {
        tRefCounted::Release( mRefractionShader[ i ] );
        mRefractionShader[ i ] = 0;
    }
    
    // damage state crapola

    if(mHoodShader) mHoodShader->Release();    
    if(mTrunkShader) mTrunkShader->Release();
    if(mDoorPShader) mDoorPShader->Release();
    if(mDoorDShader) mDoorDShader->Release();

    if(mHoodTextureNorm) mHoodTextureNorm->Release();
    if(mTrunkTextureNorm) mTrunkTextureNorm->Release();
    if(mDoorPTextureNorm) mDoorPTextureNorm->Release();
    if(mDoorDTextureNorm) mDoorDTextureNorm->Release();

    if(mHoodTextureDam) mHoodTextureDam->Release();
    if(mTrunkTextureDam) mTrunkTextureDam->Release();
    if(mDoorPTextureDam) mDoorPTextureDam->Release();
    if(mDoorDTextureDam) mDoorDTextureDam->Release();

    
    if(mChassisShader) mChassisShader->Release();
    if(mChassisTextureNorm) mChassisTextureNorm->Release();
    if(mChassisTextureDam) mChassisTextureDam->Release();

    delete mParticleEmitter;
	if ( mSpecialEffect != ParticleEnum::eNull )
	{
		GetParticleManager()->DeleteSystem( mSpecialEffect );
	}

    if ( mVariableEmissionParticleSystem != 0 )
    {
        mVariableEmissionParticleSystem->ReleaseParticles();
        mVariableEmissionParticleSystem = 0;
    }

    if(mSkidMarkGenerator)
    {
        delete mSkidMarkGenerator;
    }

    if( mTrafficBodyDrawable )
    {
        mTrafficBodyDrawable->Release();
        mTrafficBodyDrawable = NULL;
    }

    if( mTrafficDoorDrawable )
    {
        mTrafficDoorDrawable->Release();
        mTrafficDoorDrawable = NULL;
    }

    for( int i=0; i<NUM_BRAKELIGHT_BBQS; i++ )
    {
        if( mBrakeLights[i] )
        {
            mBrakeLights[i]->Release();
            mBrakeLights[i] = NULL;
        }
    }

#ifdef RAD_WIN32
    if( mFrinkArc )
    {
        mFrinkArc->Release();
        mFrinkArc = NULL;
    }
#endif
    /*
    for( int i=0; i<NUM_HEADLIGHT_BBQGS; i++ )
    {
        if( mHeadLights[i] )
        {
            mHeadLights[i]->Release();
            mHeadLights[i] = NULL;
        }
    }
    */
    for( int i=0; i<NUM_GHOSTGLOW_BBQGS; i++ )
    {
        if( mGhostGlows[i] )
        {
            mGhostGlows[i]->Release();
            mGhostGlows[i] = NULL;
        }
    }
    for( int i=0; i<NUM_NUKEGLOW_BBQGS; i++ )
    {
        if( mNukeGlows[i] )
        {
            mNukeGlows[i]->Release();
            mNukeGlows[i] = NULL;
        }
    }
    if( mRoofShader )
    {
        mRoofShader->Release();
        mRoofShader = NULL;
    }
    if( mRoofOpacShape )
    {
        mRoofOpacShape = NULL;
    }
    if( mRoofAlphaShape )
    {
        mRoofAlphaShape = NULL;
    }


    for ( unsigned int fc = 0 ; fc < mFrameControllers.size() ; fc++ )
    {
        mFrameControllers[fc].frameController->Release();
    }

    if ( m_Collectible != NULL )
    {
        m_Collectible->Release();
        m_Collectible = NULL;
    }
}


//------------------------------------------------------------------------
bool GeometryVehicle::Init( const char* name, Vehicle* owner, int num)
{
#ifdef SHADOW_EDITTING
    for( int i = 0; i < 4; ++i )
    {
        mShadowPointAdjustments[ i ][ 0 ] = 0.0f;
        mShadowPointAdjustments[ i ][ 1 ] = 0.0f;
        char text[ 128 ];
        sprintf( text, "%s Shadow point %d X", name, i );
        radDbgWatchAddFloat( &(mShadowPointAdjustments[ i ][ 0 ]), text, "VehicleShadow", 0, 0, -5.0f, 5.0f );
        sprintf( text, "%s Shadow point %d Y", name, i );
        radDbgWatchAddFloat( &(mShadowPointAdjustments[ i ][ 1 ]), text, "VehicleShadow", 0, 0, -5.0f, 5.0f );
    }
#endif
    mVehicleOwner = owner;

    mCurEnvMapRotation = 0.0f;

    return GetArt(name);
}


//=============================================================================
// GeometryVehicle::InitSkidMarks
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::InitSkidMarks()
{
    // called from vehicle Init
    // only for user vehicles

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mSkidMarkGenerator = new(gma)SkidMarkGenerator;

    int i;
    for(i = 0; i < 4; i++)
    {
        // initial value

        rmt::Vector offset = mVehicleOwner->mSuspensionRestPoints[i];
        offset.y -= mVehicleOwner->mWheels[i]->mRadius;

        mSkidMarkGenerator->SetWheelOffset(i, offset);  // need to do this everyframe

        float length = mVehicleOwner->mWheels[i]->mRadius * 0.6f;
        //float width = mVehicleOwner->mWheels[i]->mRadius * 0.5f;    //??
        float width = mVehicleOwner->mWheels[i]->mRadius * 0.8f;    //??

        mSkidMarkGenerator->SetWheelDimensions(i, width, length);

		//void GenerateSkid( int wheel, const SkidData& );

		//void Update();

    }


}


//=============================================================================
// GeometryVehicle::InitParticles
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::InitParticles()
{
MEMTRACK_PUSH_GROUP( "GeometryVehicle" );
    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mParticleEmitter = new(gma)VehicleParticleEmitter;

    rmt::Vector smokeOffset = mVehicleOwner->GetSmokeOffset();

    mParticleEmitter->SetPartLocation(VehicleParticleEmitter::eEngine, smokeOffset);

    rmt::Vector wheelOffset = mVehicleOwner->GetWheel0Offset();
    mParticleEmitter->SetPartLocation(VehicleParticleEmitter::eRightBackTire, wheelOffset);

    wheelOffset = mVehicleOwner->GetWheel1Offset();
    mParticleEmitter->SetPartLocation(VehicleParticleEmitter::eLeftBackTire, wheelOffset);
MEMTRACK_POP_GROUP("GeometryVehicle");
}



//=============================================================================
// GeometryVehicle::GetHeadlightScale
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float GeometryVehicle::GetHeadlightScale()
{
    return mHeadLightScale;
}


//=============================================================================
// GeometryVehicle::SetHeadlightScale
//=============================================================================
// Description: Comment
//
// Parameters:  ( float scale )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetHeadlightScale( float scale )
{
    rAssert( 0.0f <= scale );
    this->mHeadLightScale = scale;
}


//=============================================================================
// GeometryVehicle::AttachCollectible
//=============================================================================
// Description: Comment
//
// Parameters:  ( StatePropCollectible* drawable )
//
// Return:      bool 
//
//=============================================================================
bool GeometryVehicle::AttachCollectible( StatePropCollectible* drawable )
{
    bool wasAttached;
    // We only have one slot for a collectible per vehicle
    // check that this vehicle's slot is currently empty
    if ( m_Collectible == NULL )
    {        
        tRefCounted::Assign( m_Collectible, drawable );
        rmt::Matrix identity;
        identity.Identity();
        drawable->SetTransform( identity );
        m_Collectible->EnableCollisionVolume( false );
        m_Collectible->SetState( 1 );
        m_Collectible->RemoveFromDSG();
        m_Collectible->EnableHudIcon( false );
        m_Collectible->EnableCollisionTesting( false );
        wasAttached = true;
    }
    else
    {
        wasAttached = false;
    }

    return wasAttached;
}


//=============================================================================
// GeometryVehicle::GetAttachedCollectible
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      StatePropCollectible
//
//=============================================================================
StatePropCollectible* GeometryVehicle::GetAttachedCollectible()
{
    return m_Collectible;
}


//=============================================================================
// GeometryVehicle::DetachCollectible
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& velocity, bool explode )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::DetachCollectible( const rmt::Vector& velocity, bool explode )
{
    if ( m_Collectible )
    {
        m_Collectible->EnableHudIcon( false );
        m_Collectible->EnableCollisionVolume( false );
        m_Collectible->SetState( 0 );
        sim::SimState* sim = m_Collectible->GetSimState();
        if ( sim )
        {
            sim->ResetVelocities();
        }
        m_Collectible->AddToDSG();
        // Lets set the transform to have no rotation, but position remains the same
        rmt::Matrix transform;
        transform.Identity();
        transform.FillTranslate( m_CollectibleTransform.Row(3) + mVehicleOwner->GetPosition() );
        m_Collectible->SetTransform(transform);

        m_Collectible->Release();
        if ( explode )
            m_Collectible->Explode();
        
        m_Collectible = NULL;
    }
}


//=============================================================================
// GeometryVehicle::Display
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::Display()
{
BEGIN_PROFILE("GeometryVehicle::Display SetUp")
    rmt::Vector pos = mVehicleOwner->GetPosition ();
    rmt::Vector movement = pos - mLastPosition;
    bool smokeFirst = true; // Used to fix draw order problem with the smoke and a vehicle's windshield.
    tCamera* camera = p3d::context->GetView()->GetCamera();
    rAssert( camera );
    const rmt::Matrix& camTrans = camera->GetCameraToWorldMatrix();

    if( mVehicleOwner->mVehicleType == VT_USER )
    {
        // Set the rotation vector on all the car's shaders.
        // Base the rotation on how far the car has moved in the last known direction,
        //   so that the reflection map moves across the car when the car moves.  -- jdy
        //

        bool fucked = false;
        if ( mLastPosition.MagnitudeSqr() != 0.0f && 
            ( rmt::Fabs( movement.x ) > 1000.0f ||
            rmt::Fabs( movement.y ) > 1000.0f ||
            rmt::Fabs( movement.z ) > 1000.0f ) )
        {
            //TODO: What the hell is causing this?  
            //mVehicleOwner position and facing are fucked.
            //We've moved too far.  Clamp.
            pos = mLastPosition;
            fucked = true;
        }

        if ( !fucked )
        {
            float distance = movement.DotProduct( mVehicleOwner->GetFacing() );
            rAssert( !rmt::IsNan( distance ) );
            if( rmt::IsNan( distance ) )
            {
                distance = 0.0f;
            }
            if( rmt::Fabs( distance ) > 0.01f && rmt::Fabs( distance ) < 50.0f )  // Car must move at least 1 cm for an adjustment to be made
            {
                // Find the normal of the plane defined by the direction of movement and the camera facing.
                //This will be the axis we rotate around.
                // Note that the reflection map is relative to the object so we need to transform everything
                //into the object's space.
                rmt::Matrix carOri = mVehicleOwner->GetTransform();
                carOri.Invert();
                carOri.FillTranslate( rmt::Vector( 0.0f, 0.0f, 0.0f ) );
                rmt::Vector camRelCar;
                carOri.Transform( camTrans.Row( 2 ), &camRelCar );
                rmt::Vector carRelMovement;
                carOri.Transform( movement, &carRelMovement );
                rmt::Vector axis;
                float camMovementFacing = carRelMovement.Dot( camRelCar );
                if( camMovementFacing < 0.0f )
                {
                    axis.CrossProduct(camRelCar, carRelMovement);
                }
                else
                {
                    axis.CrossProduct(carRelMovement, camRelCar);
                }
                // Check if the car is moving towards or away from the camera.
                mCurEnvMapRotation += distance * 0.05f;
                while( mCurEnvMapRotation >= rmt::PI_2 )
                {
                    mCurEnvMapRotation -= rmt::PI_2;
                }
                while( mCurEnvMapRotation < 0.0f )
                {
                    mCurEnvMapRotation += rmt::PI_2;
                }
                float mag;
                float angle;
                CartesianToPolar( axis.x, axis.y, &mag, &angle );

                tShaderVectorBroadcast cbPos( PDDI_SP_ROTVEC, rmt::Vector( mCurEnvMapRotation, 0.0f, -angle ) );
                int count = mCompositeDrawable->GetNumDrawableElement ();
                for (int i = 0; i < count; i++)
                {
                    tCompositeDrawable::DrawableElement* pElement = mCompositeDrawable->GetDrawableElement( i );
                    tDrawable* pDrawable = pElement->GetDrawable();
                    pDrawable->ProcessShaders( cbPos );
                }
            }
            // Remember the position of the car for next time
            //
            mLastPosition = pos;
        }
    }
    else
    {
        mLastPosition = pos;
    }

    rmt::Vector vehicleVelocity;
    mVehicleOwner->GetVelocity( &vehicleVelocity );

    if(mEngineParticleAttr.mType != ParticleEnum::eNull)
    {
        // Figure out which to draw first, the smoke or the car.
        //We do this by transforming the SmokeOffset into world space and then dot product with the
        //the camera's facing vector. If it is towards the camera we draw the smoke after the car.
        //Crude, but quick and hopefully there won't be too many visual artifacts.
        rmt::Matrix facing = mVehicleOwner->mTransform;
        facing.FillTranslate( rmt::Vector( 0.0f, 0.0f, 0.0f ) );
        rmt::Vector smoke = mVehicleOwner->GetSmokeOffset();
        facing.Transform( smoke, &smoke );
        float dot = smoke.DotProduct( camTrans.Row( 2 ) );
        smokeFirst = dot > 0.6f;

        if( ( mEngineParticleAttr.mType == ParticleEnum::eEngineSmokeLight ) ||
            ( mEngineParticleAttr.mType == ParticleEnum::eEngineSmokeMedium ) )
        {
            GetSparkleManager()->AddSmoke( mVehicleOwner->mTransform, mVehicleOwner->GetSmokeOffset(), movement, ( ( 0.4f - mVehicleOwner->GetVehicleLifePercentage( mVehicleOwner->mHitPoints ) ) * 2.5f ) );
        }
        else
        {
            // draw some particles
            //mEngineParticleAttr.mVelocity = vehicleVelocity / 45.0f;
            mParticleEmitter->Generate( VehicleParticleEmitter::eEngine,
                mEngineParticleAttr, mVehicleOwner->mTransform);


            // TODO - HOW ABOUT TRAFFIC - WITH THE ONE-TIME POOF!! DON'T DRAW CONTINUOUSLY
        }
    }
	if ( mSpecialEffect != ParticleEnum::eNull )
	{
		ParticleAttributes attr;
		attr.mType = mSpecialEffect;

		mParticleEmitter->Generate( VehicleParticleEmitter::eSpecialEmitter,
									attr, mVehicleOwner->mTransform);
			
	}
    // If this vehicle has tailpipe particles, generate them, always

    // 31.25 (trails too long)
    //mTailPipeParticleAttr.mVelocity = vehicleVelocity / 29.0f;

    if ( mTailPipeParticleAttr.mType != ParticleEnum::eNull )
    {
        mParticleEmitter->Generate( VehicleParticleEmitter::eRightTailPipe,
            mTailPipeParticleAttr, mVehicleOwner->mTransform );

        mParticleEmitter->Generate( VehicleParticleEmitter::eLeftTailPipe,
            mTailPipeParticleAttr, mVehicleOwner->mTransform );
    }

    if(mLeftWheelParticleAttr.mType != ParticleEnum::eNull)
    {
        mParticleEmitter->Generate( VehicleParticleEmitter::eLeftBackTire,
                                    mLeftWheelParticleAttr, mVehicleOwner->mTransform);       
    }
    if(mRightWheelParticleAttr.mType != ParticleEnum::eNull)
    {
        mParticleEmitter->Generate( VehicleParticleEmitter::eRightBackTire,
                                    mRightWheelParticleAttr, mVehicleOwner->mTransform);       
    }

//    if( mVehicleOwner->mVehicleType == VT_USER )
    {
        // Change the vehicle environment reflection
        //depending on if it is interior or damaged.
        unsigned char blend;
        if( !mVehicleOwner->mInterior )
        {
            blend = m_EnvRef;
        }
        else
        {
            blend = m_EnvRef >> 2; // Quarter value for interior.
        }
        float damage = mVehicleOwner->GetVehicleLifePercentage(mVehicleOwner->mHitPoints);
        damage = rmt::Clamp( ( damage * 0.75f ) + 0.25f, 0.0f, 1.0f );
        blend = (unsigned char)( blend * damage );
        tShaderColourBroadcast envBlend( PDDI_SP_ENVBLEND, tColour( blend, blend, blend ) );
        int count = mCompositeDrawable->GetNumDrawableElement ();
        for (int i = 0; i < count; i++)
        {
            tCompositeDrawable::DrawableElement* pElement = mCompositeDrawable->GetDrawableElement (i);
            tDrawable* pDrawable = pElement->GetDrawable ();
            pDrawable->ProcessShaders( envBlend );
        }
    }

END_PROFILE("GeometryVehicle::Display SetUp")
BEGIN_PROFILE("GeometryVehicle::Display Render")

    if( mUsingTrafficModel )
    {
        if( mTrafficBodyDrawable != NULL )
        {
            if( mFadeAlpha != 255 )
            {
                mTrafficBodyDrawable->mFading = true;
            }
            else
            {
                mTrafficBodyDrawable->mFading = false;
            }
            mTrafficBodyDrawable->mFadeAlpha = mFadeAlpha;
        }
        if( mTrafficDoorDrawable != NULL )
        {
            if( mFadeAlpha != 255 )
            {
                mTrafficDoorDrawable->mFading = true;
            }
            else
            {
                mTrafficDoorDrawable->mFading = false;
            }
            mTrafficDoorDrawable->mFadeAlpha = mFadeAlpha;
        }


        // NOTE:
        // This is a big cause for major grievances. It will force the blend mode of
        // all the shaders of the compositedrawable's elements to be BLEND_ALPHA...
        // bad news for billboards that are BLEND_NONE or BLEND_ADD...
        //
        tShaderIntBroadcast blendModeAlpha( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
        mCompositeDrawable->ProcessShaders( blendModeAlpha );

        tShaderIntBroadcast emissiveAlpha( PDDI_SP_EMISSIVEALPHA, mFadeAlpha );
        mCompositeDrawable->ProcessShaders( emissiveAlpha );
    }


#ifdef RAD_WIN32
    const tName& name = mVehicleOwner->GetNameObject();
    if( name == "frink_v" )
    {
        float topSpeed = mVehicleOwner->GetTopSpeed();
        float percentOfTopSpeed = vehicleVelocity.Magnitude() / topSpeed;
        float refraction = percentOfTopSpeed;
        refraction = rmt::Clamp( refraction, 0.0f, 1.0f );

        int emissiveAlpha = (int)( 255.0f * (1.0f - refraction) );

        tShaderIntBroadcast blendMode( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
        tShaderIntBroadcast emissive( PDDI_SP_EMISSIVEALPHA, emissiveAlpha );
        int count = mCompositeDrawable->GetNumDrawableElement ();
        for (int j = 0; j < count; j++)
        {
            tCompositeDrawable::DrawableElement* pElement = mCompositeDrawable->GetDrawableElement( j );
            tDrawable* pDrawable = pElement->GetDrawable();
            pDrawable->ProcessShaders( blendMode );
            pDrawable->ProcessShaders( emissive );
        }

        // so since we just made EVERY SHADER alpha blend, we need to take
        // care of the BBQGs (that need to be additive blended)
        if( mFrinkArc )
        {
            tColour arcColour;
            for( int j=0; j< NUM_FRINKARC_BBQS; j++ )
            {
                tBillboardQuad* bbq = mFrinkArc->GetQuad(j);
                rAssert( bbq );

                arcColour.Set( 
                    rmt::Clamp( (int)(mOriginalFrinkArcColour[j].Red()*(1.0f - refraction)), 0, 255 ),
                    rmt::Clamp( (int)(mOriginalFrinkArcColour[j].Green()*(1.0f - refraction)), 0, 255 ),
                    rmt::Clamp( (int)(mOriginalFrinkArcColour[j].Blue()*(1.0f - refraction)), 0, 255 ),
                    rmt::Clamp( (int)(mOriginalFrinkArcColour[j].Alpha()*(1.0f - refraction)), 0, 255 )
                );
                bbq->SetColour( arcColour );
            }
            mFrinkArc->GetShader()->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
        }
    }
#endif





    tColour brakecolour;

    // if not turning on brakelights, we need to take into account the daytime 
    // running lights settings
    float dayReduce = 2.5f; // by default, if braking is on, we magnify this value
    if( !mBrakeLightsOn )
    {
        dayReduce = mBrakeLightScale;
    }
    float fadeFactor = mFadeAlpha / 255.0f;
BEGIN_PROFILE("GeometryVehicle::Breaklights")
    for( int i=0; i<NUM_BRAKELIGHT_BBQS; i++ )
    {
        if( mBrakeLights[i] != NULL )
        {
            brakecolour.Set(
                rmt::Clamp( (int)(mOriginalBrakeLightColours[i].Red()*fadeFactor*dayReduce), 0, 255 ),
                rmt::Clamp( (int)(mOriginalBrakeLightColours[i].Green()*fadeFactor*dayReduce), 0, 255 ),
                rmt::Clamp( (int)(mOriginalBrakeLightColours[i].Blue()*fadeFactor*dayReduce), 0, 255 ),
                rmt::Clamp( (int)(mOriginalBrakeLightColours[i].Alpha()*fadeFactor*dayReduce), 0, 255 )
            );
            for( int j=0; j<mBrakeLights[i]->GetNumQuads(); j++ )
            {
                tBillboardQuad* bbq = mBrakeLights[i]->GetQuad( j );
                rAssert( bbq != NULL );
                bbq->SetColour( brakecolour );
                bbq->SetVisibility( mEnableLights );
            }

            // ProcessShaders screwed us by causing us to lose the additive blend for
            // brakelights... We set it back.
            mBrakeLights[i]->GetShader()->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
        }
    }
END_PROFILE("GeometryVehicle::Breaklights")

BEGIN_PROFILE("GeometryVehicle::Headlights")
    tColour headlightcolour;
    int count = 0;
    for( int i=0; i<VehicleCentral::NUM_HEADLIGHT_BBQGS; i++ )
    {
        if( GetVehicleCentral()->mHeadLights[i] != NULL )
        {
            for( int j=0; j<GetVehicleCentral()->mHeadLights[i]->GetNumQuads(); j++ )
            {
                headlightcolour.Set(
                    rmt::Clamp( (int)(GetVehicleCentral()->mOriginalHeadLightColours[count].Red()*fadeFactor*mHeadLightScale), 0, 255 ),
                    rmt::Clamp( (int)(GetVehicleCentral()->mOriginalHeadLightColours[count].Green()*fadeFactor*mHeadLightScale), 0, 255 ),
                    rmt::Clamp( (int)(GetVehicleCentral()->mOriginalHeadLightColours[count].Blue()*fadeFactor*mHeadLightScale), 0, 255 ),
                    rmt::Clamp( (int)(GetVehicleCentral()->mOriginalHeadLightColours[count].Alpha()*fadeFactor*mHeadLightScale), 0, 255 )
                );
                tBillboardQuad* bbq = GetVehicleCentral()->mHeadLights[i]->GetQuad( j );
                rAssert( bbq != NULL );
                bbq->SetColour( headlightcolour );
                bbq->SetVisibility( mEnableLights );
                count++;
            }

            // ProcessShaders screwed us by causing us to lose the additive blend
            // We set it back.
            GetVehicleCentral()->mHeadLights[i]->GetShader()->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
        }
    }
END_PROFILE("GeometryVehicle::Headlights")

BEGIN_PROFILE("GeometryVehicle::Ghost")
    // this means we're the ghost pirate ship... The glow must be ADDITIVE as well...
    if( mHasGhostGlow ) 
    {
        count = 0;
        tColour glowcolour;
        for( int i=0; i<NUM_GHOSTGLOW_BBQGS; i++ )
        {
            if( mGhostGlows[i] != NULL )
            {
                for( int j=0; j<mGhostGlows[i]->GetNumQuads(); j++ )
                {
                    glowcolour.Set(
                        rmt::Clamp( (int)(mOriginalGhostGlowColours[count].Red()*fadeFactor), 0, 255 ),
                        rmt::Clamp( (int)(mOriginalGhostGlowColours[count].Green()*fadeFactor), 0, 255 ),
                        rmt::Clamp( (int)(mOriginalGhostGlowColours[count].Blue()*fadeFactor), 0, 255 ),
                        rmt::Clamp( (int)(mOriginalGhostGlowColours[count].Alpha()*fadeFactor), 0, 255 )
                    );
                    tBillboardQuad* bbq = mGhostGlows[i]->GetQuad( j );
                    rAssert( bbq != NULL );
                    bbq->SetColour( glowcolour );
                    count++;
                }

                // ProcessShaders screwed us by causing us to lose the additive blend
                // We set it back.
                mGhostGlows[i]->GetShader()->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
            }
        }
    }
END_PROFILE("GeometryVehicle::Ghost")

    // this means we're the ghost pirate ship... The glow must be ADDITIVE as well...
    if( mHasNukeGlow ) 
    {
        count = 0;
        tColour glowcolour;
        for( int i=0; i<NUM_NUKEGLOW_BBQGS; i++ )
        {
            if( mNukeGlows[i] != NULL )
            {
                for( int j=0; j<mNukeGlows[i]->GetNumQuads(); j++ )
                {
                    glowcolour.Set(
                        rmt::Clamp( (int)(mOriginalNukeGlowColours[count].Red()*fadeFactor), 0, 255 ),
                        rmt::Clamp( (int)(mOriginalNukeGlowColours[count].Green()*fadeFactor), 0, 255 ),
                        rmt::Clamp( (int)(mOriginalNukeGlowColours[count].Blue()*fadeFactor), 0, 255 ),
                        rmt::Clamp( (int)(mOriginalNukeGlowColours[count].Alpha()*fadeFactor), 0, 255 )
                    );
                    tBillboardQuad* bbq = mNukeGlows[i]->GetQuad( j );
                    rAssert( bbq != NULL );
                    bbq->SetColour( glowcolour );
                    count++;
                }

                // ProcessShaders screwed us by causing us to lose the additive blend
                // We set it back.
                mNukeGlows[i]->GetShader()->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
            }
        }
    }

    // fade roof...
    if( mRoofShader )
    {
        if( mRoofAlpha > mRoofTargetAlpha )
        {
            mRoofAlpha--;
        }
        else if( mRoofAlpha < mRoofTargetAlpha )
        {
            mRoofAlpha++;
        }
        //mRoofShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
        mRoofShader->SetInt( PDDI_SP_EMISSIVEALPHA, mRoofAlpha );
        if( mRoofOpacShape && mRoofAlphaShape )
        {
            if( mRoofAlpha == 255 )
            {
                mRoofOpacShape->SetVisibility( true );
                mRoofAlphaShape->SetVisibility( false );
            }
            else
            {
                mRoofOpacShape->SetVisibility( false );
                mRoofAlphaShape->SetVisibility( true );
            }
        }
    }

BEGIN_PROFILE("GeometryVehicle::CompDraw->Disp")
    if( smokeFirst )
    {
        GetSparkleManager()->Render( Sparkle::SRM_SortedOnly );
    }
    if( sbDrawVehicle )
    {
        mCompositeDrawable->Display();
    }
    if( !smokeFirst )
    {
        GetSparkleManager()->Render( Sparkle::SRM_SortedOnly );
    }
    if( m_Collectible )
    {
        rmt::Matrix transform = mVehicleOwner->GetTransform();

        p3d::stack->PushMultiply( transform );
        p3d::stack->PushMultiply( m_CollectibleTransform );
        m_Collectible->Display();
        p3d::stack->Pop();
        p3d::stack->Pop();
    }

END_PROFILE("GeometryVehicle::CompDraw->Disp")
END_PROFILE("GeometryVehicle::Display Render")
}


//=============================================================================
// GeometryVehicle::DisplaySkids
//=============================================================================
// Description: Comment
//
// Parameters:  void GeometryVehicle::DisplaySkids(int wheel

//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetSkidValues(int wheel, float intensity, rmt::Vector& groundPlaneNormal, eTerrainType terrainType )
{
    // if this got called, intensity is > 0.0

    if(mSkidMarkGenerator)// && mDrawSkids)  // test here though this should only be getting called for vehicles that have it... ie. VT_USER
    {
        /*
	    struct SkidData
		{
			// Direction that the vehicle is moving
			rmt::Vector velocityDirection;
			// Orientation and position of the vehicle
			rmt::Matrix transform;			
			// Other data?????
			float intensity; // 1 most intense, 0 nonexistent skid
		};
        */

        rmt::Vector offset = mVehicleOwner->mSuspensionRestPoints[wheel];
        offset.y -= mVehicleOwner->mWheels[wheel]->mRadius;
        offset.y += mVehicleOwner->mWheels[wheel]->mYOffset; // hmmm....

        //if(mVehicleOwner->mWheels[wheel]->mYOffset != 0.0f)
        //{
        //    int stophere = 1;
        //}

        mSkidMarkGenerator->SetWheelOffset(wheel, offset);  // need to do this everyframe

        SkidMarkGenerator::SkidData sd;
        sd.intensity = intensity;
        sd.transform = mVehicleOwner->mTransform;
        sd.terrainType = terrainType;

        sd.groundPlaneNormal = groundPlaneNormal;


        rmt::Vector velocityDir = mVehicleOwner->mVelocityCM;
        velocityDir.NormalizeSafe();

        // TODO
        // Michael - would this blow up if I passed in 0,0,0 ?
        sd.velocityDirection = velocityDir;

		mSkidMarkGenerator->GenerateSkid(wheel, sd);

    }

}

//=============================================================================
// GeometryVehicle::DisplaySkids
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::UpdateSkids()
{
    if(mSkidMarkGenerator)
    {
        mSkidMarkGenerator->Update();
    }
}


//////////////////////////////////////////////////////////////////////////
int GeometryVehicle::CastsShadow()
{
    return 2;
}
//------------------------------------------------------------------------
void GeometryVehicle::DisplayShadow( BlobShadowParams* BlobParams )
{
    BEGIN_PROFILE("GeometryVehicle::DisplayShadow")
    rAssert(BlobParams);
    tColour OutsideColour, insideColour;

    // Hack for brightly colored shadows for special game mode
    rmt::Vector camPos;
	tCamera* camera = p3d::context->GetView()->GetCamera();
	rAssert(camera);
	camera->GetWorldPosition( &camPos );
	camPos.Sub( BlobParams->GroundPos );
	float yOffset = 0.0f; // Move the shadow up a bit with distance to avoid Z chew.
    if( ::GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
    {
        rAssert( mVehicleOwner );
        Avatar* avatar = ::GetAvatarManager()->GetAvatarForVehicle( mVehicleOwner );

        OutsideColour.Set( 0, 0, 0, 0 ); // black outline
        insideColour.Set( 0, 0, 0 ); // default black if no Avatar
        if( avatar )
        {
            int id = avatar->GetPlayerId();

            if ( this->mVehicleOwner->mNumTurbos > 0 )
            {
                insideColour = ::GetGameplayManager()->GetControllerColour( id );
            }
            else
            {
                insideColour = tColour( 0, 0, 0 );
            }
        }
    }
    else
    {
		// Camera culling.
        float camDirDot = camPos.DotProduct( camera->GetCameraToWorldMatrix().Row( 2 ) );
        // Note that the camPos vector is towards the camera so the camDirDot will be negative when the camera is facing
        //the object. So if it's positive then we can assume it's behind the camera.
        if( camDirDot > 0.0f )
        {
		    END_PROFILE("GeometryVehicle::DisplayShadow")
            return;
        }

   		// Blobby shadow for vehicle.
        OutsideColour.Set( 255, 255, 255, 255 );
        const int Inside = 128;
        
        float fadeFactor = 1.0f;
        if( mVehicleOwner->mVehicleType == VT_TRAFFIC && 
            mFadeAlpha != 255 )
        {
            fadeFactor = 1.0f - (mFadeAlpha/255.0f);
        }
        else
        {
            fadeFactor = 1.0f - BlobParams->ShadowAlpha;
        }
		// DistanceAlpha: over 30 is 1, under 10 is 0, linear between.
		float distanceAlpha = rmt::Clamp((camPos.Magnitude() - 10.0f) * 0.05f, 0.0f, 1.0f);

        fadeFactor *= 1.0f - distanceAlpha;

        int c = rmt::Clamp( int( Inside + ( 255 - Inside ) * fadeFactor ), 0, 255 );
        if( c == 255)
        {
            return;
        }
        insideColour.Set( c, c, c, c );
		yOffset = MAX_Y_OFFSET * distanceAlpha;
    }
	const int NumPoints = 6;
	const float BlobLength = 2.2f;
	const float BlobWidth = 1.2f;
    const float BlobCant = 0.2f;
	const float BlobFade = 0.4f;
	static float Points[ NumPoints ][ 2 ];
    static float Fades[ NumPoints ][ 2 ];

    static bool DoOnce = true;
    if( DoOnce )
    {
        DoOnce = false;
		/* Here are the points we'll use for the car:
			
		....----0
		.         1
		.         |
		.         |
		.    +    2
		.         3
		.         |
		.         4
		....----5
			We'll mirror the blob along the Z axis.
            The shadow point adjusts work like this:
            0 - affects points 0 and 1, the top corner.
            1 - affects point 2, the position for the top division.
            2 - affects point 3, the position for the bottom division.
            3 - affects point 4 and 5, the bottom corner.
		*/
		Points[ 0 ][ 0 ] = BlobWidth - BlobCant;
		Points[ 0 ][ 1 ] = BlobLength;
		Points[ 1 ][ 0 ] = BlobWidth;
		Points[ 1 ][ 1 ] = BlobLength - BlobCant;
		Points[ 2 ][ 0 ] = BlobWidth;
		Points[ 2 ][ 1 ] = BlobCant;
		Points[ 3 ][ 0 ] = BlobWidth;
		Points[ 3 ][ 1 ] = -BlobCant;
		Points[ 4 ][ 0 ] = BlobWidth;
		Points[ 4 ][ 1 ] = -BlobLength + BlobCant;
		Points[ 5 ][ 0 ] = BlobWidth - BlobCant;
		Points[ 5 ][ 1 ] = -BlobLength;
        Fades[ 0 ][ 0 ] = 0.5f * BlobFade; // Sin( 30 )
        Fades[ 0 ][ 1 ] = 0.866f * BlobFade; // Cos( 30 )
        Fades[ 1 ][ 0 ] = 0.866f * BlobFade; // Sin( 60 )
        Fades[ 1 ][ 1 ] = 0.5f * BlobFade; // Cos( 60 )
        Fades[ 2 ][ 0 ] = 0.866f * BlobFade; // Sin( 60 )
        Fades[ 2 ][ 1 ] = 0.5f * BlobFade; // Cos( 60 )
        Fades[ 3 ][ 0 ] = 0.866f * BlobFade; // Sin( 60 )
        Fades[ 3 ][ 1 ] = 0.5f * BlobFade; // Cos( 60 )
        Fades[ 4 ][ 0 ] = 0.866f * BlobFade; // Sin( 120 )
        Fades[ 4 ][ 1 ] = -0.5f * BlobFade; // Cos( 120 )
        Fades[ 5 ][ 0 ] = 0.5f * BlobFade; // Sin( 150 )
        Fades[ 5 ][ 1 ] = -0.866f * BlobFade; // Cos( 150 )
    }

	camPos.Normalize();
	rmt::Matrix transform;
	transform.Identity();
	transform.FillTranslate( BlobParams->GroundPos );
    transform.FillHeading( BlobParams->GroundNormal, BlobParams->ShadowFacing );
	transform.Row( 3 ).Add( camPos );
	transform.Row(3).y += yOffset;
	p3d::stack->PushMultiply( transform );

    pddiShader* blobShader = BootupContext::GetInstance()->GetSharedShader();

    if( ::GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
    {
        blobShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_NONE );
    }
    else
    {
        blobShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_MODULATE );
    }
    blobShader->SetInt( PDDI_SP_ISLIT, 0 );
    blobShader->SetInt( PDDI_SP_ALPHATEST, 0 );
    blobShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_GOURAUD );
    pddiPrimStream* blob = p3d::pddi->BeginPrims( blobShader, PDDI_PRIM_TRIANGLES, PDDI_V_C, 6 * NumPoints );
	for( int i = 0; i < NumPoints * 2; ++i )
	{
        int index = i;
        int nextIndex = ( i + 1 ) % ( NumPoints * 2 );
        int mirrorX = int( ( index * ( 1.0f / NumPoints ) ) );
        int nextMirrorX = int( ( nextIndex * ( 1.0f / NumPoints ) ) );
        index %= NumPoints;
        nextIndex %= NumPoints;
        index = rmt::Abs( ( mirrorX * 5 ) - index );
        nextIndex = rmt::Abs( ( nextMirrorX * 5 ) - nextIndex );
        mirrorX = 1 - ( mirrorX * 2 );
        nextMirrorX = 1 - ( nextMirrorX * 2 );
        int adjust = rmt::Clamp( index - 1, 0, NumPoints - 3 );
        int nextAdjust = rmt::Clamp( nextIndex - 1, 0, NumPoints - 3 );

        float x, y;
        blob->Colour( insideColour );
        blob->Coord( 0.0f, 0.0f, 0.0f );
        x = ( Points[ nextIndex ][ 0 ] + mShadowPointAdjustments[ nextAdjust ][ 0 ] ) * BlobParams->ShadowScale;
        y = ( Points[ nextIndex ][ 1 ] + mShadowPointAdjustments[ nextAdjust ][ 1 ] ) * BlobParams->ShadowScale;
		blob->Colour( insideColour );
        blob->Coord( x * nextMirrorX, y, 0.0f );
        x = ( Points[ index ][ 0 ] + mShadowPointAdjustments[ adjust ][ 0 ] ) * BlobParams->ShadowScale;
        y = ( Points[ index ][ 1 ] + mShadowPointAdjustments[ adjust ][ 1 ] ) * BlobParams->ShadowScale;
        blob->Colour( insideColour );
        blob->Coord( x * mirrorX, y, 0.0f );
	}
    p3d::pddi->EndPrims( blob );
    // How the soft edge.
    blob = p3d::pddi->BeginPrims( blobShader, PDDI_PRIM_TRISTRIP, PDDI_V_C, ( ( 2 * NumPoints ) + 1 ) * 2 );
    for( int i = 0; i <= NumPoints * 2; ++i )
    {
        int index = i;
        int mirrorX = int( ( index * ( 1.0f / NumPoints ) ) ) % 2;
        index %= NumPoints;
        index = rmt::Abs( ( mirrorX * 5 ) - index );
        mirrorX = 1 - ( mirrorX * 2 );
        int adjust = rmt::Clamp( index - 1, 0, NumPoints - 3 );

        float inX, inY;
        float outX, outY;
        inX = ( Points[ index ][ 0 ] + mShadowPointAdjustments[ adjust ][ 0 ] ) * BlobParams->ShadowScale;
        inY = ( Points[ index ][ 1 ] + mShadowPointAdjustments[ adjust ][ 1 ] ) * BlobParams->ShadowScale;
        inX *= mirrorX;
        outX = inX + ( Fades[ index ][ 0 ] * mirrorX * BlobParams->ShadowScale );
        outY = inY + ( Fades[ index ][ 1 ] * BlobParams->ShadowScale );
        blob->Colour( OutsideColour );
        blob->Coord( outX, outY, 0.0f );
        blob->Colour( insideColour );
        blob->Coord( inX, inY, 0.0f );
    }
    p3d::pddi->EndPrims( blob );
	p3d::stack->Pop();
    END_PROFILE("GeometryVehicle::DisplayShadow")
}

//=============================================================================
// GeometryVehicle::DisplayLights
//=============================================================================
// Description: Comment
//
// Parameters:  rmt::Vector& GroundPos - Position of car.
//              rmt::Vector& GroundNormal - Ground normal at car position.
//              rmt::Vector& GroundUp - Up direction at ground position
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::DisplayLights( const HeadLightParams& LightParams )
{
    BEGIN_PROFILE("GeometryVehicle::DisplayLights")
    const int NumPoints = 20;
    const float LightXSize = 1.0f;
    const float LightZSize = 0.125f;
    const float FadeSize = 0.3f;
    const float LightLength = NumPoints * LightZSize;
    static bool DoOnce = true;
	static float PointsX[ NumPoints ];
    static tColour Colours[ NumPoints ];
    static float FadePoints[ NumPoints ][ 2 ];
    const tColour Colour( 65, 70, 46 );
    const tColour Black( 0, 0, 0 );

    if( DoOnce )
    {
        DoOnce = false;
        // The light cone is made of of two tri strips one is the inner bright parabula
        // which fades out with distance, and the other is the soft edge around it.
        for( int i = 0; i < NumPoints; ++i )
        {
            float z = ( i + 1 ) * LightZSize;
            float x = rmt::Sqrt( ( i + 1 ) * 0.5f );
            PointsX[ i ] = x;
            z -= LightLength;
            float mag = rmt::Sqrt( ( x * x ) + ( z * z ) );
            mag = ( ( mag != 0.0f ) ? 1.0f / mag : 0.0f ) * FadeSize;
            x *= mag;
            z *= mag;
            FadePoints[ i ][ 0 ] = x;
            FadePoints[ i ][ 1 ] = z;
            float colourScale = ( 1.0f - ( (float)i / (float)NumPoints ) );
            int red = rmt::Clamp( int( Colour.Red() * colourScale ), 0, 255 );
            int green = rmt::Clamp( int( Colour.Green() * colourScale ), 0, 255 );
            int blue = rmt::Clamp( int( Colour.Blue() * colourScale ), 0, 255 );
            Colours[ i ].Set( red, green, blue );
        }
        // Now calculate the offset for the fade points.
    }

    float xScale = 1.0f;
    float zScale = 1.0f;
    float scale = 1.0f;

    scale = LightParams.LightReach;
    float offset = ( 1.0f - LightParams.LightReach ) * LightLength;

    rmt::Vector camPos;
    rmt::Vector groundPos = LightParams.GroundPos;
    groundPos.ScaleAdd( offset, LightParams.VehicleHeading );
	tCamera* camera = p3d::context->GetView()->GetCamera();
    rAssert( camera );
    camera->GetWorldPosition( &camPos );
	camPos.Sub( groundPos );
    float cameraToLight = camPos.Magnitude();
	camPos.Normalize();
	rmt::Matrix toCamera;
	toCamera.Identity();
	toCamera.FillTranslate( camPos );
	rmt::Matrix transform;
	transform.Identity();
	transform.FillTranslate( groundPos );
    transform.FillHeading( LightParams.GroundNormal, LightParams.VehicleHeading );
	transform.Mult( toCamera );
	p3d::stack->PushMultiply( transform );

    float dist = LightParams.VehicleHeading.Dot( LightParams.GroundNormal );
    rmt::Vector groundByVehicle( LightParams.GroundNormal );
    if( dist != 0.0f )
    {
        groundByVehicle.ScaleAdd( -dist, LightParams.VehicleHeading );
        groundByVehicle.Normalize();
    }
    float vehicleRoll = groundByVehicle.Dot( LightParams.VehicleUp );
    vehicleRoll *= vehicleRoll;
#ifdef RAD_DEBUG
    if( this->mVehicleOwner->mVehicleType != VT_TRAFFIC )
    {
        char text[128];
        sprintf( text, "Ground to vehicle ratio: %.4f", vehicleRoll );
        DebugInfo::GetInstance()->Push( "Vehicle lights" );
        DebugInfo::GetInstance()->AddScreenText( text );
        DebugInfo::GetInstance()->Pop();
    }
#endif

    xScale *= scale * LightXSize * vehicleRoll;
    zScale *= scale * LightZSize;

    pddiShader* lightShader = BootupContext::GetInstance()->GetSharedShader();
    lightShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
    lightShader->SetInt( PDDI_SP_ISLIT, 0 );
    lightShader->SetInt( PDDI_SP_ALPHATEST, 0 );
    lightShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_GOURAUD );

    // Center light.
    pddiPrimStream* light = p3d::pddi->BeginPrims( lightShader, PDDI_PRIM_TRISTRIP, PDDI_V_C, ( NumPoints << 1 ) + 1 );
    light->Colour( Colour );
    light->Coord( 0.0f, 0.0f, 0.0f );
	for( int i = 0; i < NumPoints; ++i )
	{
        float inX = PointsX[ i ] * xScale;
        float inZ = ( i + 1 ) * zScale;
		light->Colour( Colours[ i ] );
		light->Coord( inX, inZ, 0.0f );
        light->Colour( Colours[ i ] );
        light->Coord( -inX, inZ, 0.0f );
	}
    p3d::pddi->EndPrims( light );
    // Fade on the negative side.
    pddiPrimStream* nFade = p3d::pddi->BeginPrims( lightShader, PDDI_PRIM_TRISTRIP, PDDI_V_C, ( NumPoints + 1 ) << 1 );
    nFade->Colour( Black );
    nFade->Coord( 0.0f, -FadeSize, 0.0f );
    nFade->Colour( Colour);
    nFade->Coord( 0.0f, 0.0f, 0.0f );
    for( int i = 0; i < NumPoints; ++i )
    {
        float inX = -PointsX[ i ] * xScale;
        float inZ = ( i + 1 ) * zScale;
        float outX = inX - FadePoints[ i ][ 0 ];
        float outZ = inZ + FadePoints[ i ][ 1 ];
        nFade->Colour( Black );
        nFade->Coord( outX, outZ, 0.0f );
        nFade->Colour( Colours[ i ] );
        nFade->Coord( inX, inZ, 0.0f );
    }
    p3d::pddi->EndPrims( nFade );

    // Fade on the positive side.
    pddiPrimStream* pFade = p3d::pddi->BeginPrims( lightShader, PDDI_PRIM_TRISTRIP, PDDI_V_C, ( NumPoints + 1 ) << 1 );
    pFade->Colour( Colour);
    pFade->Coord( 0.0f, 0.0f, 0.0f );
    pFade->Colour( Black );
    pFade->Coord( 0.0f, -FadeSize, 0.0f );
    for( int i = 0; i < NumPoints; ++i )
    {
        float inX = PointsX[ i ] * xScale;
        float inZ = ( i + 1 ) * zScale;
        float outX = inX + FadePoints[ i ][ 0 ];
        float outZ = inZ + FadePoints[ i ][ 1 ];
        pFade->Colour( Colours[ i ] );
        pFade->Coord( inX, inZ, 0.0f );
        pFade->Colour( Black );
        pFade->Coord( outX, outZ, 0.0f );
    }
    p3d::pddi->EndPrims( pFade );

	p3d::stack->Pop();
    END_PROFILE("GeometryVehicle::DisplayLights")
}

//=============================================================================
// GeometryVehicle::FindAndTurnOffWheels
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindAndTurnOffWheels()
{
    int i;
    for(i = 0; i < 4; i++)
    {
        tPose* p3dPose = mCompositeDrawable->GetPose();

        char buffy[128];
        sprintf(buffy, "w%d", i);
        int jointIndex = p3dPose->FindJointIndex(buffy);
        
        int j;
        for(j = 0; j < mCompositeDrawable->GetNumDrawableElement(); j++)
        {
            tCompositeDrawable::DrawablePropElement* prop = (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(j));
            if(prop && prop->GetPoseIndex() == jointIndex)
            {
                prop->SetVisibility(false);
            }
        }       

    }
}


//=============================================================================
// GeometryVehicle::FindAndTurnOffFrontWheelsOnly
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindAndTurnOffFrontWheelsOnly()
{
    int i;
    for(i = 2; i < 4; i++)
    {
        tPose* p3dPose = mCompositeDrawable->GetPose();

        char buffy[128];
        sprintf(buffy, "w%d", i);
        int jointIndex = p3dPose->FindJointIndex(buffy);
        
        int j;
        for(j = 0; j < mCompositeDrawable->GetNumDrawableElement(); j++)
        {
            tCompositeDrawable::DrawablePropElement* prop = (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(j));
            if(prop && prop->GetPoseIndex() == jointIndex)
            {
                prop->SetVisibility(false);
            }
        }       

    }
}


//=============================================================================
// GeometryVehicle::FindHeadLightBillboardJoints
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindHeadLightBillboardJoints()
{
    // grab the pose
    tPose* p3dPose = mCompositeDrawable->GetPose();

    // grab the headlight bbqgs from common section

    // put in left headlight
    char buffy[128];
    sprintf( buffy, "hll" );
    int left = p3dPose->FindJointIndex( buffy );
    if( left >= 0 )
    {
        for( int i=0; i<VehicleCentral::NUM_HEADLIGHT_BBQGS; i++ )
        {
            rAssert( GetVehicleCentral()->mHeadLights[i] );
            mCompositeDrawable->AddProp( GetVehicleCentral()->mHeadLights[i], left );
        }
    }

    // now the right side
    sprintf( buffy, "hlr" );
    int right = p3dPose->FindJointIndex( buffy );
    if( right >= 0 )
    {
        for( int i=0; i<VehicleCentral::NUM_HEADLIGHT_BBQGS; i++ )
        {
            rAssert( GetVehicleCentral()->mHeadLights[i] );
            mCompositeDrawable->AddProp( GetVehicleCentral()->mHeadLights[i], right );
        }
    }

}
//=============================================================================
// GeometryVehicle::FindBrakeLightBillboardJoints
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindBrakeLightBillboardJoints()
{
    int i;
    for(i = 1; i < 5; i++)  // 'cause Kevin starts counting at 1, not 0
    {        
        tPose* p3dPose = mCompositeDrawable->GetPose();

        char buffy[128];
        sprintf(buffy, "brake%d", i);
        int jointIndex = p3dPose->FindJointIndex(buffy);
        
        int j;
        for(j = 0; j < mCompositeDrawable->GetNumDrawableElement(); j++)
        {
            tCompositeDrawable::DrawablePropElement* prop = (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(j));
            if(prop && prop->GetPoseIndex() == jointIndex)
            {
                //mBrakeLights[i - 1] = (tBillboardQuadGroup*) prop->GetDrawable();
                tBillboardQuadGroup* bbqg = dynamic_cast<tBillboardQuadGroup*>( prop->GetDrawable() );
                if( bbqg != NULL )
                {
                    tRefCounted::Assign( mBrakeLights[i - 1], bbqg );

                    // the structure for storing the original colors only supports 1 quad per quadgroup
                    rAssert( mBrakeLights[i-1]->GetNumQuads() == 1 );
                    tBillboardQuad* quad = mBrakeLights[i-1]->GetQuad( 0 );
                    mOriginalBrakeLightColours[i-1] = quad->GetColour();
                    mBrakeLightJoints[i - 1] = jointIndex;
                }
            }
        }    
        
    }
    
    for(i = 1; i < 5; i++)  // 'cause Kevin starts counting at 1, not 0
    {        
        tPose* p3dPose = mCompositeDrawable->GetPose();

        char buffy[128];
        sprintf(buffy, "rev%d", i);
        int jointIndex = p3dPose->FindJointIndex(buffy);
        
        int j;
        for(j = 0; j < mCompositeDrawable->GetNumDrawableElement(); j++)
        {
            tCompositeDrawable::DrawablePropElement* prop = (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(j));
            if(prop && prop->GetPoseIndex() == jointIndex)
            {
                prop->SetVisibility(false);
                mReverseLightJoints[i - 1] = jointIndex;
            }
        }    
        
    }
}


//=============================================================================
// GeometryVehicle::FindGhostGlowBillboards
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* model )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindGhostGlowBillboards( const char* model )
{
    int count = 0;
    for( int i = 0; i<NUM_GHOSTGLOW_BBQGS; i++ )
    {
        char name[64];
        sprintf( name, "%sGlow%dShape", model, i+1 );
        tUID test = tEntity::MakeUID( name );

        for( int j=0; j<mCompositeDrawable->GetNumDrawableElement(); j++ )
        {
            tCompositeDrawable::DrawablePropElement* prop = 
                (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(j));

            if( prop && prop->GetUID() == test )
            {
                tBillboardQuadGroup* bbqg = dynamic_cast<tBillboardQuadGroup*>( prop->GetDrawable() );
                if( bbqg != NULL )
                {
                    tRefCounted::Assign( mGhostGlows[i], bbqg );

                    for( int j=0; j<mGhostGlows[i]->GetNumQuads(); j++ )
                    {
                        tBillboardQuad* quad = mGhostGlows[i]->GetQuad( j );
                        rAssert( quad );
                        mOriginalGhostGlowColours[count] = quad->GetColour();
                        count++;
                    }
                }
            }
        }
    }    
}    


//=============================================================================
// GeometryVehicle::FindNukeGlowBillboards
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* model )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindNukeGlowBillboards( const char* model )
{
    int count = 0;
    for( int i = 0; i<NUM_NUKEGLOW_BBQGS; i++ )
    {
        char name[64];
        sprintf( name, "nucGlowGroupShape" );
        tUID test = tEntity::MakeUID( name );

        for( int j=0; j<mCompositeDrawable->GetNumDrawableElement(); j++ )
        {
            tCompositeDrawable::DrawablePropElement* prop = 
                (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(j));

            if( prop && prop->GetUID() == test )
            {
                tBillboardQuadGroup* bbqg = dynamic_cast<tBillboardQuadGroup*>( prop->GetDrawable() );
                if( bbqg != NULL )
                {
                    tRefCounted::Assign( mNukeGlows[i], bbqg );

                    for( int j=0; j<mNukeGlows[i]->GetNumQuads(); j++ )
                    {
                        tBillboardQuad* quad = mNukeGlows[i]->GetQuad( j );
                        rAssert( quad );
                        mOriginalNukeGlowColours[count] = quad->GetColour();
                        count++;
                    }
                }
            }
        }
    }    
}    



//=============================================================================
// GeometryVehicle::SetCollectibleHardpointPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& position )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetCollectibleHardpointPosition( const rmt::Vector& position )
{
    // Use the default orientation, but replace the translational compoonent
    m_CollectibleTransform = DEFAULT_COLLECTIBLE_TRANSFORM;
    m_CollectibleTransform.FillTranslate( position );
    
}


//=============================================================================
// GeometryVehicle::SetCollectibleHardpointTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rotx ,float roty ,float rotz, const rmt::Vector& position )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetCollectibleHardpointTransform( float rotx ,float roty ,float rotz, 
                                                        const rmt::Vector& position )
{
    m_CollectibleTransform.Identity();
    m_CollectibleTransform.FillRotateXYZ( rotx, roty, rotz );
    m_CollectibleTransform.FillTranslate( position );
}


//=============================================================================
// GeometryVehicle::ShowBrakeLights
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::ShowBrakeLights()
{
    mBrakeLightsOn = true;
    /*
    int i;
    for(i = 0; i < 4; i++)
    {
        if(mBrakeLightJoints[i] != -1)
        {
            this->HideFlappingPiece(mBrakeLightJoints[i], false);
        }
    }
    */
}

//=============================================================================
// GeometryVehicle::HideBrakeLights
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::HideBrakeLights()
{
    mBrakeLightsOn = false;
    /*
    int i;
    for(i = 0; i < 4; i++)
    {
        if(mBrakeLightJoints[i] != -1)
        {
            this->HideFlappingPiece(mBrakeLightJoints[i], true);
        }
    }
    */
}


//=============================================================================
// GeometryVehicle::ShowReverseLights
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::ShowReverseLights()
{
    int i;
    for(i = 0; i < 4; i++)
    {
        if(mReverseLightJoints[i] != -1)
        {
            this->HideFlappingPiece(mReverseLightJoints[i], false);
        }
    }

}


//=============================================================================
// GeometryVehicle::HideReverseLights
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::HideReverseLights()
{
    int i;
    for(i = 0; i < 4; i++)
    {
        if(mReverseLightJoints[i] != -1)
        {
            this->HideFlappingPiece(mReverseLightJoints[i], true);
        }
    }

}


//=============================================================================
// GeometryVehicle::GetArt
//=============================================================================
// Description: Comment
//
// Parameters:  (char* name)
//
// Return:      bool 
//
//=============================================================================
bool GeometryVehicle::GetArt( const char* name)
{
    // expect a composite drawable named "name" in the file.

    tCompositeDrawable* master = p3d::find<tCompositeDrawable>(name);
	rTuneAssert( master != NULL );

#ifndef FINAL
    if (master == NULL)
    {
        char buffer [255];
        sprintf(buffer,"Script  ERROR: Can't Find Vehicle:%s Make sure you loaded it!\n",name);
        rTuneAssertMsg(0,buffer);
    }
#endif

    mCompositeDrawable = master->Clone();

    //mCompositeDrawable = p3d::find<tCompositeDrawable>(name);
    rAssert(mCompositeDrawable);
    mCompositeDrawable->AddRef();

    {
        char controllerName[255];
        strcpy( controllerName, "EFX_" );
        strcat( controllerName, name );
        strcat( controllerName, "velocpartSystShape" );

        tEffectController* particleSysController = p3d::find < tEffectController >( controllerName );   
        if ( particleSysController != NULL )
        {
            tEffect* effect = particleSysController->GetEffect();
            rAssert( dynamic_cast< tParticleSystem* >( effect ) != NULL );
            tParticleSystem* particleSystem = static_cast< tParticleSystem* > ( effect );
            mVariableEmissionParticleSystem = particleSystem;
            mVariableEmissionParticleSystem->AddRef();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Traffic "swatches" (in a sense)

    // Any car using traffic model (regardless of whether they are in fact traffic cars) 
    // Must contain a trafficbodydrawable so that it will render correctly (due to fading
    // and swatch tricks done for traffic models).

    if( IsTrafficModel() )
    {
        mUsingTrafficModel = true;

        // Grab the shader to be used for swatches
        char shadername[64];
        sprintf( shadername, "%s_m", name );
        tShader* bodyShade = p3d::find<tShader>( shadername );

        // Find the body prop in compositedrawable's list of elements by name
        // e.g. compactAShape
        char propname[64];
        sprintf( propname, "%sShape", name );
        tUID uid = tEntity::MakeUID(propname);

        int poseIndex = -1;
        tCompositeDrawable::DrawablePropElement* body = NULL;

        for( int i=0; i<mCompositeDrawable->GetNumDrawableElement(); i++ )
        {
            body = (tCompositeDrawable::DrawablePropElement*) (mCompositeDrawable->GetDrawableElement(i));
            if( body != NULL )
            {
                if( body->GetDrawable()->GetUID() == uid )
                {
                    poseIndex = i;
                    break;
                }
            }
        }

        if( poseIndex > -1 && body != NULL )
        {
            // create a trafficbodydrawable
            tRefCounted::Assign( mTrafficBodyDrawable, new TrafficBodyDrawable() );
            rAssert( mTrafficBodyDrawable );

            // store away the body prop's old drawable
            rAssert( body->GetDrawable() != NULL );
            mTrafficBodyDrawable->SetBodyPropDrawable( body->GetDrawable() );

            // Grab the shader for the vehicle chassis and store it too
            if( bodyShade == NULL )
            {
                rDebugPrintf( "Warning: Couldn't find shader \"%s\" for traffic vehicle %s. Chassis color randomization will not apply to this vehicle.\n",
                    shadername, name );
            }
            mTrafficBodyDrawable->SetBodyShader( bodyShade );

            // make the new traffic body drawable the drawable for the body prop
            body->SetDrawable( mTrafficBodyDrawable );
        }
        else
        {
            rDebugPrintf( "Warning: Couldn't find tCompositeDrawable prop \"%s\" for traffic vehicle %s. Chassis color randomization will not apply to this vehicle.\n",
                propname, name );
        }

        // Find the door prop in compositedrawable's list of elements by name
        // e.g. compactAShape
        sprintf( propname, "doorPRotShape" );
        uid = tEntity::MakeUID(propname);

        poseIndex = -1;
        body = NULL;
        for( int i=0; i<mCompositeDrawable->GetNumDrawableElement(); i++ )
        {
            body = (tCompositeDrawable::DrawablePropElement*) 
                (mCompositeDrawable->GetDrawableElement(i));
            if( body != NULL )
            {
                if( body->GetDrawable()->GetUID() == uid )
                {
                    poseIndex = i;
                    break;
                }
            }
        }

        if( poseIndex > -1 && body != NULL )
        {
            // create a trafficbodydrawable
            tRefCounted::Assign( mTrafficDoorDrawable, new TrafficBodyDrawable() );
            rAssert( mTrafficDoorDrawable );

            // store away the body prop's old drawable
            rAssert( body->GetDrawable() != NULL );
            mTrafficDoorDrawable->SetBodyPropDrawable( body->GetDrawable() );

            // Grab the shader for the vehicle chassis and store it too
            if( bodyShade == NULL )
            {
                rDebugPrintf( "Warning: Couldn't find shader \"%s\" for traffic vehicle %s. Chassis color randomization will not apply to this vehicle.\n",
                    shadername, name );
            }
            mTrafficDoorDrawable->SetBodyShader( bodyShade );

            // make the new traffic body drawable the drawable for the body prop
            body->SetDrawable( mTrafficDoorDrawable );
        }
        else
        {
            rDebugPrintf( "Warning: Couldn't find tCompositeDrawable prop \"%s\" for traffic vehicle %s. Chassis color randomization will not apply to this vehicle.\n",
                propname, name );
        }

    }
    // is this ok for release?
    // I think so

    // little hack just for frink
    // no wheels
    if( strcmp(name, "frink_v") == 0 || strcmp(name, "honor_v") == 0 || strcmp(name, "hbike_v") == 0 ||
        strcmp(name, "witchcar") == 0 || strcmp(name, "ship") == 0 || strcmp(name, "mono_v") == 0 )
    {
        FindAndTurnOffWheels();
    }

    if( strcmp( name, "frink_v" ) == 0 )
    {
        int count = 0;
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "special_m" ) ); //must be first
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vInt_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vTrim_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vDoorPNorm_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vExtras_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vHoodNorm_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vVent_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vcoil_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "BottomA_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vDoorDNorm_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "engine_m" ) );
        tRefCounted::Assign( mRefractionShader[ count++ ], p3d::find< tShader >( "frink_vBackNorm_m" ) );

        int size = count;
        int i;
        for( i = 0; i < size; ++i )
        {
            rAssert( mRefractionShader[ i ] != NULL );
        }

#ifdef RAD_WIN32
        tPose* p3dPose = mCompositeDrawable->GetPose();

        char buffy[128];
        sprintf(buffy, "frinkArcGroup", i);
        int jointIndex = p3dPose->FindJointIndex(buffy);

        for(int j = 0; j < mCompositeDrawable->GetNumDrawableElement(); j++)
        {
            tCompositeDrawable::DrawablePropElement* prop = (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(j));
            if(prop && prop->GetPoseIndex() == jointIndex)
            {
                tBillboardQuadGroup* bbqg = dynamic_cast<tBillboardQuadGroup*>( prop->GetDrawable() );
                if( bbqg )
                {
                    tRefCounted::Assign( mFrinkArc, bbqg );
                    for( int k = 0; k < NUM_FRINKARC_BBQS; k++ )
                    {
                        tBillboardQuad* quad = mFrinkArc->GetQuad( k );
                        mOriginalFrinkArcColour[k] = quad->GetColour();
                    }
                }
            }
        }    
#endif
    }
    
    if(strcmp(name, "rocke_v") == 0)
    {
        this->FindAndTurnOffFrontWheelsOnly();
    }

    // store brake light joint indices
    FindBrakeLightBillboardJoints();
    FindHeadLightBillboardJoints();

    if( GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
    {
        mHeadLightScale = 1.0f;
        mBrakeLightScale = 0.4f;
        if( mVehicleOwner->mVehicleType == VT_USER || mVehicleOwner->mVehicleType == VT_AI )
        {
            EnableLights( true );
        }
    }
    else
    {
        // if we are on a level other than 4 or 7, we want to turn the headlights off   
        if( GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L1 || // BROAD DAYLIGHT
            GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L2 )
        {
            // Which values should we use for headlights and brakelights
            // when we just have running lights (when not braking)
            mHeadLightScale = 0.0f;  
            mBrakeLightScale = 0.0f;//0.4f;
        }
        else if( GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L3 ) // SUNSET
        {
            mHeadLightScale = 0.2f;
            mBrakeLightScale = 0.3f;
        }
        else if( GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L5 ) // DUSK
        {
            mHeadLightScale = 0.4f;
            mBrakeLightScale = 0.4f;
        }
        else if( GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L6 ) // TWILIGHT
        {
            mHeadLightScale = 0.4f;
            mBrakeLightScale = 0.4f;
        }
        else if( GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L4 || // NIGHT
                GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L7 )
        {
            // full headlights on night levels
            mHeadLightScale = 0.6f; 
            mBrakeLightScale = 0.4f; 
        }
        if( mVehicleOwner->mVehicleType == VT_USER )
        {
            EnableLights( false );
        }
    }        



    // hack for ghost ship
    if( strcmp(name, "ship") == 0 )
    {
        mHasGhostGlow = true;
        FindGhostGlowBillboards( name );
    }

    if( strcmp( name, "nuctruck" ) == 0 )
    {
        mHasNukeGlow = true;
        FindNukeGlowBillboards( name );
    }


    // TODO - might need to clone this too
//    mAnimController = p3d::find<tPoseAnimationController>(buffy);

	// We will advance the multicontroller, not just the posecontroller

    FindAnimationControllers( name );

	char buffy[128];
	sprintf(buffy, "PTRN_%s", name);

	tPoseAnimationController* poseController = p3d::find<tPoseAnimationController>(buffy);

	// only play an anim if it's there:

    if( poseController ) 
    {
        poseController->SetPose(mCompositeDrawable->GetPose());

        // TODO
        // cast here might be unsafe in the general case
        //mAnim = (tPoseAnimation*)(poseController->GetAnimation());
        mAnim = poseController->GetAnimation();
        mAnim->AddRef();    // ? already addref'd by controller?
        mAnim->SetCyclic(true);

        float numFrames = mAnim->GetNumFrames();
        float speed = mAnim->GetSpeed();

		rAssert( numFrames != 0 );

        mAnimRevPerSecondBase = speed / (float)numFrames;
    }

    char roofShaderName[64];
    sprintf( roofShaderName, "%sRoof_m", name );
    tRefCounted::Assign( mRoofShader, p3d::find<tShader>( tEntity::MakeUID( roofShaderName ) ) );

    FindRoofGeometry( name );
    if( mRoofOpacShape && mRoofAlphaShape )
    {
        mRoofOpacShape->SetVisibility( true );
        mRoofAlphaShape->SetVisibility( false );
    }
    
    if ( mVehicleOwner->GetNameObject() == "gramR_v" )
    {
        SetCollectibleHardpointPosition( JEEP_COLLECTIBLE_POS );
    }
    else if ( mVehicleOwner->GetNameObject() == "homer_v" )
    {
        SetCollectibleHardpointPosition( BARRACUDA_COLLECTIBLE_POS );
    }
    else if ( mVehicleOwner->GetNameObject() == "dune_v" )
    {
        SetCollectibleHardpointTransform( DUNE_COLLECTIBLE_ROT.x,
                                          DUNE_COLLECTIBLE_ROT.y,
                                          DUNE_COLLECTIBLE_ROT.z,
                                          DUNE_COLLECTIBLE_POS);
    }
    else if ( mVehicleOwner->GetNameObject() == "hallo" )
    {
        SetCollectibleHardpointPosition( HALLO_COLLECTIBLE_POS );   
    }
    else if ( mVehicleOwner->GetNameObject() == "coffin" )
    {
        SetCollectibleHardpointPosition( COFFIN_COLLECTIBLE_POS );   
    }
    else if ( mVehicleOwner->GetNameObject() == "witchcar" )
    {
        SetCollectibleHardpointPosition( WITCH_COLLECTIBLE_POS );   
    }

    if(mVehicleOwner->mVehicleType == VT_USER)
    {
        tCompositeDrawable::DrawableElement* agnes = mCompositeDrawable->FindNode("agnusGeoShape");
        if(agnes)
        {
            if(GetVehicleCentral()->IsDriverSuppressed("skinner"))
            {
                agnes->SetVisibility(false);
            }
        }
    }

    //THERE'S AN UNTRACKED ALLOCATION HERE!
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);


    return FindDamageShadersAndTextures(name);
    //return true;

}


//=============================================================================
// GeometryVehicle::FindDamageShadersAndTextures
//=============================================================================
// Description: Comment
//
// Parameters:  (char* name)
//
// Return:      void 
//
//=============================================================================
bool GeometryVehicle::FindDamageShadersAndTextures( const char* carname)
{
    // names we expect to have:
    //
    // for shaders
    // <carname>DoorDNorm_m         
    // <carname>DoorPNorm_m         
    // <carname>HoodNorm_m          
    // <carname>BackNorm_m          
   

    // for normal textures:
    // <carname>DoorDNorm.bmp       
    // <carname>DoorPNorm.bmp           
    // <carname>HoodNorm.bmp        
    // <carname>BackNorm.bmp        current: BarTrunk.bmp
    
    
    // for damage textures:
    // <carname>DoorPDam.bmp        current: BarracudaDoorPDam.bmp
    // <carname>HoodDam.bmp         current: BarracudaHoodDam.bmp
    // <carname>DoorDDam.bmp        current: BarracudaDoorDDam.bmp
    // <carname>BackDam.bmp         current: BarracudaBackDam.bmp

    // !!
    // TODO - search a particular inventory section only?
                        
    char buffy[128];

    
    // change logic here - if we find at least one set of shader, normal texture, damage texture
    // return true.



    bool result = false;

    bool doorD = true;
    bool doorP = true;
    bool hood = true;
    bool trunk = true;
    

    //-------
    //shaders
    //-------

    sprintf(buffy, "%sDoorDNorm_m", carname);
    mDoorDShader = p3d::find<tShader>(buffy);
    if(mDoorDShader)
    {
        mDoorDShader->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        doorD = false;
    }

    sprintf(buffy, "%sDoorPNorm_m", carname);
    mDoorPShader = p3d::find<tShader>(buffy);
    if(mDoorPShader)
    {
        mDoorPShader->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        doorP = false;
    }


    sprintf(buffy, "%sHoodNorm_m", carname);
    mHoodShader = p3d::find<tShader>(buffy);
    if(mHoodShader)
    {
        mHoodShader->AddRef();    
    }
    else
    {   
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        hood = false;
    }


    sprintf(buffy, "%sBackNorm_m", carname);
    mTrunkShader = p3d::find<tShader>(buffy);
    if(mTrunkShader)
    {
        mTrunkShader->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        trunk = false;
    }


    
    //----------------
    // normal textures
    //----------------
    
    sprintf(buffy, "%sDoorDNorm.bmp", carname);
    mDoorDTextureNorm = p3d::find<tTexture>(buffy);
    if(mDoorDTextureNorm)
    {
        mDoorDTextureNorm->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        doorD = false;
    }


    sprintf(buffy, "%sDoorPNorm.bmp", carname);
    mDoorPTextureNorm = p3d::find<tTexture>(buffy);
    if(mDoorPTextureNorm)
    {
        mDoorPTextureNorm->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        doorP = false;
    }


    sprintf(buffy, "%sHoodNorm.bmp", carname);
    mHoodTextureNorm = p3d::find<tTexture>(buffy);
    if(mHoodTextureNorm)
    {
        mHoodTextureNorm->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        hood = false;
    }


    sprintf(buffy, "%sBackNorm.bmp", carname);
    mTrunkTextureNorm = p3d::find<tTexture>(buffy);
    if(mTrunkTextureNorm)
    {
        mTrunkTextureNorm->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        trunk = false;
    }



    //----------------
    // damage textures
    //----------------
    
    sprintf(buffy, "%sDoorDDam.bmp", carname);
    mDoorDTextureDam = p3d::find<tTexture>(buffy);
    if(mDoorDTextureDam)
    {
        mDoorDTextureDam->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        doorD = false;
    }


    sprintf(buffy, "%sDoorPDam.bmp", carname);
    mDoorPTextureDam = p3d::find<tTexture>(buffy);
    if(mDoorPTextureDam)
    {
        mDoorPTextureDam->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        doorP = false;
    }


    sprintf(buffy, "%sHoodDam.bmp", carname);
    mHoodTextureDam = p3d::find<tTexture>(buffy);
    if(mHoodTextureDam)
    {
        mHoodTextureDam->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        hood = false;
    }


    sprintf(buffy, "%sBackDam.bmp", carname);
    mTrunkTextureDam = p3d::find<tTexture>(buffy);
    if(mTrunkTextureDam)
    {
        mTrunkTextureDam->AddRef();
    }
    else
    {
        rTunePrintf("vehicle damage: cant find %s\n", buffy);
        //result = false;
        trunk = false;
    }


    // now if any of doorD, doorP, hood, or trunk are true - return true
    
    result = doorD | doorP | hood | trunk;
    


    //----------------------------------------------------
    // all of the above was if we are a type 1 or 2 damage
    //
    // for type 3 we just expect the following:
    //----------------------------------------------------

    //-------
    // shader
    //-------
    /*
    
    
        // ancient fucking history
    
    
    sprintf(buffy, "%sChassisNorm_m", carname);
    mChassisShader = p3d::find<tShader>(buffy);
    if(mChassisShader)
    {
        mChassisShader->AddRef();
    }

    //---------------
    // normal texture
    //---------------

    sprintf(buffy, "%sChassisNorm.bmp", carname);
    mChassisTextureNorm = p3d::find<tTexture>(buffy);
    if(mChassisTextureNorm)
    {
        mChassisTextureNorm->AddRef();
    }

    //---------------
    // damage texture
    //---------------

    sprintf(buffy, "%sChassisDam.bmp", carname);
    mChassisTextureDam = p3d::find<tTexture>(buffy);
    if(mChassisTextureDam)
    {
        mChassisTextureDam->AddRef();
    }
    */


    return result;

}



//=============================================================================
// GeometryVehicle::SetSmoke
//=============================================================================
// Description: Comment
//
// Parameters:  (ParticleEnum::ParticleID pid)
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetEngineSmoke(ParticleEnum::ParticleID pid)
{
    mEngineParticleAttr.mType = pid;
}


/*
ParticleEnum::ParticleID mType;

    // only one bias so far, the emission bias
    // other biases would be simple to put in 
    // NumParticles/Life/Speed/Weight/Gravity/Drag/Size/Spin
    float mEmissionBias;
*/


//=============================================================================
// GeometryVehicle::SetWheelSmoke
//=============================================================================
// Description: Comment
//
// Parameters:  (ParticleEnum::ParticleID pid, float bias)
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetWheelSmoke( int wheel, ParticleEnum::ParticleID pid, float bias)
{
    switch( wheel )
    {
    case 0:
        mRightWheelParticleAttr.mType = pid;
        mRightWheelParticleAttr.mEmissionBias = bias;
        break;
    case 1:
        mLeftWheelParticleAttr.mType = pid;
        mLeftWheelParticleAttr.mEmissionBias = bias;
        break;
    default:
        mRightWheelParticleAttr.mType = pid;
        mRightWheelParticleAttr.mEmissionBias = bias;

        mLeftWheelParticleAttr.mType = pid;
        mLeftWheelParticleAttr.mEmissionBias = bias;
        break;
    };
}


//=============================================================================
// GeometryVehicle::DamageTextureChassis
//=============================================================================
// Description: Comment
//
// Parameters:  (bool on)
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::DamageTextureChassis(bool on)
{
    rAssert(0); // this is fucking history
    if(mChassisShader)
    {
        if(on)
        {
            mChassisShader->SetTexture(PDDI_SP_BASETEX, mChassisTextureDam);
        }
        else
        {
            mChassisShader->SetTexture(PDDI_SP_BASETEX, mChassisTextureNorm);
        }
    }
}


//=============================================================================
// GeometryVehicle::DamageTextureTrunk
//=============================================================================
// Description: Comment
//
// Parameters:  (bool on)
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::DamageTextureTrunk(bool on)
{
    if(mTrunkShader && mTrunkTextureDam && mTrunkTextureNorm)
    {
        if(on)
        {
            mTrunkShader->SetTexture(PDDI_SP_BASETEX, mTrunkTextureDam);
        }
        else
        {
            mTrunkShader->SetTexture(PDDI_SP_BASETEX, mTrunkTextureNorm);
        }
    }
}

//=============================================================================
// GeometryVehicle::DamageTextureHood
//=============================================================================
// Description: Comment
//
// Parameters:  (bool on)
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::DamageTextureHood(bool on)
{
    if(mHoodShader && mHoodTextureDam && mHoodTextureNorm)
    {
        if(on)
        {
            mHoodShader->SetTexture(PDDI_SP_BASETEX, mHoodTextureDam);
        }
        else
        {
            mHoodShader->SetTexture(PDDI_SP_BASETEX, mHoodTextureNorm);
        }
    }
}

//=============================================================================
// GeometryVehicle::DamageTextureDoorP
//=============================================================================
// Description: Comment
//
// Parameters:  (bool on)
//
// Return:      
//
//=============================================================================
void GeometryVehicle::DamageTextureDoorP(bool on)
{
    if(mDoorPShader && mDoorPTextureDam && mDoorPTextureNorm)
    {
        if(on)
        {
            mDoorPShader->SetTexture(PDDI_SP_BASETEX, mDoorPTextureDam);
        }
        else
        {
            mDoorPShader->SetTexture(PDDI_SP_BASETEX, mDoorPTextureNorm);
        }
    }
}

//=============================================================================
// GeometryVehicle::DamageTextureDoorD
//=============================================================================
// Description: Comment
//
// Parameters:  (bool on)
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::DamageTextureDoorD(bool on)
{
    if(mDoorDShader && mDoorDTextureDam && mDoorDTextureNorm)
    {
        if(on)
        {
            mDoorDShader->SetTexture(PDDI_SP_BASETEX, mDoorDTextureDam);
        }
        else
        {
            mDoorDShader->SetTexture(PDDI_SP_BASETEX, mDoorDTextureNorm);
        }
    }
}


//=============================================================================
// GeometryVehicle::HideFlappingPiece
//=============================================================================
// Description: Comment
//
// Parameters:  (int jointindex)
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::HideFlappingPiece(int jointindex, bool doit)
{

    //tCompositeDrawable::DrawablePropElement* prop = (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(jointindex));
    
    int i;
    for(i = 0; i < mCompositeDrawable->GetNumDrawableElement(); i++)
    {
        tCompositeDrawable::DrawablePropElement* prop = (tCompositeDrawable::DrawablePropElement*)(mCompositeDrawable->GetDrawableElement(i));
        if(prop && prop->GetPoseIndex() == jointindex)
        {
            //prop->SetVisibility(false);
            prop->SetVisibility(!doit);
        }
    }

}


//------------------------------------------------------------------------
void GeometryVehicle::Update(float dt)
{
    AdvanceAnimationControllers( dt * 1000.0f );
  
    
    
    rmt::Vector vehicleVelocity;
    mVehicleOwner->GetVelocity( &vehicleVelocity );
    // Certain animations and particle systems are scaled by the velocity
    // Calculate what % the scalar should be
    float animationSpeedBias = vehicleVelocity.Magnitude() / PARTICLE_SYSTEM_MAX_VELOCITY;
    animationSpeedBias = animationSpeedBias > 1.0f ? 1.0f : animationSpeedBias;
    animationSpeedBias = animationSpeedBias < 0.0f ? 0.0f : animationSpeedBias;

    if ( mVariableEmissionParticleSystem != NULL )
    {
        mVariableEmissionParticleSystem->SetBias( p3dParticleSystemConstants::EmitterBias::EMISSION, animationSpeedBias );
    }


    //
    // Vary prof frink's refraction shader every frame
    //

    const tName& name = mVehicleOwner->GetNameObject();
    if( name == "frink_v" )
    {
        float topSpeed = mVehicleOwner->GetTopSpeed();
        float percentOfTopSpeed = vehicleVelocity.Magnitude() / topSpeed;
        float refraction = percentOfTopSpeed;
        refraction = rmt::Clamp( refraction, 0.0f, 1.0f );

        /*
        //we adjust the refractive index based on distance from the camera
        rmt::Vector pos = mVehicleOwner->GetPosition ();
        rmt::Vector cameraPosition;
        unsigned int index = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCamIndex();
        SuperCam* camera = GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( index );
        camera->GetPosition( &cameraPosition );
        float distance = ( pos - cameraPosition ).Magnitude();

        float refractionAmount = refractiveIndex;
        //if( distance > 20 )
        {
            refractionAmount *= 5;
            refractionAmount /= distance;
        }
        */
        

        //
        // special case for the windshield shader
        //
        if( mRefractionShader[ 0 ] != NULL )
        {
            #ifdef RAD_GAMECUBE
                float adjustedRefraction = refraction;
            #else
                float adjustedRefraction = 0.5f + (0.5f * refraction);
            #endif

            #ifndef RAD_WIN32
                mRefractionShader[ 0 ]->SetFloat( PDDI_SP_REFRACTBLEND, adjustedRefraction );
                mRefractionShader[ 0 ]->SetFloat( PDDI_SP_REFRACTINDEX, refractiveIndex );
            #endif
        }

        //
        // all other shaders
        //
        int i;
        for( i = 1; i < MAX_REFRACTION_SHADERS; ++i )
        {
            if( mRefractionShader[ i ] != NULL )
            {
                #ifndef RAD_WIN32
                    mRefractionShader[ i ]->SetFloat( PDDI_SP_REFRACTBLEND, refraction );
                    mRefractionShader[ i ]->SetFloat( PDDI_SP_REFRACTINDEX, refractiveIndex );
                #endif
            }
        }
    }

    // Update the collectible's position
    if ( m_Collectible != NULL )
    {
        rmt::Matrix collectibleTransform;
        collectibleTransform.Mult( m_CollectibleTransform, mVehicleOwner->mTransform );
        m_Collectible->SetTransform( collectibleTransform );
    }
}


//=============================================================================
// GeometryVehicle::SetTrafficBodyColour
//=============================================================================
// Description: Comment
//
// Parameters:  ( pddiColour colour )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetTrafficBodyColour( pddiColour colour )
{
    if( mVehicleOwner->mVehicleType == VT_TRAFFIC || mVehicleOwner->mVehicleType == VT_USER )
    {
        if( mTrafficBodyDrawable != NULL )
        {
            mTrafficBodyDrawable->SetDesiredColour( colour );
        }
        if( mTrafficDoorDrawable != NULL )
        {
            mTrafficDoorDrawable->SetDesiredColour( colour );
        }
    } 
}


//=============================================================================
// GeometryVehicle::GetP3DPose
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      tPose
//
//=============================================================================
tPose* GeometryVehicle::GetP3DPose()
{
    if(mCompositeDrawable)
    {
        return mCompositeDrawable->GetPose();
    }
    else
    {
        return 0;
    }
}


//=============================================================================
// GeometryVehicle::SetShadowAdjustments
//=============================================================================
// Description: Comment
//
// Parameters:  ( float Adjustments[ 4 ][ 2 ] )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetShadowAdjustments( float Adjustments[ 4 ][ 2 ] )
{
    for( int i = 0; i < 4; ++i )
    {
        mShadowPointAdjustments[ i ][ 0 ] = Adjustments[ i ][ 0 ];
        mShadowPointAdjustments[ i ][ 1 ] = Adjustments[ i ][ 1 ];
    }
}


//=============================================================================
// GeometryVehicle::SetFadeAlpha
//=============================================================================
// Description: Comment
//
// Parameters:  ( int fade )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::SetFadeAlpha( int fade )
{
    rAssert( 0 <= fade && fade <= 255 );
    mFadeAlpha = fade;
}

// Hack: 
// We hard code the traffic models so we can tell which vehicle
// needs special treatment in terms of fading... Be sure to add
// any new traffic models here or they won't fade properly.
//
// The problem is that the intbroadcast on all the shaders in
// this composite drawable is going to reset everyone's blend
// mode to BLEND_ALPHA... Brakelights, however, need BLEND_ADDITIVE
// and some cars (cCells, cPolice) have special light effects that 
// have similar problems. Setting these all to BLEND_ALPHA will 
// cause the billboards to not display properly, so we make sure
// we do an intbroadcast (using ProcessShaders()) only for cars 
// that are using traffic models (including non-traffic cars such 
// as mission AI cars).
//
// The alternative to this is to avoid using ProcessShaders() and
// set the appropriate shader for each composite drawable's element 
// that need to fade independently... Could be messy... 
// This hard-coded list is not so bad..
//
//=============================================================================
// GeometryVehicle::IsTrafficModel
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool GeometryVehicle::IsTrafficModel()
{
    rAssert( mVehicleOwner );

    VehicleEnum::VehicleID ID = mVehicleOwner->mVehicleID;

    if( ID == VehicleEnum::HUSKA ||

        ID == VehicleEnum::COMPACTA ||
        ID == VehicleEnum::PICKUPA ||
        ID == VehicleEnum::MINIVANA ||
        ID == VehicleEnum::SUVA ||
        ID == VehicleEnum::SPORTSA ||

        ID == VehicleEnum::SPORTSB ||
        ID == VehicleEnum::SEDANA ||
        ID == VehicleEnum::SEDANB ||
        ID == VehicleEnum::TAXIA ||
        ID == VehicleEnum::WAGONA ||

        ID == VehicleEnum::COFFIN ||
        ID == VehicleEnum::HALLO ||
        ID == VehicleEnum::WITCHCAR ||
        ID == VehicleEnum::SHIP ||
        ID == VehicleEnum::AMBUL ||

        ID == VehicleEnum::BURNSARM ||
        ID == VehicleEnum::FISHTRUC ||
        ID == VehicleEnum::GARBAGE ||
        ID == VehicleEnum::ICECREAM ||
        ID == VehicleEnum::ISTRUCK ||

        ID == VehicleEnum::NUCTRUCK ||
        ID == VehicleEnum::PIZZA ||
        ID == VehicleEnum::SCHOOLBU ||
        ID == VehicleEnum::VOTETRUC ||
        ID == VehicleEnum::GLASTRUC )

    {
        return true;
    }
    return false;
}


//=============================================================================
// GeometryVehicle::FadeRoof
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool fade )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FadeRoof( bool fade )
{
    rAssert( 0 <= INCAR_ROOF_ALPHA && INCAR_ROOF_ALPHA <= 255 );
    mRoofTargetAlpha = (fade)? INCAR_ROOF_ALPHA : 255;
}


//=============================================================================
// GeometryVehicle::FindRoofGeometry
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* model )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindRoofGeometry( const char* model )
{
    char opac[64];
    sprintf(opac, "%sRoofOpacShape", model );
    tUID opacID = tEntity::MakeUID( opac );

    char alpha[64];
    sprintf(alpha, "%sRoofAlphaShape", model );
    tUID alphaID = tEntity::MakeUID( alpha );

    for( int j = 0; j < mCompositeDrawable->GetNumDrawableElement(); j++)
    {
        tCompositeDrawable::DrawablePropElement* prop = 
            (tCompositeDrawable::DrawablePropElement*)
            (mCompositeDrawable->GetDrawableElement(j));

        if( prop )
        {
            tUID uid = prop->GetUID();
            if( uid == opacID )
            {
                mRoofOpacShape = prop;
            }
            else if( uid == alphaID )
            {
                mRoofAlphaShape = prop;
            }
        }
    }    
}


//=============================================================================
// GeometryVehicle::EnableLights
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool enable )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::EnableLights( bool enable )
{
    mEnableLights = enable;
}


//=============================================================================
// GeometryVehicle::GetVehicleColour
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      tColour 
//
//=============================================================================
tColour GeometryVehicle::GetVehicleColour()const
{
    if ( mTrafficBodyDrawable != NULL )
    {
        return mTrafficBodyDrawable->GetDesiredColour();
    }
    else
    {
        const tColour DEFAULT_COLOUR( 128,128,128 );
        return DEFAULT_COLOUR;
    }
}


//=============================================================================
// GeometryVehicle::FindAnimationControllers
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* multicontrollername )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::FindAnimationControllers( const char* multicontrollername )
{

    tMultiController* mc = p3d::find< tMultiController >( multicontrollername );
    if( mc == NULL )
        return;

    HeapMgr()->PushHeap(GMA_LEVEL_OTHER);

    // Iterate through the frame controllers and add them to our own list
    for ( unsigned int i = 0 ; i < mc->GetNumTracks() ; i++ )
    {
        tFrameController* fc = mc->GetTrack( i )->Clone();

        VehicleFrameController vfc;

        vfc.trigger = eNone;
        vfc.brakeBias = vfc.gasBias = vfc.speedBias = 0;
        vfc.timeBias = 1.0f;
        vfc.frameSetter = false;

        GetSpecialController( fc->GetUID(), &vfc );
        vfc.frameController = fc;
        vfc.frameController->AddRef();
        vfc.frameController->Reset();
        vfc.frameController->SetCycleMode( FORCE_CYCLIC );  

        // What kind of animation controller are we dealing with?
        // Possibly a pose controller?
        tPoseAnimationController* poseController = dynamic_cast< tPoseAnimationController* >( vfc.frameController );
        if ( poseController )
        {
            // The compositeDrawble::clone function allocates a brand new pose
            // set the frame controller that this pose is the one to update upon
            // Advance()
            poseController->SetPose( mCompositeDrawable->GetPose() );     

        }

        mFrameControllers.push_back( vfc );
    }   
    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}


//=============================================================================
// GeometryVehicle::GetSpecialController
//=============================================================================
// Description: Comment
//
// Parameters:  ( tUID name, VehicleFrameController* outSpecialController )
//
// Return:      bool 
//
//=============================================================================
bool GeometryVehicle::GetSpecialController( tUID name, VehicleFrameController* outSpecialController )
{
    bool wasFound = false;
    // iterate through the preset "special" controllers and find any with the same name
    for ( int i = 0 ; i < NUM_FRAME_CONTROLLERS ; i++ )
    {
        if ( name == tName::MakeUID( FRAME_CONTROLLER_DATA[i].name ) )
        {
            *outSpecialController = FRAME_CONTROLLER_DATA[i];
            wasFound = true;
            break;
        }   
    }
    return wasFound;
}


//=============================================================================
// GeometryVehicle::AdvanceAnimationControllers
//=============================================================================
// Description: Comment
//
// Parameters:  ( float deltaTime )
//
// Return:      void 
//
//=============================================================================
void GeometryVehicle::AdvanceAnimationControllers( float deltaTime )
{
    int numFCs = mFrameControllers.size();

    float gas = mVehicleOwner->GetGas();
    float speed = mVehicleOwner->GetSpeedKmh();
    float brake = mVehicleOwner->GetBrake();

    if ( mVehicleOwner->IsMovingBackward() )
        speed *= -1;

    FCTrigger trigger = eNone;
    
    if ( mVehicleOwner->GetDeltaGas() > 0.001f )
    {
        trigger = eBackfire;
    }
    for ( int i = 0 ; i < numFCs ; i++ )
    {
        VehicleFrameController& vfc = mFrameControllers[i];
        if ( vfc.frameSetter )
        {
            // This thing doesn't ever run linearly, it only sets the frame
            // it uses specifically.
            // Figure out which frame to set based upon vehicle attributes
            float relativeframe;
            float numFrames = vfc.frameController->GetNumFrames();
            relativeframe = vfc.brakeBias * brake + vfc.gasBias * gas + 
                vfc.speedBias * speed;

            vfc.frameController->SetFrame( relativeframe * numFrames );
            vfc.frameController->Advance( 0.0f );
        }
        else
        {
            // Is the animation triggered by something?
            // Check to see if it is or isn't
            if ( trigger != eNone && trigger == vfc.trigger )
            {
                // Its a triggered animation thats been triggered
                // If the animation isnt playing already then restart it
                if ( vfc.frameController->GetRelativeSpeed() < 0.001f )
                {
                    vfc.frameController->Reset();
                    vfc.frameController->SetRelativeSpeed( 1.0f );                    
                }
                if ( vfc.frameController->LastFrameReached() )
                {
                    vfc.frameController->SetRelativeSpeed( 0 );
                }
            }
            // Standard animation, apply biases and play
            float advanceTime = vfc.timeBias * deltaTime;
            if ( advanceTime < 0 ) advanceTime = 0;

            float advanceGas = gas * vfc.gasBias * deltaTime;
            if ( advanceGas < 0 ) advanceGas = 0;

            float advanceBrake =  brake * vfc.brakeBias * deltaTime;
            if ( advanceBrake < 0 ) advanceBrake = 0;

            float advanceSpeed =  speed * vfc.speedBias * deltaTime;
            if ( advanceSpeed < 0 ) advanceSpeed = 0;

            float totalAdvanceTime = advanceTime + advanceGas - advanceBrake + advanceSpeed;

            vfc.frameController->Advance( totalAdvanceTime );
        }
    }
}

