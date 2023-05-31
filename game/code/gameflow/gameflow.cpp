//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gameflow.cpp
//
// Description: Implementation for GameFlow class.
//
// History:     + Stolen and cleaned up from Penthouse -- Darwin Chau
//
//=============================================================================

#define DONTCHECKVECTORRESIZING
//========================================
// System Includes
//========================================
#include <raddebug.hpp>
#include <radtime.hpp>

//========================================
// Project Includes
//========================================
#include <gameflow/gameflow.h>

#include <contexts/entrycontext.h>
#include <contexts/bootupcontext.h>
#include <contexts/frontendcontext.h>
#include <contexts/pausecontext.h>
#include <contexts/exitcontext.h>
#include <contexts/gameplay/loadinggameplaycontext.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <contexts/demo/loadingdemocontext.h>
#include <contexts/demo/democontext.h>
#include <contexts/supersprint/loadingsupersprintcontext.h>
#include <contexts/supersprint/supersprintcontext.h>
#include <contexts/supersprint/supersprintfecontext.h>

#include <sound/soundmanager.h>
#include <main/commandlineoptions.h>
#include <main/game.h>
#include <memory/memoryutilities.h>
#include <memory/srrmemory.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Static pointer to instance of this singleton.
//
GameFlow* GameFlow::spInstance = NULL;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// GameFlow::CreateInstance
//==============================================================================
//
// Description: Create the gameflow controller.
//
// Parameters:	None.
//
// Return:      Pointer to the created gameflow controller.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
GameFlow* GameFlow::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "GameFlow" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) GameFlow;
    rAssert( spInstance );
MEMTRACK_POP_GROUP( "GameFlow" );
    
    return spInstance;
}


