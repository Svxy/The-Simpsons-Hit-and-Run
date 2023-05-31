//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement PlayingContext
//
// History:     21/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <p3d/view.hpp>

//========================================
// Project Includes
//========================================
#include <contexts/playingcontext.h>

#include <debug/profiler.h>
#include <input/inputmanager.h>
#include <interiors/interiormanager.h>
#include <memory/srrmemory.h>
#include <presentation/gui/guisystem.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>

#include <mission/animatedicon.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// PlayingContext::PlayingContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PlayingContext::PlayingContext() :
    mQuitting( false )
{
}

//==============================================================================
// PlayingContext::~PlayingContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PlayingContext::~PlayingContext()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// PlayingContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void PlayingContext::OnStart( ContextEnum previousContext )
{
    SetMemoryIdentification( "PlayingContext" );
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    GetInputManager()->SetGameState( Input::ACTIVE_GAMEPLAY );
    GetRenderManager()->mpLayer( RenderEnums::LevelSlot )->Thaw();

    // disable screen clearing for GUI render layer
    //
    GetRenderManager()->mpLayer( RenderEnums::GUI )->pView( 0 )->SetClearMask( PDDI_BUFFER_DEPTH | PDDI_BUFFER_STENCIL );

#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( true );
#endif
}

//=============================================================================
// PlayingContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void PlayingContext::OnStop( ContextEnum nextContext )
{
    mQuitting = false; // I've quit after all.

#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( false );
#endif

    GetInputManager()->SetGameState( Input::ACTIVE_ALL );

    // Make sure no blur effects get carried over
    GetRenderManager()->SetBlurAlpha( 0 );

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    SetMemoryIdentification( "PlayingContext Finished" );
}

//=============================================================================
// PlayingContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void PlayingContext::OnUpdate( unsigned int elapsedTime )
{
BEGIN_PROFILE( "GuiSystem" );
    GetGuiSystem()->Update( elapsedTime );
END_PROFILE( "GuiSystem" );
}

//=============================================================================
// PlayingContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PlayingContext::OnSuspend()
{
}

//=============================================================================
// PlayingContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PlayingContext::OnResume()
{
}

//=============================================================================
// PlayingContext::OnHandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void PlayingContext::OnHandleEvent( EventEnum id, void* pEventData )
{
}

