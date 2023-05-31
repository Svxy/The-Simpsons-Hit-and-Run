//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        renderflow.cpp
//
// Description: Implementation for RenderFlow class.
//
// History:     + Stolen and cleaned up from Penthouse -- Darwin Chau
//              + Stolen from Darwin and Tailored to RenderFlow from GameFlow -- Devin [4/17/2002]
//
//=============================================================================
 
//========================================
// System Includes
//========================================
#include <p3d/billboardobject.hpp>
#include <p3d/utility.hpp>
#include <pddi/pddiext.hpp>
#include <raddebug.hpp>
#include <radtime.hpp>
#include <raddebugwatch.hpp>
#include <p3d/effects/particleutility.hpp>

//========================================
// Project Includes
//========================================
#include <render/renderflow/renderflow.h>
#include <main/game.h>
#include <events/eventmanager.h>

//////////////////////////////////////////////////////////////////////////
#include <render/particles/particlemanager.h>
#include <render/breakables/breakablesmanager.h>
#include <render/animentitydsgmanager/animentitydsgmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Static pointer to instance of this singleton.
//
RenderFlow* RenderFlow::spInstance = NULL;

bool RenderFlow::sDrawStatsOverlay = false;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// RenderFlow::CreateInstance
//==============================================================================
//
// Description: Create the RenderFlow controller if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the created RenderFlow controller.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
RenderFlow* RenderFlow::CreateInstance()
{
   rAssert( spInstance == NULL );
   spInstance = new(GMA_PERSISTENT) RenderFlow();

   HeapMgr()->PushHeap(GMA_PERSISTENT);
   BillboardQuadManager::CreateInstance();
   HeapMgr()->PopHeap(GMA_PERSISTENT);

   return spInstance;
}

//==============================================================================
// RenderFlow::GetInstance
//==============================================================================
//
// Description: Get the RenderFlow controller if exists.
//
// Parameters:	None.
//
// Return:      Pointer to the created RenderFlow controller.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
RenderFlow* RenderFlow::GetInstance()
{
   rAssert( spInstance != NULL );
   
   return spInstance;
}


//==============================================================================
// RenderFlow::DestroyInstance
//==============================================================================
//
// Description: Destroy the RenderFlow controller.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void RenderFlow::DestroyInstance()
{
    //
    // Make sure this doesn't get called twice.
    //
    rAssert( spInstance != NULL );
    delete spInstance;
    spInstance = NULL;
}

//========================================================================
// RenderFlow::DoAllRegistration
//========================================================================
//
// Description: Register with whatever external dependancies/couplings
//              RenderFlow has
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void RenderFlow::DoAllRegistration()
{
#ifdef RAD_DEBUG
   sDrawStatsOverlay = false;
#else
   sDrawStatsOverlay = false;
#endif

   if ( CommandLineOptions::Get( CLO_FPS ) )
   {
       sDrawStatsOverlay = true;
   }

   p3d::pddi->EnableStatsOverlay(sDrawStatsOverlay);

#ifndef RAD_PS2
   p3d::pddi->SetCullMode(PDDI_CULL_NONE);
#endif

#ifdef RAD_PS2
    pddiExtPS2Control* ps2Control = (pddiExtPS2Control*)p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL);

  ps2Control->DisableTexCache( false );

//	pddiExtPS2Control* ps2Control = (pddiExtPS2Control*) p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL);
//	ps2Control->ForceMFIFOSync( true );
#endif

#if (defined(RAD_XBOX))
    ((pddiExtGammaControl*)p3d::pddi->GetExtension(PDDI_EXT_GAMMACONTROL))->SetGamma(0.956f,0.914f,0.866f);
#endif
#if (defined(RAD_XBOX) && defined(DEBUGWATCH))
    mpDebugXBoxGamma = (pddiExtGammaControl*)p3d::pddi->GetExtension(PDDI_EXT_GAMMACONTROL);
    mpDebugXBoxGamma->GetGamma( &mDebugGammaR, &mDebugGammaG, &mDebugGammaB );
#endif

#ifdef RAD_WIN32
    mpGammaControl = (pddiExtGammaControl*)p3d::pddi->GetExtension(PDDI_EXT_GAMMACONTROL);

    float r,g,b;
    mpGammaControl->GetGamma( &r, &g, &b );
    mpGammaControl->SetGamma( r, r, r );    // We will only deal with one degree.

    mGamma = r;     
#endif
   ParticleSystemRandomData::SetUp();

   GetEventManager()->AddListener(GetRenderManager(),(EventEnum)(EVENT_LOCATOR+LocatorEvent::DYNAMIC_ZONE));
   GetEventManager()->AddListener(GetRenderManager(), EVENT_FIRST_DYNAMIC_ZONE_START );
   GetEventManager()->AddListener(GetRenderManager(), EVENT_ALL_DYNAMIC_ZONES_DUMPED );
   GetEventManager()->AddListener(GetRenderManager(),(EventEnum)(EVENT_LOCATOR+LocatorEvent::OCCLUSION_ZONE));
   GetEventManager()->AddListener(GetRenderManager(), static_cast<EventEnum>( EVENT_LOCATOR + LocatorEvent::LIGHT_CHANGE ) );
   GetEventManager()->AddListener(GetRenderManager(), EVENT_MISSION_RESET );
}