//==============================================================================
// GameFlow::GetInstance
//==============================================================================
//
// Description: Return the gameflow controller.
//
// Parameters:	None.
//
// Return:      Pointer to the created gameflow controller.
//
//==============================================================================
GameFlow* GameFlow::GetInstance()
{
    //rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// GameFlow::DestroyInstance
//==============================================================================
//
// Description: Destroy the gameflow controller.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void GameFlow::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}


//==============================================================================
// GameFlow::PushContext
//==============================================================================
//
// Description: Change the context.  Pushes the new context onto the stack.
//              The previous context(s) are preserved on the stack.
//              Note: the new context will not take effect until the next
//                    timer tick (game loop update).
//
// Parameters:	context - the new context
//
// Return:      None.
//
//==============================================================================
void GameFlow::PushContext( ContextEnum context )
{
    //
    // Really bad news if this assert triggers.  There's already a context
    // change in the queue when this one was requested.
    //
    // This assert cannot be ignored, you must fix the problem.
    //
    rAssert( mCurrentContext == mNextContext );

MEMTRACK_PUSH_GROUP( "GameFlow" );
    mNextContext = context;
    mContextStack.push( context );
MEMTRACK_POP_GROUP( "GameFlow" );
}


//==============================================================================
// GameFlow::PopContext
//==============================================================================
//
// Description: Restore the previous context.  
//
//              Note: the new context will not take effect until the next
//                    timer tick (game loop update).
//
// Constraints: Obviously we have a problem if there's nothing on the stack.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void GameFlow::PopContext()
{
    rAssert( mContextStack.size() > 1 );

    //
    // Really bad news if this assert triggers.  There's already a context
    // change in the queue when this one was requested.
    //
    // This assert cannot be ignored, you must fix the problem.
    //
    rAssert( mCurrentContext == mNextContext );

    mContextStack.pop();
    mNextContext = mContextStack.top();
}


//==============================================================================
// GameFlow::SetContext
//==============================================================================
//
// Description: Change the context and clear out the stack.
//
//              Note: the new context will not take effect until the next
//                    timer tick (game loop update).
//
// Parameters:	context - the new context
//
// Return:      None.
//
//==============================================================================
void GameFlow::SetContext( ContextEnum context )
{
    while( !mContextStack.empty() )
    {
        mContextStack.pop();
    }

    this->PushContext( context );
}


//==============================================================================
// GameFlow::OnTimerDone
//==============================================================================
//
// Description: This routine is invoked to run the game. It gets called by the
//              dispatcher once per frame.
//
// Parameters:  elapsedtime - time it actually took for timer to expire
//              pUserData   - custom user data
//
// Return:      None.
//
//==============================================================================
void GameFlow::OnTimerDone( unsigned int elapsedtime, void* pUserData )
{
    //////////////////////////////////////////////////
    // Debugging stuff.
    //////////////////////////////////////////////////

    bool printMemory = CommandLineOptions::Get( CLO_PRINT_MEMORY );
    if( printMemory )
    {
        static int accumulatedTime = 0;
        const int printTimeIntervalMs = 3000;
        accumulatedTime += elapsedtime;
        if( accumulatedTime > printTimeIntervalMs )
        {
            accumulatedTime %= printTimeIntervalMs;
            Memory::PrintMemoryStatsToTty();
        }
    }

    #ifndef RAD_RELEASE

    // HACK to prevent elapsedtime from being ridiculously huge.  
    // This is so that when we set breakpoints we don't have really huge 
    // elapsedtime values screwing us up.
    if( elapsedtime > 1000 )
    {
        elapsedtime = 20;
    }
    
    #endif

    //////////////////////////////////////////////////
    // Switch contexts if appropriate.
    //////////////////////////////////////////////////

    // If current and next contexts are different...
    if( mCurrentContext != mNextContext )
    {
        mpContexts[mCurrentContext]->Stop( mNextContext );
        mpContexts[mNextContext]->Start( mCurrentContext );
        
        mCurrentContext = mNextContext;
    }

    //////////////////////////////////////////////////
    // See if have anything to update.
    //////////////////////////////////////////////////

    // If current context is exit, then stop the game.
    if( mCurrentContext == CONTEXT_EXIT )
    {
        GetGame()->Stop();
        return;
    }

    //////////////////////////////////////////////////
    // Update managers and controllers.
    //////////////////////////////////////////////////
    
    //
    // Run the once-per-frame sound update
    //
    SoundManager::GetInstance()->UpdateOncePerFrame( elapsedtime, mCurrentContext );

    // Update the current context.
    mpContexts[mCurrentContext]->Update( elapsedtime );
}



//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// GameFlow::GameFlow
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
GameFlow::GameFlow() : 
    mpITimer( NULL ), 
    mCurrentContext( CONTEXT_ENTRY ), 
    mNextContext( CONTEXT_ENTRY )
{
    //
    // Initialize members.
    //
    int i = CONTEXT_ENTRY;
    for( ; i < NUM_CONTEXTS; ++i )
    {
        mpContexts[i] = NULL;
    }

    //
    // Create the context controllers.
    //
    mpContexts[CONTEXT_ENTRY]               = GetEntryContext();
    mpContexts[CONTEXT_BOOTUP]              = GetBootupContext();
    mpContexts[CONTEXT_FRONTEND]            = GetFrontEndContext();
    mpContexts[CONTEXT_LOADING_DEMO]        = GetLoadingDemoContext();
    mpContexts[CONTEXT_DEMO]                = GetDemoContext();
    mpContexts[CONTEXT_LOADING_SUPERSPRINT] = GetLoadingSuperSprintContext();
    mpContexts[CONTEXT_SUPERSPRINT]         = GetSPCTX();
    mpContexts[CONTEXT_SUPERSPRINT_FE]      = GetSuperSprintFEContext();
    mpContexts[CONTEXT_LOADING_GAMEPLAY]    = GetLoadingGameplayContext();
    mpContexts[CONTEXT_GAMEPLAY]            = GetGameplayContext();
    mpContexts[CONTEXT_PAUSE]               = GetPauseContext();
    mpContexts[CONTEXT_EXIT]                = GetExitContext();
    
    //
    // Since we're starting with the entry context, call its Start function
    // for the sake of symmetry and to allow memory tagging to work properly
    //
    mpContexts[mCurrentContext]->Start( mCurrentContext );
}

//==============================================================================
// GameFlow::~GameFlow
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
GameFlow::~GameFlow()
{
    //
    // Release the context controllers.
    //
    int i = CONTEXT_ENTRY;
    for( ; i < NUM_CONTEXTS; ++i )
    {
        rAssert( mpContexts[i] != NULL );
        mpContexts[i]->DestroyInstance();
    }
}

#undef DONTCHECKVECTORRESIZING