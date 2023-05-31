//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supercamcentral.cpp
//
// Description: Implement SuperCamCentral
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
#include <stdlib.h>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>
#include <simcollision/collisionmanager.hpp>
#include <simcommon/simstate.hpp>
#include <p3d/pointcamera.hpp>
#include <p3d/view.hpp>


//========================================
// Project Includes
//========================================
#include <camera/SuperCam.h>
#include <camera/SuperCamCentral.h>
#include <camera/animatedcam.h>
#include <camera/conversationcam.h>
#include <camera/frustrumdrawable.h>
#include <camera/isupercamtarget.h> 
#include <camera/supercamcontroller.h>
#include <camera/followcam.h>
#include <camera/wrecklesscam.h>
#ifdef RAD_WIN32
#include <camera/pccam.h>
#endif
#include <data/gamedatamanager.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>
#include <memory/srrmemory.h>
#include <input/inputmanager.h>
#include <input/controller.h>
#include <debug/profiler.h>
#include <meta/fovlocator.h>
#include <meta/eventlocator.h>
#include <worldsim/physicsairef.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <events/eventmanager.h>
#include <events/eventdata.h>

#include <render/IntersectManager/IntersectManager.h>

#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <main/game.h>

#include <cheats/cheatinputsystem.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
//#define PRINTCAMERANAMES
const rmt::Vector DEBUG_NAME_POS(0.8f, 0.8f, 0);

//Debug Camera settings.
const float DEBUG_CAM_XZ_ANGLE = 0.0f;
const float DEBUG_CAM_Y_ANGLE = 0.03f;
const float DEBUG_CAM_DIST = 70.0f;
const float DEBUG_CAM_FOV = 90.0f;
const float DEBUG_CAM_ASPECT = 4.0f / 3.0f;
const float DEBUG_CAM_NEAR = 0.1f;
const float DEBUG_CAM_FAR = 700.0f;

SuperCam::Type CAMERAS_FOR_DRIVING[] =
{
    SuperCam::NEAR_FOLLOW_CAM,
    SuperCam::FAR_FOLLOW_CAM,
    SuperCam::BUMPER_CAM,

    //Start of cheat cams
    SuperCam::COMEDY_CAM,
    SuperCam::WRECKLESS_CAM,
    SuperCam::CHASE_CAM,
    SuperCam::KULL_CAM,
    SuperCam::TRACKER_CAM,
    SuperCam::DEBUG_CAM
};

const int NUM_CAMERAS_FOR_DRIVING = 9;

// this must be less than or equal to NUM_CAMERAS_FOR_DRIVING
//
const int NUM_CAMERAS_FOR_DRIVING_WITHOUT_CHEAT = 3;

SuperCam::Type CAMERAS_FOR_WALKING[] =
{
    SuperCam::WALKER_CAM,
#ifdef RAD_WIN32
    SuperCam::PC_CAM,
#endif
    SuperCam::DEBUG_CAM,
    SuperCam::KULL_CAM
};

#ifdef RAD_WIN32
const int NUM_CAMERAS_FOR_WALKING = sizeof(CAMERAS_FOR_WALKING)/sizeof(SuperCam::Type);

// this must be less than or equal to NUM_CAMERAS_FOR_WALKING
//
const int NUM_CAMERAS_FOR_WALKING_WITHOUT_CHEAT = 2;
#else
const int NUM_CAMERAS_FOR_WALKING = 3;

// this must be less than or equal to NUM_CAMERAS_FOR_WALKING
//
const int NUM_CAMERAS_FOR_WALKING_WITHOUT_CHEAT = 1;
#endif

SuperCam::Type SUPER_SPRINT_CAMERAS[] = 
{
    SuperCam::NEAR_FOLLOW_CAM,
    SuperCam::FAR_FOLLOW_CAM,
    SuperCam::BUMPER_CAM,
    SuperCam::COMEDY_CAM,
    SuperCam::WRECKLESS_CAM
#ifndef RAD_RELEASE
    ,SuperCam::KULL_CAM
#endif
};

const int NUM_SUPERSPRINT_CAMS = 
sizeof( SUPER_SPRINT_CAMERAS ) / sizeof( SUPER_SPRINT_CAMERAS[ 0 ] );

const unsigned int DEFAULT_CAM_SWITCH_DELAY = 0;  //milliseconds

unsigned char SuperCamCentral::mTotalSuperCamCentrals = 0;
const char* SuperCamCentral::CAMERA_INVENTORY_SECTION = "CameraInventorySection";
FollowCamDataChunk SuperCamCentral::mFollowCamDataChunks[ MAX_DATA_CHUNKS ];
unsigned int SuperCamCentral::mNumUsedFDC = 0;


#ifdef DEBUGWATCH

void ToggleDebugViewCallBack( void* userData )
{
    SuperCamCentral* ssc = (SuperCamCentral*)userData;

    ssc->ToggleDebugView();
}

void ToggleCameraForwardsCallBack( void* userData )
{
    SuperCamCentral* ssc = (SuperCamCentral*)userData;

    ssc->ToggleSuperCam( true );
}

void ToggleCameraBackwardsCallBack( void* userData )
{
    SuperCamCentral* ssc = (SuperCamCentral*)userData;

    ssc->ToggleSuperCam( false );
}

void ToggleBurnoutCallBack( void* userData )
{
    static bool gBurnout = false;

    if ( !gBurnout )
    {
        GetEventManager()->TriggerEvent( EVENT_BURNOUT );
        gBurnout = true;
    }
    else
    {
        GetEventManager()->TriggerEvent( EVENT_BURNOUT_END );
        gBurnout = false;
    }
}

void ToggleCameraCut( void* userData )
{
    SuperCamCentral* ssc = (SuperCamCentral*)userData;

    ssc->DoCameraCut();
}
#endif

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperCamCentral::SuperCamCentral
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
SuperCamCentral::SuperCamCentral() :
    mCollisionAreaIndex(-1),
    mActiveSuperCam( NULL ),
    mActiveSuperCamIndex( 0 ),
    mNumRegisteredSuperCams( 0 ),
    mCamera( NULL ),
    mTarget( NULL ),
#ifdef SUPERCAM_DEBUG
    mDebugXZAngle( 0.0f ),
    mDebugYAngle( rmt::PI ),
    mDebugMagnitude( DEBUG_CAM_DIST ),
#endif
    mCurrentFOVLocator( NULL ),
    mDoCameraCut( false ),
    mChanceToBurnout( 10 ),
    mCameraSimState( NULL ),
    mCameraCollisionFudge( 1.05f ),
    mController( NULL ),
    mControllerHandle( -1 ),
    mWrecklessCount( 0 ),
    mPreferredFollowCam( FollowCam::FAR_FOLLOW_CAM ),
    mInitialCamera(false),
    mIsInvertedCameraEnabled( false ),
    mJumpCamsEnabled( true ),
    mCameraToggling( false ),
    mNastyHypeCamHackEnabled( false )
{
MEMTRACK_PUSH_GROUP( "SuperCamCentral" );
    //Each SuperCamCentral is uniquely numbered.
    //These numbers correspond to players.
    mMyNumber = mTotalSuperCamCentrals;
    ++mTotalSuperCamCentrals;

    unsigned int i;

    for ( i = 0; i < MAX_CAMERAS; ++i )
    {  
        mSuperCameras[ i ] = NULL;
    }

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

#ifdef SUPERCAM_DEBUG
    mDebugCamera = new tPointCamera();
    mDebugCamera->AddRef();

    char camName[256];
    sprintf( camName, "SuperCamCentral %d DebugCam", mMyNumber );
    mDebugCamera->SetName( camName );

    mFrustrumDrawable = new FrustrumDrawable();
    mFrustrumDrawable->AddRef();

    char frustName[256];
    sprintf( frustName, "FrustrumDrawable%d", mMyNumber );
    mFrustrumDrawable->SetName( frustName );
#endif

    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_END );
    GetEventManager()->AddListener( this, EVENT_BURNOUT );
    GetEventManager()->AddListener( this, EVENT_BURNOUT_END );

    GetEventManager()->AddListener( this, EVENT_CAMERA_SHAKE );

    GetGameDataManager()->RegisterGameData( this, 1, "Super Cam Central" );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