//==============================================================================
// RenderFlow::OnTimerDone
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
void RenderFlow::OnTimerDone( unsigned int iElapsedTime, void* pUserData )
{
    //////////////////////////////////////////////////
    // Debugging stuff.
    //////////////////////////////////////////////////

#if (defined(RAD_XBOX) && defined(DEBUGWATCH))
    if(mpDebugXBoxGamma != NULL)
        mpDebugXBoxGamma->SetGamma( mDebugGammaR, mDebugGammaG, mDebugGammaB );
#endif
    #ifndef RAD_RELEASE

    // HACK to prevent iElapsedTime from being ridiculously huge.  
    // This is so that when we set breakpoints we don't have really huge 
    // elapsedtime values screwing us up.
    if( iElapsedTime > 1000 )
    {
        iElapsedTime = 20;
    }
    
    #endif
BEGIN_PROFILE("RenderFlow");

#ifdef DEBUGWATCH
    unsigned int t0 = radTimeGetMicroseconds();
#endif
    mpRenderManager->ContextUpdate( iElapsedTime );
#ifdef DEBUGWATCH
    mDebugRenderTime = radTimeGetMicroseconds() - t0;

    if( p3d::pddi->IsStatsOverlayEnabled() != sDrawStatsOverlay )
    {
        p3d::pddi->EnableStatsOverlay( sDrawStatsOverlay );
    }
#endif
END_PROFILE("RenderFlow");

}

#ifdef RAD_WIN32
//==============================================================================
// RenderFlow::SetGamma
//==============================================================================
//
// Description: Sets the gamma ramp for the game.
//
// Parameters:  gamma - one dimensional gamma value to set for r,g, and b.
//
// Return:      None.
//
//==============================================================================

void RenderFlow::SetGamma( float gamma )
{
    if( mpGammaControl == NULL )
    {
        return;
    }

    if( gamma < 0 )
    {
        gamma = 0;
    }

    mGamma = gamma;
    mpGammaControl->SetGamma( mGamma, mGamma, mGamma );
}

//==============================================================================
// RenderFlow::GetGamma
//==============================================================================
//
// Description: Returns the current gamma for the game
//
// Parameters:  n/a
//
// Return:      gamma value - equivalent for r g and b
//
//==============================================================================

float RenderFlow::GetGamma() const
{
    return mGamma;
}

#endif //rad_win32


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// RenderFlow::RenderFlow
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
RenderFlow::RenderFlow() : 
    mpITimer( NULL ) 
{
#ifdef DEBUGWATCH
   radDbgWatchAddUnsignedInt( &mDebugRenderTime, "Debug Render Flow micros", "RenderFlow", NULL, NULL );
   radDbgWatchAddBoolean( &sDrawStatsOverlay, "Draw Stats Overlay", "RenderFlow" );
   radDbgWatchAddFloat( &mDebugGammaR, "Xbox R Gamma", "RenderFlow", NULL, NULL, 0.0f, 2.0f );
   radDbgWatchAddFloat( &mDebugGammaG, "Xbox G Gamma", "RenderFlow", NULL, NULL, 0.0f, 2.0f );
   radDbgWatchAddFloat( &mDebugGammaB, "Xbox B Gamma", "RenderFlow", NULL, NULL, 0.0f, 2.0f );

    mpDebugXBoxGamma = NULL;
#endif
#ifdef RAD_WIN32
    mpGammaControl = NULL;
    mGamma = 0.0f;
#endif
   // 
   // Only 1 unique RenderManager/etc should exist for the lifetime of 
   // RenderFlow [4/17/2002]
   // 
   mpRenderManager   = RenderManager::CreateInstance();
   mpDSGFactory      = DSGFactory::CreateInstance();
   mpLoadWrappers    = AllWrappers::CreateInstance();
   mpIntersectManager= IntersectManager::CreateInstance();
   
    ParticleManager* pParticleManager = ParticleManager::CreateInstance();
    rAssert( pParticleManager != NULL );

    BreakablesManager* pBreakablesManager = BreakablesManager::CreateInstance();
    rAssert( pBreakablesManager != NULL );

	AnimEntityDSGManager* pAnimEntityDSGManager = AnimEntityDSGManager::CreateInstance();
	rAssert( pAnimEntityDSGManager != NULL );

}

//==============================================================================
// RenderFlow::~RenderFlow
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
RenderFlow::~RenderFlow()
{
    ParticleManager::DestroyInstance();
    BreakablesManager::DestroyInstance();
	AnimEntityDSGManager::DestroyInstance();

   //
   //DebugWatch Stuff
   //
#ifdef DEBUGWATCH
   radDbgWatchDelete(&mDebugRenderTime);
   radDbgWatchDelete(&sDrawStatsOverlay);
   radDbgWatchDelete( &mDebugGammaR );
   radDbgWatchDelete( &mDebugGammaG );
   radDbgWatchDelete( &mDebugGammaB );
#endif

   //
   // Kill lifetime Singletons
   //
   RenderManager::DestroyInstance();
   AllWrappers::DestroyInstance();
   DSGFactory::DestroyInstance();
   IntersectManager::DestroyInstance();
}

