//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        missionmanager.cpp
//
// Description: Implement MissionManager
//
// History:     15/04/2002 + Created -- NAME
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

#include <ai/vehicle/vehicleai.h>

#include <events/eventmanager.h>

#include <contexts/context.h>
#include <contexts/pausecontext.h>
#include <gameflow/gameflow.h>

#include <loading/loadingmanager.h>

#include <meta/carstartlocator.h>
#include <meta/eventlocator.h>
#include <meta/locator.h>
#include <meta/triggervolumetracker.h>
#include <meta/zoneeventlocator.h>
#include <events/eventenum.h>
#include <meta/eventlocator.h>

#include <data/gamedatamanager.h>
#include <data/PersistentWorldManager.h>

#include <mission/mission.h>
#include <mission/missionmanager.h>
#include <mission/missionstage.h>
#include <mission/missionscriptloader.h>
#include <mission/conditions/missioncondition.h>
#include <mission/objectives/deliveryobjective.h>
#include <mission/objectives/gotoobjective.h>
#include <mission/objectives/missionobjective.h>
#include <mission/rewards/rewardsmanager.h>
#include <mission/rewards/reward.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/backend/guimanagerbackend.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>
#include <presentation/gui/ingame/guimanageringame.h>

#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/loaders/billboardwrappedloader.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/worldobject.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <memory/srrmemory.h>

#include <mission/animatedicon.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
MissionManager* MissionManager::spInstance = NULL;


#ifdef DEBUGWATCH
static bool gResetCurrentMission = false;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//=============================================================================
// MissionManager::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MissionManager
//
//=============================================================================
MissionManager* MissionManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}

//=============================================================================
// MissionManager::CreateInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MissionManager
//
//=============================================================================
MissionManager* MissionManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "MissionManager" );
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) MissionManager;
        rAssert( spInstance );
    }
MEMTRACK_POP_GROUP( "MissionManager" );
    
    return spInstance;
}

//=============================================================================
// MissionManager::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::DestroyInstance()
{
    if( spInstance != NULL )
    {
        delete( GMA_PERSISTENT, spInstance );
        spInstance = NULL;
    }
}

//==============================================================================
// MissionManager::MissionManager
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionManager::MissionManager() :
    mLoadingState( STATE_INVALID ),
    mMissionState( MISSION_INVALID ),
    mIsSundayDrive( true ),
    mResetting( true ),
    mHAHACK( false ),
    mCollectionEffect( NULL )
{
    SetNumPlayers( 1 );

    mLastFileName[0] = '\0';

#ifdef DEBUGWATCH
    radDbgWatchAddBoolean( &gResetCurrentMission, "Reset Current Mission", "Mission Manager" );
#endif
}

