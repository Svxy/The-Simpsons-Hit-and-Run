//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement PauseContext
//
// History:     21/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <p3d/view.hpp>
#include <p3d/shadow.hpp>

// Foundation Tech
#include <raddebug.hpp>
#include <radtime.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <ai/vehicle/vehicleairender.h>

#include <contexts/pausecontext.h>
#include <contexts/contextenum.h>
#include <memory/leakdetection.h>
#include <memory/srrmemory.h>
#include <worldsim/worldobject.h>

#include <camera/supercammanager.h>
#include <camera/animatedcam.h>
#include <interiors/interiormanager.h>
#include <meta/triggervolumetracker.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/utility/hudmap.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>
#include <render/DSG/StatePropDSG.h>
#include <sound/soundmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/footprint/footprintmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/ped/pedestrianmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>
#include <worldsim/hitnrunmanager.h>
#include <worldsim/parkedcars/parkedcarmanager.h>
#include <ai/actionbuttonmanager.h>
#include <render/breakables/breakablesmanager.h>
#include <render/particles/ParticleManager.h>
#include <worldsim/skidmarks/skidmarkgenerator.h>
#include <worldsim/skidmarks/skidmarkmanager.h>
#include <ai/actor/actormanager.h>
#include <gameflow/gameflow.h>
#include <roads/roadmanager.h>
#include <render/Loaders/AnimDynaPhysLoader.h>
#include <pedpaths/pathmanager.h>

#include <input/inputmanager.h>

#include <data/gamedatamanager.h>

#include <mission/animatedicon.h>

#include <render/animentitydsgmanager/animentitydsgmanager.h>
#include <presentation/presentation.h>
#include <presentation/tutorialmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
PauseContext* PauseContext::spInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// PauseContext::GetInstance
//==============================================================================
//
// Description: - Access point for the PauseContext singleton.  
//              - Creates the PauseContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the PauseContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
PauseContext* PauseContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) PauseContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//==============================================================================
// PauseContext::PauseContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PauseContext::PauseContext()
:   mOldState( Input::ACTIVE_NONE ),
    m_quitGamePending( false ),
    m_waitingForContextSwitch( false )
{
}

//==============================================================================
// PauseContext::~PauseContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PauseContext::~PauseContext()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// PauseContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void PauseContext::OnStart( ContextEnum previousContext )
{
    SetMemoryIdentification( "PauseContext" );
    MEMTRACK_PUSH_FLAG( "Pause" );

    mOldState = GetInputManager()->GetGameState();
    if( mOldState == Input::ACTIVE_ANIM_CAM )
    {
        // deactivate anim cam state first, since the input manager
        // won't let us set the game state to anything else prior
        // to that
        //
        GetInputManager()->SetGameState( Input::DEACTIVE_ANIM_CAM );
        SuperCam* sc = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam();
        AnimatedCam* ac = dynamic_cast<AnimatedCam*>(sc);
        if(ac)
        {
            ac->Abort();
        }
    }
    GetInputManager()->SetGameState( Input::ACTIVE_FRONTEND );

	GetCoinManager()->ClearHUDCoins();

    m_quitGamePending = false;
}


extern void OutputHandler (const char * pString );


