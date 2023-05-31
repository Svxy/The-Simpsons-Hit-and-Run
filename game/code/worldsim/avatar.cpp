//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:
//
// Description: Implementation of class Avatar
//
// History:     4/3/2002 + Created -- TBJ
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <worldsim/avatar.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactertarget.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/charactermappable.h>
#include <worldsim/character/charactercontroller.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/vehiclecontroller/vehiclecontroller.h>
#include <worldsim/redbrick/vehiclecontroller/humanvehiclecontroller.h>
#include <worldsim/redbrick/vehiclecontroller/vehiclemappable.h>
#include <input/inputmanager.h>

#include <ai/vehicle/vehicleai.h>

#include <roads/geometry.h>
#include <roads/road.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <roads/intersection.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>

#include <gameflow/gameflow.h>
#include <supersprint/supersprintmanager.h>
#include <worldsim/traffic/trafficmanager.h>
//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// If true, display avatar coordinates on screen
//
bool Avatar::s_displayCoordinates = false;

struct AvatarInputManager
{
    AvatarInputManager( void )
        :
    mVehicleMappableHandle( -1 ),
#ifdef RAD_PS2
    mVehicleMappableHandleWheel0( -1 ),
    mVehicleMappableHandleWheel1( -1 ),
#endif
    mCharacterMappableHandle( -1 )
    {
        HeapMgr()->PushHeap (GMA_LEVEL_OTHER);

        mpVehicleMappable = new VehicleMappable;
        mpVehicleMappable->AddRef();

#ifdef RAD_PS2
        mpVehicleMappableUSB0 = new VehicleMappable;
        mpVehicleMappableUSB0->AddRef();

        mpVehicleMappableUSB1 = new VehicleMappable;
        mpVehicleMappableUSB1->AddRef();
#endif
        
        mpHumanVehicleController = new HumanVehicleController;
        mpHumanVehicleController->AddRef();
        
        mpInCarCharacterMappable = new InCarCharacterMappable;
        mpInCarCharacterMappable->AddRef();

        mpBipedCharacterMappable = new BipedCharacterMappable;
        mpBipedCharacterMappable->AddRef();

        mpCameraRelativeCharacterController = new CameraRelativeCharacterController;
        mpCameraRelativeCharacterController->AddRef();

        HeapMgr()->PopHeap (GMA_LEVEL_OTHER);
    }
    ~AvatarInputManager( void )
    {
        mpVehicleMappable->ReleaseController();
        mpVehicleMappable->Release();

#ifdef RAD_PS2
        mpVehicleMappableUSB0->Release();
        mpVehicleMappableUSB1->Release();
#endif

        mpHumanVehicleController->ReleaseVehicleMappable();
        mpHumanVehicleController->Release();

        mpInCarCharacterMappable->SetCharacterController( NULL );
        mpInCarCharacterMappable->Release();

        mpBipedCharacterMappable->SetCharacterController( NULL );
        mpBipedCharacterMappable->Release();

        mpCameraRelativeCharacterController->SetCamera (0);
        mpCameraRelativeCharacterController->Release();

        int x  =5;
    }
    VehicleMappable* mpVehicleMappable;
    VehicleMappable* mpVehicleMappableUSB0;
    VehicleMappable* mpVehicleMappableUSB1;
    HumanVehicleController* mpHumanVehicleController;
    InCarCharacterMappable* mpInCarCharacterMappable;
    BipedCharacterMappable* mpBipedCharacterMappable;
    CameraRelativeCharacterController* mpCameraRelativeCharacterController;
    int mVehicleMappableHandle;
    int mVehicleMappableHandleWheel0;
    int mVehicleMappableHandleWheel1;
    int mCharacterMappableHandle;
};


