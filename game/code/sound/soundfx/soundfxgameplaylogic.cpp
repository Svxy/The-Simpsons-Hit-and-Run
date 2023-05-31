//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfxgameplaylogic.cpp
//
// Description: Implements the SoundFXGameplayLogic class, which handles
//              the translation of events into sound effects for the game
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>
#include <radtime.hpp>
#include <radsound_hal.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/soundfxgameplaylogic.h>

#include <sound/soundmanager.h>
#include <sound/soundcollisiondata.h>
#include <sound/soundfx/positionalsoundsettings.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundallocatedresource.h>
#include <sound/avatar/carsoundparameters.h>
#include <sound/tuning/globalsettings.h>

#include <events/eventmanager.h>
#include <meta/scriptlocator.h>
#include <worldsim/physicsairef.h>
#include <worldsim/character/character.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/avatarmanager.h>
#include <mission/gameplaymanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// These are compared against the collision impulse value, which is 0 - 1.
//

static const float ARBITRARY_IMPULSE_THRESHOLD = 0.01f;

// Above this value, send big crash event to dialog system
static const float ARBITRARY_BIG_CRASH_THRESHOLD = 0.15f;

// These values determine whether to play small, medium, or big crash
// sounds for vehicles
static const float ARBITRARY_VEHICLE_MEDIUM_CRASH_THRESHOLD = 0.15f;
static const float ARBITRARY_VEHICLE_BIG_CRASH_THRESHOLD = 0.25f;

static const char* s_hydrantSprayName = "hydrant_spray";

//
// Arbitrary value.  If a collision occurs between the avatar vehicle
// and an object, where the difference between their distances is greater
// than this, then the avatar hit a wall or something, so don't average
// the positions.
//
static const float POSITIONAL_COLLISION_MAX_DISTANCE_SQR = 100.0f;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundFXGameplayLogic::SoundFXGameplayLogic
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFXGameplayLogic::SoundFXGameplayLogic() :
    m_collisionMinMax( NULL ),
    m_coinCounter( 0 ),
    m_lastRonkTime( 0 ),
    m_globalSettings( NULL )
{
}

//==============================================================================
// SoundFXGameplayLogic::~SoundFXGameplayLogic
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFXGameplayLogic::~SoundFXGameplayLogic()
{
}

//=============================================================================
// SoundFXGameplayLogic::RegisterEventListeners
//=============================================================================
// Description: Register as listener of sound effect events with Event Manager
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::RegisterEventListeners()
{
    EventManager* eventMgr = GetEventManager();

    eventMgr->AddListener( this, EVENT_COLLISION );
    eventMgr->AddListener( this, EVENT_FOOTSTEP );
    eventMgr->AddListener( this, EVENT_BIG_RED_SWITCH_PRESSED );
    eventMgr->AddListener( this, EVENT_JUMP_TAKEOFF );
    eventMgr->AddListener( this, EVENT_JUMP_LANDING );
    eventMgr->AddListener( this, EVENT_POSITIONAL_SOUND_TRIGGER_HIT );
    eventMgr->AddListener( this, EVENT_GETINTOVEHICLE_START );
    eventMgr->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
    eventMgr->AddListener( this, EVENT_FE_MENU_SELECT );
    eventMgr->AddListener( this, EVENT_FE_MENU_UPORDOWN );
    eventMgr->AddListener( this, EVENT_FE_MENU_BACK );
    eventMgr->AddListener( this, EVENT_COLLECTED_COINS );
    eventMgr->AddListener( this, EVENT_LOST_COINS );
    eventMgr->AddListener( this, EVENT_SPAWNED_COINS );
    eventMgr->AddListener( this, EVENT_PHONE_BOOTH_BUSY );
    eventMgr->AddListener( this, EVENT_PLAYER_CAR_HIT_NPC );
    eventMgr->AddListener( this, EVENT_BIG_BOOM_SOUND );
    eventMgr->AddListener( this, EVENT_CARD_COLLECTED );
    eventMgr->AddListener( this, EVENT_COLLECTED_WRENCH );
    eventMgr->AddListener( this, EVENT_VEHICLE_SUSPENSION_BOTTOMED_OUT );
    eventMgr->AddListener( this, EVENT_MISSION_COLLECTIBLE_PICKED_UP );
    eventMgr->AddListener( this, EVENT_KICK_NPC );
    eventMgr->AddListener( this, EVENT_OBJECT_KICKED );
    eventMgr->AddListener( this, EVENT_WASP_BULLET_FIRED );
    eventMgr->AddListener( this, EVENT_WASP_BULLET_MISSED );
    eventMgr->AddListener( this, EVENT_WASP_BULLET_HIT_CHARACTER_STYLIZED_VIOLENCE_FOLLOWS );
    eventMgr->AddListener( this, EVENT_START_ANIMATION_SOUND );
    eventMgr->AddListener( this, EVENT_MISSION_SUCCESS );
    eventMgr->AddListener( this, EVENT_STAGE_COMPLETE );
    eventMgr->AddListener( this, EVENT_HIT_AND_RUN_CAUGHT );
    eventMgr->AddListener( this, EVENT_HIT_AND_RUN_METER_THROB );
    eventMgr->AddListener( this, EVENT_SHOW_MISSION_OBJECTIVE );
	eventMgr->AddListener( this, EVENT_PLAY_BIRD_SOUND );
    eventMgr->AddListener( this, static_cast<EventEnum>(EVENT_LOCATOR + LocatorEvent::BOUNCEPAD) );
    eventMgr->AddListener( this, EVENT_FE_PAUSE_MENU_END );
    eventMgr->AddListener( this, EVENT_FE_PAUSE_MENU_START );
    eventMgr->AddListener( this, EVENT_FE_CANCEL );
    eventMgr->AddListener( this, EVENT_FE_CONTINUE );
    eventMgr->AddListener( this, EVENT_FE_LOCKED_OUT );
    eventMgr->AddListener( this, EVENT_BARREL_BLOWED_UP );
    eventMgr->AddListener( this, EVENT_FE_CREDITS_NEW_LINE );
}

