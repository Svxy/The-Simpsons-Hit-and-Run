//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TalkToObjective.cpp
//
// Description: Implement TalkToObjective
//
// History:     29/08/2002 + Created -- Cary Brisebois
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
#include <mission/objectives/TalkToObjective.h>
#include <mission/animatedicon.h>

#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>
#include <mission/gameplaymanager.h>
#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>
#include <presentation/gui/ingame/guiscreenletterbox.h>

#include <memory/srrmemory.h>

#include <render/DSG/inststatentitydsg.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/culling/worldscene.h>
#include <worldsim/traffic/trafficmanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// TalkToObjective::TalkToObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
TalkToObjective::TalkToObjective() :
    mTalkToTarget( NULL ),
    mHudMapIconID( -1 ),
    mEvtLoc( NULL ),
    mAnimatedIcon( NULL ),
    mIconType( EXCLAMATION ),
    mYOffset( 0.0f ),
    mTriggerRadius( 1.3f ),
    mRenderLayer( RenderEnums::LevelSlot ),
    mIsDisabled( false ),
    mActionID(-1),
    m_PrevActiveGameState(Input::ACTIVE_NONE)
{
    mArrowPath.mPathRoute.Allocate( RoadManager::GetInstance()->GetNumRoads() );    
}

//=============================================================================
// TalkToObjective::~TalkToObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
TalkToObjective::~TalkToObjective()
{
}


//=============================================================================
// TalkToObjective::SetTalkToTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name, IconType type, float yOffset, float rad )
//
// Return:      void 
//
//=============================================================================
void TalkToObjective::SetTalkToTarget( const char* name, IconType type, float yOffset, float rad )
{
    unsigned int len = strlen(name) < MAX_CHARACTER_NAME - 1? strlen(name) : MAX_CHARACTER_NAME - 2;
    strncpy( mCharacterName, name, len );
    mCharacterName[len] = '\0';

    mIconType = type;
    mYOffset = yOffset;
    mTriggerRadius = rad;

    //TODO: This should also enable the exclamation mark over his head.
}

//=============================================================================
// TalkToObjective::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* currentLoc )
//
// Return:      void 
//
//=============================================================================
void TalkToObjective::GetPosition( rmt::Vector* currentLoc )
{
    rAssert( mTalkToTarget );

    mTalkToTarget->GetPosition( *currentLoc );
}