void UnRegisterMappableHandle( int controllerId, int& handle )
{
    if ( handle > -1 )
    {
        // Detach the mappable.
        //
        InputManager::GetInstance( )->UnregisterMappable( controllerId, handle );
        handle = -1;
    }
}
//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Avatar::Avatar
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
Avatar::Avatar()
:
mHasBeenUpdatedThisFrame( false ),
mLastRoadSegment( NULL ),
mLastRoadSegmentT( 0.0f ),
mLastRoadT( 0.0f ),
mControllerId( INVALID_CONTROLLER ),
mPlayerId( 0 ),
mpCharacter( 0 ),
mpVehicle( 0 ),
mpAvatarInputManager( 0 )
{
    mLastPathElement.elem = NULL;

    mpAvatarInputManager = new AvatarInputManager;

    GetCheatInputSystem()->RegisterCallback( this );

    //
    // Just in case the cheat is set before object creation
    //
    if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_SHOW_AVATAR_POSITION ) )
    {
        s_displayCoordinates = true;
    }
}

//==============================================================================
// Avatar::~Avatar
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
Avatar::~Avatar()
{
    Destroy( );

    if ( mpAvatarInputManager )
    {
        delete mpAvatarInputManager;
        mpAvatarInputManager = 0;
    }

    GetCheatInputSystem()->UnregisterCallback( this );
}

void Avatar::Destroy( void )
{

    if ( mpCharacter )
    {
        mpCharacter->SetController( NULL );
        mpCharacter->Release( );
        mpCharacter = 0;
    }
    if ( mpVehicle )
    {
        mpVehicle->Release( );
        mpVehicle = 0;
    }
    UnRegisterMappableHandle( mControllerId, mpAvatarInputManager->mCharacterMappableHandle );
    UnRegisterMappableHandle( mControllerId, mpAvatarInputManager->mVehicleMappableHandle );
#ifdef RAD_PS2
    if ( mpAvatarInputManager->mVehicleMappableHandleWheel0 != -1 )
    {
        UnRegisterMappableHandle( Input::USB0, mpAvatarInputManager->mVehicleMappableHandleWheel0 );
    }

    if ( mpAvatarInputManager->mVehicleMappableHandleWheel1 != -1 )
    {
        UnRegisterMappableHandle( Input::USB1, mpAvatarInputManager->mVehicleMappableHandleWheel1 );
    }
#endif
    mControllerId = INVALID_CONTROLLER;
}

/*
==============================================================================
Avatar::GetControllerId
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         int

=============================================================================
*/
int Avatar::GetControllerId( void ) const
{
    return mControllerId;
}

/*
==============================================================================
Avatar::SetControllerId
==============================================================================
Description:    Comment

Parameters:     ( int id )

Return:         void

=============================================================================
*/
void Avatar::SetControllerId( int id )
{
    mControllerId = id;
}
/*
==============================================================================
Avatar::GetCharacter
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         const

=============================================================================
*/
Character* Avatar::GetCharacter( void ) const
{
    return mpCharacter;
}

/*
==============================================================================
Avatar::SetCharacter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void

=============================================================================
*/
void Avatar::SetCharacter( Character* pCharacter )
{
    tRefCounted::Assign( mpCharacter, pCharacter );
    if( mpCharacter )
    {
        tColour shadowColour = ::GetGameplayManager()->GetControllerColour( mControllerId );
        mpCharacter->SetShadowColour( shadowColour );
    }
}

/*
==============================================================================
Avatar::GetVehicle
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         const

=============================================================================
*/
Vehicle* Avatar::GetVehicle( void ) const
{
    return mpVehicle;
}

/*
==============================================================================
Avatar::SetVehicle
==============================================================================
Description:    Comment

Parameters:     ( Vehicle* pVehicle )

Return:         void

=============================================================================
*/
void Avatar::SetVehicle( Vehicle* pVehicle )
{
    tRefCounted::Assign( mpVehicle, pVehicle );

    if( pVehicle )
    {
        int playerID = 0; // normal game defaults player to zero
        if( GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_SUPERSPRINT )
        {
            playerID = SuperSprintManager::GetInstance()->GetOnlyHumanPlayerID();
            if( this->mPlayerId != playerID )
            {
                return;
            }
        }

        // can be -1 if in supersprint and more than 1 human is participating...
        if( playerID == -1 ) 
        {
            return;
        }

        SuperCam* sc = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam();
        if ( sc && sc->GetType() == SuperCam::BUMPER_CAM )
        {
            //Only stop rendering for bumpercams.
            pVehicle->DrawVehicle( false );
        }
        else
        {
            pVehicle->DrawVehicle( true );
        }
    }
}

