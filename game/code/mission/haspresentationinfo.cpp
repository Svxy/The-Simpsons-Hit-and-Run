//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        haspresentationinfo.cpp
//
// Description: Implement haspresentationinfo class
//
// History:     06/05/2003 + Created -- Ian Gipson
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <camera/conversationcam.h>
#include <events/eventmanager.h>
#include <memory/classsizetracker.h>
#include <mission/haspresentationinfo.h>
#include <presentation/presentation.h>
#include <presentation/presentationanimator.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
#define TYPICAL_NUM_ANIMATIONS 16
#define TYPICAL_NUM_LINES 8

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// HasPresentationInfo::HasPresentationInfo
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
HasPresentationInfo::HasPresentationInfo():
    mConversationCamName   ( "unknown" ),  
    mConversationCamNpcName( "npc_near" ),
    mConversationCamPcName ( "pc_near" ),
    mPcIsChild( false ),
    mNpcIsChild( false ),
    mGoToPattyAndSelmaScreenWhenDone( false )
{
    CLASSTRACKER_CREATE( HasPresentationInfo );
    mAmbientPcAnimations.reserve( TYPICAL_NUM_ANIMATIONS );
    mAmbientNpcAnimations.reserve( TYPICAL_NUM_ANIMATIONS );
    mCamerasForLinesOfDialog.reserve( TYPICAL_NUM_LINES );
}


//=============================================================================
// HasPresentationInfo::HasPresentationInfo
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
HasPresentationInfo::~HasPresentationInfo()
{
    CLASSTRACKER_DESTROY( HasPresentationInfo );
}

//=============================================================================
// HasPresentationInfo::AddAmbientCharacterAnimation
//=============================================================================
// Description: adds an animation name to the list that will be randomly chosen
//              for a specific character in the conversation
// Parameters:  character 0 = PC
//                        1 = NPC
//              animationName - the tName representing the animation
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::AddAmbientCharacterAnimation( const unsigned int character, const tName& animationName )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    rAssertMsg( character < 2, "There is only code for the PC and 1 NPC" );
    const unsigned int index = character % 2;
    if( index == 0 )
    {
        mAmbientPcAnimations.push_back( animationName );
    }
    else
    {
        mAmbientNpcAnimations.push_back( animationName );
    }
    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}

//=============================================================================
// HasPresentationInfo::AmbientCharacterAnimationSetRandom
//=============================================================================
// Description: determines whether or not to randomize animations
// Parameters:  character 0 = PC
//                        1 = NPC
//              random - should animation selection be random or not?
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::AmbientCharacterAnimationSetRandom( const unsigned int character, const bool random )
{
    if( character == 0 )
    {
        mAmbientPcAnimationsRandom = random;
    }
    else
    {
        mAmbientNpcAnimationsRandom = random;
    }
}

//=============================================================================
// HasPresentationInfo::CharacterIsChild
//=============================================================================
// Description: marks a specific character as being a child 
// Parameters:  index - which character
//              0 = PC
//              1 = NPC
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::CharacterIsChild( const int index )
{
    if( index == 0 )
    {
        mPcIsChild = true;
    }
    else if( index == 1 )
    {
        mNpcIsChild = true;
    }
    else
    {
        rAssertMsg( false, "character index out of range" );
    }
}

//=============================================================================
// HasPresentationInfo::ClearAmbientAnimations
//=============================================================================
// Description: clears out all the ambient animations that have been set up
//
// Parameters:  
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::ClearAmbientAnimations()
{
    mAmbientPcAnimations.erase(  mAmbientPcAnimations.begin(),  mAmbientPcAnimations.end()  );
    mAmbientNpcAnimations.erase( mAmbientNpcAnimations.begin(), mAmbientNpcAnimations.end() );
}

