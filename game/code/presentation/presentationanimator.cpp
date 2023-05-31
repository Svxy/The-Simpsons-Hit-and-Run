//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        PresentationAnimator.cpp
//
// Description: Implement PresentationAnimator
//
// History:     9/24/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <ai/sequencer/actioncontroller.h>
#include <ai/sequencer/action.h>
#include <memory/srrmemory.h>

#include <presentation/blinker.h>
#include <presentation/mouthflapper.h>
#include <presentation/presentationanimator.h>

#include <worldsim/character/character.h>
#include <vector>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//=============================================================================
// Forward Declarations
//=============================================================================

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// PresentationAnimator::PresentationAnimator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PresentationAnimator::PresentationAnimator():
    mCharacter( NULL ),
    mMouthFlapper( NULL ),
    mRandomSelection( true ),
    mTalkTime(0)
{
    MEMTRACK_PUSH_GROUP( "PresentationAnimator" );
    mMouthFlapper = new(GMA_LEVEL_OTHER) MouthFlapper();
    mMouthFlapper->AddRef();
    MEMTRACK_POP_GROUP( "PresentationAnimator" );
}

//==============================================================================
// PresentationAnimator::~PresentationAnimator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PresentationAnimator::~PresentationAnimator()
{
    mMouthFlapper->Release();
}
//=============================================================================
// PresentationAnimator::AddAmbientAnimations
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void PresentationAnimator::AddAmbientAnimations( const TNAMEVECTOR& animations )
{
    if( !animations.empty() )
    {
        mAnimationNames.erase( mAnimationNames.begin(), mAnimationNames.end() );
        mAnimationNames.insert( mAnimationNames.begin(), animations.begin(), animations.end() );
    }
}

//=============================================================================
// PresentationAnimator::ClearAmbientAnimations
//=============================================================================
// Description: Comment
//
// Parameters:  
//
// Return:      void 
//
//=============================================================================
void PresentationAnimator::ClearAmbientAnimations( void )
{
    mAnimationNames.erase( mAnimationNames.begin(), mAnimationNames.end() );
}


//=============================================================================
// PresentationAnimator::ChooseNextAnimation
//=============================================================================
// Description: Chooses the next animation from a list of animation names
//
// Parameters:  NONE
//
// Return:      tName - the name of the animation chosen 
//
//=============================================================================
const tName PresentationAnimator::ChooseNextAnimation()
{
    size_t size = mAnimationNames.size();
    if( size == 0 )
    {
        return tName( "NONE" );
    }
    else
    {
        tName returnMe = mAnimationNames[ 0 ];
        mAnimationNames.erase( mAnimationNames.begin() );
        return returnMe;
    }
}

//=============================================================================
// PresentationAnimator::ChooseRandomAnimation
//=============================================================================
// Description: Chooses a random animation from the vector of potential 
//              animation names that have been set in the script
//
// Parameters:  NONE
//
// Return:      tName - the name of the animation chosen 
//
//=============================================================================
const tName PresentationAnimator::ChooseRandomAnimation() const
{
    int size = mAnimationNames.size();
    if( size == 0 )
    {
        return tName( "NONE" );
    }
    else
    {
        int randomNumber = rand();
        int selection = randomNumber % size;
        return mAnimationNames[ selection ];
    }
}

//=============================================================================
// PresentationAnimator::SetCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void PresentationAnimator::SetCharacter( Character* pCharacter )
{
    mCharacter = pCharacter;
    mMouthFlapper->SetCharacter( pCharacter );
}

//=============================================================================
// PresentationAnimator::GetCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Character
//
//=============================================================================
Character* PresentationAnimator::GetCharacter()
{
    return mCharacter;
}

//=============================================================================
// PresentationAnimator::PlaySpecialAmbientAnimation
//=============================================================================
// Description: triggers playing of a special ambient animation - ie homer 
//              scratching himself
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationAnimator::PlaySpecialAmbientAnimation()
{
    tName chosenAnimationName;
    if( mRandomSelection )
    {
        chosenAnimationName = ChooseRandomAnimation();
    }
    else
    {
        chosenAnimationName = ChooseNextAnimation();
    }

    if( ( chosenAnimationName == "NONE" ) || ( chosenAnimationName == "none" ) )
    {
        return;
    }

    if( mCharacter == NULL )
    {
        return;
    }
    bool canPlay = mCharacter->CanPlayAnimation( chosenAnimationName );
    if( canPlay )
    {
        ActionController* actionController = mCharacter->GetActionController();
        rAssert( actionController != NULL );
        Sequencer* pSeq = actionController->GetNextSequencer();
        rAssert( pSeq != NULL );
        if( !pSeq->IsBusy() )
        {
            pSeq->BeginSequence();
            PlayAnimationAction* pAction = 0;
            pAction = new PlayAnimationAction( mCharacter, chosenAnimationName );
            pAction->AbortWhenMovementOccurs( true );
            pSeq->AddAction( pAction );
            pSeq->EndSequence();
        }
    }
    else
    {
        #ifdef RAD_DEBUG
            const char* characterName = mCharacter->GetNameDangerous();
            const char* animationName = chosenAnimationName.GetText();
            rDebugPrintf
            ( 
                "PresentationAnimator::PlaySpecialAmbientAnimation, character'%s' cannont play'%s'\n",
                characterName, 
                animationName 
            );
        #endif
    }
}

//=============================================================================
// PresentationAnimator::SetRandomSelection
//=============================================================================
// Description: should the animations be chosen in order or randomly from the 
//              set of animations
//
// Parameters:  random - random or not
//
// Return:      NONE
//
//=============================================================================
void PresentationAnimator::SetRandomSelection( const bool random )
{
    mRandomSelection = random;
}

const bool PresentationAnimator::GetRandomSelection() const
{
    return mRandomSelection;
}


//=============================================================================
// PresentationAnimator::StartTalking
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationAnimator::StartTalking()
{
    if( mCharacter != NULL )
    {
        mCharacter->GetPuppet()->GetEngine()->GetPoseEngine()->AddPoseDriver( 2, mMouthFlapper );
    }
    mMouthFlapper->SetIsEnabled( true );
}

//=============================================================================
// PresentationAnimator::StopTalking
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationAnimator::StopTalking()
{
    mMouthFlapper->SetIsEnabled( false );
    if( mCharacter != NULL )
    {
        mCharacter->GetPuppet()->GetEngine()->GetPoseEngine()->RemovePoseDriver( 2, mMouthFlapper );
    }
}

//=============================================================================
// PresentationAnimator::Talkfor
//=============================================================================
void PresentationAnimator::TalkFor(int time)
{
    mTalkTime = time;
    StartTalking();
}

//=============================================================================
// PresentationAnimator::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void PresentationAnimator::Update( int elapsedTime )
{
    if(mTalkTime > 0)
    {
        if((mTalkTime - elapsedTime) < 0)
        {
            mTalkTime = 0;
            StopTalking();
        }
        else
        {
            mTalkTime -= elapsedTime;
        }
    }


    //mBlinker->Update( elapsedTime );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