/*
==============================================================================
Avatar::GetIntoVehicle
==============================================================================
Description:    Pseudo code.

                1. Get a "human" vehicle controller.
                2. Init HumanVehicleController with mControllerId.
                3. Register with ControllerSystem.
                4. Get pointer to Vehicle via VehicleManager using vehicleId.
                5. Set Avatar member data mpVehicle to Vehicle pointer from 4.
                6. Set Controller for Vehicle.


Parameters:     ( Vehicle* pVehicle)

Return:         void

=============================================================================
*/
void Avatar::SetInCarController( void )
{ 
    rAssert( mpVehicle );
    mpVehicle->mGeometryVehicle->FadeRoof( true );
    mpVehicle->mGeometryVehicle->EnableLights( true );
    mpVehicle->SetUserDrivingCar( true );
    mpVehicle->SetDisableGasAndBrake(false);
    
    // in case we just got into a traffi car, make sure brake lights are off
    mpVehicle->mGeometryVehicle->HideBrakeLights();
    mpVehicle->mBrakeLightsOn = false;        
    mpVehicle->mGeometryVehicle->HideReverseLights();
    mpVehicle->mReverseLightsOn = false;
    
    int vehicleId = GetVehicleCentral( )->GetVehicleId( mpVehicle );
    // Detach the vehicle controller.
    //
    UnRegisterMappableHandle( mControllerId, mpAvatarInputManager->mVehicleMappableHandle );


    HumanVehicleController* pVehicleController = mpAvatarInputManager->mpHumanVehicleController;
    
    VehicleMappable* pVehicleMappable = mpAvatarInputManager->mpVehicleMappable;
    pVehicleController->Create( mpVehicle, pVehicleMappable, mControllerId );
    pVehicleMappable->SetController( pVehicleController );

    mpAvatarInputManager->mVehicleMappableHandle = InputManager::GetInstance( )->RegisterMappable( mControllerId, pVehicleMappable );

    bool wheelAttached = false;
#ifdef RAD_PS2
    if ( GetGameplayManager()->GetGameType() != GameplayManager::GT_SUPERSPRINT )
    {
        if ( mControllerId == Input::USB0 ||
            ( mControllerId != Input::USB0 && 
              mControllerId != Input::USB1 && 
              GetInputManager()->IsControllerInPort( Input::USB0 )
            ) 
           )
        {
            mpAvatarInputManager->mVehicleMappableHandleWheel0 = InputManager::GetInstance( )->RegisterMappable( Input::USB0, mpAvatarInputManager->mpVehicleMappableUSB0 );
            pVehicleController->SetWheel( mpAvatarInputManager->mpVehicleMappableUSB0, 0 );
            mpAvatarInputManager->mpVehicleMappableUSB0->SetController( pVehicleController );
            GetInputManager()->SetRumbleForDevice( Input::USB0, GetInputManager()->IsRumbleEnabled() );
            wheelAttached = true;
        }
        else if ( mControllerId == Input::USB1 ||
                  ( mControllerId != Input::USB0 && 
                    mControllerId != Input::USB1 && 
                    GetInputManager()->IsControllerInPort( Input::USB1 ) 
                  ) 
                ) 
        {
            mpAvatarInputManager->mVehicleMappableHandleWheel1 = InputManager::GetInstance( )->RegisterMappable( Input::USB1, mpAvatarInputManager->mpVehicleMappableUSB1 );
            pVehicleController->SetWheel( mpAvatarInputManager->mpVehicleMappableUSB1, 1 );
            mpAvatarInputManager->mpVehicleMappableUSB1->SetController( pVehicleController );
            GetInputManager()->SetRumbleForDevice( Input::USB1, GetInputManager()->IsRumbleEnabled() );
            wheelAttached = true;
        }
    }
#endif

    if ( !wheelAttached )
    {
        GetInputManager()->SetRumbleForDevice( mControllerId, GetInputManager()->IsRumbleEnabled() );
    }

    if ( !GetGameplayManager()->mIsDemo )
    {
        GetVehicleCentral( )->SetVehicleController( vehicleId, pVehicleController );
    }

    // First things, detach the in car character controller.
    //
    UnRegisterMappableHandle( mControllerId, mpAvatarInputManager->mCharacterMappableHandle );

    CharacterMappable* pCharacterMappable = mpAvatarInputManager->mpInCarCharacterMappable;
    CameraRelativeCharacterController* pCharacterController = mpAvatarInputManager->mpCameraRelativeCharacterController;
    pCharacterController->Create( mpCharacter, pCharacterMappable );

    mpAvatarInputManager->mCharacterMappableHandle = InputManager::GetInstance()->RegisterMappable( mControllerId, pCharacterMappable );
    mpCharacter->SetController( pCharacterController );
 
    // new:
    // for changing the physics/collision representation if applicabble
    mpVehicle->SetInCarSimState();
    
    mpVehicle->BeefUpHitPointsOnTrafficCarsWhenUserDriving();
    

}