//==============================================================================
// MissionManager::~MissionManager
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionManager::~MissionManager()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &gResetCurrentMission );
#endif

    if ( mCollectionEffect )
    {
        delete mCollectionEffect;
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// MissionManager::Initialize
//=============================================================================
// Description: Fucking Chuck keeps shooting me
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::Initialize()
{
    //****** Dirty Hacky to get Replay to work, ask Carey More about this !!!!!!! Chuck 0ct12.
    //Missed this one...  Thanks Chuck.  This makes the SD/actual mission thing work.
    //Remind me to hunt down and kill Darryl for this.
    mResetting=true;
    GameplayManager::Initialize();

    Reward* defaultReward = NULL;

    // unlock default car for current level
    //
    defaultReward = GetRewardsManager()->GetReward( this->GetCurrentLevelIndex(), Reward::eDefaultCar );
    rAssert( defaultReward );
    if ( defaultReward != NULL )
    {
        defaultReward->UnlockReward();

        // unlock default skin for current level
        //

        defaultReward = GetRewardsManager()->GetReward( this->GetCurrentLevelIndex(), Reward::eDefaultSkin );
        rAssert( defaultReward );
        if ( defaultReward != NULL )
            defaultReward->UnlockReward();
    }

    GetEventManager()->AddListener( this, EVENT_FIRST_DYNAMIC_ZONE_END );
    GetEventManager()->AddListener( this, EVENT_GUI_LEAVING_PAUSE_MENU );
}

//=============================================================================
// MissionManager::Finalize
//=============================================================================
// Description: Take the vein train down the A-tube
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::Finalize()
{
    GetEventManager()->RemoveAll( this );

    if ( mCollectionEffect )
    {
        delete mCollectionEffect;
        mCollectionEffect = NULL;
    }

    GameplayManager::Finalize();
}

//=============================================================================
// MissionManager
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::Update( int elapsedTime )
{
#ifdef DEBUGWATCH
    if ( gResetCurrentMission )
    {
        RestartCurrentMission();
        gResetCurrentMission = false;
    }
#endif

    if( mLoadingState != STATE_INVALID )
    {
        PerformLoading();
    }
    else if ( mMissionState == MISSION_INIT )
    {
        //We're here because the loading is all done, and it is time to set up the current mission.
        //I think this is better than an incestuous relationship with the GUI...  WTF Darryl?

        Mission* currentMission = GetCurrentMission();
        currentMission->Reset( mResetting );

        //Start right away.
        currentMission->GetCurrentStage()->Start();
        mMissionState = MISSION_RUNNING;

        if ( currentMission->IsSundayDrive() )
        {
            //Reenable the bonus missions.
            EnableBonusMissions();
        }
        else
        {
            HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
            GetEventManager()->TriggerEvent( EVENT_MISSION_START, reinterpret_cast< void* >( currentMission ) );
            HeapMgr()->PopHeap (GMA_LEVEL_MISSION);

            //No bonus missions while we're in a mission.
            DisableBonusMissions();
        }

        if ( mResetting || currentMission->IsForcedCar() || currentMission->IsRaceMission() )
        {
            if ( mResetting || mMissionState != MISSION_RUNNING )
            {
                currentMission->ResetPlayer();
                GetEventManager()->TriggerEvent( EVENT_MISSION_CHARACTER_RESET, (void*)1 );
//				GetCharacterManager()->Update(0.033f);
  				BEGIN_PROFILE("WorldPhysics");
  					GetWorldPhysicsManager()->Update(33);
  				END_PROFILE("WorldPhysics");
            }

            mResetting = false;

            //HACK
            if ( mHAHACK )
            {
                GetGuiSystem()->HandleMessage( GUI_MSG_RESUME_INGAME );
                mHAHACK = false;
            }
        }
        else
        {
            GetEventManager()->TriggerEvent( EVENT_MISSION_CHARACTER_RESET, (void*)0 );
        }
    }
    else if ( mMissionState == MISSION_SUSPEND )
    {
        GetEventManager()->RemoveListener( GetCurrentMission(), EVENT_GUI_MISSION_START );
        mMissionState = MISSION_RUNNING;
     }
    else
    {
        GameplayManager::Update( elapsedTime );

        if ( mMissionState == MISSION_RUNNING && mLoadingState == STATE_INVALID )
        {
            if ( mIrisClosed )
            {
                PauseForIrisOpen();
            }
            else if ( mFadedToBlack )
            {
                //check if there is manual control of the fade in/out 
                //this could be called by the mission stage swapcarstart
                //to hide the loading
                if (QueryManualControlFade() !=true)
                {
                    PauseForFadeFromBlack();
                }
            }
        }
    }

    if ( mCollectionEffect )
    {
        mCollectionEffect->Update( elapsedTime );
    }
}

//=============================================================================
// MissionManager::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::Reset()
{
    // start the first mission

    Mission* mission = GetCurrentMission();
    rAssert( mission != NULL );

    mission->Reset();

    MissionStage* stage = mission->GetCurrentStage();
    rAssert( stage != NULL );
    stage->Start();

    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
    GetEventManager()->TriggerEvent( EVENT_MISSION_START );
    HeapMgr()->PopHeap (GMA_LEVEL_MISSION);

    //Hmmm.  Disable the bonus missions.
    DisableBonusMissions();
}

//=============================================================================
// virtual void HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      virtual 
//
//=============================================================================
void MissionManager::HandleEvent( EventEnum id, void* pEventData )
{
    if( mLoadingState == STATE_WAIT_FOR_DYNALOAD && id == EVENT_FIRST_DYNAMIC_ZONE_END )
    {
         mLoadingState = STATE_MISSION_START;

        if ( !GetCurrentMission()->IsSundayDrive() && CommandLineOptions::Get( CLO_SKIP_SUNDAY ) || mIsDemo )
        {
            if ( mIsDemo )
            {
                if ( GetGameFlow()->GetNextContext() != CONTEXT_DEMO )
                {
                    GetGameFlow()->SetContext( CONTEXT_DEMO );
                }
            }
            else
            {
                rTuneAssert(GetGameFlow()->GetCurrentContext() != CONTEXT_PAUSE && GetGameFlow()->GetNextContext() != CONTEXT_PAUSE && GetGameFlow()->GetNextContext() != CONTEXT_GAMEPLAY);
                if (GetGameFlow()->GetCurrentContext() != CONTEXT_PAUSE && GetGameFlow()->GetNextContext() != CONTEXT_PAUSE && GetGameFlow()->GetNextContext() != CONTEXT_GAMEPLAY )
                {
                    //MAYBE bad - could return us to gameplay context while in pause screen
                    GetGameFlow()->SetContext( CONTEXT_GAMEPLAY );
                }
            }
        }
    }
    else if ( id == EVENT_GUI_LEAVING_PAUSE_MENU )
    {
        if ( mMissionState != MISSION_SUSPEND )
        {
            //Reenable the gameplay...
            GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Resume();
        }
    }
    else
    {
        Mission* pMission = GetCurrentMission();
        if( pMission != NULL )
        {
            pMission->HandleEvent( id, pEventData );
        }
    }

    GameplayManager::HandleEvent(id, pEventData);
}

//=============================================================================
// MissionManager::PerformLoading
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::PerformLoading()
{
MEMTRACK_PUSH_GROUP( "MissionManager - Loading" );  
    switch( mLoadingState )
    {
    case STATE_LEVEL:
    case STATE_MISSION_LOADING:
        {
            if( GetLoadingManager()->IsLoading() || GetVehicleCentral()->GetCurrentVehicleUnderConstruction() != NULL )
            {
                break;
            }

            mLoadingState = STATE_MISSION_INIT;
            break;
        }
    case STATE_MISSION_INITING:
        {
            if( GetLoadingManager()->IsLoading() || GetVehicleCentral()->GetCurrentVehicleUnderConstruction() != NULL)
            {
                break;
            }

            mLoadingState = STATE_MISSION_DYNALOAD;           
            break;
        }
    case STATE_MISSION_LOAD:
        {
            #ifdef RAD_DEBUG
                static int missionsLoaded = 0;
                ++missionsLoaded;
                char label[ 256 ];
                sprintf( label, "mission%d", missionsLoaded );
                SetMemoryIdentification( label );
            #endif
            mLoadingState = STATE_MISSION_LOADING;

            Mission* mission = GetCurrentMission();
            rAssert( mission != NULL );

            //Load the mission data.
            int levelnum = GetCurrentLevelIndex() + 1;
            sprintf( mLastFileName, "scripts\\missions\\level0%d\\%sl.mfk", levelnum, mission->GetName() );

            HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
            GetMissionScriptLoader()->SetFileHander( FILEHANDLER_MISSION );
            GetMissionScriptLoader()->LoadScriptAsync( mLastFileName );
            HeapMgr()->PopHeap(GMA_LEVEL_MISSION);

            break;
        }
    case STATE_MISSION_INIT:
        {
            HeapMgr()->PushHeap (GMA_LEVEL_MISSION);

            CGuiManagerInGame* pCGuiManagerInGame = NULL;

            pCGuiManagerInGame= GetGuiSystem()->GetInGameManager();
            //if the screen is black or there is a loading screen up
            //can we clean up cars.

            if (pCGuiManagerInGame != NULL)
            {
                if (        GetGameplayManager()->IsIrisClosed() 
                        || 
                            pCGuiManagerInGame->IsLoadingNewMissionInSundayDrive()
                        || 
                            pCGuiManagerInGame->IsLoadingNewMission()                        
                    )
                {
                    GetGameplayManager()->MDKVDU();
                   
                }
            }
            mLoadingState = STATE_MISSION_INITING;

            Mission* mission = GetCurrentMission();
            rAssert( mission != NULL );

            // MS9: See previous cursing
            GetTriggerVolumeTracker()->SetTriggerSphere( p3d::find<tDrawable>( "triggersphere_000" ));

            rAssert( mCollectionEffect == NULL );

            //Set up the collection effect.
            mCollectionEffect = new AnimatedIcon();
            mCollectionEffect->Init( "mission_col", rmt::Vector( 0.0f, 0.0f, 0.0f ), false, true );

            //Setup the default car if required
            if ( mShouldLoadDefaultVehicle )
            {
                AddLevelVehicle( mDefaultLevelVehicleName, eDefaultCar, mDefaultLevelVehicleConfile );
                PlaceVehicleAtLocatorName( GetVehicleInSlot( eDefaultCar ), mDefaultLevelVehicleLocator );
                SetCurrentVehicle( GetVehicleInSlot( eDefaultCar ) );

                mShouldLoadDefaultVehicle = false;
            }

            char filename[128];

            //Init the mission data
            int levelnum = GetCurrentLevelIndex() + 1;

            GetCharacterManager()->ClearInitialWalk();

            sprintf( filename, "scripts\\missions\\level0%d\\%si.mfk", levelnum, mission->GetName() );

            GetMissionScriptLoader()->SetFileHander( FILEHANDLER_MISSION );
            GetMissionScriptLoader()->LoadScriptAsync( filename );

            //
            // NOTE: if the script gets changed to load things (which it shouldn't)
            // then this should call ProcessRequests instead, and STATE_MISSION_INITING
            // should just break (since the gameplaymanager is spinning)
            //
            PerformLoading();
            //GetLoadingManager()->ProcessRequests( this );

            HeapMgr()->PopHeap (GMA_LEVEL_MISSION);

            break;
        }
    case STATE_MISSION_DYNALOAD:
        {
            Mission* mission = GetCurrentMission();
    
            mission->Initialize( GetCurrentMissionHeap() );
            
            mLoadingState = STATE_WAIT_FOR_DYNALOAD;
            //chuck remove the default vehicle if user has selected a traffic car as current car

            if ( GetCurrentVehicle() != mVehicleSlots[eDefaultCar].mp_vehicle)
            {
                ClearVehicleSlot(eDefaultCar);
            }            
            
            //check if we are in the orange loading screen. if we are then lets clear some traffic 
            //so we dont have user traffic cars spawning on a parked user traffic car.
            if (
                        (GetGuiSystem()->GetBackendManager()->GetCurrentScreen() == CGuiWindow::GUI_SCREEN_ID_LOADING) 
                
                &&
                         GetGuiSystem()->GetBackendManager()->GetCurrentWindow()->IsRunning()
                
                )
            {
                rmt::Vector position(0,0,0);            
                rmt::Sphere nuke(position, 1000.0f);                
                TrafficManager::GetInstance()->ClearTrafficInSphere(nuke);
                GetGameplayManager()->MDKVDU();
            }

            if ( mResetting )
            {
                //HeapMgr()->PushHeap (GMA_LEVEL_ZONE);   // Is this the right one??  -- jdy
                mission->InitDynaLoad();
                //HeapMgr()->PopHeap ();
            }

            // Really there should always be stages, but some of
            // the test levels don't have any
            //
            if( mission->GetNumStages() == 0 || !mResetting ) // || !mission->GetStage( 0 )->LoadMissionStart() )
            {
                mLoadingState = STATE_MISSION_START;
            }

            break;
        }
    case STATE_WAIT_FOR_DYNALOAD:
        {
            break;
        }
    case STATE_MISSION_START:
        {
            if( GetCurrentMission()->IsSundayDrive() )
            {

                GameFlow* gf = GetGameFlow();
                ContextEnum currcon = gf->GetCurrentContext();
                ContextEnum nextcon = gf->GetNextContext();

                //Try this.
                if( GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_DEMO )
                {
                    GetGameFlow()->SetContext( CONTEXT_DEMO );
                }
                else if ( GetGameFlow()->GetNextContext() != CONTEXT_GAMEPLAY )
                {
                    if ( GetGameFlow()->GetCurrentContext() != CONTEXT_PAUSE || 
                        ( GetGameFlow()->GetCurrentContext() == CONTEXT_PAUSE && GetPauseContext()->IsWaitingForContextSwitch()))
                    {
                        GetGameFlow()->SetContext( CONTEXT_GAMEPLAY );
                        GetPauseContext()->SetWaitingForContextSwitch(false);
                    }
                }
                //set the Dirty Flag in the player car info struct to record stuff
                mPlayerAndCarInfo.mbDirtyFlag = false;
            }
            else
            {
                HeapMgr()->PushHeap (GMA_LEVEL_MISSION);

                //Chuck: Do our exception handling while we haven't officially started the mission

                Mission* pCurrentMission= NULL;

                pCurrentMission =  GetCurrentMission();

                //if we are going into a new non sunday check to see if we can repair the users car
                //if and only if its a non main car ie traffic etc
                if (pCurrentMission->IsSundayDrive() != true)
                {
                    if(GetCurrentVehicle() != NULL)
                    {
                        if (GetCurrentVehicle()->IsVehicleDestroyed() == true)
                        {
                            //if the car is not owned by the player then it could be a traffic car free car or car from a cheat at the phone booth
                            // if car index returns a -1 then the car is not owned by the player
                            if (GetCurrentVehicle()->mVehicleType == VT_TRAFFIC) 
                            {
                                GetEventManager()->TriggerEvent(EVENT_REPAIR_CAR);
                            }
                        }
                    }
                }


                //clear traffic at the start of these mission as per designer request
                if (pCurrentMission->IsWagerMission() ==true)
                {
                    //remove traffic
                    //get a traffic ptr
                    ITrafficSpawnController* pTraffic = TrafficManager::GetSpawnController();
                    pTraffic->DisableTraffic();
                    rmt::Vector vector;
                    GetCharacterManager()->GetCharacter(0)->GetPosition(&vector);    
                    rmt::Sphere sphere (vector, 400.0f);
                    pTraffic->ClearTrafficInSphere( sphere);            
                }

                // if the user is using something other than their default car at the start of these missions then remove them.
                if(pCurrentMission->IsSundayDrive() != true )
                {
                    //also remove the user default car if they have hijacked a traffic or parked car or etc so as not to impede the race
                    if (GetCurrentVehicle() != mVehicleSlots[eDefaultCar].mp_vehicle)
                    {
                        ClearVehicleSlot(eDefaultCar);
                    }
                }

                if (pCurrentMission->IsForcedCar() == true)
                {
                    //record players position and location of forced car spawn pt

                    if(mPlayerAndCarInfo.mbDirtyFlag != true)
                    {
                        GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter()->GetPosition( mPlayerAndCarInfo.mPlayerPosition);
                        pCurrentMission->GetVehicleRestart()->GetLocation(&mPlayerAndCarInfo.mForceLocation);
                        mPlayerAndCarInfo.mbDirtyFlag = true;
                    }
                } 

                Mission* mission = GetCurrentMission();
                mission->Reset();

                if (GetGameplayManager()->IsIrisClosed() == true)
                {
                    GetGameplayManager()->PauseForIrisOpen( 1.0f);
                }


                if ( mResetting || mission->IsForcedCar() || mission->IsRaceMission() )
                {
                    GetCurrentMission()->ResetPlayer();
//					GetCharacterManager()->Update(0.033f);
	  				BEGIN_PROFILE("WorldPhysics");
	  					GetWorldPhysicsManager()->Update(33);
	  				END_PROFILE("WorldPhysics");
                }
                
                // Tell the GUI to hide the mission loading screen
                //
                GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_MISSION_LOAD_END );

                HeapMgr()->PopHeap (GMA_LEVEL_MISSION);
            }

            mLoadingState = STATE_INVALID;

            mMissionState = MISSION_INIT;           
           
            break;
        }
    case STATE_INVALID:
        {
            break;
        }
    default:
        // fook!
        rAssert( false );
        break;
    }
MEMTRACK_POP_GROUP( "MissionManager - Loading" );
}