#ifdef DEBUGWATCH
    char player[256];
    sprintf( player, "SuperCamCentral\\Player%d", mMyNumber );
    radDbgWatchAddFunction( "Toggle Debug View", &ToggleDebugViewCallBack, this, player );
    radDbgWatchAddFunction( "Toggle Cameras Forwards", &ToggleCameraForwardsCallBack, this, player );
    radDbgWatchAddFunction( "Toggle Cameras Backwards", &ToggleCameraBackwardsCallBack, this, player );
    radDbgWatchAddFunction( "Toggle Burnout Test", &ToggleBurnoutCallBack, this, player );
    radDbgWatchAddFunction( "Toggle Camera Cut", &ToggleCameraCut, this, player );
    
    radDbgWatchAddFloat( &mDebugXZAngle, "Debug Camera XZ Angle", player, NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &mDebugYAngle, "Debug Camera Y Angle", player, NULL, NULL, 0.001f, rmt::PI_2 );
    radDbgWatchAddFloat( &mDebugMagnitude, "Debug Camera Distance", player, NULL, NULL, 10.0f, 1000.0f );
    radDbgWatchAddUnsignedInt( &mChanceToBurnout, "Burnout Cam Chance", player, NULL, NULL, 1, 100 );
    radDbgWatchAddFloat( &mCameraCollisionFudge, "Camera Collision Fudge", player, NULL, NULL, 0.01f, 2.0f );
#endif
MEMTRACK_POP_GROUP( "SuperCamCentral" );
}

//=============================================================================
// SuperCamCentral::~SuperCamCentral
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
SuperCamCentral::~SuperCamCentral()
{
    const bool shutdown = true;
    Init( shutdown ); //Clear everything out.

#ifdef SUPERCAM_DEBUG
    if ( mDebugCamera )
    {
        mDebugCamera->Release();
        mDebugCamera = NULL;
    }

    if ( mFrustrumDrawable )
    {
        mFrustrumDrawable->Release();
        mFrustrumDrawable = NULL;
    }
#endif

    if ( mCurrentFOVLocator )
    {
        mCurrentFOVLocator->Release();
        mCurrentFOVLocator = NULL;
    }

    GetEventManager()->RemoveAll( this );

#ifdef DEBUGWATCH
    radDbgWatchDelete( & ToggleDebugViewCallBack );
    radDbgWatchDelete( & ToggleCameraForwardsCallBack );
    radDbgWatchDelete( & ToggleCameraBackwardsCallBack );
    radDbgWatchDelete( & mDebugXZAngle );
    radDbgWatchDelete( & mDebugYAngle );
    radDbgWatchDelete( & mDebugMagnitude );
    radDbgWatchDelete( & mChanceToBurnout );
    radDbgWatchDelete( & mCameraCollisionFudge );
#endif
}

//=============================================================================
// SuperCamCentral::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool shutdown )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::Init( bool shutdown )
{
    // Make sure this is a valid call
    if( !shutdown && IsInit() ||    // already initialized
        shutdown && !IsInit() )     // already shut down
    {
        return;
    }

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    unsigned int i;

    if ( mActiveSuperCam )
    {
        mActiveSuperCam->Shutdown();
    }

    for ( i = 0; i < MAX_CAMERAS; ++i )
    {  
        if ( mSuperCameras[ i ] )
        {
            mSuperCameras[ i ]->UnregisterDebugControls();
            mSuperCameras[ i ]->Release();
        }

        mSuperCameras[ i ] = NULL;
    }

    mActiveSuperCam = NULL;
    mActiveSuperCamIndex = 0;

    if ( mCurrentFOVLocator )
    {
        mCurrentFOVLocator->Release();
        mCurrentFOVLocator = NULL;
    }

    if ( mCamera )
    {
        mCamera->Release();
    }
    mCamera = NULL;

    mTarget = NULL;

#ifdef SUPERCAM_DEBUG
    mDebugViewOn = false;
    mDebugCamera->SetFOV( rmt::DegToRadian( DEBUG_CAM_FOV ), DEBUG_CAM_ASPECT );
    mDebugCamera->SetNearPlane( DEBUG_CAM_NEAR );
    mDebugCamera->SetFarPlane( DEBUG_CAM_FAR );
#endif

    mIntersectionList.Clear();

    if ( !shutdown )
    {
        // find mCollisionAreaIndex we can use:
        mCollisionAreaIndex = GetWorldPhysicsManager()->GetCameraCollisionAreaIndex();
        rAssert(mCollisionAreaIndex != -1);

   
        // probably a good place to make the camera collision sphere

        const rmt::Vector p(0.0f, 0.0f,0.0f);
        const float sphereRadius = 2.0f;    // TODO - what should this be.

        mCameraCollisionVolume = new sim::SphereVolume(p, sphereRadius);
        mCameraCollisionVolume->AddRef();
        
        //mCameraCollisionObject = new(GMA_PERSISTENT)sim::CollisionObject(mCameraCollisionVolume);
        //mCameraCollisionObject->AddRef();
        
        mCameraSimState = (sim::ManualSimState*)sim::SimState::CreateManualSimState(mCameraCollisionVolume);
        mCameraSimState->AddRef();

        mCameraCollisionObject = mCameraSimState->GetCollisionObject();
        mCameraCollisionObject->AddRef();

        mCameraCollisionObject->SetManualUpdate(true);
        mCameraCollisionObject->SetAutoPair(true);
        mCameraCollisionObject->SetIsStatic(false); // hmmmmmm....
        
        mCameraCollisionObject->SetCollisionEnabled(true);

        char buffy[128];
        sprintf(buffy, "camera_with_collision_area_index_%d", mCollisionAreaIndex);
        mCameraCollisionObject->SetName(buffy);

        mCameraSimState->mAIRefIndex = PhysicsAIRef::CameraSphere;
        mCameraSimState->mAIRefPointer = (void*)this;

        
        GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(mCameraCollisionObject, mCollisionAreaIndex);

        mCameraCollisionCount = 0;
        mCameraTerrainCollisionOffsetFix.Set(0.0f, 0.0f, 0.0f);

        mController = new SuperCamController();
        rAssert( mController );
        mController->AddRef();

        InputManager* im = InputManager::GetInstance();
        rAssert( im ); 

        int controllerID = GetInputManager()->GetControllerIDforPlayer( mMyNumber );
        if( controllerID != -1 )
        {
            mControllerHandle = im->RegisterMappable( controllerID, mController );

            if( mMyNumber == 0 )
            {
                const int NUM_CONTROLLERS = Input::MaxControllers;

                // set secondary SuperCam controller to be = (primary + 1),
                // and wrap back to controller 0
                //
                int secondaryControllerID = (controllerID + 1) % NUM_CONTROLLERS;
                while ( !(GetInputManager()->GetController( secondaryControllerID )->IsConnected()) )
                {
                    secondaryControllerID = (secondaryControllerID + 1) % NUM_CONTROLLERS;
                    if ( secondaryControllerID == controllerID )
                    {
                        secondaryControllerID = (secondaryControllerID + 1) % NUM_CONTROLLERS;
                        break;
                    }
                }

                SuperCam::SetSecondaryControllerID( static_cast<unsigned int>( secondaryControllerID ) );
            }
        }

        if ( GetGameplayManager()->mIsDemo )
        {
            //GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::CAMERA_CUT) );
        }

        p3d::inventory->AddSection( CAMERA_INVENTORY_SECTION );
    }
    else
    {
        // remove from collision manager
        // empty area?!

        mCameraCollisionVolume->Release();
        mCameraCollisionObject->Release();

        GetWorldPhysicsManager()->FreeCollisionAreaIndex( mCollisionAreaIndex );

        if ( mController )
        {
            InputManager* im = InputManager::GetInstance();
            rAssert( im );

            if( mControllerHandle != -1 )
            {
                int controllerID = GetInputManager()->GetControllerIDforPlayer( mMyNumber );
                
                if ( controllerID == -1 )
                {
                    im->UnregisterMappable( mController );
                }
                else
                {
                    im->UnregisterMappable( controllerID, mControllerHandle );
                }
            }

            mController->Release();
            mController = NULL;
            mControllerHandle = -1;
        }

        if ( mCameraSimState )
        {
            mCameraSimState->Release();
            mCameraSimState = NULL;
        }

        p3d::inventory->RemoveSectionElements( tEntity::MakeUID( CAMERA_INVENTORY_SECTION ) );
        p3d::inventory->DeleteSection( CAMERA_INVENTORY_SECTION );
    }

    //This is safe for startup or shutdown.
    CleanupDataChunks();

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
}