void Avatar::SetCameraTargetToVehicle( bool cut ) 
{
    // Set the camera target.
    //
    GetSuperCamManager()->GetSCC( mPlayerId )->SetTarget( mpVehicle );

    // Select the follow cam.
    //
    GetSuperCamManager()->GetSCC( mPlayerId )->SelectSuperCam( SuperCam::FOLLOW_CAM, cut ? (SuperCamCentral::CUT|SuperCamCentral::FORCE ) : 0, cut ? 0 : 7000 );
}

void Avatar::GetIntoVehicleStart( Vehicle* pVehicle)
{
    SetVehicle( pVehicle );
    
    // check to see if the target vehicle is currently controlled by traffic
    // if it is, we need to work a little harder to carjack it :-)
    if(pVehicle->GetLocomotionType() == VL_TRAFFIC)
    {
        pVehicle->SetLocomotion(VL_PHYSICS);
        pVehicle->mHijackedByUser = true;
    }

    if(GetSuperCamManager()->GetSCC( mPlayerId )->GetPreferredFollowCam() != SuperCam::BUMPER_CAM)
    {
        SetCameraTargetToVehicle( );
    }

    CGuiScreenMultiHud* currentHUD = GetCurrentHud();
    if( currentHUD != NULL )
    {
        currentHUD->GetHudMap( mPlayerId )->UnregisterIcon( 0 );
        currentHUD->GetHudMap( mPlayerId )->RegisterIcon( HudMapIcon::ICON_PLAYER,
                                                            rmt::Vector( 0, 0, 0 ),
                                                            pVehicle );
    }

    UnRegisterMappableHandle( mControllerId, mpAvatarInputManager->mCharacterMappableHandle );
    
    
}

void Avatar::GetIntoVehicleEnd( Vehicle* pVehicle)
{
    SetCameraTargetToVehicle( );

    SetInCarController( );
    // this call is to record the rest seating positions of the driver and passenger
    pVehicle->RecordRestSeatingPositionsOnEntry();
}

