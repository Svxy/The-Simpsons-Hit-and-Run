//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement LoadingDemoContext
//
// History:     21/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/demo/loadingdemocontext.h>

#include <loading/loadingmanager.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/guisystem.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/vehiclecentral.h>
#include <sound/soundmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
LoadingDemoContext* LoadingDemoContext::spInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// LoadingDemoContext::GetInstance
//==============================================================================
//
// Description: - Access point for the LoadingDemoContext singleton.  
//              - Creates the LoadingDemoContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the LoadingDemoContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
LoadingDemoContext* LoadingDemoContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new LoadingDemoContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//==============================================================================
// LoadingDemoContext::LoadingDemoContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingDemoContext::LoadingDemoContext()
{
}

//==============================================================================
// LoadingDemoContext::~LoadingDemoContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingDemoContext::~LoadingDemoContext()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// LoadingDemoContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void LoadingDemoContext::OnStart( ContextEnum previousContext )
{
    GetGameplayManager()->mIsDemo = true;

    // Common to all loading contexts.
    //
    LoadingContext::OnStart( previousContext );

/*****************************************************************************
 *	Start inserting stuff below ...
 *****************************************************************************/

    // NOTE: 
    // Assumes we never start a DEMO on foot (always start inside the car)
    GetVehicleCentral()->ActivateVehicleTriggers( false ); 

    TrafficManager::GetInstance()->InitDefaultModelGroups();

    // initialize GUI in-game mode (and load resources)
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_INIT_INGAME );

    GetLoadingManager()->AddCallback( this );
}

//=============================================================================
// LoadingDemoContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void LoadingDemoContext::OnStop( ContextEnum nextContext )
{
    // Common to all loading contexts.
    //
    LoadingContext::OnStop( nextContext );
}

//=============================================================================
// LoadingDemoContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void LoadingDemoContext::OnUpdate( unsigned int elapsedTime )
{
    GetGameplayManager()->Update( elapsedTime );

    // Common to all loading contexts.
    //
    LoadingContext::OnUpdate( elapsedTime );
}

//=============================================================================
// LoadingDemoContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingDemoContext::OnSuspend()
{
    // Common to all loading contexts.
    //
    LoadingContext::OnSuspend();
}

//=============================================================================
// LoadingDemoContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingDemoContext::OnResume()
{
    // Common to all loading contexts.
    //
    LoadingContext::OnResume();
}

//=============================================================================
// LoadingDemoContext::PrepareNewHeaps
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingDemoContext::PrepareNewHeaps()
{
    HeapMgr()->PrepareHeapsInGame();
}

//=============================================================================
// LoadingDemoContext::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void LoadingDemoContext::OnProcessRequestsComplete( void* pUserData )
{
    GetGameplayManager()->LevelLoaded();

    //
    // Queue the loading for level sounds
    //
    GetSoundManager()->QueueLevelSoundLoads();

    // Common to all loading contexts.
    //
    LoadingContext::OnProcessRequestsComplete( pUserData );
}
