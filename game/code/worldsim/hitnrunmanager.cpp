//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        HitnRunManager.cpp
//
// Description: Implementation of class HitnRunManager
//
// History:     26/03/2003 + Created -- Jesse Cluff
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <radmath/matrix.hpp>
#include <radmath/random.hpp>

//#include <math.h> //need this if we want extra accurate coin paths.

//========================================
// Project Includes
//========================================
#include <gameflow/gameflow.h>

#include <worldsim/hitnrunmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/coins/coinmanager.h>
#include <memory/srrmemory.h>
#include <events/eventmanager.h>
#include <render/intersectmanager/intersectmanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/dsg/statepropdsg.h>
#include <data/persistentworldmanager.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <interiors/interiormanager.h>
#include <constants/breakablesenum.h>
#include <constants/vehicleenum.h>
#include <worldsim/harass/chasemanager.h>
#include <presentation\gui\ingame\guimanageringame.h>
#include <presentation\gui\ingame\guiscreenhud.h>
#include <presentation\gui\guisystem.h>
#include <ai\statemanager.h>
#include <contexts/context.h>

/* Watcher stuff */
#ifndef RAD_RELEASE
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#endif
//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
static const float MAX_HITNRUN = 100.0f;
static const float MIN_HITNRUN = 0.0f;
static const float FADE_TIME = 750.0f;
static float STOP_HITNRUN = 5.0f; //was 78
static const float ALLOW_THROB = 60.0f;

HitnRunManager* HitnRunManager::smpHitnRunManager = NULL;

static const char* OBJECTS_THAT_NEVER_GIVE_COINS[] = 
{
    "l1_hedgeunit",
    "l7_hedgeunit"
};

static const int NUM_OBJECTS_THAT_NEVER_GIVE_COINS = sizeof(OBJECTS_THAT_NEVER_GIVE_COINS)/sizeof(OBJECTS_THAT_NEVER_GIVE_COINS[0]);

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

HitnRunManager* HitnRunManager::CreateInstance( void )
{
    rAssertMsg( smpHitnRunManager == NULL, "HitnRunManager already created.\n" );
    HeapManager::GetInstance()->PushHeap( GMA_PERSISTENT );
    smpHitnRunManager = new HitnRunManager;
    rAssert( smpHitnRunManager );
    HeapManager::GetInstance()->PopHeap( GMA_PERSISTENT );
    return HitnRunManager::GetInstance();
}

HitnRunManager* HitnRunManager::GetInstance( void )
{
    rAssertMsg( smpHitnRunManager != NULL, "HitnRunManager has not been created yet.\n" );
    return smpHitnRunManager;
}

void HitnRunManager::DestroyInstance( void )
{
    rAssertMsg( smpHitnRunManager != NULL, "HitnRunManager has not been created.\n" );
    delete smpHitnRunManager;
    smpHitnRunManager = NULL;
}

//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
HitnRunManager::HitnRunManager() :
    mCurrHitnRun(0.0f),
    mDecayRatePerSecond(1.0f),
    mDecayRateWhileSpawning(6.0f),
    mDecayRateInsidePerSecond(10.0f),
    mVehicleDestroyedDelta(25.0f),
    mVehicleDestroyedCoins( 10 ),
    mVehicleHitDelta(5.0f),
    mHitBreakableDelta(7.5f),
    mHitBreakableCoins( 1 ),
    mHitMoveableDelta(7.5f),
    mHitMoveableCoins( 1 ),
    mHitKrustyGlassDelta(0.0f),
    mHitKrustyGlassCoins( 5 ),
    mColaPropDestroyedDelta(0.0f),
    mColaPropDestroyedCoins( 10 ),
    mKickNPCDelta(10.0f),
    mKickNPCCoins( 0 ),				//no coins on peds
    mPlayerCarHitNPCDelta(13.0f),
    mPlayerCarHitNPCCoins( 0 ),		//no coins on peds
    mBustedCoins( 50 ),
    mSwitchSkinDelta(-100.0f),
    mChangeVehicleDelta(-100.0f),
    mPrevVehicleID(VehicleEnum::INVALID),