/*
==============================================================================
Avatar::GetOutOfVehicle
==============================================================================
Description:    Pseudo code
                2. Set Controller for Vehicle to NULL ( or AI controller )
                3. Set Vehicle member data to NULL.
                4. Unregister controller with controller system.

Parameters:     ( Vehicle* pVehicle)

Return:         void

=============================================================================
*/
void Avatar::GetOutOfVehicleStart( Vehicle* pVehicle)
{    
    // First things, detach the in car character controller.
    //
    UnRegisterMappableHandle( mControllerId, mpAvatarInputManager->mCharacterMappableHandle );
    
    // Detach the vehicle controller.
    UnRegisterMappableHandle( mControllerId, mpAvatarInputManager->mVehicleMappableHandle );
#ifdef RAD_PS2
    if ( mpAvatarInputManager->mVehicleMappableHandleWheel0 != -1 )
    {
        UnRegisterMappableHandle( Input::USB0, mpAvatarInputManager->mVehicleMappableHandleWheel0 );
        GetInputManager()->SetRumbleForDevice( Input::USB0, false );
    }

    if ( mpAvatarInputManager->mVehicleMappableHandleWheel1 != -1 )
    {
        UnRegisterMappableHandle( Input::USB1, mpAvatarInputManager->mVehicleMappableHandleWheel1 );
        GetInputManager()->SetRumbleForDevice( Input::USB1, false );
    }
#endif

    // attach the on-foot controller 
    // we do this early (don't wait for end of get out of car) so that we can abort \
    // the door close if there is movement)
    SetOutOfCarController();

    int vehicleId = GetVehicleCentral()->GetVehicleId( pVehicle, false );

    if ( !GetGameplayManager()->mIsDemo && (vehicleId != -1))
    {
        GetVehicleCentral( )->SetVehicleController( vehicleId, 0 );
    }

    if(GetSuperCamManager()->GetSCC( mPlayerId )->GetPreferredFollowCam() == SuperCam::BUMPER_CAM)
    {
        SetCameraTargetToCharacter();
    }
}

void Avatar::SetOutOfCarController( void )
{
    if( GetVehicle() )
    {
        GetVehicle()->mGeometryVehicle->FadeRoof( false );

        if( !TrafficManager::GetInstance()->IsVehicleTrafficVehicle( GetVehicle() ) )
        {
            GetVehicle()->mGeometryVehicle->EnableLights( false );
        }
        
        GetVehicle()->SetOutOfCarSimState();

        GetVehicle()->SetUserDrivingCar( false );
    }

    // Then, make a new character controller.
    //
    CharacterMappable* pCharacterMappable = mpAvatarInputManager->mpBipedCharacterMappable;
    CameraRelativeCharacterController* pCharacterController = mpAvatarInputManager->mpCameraRelativeCharacterController;
    pCharacterController->Create( mpCharacter, pCharacterMappable );

    // Register the new character controller.
    //
    mpAvatarInputManager->mCharacterMappableHandle = InputManager::GetInstance()->RegisterMappable( mControllerId, pCharacterMappable );
    mpCharacter->SetController( pCharacterController );

    pCharacterController->SetCamera( GetSuperCamManager()->GetSCC( mPlayerId )->GetCamera() );

    GetInputManager()->SetRumbleForDevice( mControllerId, false );
}

void Avatar::SetCameraTargetToCharacter( bool cut )
{
    // Set the camera target.
    //
    GetSuperCamManager()->GetSCC( mPlayerId )->SetTarget( mpCharacter->GetTarget( ) );
    // Select the follow cam.
    //
#ifdef RAD_WIN32
    GetSuperCamManager()->GetSCC( mPlayerId )->SelectSuperCam( SuperCam::ON_FOOT_CAM, cut ? SuperCamCentral::CUT : 0 );
#else
    GetSuperCamManager()->GetSCC( mPlayerId )->SelectSuperCam( SuperCam::WALKER_CAM, cut ? SuperCamCentral::CUT : 0 );
#endif
}

