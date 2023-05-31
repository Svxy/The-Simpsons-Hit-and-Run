//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement LoadingContext
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
#include <contexts/loadingcontext.h>
#include <camera/animatedcam.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <presentation/presentation.h>
#include <presentation/gui/guisystem.h>
#include <render/rendermanager/rendermanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>
#include <worldsim/hitnrunmanager.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/vehiclecentral.h>

#include <mission/animatedicon.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifndef FINAL
static unsigned int g_Timer;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// LoadingContext::LoadingContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingContext::LoadingContext()
{
}

//==============================================================================
// LoadingContext::~LoadingContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingContext::~LoadingContext()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// LoadingContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void LoadingContext::OnStart( ContextEnum previousContext )
{
    SetMemoryIdentification( "LoadingContext" );
#ifndef FINAL
    if( CommandLineOptions::Get( CLO_PRINT_LOAD_TIME ) )
    {
        g_Timer = radTimeGetMicroseconds();
    }
#endif

    MEMTRACK_PUSH_FLAG( "Loading Context" );

    this->PrepareNewHeaps();

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    // Note: *** Do NOT add any calls before the following GUI system call
    //           that will cause an AddRequest to the loading manager.
    //           The GUI system should have first dibs at queueing whatever it
    //           wants loaded for the loading screen.
    //

    AnimatedIcon::InitAnimatedIcons( HeapMgr()->GetCurrentHeap() );

    // no controller input while loading
    //
    GetInputManager()->SetGameState( Input::ACTIVE_NONE );

    // tell GUI system to run backend during loading
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_RUN_BACKEND );


    GetCoinManager()->Init();
    GetSparkleManager()->Init();

	GetHitnRunManager()->Init();

    // This is here since it needs to be in all modes.
    //
    const bool shutdown = false;
    GetSuperCamManager()->Init( shutdown );

    GetVehicleCentral()->PreLoad();

    GetRenderManager()->LoadAllNeededData();

    PedestrianManager::GetInstance()->InitDefaultModelGroups();

    GetPresentationManager()->Initialize();

    GetGameplayManager()->Initialize();
    GetGameplayManager()->LoadLevelData();
}

//=============================================================================
// LoadingContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void LoadingContext::OnStop( ContextEnum nextContext )
{
#ifdef RAD_WIN32
    if( nextContext == CONTEXT_EXIT )
    {
        GetLoadingManager()->CancelPendingRequests();
        p3d::loadManager->CancelAll();
    }
#endif

    RoadManager::GetInstance()->DumpRoadSegmentDataMemory();

    // tell GUI system to quit backend
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_QUIT_BACKEND );

    GetInputManager()->SetGameState( Input::ACTIVE_ALL );

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER ); 
#ifndef FINAL
    if( CommandLineOptions::Get( CLO_PRINT_LOAD_TIME ) )
    {
        unsigned int stopTime = radTimeGetMicroseconds();
        unsigned int time = stopTime - g_Timer;
        float timeInSeconds = time / 1000000.0f;
        rReleasePrintf( "Loading Time (s) = %f\n", timeInSeconds );
    }
#endif
    AnimatedCam::TriggerMissionStartCamera();
    MEMTRACK_POP_FLAG( "" );
    SetMemoryIdentification( "LoadingContext Finished" );

    GetInputManager()->EnableReset( true );
}

//=============================================================================
// LoadingContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void LoadingContext::OnUpdate( unsigned int elapsedTime )
{
    // update GUI system
    //
    GetGuiSystem()->Update( elapsedTime );
}

//=============================================================================
// LoadingContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingContext::OnSuspend()
{
}

//=============================================================================
// LoadingContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingContext::OnResume()
{
}

//=============================================================================
// LoadingContext::OnHandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void LoadingContext::OnHandleEvent( EventEnum id, void* pEventData )
{
}

//=============================================================================
// LoadingContext::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void LoadingContext::OnProcessRequestsComplete( void* pUserData )
{
    GetRenderManager()->DoPostLevelLoad();
}

