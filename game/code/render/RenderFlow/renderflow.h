//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   RenderFlow   
//
// Description: The RenderFlow Controller orchestrates the overall render
//              loop [4/17/2002].
//
// History:     + Stolen and cleaned up from Penthouse -- Darwin Chau
//              + Stolen from Darwin and Tailored to RenderFlow from GameFlow -- Devin [4/17/2002]
//
//=============================================================================

#ifndef RENDERFLOW_H
#define RENDERFLOW_H

//========================================
// System Includes
//========================================
#include <radtime.hpp>  // IRadTimerCallback
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <render/RenderManager/RenderManager.h>
#include <render/Loaders/AllWrappers.h>
#include <render/DSG/DSGFactory.h>
#include <render/IntersectManager/IntersectManager.h>
#include <pddi/pddiext.hpp>


//=============================================================================
//
// Synopsis:    The render "loop"
//
//=============================================================================
class RenderFlow : public IRadTimerCallback
{
public:

   // Static Methods (for creating and getting an instance of the game)
   static RenderFlow* CreateInstance();
   static RenderFlow* GetInstance();
   static void  DestroyInstance();

   // Establish all persistent couplings; must be called before 
   // Instance can be operable
   void DoAllRegistration();

   // Implement IRadTimerCallback interface.
   // This member is called whenever the timer expires.
   void OnTimerDone( unsigned int iElapsedTime, void* pUserData );

#ifdef RAD_WIN32
   float GetGamma() const;
   void SetGamma( float gamma );
#endif

private:

   // Declared but not defined to prevent copying and assignment.
   RenderFlow( const RenderFlow& );
   RenderFlow& operator=( const RenderFlow& );

   // Constructor - these are private to prevent anybody else from 
   // creating me.
   RenderFlow();
   virtual ~RenderFlow();

   // This member is called when the gameflow is being initialized.
   void Initialize();

   // The one and only RenderFlow instance.
   static RenderFlow* spInstance;

#ifdef DEBUGWATCH
    unsigned int mDebugRenderTime;
    pddiExtGammaControl* mpDebugXBoxGamma;
    float mDebugGammaR, mDebugGammaG, mDebugGammaB;
#endif
#ifdef RAD_WIN32
    pddiExtGammaControl* mpGammaControl;
    float mGamma;
#endif
   static bool sDrawStatsOverlay;

   // Timer for gameflow updates.
   IRadTimer* mpITimer;

   // Maintain a pointer to the RenderManager; [4/17/2002]
   // Maintain a ptr to all load wrappers
   RenderManager*     mpRenderManager;
   AllWrappers*       mpLoadWrappers;  
   DSGFactory*        mpDSGFactory;
   IntersectManager*  mpIntersectManager;
};


//
// A little syntactic sugar for getting at this singleton.
//
inline RenderFlow* GetRenderFlow() { return( RenderFlow::GetInstance() ); }


#endif