//    mPrevVehicleHit(NULL),
//    mPrevMoveableHit(NULL),
    mLeastRecentlyHit(0),
    mChaseOn(false),
    mSpawnOn(false),
    mDecayDelayMS(0.0f),
    mDecayDelay(3000.0f),
    mDecayDelayWhileSpawning(0.0f), // was 3000
    mNumChaseCars(1),
    mHitnRunDisabled(false),
    mDecayDisabled(false),
    mCopTicketDistance(10.0f),
    mCopTicketDistanceOnFoot(10.0f),
    mCopTicketSpeedThreshold(30.0f),
    mCopTicketTimeThreshold(0.75f), // was 0.1; trying something to get rid of "bump = busted"
    mTicketTimer(0.0f),
    mLastUpdateValue(0.0f),
    mLastHitnRunValue(0.0f),
    mVehicleReset(true),
    mVehicleResetTimer(0.0f),
    mUnresponsiveTime(1500.0f),
    mFadeDone(true),
    mFadeTimer(0.0f),
    mImmunityVehicle(NULL),
    mWarningThreshold(0.78f),
    mAllowThrob(true)
{
    for(int i = 0; i < 16; i++)
    {
        mPrevHits[i] = NULL;
    }
    // We don't want to do string compares when determining if an object that we just hit
    // is to not give a coin
    // So build a list of tUIDs in the ctor
    mObjectsThatNeverGiveCoins = new tUID[ NUM_OBJECTS_THAT_NEVER_GIVE_COINS ];
    for(int i = 0 ; i < NUM_OBJECTS_THAT_NEVER_GIVE_COINS ; i++ )
    {
        mObjectsThatNeverGiveCoins[ i ] = tName::MakeUID( OBJECTS_THAT_NEVER_GIVE_COINS[i] );            
    }

#ifndef RAD_RELEASE
    radDbgWatchAddFloat( &mCurrHitnRun, "Current HitnRun Value", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mDecayRatePerSecond, "Decay Rate", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mDecayRateWhileSpawning, "Decay Rate while spawning", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mDecayRateInsidePerSecond, "Interior Decay Rate", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mVehicleDestroyedDelta, "Vehicle Destroyed Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mVehicleDestroyedCoins, "Vehicle Destroyed Coins", "HitnRun", NULL, NULL, 0, 100, false );
    radDbgWatchAddFloat( &mHitBreakableDelta, "Hit Breakable Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mVehicleHitDelta, "Vehicle Hit Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mHitBreakableCoins, "Hit Breakable Coins", "HitnRun", NULL, NULL, 0, 100, false );
    radDbgWatchAddFloat( &mHitMoveableDelta, "Hit Moveable Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mHitMoveableCoins, "Hit Moveable Coins", "HitnRun", NULL, NULL, 0, 100, false );
    radDbgWatchAddFloat( &mHitKrustyGlassDelta, "Hit Krusty Glass Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mHitKrustyGlassCoins, "Hit Krusty Glass Coins", "HitnRun", NULL, NULL, 0, 100, false );
    radDbgWatchAddFloat( &mColaPropDestroyedDelta, "Cola Prop Destroyed Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mColaPropDestroyedCoins, "Cola Prop Destroyed Coins", "HitnRun", NULL, NULL, 0, 100, false );
    radDbgWatchAddFloat( &mKickNPCDelta, "Kick NPC Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mKickNPCCoins, "Kick NPC Coins", "HitnRun", NULL, NULL, 0, 100, false );
    radDbgWatchAddFloat( &mPlayerCarHitNPCDelta, "Vehicle Hit NPC Delta", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mPlayerCarHitNPCCoins, "Vehicle Hit NPC Coins", "HitnRun", NULL, NULL, 0, 100, false );
    radDbgWatchAddInt( &mBustedCoins, "Coins lost when busted", "HitnRun", 0, 0, 0, 100, false );
    radDbgWatchAddFloat( &mSwitchSkinDelta, "Switch Skin Delta", "HitnRun", NULL, NULL, 0.0f, -100.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mChangeVehicleDelta, "Change Vehicle Delta", "HitnRun", NULL, NULL, 0.0f, -100.0f, false ); //false for read and write
    radDbgWatchAddInt( &mNumChaseCars, "Number of Chase Vehicles", "HitnRun", NULL, NULL, 0, 5, false );
    radDbgWatchAddFloat( &mDecayDelay, "Delay until meter decay", "HitnRun", NULL, NULL, 0.0f, 5000.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mDecayDelayWhileSpawning, "Decay delay while spawning", "HitnRun", NULL, NULL, 0.0f, 5000.0f, false ); //false for read and write
    radDbgWatchAddFloat( &STOP_HITNRUN, "Spawning Stop Percentage", "HitnRun", NULL, NULL, 0.0f, 100.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mCopTicketDistance, "Cop Ticket Distance", "HitnRun", NULL, NULL, 0.0f, 20.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mCopTicketDistanceOnFoot, "Cop Ticket Distance On Foot", "HitnRun", NULL, NULL, 0.0f, 20.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mCopTicketSpeedThreshold, "Cop Ticket Speed Threshold", "HitnRun", NULL, NULL, 0.0f, 30.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mCopTicketTimeThreshold, "Cop Ticket Time Threshold", "HitnRun", NULL, NULL, 0.0f, 10.0f, false ); //false for read and write
    radDbgWatchAddFloat( &mUnresponsiveTime, "Busted Unresponsive Time", "HitnRun", NULL, NULL, 0.0f, 10000.0f, false ); //false for read and write
#endif
}

//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
HitnRunManager::~HitnRunManager()
{
#ifndef RAD_RELEASE
    radDbgWatchDelete( &mCurrHitnRun );
    radDbgWatchDelete( &mDecayRatePerSecond );
    radDbgWatchDelete( &mDecayRateWhileSpawning );
    radDbgWatchDelete( &mDecayRateInsidePerSecond );
    radDbgWatchDelete( &mVehicleDestroyedDelta );
    radDbgWatchDelete( &mVehicleDestroyedCoins );
    radDbgWatchDelete( &mHitBreakableDelta );
    radDbgWatchDelete( &mVehicleHitDelta );
    radDbgWatchDelete( &mHitBreakableCoins );
    radDbgWatchDelete( &mHitMoveableDelta );
    radDbgWatchDelete( &mHitMoveableCoins );
    radDbgWatchDelete( &mHitKrustyGlassDelta );
    radDbgWatchDelete( &mHitKrustyGlassCoins );
    radDbgWatchDelete( &mColaPropDestroyedDelta );
    radDbgWatchDelete( &mColaPropDestroyedCoins );
    radDbgWatchDelete( &mKickNPCDelta );
    radDbgWatchDelete( &mKickNPCCoins );
    radDbgWatchDelete( &mPlayerCarHitNPCDelta );
    radDbgWatchDelete( &mPlayerCarHitNPCCoins );
    radDbgWatchDelete( &mBustedCoins );
    radDbgWatchDelete( &mSwitchSkinDelta );
    radDbgWatchDelete( &mChangeVehicleDelta );
    radDbgWatchDelete( &mNumChaseCars );
    radDbgWatchDelete( &mDecayDelay );
    radDbgWatchDelete( &mDecayDelayWhileSpawning );
    radDbgWatchDelete( &STOP_HITNRUN );
    radDbgWatchDelete( &mCopTicketDistance );
    radDbgWatchDelete( &mCopTicketDistanceOnFoot );
    radDbgWatchDelete( &mCopTicketSpeedThreshold );
    radDbgWatchDelete( &mCopTicketTimeThreshold );
    radDbgWatchDelete( &mUnresponsiveTime );
#endif
    Destroy( );
    delete [] mObjectsThatNeverGiveCoins;
    mObjectsThatNeverGiveCoins = NULL;
}

//==============================================================================
// Description: Destruction method for all transient data.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
void HitnRunManager::Destroy( void )
{
    GetEventManager()->RemoveAll( this );
}

/*==============================================================================
Description:    This will be called set up as the game session begins.

Parameters:     ( void )

Return:         void 

=============================================================================*/
void HitnRunManager::Init( void )
{
    #ifdef RAD_GAMECUBE
        HeapManager::GetInstance()->PushHeap( GMA_GC_VMM );
    #else
        HeapManager::GetInstance()->PushHeap( GMA_LEVEL_ZONE );
    #endif

	//do some allocations here

    #ifdef RAD_GAMECUBE
        HeapManager::GetInstance()->PopHeap( GMA_GC_VMM );
    #else
        HeapManager::GetInstance()->PopHeap( GMA_LEVEL_ZONE );
    #endif

    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED_BY_USER );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_VEHICLE_COLLISION );
    //GetEventManager()->AddListener( this, EVENT_HIT_BREAKABLE );  //currently get multiple events - don't use
    GetEventManager()->AddListener( this, EVENT_HIT_MOVEABLE );
    GetEventManager()->AddListener( this, EVENT_OBJECT_KICKED );
    GetEventManager()->AddListener( this, EVENT_COLAPROP_DESTROYED ); //event waiting for mike r
    GetEventManager()->AddListener( this, EVENT_KICK_NPC );           //add trigger - complicated
    GetEventManager()->AddListener( this, EVENT_PLAYER_CAR_HIT_NPC );
    
    GetEventManager()->AddListener( this, EVENT_SWITCH_SKIN );
    //GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_END ); //doesn't work for traffic cars
    GetEventManager()->AddListener( this, EVENT_ENTERING_PLAYER_CAR ); 
    GetEventManager()->AddListener( this, EVENT_ENTERING_TRAFFIC_CAR ); 
    GetEventManager()->AddListener( this, EVENT_LEVEL_START );

    GetEventManager()->AddListener( this, EVENT_HIT_AND_RUN_CAUGHT );
    GetEventManager()->AddListener( this, EVENT_CHASE_VEHICLE_SPAWNED );

    GetEventManager()->AddListener( this, EVENT_MISSION_START );
    GetEventManager()->AddListener( this, EVENT_MISSION_SUCCESS );
    GetEventManager()->AddListener( this, EVENT_MISSION_FAILURE );
    GetEventManager()->AddListener( this, EVENT_MISSION_RESET );

    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_START );
    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_END );
    GetEventManager()->AddListener( this, EVENT_EXIT_INTERIOR_START );
    GetEventManager()->AddListener( this, EVENT_EXIT_INTERIOR_END );
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_START );
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE );
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_SKIP );
}

