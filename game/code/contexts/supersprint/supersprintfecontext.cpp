//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SuperSprintFEContext.cpp
//
// Description: Implement SuperSprintFEContext
//
// History:     03/26/2003 + Created -- Tony Chu
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <contexts/supersprint/supersprintfecontext.h>

#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/guisystem.h>
#include <supersprint/supersprintdata.h>
#include <supersprint/supersprintmanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

SuperSprintFEContext* SuperSprintFEContext::spInstance = NULL;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

SuperSprintFEContext* SuperSprintFEContext::GetInstance()
{
    if( spInstance == NULL )
    {
        HeapMgr()->PushHeap( GMA_PERSISTENT );
        spInstance = new SuperSprintFEContext;
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    }

    return spInstance;
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperSprintFEContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void SuperSprintFEContext::OnStart( ContextEnum previousContext )
{
    if( previousContext != CONTEXT_SUPERSPRINT )
    {
        HeapMgr()->PrepareHeapsSuperSprint();

        // run backend loading screen
        //
        GetGuiSystem()->HandleMessage( GUI_MSG_RUN_BACKEND );

        // load mini-game FE
        //
        GetGuiSystem()->HandleMessage( GUI_MSG_INIT_MINIGAME );

        GetLoadingManager()->AddCallback( this );
    }

    // register GUI user input handlers
    //
    GetGuiSystem()->RegisterUserInputHandlers();

    // unregister all players' controller IDs
    //
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        GetInputManager()->UnregisterControllerID( i );
    }

    GetInputManager()->SetGameState( Input::ACTIVE_FRONTEND );
}

//=============================================================================
// SuperSprintFEContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void SuperSprintFEContext::OnStop( ContextEnum nextContext )
{
    p3d::pddi->DrawSync();

    GetInputManager()->SetGameState( Input::ACTIVE_NONE );

    // unregister GUI user input handlers
    //
    GetGuiSystem()->UnregisterUserInputHandlers();

    if( nextContext != CONTEXT_LOADING_SUPERSPRINT )
    {
        SuperSprintManager::DestroyInstance();

        // unload mini-game FE
        //
        GetGuiSystem()->HandleMessage( GUI_MSG_RELEASE_MINIGAME );

        //Clear the gameplay manager.
        SetGameplayManager( NULL );
    }
}

//=============================================================================
// SuperSprintFEContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void SuperSprintFEContext::OnUpdate( unsigned int elapsedTime )
{
    GetGuiSystem()->Update( elapsedTime );
}

//=============================================================================
// SuperSprintFEContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintFEContext::OnSuspend()
{
}

//=============================================================================
// SuperSprintFEContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintFEContext::OnResume()
{
}

//=============================================================================
// SuperSprintFEContext::OnHandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void SuperSprintFEContext::OnHandleEvent( EventEnum id, void* pEventData )
{
}

//=============================================================================
// SuperSprintFEContext::OnProcessRequestsComplete
//=============================================================================
// Description: Called when startup loading is done
//
// Parameters:  pUserData - unused
//
// Return:      void 
//
//=============================================================================
void SuperSprintFEContext::OnProcessRequestsComplete( void* pUserData )
{
    // quit backend loading screen
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_QUIT_BACKEND );

    // startup the mini-game FE
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_RUN_MINIGAME );
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperSprintFEContext::SuperSprintFEContext
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
SuperSprintFEContext::SuperSprintFEContext()
{
}

//=============================================================================
// SuperSprintFEContext::~SuperSprintFEContext
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
SuperSprintFEContext::~SuperSprintFEContext()
{
}