//=============================================================================
// MissionManager::RestartCurrentMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::RestartCurrentMission()
{
    mResetting = true;

    GetVehicleCentral()->DetachAllCollectibles();

    //Oh boy..  We need to dump everything else and don't know what it is...
    //GetRenderManager()->pWorldRenderLayer()->DumpAllDynaLoads( 1, GetRenderManager()->mEntityDeletionList);
    GetRenderManager()->mpLayer( RenderEnums::LevelSlot )->DumpAllDynaLoads(1, GetRenderManager()->mEntityDeletionList );

    SetCurrentMission( GetCurrentMissionNum() );
}

//=============================================================================
// MissionManager::RestartToMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( RenderEnums::MissionEnum mission )
//
// Return:      void 
//
//=============================================================================
void MissionManager::RestartToMission( RenderEnums::MissionEnum mission )
{
    mResetting = true;

    //Oh boy..  We need to dump everything else and don't know what it is...
    //GetRenderManager()->pWorldRenderLayer()->DumpAllDynaLoads( 1, GetRenderManager()->mEntityDeletionList );
    GetRenderManager()->mpLayer( RenderEnums::LevelSlot )->DumpAllDynaLoads(1, GetRenderManager()->mEntityDeletionList );

    GetCharacterManager()->AllowGarbageCollection(true); 
    GetCharacterManager()->GarbageCollect(true);

    SetCurrentMission( mission * 2 + mSkipSunday );

    //HACK: until the screen thing is resolved by Carey and Joe.
    mHAHACK = true;
}