void HitnRunManager::ResetState( void )
{
    mCurrHitnRun = MIN_HITNRUN;
    mSpawnOn = false;
    mChaseOn = false;
    mDecayDisabled = false;
    mHitnRunDisabled = false;
    mAllowThrob = true;
    mVehicleResetTimer = 0.0f;
    mFadeTimer = 0.0f;
    mFadeDone = true;
    mVehicleReset = true;
    GameplayManager* gameplayManager = GetGameplayManager();
    if ( gameplayManager != NULL )
    {
        ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
        if ( chaseManager != NULL )
        {
            //chaseManager->DisableAllActiveVehicleAIs();
            //chaseManager->MarkAllVehiclesForDeletion();
            chaseManager->ClearAllObjects();
        }
    }
}

void HitnRunManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_ENTER_INTERIOR_START:
    case EVENT_EXIT_INTERIOR_START:
    case EVENT_CONVERSATION_START:
        {
            mHitnRunDisabled = true;

            GameplayManager* gameplayManager = GetGameplayManager();
            if ( gameplayManager != NULL )
            {
                ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                if ( chaseManager != NULL )
                {
                    chaseManager->DisableAllActiveVehicleAIs();
                    chaseManager->SuspendAllVehicles();
                }
            }
        }
        break;
    case EVENT_ENTER_INTERIOR_END:
    case EVENT_EXIT_INTERIOR_END:
    case EVENT_CONVERSATION_DONE:
    case EVENT_CONVERSATION_SKIP:
        {
            mHitnRunDisabled = false;

            GameplayManager* gameplayManager = GetGameplayManager();
            if ( gameplayManager != NULL )
            {
                ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                if ( chaseManager != NULL )
                {
                    chaseManager->EnableAllActiveVehicleAIs();
                    chaseManager->ResumeAllVehicles();
                }
            }
        }
        break;
    default:
        break;
    }

    if(!mHitnRunDisabled && !GetInteriorManager()->IsInside())
    {
        float decayDelay = mDecayDelay;
        if(mSpawnOn)
        {
            decayDelay = mDecayDelayWhileSpawning;
        }

        switch ( id )
        {
        case EVENT_VEHICLE_DESTROYED_BY_USER:
            {
                Vehicle* vehicle = static_cast<Vehicle*>( pEventData );
                bool isChaseVehicle = false;
                GameplayManager* gameplayManager = GetGameplayManager();
                if ( gameplayManager != NULL )
                {
                    ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                    if ( chaseManager != NULL )
                    {
                        if(vehicle->mName)
                        {
                            isChaseVehicle = chaseManager->IsModelRegistered(vehicle->mName);
                        }
                    }
                }
                if( !isChaseVehicle && ( vehicle->mVehicleType == VT_TRAFFIC || vehicle->mVehicleType == VT_AI ) && vehicle != mImmunityVehicle)
                {
                    mCurrHitnRun += mVehicleDestroyedDelta;
                    mDecayDelayMS = decayDelay;
                    GetCoinManager()->SpawnCoins(  mVehicleDestroyedCoins, vehicle->GetPosition(), vehicle->GetGroundY() );
                }
            }
            break;
        case EVENT_VEHICLE_VEHICLE_COLLISION:
            {
                CarOnCarCollisionEventData* data = static_cast<CarOnCarCollisionEventData*>( pEventData );
                Vehicle* vehicle = data->vehicle;
                bool isChaseVehicle = false;
                GameplayManager* gameplayManager = GetGameplayManager();
                if ( gameplayManager != NULL )
                {
                    ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                    if ( chaseManager != NULL )
                    {
                        if(vehicle->mName)
                        {
                            isChaseVehicle = chaseManager->IsModelRegistered(vehicle->mName);
                        }
                    }
                }
                if( !isChaseVehicle && ( vehicle->mVehicleType == VT_TRAFFIC || vehicle->mVehicleType == VT_AI ) && vehicle != mImmunityVehicle)
                {
                    if(mDecayDelayMS <= decayDelay/2)
                    {
                        mCurrHitnRun += mVehicleHitDelta;
                        mDecayDelayMS = decayDelay;
                    }
                }
            }
            break;
        case EVENT_HIT_MOVEABLE:
            //case EVENT_HIT_BREAKABLE:
            {
                //currently this assumes hitting a breakable breaks it ;)
                sim::SimState* simState = static_cast<sim::SimState*>(pEventData);
                //if(simState && simState->GetControl() == sim::simAICtrl)
                if(simState)
                {
                    sim::SimControlEnum con = simState->GetControl();
                    if(con == sim::simAICtrl)
                    {
                        CollisionEntityDSG* cedsg = static_cast< CollisionEntityDSG*>(simState->mAIRefPointer);
                        CollisionAttributes* collAttribs = cedsg->GetCollisionAttributes();
                        if(collAttribs)
                        {
                            if(HasntBeenHit((void*)cedsg))
                            {
                                BreakablesEnum::BreakableID id = collAttribs->GetBreakable();            
                                if(id == BreakablesEnum::eKrustyGlassBreaking)
                                {
                                    mCurrHitnRun += mHitKrustyGlassDelta;
                                    GetCoinManager()->SpawnCoins( mHitKrustyGlassCoins, simState->GetPosition() );
                                }
                                else if(id == BreakablesEnum::eNull)
                                {
                                    // Lets check if this thing that we impacted is actually allowed to give
                                    // coins. (e.g. hedges are stateprops that unlike most
                                    // others, should NEVER give coins and are thus in this list
                                    if ( DoesObjectGiveCoins( cedsg ) )
                                    {
                                        mCurrHitnRun += mHitMoveableDelta;
                                        GetCoinManager()->SpawnCoins( mHitMoveableCoins, simState->GetPosition() );
                                    }
                                }
                                else
                                {
                                    mCurrHitnRun += mHitBreakableDelta;
                                    GetCoinManager()->SpawnCoins( mHitBreakableCoins, simState->GetPosition() );
                                }
                                RegisterHit((void*)cedsg);
                                mDecayDelayMS = decayDelay;
                            }
                        }
                    }
                }
            }
            break;
        case EVENT_OBJECT_KICKED:
            {
                CollisionEntityDSG* cedsg = static_cast< CollisionEntityDSG*>(pEventData);
                CollisionAttributes* collAttribs = cedsg->GetCollisionAttributes();
                if(collAttribs)
                {
                    if(HasntBeenHit((void*)cedsg))
                    {

                        StatePropDSG* spdsg = dynamic_cast<StatePropDSG*>(cedsg);
                        if(spdsg)
                        {
                            if(spdsg->GetStatePropUID() == tEntity::MakeUID("waspray"))
                            {
                                break;
                            }
                        }

                        rmt::Vector pos;
                        cedsg->GetPosition(&pos);

                        BreakablesEnum::BreakableID id = collAttribs->GetBreakable();
                        if(id == BreakablesEnum::eKrustyGlassBreaking)
                        {
                            mCurrHitnRun += mHitKrustyGlassDelta;
                            GetCoinManager()->SpawnCoins( mHitKrustyGlassCoins, pos );
                        }
                        else if(id == BreakablesEnum::eNull)
                        {
                            mCurrHitnRun += mHitMoveableDelta;
                            GetCoinManager()->SpawnCoins( mHitMoveableCoins, pos );
                        }
                        else
                        {
                            mCurrHitnRun += mHitBreakableDelta;
                            GetCoinManager()->SpawnCoins( mHitBreakableCoins, pos );
                        }
                        RegisterHit((void*)cedsg);
                        mDecayDelayMS = decayDelay;
                    }
                }
            }
            break;
        case EVENT_COLAPROP_DESTROYED:
            {
                //handles cola state props being destroyed (krustyglass is handled as a breakable)
                mCurrHitnRun += mColaPropDestroyedDelta;
                mDecayDelayMS = decayDelay;
                StatePropDSG* prop = static_cast<StatePropDSG*>( pEventData );
                GetCoinManager()->SpawnCoins( mColaPropDestroyedCoins, prop->rPosition() );
            }
            break;
        case EVENT_KICK_NPC:
            {
                Character* ch = static_cast<Character*>(pEventData);
                mCurrHitnRun += mKickNPCDelta;
                mDecayDelayMS = decayDelay;
                if( !ch->HasBeenHit() )
                {
                    rmt::Vector pos;
                    ch->GetPosition( pos );
                    GetCoinManager()->SpawnCoins( mKickNPCCoins, pos );
                }
            }
            break;
        case EVENT_PLAYER_CAR_HIT_NPC:
            {
                Character* ch = static_cast<Character*>(pEventData);
                if(ch->IsNPC())
                {
                    // TODO:
                    // Make sure this is correct by verifying with Jesse.
                    // We don't use NPCController::FLAILING state anymore..
                    // Need a diff mechanism
                    if( ch->GetStateManager()->GetState() != CharacterAi::INSIM )
                    {
                        mCurrHitnRun += mPlayerCarHitNPCDelta;
                        mDecayDelayMS = decayDelay;
                    }
                    /*
                    NPCharacter* npc = static_cast<NPCharacter*>(ch);
                    NPCController* npcCont = static_cast<NPCController*>(npc->GetController());
                    if(npcCont->GetState() != NPCController::FLAILING)
                    {
                        mCurrHitnRun += mPlayerCarHitNPCDelta;
                        mDecayDelayMS = decayDelay;
                    }
                    */
                }
                if( !ch->HasBeenHit() )
                {
                    rmt::Vector pos;
                    ch->GetPosition( pos );
                    GetCoinManager()->SpawnCoins( mPlayerCarHitNPCCoins, pos );
                }
            }
            break;
        case EVENT_SWITCH_SKIN:
            {
                //switching to same skin not handled currently 
                mCurrHitnRun += mSwitchSkinDelta;
                mDecayDelayMS = decayDelay;
            }
            break;
            //case EVENT_GETINTOVEHICLE_END:
        case EVENT_ENTERING_PLAYER_CAR:
        case EVENT_ENTERING_TRAFFIC_CAR:
            {
                //have to track vehicle type to prevent getting in and out of same car
                //Character* character = static_cast<Character*>(pEventData);
                //Vehicle* vehicle = character->GetTargetVehicle();
                Vehicle* vehicle = static_cast<Vehicle*>(pEventData);
                if(mPrevVehicleID != vehicle->mVehicleID)
                {
                    mCurrHitnRun += mChangeVehicleDelta;
                    mDecayDelayMS = decayDelay;
                }
                mPrevVehicleID = vehicle->mVehicleID;
            }
            break;
        case EVENT_HIT_AND_RUN_CAUGHT: //triggered via collision system
            {
                GameplayManager* gameplayManager = GetGameplayManager();
                if ( gameplayManager != NULL )
                {
                    ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                    if ( chaseManager != NULL && mChaseOn )
                    {
                        chaseManager->DisableAllActiveVehicleAIs();
                        chaseManager->MarkAllVehiclesForDeletion();
                        mCurrHitnRun = MIN_HITNRUN;
                        mSpawnOn = false;
                        mChaseOn = false;
                        GetCoinManager()->LoseCoins( mBustedCoins );

                        Avatar* player = GetAvatarManager()->GetAvatarForPlayer(0);
                        rAssert( player );
                        Character* ch = player->GetCharacter();

                        mVehicleResetTimer = mUnresponsiveTime;
                        mVehicleReset = false;

                        if(ch->GetStateManager()->GetState() == CharacterAi::INCAR)
                        {
                            Vehicle* playerVehicle = player->GetVehicle();
                            //make car unresponsive
                            playerVehicle->SetDisableGasAndBrake(true);
                        }
                    }
                }
            }
            break;
        case EVENT_CHASE_VEHICLE_SPAWNED:
            {
                mChaseOn = true;
            }
            break;
        default:
            {
            }
            break;
        }
    }

    switch ( id ) //do a second switch because these events are handled whether h&r is disabled or not
    {
    //case EVENT_MISSION_START:
    //    {
    //        mLastHitnRunValue = mCurrHitnRun;
    //    }
    //    break;
    //case EVENT_MISSION_RESET: //commented out because it happens all the time before EVENT_MISSION_START
    //    {
    //        if(mLastHitnRunValue < mCurrHitnRun)
    //        {
    //            mCurrHitnRun = mLastHitnRunValue;
    //        }
    //        mSpawnOn = false;
    //        mChaseOn = false;
    //    }
    //    break;
    case EVENT_MISSION_START:
        {
            mCurrHitnRun = MIN_HITNRUN;
            mSpawnOn = false;
            mChaseOn = false;
            mAllowThrob = true;
            GameplayManager* gameplayManager = GetGameplayManager();
            if ( gameplayManager != NULL )
            {
                ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                if ( chaseManager != NULL )
                {
                    chaseManager->DisableAllActiveVehicleAIs();
                    chaseManager->MarkAllVehiclesForDeletion();
                }
            }
        }
        break;
    case EVENT_MISSION_SUCCESS:
        {
            mCurrHitnRun = MIN_HITNRUN;
            mSpawnOn = false;
            mChaseOn = false;
            mDecayDisabled = false;
            mHitnRunDisabled = false;
            mAllowThrob = true;
            GameplayManager* gameplayManager = GetGameplayManager();
            if ( gameplayManager != NULL )
            {
                ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                if ( chaseManager != NULL )
                {
                    chaseManager->DisableAllActiveVehicleAIs();
                    chaseManager->MarkAllVehiclesForDeletion();
                }
            }
        }
        break;
    //case EVENT_MISSION_FAILURE:
    //    {
    //        //if(mLastHitnRunValue < mCurrHitnRun)
    //        //{
    //        //    mCurrHitnRun = mLastHitnRunValue;
    //        //}
    //        mCurrHitnRun = MIN_HITNRUN;
    //        mDecayDisabled = false;
    //        mHitnRunDisabled = false;
    //        mSpawnOn = false;
    //        mChaseOn = false;
    //    }
    //    break;
    case EVENT_GUI_FADE_OUT_DONE:
        {
            mFadeTimer = FADE_TIME;
            mFadeDone = false;
            GetEventManager()->RemoveListener( this, EVENT_GUI_FADE_OUT_DONE );
            GameplayManager* gameplayManager = GetGameplayManager();
            if ( gameplayManager != NULL )
            {
                ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                if ( chaseManager != NULL )
                {
                    chaseManager->ClearAllObjects();
                }
            }
        }
        break;
    default:
        {
        }
        break;

    }
    if(mCurrHitnRun < MIN_HITNRUN)
    {
        mCurrHitnRun = MIN_HITNRUN;
    }
    else if(mCurrHitnRun > MAX_HITNRUN)
    {
        mCurrHitnRun = MAX_HITNRUN;
    }

}





