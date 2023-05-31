//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        RenderLayer.cpp
//
// Description: Implementation for RenderManager class.
//
// History:     + Stolen and cleaned up from Penthouse -- Darwin Chau
//              + Stolen from Darwin and Tailored 
//                  to RenderFlow from GameFlow       -- Devin [4/17/2002]
//              + Stolen from Devin  and Tailored 
//                  to RenderManager from RenderFlow  -- Devin [4/17/2002]
//              + Stolen from Devin  and Tailored 
//                  to RenderManager from RenderLayer -- Devin [4/18/2002]
//
//========================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>
#include <p3d/pointcamera.hpp>
#include <p3d/view.hpp>

//========================================
// Project Includes
//========================================
#include <render/RenderManager/RenderLayer.h>
#include <render/DSG/IntersectDSG.h>
#include <render/DSG/StaticEntityDSG.h>
#include <render/DSG/StaticPhysDSG.h>
#include <render/DSG/FenceEntityDSG.h>
#include <render/Culling/SpatialTree.h>

#include <memory/srrmemory.h>

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
// RenderLayer::RenderLayer
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
RenderLayer::RenderLayer() :
mIsBeginView( true )
{
    mNumViews = 1;
    OnRenderLayerInit();
    mExportedState = msDead;
    mPreviousState = msDead;
}

//========================================================================
// RenderLayer::~RenderLayer
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
RenderLayer::~RenderLayer()
{
   if(!IsDead())
      Kill();
}

//************************************************************************
// Render Interface
//************************************************************************
//========================================================================
// RenderLayer::Render
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
void RenderLayer::Render()
{
    BEGIN_PROFILE( "UNKNOWN Render" );
    for( unsigned int view = 0; view < mNumViews; view++ )
    {
        if( mIsBeginView )
        {
            mpView[ view ]->BeginRender();
        }

        rAssert(!IsDead());

        //   if( mExportedState == msRenderReady )
        //   {
        for(int i = mpGuts.mUseSize-1; i>-1; i-- )
        {
            mpGuts[i]->Display();
        }
        //   }

        if( mIsBeginView )
        {
            mpView[ view ]->EndRender();
        }
    }
    END_PROFILE( "UNKNOWN Render" );
}

//************************************************************************
// Exported Class/State Manipulators
//************************************************************************
//========================================================================
// RenderLayer::Kill
//========================================================================
//
// Description: ExportedState: RenderReady||Frozen >> Dead;
//              Dump any data and set state to dead
//
// Parameters:	 None
//
// Return:      None.
//
// Constraints: 
//
//========================================================================
void RenderLayer::Kill()
{
    if( IsDead() )
    {
        return;
    }
   
    NullifyGuts();
    NullifyViewCam();

    mExportedState = msDead;
}

//========================================================================
// RenderLayer::Resurrect
//========================================================================
//
// Description: ExportedState: Dead >> RenderReady
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void RenderLayer::Resurrect()
{
   if( !IsDead() )
   {
       return;
   }
   rAssert(mpView != NULL);

   mExportedState = msRenderReady;
}

//========================================================================
// RenderLayer::FreezeCorpse
//========================================================================
//
// Description: Dead >> Frozen 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void RenderLayer::FreezeCorpse()
{
   if( IsFrozen() )
   {
       return;
   }
   rAssert(mpView != NULL);

   mExportedState = msFrozen;
}

//========================================================================
// RenderLayer::Freeze
//========================================================================
//
// Description: ExportedState: RenderReady >> Frozen; don't render
//
// Parameters:	 None
//
// Return:      None.
//
// Constraints: 
//
//========================================================================
void RenderLayer::Freeze()
{
   if( !IsRenderReady() )
   {
       return;
   }
   mExportedState = msFrozen;
}