//=============================================================================
// PauseContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void PauseContext::OnStop( ContextEnum nextContext )
{
    // do gameplay terminations only if next context is frontend context or exit.
    if( nextContext == CONTEXT_FRONTEND ||
        nextContext == CONTEXT_LOADING_GAMEPLAY ||
        nextContext == CONTEXT_EXIT )
    {
        rReleasePrintf("PauseContext::OnStop Begins\n");

        GetInputManager()->EnableReset( false );

        RenderLayer* l = GetRenderManager()->mpLayer( RenderEnums::GUI );
        rAssert( l );
        l->Thaw();
//        GetGameFlow()->SetQuickStartLoading( false );
        // STL reallocs, etc may result in actually allocating some memory during this process.
        // As such, reroute everything to temp.
        //
        HeapMgr()->PushHeap (GMA_TEMP);

        //
        // This is called to prevent DMA of destroyed textures,
        // because we don't swap buffers until the next frame.
        //
        p3d::pddi->DrawSync();

        //::radThreadSleep (500);

        //::rDebugSetOutputHandler (OutputHandler);

        //rReleaseString ("DrawSync\n");

        // Clear the shadow list
        AnimDynaPhysLoader::ClearShadowList();

        GetCoinManager()->Destroy();
        GetSparkleManager()->Destroy();
		GetHitnRunManager()->Destroy();

        StatePropDSG::RemoveAllSharedtPoses();


        GetInteriorManager()->OnGameplayEnd();

        //rReleaseString ("GetInteriorManager()->OnGameplayEnd()\n");

        const bool shutdown = true;
        GetSuperCamManager()->Init( shutdown );

        //rReleaseString ("GetSuperCamManager()->Init( shutdown )\n");

        TriggerVolumeTracker::GetInstance()->Cleanup();
        
        //rReleaseString ("TriggerVolumeTracker::GetInstance()->Cleanup()\n");

        // Clean up lights!
        //
        RenderLayer* rl = GetRenderManager()->mpLayer( RenderEnums::LevelSlot );
        rAssert( rl );
        for( unsigned int i = 0; i < rl->GetNumViews(); i++ )
        {
            rl->pView(i)->RemoveAllLights ();
        }  

        //rReleaseString ("Remove all lights from all views\n");

        //rReleaseString ("GetAvatarManager()->Destroy()\n");
        GetActorManager()->RemoveAllActors();
        GetActorManager()->RemoveAllSpawnPoints();

        GetPresentationManager()->Finalize();

        GetGameplayManager()->Finalize();
        //rReleaseString ("GetGameplayManager()->Finalize()\n");

        // needs to be finalized after gameplay manager (Mission shudown needs avatars)
        GetAvatarManager()->Destroy();

        ParkedCarManager::DestroyInstance();
        //rReleaseString ("ParkedCarManager::DestroyInstance()\n");

        TrafficManager::DestroyInstance();
        //rReleaseString ("TrafficManager::DestroyInstance()\n");

        PedestrianManager::DestroyInstance();
        //rReleaseString ("PedestrianManager::DestroyInstance()\n");

        GetCharacterManager()->Destroy();
        //rReleaseString ("GetCharacterManager()->Destroy()\n");

        GetVehicleCentral()->Unload();

        GetActionButtonManager( )->Destroy( );
        //rReleaseString ("GetActionButtonManager( )->Destroy( )\n");


		GetBreakablesManager()->FreeAllBreakables();
        //rReleaseString ("GetBreakablesManager()->FreeAllBreakables()\n");

		GetParticleManager()->ClearSystems();
        //rReleaseString ("GetParticleManager()->ClearSystems()\n");
        
        
        GetWorldPhysicsManager()->OnQuitLevel();    // just some cleanup checks...
        
        
        
        SkidMarkGenerator::ReleaseShaders();
        //rReleaseString ("SkidMarkGenerator::ReleaseShaders()\n");
        GetSkidmarkManager()->Destroy();
        GetFootprintManager()->FreeTextures();

        GetRenderManager()->ClearLevelLayerLights();
        rReleasePrintf("PauseContext::OnStop DumpAllLoadedData\n");
		GetRenderManager()->DumpAllLoadedData();
        //rReleaseString ("GetRenderManager()->DumpAllLoadedData()\n");




#ifdef DEBUGWATCH
        VehicleAIRender::Destroy();
        //rReleaseString ("VehicleAIRender::Destroy()\n");

#endif
        GetSoundManager()->OnGameplayEnd( nextContext == CONTEXT_FRONTEND );
        //rReleaseString ("GetSoundManager()->OnGameplayEnd ()\n");


        PathManager::GetInstance()->Destroy();
        //rReleaseString ("PathManager::GetInstance()->Destroy()\n");

        

        // TODO - Darryl?
        // 
        // all active vehicles should be destroyed
        // they are owned by the missions that created them.

        //This does cleanup.
        RoadManager::GetInstance()->Destroy();
        //rReleaseString ("RoadManager::GetInstance()->Destroy()\n");



        if( nextContext == CONTEXT_LOADING_GAMEPLAY )
        {
            // if reloading in-game, set new level and mission to load
            //
            int level = GetGuiSystem()->GetInGameManager()->GetNextLevelToLoad();
            GetGameplayManager()->SetLevelIndex( static_cast<RenderEnums::LevelEnum>( level ) );

            int mission = GetGuiSystem()->GetInGameManager()->GetNextMissionToLoad();
            GetGameplayManager()->SetMissionIndex( static_cast<RenderEnums::MissionEnum>( mission ) );
            //rReleaseString ("Set level and mission indices\n");

        }
        else
        {
            SetGameplayManager( NULL );
        }

        // Cleanup the Avatar Manager
        //
        GetAvatarManager()->ExitGame();
        //rReleaseString ("GetAvatarManager()->ExitGame()\n");


        // Flush out the special section used by physics to cache SkeletonInfos.
        //
        p3d::inventory->RemoveSectionElements (SKELCACHE);
        p3d::inventory->DeleteSection (SKELCACHE);
        rReleaseString ("Delete SKELCACHE inventory section\n");


        // release GUI in-game
        GetGuiSystem()->HandleMessage( GUI_MSG_RELEASE_INGAME );
        rReleaseString ("GetGuiSystem()->HandleMessage( GUI_MSG_RELEASE_INGAME )\n");

        CHudMap::ClearAllRegisteredIcons();

        // unregister GUI user input handler for active players
        //
        int activeControllerIDs = 0;
        for( int i = 0; i < MAX_PLAYERS; i++ )
        {
            int controllerID = GetInputManager()->GetControllerIDforPlayer( i );
            if( controllerID != -1 )
            {
                activeControllerIDs |= (1 << controllerID);
            }
        }
        rReleaseString ("Change active controllers\n");


        GetGuiSystem()->UnregisterUserInputHandlers( activeControllerIDs );
        rReleaseString ("Change active controllers\n");


        // enable screen clearing
        GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearMask( PDDI_BUFFER_ALL );
        rReleaseString ("Enable screen clearing\n");

#ifndef RAD_RELEASE
        // Dump out the contents of the inventory sections
        //
        p3d::inventory->Dump (true);
        rReleaseString ("Dump inventory\n");

#endif
        AnimatedIcon::ShutdownAnimatedIcons();
        
        GetAnimEntityDSGManager()->RemoveAll();

        GetTutorialManager()->EnableTutorialMode( false );
        
        HeapMgr()->PopHeap (GMA_TEMP);
    }

    if ( nextContext == CONTEXT_GAMEPLAY )
    {
        if( mOldState == Input::ACTIVE_ANIM_CAM )
        {
            GetInputManager()->SetGameState( Input::ACTIVE_GAMEPLAY );
        }
        else
        {
            GetInputManager()->SetGameState( mOldState );
        }

        int controllerID = GetInputManager()->GetControllerIDforPlayer( 0 );
        bool vibrationOn = GetInputManager()->IsRumbleEnabled();

        if ( GetGameplayManager()->GetGameType() == GameplayManager::GT_NORMAL &&
            GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle() )
        {
#ifdef RAD_PS2
            bool hasWheel = false;

            if ( controllerID != Input::USB0 &&
                 controllerID != Input::USB1 )
            {
                if ( GetInputManager()->GetController( Input::USB0 )->IsConnected() )
                {
                    GetInputManager()->SetRumbleForDevice( Input::USB0, vibrationOn );
                    hasWheel = true;
                }
                else if ( GetInputManager()->GetController( Input::USB1 )->IsConnected() )
                {
                    GetInputManager()->SetRumbleForDevice( Input::USB1, vibrationOn );
                    hasWheel = true;
                }
            }
            //I hate this.
            if ( !hasWheel )
#endif
            GetInputManager()->SetRumbleForDevice( controllerID, vibrationOn );
        }

    }
    else
    {
        GetInputManager()->SetGameState( Input::ACTIVE_ALL );
    }
    SetMemoryIdentification( "PauseContext Finished" );
    MEMTRACK_POP_FLAG( "" );
}