//=============================================================================
// TalkToObjective::GetHeading
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* heading )
//
// Return:      void 
//
//=============================================================================
void TalkToObjective::GetHeading( rmt::Vector* heading )
{
    rAssert( mTalkToTarget );

    mTalkToTarget->GetFacing( *heading );
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// TalkToObjective::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      virtual 
//
//=============================================================================
void TalkToObjective::OnInitialize()
{
    rAssert( mCharacterName );

    MEMTRACK_PUSH_GROUP( "Mission - TalkToObjective" );

    //Setup the target
    //This should set the character that the objective is waiting to talk to
    Character* character = GetCharacterManager()->GetMissionCharacter( mCharacterName );

    if ( character )
    {
        mTalkToTarget = character;

        TrafficManager::GetInstance()->AddCharacterToStopFor( mTalkToTarget );
    }
    else
    {
#ifdef RAD_DEBUG
        char error[256];
        sprintf( error, "Can not find character: %s\n", mCharacterName );
        rAssertMsg( false, error );
#endif
    }


    //Create the Action Event button to trigger the talky-talk
    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();

    mEvtLoc = new(gma)EventLocator();

    rAssert( mEvtLoc );
    mEvtLoc->AddRef();

    mEvtLoc->SetName( mCharacterName );
    mEvtLoc->SetEventType( LocatorEvent::GENERIC_BUTTON_HANDLER_EVENT );
    
    ActionButton::GenericEventButtonHandler* pABHandler = dynamic_cast<ActionButton::GenericEventButtonHandler*>( ActionButton::GenericEventButtonHandler::NewAction( mEvtLoc, EVENT_TALK_TO_NPC, gma ) ); 
    rAssert( pABHandler );

    pABHandler->SetEventData( mTalkToTarget );

    bool bResult = GetActionButtonManager()->AddAction( pABHandler, mActionID );
    
    rAssert( bResult );
    
    mEvtLoc->SetData( mActionID );

    // Radius should equal about 1m.
    //
    float r = mTriggerRadius;
    rmt::Vector charPos;
    mTalkToTarget->GetPosition( charPos );
    SphereTriggerVolume* pTriggerVolume = new(gma) SphereTriggerVolume( charPos, r );

    mEvtLoc->SetNumTriggers( 1, gma );
    
    mEvtLoc->AddTriggerVolume( pTriggerVolume );
    pTriggerVolume->SetLocator( mEvtLoc );
    pTriggerVolume->SetName( "Talk Trigger" );   

    TriggerVolumeTracker::GetInstance()->AddTrigger( pTriggerVolume );

    //This should be the event that the ActionButtonManager sends...
    GetEventManager()->AddListener( this, EVENT_TALK_TO_NPC );

    rmt::Vector pos;
    mTalkToTarget->GetPosition( pos );

    pos.y += mYOffset;

    //Put the location on the HUD
    RegisterPosition( pos, mHudMapIconID, true, HudMapIcon::ICON_MISSION, this );


    //========================= SET UP THE ICON

    rAssert( mAnimatedIcon == NULL );

    mAnimatedIcon = new(gma) AnimatedIcon();

    const char* iconName;

    if ( mIconType == EXCLAMATION )
    {
        iconName = "exclamation";
    }
    else if ( mIconType == GIFT )
    {
        iconName = "gift";
    }
    else if ( mIconType == DOOR )
    {
        iconName = "interior_icon";
    }
    else 
    {
        rAssert( false );
        MEMTRACK_POP_GROUP("Mission - TalkToObjective");
        return;
    }
    
    mAnimatedIcon->Init( iconName, pos );

    LightPath( charPos, mArrowPath );


    MEMTRACK_POP_GROUP("Mission - TalkToObjective");
}

//=============================================================================
// TalkToObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      virtual 
//
//=============================================================================
void TalkToObjective::OnFinalize()
{
    rAssert( mAnimatedIcon );

    if(mActionID != -1)
    {
        GetCharacterManager()->GetCharacter(0)->RemoveActionButtonHandler(GetActionButtonManager()->GetActionByIndex(mActionID));
        GetActionButtonManager()->RemoveActionByIndex(mActionID);
        mActionID = -1;
    }

    if( mTalkToTarget != NULL )
    {
        TrafficManager::GetInstance()->RemoveCharacterToStopFor( mTalkToTarget );
    }

    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;
    }

    TriggerVolumeTracker::GetInstance()->RemoveTrigger( mEvtLoc->GetTriggerVolume( 0 ) );
    mEvtLoc->Release();
    mEvtLoc = NULL;

    if(m_PrevActiveGameState != Input::ACTIVE_NONE)
    {
    	GetInputManager()->SetGameState(m_PrevActiveGameState);
    }

    GetEventManager()->RemoveListener( this, EVENT_TALK_TO_NPC );

    UnregisterPosition( mHudMapIconID );

    UnlightPath( mArrowPath.mPathRoute );
}

//=============================================================================
// TalkToObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      virtual 
//
//=============================================================================
void TalkToObjective::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == EVENT_TALK_TO_NPC )
    {
        //Hey hey!

        //Test the event data to make sure it's the right NPC

        if ( mTalkToTarget == (Character*)pEventData )
        {
            TriggerVolumeTracker::GetInstance()->RemoveTrigger( mEvtLoc->GetTriggerVolume( 0 ) );
            SetFinished( true );

            //
            // Disable controller input so that you can't pause the game
            //
			m_PrevActiveGameState = GetInputManager()->GetGameState();
            GetInputManager()->SetGameState( Input::ACTIVE_NONE );
            CGuiScreenLetterBox::UnSurpressSkipButton();
        }
    }
}

//=============================================================================
// TalkToObjective::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void TalkToObjective::OnUpdate( unsigned int elapsedTime )
{
    //Update the position of the bv...
    rmt::Vector charPos;
    mTalkToTarget->GetPosition( charPos );

    if ( mTalkToTarget->IsBusy() && !mIsDisabled )
    {
        //Turn off the trigger volume.
        TriggerVolumeTracker::GetInstance()->RemoveTrigger( mEvtLoc->GetTriggerVolume( 0 ) );
        mIsDisabled = true;
    }
    else if ( !mTalkToTarget->IsBusy() && mIsDisabled )
    {
        //Turn it back on.
        TriggerVolumeTracker::GetInstance()->AddTrigger( mEvtLoc->GetTriggerVolume( 0 ) );
        mIsDisabled = false;
    }

    if ( !mIsDisabled )
    {
        mEvtLoc->GetTriggerVolume( 0 )->SetPosition( charPos );
    }

    charPos.y += mYOffset;

    mAnimatedIcon->Move( charPos );
    mAnimatedIcon->Update( elapsedTime );
    UpdateLightPath(mArrowPath);
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
