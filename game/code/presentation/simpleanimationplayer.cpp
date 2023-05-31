//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        simpleanimationplayer.cpp
//
// Description: Implement SimpleAnimationPlayer
//
// History:     29/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/camera.hpp>
#include <p3d/utility.hpp>
#include <p3d/view.hpp>

//========================================
// Project Includes
//========================================
#include <presentation/simpleanimationplayer.h>



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
// SimpleAnimationPlayer::SimpleAnimationPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
SimpleAnimationPlayer::SimpleAnimationPlayer() :
    mpMasterController( NULL ),
    mCycleMode( DEFAULT_CYCLE_MODE ),
    mpCamera( NULL ),
    mpViewCamera( NULL ),
    mbSetCamera( false ),
    mIntroFrames(0),
    mOutroFrames(0),
    mInIntro(false)
{
    strcpy( msCamera, "" );
    strcpy( msController, "" );
    strcpy( msAnimation, "" );
}

//==============================================================================
// SimpleAnimationPlayer::~SimpleAnimationPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
SimpleAnimationPlayer::~SimpleAnimationPlayer()
{
    tRefCounted::Release( mpViewCamera );
}

//=============================================================================
// SimpleAnimationPlayer::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void SimpleAnimationPlayer::Update( unsigned int elapsedTime )
{
    if ( (GetState() == ANIM_PLAYING) && mpMasterController)
    {
        if(mOutroFrames && (mpMasterController->GetFrame() > static_cast<float>(mNumFrames - mOutroFrames)))
        {
            mpMasterController->SetFrameRange(static_cast<float>(mNumFrames - mOutroFrames), static_cast<float>(mNumFrames));
            mpMasterController->SetCycleMode(FORCE_CYCLIC);
        }

        mpMasterController->Advance( static_cast<float>(elapsedTime) );
    }
}


//==============================================================================
// SimpleAnimationPlayer::Rewind
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void SimpleAnimationPlayer::Rewind()
{
    if(mpMasterController)
    {
        mpMasterController->Reset();
        mpMasterController->Advance(0.0f);

        SetState( ANIM_LOADED );
    }
}

//=============================================================================
// SimpleAnimationPlayer::ClearData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SimpleAnimationPlayer::ClearData()
{
    AnimationPlayer::ClearData();

    if( mpMasterController != NULL )
    {
        mpMasterController->Release();
        mpMasterController = NULL;
    }

    mCycleMode = DEFAULT_CYCLE_MODE;

    if( mpCamera != NULL )
    {
        mpCamera->Release();
        mpCamera = NULL;
        mbSetCamera = false;
    }
}

//=============================================================================
// SimpleAnimationPlayer::SetNameData
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* controller, char* camera, char* animation )
//
// Return:      void 
//
//=============================================================================
void SimpleAnimationPlayer::SetNameData( char* controller, char* camera, char* animation )
{
    strcpy( msController, controller );
    if( camera != NULL )
    {
        strcpy( msCamera, camera );
    }
    strcpy( msAnimation, animation );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SimpleAnimationPlayer::DoLoaded
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SimpleAnimationPlayer::DoLoaded()
{
    tRefCounted::Assign(mpMasterController, p3d::find<tMultiController>( msController ));

    if(mpMasterController)
    {
        // reset to the first frame, and update (so evrything is in the correct 
        // place if we display before calling update again)
        mpMasterController->Reset();
        mpMasterController->SetFrame(0);
        mpMasterController->Advance(0.0f);

        mpMasterController->SetCycleMode( mCycleMode );

        mNumFrames = rmt::FtoL(mpMasterController->GetNumFrames());

        if(mIntroFrames != 0)
        {
            mpMasterController->SetFrameRange(0.0f, (float)mIntroFrames);
            mpMasterController->SetCycleMode( FORCE_CYCLIC );
        }
    }

    if( strlen( msCamera ) > 0 )
    {
        tRefCounted::Assign(mpCamera,p3d::find<tCamera>( msCamera ));
    }

    mbSetCamera = false;
}

//=============================================================================
// SimpleAnimationPlayer::DoRender
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SimpleAnimationPlayer::DoRender()
{
    if(!mpMasterController)
    {
        Stop();
        return;
    }

    if ( !mbSetCamera && mpCamera != NULL )
    {
        tView* view = p3d::context->GetView();
        rAssert( view );

        mpViewCamera = view->GetCamera();
        mpViewCamera->AddRef();

        view->SetCamera( mpCamera );

        mbSetCamera = true;
    }

    if ( (mpMasterController->GetFrame() >= mpMasterController->GetNumFrames()) && (mOutroFrames == 0))
    {
        if ( mbSetCamera )
        {
            tView* view = p3d::context->GetView();
            rAssert( view );

            view->SetCamera( mpViewCamera );
            tRefCounted::Release( mpViewCamera );
        }

        Stop();
    }
}


void SimpleAnimationPlayer::SetIntroLoop(unsigned nFrames)
{
    mIntroFrames = nFrames;
}

void SimpleAnimationPlayer::SetOutroLoop(unsigned nFrames)
{
    mOutroFrames = nFrames;
}

void SimpleAnimationPlayer::Play(void)
{
    AnimationPlayer::Play();

    if(mIntroFrames)
    {
        mInIntro = true;
    }
}

void SimpleAnimationPlayer::DoneIntro(void)
{
    if(mpMasterController)
    {
        mInIntro = false;
        mpMasterController->SetFrameRange(0.0f, static_cast<float>(mNumFrames));
        mpMasterController->SetCycleMode(mCycleMode);
    }
}