//=============================================================================
// MissionManager::AbortCurrentMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::AbortCurrentMission()
{   
    rAssert( !GetCurrentMission()->IsSundayDrive() ); //This should NEVER happen.

    if ( GetCurrentMission()->IsForcedCar() )
    {
        PauseForIrisClose( 1.0f );
        mShouldLoadDefaultVehicle = true;  //This is hackish
    }

    PrevMission();  //This should take us to the SD for this mission.
}


//=============================================================================
// MissionManager::LoadLevelData
//=============================================================================
// Description: 
//
// Parameters:  ( char* name )
//
// Return:      void 
//
//=============================================================================
void MissionManager::LoadLevelData()
{
    mLoadingState = STATE_LEVEL;

    char name[128];

    if ( !mIsDemo )
    {
#ifdef RAD_DEMO
        if( GetCurrentLevelIndex() == RenderEnums::L7 )
        {
            sprintf( name, "scripts\\missions\\level0%d\\level-d.mfk", GetCurrentLevelIndex() + 1 );
        }
        else
#endif
        {
            sprintf( name, "scripts\\missions\\level0%d\\level.mfk", GetCurrentLevelIndex() + 1 );
        }
    }
    else
    {
        sprintf( name, "scripts\\missions\\level0%d\\demo.mfk", GetCurrentLevelIndex() + 1 );
    }

    GetPersistentWorldManager()->OnLevelLoad( GetCurrentLevelIndex() );
    BillboardWrappedLoader::OverrideLoader( true );
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );  // I choose other because this stuff will exist longer than a single mission
    GetMissionScriptLoader()->SetFileHander( FILEHANDLER_LEVEL );
    GetMissionScriptLoader()->LoadScriptAsync( name );
    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
    BillboardWrappedLoader::OverrideLoader( false );
}