void Avatar::GetOutOfVehicleEnd( Vehicle* pVehicle)
{
    pVehicle->DrawVehicle(true);    // just in case it was in bumper cam

    SetCameraTargetToCharacter();

    SetVehicle( 0 );

    CGuiScreenMultiHud* currentHUD = GetCurrentHud();
    if( currentHUD != NULL )
    {
        currentHUD->GetHudMap( mPlayerId )->UnregisterIcon( 0 );
        currentHUD->GetHudMap( mPlayerId )->RegisterIcon( HudMapIcon::ICON_PLAYER,
                                                          rmt::Vector( 0, 0, 0 ),
                                                          mpCharacter->GetTarget() );
    }
}
 /*
==============================================================================
Avatar::IsInCar
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool

=============================================================================
*/
bool Avatar::IsInCar( void ) const
{
    rAssert( mpCharacter );
    return mpCharacter->IsInCar( ) && (mpVehicle != NULL);
}


const void Avatar::GetPosition( rmt::Vector& pos ) const
{
    if( this->IsInCar() )
    {
        mpVehicle->GetPosition( &pos );
    }
    else
    {
        mpCharacter->GetPosition( pos );
    }
}

const void Avatar::GetHeading( rmt::Vector& irHeading ) const
{
    if( this->IsInCar() )
    {
        mpVehicle->GetHeading( &irHeading );
    }
    else
    {
        mpCharacter->GetFacing( irHeading );
    }
}

const void Avatar::GetNormalizedHeadingSafe( rmt::Vector& heading ) const
{
    if( this->IsInCar() )
    {
        assert( mpVehicle != NULL );
        mpVehicle->GetVelocity( &heading );
        float speedMps = mpVehicle->GetSpeedKmh() * KPH_2_MPS;
        if( rmt::Fabs(speedMps) < 0.00001f )
        {
            heading.Set( 0.0f, 0.0f, 0.0f );
        }
        else
        {
            heading.Scale( 1.0f/speedMps );
        }
    }
    else
    {
        // for characters, facing is same as heading (no skidding/sliding)
        // I hope...
        assert( mpCharacter != NULL );
        mpCharacter->GetFacing( heading );
    }
    assert( rmt::Epsilon( heading.MagnitudeSqr(), 1.0f, 0.00001f ) ||
            rmt::Epsilon( heading.MagnitudeSqr(), 0.0f, 0.00001f ) );
}

const void Avatar::GetVelocity( rmt::Vector& vel ) const
{
    if( this->IsInCar() )
    {
        assert( mpVehicle != NULL );
        mpVehicle->GetVelocity( &vel );
    }
    else 
    {
        // Not sure here if the Choreo puppet's velocity value
        // (which is what Character::GetVelocity() retrieves)
        // is a good one. So we just track our own.
        assert( mpCharacter != NULL );
        mpCharacter->GetVelocity( vel );
    }
}

const float Avatar::GetSpeedMps() const
{
    if( this->IsInCar() )
    {
        assert( mpVehicle != NULL );
        return mpVehicle->GetSpeedKmh() * KPH_2_MPS;
    }
    else
    {
        assert( mpCharacter != NULL );
        return mpCharacter->GetSpeed();
    }

}

void Avatar::OnCheatEntered( eCheatID cheatID, bool isEnabled )
{
    if( cheatID == CHEAT_ID_SHOW_AVATAR_POSITION )
    {
        s_displayCoordinates = isEnabled;
    }
}

/*
==============================================================================
Avatar::Update
==============================================================================
Description:    Comment

Parameters:     (float dt)

Return:         void

=============================================================================
*/
void Avatar::Update(float dt)
{
    mHasBeenUpdatedThisFrame = false;
#ifndef FINAL
    char buffy[256];
    rmt::Vector posn;
    const pddiColour YELLOW(255,255,0);

    //
    // Coordinate display cheat
    //
    if( s_displayCoordinates )
    {
        if( mpCharacter != NULL )
        {
            GetPosition( posn );
        }
        else
        {
            posn.Set( 0.0f, 0.0f, 0.0f );
        }
        sprintf( buffy, "Avatar %d position: %f %f %f", mPlayerId, posn.x, posn.y, posn.z );
        p3d::pddi->DrawString( buffy, 40, 40 + ( mPlayerId * 20), YELLOW );
    }
#endif
}

