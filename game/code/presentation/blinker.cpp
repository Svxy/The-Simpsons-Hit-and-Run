//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Blinker.cpp
//
// Description: Implement Blinker
//
// History:     9/24/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/anim/textureanimation.hpp>
#include <stdlib.h>

//========================================
// Project Includes
//========================================

#include <memory/srrmemory.h>
#include <presentation/blinker.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <main/game.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

static const int MIN_TIME = 2000;
static const int MAX_TIME = 4000;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Blinker::Blinker
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Blinker::Blinker():
    mCharacter( NULL ),
    mController( NULL ) ,
    mState( STATE_BLINKING )
{
}

//==============================================================================
// Blinker::~Blinker
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Blinker::~Blinker()
{
    if( mController != NULL )
    {
        mController->Release();
    }
}

//=============================================================================
// Blinker:SetCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void Blinker::SetCharacter( Character* pCharacter )
{
//    if( mCharacter  == pCharacter )
//    {
//        return;
//    }
    mCharacter = pCharacter;

    if (pCharacter == 0)
    {
        if (mController)
        {
            mController->Release ();
            mController = 0;
        }
    }
    else
    {
        const char* modelName = GetCharacterManager()->GetModelName( mCharacter );
        tMultiController* multicontroller = 0;
        if( tName::MakeUID( modelName ) != tName::MakeUID( "npd" ) )
        {
            multicontroller = p3d::find< tMultiController >( modelName );
        }
        tRefCounted::Assign( mController, multicontroller );
    }

    if( mController == NULL )
    {
        return;
    }

    mController->SetCycleMode( FORCE_NON_CYCLIC );
}

//=============================================================================
// Blinker::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void Blinker::Update( int elapsedTime )
{
    mTimeSinceBlink += elapsedTime;

    if( mController == NULL )
    {
        if( (mCharacter->GetActiveFrame() & 0xf) == (GetGame()->GetFrameCount() & 0xf))
        {
            mTimeSinceBlink = 0;
            const char* modelName = GetCharacterManager()->GetModelName( mCharacter );
            tMultiController* multicontroller = 0;
            if(tName::MakeUID(modelName) != tName::MakeUID("npd"))
            {
                multicontroller = p3d::find< tMultiController >( modelName );
            }
            if( multicontroller != NULL )
            {
                tRefCounted::Assign( mController, multicontroller );
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    }

    switch( mState )
    {
    case STATE_WAITING:
        {
            rAssert( mController != NULL );
            if( mTimeSinceBlink >= mTimeTarget )
            {
                mController->Reset();
                mState = STATE_BLINKING;
            }

            break;
        }
    case STATE_BLINKING:
        {
            rAssert( mController != NULL );

            mController->Advance( static_cast<float>( elapsedTime ), true );

            float frames = mController->GetNumFrames();
            float frame = mController->GetFrame();
            if( frame >= frames )
            {
                mTimeTarget = MIN_TIME + (MAX_TIME - MIN_TIME) * rand() / RAND_MAX;
                mTimeSinceBlink = 0;
                mState = STATE_WAITING;
            }

            break;
        }
    default:
        {
            break;
        }
    }
}

//=============================================================================
// Blinker::StartBlinking
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Blinker::StartBlinking()
{
    if( mState == STATE_INVALID )
    {
        mState = STATE_BLINKING;
    }
}

//=============================================================================
// Blinker::StopBlinking
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Blinker::StopBlinking()
{
    if( mState != STATE_INVALID )
    {
        mState = STATE_INVALID;
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