//=============================================================================
// SuperCamCentral::IsInit
//=============================================================================
// Description: Returns true if cam initialized.
//
// Parameters:  n/a
//
// Return:      bool...
//
//=============================================================================

bool SuperCamCentral::IsInit() const
{
    return mController != NULL;
}

//=============================================================================
// SuperCamCentral::UpdateCameraCollisionSpherePosition
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& p)
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::UpdateCameraCollisionSpherePosition(rmt::Vector& p)
{
    mCameraCollisionVolume->mPosition = p;

    //mCameraCollisionObject->Update();

    mCameraCollisionObject->PostManualUpdate();
}


//=============================================================================
// SuperCamCentral::UpdateCameraCollisionSphereRadius
//=============================================================================
// Description: Comment
//
// Parameters:  (float radius)
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::UpdateCameraCollisionSphereRadius(float radius)
{
    // this if for Cary
    //
    // not sure how he wants to use it.

     mCameraCollisionVolume->UpdateRadius(radius);

}

//=============================================================================
// SuperCamCentral::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds, bool isFirstSubstep )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::Update( unsigned int milliseconds, bool isFirstSubstep )
{
#if defined(RAD_XBOX) || defined(RAD_WIN32)
    if ( mController && 
         !mCameraToggling && 
         mController->GetValue( SuperCamController::cameraToggle ) == 1.0f &&
         AllowCameraToggle() && 
         GetGameplayManager()->GetGameType() != GameplayManager::GT_SUPERSPRINT &&
         mActiveSuperCam &&
#ifdef RAD_WIN32
         mActiveSuperCam->GetType() != SuperCam::PC_CAM &&
#endif
         mActiveSuperCam->GetType() != SuperCam::WALKER_CAM )
    {
        ToggleSuperCam( true );
        mCameraToggling = true;
    }
    else if ( mController && mCameraToggling && mController->GetValue( SuperCamController::cameraToggle) < 0.5f )
    {
        mCameraToggling = false;
    }
#endif

    //Test to see if we should change cameras or not.
    if ( mNextSuperCam.incoming )
    {
        //Let's see if we should switch cameras.
        if ( mNextSuperCam.nextCamDelay - static_cast<int>(milliseconds) <= 0 || mActiveSuperCam == NULL || mDoCameraCut )
        {
            if ( mDoCameraCut )
            {
                mNextSuperCam.timems = 0;
            }

            SetActiveSuperCam( mNextSuperCam.nextSuperCam, mNextSuperCam.flags, mNextSuperCam.timems );
        }
        else
        {
            mNextSuperCam.nextCamDelay -= milliseconds;
        }
    }

    if ( mActiveSuperCam != NULL )
    {
  
#ifdef PROFILER_ENABLED
        char sCCname[256];
        sprintf( sCCname, "SCC: %d Update", mMyNumber );
        BEGIN_PROFILE( sCCname )
#endif
#if defined(DEBUGMENU) && defined(SUPERCAM_DEBUG)
        if ( BEGIN_DEBUGINFO_SECTION("Super Cam Debug") )
        {
            DEBUGINFO_ADDLINE( rmt::Vector( 0,0,0 ), rmt::Vector(10.0f,0,0), tColour( 255, 0, 0 ) );
            DEBUGINFO_ADDLINE( rmt::Vector( 0,0,0 ), rmt::Vector(0,10.0f,0), tColour( 0, 255, 0 ) );
            DEBUGINFO_ADDLINE( rmt::Vector( 0,0,0 ), rmt::Vector(0,0,10.0f), tColour( 0, 0, 255 ) );
        }
#endif

        //Do the reverse cam thing if we're using the followcam
        if ( ( mActiveSuperCam->GetType() == SuperCam::NEAR_FOLLOW_CAM ||
               mActiveSuperCam->GetType() == SuperCam::FAR_FOLLOW_CAM ) &&
             static_cast<FollowCam*>(mActiveSuperCam)->ShouldReverse() )
        {
            if ( !mNextSuperCam.incoming || mSuperCameras[mNextSuperCam.nextSuperCam]->GetType() != SuperCam::REVERSE_CAM )
            {
                SelectSuperCam( SuperCam::REVERSE_CAM, FORCE | NO_TRANS, 0 );
            }
        }
        else if ( mActiveSuperCam->GetType() == SuperCam::REVERSE_CAM && mActiveSuperCam->CanSwitch() )
        {
            if ( !mNextSuperCam.incoming || 
                 ( mSuperCameras[mNextSuperCam.nextSuperCam]->GetType() != SuperCam::NEAR_FOLLOW_CAM && 
                   mSuperCameras[mNextSuperCam.nextSuperCam]->GetType() != SuperCam::FAR_FOLLOW_CAM ) )
            {
                SelectSuperCam( SuperCam::FOLLOW_CAM, FORCE | NO_TRANS, 0 );
            }
        }

        if ( mDoCameraCut )
        {
            mActiveSuperCam->DoCameraCut();
            mDoCameraCut = false;
        }

        if ( mController )
        {
            //Test for lookback.
            float lookBack = mController->GetValue( SuperCamController::lookBack );

#if defined(RAD_GAMECUBE) || defined(RAD_XBOX)  //Both now!
            float altLookBack = mController->GetValue( SuperCamController::altLookBack );

            lookBack = mController->GetAxisValue( SuperCamController::stickY );
#if defined(RAD_XBOX)
            lookBack = lookBack < -0.8f ? -1.0f : 0.0f;
#endif

            if ( altLookBack == 1.0f || lookBack == -1.0f )
#elif defined(RAD_WIN32)
            if( lookBack > 0.8f )
#else //This is PS2
            if ( mController->GetValue( SuperCamController::l2) >= 0.9f && mController->GetValue( SuperCamController::r2 ) >= 0.9f )
            {
                lookBack = 1.0f;
            }

            if ( rmt::Fabs( lookBack ) > STICK_DEAD_ZONE )
#endif
            {
                //This will be reset at end of use.
                mActiveSuperCam->LookBack( true );
            }
        }

#ifdef PROFILER_ENABLED
        char sCname[256];
        sprintf( sCname, "SC: %s #%d Update", mActiveSuperCam->GetName(), mMyNumber );
        BEGIN_PROFILE( sCname )
#endif

        if ( isFirstSubstep )
        {
            mIntersectionList.Clear();

            rmt::Vector targetPos;
            mTarget->GetPosition( &targetPos );
            mIntersectionList.FillIntersectionListStatics( targetPos, mActiveSuperCam->GetIntersectionRadius() );
            if ( mTarget->IsCar() )
            {
                Vehicle* playerCar = GetGameplayManager()->GetCurrentVehicle();
                mIntersectionList.FillIntersectionListDynamics( targetPos, mActiveSuperCam->GetIntersectionRadius(), false, playerCar );
            }
            else
            {
                Character* playerCharacter = GetCharacterManager()->GetCharacter( 0 );
                mIntersectionList.FillIntersectionListDynamics( targetPos, mActiveSuperCam->GetIntersectionRadius(), false, playerCharacter );
                mIntersectionList.FillIntersectionListAnimPhys( targetPos, mActiveSuperCam->GetIntersectionRadius() );
            }
        }

        //Update the currently active supercam.
        #ifdef DEBUGWATCH
            #ifdef PRINTCAMERANAMES
                mActiveSuperCam->PrintClassName();
            #endif
        #endif
        mActiveSuperCam->Update( milliseconds );

#ifdef PROFILER_ENABLED
        END_PROFILE( sCname )
#endif

#if defined(DEBUGMENU) && defined(SUPERCAM_DEBUG)
        //This displays the name of the active SuperCam
        DEBUGINFO_ADDSCREENTEXTVECTOR( mActiveSuperCam->GetName(), DEBUG_NAME_POS )
        END_DEBUGINFO_SECTION;
#endif
#ifdef PROFILER_ENABLED
        END_PROFILE( sCCname )
#endif

        //Hmmm  Must think about this.  This could be a virtual position and
        //not really where the camera is.
        rmt::Vector pos;
        mActiveSuperCam->GetPosition(&pos);

        if ( mActiveSuperCam->GetType() == SuperCam::WALKER_CAM ||
             mActiveSuperCam->GetType() == SuperCam::FIRST_PERSON_CAM )
        {
            rmt::Vector heading;
            mActiveSuperCam->GetHeading( &heading );
            heading.NormalizeSafe();
            heading.Scale( mActiveSuperCam->GetCollisionRadius() );

            pos.Add( heading );
        }

        UpdateCameraCollisionSpherePosition(pos);
        UpdateCameraCollisionSphereRadius( mActiveSuperCam->GetCollisionRadius() );
    }
}