/*=============================================================================
Description:    Call to freeze the layer and remember the previous state.
                Match with a call to Warm().
=============================================================================*/
void RenderLayer::Chill( void )
{
    mPreviousState = mExportedState;
    mExportedState = msFrozen;
}
/*=============================================================================
Description:    Call to restore the state frozen in Chill. Make sure it is a
                match to a Chill() call.
=============================================================================*/
void RenderLayer::Warm( void )
{
    mExportedState = mPreviousState;
    mPreviousState = msDead;
}
//========================================================================
// RenderLayer::Thaw
//========================================================================
//
// Description: ExportedState: Frozen >> RenderReady
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void RenderLayer::Thaw()
{
   if( !IsFrozen() )
   {
       return;
   }
   mExportedState = msRenderReady;
}

//************************************************************************
// Resource Interface
//************************************************************************
//////////////////////////////////////////////////////////////////////////
// Guts; Renderable Type Things
//////////////////////////////////////////////////////////////////////////
//========================================================================
// RenderLayer::AddGuts
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
void RenderLayer::AddGuts( tDrawable* ipDrawable )
{
    rAssert( ipDrawable != NULL );
    ipDrawable->AddRef();
    mpGuts.Add( ipDrawable );
}
//========================================================================
// RenderLayer::AddGuts
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
void RenderLayer::RemoveGuts( tDrawable* ipDrawable )
{
    rAssert( ipDrawable != NULL );
    for(int i=mpGuts.mUseSize-1; i>-1; i--)
    {
        if(mpGuts[i] == ipDrawable)
        {
            mpGuts.Remove(i);
            ipDrawable->Release();
        }
    }
}

//========================================================================
// RenderLayer::AddGuts
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
void RenderLayer::AddGuts( tGeometry* ipGeometry )
{
   ipGeometry->AddRef();
   mpGuts.Add( (tDrawable*&)ipGeometry );
}

//========================================================================
// RenderLayer::AddGuts
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
void RenderLayer::AddGuts( IntersectDSG* ipIntersectDSG )
{
   //The Basic RenderLayer does not support adding of IntersectDSG's
   rAssert(false);
}

//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( StaticEntityDSG* ipStaticEntityDSG )
{
   //The Basic RenderLayer does not support adding of IntersectDSG's
   rAssert(false);
}

//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( StaticPhysDSG* ipStaticPhysDSG )
{
   //The Basic RenderLayer does not support adding of IntersectDSG's
   rAssert(false);
}

//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( Scrooby::App* ipScroobyApp )
{
   //The Basic RenderLayer does not support adding of Scrooby App
   rAssert(false);
}
//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( SpatialTree* ipSpatialTree )
{
   //The Basic RenderLayer does not support adding of SpatialTree
   rAssert(false);
}
//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( FenceEntityDSG* ipFenceEntityDSG )
{
   //The Basic RenderLayer does not support adding of fence entities
   rAssert(false);
}
//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( AnimCollisionEntityDSG* ipAnimCollDSG )
{
   //The Basic RenderLayer does not support adding of Animated Collision entities
   rAssert(false);
}
//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( AnimEntityDSG* ipAnimDSG )
{
   //The Basic RenderLayer does not support adding of Animated entities
   rAssert(false);
}
//========================================================================
// RenderLayer::
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
void  RenderLayer::AddGuts( DynaPhysDSG* ipDynaPhysDSG )
{
   //The Basic RenderLayer does not support adding of Dyna Phys entities
   rAssert(false);
}
//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( TriggerVolume* ipTriggerVolume )
{
   //The Basic RenderLayer does not support adding of Trigger Volume entities
   rAssert(false);
}
//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( WorldSphereDSG* ipWorldSphereDSG )
{
   //The Basic RenderLayer does not support adding of WorldSphere entities
   rAssert(false);
}

//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( RoadSegment* ipRoadSegment )
{
   //The Basic RenderLayer does not support adding of road segment entities
   rAssert(false);
}

//========================================================================
// RenderLayer::
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
void RenderLayer::AddGuts( PathSegment* ipPathSegment )
{
   //The Basic RenderLayer does not support adding of path segment entities
   rAssert(false);
}