/*=============================================================================
Update the position/animation of all the coins.
=============================================================================*/
void HitnRunManager::Update( int elapsedMS )
{
    if(mHitnRunDisabled)
        return;

    if(mCurrHitnRun < MAX_HITNRUN * (STOP_HITNRUN/100.0f))
    {
        //do chase stuff
        mSpawnOn = false;
    }

    if(mCurrHitnRun < ALLOW_THROB)
    {
        mAllowThrob = true;
    }
    else if(mAllowThrob && (mCurrHitnRun > (mWarningThreshold * 100.0f)))
    {
        mAllowThrob = false;
        GetEventManager()->TriggerEvent( EVENT_HIT_AND_RUN_METER_THROB, NULL );
    }

    GameplayManager* gameplayManager = GetGameplayManager();
    if ( gameplayManager != NULL )
    {
        ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
        if ( chaseManager != NULL )
        {
            if(!mSpawnOn)
            {
                if(mCurrHitnRun >= MAX_HITNRUN)
                {
                    //do chase stuff
                    mSpawnOn = true;
                    chaseManager->SetMaxObjects(mNumChaseCars);
                    chaseManager->SetActive(true);
                    chaseManager->EnableAdd( true );
                    chaseManager->EnableRemove( true );
                    chaseManager->EnableUpdate( true );
                    GetEventManager()->TriggerEvent( EVENT_HIT_AND_RUN_START, NULL );
                }
                else
                {
                    chaseManager->SetMaxObjects(0); //!!!!!!!!!!!!!!!!!!!!!!!!!!!every frame!
                    //chaseManager->SetActive( true );
                    chaseManager->EnableAdd( false );
                    chaseManager->EnableRemove( true );
                    chaseManager->EnableUpdate( true );
                }
            }

            if(mChaseOn)
            {
                int cars = chaseManager->GetNumActiveVehicles();
                if(cars <= 0)
                {
                    if(!mSpawnOn)
                    {
                        mChaseOn = false;
                        GetEventManager()->TriggerEvent( EVENT_HIT_AND_RUN_EVADED, NULL );
                    }
                }
                else
                {
                    //If the player's vehicle speed is < CopTicketSpeedThreshold FOR CopTicketTimeThreshold seconds 
                    //WHILE the distance from the nearest cop is < CopTicketDistance, you get ticketed

                    Avatar* player = GetAvatarManager()->GetAvatarForPlayer(0);
                    rAssert( player );

                    Vehicle* playerVehicle = player->GetVehicle();
                    if( playerVehicle != NULL )
                    {
                        float currSpeed = playerVehicle->mSpeedKmh;
                        float copTicketDistSqr = mCopTicketDistance * mCopTicketDistance;

                        // figure rough distance to player
                        rmt::Vector copPos, playerPos;
                        playerVehicle->GetPosition( &playerPos );
                        float distToPlayerSqr = chaseManager->GetClosestCarPosition( &playerPos, &copPos );

                        if( currSpeed < mCopTicketSpeedThreshold && distToPlayerSqr <  copTicketDistSqr)
                        {
                            mTicketTimer += elapsedMS;
                        }
                        else
                        {
                            mTicketTimer = 0.0f;
                        }
                        //rTunePrintf("dist: %f, timer: %f\n", distToPlayerSqr, mTicketTimer);

                        if( mTicketTimer >= (mCopTicketTimeThreshold*1000.0f)) //convert from seconds to MS
                        {
                            GetEventManager()->TriggerEvent( EVENT_HIT_AND_RUN_CAUGHT, NULL );
                            mTicketTimer = 0.0f;
                        }
                    }
                    else
                    {
                        float copTicketDistSqr = (mCopTicketDistanceOnFoot) * (mCopTicketDistanceOnFoot);

                        // figure rough distance to player
                        rmt::Vector copPos, playerPos;
                        player->GetPosition( playerPos );
                        float distToPlayerSqr = chaseManager->GetClosestCarPosition( &playerPos, &copPos );

                        if( distToPlayerSqr <  copTicketDistSqr)
                        {
                            mTicketTimer += elapsedMS;
                        }
                        else
                        {
                            mTicketTimer = 0.0f;
                        }
                        //rTunePrintf("dist: %f, timer: %f\n", distToPlayerSqr, mTicketTimer);

                        if( mTicketTimer >= (mCopTicketTimeThreshold*1000.0f*2)) //convert from seconds to MS
                        {
                            GetEventManager()->TriggerEvent( EVENT_HIT_AND_RUN_CAUGHT, NULL );
                            mTicketTimer = 0.0f;
                        }

                    }
                }
            }
            else //if(!mChaseOn)
            {
                mTicketTimer = 0.0f;
            }
        }
    }

    if( fabsf(mLastUpdateValue - mCurrHitnRun) > 0.5f)
    {
        //update hud (before decreasing value)
        CGuiScreenHud* hud = GetCurrentHud();
        if(hud)
        {
            hud->SetHitAndRunMeter( mCurrHitnRun / MAX_HITNRUN );
            mLastUpdateValue = mCurrHitnRun;
        }
    }

    if(mDecayDelayMS <= 0.0f)
    {
        if( !mDecayDisabled )
        {
            if(mCurrHitnRun > MIN_HITNRUN)
            {
                float decayRatePerSecond = mDecayRatePerSecond;

                if( GetInteriorManager()->IsInside() )
                {
                    decayRatePerSecond = mDecayRateInsidePerSecond;
                }
                else if(mSpawnOn)
                {
                    decayRatePerSecond = mDecayRateWhileSpawning;
                }


                float delta = decayRatePerSecond * elapsedMS * (-1.0f / 1000.0f);
                mCurrHitnRun += delta;
                if(mCurrHitnRun <= MIN_HITNRUN)
                {
                    GameplayManager* gameplayManager = GetGameplayManager();
                    if ( gameplayManager != NULL )
                    {
                        ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                        if ( chaseManager != NULL )
                        {
                            if(chaseManager->GetNumActiveVehicles() > 0)
                            {
                                chaseManager->MarkAllVehiclesForDeletion();
                            }
                        }
                    }
                    mCurrHitnRun = MIN_HITNRUN;
                }
            }
        }
    }
    else
    {
        mDecayDelayMS -= elapsedMS;
    }

    if(!mVehicleReset)
    {
        if(mVehicleResetTimer <= 0.0f)
        {
            //reset vehicle
            mVehicleReset = true;

            Avatar* player = GetAvatarManager()->GetAvatarForPlayer(0);
            rAssert( player );

            if( player->IsInCar() )
            {
                Vehicle* playerVehicle = player->GetVehicle();
                playerVehicle->SetDisableGasAndBrake(false);
                GetGuiSystem()->HandleMessage( GUI_MSG_MANUAL_RESET, reinterpret_cast< unsigned int >(playerVehicle) );
                GameplayManager* gameplayManager = GetGameplayManager();
                if ( gameplayManager != NULL )
                {
                    ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
                    if ( chaseManager != NULL )
                    {
                        chaseManager->ClearAllObjects();
                    }
                }
            }
            else
            {
                GetEventManager()->AddListener( this, EVENT_GUI_FADE_OUT_DONE );
                GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
                GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_FADE_OUT );
            }

        }
        else
        {
            mVehicleResetTimer -= elapsedMS;
        }
    }

    if(!mFadeDone)
    {
        if(mFadeTimer <= 0.0f)
        {
            //reset vehicle
            mFadeDone = true;
            GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_FADE_IN );
            GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Resume();
        }
        else
        {
            mFadeTimer -= elapsedMS;
        }
    }

}

