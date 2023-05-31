//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement DemoContext
//
// History:     21/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <p3d/shadow.hpp>

//========================================
// Project Includes
//========================================
#include <ai/vehicle/vehicleairender.h>

#include <contexts/demo/democontext.h>
#include <gameflow/gameflow.h>
#include <main/commandlineoptions.h>
#include <main/game.h>
#include <memory/srrmemory.h>
#include <memory/leakdetection.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/guisystem.h>
#include <render/rendermanager/rendermanager.h>
#include <render/DSG/StatePropDSG.h>
#include <render/rendermanager/renderlayer.h>
#include <ai/actor/actormanager.h>

#include <worldsim/character/footprint/footprintmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>
#include <worldsim/hitnrunmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/ped/pedestrianmanager.h>

#include <worldsim/skidmarks/skidmarkmanager.h>

#include <camera/walkercam.h>
#include <camera/followcam.h>
#include <camera/wrecklesscam.h>
#include <camera/chasecam.h>
#include <camera/conversationcam.h>
#include <camera/supercammanager.h>

#include <render/breakables/breakablesmanager.h>
#include <render/particles/particlemanager.h>
#include <render/animentitydsgmanager/animentitydsgmanager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <worldsim/skidmarks/skidmarkgenerator.h>
#include <sound/soundmanager.h>
#include <meta/triggervolumetracker.h>
#include <render/Loaders/AnimDynaPhysLoader.h>

#include <pedpaths/pathmanager.h>

#include <input/inputmanager.h>

// TODO: Remove once we put CreateRoadNetwork in the levels pipe
#include <roads/roadmanager.h>
#include <p3d/light.hpp>
#include <p3d/view.hpp>

#include <worldsim/worldobject.h>
#include <ai/actionbuttonmanager.h>

#include <presentation/presentation.h>

#include <mission/animatedicon.h>

#include <render/animentitydsgmanager/animentitydsgmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

extern bool g_inDemoMode;

// Static pointer to instance of singleton.
DemoContext* DemoContext::spInstance = NULL;

const unsigned int DEMO_LOOP_TIME = 60000; // in msec

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DemoContext::GetInstance
//==============================================================================
//
// Description: - Access point for the DemoContext singleton.  
//              - Creates the DemoContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the DemoContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
DemoContext* DemoContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new DemoContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//==============================================================================
// DemoContext::DemoContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DemoContext::DemoContext()
:   m_demoLoopTime( DEMO_LOOP_TIME ),
    m_elapsedTime( -1 )
{
}