//=============================================================================
// SuperCamCentral::PreCollisionPrep
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::PreCollisionPrep()
{
    mCameraCollisionCount = 0;
    
    mCameraTerrainCollisionOffsetFix.Set(0.0f, 0.0f, 0.0f);
    // new
    // special vector just for ground collision
    
    // do get intersects here.
    
    bool foundTri = false;
    bool foundPlane = false;
    
    rmt::Vector closestTriNormal, closestTriPosn;
    rmt::Vector planeNormal, planePosn;
    rmt::Vector fudgedCollisionPos = mCameraCollisionVolume->mPosition;
//    fudgedCollisionPos.y -= 1.0f;  //Lower it down.

    GetIntersectManager()->FindIntersection(fudgedCollisionPos, 
                                            foundPlane,
                                            planeNormal,
                                            planePosn );


    // hmmm....
    // if we get both a tri and a plane, use the one with the more upright normal?
    
    // for starters, just use plane
    
    if(foundPlane)
    {
        mCameraTerrainCollisionOffsetFix = planeNormal;  
        //mCameraTerrainCollisionOffsetFix.Set(0.0f, 1.0f, 0.0f);
        
        rmt::Vector uptest(0.0f, 1.0f, 0.0f);
        rAssert(planeNormal.DotProduct(uptest) > 0.0f);
        
        float y = planePosn.y;
        float penetratingDepth = y - (fudgedCollisionPos.y - mCameraCollisionVolume->mSphereRadius);
        
        if(penetratingDepth > 0.0f)        
        {
         
            float fixAlongPlaneNormal = penetratingDepth / rmt::Fabs( (planeNormal.DotProduct(uptest))); // the fabs should be redundant here

            mCameraTerrainCollisionOffsetFix.Scale(fixAlongPlaneNormal);            

        }
        else
        {
            mCameraTerrainCollisionOffsetFix.Set( 0.0f, 0.0f, 0.0f );
        }
    
    }
}


//=============================================================================
// SuperCamCentral::AddCameraCollisionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& fixOffset)
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::AddCameraCollisionOffset(rmt::Vector& fixOffset)
{
    rAssertMsg( mCameraCollisionCount < MAX_COLLISIONS, "Too many collisions for the camera!  This one is ignored! \n" );
    if ( mCameraCollisionCount >= MAX_COLLISIONS )
    {
        return;
    }

    if ( rmt::Epsilon( fixOffset.MagnitudeSqr(), 0.0000001f ) )
    {
        //Too small to care about.
        return;
    }

//    if ( fixOffset.DotProduct( rmt::Vector( 0.0f, -1.0f, 0.0f ) ) > 0.0f )
//    {
//        //We don't get forced down.
//        return;
//    }

    //Do we want to reject this collision?
    rmt::Vector normalTestFix = fixOffset;
    normalTestFix.NormalizeSafe();
    unsigned int i;
    for ( i = 0; i < mCameraCollisionCount; ++i )
    {
        const float dotTest = 0.7f;
        float dotResult = normalTestFix.DotProduct( mCameraCollisionOffsetFix[ i ] );
        if ( dotResult >= dotTest )
        {
            //Throw me away please.
            return;
        }
    }

    //Fuge-ma scale
    fixOffset.Scale( 1.0f + mCameraCollisionFudge );

    if ( mCameraCollisionCount < MAX_COLLISIONS )
    {
        mCameraCollisionOffsetFix[ mCameraCollisionCount ] = fixOffset;
        mCameraCollisionCount++;
    }
}
    

//=============================================================================
// SuperCamCentral::RegisterSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( SuperCam* cam )
//
// Return:      unsigned int 
//
//=============================================================================
unsigned int SuperCamCentral::RegisterSuperCam( SuperCam* cam )
{
    rAssert( cam->GetType() < SuperCam::INVALID );

    cam->AddRef();

    cam->SetPlayerID( (int)mMyNumber );

    unsigned int i;
    bool added = false;

    for ( i = 0; i < MAX_CAMERAS; ++i )
    {  
        if ( mSuperCameras[ i ] == NULL )
        {
            mSuperCameras[ i ] = cam;
            added = true;
            break;
        }
    }

    rAssert( added );
    
    if ( mCamera )
    {
        cam->SetCamera( mCamera );
    }

    if ( mTarget )
    {
        cam->SetTarget( mTarget );
    }

    //DO this last in case the init needs to know about the cameras.
//    cam->Init();  //Took out since it was call to often.  Cary Jan 29

    // increment number of registered super cams
    mNumRegisteredSuperCams++;

    cam->SetRegistered( true );

    return i;
}

//=============================================================================
// SuperCamCentral::UnregisterSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( SuperCam* cam )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::UnregisterSuperCam( SuperCam* cam )
{
    rAssert( cam != NULL );
    if ( cam == NULL )
        return;

    rAssert( cam->GetType() < SuperCam::INVALID );

    bool found = false;
    unsigned int i;
    for ( i = 0; i < MAX_CAMERAS; ++i )
    {  
        if ( mSuperCameras[ i ] == cam )
        {
            mSuperCameras[ i ]->SetRegistered( false );
            mSuperCameras[ i ]->UnregisterDebugControls();
            mSuperCameras[ i ]->Release();
            mSuperCameras[ i ] = NULL;

            found = true;
            break;
        }
    }

    if ( !found )
    {
        return;
    }

    //If there is no incoming camera and the current cam is the active one, let's choose a new
    //one intelligently.
    if ( !mNextSuperCam.incoming && mActiveSuperCam == cam )
    {
        //This one is gone now.
        mActiveSuperCam = NULL;

        unsigned int timems = 7000;
        int extraFlags = 0;
        if ( mDoCameraCut )
        {
            timems = 0;
            extraFlags = CUT;
        }

        //Should do something smart here.  How about this...
        SelectSuperCam( SuperCam::DEFAULT_CAM, FORCE | extraFlags, timems );

        rAssert( mActiveSuperCam );
    }
    else if ( mNextSuperCam.incoming && mActiveSuperCam == cam )
    {
        SetActiveSuperCam( mNextSuperCam.nextSuperCam, mNextSuperCam.flags, mNextSuperCam.timems );
    }

    // decrement number of registered super cams
    mNumRegisteredSuperCams--;
}

//=============================================================================
// SuperCamCentral::UnregisterSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int which )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::UnregisterSuperCam( unsigned int which )
{
    rAssert( mSuperCameras[ which ] != NULL );

    UnregisterSuperCam( mSuperCameras[ which ] );
}

//=============================================================================        
// SuperCamCentral::GetNumRegisteredSuperCams
//=============================================================================
// Description: Comment
//
// Parameters:  none
//
// Return:      number of registered super cams 
//
//=============================================================================
unsigned int SuperCamCentral::GetNumRegisteredSuperCams() const
{
    return mNumRegisteredSuperCams;
}

//=============================================================================        
// SuperCamCentral::GetActiveSuperCamIndex
//=============================================================================
// Description: Comment
//
// Parameters:  none
//
// Return:      index to active super cam
//
//=============================================================================
unsigned int SuperCamCentral::GetActiveSuperCamIndex() const
{
    return mActiveSuperCamIndex;
}

