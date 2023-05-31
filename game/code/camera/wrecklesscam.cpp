//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wrecklesscam.cpp
//
// Description: Implement WrecklessCam
//
// History:     25/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
//These are included in the precompiled headers on XBOX and GC
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <camera/WrecklessCam.h>
#include <camera/isupercamtarget.h>

#include <events/eventmanager.h>
#include <events/eventenum.h>

#include <meta/eventlocator.h>

#include <mission/gameplaymanager.h>

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
// WrecklessCam::WrecklessCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WrecklessCam::WrecklessCam() :
    mNumTargets( 0 ),
    mActiveTarget( 0 ),
    mFOVDelta( 0 ),
    mMaxFOV( 1.5707f ),
    mMinFOV( 0.5f ),
    mMaxDistance( 35.0f ),
    mMinDistance( 10.0f ),
    mLag( 0.08f )
{
    unsigned int i;
    for ( i = 0; i < MAX_TARGETS; ++i )
    {
        mTargets[ i ] = NULL;
    }    
}

//==============================================================================
// WrecklessCam::~WrecklessCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WrecklessCam::~WrecklessCam()
{
}

//=============================================================================
// WrecklessCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void WrecklessCam::Update( unsigned int milliseconds )
{
    float timeMod = 1.0f;
    
    timeMod = (float)milliseconds / 16.0f;

    rmt::Vector position;
    mEventListener.GetLastPosition( &position );

    if ( position != rmt::Vector(0.0f, 0.0f, 0.0f) )
    {
        //Use this position for placing the camera.
    }
    else
    {
        //Get the target position.
        mTargets[ mActiveTarget ]->GetPosition( &position );

        //HACK
        position.y += 10.0f;
    }

    rmt::Vector targetPos;
    mTargets[ mActiveTarget ]->GetPosition( &targetPos );

    if ( GetFlag( (Flag)CUT ) )
    {
        mFOV = mMaxFOV;
        SetFlag( (Flag)CUT, false );
    }

    rmt::Vector camToTarg;
    camToTarg.Sub( position, targetPos );
    float dist = camToTarg.Magnitude();
    DoWrecklessZoom( dist, mMinDistance, mMaxDistance, mMinFOV, mMaxFOV, mFOV, mFOVDelta, mLag, timeMod );
    
    SetFOV( mFOV );

    SetCameraValues( milliseconds, position, targetPos );
}

//=============================================================================
// WrecklessCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void WrecklessCam::SetTarget( ISuperCamTarget* target )
{
    rAssert( target );

    //First test to make sure we don't already have this target.
    if( mNumTargets != 0 )
    {
        unsigned int i;
        for ( i = 0; i < mNumTargets; ++i )
        {
            if ( mTargets[ i ] == target )
            {
                //Already got one!
                mActiveTarget = i;
                return;
            }
        }                
    }

    //This is going to be the first target
    mTargets[ mNumTargets ] = target;
    mActiveTarget = mNumTargets;
    mNumTargets = 1;
}

//=============================================================================
// WrecklessCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
void WrecklessCam::AddTarget( ISuperCamTarget* target )
{
    rAssert( mNumTargets < MAX_TARGETS );
    rAssert( target );

    if ( mNumTargets < MAX_TARGETS )
    {
        //Add the target
        mTargets[ mNumTargets ] = target;

        ++mNumTargets;
    }
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// WrecklessCam::OnInit
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WrecklessCam::OnInit()
{
    mEventListener.SetPlayerID( GetPlayerID() );

    EventManager* evtMngr = GetEventManager();
    evtMngr->AddListener( GetEventListener(), (EventEnum)(EVENT_LOCATOR + LocatorEvent::CAMERA_CUT) );

    if ( !GetGameplayManager()->mIsDemo )
    {
        //Find all the CUT_CAM events and make them active.
        p3d::inventory->PushSection();

        if ( GetGameplayManager()->IsSuperSprint() || GetGameplayManager()->GetCurrentLevelIndex() > RenderEnums::L3 ) //Fucking hack.  It's this or change the art pipes.
        {
            p3d::inventory->SelectSection( "Level" );
        }
        else
        {
            p3d::inventory->SelectSection( "Default" );
        }

        HeapMgr()->PushHeap( GMA_TEMP );
        tInventory::Iterator<EventLocator> it;
        HeapMgr()->PopHeap( GMA_TEMP );

        EventLocator* evtLoc = it.First();

        while( evtLoc )
        {
            if ( evtLoc->GetEventType() == LocatorEvent::CAMERA_CUT )
            {
                evtLoc->SetFlag( Locator::ACTIVE, true );
            }

            evtLoc = it.Next();
        }

        p3d::inventory->PopSection();
    }
}

//=============================================================================
// WrecklessCam::OnShutdown
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WrecklessCam::OnShutdown()
{
    GetEventManager()->RemoveAll( GetEventListener() );

    if ( !GetGameplayManager()->mIsDemo )
    {
        //Find all the CUT_CAM events and make them inactive.
        p3d::inventory->PushSection();
        p3d::inventory->SelectSection( "Default" );

        HeapMgr()->PushHeap( GMA_TEMP );
        tInventory::Iterator<EventLocator> it;
        HeapMgr()->PopHeap( GMA_TEMP );

        EventLocator* evtLoc = it.First();

        while( evtLoc )
        {
            if ( evtLoc->GetEventType() == LocatorEvent::CAMERA_CUT )
            {
                evtLoc->SetFlag( Locator::ACTIVE, false );
            }

            evtLoc = it.Next();
        }

        p3d::inventory->PopSection();
    }
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//=============================================================================
// WrecklessCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WrecklessCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Wreckless", GetPlayerID() );

    radDbgWatchAddFloat( &mMinFOV, "Min FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mMaxFOV, "Max FOV", nameSpace, NULL, NULL, 0.0f, rmt::PI );
    radDbgWatchAddFloat( &mMinDistance, "Min Dist", nameSpace, NULL, NULL, 0.0f, 100.0f );
    radDbgWatchAddFloat( &mMaxDistance, "Max Dist", nameSpace, NULL, NULL, 0.0f, 100.0f );
    radDbgWatchAddFloat( &mLag, "Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
#endif
}

//=============================================================================
// WrecklessCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WrecklessCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mMinFOV );
    radDbgWatchDelete( &mMaxFOV );
    radDbgWatchDelete( &mMinDistance );
    radDbgWatchDelete( &mMaxDistance );
    radDbgWatchDelete( &mLag );
#endif
}