//==============================================================================
// DemoContext::~DemoContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DemoContext::~DemoContext()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// DemoContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void DemoContext::OnStart( ContextEnum previousContext )
{
    // Common to all playing contexts.
    //
    PlayingContext::OnStart( previousContext );

    GetInputManager()->SetGameState( Input::ACTIVE_FRONTEND );

    // We count the number of demos run.
    //
    GetGame()->IncrementDemoCount();

    m_elapsedTime = 0; // reset elapsed demo time

    ////////////////////////////////////////////////////////////
    // RenderManager 
    RenderManager* rm = GetRenderManager();
    RenderLayer* rl = rm->mpLayer( RenderEnums::LevelSlot );
    rAssert( rl );

#ifdef DEBUGWATCH
    // bootstrap vehicleai renderer
    VehicleAIRender::GetVehicleAIRender();
#endif

    ////////////////////////////////////////////////////////////
    // Cameras set up
    unsigned int iNumPlayers = GetGameplayManager()->GetNumPlayers();
    rl->SetNumViews( iNumPlayers );
    rl->SetUpViewCam();

  	p3d::inventory->SelectSection("Default");
    tLightGroup* sun = p3d::find<tLightGroup>("sun");   
    rAssert( sun );
    rm->SetLevelLayerLights( sun );

    float aspect = p3d::display->IsWidescreen() ? (16.0f / 9.0f) : (4.0f / 3.0f);

    unsigned int view = 0;

    tPointCamera* cam = static_cast<tPointCamera*>( rl->pCam( view ) );
    rAssert( dynamic_cast<tPointCamera*> ( cam ) != NULL );
    rAssert( cam );

    SuperCamCentral* scc = GetSuperCamManager()->GetSCC( view );
    rAssert( scc );

    scc->SetCamera( cam );

    FollowCam* fc = new FollowCam( FollowCam::FOLLOW_NEAR );
    fc->SetAspect( aspect );
    fc->CopyToData();
    scc->RegisterSuperCam( fc );

    fc = new FollowCam( FollowCam::FOLLOW_FAR );
    fc->SetAspect( aspect );
    fc->CopyToData();
    scc->RegisterSuperCam( fc );

    SuperCam* sc = new WalkerCam();
    sc->SetAspect( aspect );
    scc->RegisterSuperCam( sc );

    sc = new ChaseCam();
    sc->SetAspect( aspect );
    scc->RegisterSuperCam( sc );

    sc = new WrecklessCam();
    sc->SetAspect( aspect );
    scc->RegisterSuperCam( sc );

    sc = new ConversationCam();
    sc->SetAspect( aspect );
    scc->RegisterSuperCam( sc );


    ////////////////////////////////////////////////////////////
    // AvatarManager Bootstrapping
    GetAvatarManager( )->EnterGame( );
    /*
    GetActionButtonManager( )->EnterGame( );
    */

    ////////////////////////////////////////////////////////////
    // TrafficManager Init
    TrafficManager::GetInstance()->Init();

    ////////////////////////////////////////////////////////////
    // PedestrianManager Init
    PedestrianManager::GetInstance()->Init();
    
    // TODO: Move this into level pipe
    //Set up the sorting of the intersections and stuff.
    RoadManager::GetInstance()->CreateRoadNetwork();

    ////////////////////////////////////////////////////////////
    // SkidMark Init Init
    //Find skid mark shaders in the inventory and set proper values
    SkidMarkGenerator::InitShaders();

    GetSkidmarkManager()->Init();

    ////////////////////////////////////////////////////////////
    // OnStart calls
    //
    // Notify the sound system of gameplay start.  This has been moved after
    // the GUI startup above, since that leads to a mission reset, which causes
    // character position in or out of the car to be decided, which the sound
    // system uses to determine which sounds to start playing.
    //
    SoundManager::GetInstance()->OnGameplayStart();
    /*
    GetPresentationManager()->OnGameplayStart();
    */

    GetGuiSystem()->HandleMessage( GUI_MSG_RUN_INGAME );
    GetGuiSystem()->HandleMessage( GUI_MSG_RUN_DEMO ); // TC: must be called after GUI_MSG_RUN_INGAME

    GetGuiSystem()->RegisterUserInputHandlers();

    GetGame()->SetTime( 0 );

    g_inDemoMode = true;
}

//=============================================================================
// DemoContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void DemoContext::OnStop( ContextEnum nextContext )
{
    HeapMgr()->PushHeap (GMA_TEMP);

    //
    // This is called to prevent DMA of destroyed textures,
    // because we don't swap buffers until the next frame.
    //
    p3d::pddi->DrawSync();

    // Clear the shadow list
    AnimDynaPhysLoader::ClearShadowList();

    GetCoinManager()->Destroy();
    GetSparkleManager()->Destroy();
    GetSkidmarkManager()->Destroy();
	GetHitnRunManager()->Destroy();

    StatePropDSG::RemoveAllSharedtPoses();


    const bool shutdown = true;
    GetSuperCamManager()->Init( shutdown );

    TriggerVolumeTracker::GetInstance()->Cleanup();

    // Clean up lights!
    //
    RenderLayer* rl = GetRenderManager()->mpLayer( RenderEnums::LevelSlot );
    rAssert( rl );
    for( unsigned int i = 0; i < rl->GetNumViews(); i++ )
    {
        rl->pView(i)->RemoveAllLights ();
    }  
  
#ifdef DEBUGWATCH
    VehicleAIRender::Destroy();
#endif

    GetPresentationManager()->OnGameplayStop();

    GetPresentationManager()->Finalize();

    //Destroy Avatars stuff first

    GetGameplayManager()->Finalize();
    SetGameplayManager( NULL );

    GetAvatarManager()->Destroy();

    TrafficManager::DestroyInstance();
    PedestrianManager::DestroyInstance();
    GetCharacterManager()->Destroy();
    GetVehicleCentral()->Unload();
    GetActorManager()->RemoveAllActors();
    GetActorManager()->RemoveAllSpawnPoints();

    //We never entered.
    //GetActionButtonManager( )->Destroy( );
    
	GetBreakablesManager()->FreeAllBreakables();
	GetParticleManager()->ClearSystems();
	GetRenderManager()->DumpAllLoadedData();
    SkidMarkGenerator::ReleaseShaders();

    GetSoundManager()->OnGameplayEnd( true );

    PathManager::GetInstance()->Destroy();


    // TODO - Darryl?
    // 
    // all active vehicles should be destroyed
    // they are owned by the missions that created them.

    //This does cleanup.
    RoadManager::GetInstance()->Destroy();

    // release GUI in-game
    GetGuiSystem()->HandleMessage( GUI_MSG_RELEASE_INGAME );

    /*
    // enable screen clearing
    GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearMask( PDDI_BUFFER_ALL );
    */

    // Cleanup the Avatar Manager
    //
    GetAvatarManager()->ExitGame();

    // Flush out the special section used by physics to cache SkeletonInfos.
    //
    p3d::inventory->RemoveSectionElements (SKELCACHE);
    p3d::inventory->DeleteSection (SKELCACHE);

    GetGuiSystem()->UnregisterUserInputHandlers();

    // enable screen clearing
    //
    GetRenderManager()->mpLayer( RenderEnums::GUI )->pView( 0 )->SetClearMask( PDDI_BUFFER_ALL );

#ifndef RAD_RELEASE
    // Dump out the contents of the inventory sections
    //
    p3d::inventory->Dump (true);
#endif

    AnimatedIcon::ShutdownAnimatedIcons();
    GetAnimEntityDSGManager()->RemoveAll();

    HeapMgr()->PopHeap (GMA_TEMP);


    // Common to all playing contexts.
    //
    PlayingContext::OnStop( nextContext );
}