//========================================================================
// RenderLayer::
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
void RenderLayer::RemoveGuts( IEntityDSG* ipEDSG )
{
    //Only supported in WorldRenderLayer
    rAssert(false);
}
//========================================================================
// RenderLayer::RemoveGuts
//========================================================================
//
// Description: Removes multicontrollers
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: Assert failure, always. Only works on WorldRenderLayers
//
//========================================================================

void RenderLayer::RemoveGuts( tMultiController* ipZoneController )
{
	//Only supported in WorldRenderLayer
	rAssert(false);
}
//////////////////////////////////////////////////////////////////////////
// Other; Other Layer Resources of interest
//////////////////////////////////////////////////////////////////////////

//========================================================================
// RenderLayer::pCam
//========================================================================
//
// Description: Get Cam Ptr
//
// Parameters:	 None.
//
// Return:      *Cam (*tPointCamera)
//
// Constraints: 
//
//========================================================================
tCamera* RenderLayer::pCam( unsigned int viewIndex )
{
   return mpView[ viewIndex ]->GetCamera();
}

//========================================================================
// RenderLayer::pView
//========================================================================
//
// Description: Get View Ptr
//
// Parameters:	 None.
//
// Return:      *View (*tView)
//
// Constraints: 
//
//========================================================================
tView* RenderLayer::pView( unsigned int viewIndex )
{
   return mpView[ viewIndex ];
}

//========================================================================
// RenderLayer::rAlpha
//========================================================================
//
// Description: Get Alpha Ref
//
// Parameters:	 None.
//
// Return:      *Alpha 
//
// Constraints: 
//
//========================================================================
float& RenderLayer::rAlpha()
{
   // This is a currently unsupported function
   rAssert(false);

   return mAlpha;
}


//========================================================================
// RenderLayer::DoAllSetups()
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
void RenderLayer::DoAllSetups()
{
   SetUpViewCam();
   SetUpGuts();
}
//========================================================================
// RenderLayer::SetUpViewCam
//========================================================================
//
// Description: Setup View, Cam and alpha to default/initial/non-null 
//              values
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void RenderLayer::SetUpViewCam()
{
MEMTRACK_PUSH_GROUP( "RenderLayer" );
    for( unsigned int i = 0; i < mNumViews; i++ )
    {
        if( mpView[ i ] != NULL ) 
        {
            mpView[ i ]->Release();
        }

        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        mpView[ i ] = new tView();
        mpView[ i ]->AddRef();

        tPointCamera* pDefaultCam = new tPointCamera;
        pDefaultCam->AddRef();
        mpView[ i ]->SetCamera( pDefaultCam );
        pDefaultCam->Release();
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );


// Senta  mpView[ i ]->SetClearColour( tColour(0x60,0x70,0xA0,0x00) );
//        mpView[ i ]->SetClearColour( tColour(0x60,0x70,0xA0,0xff) );
        mpView[ i ]->SetClearColour( tColour(0x9A,0xC2,0xDE,0xff) );
        mpView[ i ]->SetClearMask( PDDI_BUFFER_ALL );

        mpView[ i ]->EnableFog(false);
    }

    switch ( mNumViews )
    {
    case 1:
        {
            mpView[ 0 ]->SetWindow( 0.0f, 0.0f, 1.0f, 1.0f );
            break;
        }
    case 2:
        {
            mpView[ 0 ]->SetWindow( 0.0f, 0.0f, 1.0f, 0.5f );
            mpView[ 1 ]->SetWindow( 0.0f, 0.5f, 1.0f, 1.0f );
            break;
        }
    default:
        {
            rAssertMsg(false, "Only have support for 1 or 2 players right now!");
        }
    }        
MEMTRACK_POP_GROUP( "RenderLayer" );
}