//=============================================================================        
// SuperCamCentral::GetSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  super cam index
//
// Return:      reference to specified super cam
//
//=============================================================================
SuperCam* SuperCamCentral::GetSuperCam( unsigned int which ) const
{
    rAssert( which < MAX_CAMERAS );

    return mSuperCameras[ which ];
}

//=============================================================================        
// SuperCamCentral::GetSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  super cam type
//
// Return:      if found, return reference to super cam; otherwise, return NULL
//
//=============================================================================
SuperCam* SuperCamCentral::GetSuperCam( SuperCam::Type type ) const
{
    SuperCam* superCam = NULL;

    for( int i = 0; i < MAX_CAMERAS; i++ )
    {
        if( mSuperCameras[ i ] != NULL &&
            mSuperCameras[ i ]->GetType() == type )
        {
            // found it!
            //
            superCam = mSuperCameras[ i ];

            break;
        }
    }

    return superCam;
}

//=============================================================================        
// SuperCamCentral::ToggleSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool forwards, bool quickTransition )
//
// Return:      bool 
//
//=============================================================================
bool SuperCamCentral::ToggleSuperCam( bool forwards, bool quickTransition )
{
    SuperCam::Type* cameras = NULL;
    unsigned int numCameras = 0;
    if ( mTarget->IsCar() )
    {
        if ( GetGameplayManager()->IsSuperSprint() )
        {
            cameras = SUPER_SPRINT_CAMERAS;
            numCameras = NUM_SUPERSPRINT_CAMS;
        }
        else
        {
            cameras = CAMERAS_FOR_DRIVING;

            if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_CAMERAS ) )
            {
                numCameras = NUM_CAMERAS_FOR_DRIVING;
            }
            else
            {
                numCameras = NUM_CAMERAS_FOR_DRIVING_WITHOUT_CHEAT;
            }
        }

    }
    else
    {
        cameras = CAMERAS_FOR_WALKING;

        if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_CAMERAS ) )
        {
            numCameras = NUM_CAMERAS_FOR_WALKING;
        }
        else
        {
            numCameras = NUM_CAMERAS_FOR_WALKING_WITHOUT_CHEAT;
        }
    }

    unsigned int start = 0;

    unsigned int i;
    for ( i = 0; i < numCameras; ++i )
    {
        if ( mActiveSuperCam && cameras[ i ] == mActiveSuperCam->GetType() )
        {
            start = i;
            break;
        }
    }

    unsigned int current;

    if ( forwards )
    {
        current = (start+1) % numCameras;
    }
    else
    {
        current = ((start-1) + numCameras) % numCameras;
    }

    SelectSuperCam( cameras[ current ], SuperCamCentral::CUT | SuperCamCentral::QUICK, 0 );

    return true;
}

//=============================================================================    
// SuperCamCentral::SelectSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int which, int flags, unsigned int timems )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SelectSuperCam( unsigned int which, int flags, unsigned int timems )
{
    if ( mNastyHypeCamHackEnabled )
    {
        //This is for the frickin' camera on level 6 where the user should never
        //go, but can and will enevitably fuck the camera system.
        return;
    }

    if ( which < MAX_CAMERAS && mSuperCameras[ which ] != NULL )
    {
        SuperCam::Type whichType = mSuperCameras[ which ]->GetType();

        if ( !IsLegalType( whichType ) )
        {
            //This is a fix to trying to select cams you shouldn't (hack)
            //You can't switch to a camera that isn't legal.
            SelectSuperCam( SuperCam::DEFAULT_CAM, flags, timems );
            return;
        }

        if( whichType == SuperCam::CONVERSATION_CAM )
        {
            ConversationCam* conv = dynamic_cast< ConversationCam* >( mSuperCameras[ which ] );
            rAssert( conv != NULL );
            conv->LockCharacterPositions();
        }
        else if ( whichType == SuperCam::BUMPER_CAM &&
                  mTarget->IsCar() )
        {
            //Always force.
            flags |= FORCE;

            //No bumper cam for RC Car.
            Vehicle* target = dynamic_cast<Vehicle*>( mTarget );
            rAssert( target );

            if ( target->mVehicleID == VehicleEnum::DUNE_V )
            {
                SelectSuperCam( SuperCam::NEAR_FOLLOW_CAM, flags, timems );
                return;
            }
        }

        SuperCam::Type activeType;
        if( mActiveSuperCam != NULL )
        {
            activeType = mActiveSuperCam->GetType();
        }

        if ( mActiveSuperCam == NULL || timems == 0 )
        {
            SetActiveSuperCam( which, flags, timems );
        }
        else if(
            ( mSuperCameras[ which ]->GetType() == SuperCam::ANIMATED_CAM ) ||
            ( mSuperCameras[ which ]->GetType() == SuperCam::RELATIVE_ANIMATED_CAM )
            )
        {
            AnimatedCam* cam = static_cast< AnimatedCam* >( mSuperCameras[ which ] );
            SuperCam::Type currentType = mSuperCameras[ mActiveSuperCamIndex ]->GetType();
            cam->SetNextCameraType( currentType );
            SetActiveSuperCam( which, flags, timems );
        }
        else if( mActiveSuperCam && ( activeType == SuperCam::ANIMATED_CAM ) && ( ( flags & FORCE ) == 0x00 ) )
        {
            AnimatedCam* cam = dynamic_cast< AnimatedCam* >( GetSuperCam( SuperCam::ANIMATED_CAM ) );
            rAssert( cam != NULL );
            cam->SetNextCameraType( mSuperCameras[ which ]->GetType() );
        }
        else
        {
            //Crap..  Let's eliminate this...
            /*
            mNextSuperCam.nextSuperCam = which;

            if ( timems == 0 )
            {
                mNextSuperCam.nextCamDelay = 0;
            }
            else
            {
                mNextSuperCam.nextCamDelay = DEFAULT_CAM_SWITCH_DELAY;
            }

            mNextSuperCam.flags = flags;
            mNextSuperCam.timems = timems;
            mNextSuperCam.incoming = true;
            */

            SetActiveSuperCam( which, flags, timems );
        }
    }
}

//=============================================================================
// SuperCamCentral::SelectSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( SuperCam::Type type, int flags, unsigned int timems )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SelectSuperCam( SuperCam::Type type, int flags, unsigned int timems )
{
    if ( type == SuperCam::DEFAULT_CAM )
    {
		if( mTarget == NULL )
		{
			type = SuperCam::FOLLOW_CAM;
		}
		else if ( mTarget->IsCar() )
		{
			if ( GetGameplayManager()->mIsDemo )
			{
				type =  SuperCam::FOLLOW_CAM;
			}
			else if ( GetGameplayManager()->IsSuperSprint() )
			{
				type = SuperCam::SUPER_SPRINT_CAM;
			}
			else
			{
				type =  SuperCam::FOLLOW_CAM;
			}
		}
		else
		{
#ifdef RAD_WIN32
            type =  SuperCam::ON_FOOT_CAM;
#else
            type =  SuperCam::WALKER_CAM;
#endif
		}
    }

#ifdef RAD_WIN32
    if ( type == SuperCam::ON_FOOT_CAM )
    {
        //Ziemek?   Check the PC cam flag here.
        bool bMouseLookMode = GetInputManager()->GetController(0)->IsMouseLookOn();
        if ( bMouseLookMode ) //Or PC flag is set
        {
            type = SuperCam::PC_CAM;
        }
        else
        {
            type = SuperCam::WALKER_CAM;
        }
    }
#endif

    if ( type == SuperCam::FOLLOW_CAM )
    {
        //Find the preferred type
        type = mPreferredFollowCam;
    }
    else if ( type == SuperCam::NEAR_FOLLOW_CAM || type == SuperCam::FAR_FOLLOW_CAM || type == SuperCam::BUMPER_CAM || type == SuperCam::COMEDY_CAM )
    {
        //Set the new preferred type
        mPreferredFollowCam = type;
    }

    //If it still ain't legal, make it legal.
    if ( !IsLegalType( type ) )
    {
        type = SuperCam::FAR_FOLLOW_CAM;
        mPreferredFollowCam = type;
    }

    unsigned int i;
    for ( i = 0; i < MAX_CAMERAS; ++i )
    {
        if ( mSuperCameras[ i ] != NULL )
        {
            if ( mSuperCameras[ i ]->GetType() == type )
            {
                SelectSuperCam( i, flags, timems );
                break;
            }
        }
    }
}