float HitnRunManager::GetHitnRunValue() const
{
    return mCurrHitnRun;
}

/*=============================================================================
Modify the player's Hit and Run meter
=============================================================================*/
void HitnRunManager::AdjustHitnRunValue( float delta )
{
    mCurrHitnRun += delta;
    if(mSpawnOn)
    {
        mDecayDelayMS = mDecayDelayWhileSpawning;
    }
    else
    {
        mDecayDelayMS = mDecayDelay;
    }

    if(mCurrHitnRun < MIN_HITNRUN)
    {
        mCurrHitnRun = MIN_HITNRUN;
    }
    else if(mCurrHitnRun > MAX_HITNRUN)
    {
        mCurrHitnRun = MAX_HITNRUN;
    }

}


/*=============================================================================
Force the player's Hit and Run meter to a specific value
=============================================================================*/
void HitnRunManager::SetHitnRunValue( float value )
{
    rAssert(value <= MAX_HITNRUN && value >= MIN_HITNRUN);
    mCurrHitnRun = value;
}

void HitnRunManager::MaxHitnRunValue()
{
    mCurrHitnRun = MAX_HITNRUN;
}

bool HitnRunManager::HasntBeenHit( void* ptr )
{
    for(int i = 0; i < 16; i++)
    {
        if( mPrevHits[i] == ptr )
        {
            return false;
        }
    }
    return true;
}