//=============================================================================
// SoundFXGameplayLogic::HandleEvent
//=============================================================================
// Description: Play sound effects in response to events
//
// Parameters:  id - Sound effect event identifier
//              pEventData - Currently unused
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::HandleEvent( EventEnum id, void* pEventData )
{
    Vehicle* vehiclePtr;
    unsigned int ronkTime;
    const char* soundName;
    RenderEnums::LevelEnum level;
    AnimSoundData* animData;

    switch( id )
    {
        case EVENT_COLLISION:
            handleCollisionEvent( static_cast<SoundCollisionData*>(pEventData) );
            break;

        case EVENT_FOOTSTEP:
            handleFootstepEvent( static_cast<Character*>(pEventData) );
            break;

        case EVENT_JUMP_TAKEOFF:
            playSFXSound( "jump", false );
            break;

        case EVENT_JUMP_LANDING:
            //
            // TEMPORARY: we need a surface-sensitive solution
            //
            playSFXSound( "feet_concrete_jump", false );
            break;

        case EVENT_BIG_RED_SWITCH_PRESSED:
            handleSwitchEvent();
            break;

        case EVENT_POSITIONAL_SOUND_TRIGGER_HIT:
            playPositionalSound( static_cast<ScriptLocator*>(pEventData) );
            break;

        case EVENT_GETINTOVEHICLE_START:
        case EVENT_GETOUTOFVEHICLE_END:
            playCarDoorSound( id, static_cast<Character*>(pEventData) );
            break;

        case EVENT_FE_MENU_SELECT:
            playSFXSound( "accept", false, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;
        case EVENT_FE_MENU_UPORDOWN:
            playSFXSound( "scroll", false, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;
        case EVENT_FE_MENU_BACK:
            playSFXSound( "back", false, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;
        case EVENT_FE_LOCKED_OUT:
            playSFXSound( "locked_out", false );
            break;

        case EVENT_COLLECTED_COINS:
            playCoinCollectSound();
            break;

        case EVENT_SPAWNED_COINS:
            //
            // Should this do anything?
            //
            break;

        case EVENT_LOST_COINS:
            playSFXSound( "coin_lose", true );
            break;

        case EVENT_PHONE_BOOTH_BUSY:
            playSFXSound( "phonebusy", false );
            break;

        case EVENT_PLAYER_CAR_HIT_NPC:
            {
                //
                // Set up special collision data for the collision code, because
                // it won't be getting a collision event.  I like saying "collision".
                //
                SoundCollisionData collData( 0.0f, static_cast<CollisionEntityDSG*>(pEventData), NULL );

                handleCollisionEvent( &collData );
            }
            break;

        case EVENT_KICK_NPC:
            playSFXSound( "car_hit_pedestrian", true );
            break;

        case EVENT_OBJECT_KICKED:
            handleObjectKick( static_cast<CollisionEntityDSG*>(pEventData) );
            break;

        case EVENT_BIG_BOOM_SOUND:
            vehiclePtr = static_cast<Vehicle*>(pEventData);
            rAssert( vehiclePtr != NULL );
            if( vehiclePtr->mVehicleType == VT_USER )
            {
                playSFXSound( "generic_car_explode", false );
            }
            break;

        case EVENT_BARREL_BLOWED_UP:
            playSFXSound( "generic_car_explode", false );
            break;

        case EVENT_CARD_COLLECTED:
            //
            // Only play card sound effect in car, the music system
            // handles it when on foot
            //
            if( GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar() )
            {
                playSFXSound( "card_collect", false );
            }
            break;

        case EVENT_COLLECTED_WRENCH:
            playSFXSound( "wrench_collect", false );
            break;

        case EVENT_VEHICLE_SUSPENSION_BOTTOMED_OUT:
            //
            // Play only if we haven't played the last one within a half-second.
            // Use a simple timestamp to determine this.
            //
            ronkTime = ::radTimeGetMilliseconds();
            if( ( ronkTime < m_lastRonkTime )
                || ( ( ronkTime - m_lastRonkTime ) > 500 ) )
            {
                m_lastRonkTime = ronkTime;
                playSFXSound( "suspension_ronks", true );
            }
            break;

        case EVENT_MISSION_COLLECTIBLE_PICKED_UP:
            handleCollection();
            break;

        case EVENT_WASP_BULLET_FIRED:
            playSFXSound( "wasp_lasers", true );
            break;

        case EVENT_WASP_BULLET_MISSED:
            playSFXSound( "laser_miss", true );
            break;

        case EVENT_WASP_BULLET_HIT_CHARACTER_STYLIZED_VIOLENCE_FOLLOWS:
            playSFXSound( "character_zapped", true );
            break;

        case EVENT_START_ANIMATION_SOUND:
            animData = static_cast<AnimSoundData*>( pEventData );
            soundName = animData->soundName;
            if( ( soundName != NULL ) && ( animData->animJoint == NULL ) )
            {
                playSFXSound( soundName, false );
            }
            break;

        case EVENT_MISSION_SUCCESS:
            playSFXSound( "mission_complete", false );
            break;

        case EVENT_STAGE_COMPLETE:
            playSFXSound( "stage_complete", false );
            break;

        case EVENT_HIT_AND_RUN_CAUGHT:
            playSFXSound( "busted", false );
            break;

        case EVENT_HIT_AND_RUN_METER_THROB:
            playSFXSound( "rage_warning", false );
            break;

        case EVENT_SHOW_MISSION_OBJECTIVE:
            playSFXSound( "pop_up", true );
            break;

        case EVENT_PLAY_BIRD_SOUND:
            //
            // Pick the correct bird effect for the level
            //
            level = GetGameplayManager()->GetCurrentLevelIndex();
            if( ( level == RenderEnums::L2 )
                || ( level == RenderEnums::L5 ) )
            {
                playSFXSound( "pigeon_takeoff", false );
            }
            else if( ( level == RenderEnums::L3 )
                     || ( level == RenderEnums::L6 ) )
            {
                playSFXSound( "gull_takeoff", false );
            }
            break;

        case EVENT_LOCATOR + LocatorEvent::BOUNCEPAD :
            playSFXSound( "trampoline", true );
            break;

        case EVENT_FE_PAUSE_MENU_START:
            playSFXSound( "pause_on", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_PAUSE_MENU_END:
            playSFXSound( "pause_off", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_CONTINUE:
            playSFXSound( "continue", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_CANCEL:
            playSFXSound( "cancel", true, false, NULL, GetSoundManager()->GetSfxVolume() );
            break;

        case EVENT_FE_CREDITS_NEW_LINE:
            playCreditLine( reinterpret_cast<int>(pEventData) );
            break;

        default:
            rAssertMsg( false, "Unexpected event received in SoundFXGameplayLogic\n" );
            break;
    }
}

//=============================================================================
// SoundFXGameplayLogic::OnPlaybackComplete
//=============================================================================
// Description: No longer used, now that tutorial lines are played through
//              the dialog system
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::OnPlaybackComplete()
{
}

//=============================================================================
// SoundFXGameplayLogic::Cleanup
//=============================================================================
// Description: Called on gameplay exit.  Use this to shut down any
//              positional sounds that might be playing.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::Cleanup()
{
    int i;

    for( i = 0; i < s_numPositionalSounds; i++ )
    {
        if( m_positionalSounds[i].IsInUse() )
        {
            m_positionalSounds[i].Stop();
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SoundFXGameplayLogic::getGlobalSettings
//=============================================================================
// Description: Initialize the member pointer to the global settings object
//              if not done yet, and return it
//
// Parameters:  None
//
// Return:      pointer to globalSettings object
//
//=============================================================================
globalSettings* SoundFXGameplayLogic::getGlobalSettings()
{
    IRadNameSpace* nameSpace;

    if( m_globalSettings == NULL )
    {
        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );
        m_globalSettings = static_cast<globalSettings*>(nameSpace->GetInstance( "tuner" ));
        rAssert( m_globalSettings != NULL );
    }

    return( m_globalSettings );
}

//=============================================================================
// SoundFXGameplayLogic::handleCollisionEvent
//=============================================================================
// Description: Play a collision sound appropriate for this event, unless we've
//              already received an event for this collision.  A typical collision
//              throws a lot of duplicate events, apparently, so we gotta screen
//              'em out.
//
// Parameters:  collisionData - data we need pertaining to the collision, passed
//                              through the event mechanism
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::handleCollisionEvent( SoundCollisionData* collisionData )
{
    int i;

    //
    // FOR NOW: filter out collisions that don't involve the user's car.  Later,
    // we might want to do AI-car collisions within a certain radius or
    // something
    //

    //TODO (Cary):  I think that this should search the avatar manager for each vehicle in the
    //collision pair (or always take A so that vehicles don't get doubled events) and test to
    //see if the vehicle is user controlled.  The code as it is only sends events for player 0
    //and won't work for multiplayer.
    Vehicle* pVehicle = GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle();

    if( !pVehicle )
    {
        return;
    }

    //
    // See if we're already playing a sound for this collision, or if all collision
    // players are in use.  If so, exit
    //
    for( i = 0; i < s_numCollisionSounds; i++ )
    {
        if( m_collisionSounds[i].soundPlayer.IsInUse() )
        {
            if( collisionPairMatches( i, collisionData->collObjA, collisionData->collObjB ) )
            {
                // Collision sound already being played, do nothing
                return;
            }
        }
        else
        {
            break;
        }
    }

    if( i == s_numCollisionSounds )
    {
        //
        // All players are being used
        //
        return;
    }

    CollisionAttributes* attributes;
    CollisionEntityDSG* collEntities[2];
    const char* vehicleCrashName = NULL;
    const char* nonVehicleCrashName = NULL;
    CollisionEntityDSG* hydrantObject = NULL;
    bool hydrantHit = false;
    EventEnum minorCrash = EVENT_MINOR_VEHICLE_CRASH;
    EventEnum bigCrash = EVENT_BIG_VEHICLE_CRASH;
    Vehicle* collisionVehicle = NULL;
    CollisionEntityDSG* otherEntity = NULL;
    rmt::Vector diffVector;
    rmt::Vector avatarPosition;
    rmt::Vector* vectorPtr = NULL;
    bool avatarVehicleInvolved = false;

    collEntities[0] = collisionData->collObjA;
    collEntities[1] = collisionData->collObjB;

    //
    // NOTE: this code assumes that we have at most one vehicle and one non-vehicle
    // in the collision pair.  If we have two non-vehicles, one is lost.  Eh.
    //
    for( i = 0; i < 2; i++ )
    {
        //
        // If the DSG object is NULL, one of the objects is a fence or something.  
        // Ignore and move on.
        //
        if( collEntities[i] != NULL )
        {
            if( collEntities[i]->GetAIRef() == PhysicsAIRef::redBrickVehicle )
            {
                //
                // Vehicle, play smashy sound
                //
                if( collisionData->mIntensity > ARBITRARY_VEHICLE_BIG_CRASH_THRESHOLD )
                {
                    vehicleCrashName = "large_car_crash";
                }
                else if( collisionData->mIntensity > ARBITRARY_VEHICLE_MEDIUM_CRASH_THRESHOLD )
                {
                    vehicleCrashName = "medium_car_crash";
                }
                else
                {
                    vehicleCrashName = "small_car_crash";
                }

                collisionVehicle = static_cast<Vehicle*>( collEntities[i] );

                if( collEntities[i] == pVehicle )
                {
                    //
                    // Collision involves the user vehicle, store the other
                    // entity
                    //
                    if( i == 0 )
                    {
                        otherEntity = collEntities[1];
                    }
                    else
                    {
                        otherEntity = collEntities[0];
                    }

                    avatarVehicleInvolved = true;
                }
            }
            else
            {
                //
                // Non-vehicle, get attributes
                //
                attributes = collEntities[i]->GetCollisionAttributes();
                if( attributes == NULL )
                {
                    //
                    // No attributes.  Character, I assume?  It'd be nice if the
                    // Character class filled this in.  Something to try.
                    //
                    nonVehicleCrashName = "car_hit_pedestrian";
                }
                else
                {
                    nonVehicleCrashName = attributes->GetSound();

                    //
                    // Stinky special case
                    //
                    if( attributes->GetBreakable() == BreakablesEnum::eHydrantBreaking )
                    {
                        hydrantHit = true;
                        hydrantObject = collEntities[i];
                    }
                }

                //
                // This eliminates vehicle-on-vehicle collision, so don't use that
                // type of event for the dialog system
                //
                minorCrash = EVENT_MINOR_CRASH;
                bigCrash = EVENT_BIG_CRASH;
           }
        }
        else
        {
            //
            // This eliminates vehicle-on-vehicle collision, so don't use that
            // type of event for the dialog system
            //
            minorCrash = EVENT_MINOR_CRASH;
            bigCrash = EVENT_BIG_CRASH;
        }
    }


    //
    // Based on how hard the crash is, pass on an event that the dialog
    // system can use (Q: is this the best place for this?  If other
    // game components start using it, this should be moved).
    //
    if( avatarVehicleInvolved )
    {
        if( collisionData->mIntensity < ARBITRARY_BIG_CRASH_THRESHOLD )
        {
            if( collisionData->mIntensity >= ARBITRARY_IMPULSE_THRESHOLD )
            {
                if( minorCrash == EVENT_MINOR_CRASH )
                {
                    //
                    // Can't be vehicle-on-vehicle
                    //
                    collisionVehicle = NULL;
                }
                GetEventManager()->TriggerEvent( minorCrash, collisionVehicle );
            }
        }
        else
        {
            if( bigCrash == EVENT_BIG_CRASH )
            {
                //
                // Isn't vehicle-on-vehicle
                //
                collisionVehicle = NULL;
            }
            GetEventManager()->TriggerEvent( bigCrash, collisionVehicle );
        }
    }

    //
    // Play the collision sound if we've got a player free
    //
    rAssert( collisionData->mIntensity >= 0.0f );
    rAssert( collisionData->mIntensity <= 1.0f );

    //
    // Check whether the colliding entities are really far apart.  If so,
    // then the player has hit a wall or something, so don't bother averaging
    // the distances between them, just play at the avatar position
    //
    if( otherEntity != NULL )
    {
         pVehicle->GetPosition( &avatarPosition );
         otherEntity->GetPosition( &diffVector );
         diffVector -= avatarPosition;

         if( diffVector.MagnitudeSqr() > POSITIONAL_COLLISION_MAX_DISTANCE_SQR )
         {
             vectorPtr = &avatarPosition;
         }
    }

    //
    // Play the sounds in free players (or kill something
    // if they're all in use)
    //
    if( vehicleCrashName != NULL )
    {
        startCollisionPlayer( vehicleCrashName, collisionData->collObjA, collisionData->collObjB, vectorPtr );
    }
    if( nonVehicleCrashName != NULL )
    {
        startCollisionPlayer( nonVehicleCrashName, collisionData->collObjA, collisionData->collObjB, vectorPtr );

        //
        // Stinky fire hydrant hack.
        //
        if( hydrantHit )
        {
            rAssert( hydrantObject != NULL );
            startCollisionPlayer( s_hydrantSprayName, hydrantObject, NULL, NULL );
        }
    }
}

//=============================================================================
// SoundFXGameplayLogic::handleObjectKick
//=============================================================================
// Description: Play a sound for an object that gets kicked
//
// Parameters:  collObject - kicked object
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::handleObjectKick( CollisionEntityDSG* collObject )
{
    CollisionAttributes* attributes;
    const char* soundName;

    attributes = collObject->GetCollisionAttributes();
    if( attributes != NULL )
    {
        soundName = attributes->GetSound();

        //
        // Cheat: don't play this positionally because I'd have to work around
        // the double-play filter because of the stinky fire hydrant.  Hope
        // that's okay
        //
        playSFXSound( soundName, true );

        //
        // More hydrant hack
        //
        if( attributes->GetBreakable() == BreakablesEnum::eHydrantBreaking )
        {
            startCollisionPlayer( s_hydrantSprayName, collObject, NULL, NULL );
        }
    }
}

//=============================================================================
// SoundFXGameplayLogic::collisionPairMatches
//=============================================================================
// Description: Check for match between parameters and collision pair at
//              given index
//
// Parameters:  index - index into m_collisionSource that we're matching against
//              firstObj, secondObj - collision objects to match with
//
// Return:      true if matching, false otherwise
//
//=============================================================================
bool SoundFXGameplayLogic::collisionPairMatches( int index, void* firstObj, void* secondObj )
{
    PositionalSFXPlayer* storedPair;

    rAssert( index < s_numCollisionSounds );

    storedPair = &(m_collisionSounds[index]);

    return( ( ( storedPair->collObjA == firstObj ) &&
              ( storedPair->collObjB == secondObj ) )

            ||

            ( ( storedPair->collObjA == secondObj ) &&
              ( storedPair->collObjB == firstObj ) ) );
}

//=============================================================================
// SoundFXGameplayLogic::startCollisionPlayer
//=============================================================================
// Description: Find a free positional sound player (or stop one if necessary)
//              and play the sound and store the collision pair
//
// Parameters:  soundName - name of sound resource to play
//              objA - first colliding object
//              objB - second colliding object
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::startCollisionPlayer( const char* soundName, 
                                                 CollisionEntityDSG* objA,
                                                 CollisionEntityDSG* objB,
                                                 rmt::Vector* positionPtr )
{
    int index;
    rmt::Vector posnA;
    rmt::Vector posnB;
    rmt::Vector average;
    rmt::Vector vectorToListener;
    radSoundVector rsListenerPosition;
    rmt::Vector listenerPosition;
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    IRadSoundHalListener* theListener;


    //
    // Look for a free player first
    //
    for( index = 0; index < s_numCollisionSounds; index++ )
    {
        if( !(m_collisionSounds[index].soundPlayer.IsInUse()) )
        {
            break;
        }
    }

    //
    // If all are in use, stop one.  Arbitrarily, use the one
    // at the start of the list, since it's most likely to have
    // been playing the longest.  No need to get cute.
    //
    if( index >= s_numCollisionSounds )
    {
        index = 0;
        m_collisionSounds[0].soundPlayer.Stop();

        rWarningMsg( false, "Collision sound dropped for lack of players\n" );
    }

    //
    // Store the collision pair
    //
    m_collisionSounds[index].collObjA = objA;
    m_collisionSounds[index].collObjB = objB;

    //
    // Play the sound halfway between the positions of the
    // colliding objects (if two objects are used)
    //
    if( positionPtr != NULL )
    {
        average = *positionPtr;
    }
    else if( objA == NULL )
    {
        objB->GetPosition( &average );
    }
    else if( objB == NULL )
    {
        objA->GetPosition( &average );
    }
    else
    {
        objA->GetPosition( &posnA );
        objB->GetPosition( &posnB );
        average.Add( posnA, posnB );
        average *= 0.5f;
    }

    if( GetGameplayManager()->IsSuperSprint() )
    {
        //
        // Hoo boy.  Big stinky hack coming up.  The problem is that sounds are
        // positional, and the camera is a very long ways away in the bonus game.
        // Fudge the position to make the collisions suitably close.
        //
        theListener = ::radSoundHalListenerGet( );
        rAssert( theListener != NULL );

        theListener->GetPosition( &rsListenerPosition );
        listenerPosition.Set( rsListenerPosition.m_x, rsListenerPosition.m_y, rsListenerPosition.m_z );
        vectorToListener = average - listenerPosition;
        vectorToListener.Scale( 0.01f, 0.01f, 0.01f );
        average = listenerPosition + vectorToListener;
    }

    if( m_collisionMinMax == NULL )
    {
        //
        // Lazy initialization.  Grab the positional characteristics for collision sounds
        //
        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );
        nameSpaceObj = nameSpace->GetInstance( "collision_sounds" );
        rAssert( nameSpaceObj != NULL );
        m_collisionMinMax = reinterpret_cast<positionalSoundSettings*>(nameSpaceObj);
    }

    m_collisionSounds[index].soundPlayer.SetPosition( average.x, average.y, average.z );
    m_collisionSounds[index].soundPlayer.SetParameters( m_collisionMinMax );
    m_collisionSounds[index].soundPlayer.PlaySound( soundName );
    m_collisionSounds[index].soundPlayer.SetTrim( 1.0f );
}

//=============================================================================
// SoundFXGameplayLogic::handleFootstepEvent
//=============================================================================
// Description: Plays appropriate footstep sound
//
// Parameters:  walkingCharacter - character doing the walking
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::handleFootstepEvent( Character* walkingCharacter )
{
    eTerrainType terrain;
    bool isInterior;
    globalSettings* clipNameObj;
    const char* name = NULL;

    rAssert( walkingCharacter != NULL );
    walkingCharacter->GetTerrainType( terrain, isInterior );

    //
    // Get the parameter object for this positional sound.
    //
    clipNameObj = getGlobalSettings();

    if( isInterior )
    {
        //
        // Use the road clip indoors
        //
        name = clipNameObj->GetFootstepRoadClipName();
    }
    else if( terrain == TT_Metal )
    {
        name = clipNameObj->GetFootstepMetalClipName();
    }
    else if( terrain == TT_Wood )
    {
        name = clipNameObj->GetFootstepWoodClipName();
    }
    else
    {
        name = clipNameObj->GetFootstepRoadClipName();
    }

    rAssert( name != NULL );

    playSFXSound( name, true );
}

//=============================================================================
// SoundFXGameplayLogic::handleSwitchEvent
//=============================================================================
// Description: Play clicking sound for big red switches
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::handleSwitchEvent()
{
    playSFXSound( "switch", false );
}

//=============================================================================
// SoundFXGameplayLogic::handleCollection
//=============================================================================
// Description: Play the appropriate sound for picking up a collectible
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::handleCollection()
{
    RenderEnums::LevelEnum level;
    int mission;
    const char* soundName = NULL;

    //
    // We don't have a way to identify collectible types, so select a
    // sound resource based on our level/mission
    //
    level = GetGameplayManager()->GetCurrentLevelIndex();
    mission = GetGameplayManager()->GetCurrentMissionIndex();
    switch( level )
    {
        case RenderEnums::L1 :
            soundName = "level_1_pickup_sfx";
            break;
        case RenderEnums::L2 :
            if( mission == 6 )
            {
                soundName = "monkey_collect";
            }
            else
            {
                soundName = "level_2_pickup_sfx";
            }
            break;
        case RenderEnums::L3 :
            soundName = "level_3_pickup_sfx";
            break;
        case RenderEnums::L4 :
            soundName = "level_4_pickup_sfx";
            break;
        case RenderEnums::L5 :
            soundName = "level_5_pickup_sfx";
            break;
        case RenderEnums::L6 :
            soundName = "level_6_pickup_sfx";
            break;
        case RenderEnums::L7 :
            soundName = "nuclear_waste_collect";
            break;
        default:
            break;
    }

    rAssertMsg( ( soundName != NULL ), "Collection without sound effect, tell Esan\n" );

    playSFXSound( soundName, true );
}

//=============================================================================
// SoundFXGameplayLogic::playPositionalSound
//=============================================================================
// Description: Plays a positional sound in the world when a positional
//              (a.k.a. script) trigger is hit
//
// Parameters:  locator - locator for the sound
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::playPositionalSound( ScriptLocator* locator )
{
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    positionalSoundSettings* parameters;
    float diceRoll;
    float probability;
    int i;
    rmt::Vector locatorPosition;

    rAssert( locator != NULL );

    //
    // Get the parameter object for this positional sound.
    //
    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    nameSpaceObj = nameSpace->GetInstance( locator->GetKey() );
    if( nameSpaceObj != NULL )
    {
        parameters = reinterpret_cast<positionalSoundSettings*>( nameSpaceObj );

        if( !(locator->GetPlayerEntered()) )
        {
            //
            // Player is exiting volume, not entering.  If we're playing a sound
            // already, stop it
            //
            for( i = 0; i < s_numPositionalSounds; i++ )
            {
                if( m_positionalSounds[i].IsInUse() 
                    && ( m_positionalSounds[i].GetParameters() == parameters ) )
                {
                    m_positionalSounds[i].Stop();
                    break;
                }
            }
        }
        else
        {
            probability = parameters->GetPlaybackProbability();
            if( probability < 1.0f )
            {
                //
                // Random play
                //
                diceRoll = (static_cast<float>( rand() % 100 )) / 100.0f;
                if( diceRoll >= probability )
                {
                    return;
                }
            }

            //
            // Find a player and play
            //
            for( i = 0; i < s_numPositionalSounds; i++ )
            {
                if( !(m_positionalSounds[i].IsInUse()) )
                {
                    locator->GetLocation( &locatorPosition );
                    m_positionalSounds[i].SetPosition( locatorPosition.x,
                                                       locatorPosition.y,
                                                       locatorPosition.z );
                    m_positionalSounds[i].SetParameters( parameters );

                    //
                    // Don't buffer, to save IOP
                    //

                    m_positionalSounds[i].PlaySound( parameters->GetClipName(), NULL );
                    break;
                }
            }
        }
    }
    else
    {
        rDebugString( "Couldn't play missing positional sound" );
    }
}

void SoundFXGameplayLogic::playCarDoorSound( EventEnum eventType, Character* playerCharacter )
{
    Vehicle* car;
    carSoundParameters* parameters;
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    const char* clipName;

    //
    // Get name of clip
    //
    car = playerCharacter->GetTargetVehicle();
    if( car != NULL )
    {
        //
        // Get the car sound parameter object for this vehicle.
        //
        // IMPORTANT: We assume that the object in the namespace has the same
        // name as the one in the vehicle object, which comes from the loading
        // script for that car, I think.
        //
        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );

        nameSpaceObj = nameSpace->GetInstance( car->GetName() );
        if( nameSpaceObj != NULL )
        {
            parameters = reinterpret_cast<carSoundParameters*>( nameSpaceObj );

            if( eventType == EVENT_GETINTOVEHICLE_START )
            {
                clipName = parameters->GetCarDoorOpenClipName();
            }
            else
            {
                clipName = parameters->GetCarDoorCloseClipName();
            }

            if( clipName != NULL )
            {
                playSFXSound( clipName, false );
            }
        }
        else
        {
            rDebugString( "Couldn't find car door sound\n" );
        }
    }
}

//=============================================================================
// SoundFXGameplayLogic::playCoinCollectSound
//=============================================================================
// Description: Play next coin collect sound in sequence
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXGameplayLogic::playCoinCollectSound()
{
    float pitch;
    unsigned int numPitches;
    globalSettings* pitchSequence = getGlobalSettings();

    pitch = pitchSequence->GetCoinPitch( m_coinCounter );
    playSFXSound( "coin_collect_01", true, false, NULL, 1.0f, pitch );

    numPitches = pitchSequence->GetNumCoinPitches();
    if( numPitches > 0 )
    {
        m_coinCounter = ( m_coinCounter + 1 ) % numPitches;
    }
}