//=============================================================================
// SuperCamCentral::SelectSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( SuperCam* cam, int flags = CUT | QUICK, unsigned int timems = 7000 )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SelectSuperCam( SuperCam* cam, int flags, unsigned int timems )
{
    rAssert( cam );

    if ( cam )
    {
        unsigned int i;
        for ( i = 0; i < MAX_CAMERAS; ++i )
        {
            if ( mSuperCameras[ i ] != NULL )
            {
                if ( mSuperCameras[ i ] == cam )
                {
                    SelectSuperCam( i, flags, timems );
                    break;
                }
            }
        }
    }
}

//=============================================================================
// SuperCamCentral::SetCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( tPointCamera* cam )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SetCamera( tPointCamera* cam )
{
    if ( mCamera )
    {
        mCamera->Release();
    }

    unsigned int i;
    for ( i = 0; i < MAX_CAMERAS; ++i )
    {
        if ( mSuperCameras[ i ] )
        {
            mSuperCameras[ i ]->SetCamera( cam );
        }
    }

    mCamera = cam;
    cam->AddRef();
}

//=============================================================================
// SuperCamCentral::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SetTarget( ISuperCamTarget* target )
{
    if ( mTarget != target )
    {
        unsigned int i;
        for ( i = 0; i < MAX_CAMERAS; ++i )
        {
            if ( mSuperCameras[ i ] )
            {
                mSuperCameras[ i ]->SetTarget( target );
            }
        }

        //We should do a transition on the active super cam to make sure that
        //We don't jump around too much.
        if ( mActiveSuperCam )
        {
            mActiveSuperCam->DoCameraTransition( false );
        }

        mTarget = target;
    }
    
}

//=============================================================================
// SuperCamCentral::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::AddTarget( ISuperCamTarget* target )
{
    mActiveSuperCam->AddTarget( target );   

    if ( mNextSuperCam.incoming )
    {
        rAssert( mNextSuperCam.nextSuperCam < MAX_CAMERAS );
        rAssert( mSuperCameras[ mNextSuperCam.nextSuperCam ] != NULL );
        mSuperCameras[ mNextSuperCam.nextSuperCam ]->AddTarget( target );
    }
}

//=============================================================================
// SuperCamCentral::SetActiveSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int which, int flags, unsigned int timems )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SetActiveSuperCam( unsigned int which, int flags, unsigned int timems )
{   
    //Clear the incoming.
    mNextSuperCam.incoming = false;

    if ( !AllowCameraToggle() && (flags & FORCE) == 0 )  //Not forced
    {
        return;
    }

    if ( mSuperCameras[ which ] == mActiveSuperCam )
    {
        //This one is already active.
        if( ( mNextSuperCam.flags & FORCE ) != 0 )
        {
            mActiveSuperCam->EndTransition();
        }
        if ( (flags & CUT) != 0 )
        {
            mActiveSuperCam->DoCameraCut();
        }
        return;
    }

    bool oldWasBumperCam = false;

    if ( mActiveSuperCam != NULL )
    {
        mActiveSuperCam->UnregisterDebugControls();

        mActiveSuperCam->Shutdown();

        oldWasBumperCam = mActiveSuperCam->GetType() == SuperCam::BUMPER_CAM;
    }

    if ( mSuperCameras[ which ] != NULL)
    {
        mActiveSuperCam = mSuperCameras[ which ];
        mActiveSuperCam->RegisterDebugControls();

        GetEventManager()->TriggerEvent( EVENT_CAMERA_CHANGE, mActiveSuperCam );
        mActiveSuperCamIndex = which;

        //Hack..  TODO:  Should this be moved?
        mActiveSuperCam->SetNearPlane( SUPERCAM_NEAR );

        mActiveSuperCam->Init();
        mActiveSuperCam->ResetTwistDelta();  //This stops funny twist snap

        if ( mCurrentFOVLocator )
        {
            //Enable the FOV override here.  There could be some issues with timing, but we'll see.
            mActiveSuperCam->SetFOVOverride( mCurrentFOVLocator->GetFOV() );
            mActiveSuperCam->OverrideFOV( true, mCurrentFOVLocator->GetTime(), mCurrentFOVLocator->GetRate() );
        }
        
        //Cut the camera view
        if ( (flags & CUT) != 0 || 
             oldWasBumperCam ||
             mActiveSuperCam->GetType() == SuperCam::BUMPER_CAM )  //Auto cut on bumper cam too
        {
            mActiveSuperCam->DoCameraCut();
        }
        else if ( (flags & NO_TRANS) == 0 )
        {
            //Smothly blend the camera views
            mActiveSuperCam->DoCameraTransition( (flags & QUICK) != 0, timems );
        }

        mActiveSuperCam->DoFirstTime();

        rDebugString( "NEW SUPER CAM!\n" );
    }
}


//=============================================================================
// SuperCamCentral::SubmitStatics
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SubmitStatics()
{
    // only do this for certain types of camers:
    // TODO - add others?
    SuperCam::Type camType = mActiveSuperCam->GetType();
    rmt::Vector pos;
    pos = mCameraCollisionVolume->mPosition;

    if( mActiveSuperCam && 
        (camType == SuperCam::NEAR_FOLLOW_CAM ||
         camType == SuperCam::FAR_FOLLOW_CAM ||
         camType == SuperCam::WALKER_CAM ||
#ifdef RAD_WIN32
         camType == SuperCam::PC_CAM ||
#endif
         camType == SuperCam::COMEDY_CAM ||
         camType == SuperCam::FIRST_PERSON_CAM ||
         camType == SuperCam::REVERSE_CAM)
       )
    {

        GetWorldPhysicsManager()->SubmitStaticsPseudoCallback(pos, mActiveSuperCam->GetCollisionRadius() /* need multiplier? */, mCollisionAreaIndex, mCameraSimState);
        GetWorldPhysicsManager()->SubmitFencePiecesPseudoCallback(pos, mActiveSuperCam->GetCollisionRadius(), 
                                                                  mCollisionAreaIndex, mCameraSimState);
        GetWorldPhysicsManager()->SubmitAnimCollisionsPseudoCallback( pos, mActiveSuperCam->GetCollisionRadius(), mCollisionAreaIndex, mCameraSimState );
#ifdef RAD_WIN32
        if ( camType == SuperCam::PC_CAM )
        {
            //No dynamics in PC_CAM
            return;
        }
#endif
        GetWorldPhysicsManager()->SubmitDynamicsPseudoCallback( pos, mActiveSuperCam->GetCollisionRadius(), mCollisionAreaIndex, mCameraSimState );
    }
}

//=============================================================================
// SuperCamCentral::UpdateForPhysics
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::UpdateForPhysics( unsigned int milliseconds )
{
    if ( mActiveSuperCam )
    {
        //Inform the supercam of the physics collision


        // hi Cary
        // at this point you should now also have a 
        // rmt::Vector mCameraTerrainCollisionOffsetFix
        // that will be something other than 0,0,0 if the camera sphere hit the terrain
        //
        // use as you please.    

        mActiveSuperCam->SetCollisionOffset( mCameraCollisionOffsetFix, mCameraCollisionCount, mCameraTerrainCollisionOffsetFix );
        mActiveSuperCam->UpdateForPhysics( milliseconds );

#ifdef SUPERCAM_DEBUG
        //This is the debug system to allow the game to "see" what the active supercam sees.
        if ( mDebugViewOn )
        {
            //Position the debug cam relative to the actual cam
            rmt::Vector newPos, camPos;
            mCamera->GetPosition( &camPos );

            rmt::SphericalToCartesian( mDebugMagnitude, mDebugXZAngle, mDebugYAngle, 
                &newPos.x, &newPos.z, &newPos.y );
            newPos.Add( camPos );
            mDebugCamera->SetPosition( newPos );
            mDebugCamera->SetTarget( camPos );
#ifdef DEBUGWATCH
            //Draw the cam's frustrum.
            DrawFrustrum( );
#endif
        }
#endif
    }
}