//=============================================================================
// DemoContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void DemoContext::OnUpdate( unsigned int elapsedTime )
{
    if ( !mQuitting )
    {
        bool cont = true;

        if( m_elapsedTime != -1 )
        {
            m_elapsedTime += static_cast<int>( elapsedTime );
            if( m_elapsedTime > static_cast<int>( m_demoLoopTime ) )
            {
                if ( GetLoadingManager()->IsLoading() )
                {
                    //HACK!
                    //Oops!  Can't quit yet!
                    m_elapsedTime -= static_cast<int>( elapsedTime );
                }
                else
                {
                    // demo loop time expired, quit out of demo
                    //
                    GetGuiSystem()->HandleMessage( GUI_MSG_QUIT_DEMO );

                    m_elapsedTime = -1;
                    mQuitting = true; //Now we're quitting
                }
                cont = false;
            }
        }

        if ( cont )
        {
            float timeins = (float)(elapsedTime) / 1000.0f;
            GetAvatarManager()->Update( timeins );

            GetCharacterManager()->GarbageCollect( );

            ///////////////////////////////////////////////////////////////
            // Update Particles
            GetParticleManager()->Update( elapsedTime );
            GetBreakablesManager()->Update( elapsedTime );
            GetAnimEntityDSGManager()->Update( elapsedTime );

            ///////////////////////////////////////////////////////////////
            // Update Gameplay Manager
            GetGameplayManager()->Update( elapsedTime );

            ///////////////////////////////////////////////////////////////
            // Update Physics
            GetWorldPhysicsManager()->Update(elapsedTime);

            ///////////////////////////////////////////////////////////////
            // Update Peds
            // ordering is important. Unless other parts of code change, we must call
            // this before WorldPhysManager::Update() because PedestrianManager
            // sets the flags for all characters to be updated in WorldPhys Update 
            PedestrianManager::GetInstance()->Update( elapsedTime );

            /*
            ///////////////////////////////////////////////////////////////
            // Update PresentationManager
            GetPresentationManager()->Update( elapsedTime );
            */

            ///////////////////////////////////////////////////////////////
            // Update Trigger volumes
            GetTriggerVolumeTracker()->Update( elapsedTime );

            ///////////////////////////////////////////////////////////////
            // Update Traffic
            TrafficManager::GetInstance()->Update( elapsedTime );

            /*
            ///////////////////////////////////////////////////////////////
            // Update Interiors
            GetInteriorManager()->Update( elapsedTime );
            */

            //extra updates
            GetFootprintManager()->Update( elapsedTime );
            BEGIN_PROFILE("SkidmarkManager");
            GetSkidmarkManager()->Update( elapsedTime );
            END_PROFILE("SkidmarkManager");
            ActorManager::GetInstance()->Update( elapsedTime );
            GetCoinManager()->Update( elapsedTime );
            GetSparkleManager()->Update( elapsedTime );
            GetHitnRunManager()->Update( elapsedTime );
        }
    } //!mQuitting

    // Common to all playing contexts.
    //
    PlayingContext::OnUpdate( elapsedTime );
}

//=============================================================================
// DemoContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DemoContext::OnSuspend()
{
    // Common to all playing contexts.
    //
    PlayingContext::OnSuspend();
}

//=============================================================================
// DemoContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DemoContext::OnResume()
{
    // Common to all playing contexts.
    //
    PlayingContext::OnResume();
}