//=============================================================================
// MissionManager::InitLevelData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::InitLevelData()
{
    //WARNING!  NO data should be loaded here!

    char name[128];

    if ( !mIsDemo )
    {
#ifdef RAD_DEMO
        if( GetCurrentLevelIndex() == RenderEnums::L7 )
        {
            sprintf( name, "scripts\\missions\\level0%d\\leveli-d.mfk", GetCurrentLevelIndex() + 1 );
        }
        else
#endif
        {
            sprintf( name, "scripts\\missions\\level0%d\\leveli.mfk", GetCurrentLevelIndex() + 1 );
        }
    }
    else
    {
        sprintf( name, "scripts\\missions\\level0%d\\demoi.mfk", GetCurrentLevelIndex() + 1 );
    }

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    GetMissionScriptLoader()->SetFileHander( FILEHANDLER_LEVEL );
    GetMissionScriptLoader()->LoadScriptAsync( name );
    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}

//=============================================================================
// MissionManager::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void MissionManager::OnProcessRequestsComplete( void* pUserData )
{
    switch( mLoadingState )
    {
    case STATE_MISSION_LOADING:
        {

            //Turn off the override.
            BillboardWrappedLoader::OverrideLoader( false );
            mLoadingState = STATE_MISSION_INIT;

            break;
        }
    case STATE_MISSION_INITING:
        {
            mLoadingState = STATE_MISSION_DYNALOAD;
            break;
        }
    default:
        // fook!
        rAssert( false );
    }
    GetRenderManager()->pWorldRenderLayer()->DoPostDynaLoad();

}

