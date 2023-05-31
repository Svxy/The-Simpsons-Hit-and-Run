//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        animplayer.cpp
//
// Description: Implement AnimationPlayer
//
// History:     22/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================

#include <memory/srrmemory.h>

#include <presentation/animplayer.h>
#include <interiors/interiormanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/RenderManager/RenderLayer.h>

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
// AnimationPlayer::AnimationPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
AnimationPlayer::AnimationPlayer() :
    mState( ANIM_IDLE ),
    mbPlayAfterLoad( true ),
    mbExclusive( true ),
    mbShowAlways ( false ),
    mbKeepLayersFrozen( false ),
	mbIsSkippable(true),
    mpLoadDataCallback( NULL )
{
}

//==============================================================================
// AnimationPlayer::~AnimationPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
AnimationPlayer::~AnimationPlayer()
{
}


//==============================================================================
// AnimationPlayer::LoadData
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void AnimationPlayer::LoadData
( 
    const char* fileName,
    bool bInInventory,
    void* pUserData
)
{
    if( mState == ANIM_IDLE )
    {
        mState = ANIM_LOADING;

        if( bInInventory )
        {
            OnProcessRequestsComplete( NULL );
            mSection = 0;
        }
        else
        {
            if(!GetInteriorManager()->IsInside())
            {
                GetLoadingManager()->AddRequest( FILEHANDLER_ANIMATION,
                                             fileName,
                                             GMA_CHARS_AND_GAGS,
                                             fileName,
                                             "Animation Player",
                                             this );
            }
            else
            {
                GetLoadingManager()->AddRequest( FILEHANDLER_ANIMATION,
                                             fileName,
                                             GMA_DEFAULT,
                                             fileName,
                                             "Animation Player",
                                             this );
            }

            mSection = tEntity::MakeUID(fileName);
        }
    }
}


//=============================================================================
// AnimationPlayer::LoadData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* fileName )
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::LoadData
( 
    const char* fileName,
    LoadDataCallBack* pCallback,
    bool bInInventory,
    void* pUserData
)
{
    mpLoadDataCallback = pCallback;

    this->LoadData( fileName, bInInventory, pUserData );
}

//=============================================================================
// AnimationPlayer::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::OnProcessRequestsComplete( void* pUserData )
{
    p3d::inventory->PushSection();

    p3d::inventory->SelectSection( mSection );
    bool currentOnly = p3d::inventory->GetCurrentSectionOnly( );
    p3d::inventory->SetCurrentSectionOnly( true );

    mState = ANIM_LOADED;

    this->DoLoaded();

    if( mpLoadDataCallback != NULL )
    {
        mpLoadDataCallback->OnLoadDataComplete();
    }

    if( mbPlayAfterLoad )
    {
        Play();
    }
    
    p3d::inventory->SetCurrentSectionOnly( currentOnly );

    p3d::inventory->PopSection();
}

//=============================================================================
// AnimationPlayer::Play
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::Play()
{
    if( mState == ANIM_STOPPED )
    {
        mState = ANIM_PLAYING;
    }

    if( mState == ANIM_LOADED )
    {
        mState = ANIM_PLAYING;

        if( mbExclusive )
        {
            EnterExclusive();
        }
    }
/* DARWIN TODO: Why was this necessary?
    else
    {
        mbPlayAfterLoad = true;
    }
*/
}


/*
//=============================================================================
// AnimationPlayer::StopAndCleanUp
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::StopAndCleanUp()
{
    Stop();
}
*/


//=============================================================================
// AnimationPlayer::Render
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::Render()
{
    if( (mState == ANIM_PLAYING) || mbShowAlways )
    {
        DoRender();
    }
}

//=============================================================================
// AnimationPlayer::Stop
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::Stop()
{
    if( mbExclusive )
    {
        LeaveExclusive();
    }

    //
    // Hope this makes sense.  Calling Stop() after ClearData() was causing
    // the player to be stuck in the stopped state, since LoadData() wouldn't
    // do anything.  Added check for idle state. -- DE
    //
    if( mState != ANIM_IDLE )
    {
        mState = ANIM_STOPPED;
    }
}

//=============================================================================
// AnimationPlayer::ClearData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::ClearData()
{
    mState = ANIM_IDLE;
    p3d::inventory->DeleteSection( mSection );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// AnimationPlayer::EnterExclusive
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::EnterExclusive()
{
    GetRenderManager()->FreezeForPresentation();

    RenderLayer* pLayer = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );

    if ( pLayer->IsDead() )
    {
        pLayer->Resurrect();
    }
    pLayer->Thaw();
}

//=============================================================================
// AnimationPlayer::LeaveExclusive
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimationPlayer::LeaveExclusive()
{
    if( !mbKeepLayersFrozen )
    {
        GetRenderManager()->ThawFromPresentation();

        RenderLayer* pLayer = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );

        pLayer->Freeze();
    }
}
