//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement FrontEndContext
//
// History:     21/05/2002 + Created -- NAME
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
#include <cheats/cheatinputsystem.h>
#include <contexts/frontendcontext.h>
#include <contexts/contextenum.h>
#include <memory/leakdetection.h>
#include <memory/srrmemory.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/frontend/guimanagerfrontend.h>
#include <mission/rewards/rewardsmanager.h>

#include <sound/soundmanager.h>

#include <input/inputmanager.h>

#include <data/gamedatamanager.h>

#include <worldsim/coins/coinmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
FrontEndContext* FrontEndContext::spInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// FrontEndContext::GetInstance
//==============================================================================
//
// Description: - Access point for the FrontEndContext singleton.  
//              - Creates the FrontEndContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the FrontEndContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
FrontEndContext* FrontEndContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) FrontEndContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//==============================================================================
// FrontEndContext::FrontEndContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FrontEndContext::FrontEndContext()
{
}

//==============================================================================
// FrontEndContext::~FrontEndContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FrontEndContext::~FrontEndContext()
{
}


//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// FrontEndContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::OnStart( ContextEnum previousContext )
{
    SetMemoryIdentification( "FEContext" );
    MEMTRACK_PUSH_FLAG( "Front End" );

    HeapMgr()->PushHeap( GMA_LEVEL_FE );

    if( previousContext != CONTEXT_BOOTUP )
    {
        HeapMgr()->PrepareHeapsFeCleanup();
        LEAK_DETECTION_CHECKPOINT();
        HeapMgr()->PrepareHeapsFeSetup();

        // reset all cheats
        //
        GetCheatInputSystem()->Reset();

        // unregister controller ID from all players
        //
        GetInputManager()->UnregisterAllControllerID();

        // tell GUI system to run backend during loading
        //
        GetGuiSystem()->HandleMessage( GUI_MSG_RUN_BACKEND );

        // initialize GUI frontend mode (and load resources)
        GetGuiSystem()->HandleMessage( GUI_MSG_INIT_FRONTEND );

        GetLoadingManager()->AddCallback( this );
    }
    else
    {
        // Start the front end.
        LEAK_DETECTION_CHECKPOINT();
        this->StartFrontEnd( CGuiWindow::GUI_SCREEN_ID_SPLASH );
    }

    GetInputManager()->ToggleRumble( false );

    GetGuiSystem()->RegisterUserInputHandlers();

    GetInputManager()->SetGameState( Input::ACTIVE_FRONTEND );
}

//=============================================================================
// FrontEndContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::OnStop( ContextEnum nextContext )
{
    GetGuiSystem()->UnregisterUserInputHandlers();

    // release GUI frontend
    GetGuiSystem()->HandleMessage( GUI_MSG_RELEASE_FRONTEND );

    //
    // Notify the sound system that the front end is stopping
    //
    GetSoundManager()->OnFrontEndEnd();

    GetInputManager()->SetGameState( Input::ACTIVE_ALL );

    HeapMgr()->PopHeap(GMA_LEVEL_FE);

    MEMTRACK_POP_FLAG( "" );

    SetMemoryIdentification( "FEContext Finished" );
}

//=============================================================================
// FrontEndContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::OnUpdate( unsigned int elapsedTime )
{
    // update game data manager
    //
    GetGameDataManager()->Update( elapsedTime );

    // update GUI system
    //
    GetGuiSystem()->Update( elapsedTime );

    //Chuck: adding this so that the rewards manager reflects changes found in the charactersheet.
    GetRewardsManager()->SynchWithCharacterSheet();
}

//=============================================================================
// FrontEndContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::OnSuspend()
{
}

//=============================================================================
// FrontEndContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::OnResume()
{
}

//=============================================================================
// FrontEndContext::OnHandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::OnHandleEvent( EventEnum id, void* pEventData )
{
}

//=============================================================================
// FrontEndContext::OnProcessRequestsComplete
//=============================================================================
// Description: Called when startup loading is done
//
// Parameters:  pUserData - unused
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::OnProcessRequestsComplete( void* pUserData )
{
    // tell GUI system to quit backend
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_QUIT_BACKEND );

#ifndef RAD_DEMO
    if( GetGuiSystem()->IsSplashScreenFinished() )
    {
        if( GetGuiSystem()->IsShowCreditsUponReturnToFE() )
        {
            this->StartFrontEnd( CGuiWindow::GUI_SCREEN_ID_VIEW_CREDITS );
        }
        else
        {
            this->StartFrontEnd( CGuiWindow::GUI_SCREEN_ID_MAIN_MENU );
        }
    }
    else
#endif
    {
        this->StartFrontEnd( CGuiWindow::GUI_SCREEN_ID_SPLASH );
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// FrontEndContext::StartFrontEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FrontEndContext::StartFrontEnd( unsigned int initialScreen )
{
    // Start up GUI frontend manager
    GetGuiSystem()->HandleMessage( GUI_MSG_RUN_FRONTEND, initialScreen );

    //
    // Notify the sound system that the front end is starting
    //
    GetSoundManager()->OnFrontEndStart();
}