//=============================================================================
// MissionManager::LoadMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::LoadMission()
{

    //If this is a Sunday Drive mission, we are not going to load anything.
    //But we should display the new post-mission screen.
    //TODO POST MISSION SCREEN

    if ( !GetCurrentMission()->IsSundayDrive() && !CommandLineOptions::Get( CLO_SKIP_SUNDAY ) )
    {
        // Tell the GUI to show the mission loading screen
        //
        GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_MISSION_LOAD_BEGIN );

        //What the heck is this for?  TODO: Remove?
        //GetEventManager()->TriggerEvent( EVENT_MISSION_INTRO );
    }

    // Clear our the p3d inventory
    //
    p3d::pddi->DrawSync();
    p3d::inventory->RemoveSectionElements( "Mission" );

    if ( mCollectionEffect )
    {
        delete mCollectionEffect;
        mCollectionEffect = NULL;
    }

    // Instead of processing mission flow, each Update performs loading
    //
    mLoadingState = STATE_MISSION_LOAD;
}

//=============================================================================
// MissionManager::CleanMissionData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionManager::CleanMissionData()
{
//DEVIN LOOK AT THIS EVIL Memory stompage!!!!!
    p3d::pddi->DrawSync();

    if ( mLastFileName[0] != '\0' )
    {
        HeapMgr()->PushHeap (GMA_TEMP);
        tName name( mLastFileName );
        HeapMgr()->PopHeap (GMA_TEMP);
/*
		//Doing this because dumps were occuring in the middle of other loads
		//which = BADNESS
		char tempName[100];
		strcpy( tempName, mLastFileName );
		tempName[strlen(tempName)] = ':';
		tempName[strlen(tempName)] = '\0';

		ZoneEventLocator* pZEL = new(GMA_TEMP) ZoneEventLocator;
		pZEL->SetName("gawdDamn");
		pZEL->AddRef();
		pZEL->AddRef();
        pZEL->SetZoneSize( strlen(tempName) + 3 );
        pZEL->SetZone( tempName );
        pZEL->SetPlayerEntered();

        GetEventManager()->TriggerEvent( (EventEnum)(EVENT_LOCATOR+LocatorEvent::DYNAMIC_ZONE), 
                                         static_cast<void*>( pZEL ) );

		//THIS IS A LEAK!

  */
        GetRenderManager()->pWorldRenderLayer()->DumpDynaLoad( name, GetRenderManager()->mEntityDeletionList );
    }

    p3d::inventory->PushSection();
 
    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
    p3d::inventory->SelectSection( "Mission" );

    HeapMgr()->PushHeap( GMA_TEMP );
    tInventory::Iterator<Locator> iterator;
    HeapMgr()->PopHeap( GMA_TEMP );

    Locator* loc = iterator.First();

    while( loc != NULL )
    {
        TriggerVolume* volume;

        unsigned int i;
        for( i = 0; i < loc->GetNumTriggers(); i++ )
        {
            volume = reinterpret_cast<TriggerLocator*>(loc)->GetTriggerVolume( i );
            GetTriggerVolumeTracker()->RemoveTrigger( volume );
        }        

        loc = iterator.Next();
    }

    if ( mCollectionEffect )
    {
        delete mCollectionEffect;
        mCollectionEffect = NULL;
    }

    //
    // TODO: This string shouldn't be hardcoded
    //
    p3d::inventory->RemoveSectionElements( "Mission" );
    HeapMgr()->PopHeap (GMA_LEVEL_MISSION);

    p3d::inventory->PopSection();

}

//=============================================================================
// MissionManager::PutEffectHere
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& pos )
//
// Return:      void 
//
//=============================================================================
void MissionManager::PutEffectHere( const rmt::Vector& pos )
{
    mCollectionEffect->Reset();
    mCollectionEffect->Move( pos );
    mCollectionEffect->ShouldRender( true );
}
