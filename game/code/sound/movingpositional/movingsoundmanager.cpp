//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        movingsoundmanager.cpp
//
// Description: Manages the playing of moving sounds
//
// History:     1/4/2003 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/movingpositional/movingsoundmanager.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/avatar/carsoundparameters.h>

#include <events/eventmanager.h>
#include <events/eventdata.h>
#include <ai/actor/actor.h>
#include <worldsim/redbrick/vehicle.h>
#include <constants/vehicleenum.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

tUID MovingSoundManager::s_waspUID = 0;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// MovingSoundManager::MovingSoundManager
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
MovingSoundManager::MovingSoundManager()
{
    EventManager* eventMgr = GetEventManager();

    //
    // Start listening for events
    //
    eventMgr->AddListener( this, EVENT_CHASE_VEHICLE_SPAWNED );
    eventMgr->AddListener( this, EVENT_CHASE_VEHICLE_DESTROYED );
    eventMgr->AddListener( this, EVENT_TRAFFIC_SPAWN );
    eventMgr->AddListener( this, EVENT_TRAFFIC_REMOVE );
    eventMgr->AddListener( this, EVENT_ACTOR_CREATED );
    eventMgr->AddListener( this, EVENT_TRAFFIC_GOT_HIT );
    eventMgr->AddListener( this, EVENT_TRAFFIC_IMPEDED );
    eventMgr->AddListener( this, EVENT_BIG_BOOM_SOUND );
    eventMgr->AddListener( this, EVENT_MISSION_VEHICLE_CREATED );
    eventMgr->AddListener( this, EVENT_MISSION_VEHICLE_RELEASED );
    eventMgr->AddListener( this, EVENT_START_ANIMATION_SOUND );
    eventMgr->AddListener( this, EVENT_STOP_ANIMATION_SOUND );
    eventMgr->AddListener( this, EVENT_START_ANIM_ENTITY_DSG_SOUND );
    eventMgr->AddListener( this, EVENT_STOP_ANIM_ENTITY_DSG_SOUND );
    eventMgr->AddListener( this, static_cast<EventEnum>( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PP_ROOM_2 ) );
    eventMgr->AddListener( this, EVENT_AVATAR_VEHICLE_TOGGLE );

    if( s_waspUID == static_cast< tUID >( 0 ) )
    {
        s_waspUID = tEntity::MakeUID( "beecamera" );
    }

    TrafficSoundPlayer::InitializeClass( NUM_TRAFFIC_SOUND_PLAYERS );
}

//=============================================================================
// MovingSoundManager::~MovingSoundManager
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
MovingSoundManager::~MovingSoundManager()
{
    GetEventManager()->RemoveAll( this );
}

