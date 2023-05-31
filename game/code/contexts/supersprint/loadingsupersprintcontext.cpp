//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement LoadingSuperSprintContext
//
// History:     21/05/2002 + Created -- Cary Brisebois
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
#include <contexts/supersprint/loadingsupersprintcontext.h>

#include <gameflow/gameflow.h>
#include <loading/loadingmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/gameplaymanager.h>
#include <render/rendermanager/rendermanager.h>
#include <supersprint/supersprintmanager.h>
#include <sound/soundmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
LoadingSuperSprintContext* LoadingSuperSprintContext::spInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// LoadingSuperSprintContext::GetInstance
//==============================================================================
//
// Description: - Access point for the LoadingSuperSprintContext singleton.  
//              - Creates the LoadingSuperSprintContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the LoadingSuperSprintContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
LoadingSuperSprintContext* LoadingSuperSprintContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) LoadingSuperSprintContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//==============================================================================
// LoadingSuperSprintContext::LoadingSuperSprintContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingSuperSprintContext::LoadingSuperSprintContext()
{
}

//==============================================================================
// LoadingSuperSprintContext::~LoadingSuperSprintContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingSuperSprintContext::~LoadingSuperSprintContext()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// LoadingSuperSprintContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void LoadingSuperSprintContext::OnStart( ContextEnum previousContext )
{
    GetGameplayManager()->mIsDemo = false;

    //
    // Queue the loading for level sounds
    //
    GetSoundManager()->QueueLevelSoundLoads();

    // Common to all loading contexts.
    //
    LoadingContext::OnStart( previousContext );

/*****************************************************************************
 *	Start inserting stuff below ...
 *****************************************************************************/

    GetSSM()->LoadCars();

    // This is an extra callback to let this system know when all the loading is done for the
    // context..  Kinda sneaky..
    //
    GetLoadingManager()->AddCallback( this );
}

//=============================================================================
// LoadingSuperSprintContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void LoadingSuperSprintContext::OnStop( ContextEnum nextContext )
{
    // Common to all loading contexts.
    //
    LoadingContext::OnStop( nextContext );
}

//=============================================================================
// LoadingSuperSprintContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void LoadingSuperSprintContext::OnUpdate( unsigned int elapsedTime )
{
    // Common to all loading contexts.
    //
    LoadingContext::OnUpdate( elapsedTime );
}

//=============================================================================
// LoadingSuperSprintContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingSuperSprintContext::OnSuspend()
{
    // Common to all loading contexts.
    //
    LoadingContext::OnSuspend();
}

//=============================================================================
// LoadingSuperSprintContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingSuperSprintContext::OnResume()
{
    // Common to all loading contexts.
    //
    LoadingContext::OnResume();
}

//=============================================================================
// LoadingSuperSprintContext::PrepareNewHeaps
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingSuperSprintContext::PrepareNewHeaps()
{
}

//=============================================================================
// LoadingSuperSprintContext::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void LoadingSuperSprintContext::OnProcessRequestsComplete( void* pUserData )
{
    GetSSM()->LoadCharacters();
    GetSSM()->LoadScriptData();

    // Common to all loading contexts.
    //
    LoadingContext::OnProcessRequestsComplete( pUserData );
}