//========================================================================
// RenderLayer::NullifyViewCam
//========================================================================
//
// Description: Nullify View, Cam and alpha to null values safely
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void RenderLayer::NullifyViewCam()
{
    for( unsigned int view = 0; view < mNumViews; view++ )
    {
        mpView[ view ]->Release();
        mpView[ view ] = NULL;
    }
}

//========================================================================
// RenderLayer::SetUpGuts()
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
void RenderLayer::SetUpGuts()
{
   rAssert( !mpGuts.IsSetUp() );
   mpGuts.Allocate( msMaxGuts );
}

//========================================================================
// RenderLayer::NullifyGuts()
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
void RenderLayer::NullifyGuts()
{
    if( mExportedState == msDead )
    {
        return;
    }

    for(int i = mpGuts.mUseSize-1; i>-1; i-- )
    {
        mpGuts[i]->Release();
    }
    //mpGuts.mUseSize = 0;
    mpGuts.Clear();
}

//=============================================================================
// RenderLayer::HasGuts
//=============================================================================
// Description: Comment
//
// Parameters:  ( tDrawable* guts )
//
// Return:      bool 
//
//=============================================================================
bool RenderLayer::HasGuts( tDrawable* guts )
{
    for ( int i = mpGuts.mUseSize-1; i>-1; i-- )
    {
        if ( mpGuts[i] == guts )
        {
            return true;
        }
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////
// Load Related interfaces
//////////////////////////////////////////////////////////////////////////
void RenderLayer::DoPreStaticLoad(){}
void RenderLayer::DoPostStaticLoad(){}
void RenderLayer::DumpAllDynaLoads(unsigned int count, SwapArray<tRefCounted*>& irEntityDeletionList ){}
void RenderLayer::DumpDynaLoad(tName& irGiveItAFuckinName, SwapArray<tRefCounted*>& irEntityDeletionList){}
bool RenderLayer::DoPreDynaLoad(tName& irGiveItAFuckinName){ return false; }
void RenderLayer::DoPostDynaLoad(){}

//************************************************************************
// Exported Class/State
//************************************************************************
//========================================================================
// RenderLayer::IsDead
//========================================================================
//
// Description: Is this layer dead?
//
// Parameters:	 None.
//
// Return:      bool Is this layer dead?
//
// Constraints: 
//
//========================================================================
bool RenderLayer::IsDead()
{
    return( mExportedState == msDead );
}

//========================================================================
// RenderLayer::IsDead
//========================================================================
//
// Description: Is this layer dead?
//
// Parameters:	 None.
//
// Return:      bool Is this layer dead?
//
// Constraints: 
//
//========================================================================
bool RenderLayer::IsFrozen()
{
    return( mExportedState == msFrozen );
}
//========================================================================
// RenderLayer::IsDead
//========================================================================
//
// Description: Is this layer dead?
//
// Parameters:	 None.
//
// Return:      bool Is this layer dead?
//
// Constraints: 
//
//========================================================================
bool RenderLayer::IsRenderReady()
{
    return( mExportedState == msRenderReady );
}


//************************************************************************
// Protected
//************************************************************************
//========================================================================
// RenderLayer::IsGutsSetup
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      IsGutsSetup
//
// Constraints: None.
//
//========================================================================
bool RenderLayer::IsGutsSetup()
{
    return( mpGuts.IsSetUp() );
}

//========================================================================
// RenderLayer::IsViewCamSetup
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      IsViewCamSetup
//
// Constraints: None.
//
//========================================================================
bool RenderLayer::IsViewCamSetup( unsigned int viewIndex )
{
    return( mpView[ viewIndex ] != NULL );
}

//========================================================================
// RenderLayer::OnRenderLayerInit()
//========================================================================
//
// Description: Called on & only on construction
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void RenderLayer::OnRenderLayerInit()
{
    for( int view = 0; view < MAX_PLAYERS; view++ )
    {
        mpView[view]         = NULL;
    }

    mAlpha         = 1.0f;

    mExportedState = msDead;
}