void MovingSoundManager::HandleEvent( EventEnum id, void* pEventData )
{
    AnimSoundData* soundData;
    AnimSoundDSGData* soundDSGData;
    Vehicle* theCar;

    switch( id )
    {
        case EVENT_CHASE_VEHICLE_SPAWNED:
            //
            // New chase AI vehicle, play siren for it
            //
            addAISound( "siren", static_cast<Vehicle*>(pEventData), false );
            break;

        case EVENT_CHASE_VEHICLE_DESTROYED:
            //
            // Chase AI vehicle gone, stop its siren sound
            //
            stopAISound( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_MISSION_VEHICLE_CREATED:
            //
            // New mission AI vehicle, play engine for it
            //
            theCar = static_cast<Vehicle*>(pEventData);
            addAISound( getPositionalSettingName( theCar, true ), theCar, true );
            break;

        case EVENT_MISSION_VEHICLE_RELEASED:
            //
            // Mission AI vehicle gone, stop its engine
            //
            stopAISound( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_TRAFFIC_SPAWN:
            //
            // New traffic vehicle, play engine sound for it
            //
            theCar = static_cast<Vehicle*>(pEventData);
            addTrafficSound( getPositionalSettingName( theCar, false ), theCar, true );
            break;

        case EVENT_TRAFFIC_REMOVE:
            //
            // Traffic vehicle gone
            //
            stopTrafficSound( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_AVATAR_VEHICLE_TOGGLE:
            //
            // Player avatar has gotten in or out of a vehicle.  If it's a traffic
            // vehicle with an overlay clip, we'll need to toggle it on or off.
            //
            toggleOverlayClip( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_ACTOR_CREATED:
            startWaspSound( static_cast<Actor*>(pEventData) );
            break;

        case EVENT_TRAFFIC_GOT_HIT:
        case EVENT_TRAFFIC_IMPEDED:
            handleTrafficHornEvent( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_BIG_BOOM_SOUND:
            makeCarGoBoom( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_START_ANIMATION_SOUND:
            soundData = static_cast<AnimSoundData*>(pEventData);
            if( soundData->animJoint != NULL )
            {
                startPlatformSound( soundData );
            }
            break;

        case EVENT_STOP_ANIMATION_SOUND:
            stopPlatformSound( static_cast<ActionButton::AnimSwitch*>(pEventData) );
            break;

        case EVENT_START_ANIM_ENTITY_DSG_SOUND:
            soundDSGData = static_cast<AnimSoundDSGData*>(pEventData);
            startAnimObjSound( soundDSGData );
            break;

        case EVENT_STOP_ANIM_ENTITY_DSG_SOUND:
            stopAnimObjSound( static_cast<AnimCollisionEntityDSG*>(pEventData) );
            break;

        case EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PP_ROOM_2:
            //
            // If we're hitting this ambience boundary, then we're leaving the platform
            // room, so kill all the platforms and save ourselves some streamers.
            //
            //stopAllPlatforms();
            break;

        default:
            rAssertMsg( false, "Unexpected event in MovingSoundManager\n" );
            break;
    }
}

//=============================================================================
// MovingSoundManager::ServiceOncePerFrame
//=============================================================================
// Description: Do once-per-frame update stuff
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::ServiceOncePerFrame()
{
    int i;

    for( i = 0; i < NUM_TRAFFIC_SOUND_PLAYERS; i++ )
    {
        m_trafficPlayer[i].ServiceOncePerFrame();
    }
    for( i = 0; i < NUM_AI_SOUND_PLAYERS; i++ )
    {
        m_aiPlayer[i].ServiceOncePerFrame();
    }
    for( i = 0; i < NUM_PLATFORM_SOUND_PLAYERS; i++ )
    {
        m_platformPlayer[i].ServiceOncePerFrame();
    }
    for( i = 0; i < NUM_ANIM_OBJ_SOUND_PLAYERS; i++ )
    {
        m_animObjPlayer[i].ServiceOncePerFrame();
    }
    for( i = 0; i < NUM_WASP_SOUND_PLAYERS; i++ )
    {
        m_waspPlayer[i].ServiceOncePerFrame();
    }

    m_avatarVehiclePlayer.ServiceOncePerFrame();

    TrafficSoundPlayer::ServiceTimerList();
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// MovingSoundManager::addTrafficSound
//=============================================================================
// Description: Find an inactive sound player and start it up with the given
//              sound and vehicle
//
// Parameters:  soundName - name of sound resource to play for vehicle
//              vehiclePtr - vehicle whose position the sound will be
//                           played from
//              tiePitchToVelocity - set to true if we want the sound pitch
//                                   to vary (i.e. engine sound)
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::addTrafficSound( const char* soundName, Vehicle* vehiclePtr, bool tiePitchToVelocity )
{
    int i;
    carSoundParameters* carSettings;

    for( i = 0; i < NUM_TRAFFIC_SOUND_PLAYERS; i++ )
    {
        if( !m_trafficPlayer[i].IsActive() )
        {
            m_trafficPlayer[i].ActivateByName( soundName, vehiclePtr );
            m_trafficPlayer[i].TiePitchToVelocity( tiePitchToVelocity );

            //
            // Play overlay clips for traffic if they've got 'em (e.g. quimby truck)
            //
            if( hasOverlayClip( vehiclePtr, &carSettings ) )
            {
                m_trafficPlayer[i].AddOverlayClip( carSettings, soundName );
            }
            break;
        }
    }

#ifdef RAD_DEBUG
    if( i == NUM_TRAFFIC_SOUND_PLAYERS )
    {
        rDebugString( "AI Vehicle sound dropped" );
    }
#endif
}

//=============================================================================
// MovingSoundManager::stopTrafficSound
//=============================================================================
// Description: Stop the sound coming from the given vehicle
//
// Parameters:  vehiclePtr - vehicle whose sound is to be stopped.
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::stopTrafficSound( Vehicle* vehiclePtr )
{
    int i;

    for( i = 0; i < NUM_TRAFFIC_SOUND_PLAYERS; i++ )
    {
        if( m_trafficPlayer[i].UsesVehicle( vehiclePtr ) )
        {
            m_trafficPlayer[i].Deactivate();
        }
    }
}

//=============================================================================
// MovingSoundManager::addAISound
//=============================================================================
// Description: Find an inactive sound player and start it up with the given
//              sound and vehicle
//
// Parameters:  soundName - name of sound resource to play for vehicle
//              vehiclePtr - vehicle whose position the sound will be
//                           played from
//              tiePitchToVelocity - set to true if we want the sound pitch
//                                   to vary (i.e. engine sound)
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::addAISound( const char* soundName, Vehicle* vehiclePtr, bool tiePitchToVelocity )
{
    int i;

    for( i = 0; i < NUM_AI_SOUND_PLAYERS; i++ )
    {
        if( !m_aiPlayer[i].IsActive() )
        {
            m_aiPlayer[i].ActivateByName( soundName, vehiclePtr );
            m_aiPlayer[i].TiePitchToVelocity( tiePitchToVelocity );
            break;
        }
    }

#ifdef RAD_DEBUG
    if( i == NUM_AI_SOUND_PLAYERS )
    {
        rDebugString( "AI Vehicle sound dropped" );
    }
#endif
}

//=============================================================================
// MovingSoundManager::stopAISound
//=============================================================================
// Description: Stop the sound coming from the given vehicle
//
// Parameters:  vehiclePtr - vehicle whose sound is to be stopped.
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::stopAISound( Vehicle* vehiclePtr )
{
    int i;

    for( i = 0; i < NUM_AI_SOUND_PLAYERS; i++ )
    {
        if( m_aiPlayer[i].UsesVehicle( vehiclePtr ) )
        {
            m_aiPlayer[i].Deactivate();
        }
    }
}

//=============================================================================
// MovingSoundManager::handleTrafficHornEvent
//=============================================================================
// Description: Horn the horn for the specified car
//
// Parameters:  vehiclePtr - pointer to vehicle to honk
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::handleTrafficHornEvent( Vehicle* vehiclePtr )
{
    int i;

    rAssert( vehiclePtr != NULL );

    //
    // Find the car matching the vehicle pointer
    //
    for( i = 0; i < NUM_TRAFFIC_SOUND_PLAYERS; i++ )
    {
        if( m_trafficPlayer[i].UsesVehicle( vehiclePtr ) )
        {
            m_trafficPlayer[i].HonkHorn();
        }
    }
}

//=============================================================================
// MovingSoundManager::makeCarGoBoom
//=============================================================================
// Description: If the vehicle in the explosion event matches one of our
//              vehicles, play a positional explosion
//
// Parameters:  vehiclePtr - pointer to exploding car
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::makeCarGoBoom( Vehicle* vehiclePtr )
{
    int i;

    rAssert( vehiclePtr != NULL );

    if( vehiclePtr->mVehicleType == VT_USER )
    {
        //
        // User vehicles are handled by the sound effect system
        //
        return;
    }

    //
    // Find the car matching the vehicle pointer
    //
    for( i = 0; i < NUM_TRAFFIC_SOUND_PLAYERS; i++ )
    {
        if( m_trafficPlayer[i].UsesVehicle( vehiclePtr ) )
        {
            m_trafficPlayer[i].BlowUp();
            break;
        }
    }

    if( i == NUM_TRAFFIC_SOUND_PLAYERS )
    {
        //
        // It's not a traffic vehicle, try AI
        //
        for( i = 0; i < NUM_AI_SOUND_PLAYERS; i++ )
        {
            if( m_aiPlayer[i].UsesVehicle( vehiclePtr ) )
            {
                m_aiPlayer[i].BlowUp();
                break;
            }
        }
    }
}

//=============================================================================
// MovingSoundManager::startPlatformSound
//=============================================================================
// Description: Find a player for the moving platform and hook it up
//
// Parameters:  soundData - data we need to identify platform identity and
//                          position
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::startPlatformSound( AnimSoundData* soundData )
{
    int i;

    for( i = 0; i < NUM_PLATFORM_SOUND_PLAYERS; i++ )
    {
        if( !m_platformPlayer[i].IsActive() )
        {
            m_platformPlayer[i].Activate( soundData );
            break;
        }
    }

#ifdef RAD_DEBUG
    if( i == NUM_PLATFORM_SOUND_PLAYERS )
    {
        rDebugString( "AI Vehicle sound dropped" );
    }
#endif
}

//=============================================================================
// MovingSoundManager::stopPlatformSound
//=============================================================================
// Description: Find the player for this object and stop it
//
// Parameters:  soundObject - platform identity
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::stopPlatformSound( ActionButton::AnimSwitch* soundObject )
{
    int i;

    for( i = 0; i < NUM_PLATFORM_SOUND_PLAYERS; i++ )
    {
        if( m_platformPlayer[i].UsesObject( soundObject ) )
        {
            m_platformPlayer[i].Deactivate();
            break;
        }
    }
}

//=============================================================================
// MovingSoundManager::stopAllPlatforms
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::stopAllPlatforms()
{
    int i;

    for( i = 0; i < NUM_PLATFORM_SOUND_PLAYERS; i++ )
    {
        m_platformPlayer[i].Deactivate();
    }
}

//=============================================================================
// MovingSoundManager::startAnimObjSound
//=============================================================================
// Description: Find a player for the moving platform and hook it up
//
// Parameters:  soundData - data we need to identify platform identity and
//                          position
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::startAnimObjSound( AnimSoundDSGData* soundData )
{
    int i;

    for( i = 0; i < NUM_ANIM_OBJ_SOUND_PLAYERS; i++ )
    {
        if( !m_animObjPlayer[i].IsActive() )
        {
            m_animObjPlayer[i].Activate( soundData );
            break;
        }
    }

#ifdef RAD_DEBUG
    if( i == NUM_ANIM_OBJ_SOUND_PLAYERS )
    {
        rDebugString( "AnimObj sound dropped" );
    }
#endif
}

//=============================================================================
// MovingSoundManager::stopAnimObjSound
//=============================================================================
// Description: Find the player for this object and stop it
//
// Parameters:  soundObject - platform identity
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::stopAnimObjSound( AnimCollisionEntityDSG* soundObject )
{
    int i;

    for( i = 0; i < NUM_ANIM_OBJ_SOUND_PLAYERS; i++ )
    {
        if( m_animObjPlayer[i].UsesObject( soundObject ) )
        {
            m_animObjPlayer[i].Deactivate();
            break;
        }
    }
}

//=============================================================================
// MovingSoundManager::hasOverlayClip
//=============================================================================
// Description: Check whether the given vehicle has an overlay clip that
//              we want to move around with its engine sounds
//
// Parameters:  vehiclePtr - vehicle we're finding a clip name for
//              parameters - pointer which is pointed at car sound settings
//                           if clip exists, NULL otherwise
//
// Return:      true if overlay clip found, false otherwise
//
//=============================================================================
bool MovingSoundManager::hasOverlayClip( Vehicle* vehiclePtr, carSoundParameters** parameters )
{
    IRadNameSpace* nameSpace;
    const char* clipName;
    bool retVal = false;

    rAssert( vehiclePtr != NULL );
    rAssert( parameters != NULL );

    //
    // Ignore the husk
    //
    if( vehiclePtr->mVehicleID == VehicleEnum::HUSKA )
    {
        return( false );
    }

    //
    // Find the settings for this positional sound first
    //
    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    *parameters = reinterpret_cast<carSoundParameters*>( nameSpace->GetInstance( vehiclePtr->GetName() ) );
    if( *parameters != NULL )
    {
        clipName = (*parameters)->GetOverlayClipName();
        if( clipName != NULL )
        {
            retVal = true;
        }
        else
        {
            *parameters = NULL;
        }
    }

    return( retVal );
}

//=============================================================================
// MovingSoundManager::toggleOverlayClip
//=============================================================================
// Description: Need to switch the overlay clip for the given vehicle
//              if it's a traffic vehicle, because the player is using it
//
// Parameters:  vehiclePtr - vehicle being gotten in or out of
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::toggleOverlayClip( Vehicle* vehiclePtr )
{
    int i;
    carSoundParameters* parameters;

    for( i = 0; i < NUM_TRAFFIC_SOUND_PLAYERS; i++ )
    {
        if( m_trafficPlayer[i].UsesVehicle( vehiclePtr ) )
        {
            if( hasOverlayClip( vehiclePtr, &parameters ) )
            {
                m_trafficPlayer[i].ToggleOverlayClip( parameters, getPositionalSettingName( vehiclePtr, false ) );
            }
        }
    }
}

//=============================================================================
// MovingSoundManager::getPositionalSettingName
//=============================================================================
// Description: Figure out which positional values to use for traffic sounds
//              for the given vehicle.
//
// Parameters:  vehiclePtr - traffic vehicle
//              isMissionVehicle - if true, use louder positional setting
//
// Return:      name of positional settings object
//
//=============================================================================
const char* MovingSoundManager::getPositionalSettingName( Vehicle* vehiclePtr, bool isMissionVehicle )
{
    VehicleEnum::VehicleID carID;
    const char* settingName;

    rAssert( vehiclePtr != NULL );

    carID = vehiclePtr->mVehicleID;
    if( carID == VehicleEnum::COFFIN )
    {
        settingName = "coffin_posn";
    }
    else if( carID == VehicleEnum::HALLO )
    {
        settingName = "hearse_posn";
    }
    else if( carID == VehicleEnum::CHEARS )
    {
        settingName = "chase_hearse_posn";
    }
    else if( isMissionVehicle )
    {
        settingName = "loud_ai_vehicle";
    }
    else
    {
        settingName = "generic_traffic";
    }

    return( settingName );
}

//=============================================================================
// MovingSoundManager::startWaspSound
//=============================================================================
// Description: Start making waspy noises
//
// Parameters:  wasp - wasp to start making noise
//
// Return:      void 
//
//=============================================================================
void MovingSoundManager::startWaspSound( Actor* wasp )
{
    int i;

    if( wasp->GetStatePropUID() == s_waspUID )
    {
        for( i = 0; i < NUM_WASP_SOUND_PLAYERS; i++ )
        {
            if( !m_waspPlayer[i].IsActive() )
            {
                m_waspPlayer[i].Activate( wasp );
                break;
            }
        }
    }
}