//=============================================================================
// HasPresentationInfo::GoToPattyAndSelmaScreenWhenDone
//=============================================================================
// Description: if this is set, the race or mission will go to the patty and
//              selma screen when it is completed
//
// Parameters:  
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::GoToPattyAndSelmaScreenWhenDone()
{
    mGoToPattyAndSelmaScreenWhenDone = true;
}

//=============================================================================
// HasPresentationInfo::OnStageCompleteSuccessful
//=============================================================================
// Description: called when the mission is complete
//
// Parameters:  
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::OnStageCompleteSuccessful() const
{
    if( mGoToPattyAndSelmaScreenWhenDone )
    {
        GetEventManager()->TriggerEvent( EVENT_GUI_TRIGGER_PATTY_AND_SELMA_SCREEN );
    }
}

//=============================================================================
// HasPresentationInfo::Reset
//=============================================================================
// Description: called to activate all the info stored in this class
//
// Parameters:  
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::Reset()
{
    PresentationManager* pm = PresentationManager::GetInstance();

    //
    // Set Up the Conversation cameras
    //
    ConversationCam::SetNpcIsChild( mNpcIsChild );
    ConversationCam::SetPcIsChild( mPcIsChild );
    ConversationCam::SetNpcCameraByName( mConversationCamNpcName );
    ConversationCam::SetPcCameraByName( mConversationCamPcName );
    size_t size = mCamerasForLinesOfDialog.size();
    rAssert( size < 10 );
    pm->SetCamerasForLineOfDialog( mCamerasForLinesOfDialog );
    ConversationCam::SetCamBestSide( mBestSideLocator );

    //
    // Set up the Ambient Animations
    PresentationAnimator* paPc  = pm->GetAnimatorPc();
    PresentationAnimator* paNpc = pm->GetAnimatorNpc();
    size = mAmbientPcAnimations.size();
    paPc-> AddAmbientAnimations( mAmbientPcAnimations );
    paNpc->AddAmbientAnimations( mAmbientNpcAnimations );
    paPc->SetRandomSelection( mAmbientPcAnimationsRandom );
    paNpc->SetRandomSelection( mAmbientNpcAnimationsRandom );

}

//=============================================================================
// HasPresentationInfo::SetCameraForDialogLine
//=============================================================================
// Description: some lines of dialog need specific cameras attached to them
// Parameters:  dialogLine - which dialog line do we care about?
//              camera - which camera should we use
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::SetCameraForDialogLine( const unsigned int dialogLine, const tName& camera )
{
    size_t currentSize = mCamerasForLinesOfDialog.size();
    if( currentSize < dialogLine + 1 )
    {
        mCamerasForLinesOfDialog.resize( dialogLine + 1, tName( "NONE" ) );
    }
    mCamerasForLinesOfDialog[ dialogLine ] = camera;
}

//=============================================================================
// HasPresentationInfo::SetConversationCamName
//=============================================================================
// Description: sets the name of the conversation camera used by default
// Parameters:  name - the tname representing this camera
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::SetConversationCamName( const tName& name )
{
    mConversationCamName = name;
}
//=============================================================================
// HasPresentationInfo::SetConversationCamPcName
//=============================================================================
// Description: sets the name of the conversation camera used when the PC is 
//              talking
// Parameters:  name - the tname representing this camera
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::SetConversationCamPcName ( const tName& name )
{
    mConversationCamPcName = name;
}
//=============================================================================
// HasPresentationInfo::SetConversationCamNpcName
//=============================================================================
// Description: sets the name of the conversation camera used when the PC is 
//              talking
// Parameters:  name - the tname representing this camera
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::SetConversationCamNpcName( const tName& name )
{
    mConversationCamNpcName = name;
}

//=============================================================================
// HasPresentationInfo::SetBestSideLocator
//=============================================================================
// Description: sets the name of the bestsidelocator for this stage
// Parameters:  name - the tname representing this locator
//
// Return:      void
//
//=============================================================================
void HasPresentationInfo::SetBestSideLocator( const tName& name )
{
    mBestSideLocator = name;
}