//=============================================================================
// PauseContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void PauseContext::OnUpdate( unsigned int elapsedTime )
{
    // update game data manager
    GetGameDataManager()->Update( elapsedTime );

    // update GUI system
    GetGuiSystem()->Update( elapsedTime );

    // update coin manager
    GetCoinManager()->Update( elapsedTime );
    GetSparkleManager()->Update( elapsedTime );

	//update hitnrun????
	//GetHitnRunManager()->Update(elapsedTime);

#ifdef RAD_DEMO
    GetGameplayManager()->UpdateIdleTime( elapsedTime );
#endif

    // Check to see if we're loading so we don't screw up when we quit the level.
    //
    if( GetGameplayManager()->GetLevelComplete() && !GetLoadingManager()->IsLoading() ) 
    {
        if( !m_quitGamePending )
        {
            m_quitGamePending = true;

#ifdef RAD_DEMO
            GetGuiSystem()->HandleMessage( GUI_MSG_QUIT_INGAME );
#else
            if( GetGameplayManager()->GetGameComplete() )
            {
                GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_VIEW_CREDITS,
                                            0, 0, CLEAR_WINDOW_HISTORY );
            }
            else
            {
                GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_LEVEL_END,
                                            0, 0, CLEAR_WINDOW_HISTORY );
            }
#endif // RAD_DEMO
        }
    }
    else
    {
        // update mission loading
        //
        GetGameplayManager()->PerformLoading();
    }
}

//=============================================================================
// PauseContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PauseContext::OnSuspend()
{
}

//=============================================================================
// PauseContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PauseContext::OnResume()
{
}

//=============================================================================
// PauseContext::OnHandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void PauseContext::OnHandleEvent( EventEnum id, void* pEventData )
{
}
