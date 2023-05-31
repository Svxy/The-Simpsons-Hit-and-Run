//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement LoadingGameplayContext
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
#include <contexts/gameplay/loadinggameplaycontext.h>

#include <interiors/interiormanager.h>
#include <loading/loadingmanager.h>
#include <memory/leakdetection.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/guisystem.h>
#include <worldsim/parkedcars/parkedcarmanager.h>
#include <worldsim/traffic/trafficmanager.h>
#include <sound/soundmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
LoadingGameplayContext* LoadingGameplayContext::spInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// LoadingGameplayContext::GetInstance
//==============================================================================
//
// Description: - Access point for the LoadingGameplayContext singleton.  
//              - Creates the LoadingGameplayContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the LoadingGameplayContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
LoadingGameplayContext* LoadingGameplayContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) LoadingGameplayContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//==============================================================================
// LoadingGameplayContext::LoadingGameplayContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingGameplayContext::LoadingGameplayContext()
{
}

//==============================================================================
// LoadingGameplayContext::~LoadingGameplayContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoadingGameplayContext::~LoadingGameplayContext()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// LoadingGameplayContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void LoadingGameplayContext::OnStart( ContextEnum previousContext )
{
    GetGameplayManager()->mIsDemo = false;

    // Common to all loading contexts.
    //
    LoadingContext::OnStart( previousContext );

/*****************************************************************************
 *	Start inserting stuff below ...
 *****************************************************************************/

    // NOTE: 
    // Assumes we never start a level in the car
    GetVehicleCentral()->ActivateVehicleTriggers( true ); 

    GetPCM();  //Start the parked car manager

    TrafficManager::GetInstance()->InitDefaultModelGroups();

    // initialize GUI in-game mode (and load resources)
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_INIT_INGAME );

    GetLoadingManager()->AddCallback( this );
}

//=============================================================================
// LoadingGameplayContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void LoadingGameplayContext::OnStop( ContextEnum nextContext )
{
    // Common to all loading contexts.
    //
    LoadingContext::OnStop( nextContext );
}

//=============================================================================
// LoadingGameplayContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void LoadingGameplayContext::OnUpdate( unsigned int elapsedTime )
{
    GetGameplayManager()->Update( elapsedTime );

    // Common to all loading contexts.
    //
    LoadingContext::OnUpdate( elapsedTime );
}

//=============================================================================
// LoadingGameplayContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingGameplayContext::OnSuspend()
{
    // Common to all loading contexts.
    //
    LoadingContext::OnSuspend();
}

//=============================================================================
// LoadingGameplayContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingGameplayContext::OnResume()
{
    // Common to all loading contexts.
    //
    LoadingContext::OnResume();
}

//=============================================================================
// LoadingGameplayContext::PrepareNewHeaps
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingGameplayContext::PrepareNewHeaps()
{
    HeapMgr()->PrepareHeapsInGame();
}

//=============================================================================
// LoadingGameplayContext::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void LoadingGameplayContext::OnProcessRequestsComplete( void* pUserData )
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
