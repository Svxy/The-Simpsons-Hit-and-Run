//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        exitcontext.cpp
//
// Description: Implementation of ExitContext.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <p3d/view.hpp>
#include <p3d/shadow.hpp>
#include <raddebug.hpp>

#include <p3d/shadow.hpp>
//========================================
// Project Includes
//========================================
#include <ai/vehicle/vehicleairender.h>
#include <contexts/pausecontext.h>
#include <contexts/exitcontext.h>
#include <memory/leakdetection.h>
#include <memory/srrmemory.h>
#include <worldsim/worldobject.h>
#include <camera/supercammanager.h>
#include <interiors/interiormanager.h>
#include <meta/triggervolumetracker.h>
#include <loading/loadingmanager.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/guisystem.h>
#include <render/RenderManager/RenderManager.h>
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
#include <data/gamedatamanager.h>
#include <mission/animatedicon.h>
#include <render/animentitydsgmanager/animentitydsgmanager.h>
#include <presentation/presentation.h>
#include <main/game.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
ExitContext* ExitContext::spInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ExitContext::GetInstance
//==============================================================================
//
// Description: - Access point for the ExitContext singleton.  
//              - Creates the ExitContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the ExitContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
ExitContext* ExitContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) ExitContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}


//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//==============================================================================
// ExitContext::OnStart
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
// Note:  Exiting is currently not valid from every context.  This needs more
//        checking and handling of special cases.
//
//==============================================================================
void ExitContext::OnStart( ContextEnum previousContext )
{
    MEMTRACK_PUSH_FLAG( "Exit" );

    if( previousContext == CONTEXT_GAMEPLAY || previousContext == CONTEXT_LOADING_GAMEPLAY )
    {
        // My precious little hack... my preciouss.
        GameFlow::GetInstance()->GetContext( CONTEXT_PAUSE )->Start( CONTEXT_GAMEPLAY );
        GameFlow::GetInstance()->GetContext( CONTEXT_PAUSE )->Stop( CONTEXT_EXIT );
    }

    GetLoadingManager()->CancelPendingRequests();
    p3d::loadManager->CancelAll();

    GetSoundManager()->OnGameplayEnd( false );
}


//==============================================================================
// ExitContext::OnStop
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ExitContext::OnStop( ContextEnum nextContext )
{
    MEMTRACK_POP_FLAG( "" );
    GetGame()->Stop();

    rAssertMsg( false, "No happen should." );
}


//==============================================================================
// ExitContext::OnUpdate
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ExitContext::OnUpdate( unsigned int elapsedTime )
{
}


//==============================================================================
// ExitContext::OnSuspend
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ExitContext::OnSuspend()
{
}


//==============================================================================
// ExitContext::OnResume
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ExitContext::OnResume()
{
}


//==============================================================================
// ExitContext::OnHandleEvent
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ExitContext::OnHandleEvent( EventEnum id, void* pEventData )
{
}



//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// ExitContext::ExitContext
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================// 
ExitContext::ExitContext()
{
}


//==============================================================================
// ExitContext::~ExitContext
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================// 
ExitContext::~ExitContext()
{
    spInstance = NULL;
}