void Avatar::GetLastPathInfo( 
    RoadManager::PathElement& oElem, 
    RoadSegment*& oSeg,
    float& oSegT,
    float& oRoadT )
{
    oElem.elem = NULL;
    oSeg = NULL;
    oSegT = 0.0f;
    oRoadT = 0.0f;

    // don't do it again if already done
    if( !mHasBeenUpdatedThisFrame )
    {
        mHasBeenUpdatedThisFrame = true;
        //////////////////////////////////////////////////////////////////////
        // Update the Avatar's closest path entities in the world.

        rmt::Vector myPos;
        GetPosition( myPos );


        RoadSegment* seg = NULL;
        float segT = 0.0f;
        float roadT = 0.0f;
        RoadManager::PathElement elem;
        elem.elem = NULL;

        /*
        RoadManager::PathfindingOptions options = RoadManager::PO_SEARCH_SHORTCUTS;
        RoadManager::GetInstance()->FindClosestPathElement( 
            pos, 100.0f, options, tmpPathElem, tmpSeg, tmpSegT, tmpRoadT );
        */

        // Special initial case: 
        // If my last path info is completely unset (started off the road)
        // then we fetch the closest path element using Devin's thing 
        if( mLastPathElement.elem == NULL && mLastRoadSegment == NULL )
        {
            RoadSegment* closestSeg = NULL;
            float dummy;
            GetIntersectManager()->FindClosestAnyRoad( myPos, 100.0f, closestSeg, dummy );

            seg = (RoadSegment*) closestSeg;
            segT = RoadManager::DetermineSegmentT( myPos, seg );
            roadT = RoadManager::DetermineRoadT( seg, segT );
            elem.elem = seg->GetRoad();
            elem.type = RoadManager::ET_NORMALROAD;
        }
        else
        {
            VehicleAI::FindClosestPathElement( myPos, elem, seg, segT, roadT, true );
        }

        if( elem.elem == NULL )
        {
            GetEventManager()->TriggerEvent( EVENT_AVATAR_OFF_ROAD );

            oElem = mLastPathElement;
            oSeg = mLastRoadSegment;
            oSegT = mLastRoadSegmentT;
            oRoadT = mLastRoadT;
            return;
        }
        else
        {
            GetEventManager()->TriggerEvent( EVENT_AVATAR_ON_ROAD );
        }

        rAssert( elem.elem != NULL );

        mLastPathElement = elem;
        if( seg )
        {
            mLastRoadSegment = seg;
            mLastRoadSegmentT = segT;
            mLastRoadT = roadT;
        }

        /*
        //
        // true means not to ignore the path element we just found
        // default is true because we could have lastpathelement == elem
        // but we could be on different segments...
        //
        bool OK = true; 

        if( mLastPathElement != elem )
        {
            if( mLastPathElement.elem == NULL )
            {
                OK = true;
            }
            else
            {
                Road* lastRoad = NULL;
                Intersection* lastInt = NULL;
                Road* currRoad = NULL;
                Intersection* currInt = NULL;

                // temporary type: 0 = intersection, 1 = road, 2 = shortcut

                // Figure out what type is LAST
                int lastElemType = 0; 
                if( mLastPathElement.type == RoadManager::ET_NORMALROAD )
                {
                    lastRoad = (Road*) mLastPathElement.elem;
                    lastElemType++;

                    if( lastRoad->GetShortCut() )
                    {
                        lastElemType++;
                    }
                }
                else
                {
                    lastInt = (Intersection*) mLastPathElement.elem;
                }

                // Figure out what type is CURR
                int currElemType = 0;
                if( elem.type == RoadManager::ET_NORMALROAD )
                {
                    currRoad = (Road*) elem.elem;
                    currElemType++;

                    if( currRoad->GetShortCut() )
                    {
                        currElemType++;
                    }
                }
                else
                {
                    currInt = (Intersection*) elem.elem;
                }

                //
                // Start resolving ...
                ///////////////
                // LAST: Intersection
                if( lastElemType == 0 ) 
                {
                    rAssert( lastInt );
                    if( currElemType == 2 )// CURR: shortcut
                    {
                        rAssert( currRoad );
                        if( lastInt == (Intersection*) currRoad->GetSourceIntersection() ||
                            lastInt == (Intersection*) currRoad->GetDestinationIntersection() )
                        {
                            OK = true;
                        }
                        else
                        {
                            OK = false;
                        }
                    }
                    else
                    {
                        OK = true;
                    }
                }
                /////////////////
                // LAST: Normal road 
                else if( lastElemType == 1 ) 
                {
                    rAssert( lastRoad );

                    if( currElemType == 0 ) // CURR: intersection
                    {
                        rAssert( currInt );
                        if( currInt == (Intersection*)lastRoad->GetSourceIntersection() ||
                            currInt == (Intersection*)lastRoad->GetDestinationIntersection() )
                        {
                            OK = true;
                        }
                        else
                        {
                            OK = false;
                        }
                    }
                    else if( currElemType == 1 ) // CURR: normal road
                    {
                        rAssert( currRoad );
                        OK = true;
                    }
                    else // CURR: shortcut
                    {
                        rAssert( currRoad );
                        OK = false;
                    }
                }
                ////////////////
                // LAST: Shortcut
                else
                {
                    rAssert( lastRoad );

                    if( currElemType == 0 ) // CURR: intersection
                    {
                        rAssert( currInt );

					    // NOTE:
					    // This is the fix to Level 3 Mission 7 where the limo drives over the where the shortcuts 
					    // and normal road segments merge near the squidport.... This was where its last good element
					    // was found... It then transitted onto the intersection, but rejected it because back then
					    // we only tested for the destination intersection... So it was stuck there for a long time
					    // but didn't realize it until it hit the next waypoint (wayy... over by the dam) and decided
					    // it needed to turn back. 
                        if( currInt == (Intersection*)lastRoad->GetDestinationIntersection() ||
						    currInt == (Intersection*)lastRoad->GetSourceIntersection() )
                        {
                            OK = true;
                        }
                        else
                        {
                            OK = false;
                        }
                    }
                    else if( currElemType == 1 ) // CURR: normal road
                    {
                        rAssert( currRoad );
                        OK = false;
                    }
                    else // CURR: shortcut
                    {
                        rAssert( currRoad );
                        OK = false;
                    }
                }
            }
        }

        if( OK )
        {
            mLastPathElement = elem;
            if( seg )
            {
                if( mLastRoadSegment != seg )
                {
                    mLastRoadSegment = seg;
                }
                mLastRoadSegmentT = segT;
                mLastRoadT = roadT;
            }
        }
        */
    }

    oElem = mLastPathElement;
    oSeg = (RoadSegment*) mLastRoadSegment;
    oSegT = mLastRoadSegmentT;
    oRoadT = mLastRoadT;
}

void Avatar::GetRaceInfo( float& distToCurrCollectible, int& currCollectible, int& numLapsCompleted )
{
    distToCurrCollectible = mDistToCurrentCollectible;
    currCollectible = mCurrentCollectible;
    numLapsCompleted = mNumLapsCompleted;
}

void Avatar::SetRaceInfo( float distToCurrCollectible, int currCollectible, int numLapsCompleted )
{
    mDistToCurrentCollectible = distToCurrCollectible;
    mCurrentCollectible = currCollectible;
    mNumLapsCompleted = numLapsCompleted;
}

/*
==============================================================================
Avatar::SetCamera
==============================================================================
Description:    Controller needs to be aware of camera changes so that
                direction can be normalized.

Parameters:     ( tCamera* pCamera )

Return:         void

=============================================================================
*/
void Avatar::SetCamera( tCamera* pCamera )
{
    rAssert( pCamera != 0 );
    mpAvatarInputManager->mpCameraRelativeCharacterController->SetCamera( pCamera );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
