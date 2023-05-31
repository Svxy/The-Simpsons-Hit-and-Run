//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        FrontEndRenderLayer.cpp
//
// Description: Implementation for FrontEndRenderLayer class.
//
// History:     + Initial Implementation -- Tony [6/05/2002]
//
//========================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>
#include <p3d/view.hpp>
#include <p3d/billboardobject.hpp>

//========================================
// Project Includes
//========================================
#include <render/RenderManager/FrontEndRenderLayer.h>
#include <debug/profiler.h>
#include <presentation/gui/guisystem.h>
#include <presentation/presentation.h>
#include <gameflow/gameflow.h>
#include <contexts/contextenum.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>
#ifdef RAD_WIN32
#include <input/inputmanager.h>
#endif
//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : Context Interface
//
//************************************************************************

//========================================================================
// FrontEndFrontEndRenderLayer::FrontEndRenderLayer
//========================================================================
//
// Description: Inits state and variables to represent Dead State
//
// Parameters:	 None
//
// Return:      None.
//
// Constraints: 
//
//========================================================================
FrontEndRenderLayer::FrontEndRenderLayer()
:   RenderLayer(),
    mpScroobyApp( NULL )
{
#ifdef DEBUGWATCH
    radDbgWatchAddUnsignedInt(&mDebugRenderTime, "Render Time", "Front End Render Layer" );
#endif
}

//========================================================================
// FrontEndFrontEndRenderLayer::~FrontEndRenderLayer
//========================================================================
//
// Description: Cleans state and variables to represent Dead State
//
// Parameters:	 None
//
// Return:      None.
//
// Constraints: 
//
//========================================================================
FrontEndRenderLayer::~FrontEndRenderLayer()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete(&mDebugRenderTime);
#endif
}

void FrontEndRenderLayer::DrawCoinObject()
{
    // Render HUD coin effects.
    if((GetGameFlow()->GetCurrentContext() == CONTEXT_GAMEPLAY ||
        GetGameFlow()->GetCurrentContext() == CONTEXT_PAUSE) &&
	   !GetPresentationManager()->IsBusy())
	{
        GetCoinManager()->HUDRender();
        GetSparkleManager()->HUDRender();
        //??? GetHitnRunManager()->HUDRender();
    }
	else
	{
		GetCoinManager()->ClearHUDCoins();
	}

}
//************************************************************************
// Render Interface
//************************************************************************
//========================================================================
// FrontEndFrontEndRenderLayer::Render
//========================================================================
//
// Description: Renders all (TODO:visible/DSG) drawables
//
// Parameters:	 None
//
// Return:      None.
//
// Constraints: 
//
//========================================================================
void FrontEndRenderLayer::Render()
{
    BEGIN_PROFILE( "FE Render" );

#ifdef DEBUGWATCH
    mDebugRenderTime = radTimeGetMicroseconds();
#endif

    for( unsigned int view = 0; view < mNumViews; view++ )
    {
        mpView[ view ]->BeginRender();

        rAssert(!IsDead());

        HeapMgr()->PushHeap( GMA_TEMP );

        if (!GetCoinManager()->DrawAfterGui())
            DrawCoinObject();
        // display Scrooby screen (and updates all Pure3d objects)
        //
        mpScroobyApp->DrawFrame( static_cast<float>( g_scroobySimulationTime ) );
#ifdef RAD_WIN32
        // Update the frontend cursor.
        GetInputManager()->GetFEMouse()->Update();
#endif

        if (GetCoinManager()->DrawAfterGui())
            DrawCoinObject();

        HeapMgr()->PopHeap ( GMA_TEMP );

//        GetBillboardQuadManager()->DisplayAll();

        mpView[ view ]->EndRender();
    }

#ifdef DEBUGWATCH
    mDebugRenderTime = radTimeGetMicroseconds()-mDebugRenderTime;
#endif

    END_PROFILE( "FE Render" );
}

//************************************************************************
// Resource Interface
//************************************************************************
//////////////////////////////////////////////////////////////////////////
// Guts; Renderable Type Things
//////////////////////////////////////////////////////////////////////////
//========================================================================
// FrontEndRenderLayer::AddGuts
//========================================================================
//
// Description: Add a tDrawable
//
// Parameters:	 tDrawable to add
//
// Return:      None.
//
// Constraints: 
//
//========================================================================
void FrontEndRenderLayer::AddGuts( tDrawable* ipDrawable )
{
   //The Basic FrontEndRenderLayer does not support this type
   rAssert(false);
}

//========================================================================
// FrontEndRenderLayer::AddGuts
//========================================================================
//
// Description: Add a tGeometry
//
// Parameters:	 tGeometry to add
//
// Return:      None.
//
// Constraints: 
//
//========================================================================
void FrontEndRenderLayer::AddGuts( tGeometry* ipGeometry )
{
   //The Basic FrontEndRenderLayer does not support this type
   rAssert(false);
}

//========================================================================
// FrontEndRenderLayer::AddGuts
//========================================================================
//
// Description: Add an IntersectDSG
//
// Parameters:  IntersectDSG to add
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void FrontEndRenderLayer::AddGuts( IntersectDSG* ipIntersectDSG )
{
   //The Basic FrontEndRenderLayer does not support this type
   rAssert(false);
}

//========================================================================
// FrontEndRenderLayer::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void FrontEndRenderLayer::AddGuts( StaticEntityDSG* ipStaticEntityDSG )
{
   //The Basic FrontEndRenderLayer does not support this type
   rAssert(false);
}

//========================================================================
// FrontEndRenderLayer::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void FrontEndRenderLayer::AddGuts( StaticPhysDSG* ipStaticPhysDSG )
{
   //The Basic FrontEndRenderLayer does not support this type
   rAssert(false);
}

//========================================================================
// FrontEndRenderLayer::AddGuts
//========================================================================
//
// Description: Add the Scrooby App reference
//
// Parameters:  Scrooby App to add
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void FrontEndRenderLayer::AddGuts( Scrooby::App* ipScroobyApp )
{
    rAssert( mpScroobyApp == NULL );

    mpScroobyApp = ipScroobyApp;
}

//========================================================================
// FrontEndRenderLayer::SetUpGuts
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void FrontEndRenderLayer::SetUpGuts()
{
    // do nothing
}

//========================================================================
// FrontEndRenderLayer::NullifyGuts
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void FrontEndRenderLayer::NullifyGuts()
{
    mpScroobyApp = NULL;
}