void HitnRunManager::RegisterHit( void* ptr )
{
    mPrevHits[mLeastRecentlyHit] = ptr;
    mLeastRecentlyHit++;
    if(mLeastRecentlyHit >= 16)
    {
        mLeastRecentlyHit = 0;
    }
}


bool HitnRunManager::DoesObjectGiveCoins( CollisionEntityDSG* cedsg )
{
    bool givesCoins = true;

    // We want to get the type name, not the instance name. Get the type name from a
    // stateprop from its embedded CStateProp object
    if ( cedsg->GetAIRef() == PhysicsAIRef::StateProp )
    {
        const StatePropDSG* spdsg = static_cast< const StatePropDSG* >( cedsg );
        rAssert( dynamic_cast< StatePropDSG* >( cedsg ) != NULL );
        tUID typeName = spdsg->GetStatePropUID();
        for ( int i = 0 ; i < NUM_OBJECTS_THAT_NEVER_GIVE_COINS ; i++ )
        {
            if ( typeName == mObjectsThatNeverGiveCoins[i] )
            {
                // Its a match, this object does not give coins
                givesCoins = false;
                break;
            }
        }
    }
    return givesCoins;
}

void HitnRunManager::RegisterVehicleImmunity( Vehicle* v )
{
    mImmunityVehicle = v;
}