//=============================================================================
// SuperCamCentral::AllowCameraToggle
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool SuperCamCentral::AllowCameraToggle()
{
    if ( mActiveSuperCam == NULL )
    {
        return true;
    }

    //In these modes do not allow the camera to be toggled.
    SuperCam::Type type = mActiveSuperCam->GetType();
    
    return !( type == SuperCam::RAIL_CAM || type == SuperCam::STATIC_CAM || type == SuperCam::ANIMATED_CAM );
}

//=============================================================================
// SuperCamCentral::AllowAutoCameraChange
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool SuperCamCentral::AllowAutoCameraChange()
{
    if ( mActiveSuperCam == NULL )
    {
        return true;
    }

    SuperCam::Type type = mActiveSuperCam->GetType();

    return ( type == SuperCam::FOLLOW_CAM || 
             type == SuperCam::NEAR_FOLLOW_CAM || 
             type == SuperCam::FAR_FOLLOW_CAM || 
             type == SuperCam::WALKER_CAM || 
#ifdef RAD_WIN32
             type == SuperCam::PC_CAM || 
#endif
             type == SuperCam::COMEDY_CAM ||
             type == SuperCam::BUMPER_CAM ||
             type == SuperCam::RAIL_CAM ||
             type == SuperCam::STATIC_CAM ||
             type == SuperCam::SPLINE_CAM );
}

//=============================================================================
// SuperCamCentral::RegisterFOVLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( FOVLocator* loc )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::RegisterFOVLocator( FOVLocator* loc )
{
    rAssertMsg( NULL == mCurrentFOVLocator, "There should never be FOV locators that interpenetrate!" );

    mCurrentFOVLocator = loc;
    mCurrentFOVLocator->AddRef();

    if ( mActiveSuperCam )
    {
        mActiveSuperCam->SetFOVOverride( loc->GetFOV() );
        mActiveSuperCam->OverrideFOV( true, loc->GetTime(), loc->GetRate() );
    }
}

//=============================================================================
// SuperCamCentral::UnregisterFOVLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::UnregisterFOVLocator()
{
    rAssert( mCurrentFOVLocator );

    if ( mActiveSuperCam )
    {
        mActiveSuperCam->OverrideFOV( false, mCurrentFOVLocator->GetTime(), mCurrentFOVLocator->GetRate() );
    }

    mCurrentFOVLocator->Release();
    mCurrentFOVLocator = NULL;
}

//=============================================================================
// SuperCamCentral::NoTransition
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::NoTransition()
{
    if ( mNextSuperCam.incoming )
    {
        mNextSuperCam.flags |= FORCE;
    }
    else
    {
        if( mActiveSuperCam != NULL )
        {
            mActiveSuperCam->EndTransition();
        }
    }
}

//=============================================================================
// SuperCamCentral::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_ENTER_INTERIOR_END:
        {
            DoCameraCut();
            break;
        }
    case EVENT_CAMERA_SHAKE:
        {
            ShakeEventData* shakeData = static_cast<ShakeEventData*>(pEventData);
            if ( shakeData->playerID == mMyNumber && GetGameplayManager()->GetGameType() == GameplayManager::GT_NORMAL )
            {
                //Do the shaking.
                mActiveSuperCam->EnableShake();
                mActiveSuperCam->SetCameraShakerData( shakeData );
            }
            break;
        }
    case EVENT_LOCATOR + LocatorEvent::CAMERA_CUT:
        {
            EventLocator* evtLoc = static_cast<EventLocator*>(pEventData);
            if ( evtLoc->GetPlayerID() == mMyNumber )
            {
                //Only switch to Wreckless cam in demo mode.
                if ( GetGameplayManager()->mIsDemo )
                {
                    if ( evtLoc->GetPlayerEntered() )
                    {
                        SelectSuperCam( SuperCam::WRECKLESS_CAM, CUT | FORCE, 0 );  //0 means do right away.
                        
                        //Hackish, trigger the cameras event listener.  This is specific to wreckless cams.
                        WrecklessCam* wc = static_cast<WrecklessCam*>(GetSuperCam( SuperCam::WRECKLESS_CAM ));
                        if ( wc )
                        {
                            wc->GetEventListener()->HandleEvent( id, pEventData );
                        }

                        ++mWrecklessCount;
                    }
                    else
                    {
                        if ( mWrecklessCount == 1 )
                        {
                            if ( mTarget->IsCar() )
                            {
                                SelectSuperCam( SuperCam::FOLLOW_CAM, CUT | FORCE );
                            }
                            else
                            {
#ifdef RAD_WIN32
                                SelectSuperCam( SuperCam::ON_FOOT_CAM, CUT | FORCE );
#else
                                SelectSuperCam( SuperCam::WALKER_CAM, CUT | FORCE );
#endif
                            }
                        }

                        mWrecklessCount--;
                    }
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }



/*  else if ( id == EVENT_BURNOUT )
    {
        if ( rand() % mChanceToBurnout == 0 )
        {
            SelectSuperCam( SuperCam::BURNOUT_CAM, true, false, true );
        }
    }
    else if ( id == EVENT_BURNOUT_END )
    {
        SelectSuperCam( SuperCam::FOLLOW_CAM, false, false, true );
    }
*/

}

//=============================================================================
// SuperCamCentral::ToggleFirstPerson
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerID )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::ToggleFirstPerson( int controllerID )
{
    /*
    if ( controllerID == GetInputManager()->GetControllerIDforPlayer( mMyNumber ) )
    {
        if ( mActiveSuperCam && mActiveSuperCam->GetType() == SuperCam::WALKER_CAM )
        {
            //Switch to first person cam.
            SelectSuperCam( SuperCam::FIRST_PERSON_CAM, SuperCamCentral::CUT | SuperCamCentral::FORCE, 0 );
            GetEventManager()->TriggerEvent(EVENT_TOGGLE_FIRSTPERSON, (void*)true); 
        }
        else if ( mActiveSuperCam && mActiveSuperCam->GetType() == SuperCam::FIRST_PERSON_CAM )
        {
            //Switch to walker cam
            SelectSuperCam( SuperCam::WALKER_CAM, SuperCamCentral::CUT | SuperCamCentral::FORCE, 0 );
            GetEventManager()->TriggerEvent(EVENT_TOGGLE_FIRSTPERSON, (void*)false); 
        }
    }
    */
}

//=============================================================================
// SuperCamCentral::IsCutCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool SuperCamCentral::IsCutCam()
{
    return !( mActiveSuperCam != NULL &&
              mActiveSuperCam->GetType() != SuperCam::STATIC_CAM &&
              mActiveSuperCam->GetType() != SuperCam::RAIL_CAM );
}


//*****************************************************************************
// Debug Stuff
//*****************************************************************************

//=============================================================================
// SuperCamCentral::ToggleDebugView
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool on )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::ToggleDebugView( bool on )
{  
#ifdef SUPERCAM_DEBUG
    rAssert( mCamera );
    RenderManager* rm = GetRenderManager();
    RenderLayer* rloutside = rm->mpLayer( RenderEnums::LevelSlot );
    rAssert( rloutside );
    
    // TO DO: Cary, this isn't good... do we care which player?
    tView* viewOutside = rloutside->pView( 0 );

    if ( on && !mDebugViewOn )
    {
        //Turn on
        viewOutside->SetCamera( mDebugCamera );

        rloutside->AddGuts( mFrustrumDrawable );
        
        mFrustrumDrawable->Enable();
        
        mDebugXZAngle = DEBUG_CAM_XZ_ANGLE;
        mDebugYAngle = DEBUG_CAM_Y_ANGLE;
        mDebugMagnitude = DEBUG_CAM_DIST;
    }
    else if ( !on && mDebugViewOn )
    {
        //Turn off
        viewOutside->SetCamera( mCamera );

        rloutside->RemoveGuts( mFrustrumDrawable );

        mFrustrumDrawable->Disable();
    }

    mDebugViewOn = on;
#endif
}

