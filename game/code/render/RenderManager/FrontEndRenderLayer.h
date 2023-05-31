#ifndef __FRONTEND_RENDER_LAYER_H__
#define __FRONTEND_RENDER_LAYER_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   FrontEndRenderLayer 
//
// Description: 
//
// History:     + Implemented Initial interfaces      -- Tony [6/05/2002]
//              
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/RenderManager/RenderLayer.h>
#include <raddebugwatch.hpp>

//=================================================
// TODOs: Re-encapsulate (into other file(s)) the
//        data-sets below:
//=================================================

//========================================================================
//
// Synopsis:   The FrontEndRenderLayer
//
//========================================================================
class FrontEndRenderLayer : public RenderLayer
{
public:
   FrontEndRenderLayer();
   ~FrontEndRenderLayer();

   ///////////////////////////////////////////////////////////////////////
   // Render Interface
   ///////////////////////////////////////////////////////////////////////
   virtual void Render();

   ///////////////////////////////////////////////////////////////////////
   // Resource Interfaces
   ///////////////////////////////////////////////////////////////////////
   // Guts; Renderable Type Things
   virtual void   AddGuts( tDrawable* ipDrawable );
   virtual void   AddGuts( tGeometry* ipGeometry );
   virtual void   AddGuts( IntersectDSG* ipIntersectDSG );
   virtual void   AddGuts( StaticEntityDSG* ipStaticEntityDSG );
   virtual void   AddGuts( StaticPhysDSG* ipStaticPhysDSG );
   virtual void   AddGuts( Scrooby::App* ipScroobyApp );
   virtual void   SetUpGuts();
   virtual void   NullifyGuts();

protected:
    Scrooby::App* mpScroobyApp;

private:
    void DrawCoinObject();
#ifdef DEBUGWATCH
   unsigned int mDebugRenderTime;
#endif

};

#endif // __FRONTEND_RENDER_LAYER_H__
