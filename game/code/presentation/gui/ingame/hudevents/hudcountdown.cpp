//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudCountDown
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudcountdown.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guitextbible.h>

#include <events/eventmanager.h>
#include <input/controller.h>
#include <input/inputmanager.h>
#include <mission/gameplaymanager.h>
#include <gameflow/gameflow.h>
#include <worldsim/character/charactermanager.h>

// Scrooby
#include <group.h>
#include <page.h>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

HudCountDown::HudCountDown( Scrooby::Page* pPage )
:   HudEventHandler( pPage->GetGroup( "CountDown" ) ),
    m_countDownMessage( NULL ),
    m_missionStage( NULL ),
    m_currentSequenceUnit( NULL ),
    m_nextSequenceIndex( 0 ),
    m_InputDisablePending( false ),
    m_isDialogTriggerPending( false )
{
//    const float BITMAP_TEXT_SPACING = 0.8f;

    rAssert( pPage != NULL );
    m_countDownMessage = pPage->GetSprite( "CountDown" );
    rAssert( m_countDownMessage != NULL );
    m_countDownMessage->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_countDownMessage->CreateBitmapTextBuffer( 32 );
//    m_countDownMessage->SetBitmapTextSpacing( BITMAP_TEXT_SPACING );
}

HudCountDown::~HudCountDown()
{
}

//=============================================================================
// HudCountDown::OnStart
//=============================================================================
// Description: Called when the HudCountDown starts
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void HudCountDown::OnStart()
{
    Parent::OnStart();
}

//=============================================================================
// HudCountDown::QueueDisableInput
//=============================================================================
// Description: tells the object that the next time update is called, we need
//              to disable the input system
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void HudCountDown::QueueDisableInput()
{
    m_InputDisablePending = true;
}

void
HudCountDown::Start()
{
    Character* npcPtr;
    const char* modelName;

    this->OnStart();

    m_missionStage = GetGameplayManager()->GetCurrentMission()->GetCurrentStage();

    m_nextSequenceIndex = 0;
    this->GetNextSequenceUnit();

    // get dialog event data and queue it up for triggering on the next update
    //
    BonusMissionInfo* bonusMissionInfo = const_cast<BonusMissionInfo*>( GetGameplayManager()->GetCurrentBonusMissionInfo() );
    if( bonusMissionInfo != NULL )
    {
        npcPtr = bonusMissionInfo->GetNPC();
        rAssert( npcPtr != NULL );
        modelName = GetCharacterManager()->GetModelName( npcPtr );

        m_dialogData.charUID1 = tEntity::MakeUID( modelName );
        m_dialogData.charUID2 = m_missionStage->GetCountdownSecondSpeakerUID();
        m_dialogData.dialogName = m_missionStage->GetCountdownDialogID();

        m_isDialogTriggerPending = true;
    }

    QueueDisableInput();
}

void
HudCountDown::Stop()
{
    m_missionStage = NULL;
    m_currentSequenceUnit = NULL;

    // enable the controller - yes i know this is not an animated cam
    //
    if( GetInputManager()->GetGameState() == Input::ACTIVE_ANIM_CAM )
    {
        InputManager::GetInstance()->SetGameState( Input::DEACTIVE_ANIM_CAM );
    }

    this->OnStop();
}

void
HudCountDown::Update( float elapsedTime )
{
    if( m_InputDisablePending )
    {
        // disable the controller - yes i know this is not an animated cam
        //
        InputManager::GetInstance()->SetGameState( Input::ACTIVE_ANIM_CAM );
        m_InputDisablePending = false;
    }

    if( m_currentState == STATE_RUNNING )
    {
        if( m_isDialogTriggerPending )
        {
            m_isDialogTriggerPending = false;

            GetEventManager()->TriggerEvent( EVENT_IN_GAMEPLAY_CONVERSATION, static_cast<void*>( &m_dialogData ) );
        }

#ifdef RAD_WIN32
        static float COUNT_DOWN_MAX_SCALE = 1.5f;
#else
        static float COUNT_DOWN_MAX_SCALE = 3.0f;
#endif
        static float COUNT_DOWN_THRESHOLD_SCALE = 2.5f;
        static int COUNT_DOWN_ZOOM_RATE = 2;

        if( m_currentSequenceUnit != NULL )
        {
            bool done = GuiSFX::Flash( m_countDownMessage,
                                       m_elapsedTime,
                                       (float)m_currentSequenceUnit->durationTime,
                                       COUNT_DOWN_ZOOM_RATE,
                                       COUNT_DOWN_MAX_SCALE,
                                       COUNT_DOWN_THRESHOLD_SCALE );
            if( done )
            {
                m_elapsedTime = 0;

                this->GetNextSequenceUnit();
            }
        }
        else
        {
            // countdown finished
            //
            this->Stop();

            // start the mission!
            //
            GetEventManager()->TriggerEvent( EVENT_GUI_MISSION_START );

            //
            // Take all the AI cars out of the limbo state
            //
            MissionStage* stage = GetGameplayManager()->GetCurrentMission()->GetCurrentStage();
            rAssert( stage != NULL );
            stage->PutAllAisInLimbo( false );
        }

        m_elapsedTime += elapsedTime;
    }
}

void
HudCountDown::GetNextSequenceUnit()
{
    rAssert( m_missionStage != NULL );
    m_currentSequenceUnit = m_missionStage->GetCountdownSequenceUnit( m_nextSequenceIndex );

    if( m_currentSequenceUnit != NULL )
    {
        // set text message
        //
        rAssert( m_countDownMessage != NULL );
        m_countDownMessage->SetAlpha( 0.0f );

        P3D_UNICODE* text = GetTextBibleString( m_currentSequenceUnit->textID );
        if( text != NULL )
        {
            m_countDownMessage->SetBitmapText( text );
        }
        else
        {
            m_countDownMessage->SetBitmapText( "" );
        }
    }

    m_nextSequenceIndex++;
}