//=============================================================================
// SuperCamCentral::ToggleDebugView
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::ToggleDebugView()
{
#ifdef SUPERCAM_DEBUG
    ToggleDebugView( !mDebugViewOn );
#endif
}
        
#ifdef DEBUGWATCH
//=============================================================================
// SuperCamCentral::DrawFrustrum
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::DrawFrustrum()
{
    rmt::Vector wnp[4], wfp[4];
    rmt::Vector temp;
    rmt::Vector realPos;
    const tColour colour( tColour( 255, 255, 255 ) );

    mCamera->GetPosition( &realPos );

    float tempFP  = mCamera->GetFarPlane();

    mCamera->SetFarPlane(200.0f);
    mCamera->ViewToWorld( rmt::Vector( -0.5f, 0.5f, 0 ), &wnp[0], &wfp[0] );
    mCamera->ViewToWorld( rmt::Vector( 0.5f, 0.5f, 0 ), &wnp[1], &wfp[1] );
    mCamera->ViewToWorld( rmt::Vector( 0.5f, -0.5f, 0 ), &wnp[2], &wfp[2] );
    mCamera->ViewToWorld( rmt::Vector( -0.5f, -0.5f, 0 ), &wnp[3], &wfp[3] );

    mCamera->SetFarPlane(tempFP);
    //This is hard coded line drawing.
    mFrustrumDrawable->SetColour( colour );
    mFrustrumDrawable->SetPoints( mCameraCollisionVolume->mPosition, realPos, wfp[0], wfp[1], wfp[2], wfp[3] );
    mFrustrumDrawable->SetScale( mActiveSuperCam->GetCollisionRadius() );
    mFrustrumDrawable->SetSuperCam( mActiveSuperCam );
}
#endif

//=============================================================================
// SuperCamCentral::LoadData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const GameDataByte* dataBuffer, unsigned int numBytes )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::LoadData( const GameDataByte* dataBuffer, unsigned int numBytes )
{
    GameDataByte bitmask = dataBuffer[ 0 ];

    /*
     *  BITS:       7654 3210
     *
     *	bitmask = [ **** ***? ] = jump cameras on/off flag
     *	bitmask = [ **** **?* ] = inverted camera on/off flag
     *	bitmask = [ ???? ??** ] = preferred follow camera type
     *
     */

    mJumpCamsEnabled =          ( (bitmask & 0x01) > 0 );
    mIsInvertedCameraEnabled =  ( (bitmask & 0x02) > 0 );

    GameDataByte preferredFollowCam = (bitmask >> 2);
    if( preferredFollowCam == 0 )
    {
        // this is for backward-compatibility w/ old saved game formats
        // that didn't have the preferred follow cam data
        //
        mPreferredFollowCam = SuperCam::FAR_FOLLOW_CAM;
    }
    else
    {
        mPreferredFollowCam = static_cast<SuperCam::Type>( preferredFollowCam );
    }
}

//=============================================================================
// SuperCamCentral::SaveData
//=============================================================================
// Description: Comment
//
// Parameters:  ( GameDataByte* dataBuffer, unsigned int numBytes )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::SaveData( GameDataByte* dataBuffer, unsigned int numBytes )
{
    GameDataByte bitmask = 0x00;

    /*
     *  BITS:       7654 3210
     *
     *	bitmask = [ **** ***? ] = jump cameras on/off flag
     *	bitmask = [ **** **?* ] = inverted camera on/off flag
     *	bitmask = [ ???? ??** ] = preferred follow camera type
     *
     */

    if( mJumpCamsEnabled )
    {
        bitmask |= 0x01;
    }

    if( mIsInvertedCameraEnabled )
    {
        bitmask |= 0x02;
    }

    GameDataByte preferredFollowCam = static_cast<GameDataByte>( mPreferredFollowCam );
    rAssertMsg( (preferredFollowCam & 0xC0) == 0,
                "Not enough bits to save preferred follow cam data!" );

    bitmask |= (preferredFollowCam << 2);

    dataBuffer[ 0 ] = bitmask;
}

//=============================================================================
// SuperCamCentral::ResetData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::ResetData()
{
    mJumpCamsEnabled = true;
    mIsInvertedCameraEnabled = false;
    mPreferredFollowCam = FollowCam::FAR_FOLLOW_CAM;
}

//=============================================================================
// SuperCamCentral::IsLegalType
//=============================================================================
// Description: Comment
//
// Parameters:  ( SuperCam::Type type )
//
// Return:      bool 
//
//=============================================================================
bool SuperCamCentral::IsLegalType( SuperCam::Type type )
{
	if( mTarget == NULL )
	{
		return false;
	}

    if ( type == SuperCam::STATIC_CAM ||
         type == SuperCam::RAIL_CAM ||
         type == SuperCam::CONVERSATION_CAM ||
         (mTarget != NULL && !mTarget->IsCar() && type == SuperCam::FIRST_PERSON_CAM ) ||
         (mTarget != NULL && mTarget->IsCar() && type == SuperCam::REVERSE_CAM) ||
         type == SuperCam::ANIMATED_CAM ||
         type == SuperCam::RELATIVE_ANIMATED_CAM ||
         type == SuperCam::SUPER_SPRINT_CAM )
    {
        return true;
    }

    SuperCam::Type* cameras = NULL;
    unsigned int numCameras = 0;

    if( mTarget != NULL && mTarget->IsCar() )
    {
        if ( GetGameplayManager()->IsSuperSprint() )
        {
            cameras = SUPER_SPRINT_CAMERAS;
            numCameras = NUM_SUPERSPRINT_CAMS;
        }
        else
        {
            cameras = CAMERAS_FOR_DRIVING;

            if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_CAMERAS ) )
            {
                numCameras = NUM_CAMERAS_FOR_DRIVING;
            }
            else
            {
                numCameras = NUM_CAMERAS_FOR_DRIVING_WITHOUT_CHEAT;
            }
        }
    }
    else
    {
        cameras = CAMERAS_FOR_WALKING;

        if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_CAMERAS ) )
        {
            numCameras = NUM_CAMERAS_FOR_WALKING;
        }
        else
        {
            numCameras = NUM_CAMERAS_FOR_WALKING_WITHOUT_CHEAT;
        }
    }

    unsigned int i;
    for ( i = 0; i < numCameras; ++i )
    {
        if ( cameras[ i ] == type )
        {
            return true;
        }
    }

    return false;
}

//=============================================================================
// SuperCamCentral::GetNewFollowCamDataChunk
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      FollowCamDataChunk
//
//=============================================================================
FollowCamDataChunk& SuperCamCentral::GetNewFollowCamDataChunk()
{
    rAssert( mNumUsedFDC < MAX_DATA_CHUNKS );

    if ( mNumUsedFDC < MAX_DATA_CHUNKS )
    {
        FollowCamDataChunk& chunk = mFollowCamDataChunks[ mNumUsedFDC ];
        ++mNumUsedFDC;
        return chunk;
    }

    return mFollowCamDataChunks[ 0 ];  //This is BAD, but not fatal
}

//=============================================================================
// SuperCamCentral::ReleaseFollowCamDataChunk
//=============================================================================
// Description: Comment
//
// Parameters:  ( FollowCamDataChunk& chunk )
//
// Return:      void 
//
//=============================================================================
void SuperCamCentral::ReleaseFollowCamDataChunk( FollowCamDataChunk& chunk )
{
    unsigned int i;
    for ( i = 0; i < mNumUsedFDC; ++i )
    {
        if ( mFollowCamDataChunks[ i ] == chunk )
        {
            mFollowCamDataChunks[ i ] = mFollowCamDataChunks[ mNumUsedFDC - 1 ];
            mNumUsedFDC--;
            return;
        }
    }
}

//=============================================================================
// SuperCamCentral::FindFCD
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int id )
//
// Return:      FollowCamDataChunk*
//
//=============================================================================
FollowCamDataChunk* SuperCamCentral::FindFCD( unsigned int id )
{
    unsigned int i;
    for ( i = 0; i < mNumUsedFDC; ++i )
    {
        if ( mFollowCamDataChunks[ i ].mID == id )
        {
            return &mFollowCamDataChunks[ i ];
        }
    }

    return NULL;
}


//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
 
