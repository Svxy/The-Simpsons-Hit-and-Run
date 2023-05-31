//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionOver
//
// Description: Implementation of the CGuiScreenMissionOver class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenmissionover.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>

#include <events/eventmanager.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/mission.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <sound/soundmanager.h>

#include <p3d/unicode.hpp>
#include <raddebug.hpp> // Foundation
#include <layer.h>
#include <page.h>
#include <screen.h>
#include <text.h>

#include <stdlib.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const int NUM_ATTEMPTS_REQUIRED_FOR_SKIPPING = 7;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMissionOver::CGuiScreenMissionOver
//===========================================================================
// Description: Constructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenMissionOver::CGuiScreenMissionOver
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MISSION_OVER ),
    m_pMenu( NULL ),
    m_failureReason( NULL ),
    m_failureHint( NULL )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenMissionOver" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "MissionOver" );
	rAssert( pPage );

    Scrooby::Layer* foreground = pPage->GetLayer( "Foreground" );
    rAssert( foreground != NULL );

    // get 'mission failed' text bible string
    //
    P3D_UNICODE* text = GetTextBibleString( "MISSION_FAILED" );
    rAssert( text != NULL );
    int textLength = p3d::UnicodeStrLen( text ) + 1;

    Scrooby::Sprite* missionFailedText = foreground->GetSprite( "MissionFailed" );
    if( missionFailedText != NULL )
    {
        missionFailedText->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
        missionFailedText->CreateBitmapTextBuffer( textLength );
        missionFailedText->SetBitmapText( text );
        missionFailedText->SetBitmapTextLineSpacing( 10 );
#ifdef RAD_WIN32
        missionFailedText->ResetTransformation();
        missionFailedText->ScaleAboutCenter( 0.5f );
#endif
    }

    m_failureReason = foreground->GetText( "MissionFailureReason" );
    rAssert( m_failureReason != NULL );
    m_failureReason->SetTextMode( Scrooby::TEXT_WRAP );

    m_failureHint = foreground->GetText( "MissionFailureHint" );
    rAssert( m_failureHint != NULL );
    m_failureHint->SetTextMode( Scrooby::TEXT_WRAP );

    // Create a menu.
    //
    m_pMenu = new CGuiMenuPrompt( this, pPage, 3 );
    rAssert( m_pMenu != NULL );

    // register events to listen for
    //
    GetEventManager()->AddListener( this, EVENT_MISSION_FAILURE );

MEMTRACK_POP_GROUP("CGUIScreenMissionOver");
}


//===========================================================================
// CGuiScreenMissionOver::~CGuiScreenMissionOver
//===========================================================================
// Description: Destructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenMissionOver::~CGuiScreenMissionOver()
{
    // unregister events
    //
    GetEventManager()->RemoveListener( this, EVENT_MISSION_FAILURE );

    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenMissionOver::HandleMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionOver::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                if( param1 == 0 ) // 'yes' response
                {
                    // restart mission
                    //
                    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME,
                                              ON_HUD_ENTER_RESTART_MISSION );
                }
                else if( param1 == 1 ) // 'no' response
                {
                    // abort mission
                    //
                    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME,
                                              ON_HUD_ENTER_ABORT_MISSION );
                }
                else if( param1 == 2 ) // 'skip' response
                {
                    // skip to next mission
                    //
                    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME,
                                              ON_HUD_ENTER_SKIP_MISSION );
/*
                    CurrentMissionStruct currentMission = GetCharacterSheetManager()->QueryCurrentMission();

                    unsigned int nextLevel = currentMission.mLevel;
                    unsigned int nextMission = currentMission.mMissionNumber + 1;

                    if( currentMission.mMissionNumber == RenderEnums::M7 ) // last mission
                    {
                        // go to next level's first mission
                        //
                        nextLevel++;
                        nextMission = RenderEnums::M1;

                        rAssert( nextLevel < RenderEnums::numLevels );
                    }
*/
                }
                else
                {
                    rAssertMsg( 0, "WARNING: *** Unexpected response!\n" );
                }

                break;
            }

            default:
            {
                break;
            }
        }

        // relay message to menu
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

void
CGuiScreenMissionOver::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_MISSION_FAILURE:
        {
            MissionCondition* failureCondition = reinterpret_cast<MissionCondition*>( pEventData );
            rAssert( failureCondition != NULL );

            this->SetFailureMessage( failureCondition->GetType() );

            break;
        }
        default:
        {
            rWarningMsg( false, "Unhandled event ID!" );

            break;
        }
    }
}


//===========================================================================
// CGuiScreenMissionOver::InitIntro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionOver::InitIntro()
{
    rAssert( m_pMenu != NULL );

    // reset menu to "Yes" selection
    //
    m_pMenu->Reset();

    // show/hide 'skip' menu response
    //
    CurrentMissionStruct currentMission = GetCharacterSheetManager()->QueryCurrentMission();
    int numAttemps = GetCharacterSheetManager()->QueryNumberOfAttempts( currentMission.mLevel, currentMission.mMissionNumber );
    bool isSkipAllowed = (numAttemps >= NUM_ATTEMPTS_REQUIRED_FOR_SKIPPING);

    if( currentMission.mLevel == RenderEnums::L7 && static_cast<int>( currentMission.mMissionNumber ) >= RenderEnums::M5 )
    {
        // can't skip the last mission of the game
        //
        isSkipAllowed = false;
    }

#ifdef RAD_E3
    // no skipping mission allowed on E3 build
    //
    isSkipAllowed = false;
#endif

    m_pMenu->SetMenuItemEnabled( 2, isSkipAllowed, true );

    GetSoundManager()->OnStoreScreenStart( false );
}


//===========================================================================
// CGuiScreenMissionOver::InitRunning
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionOver::InitRunning()
{
}


//===========================================================================
// CGuiScreenMissionOver::InitOutro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionOver::InitOutro()
{
    GetEventManager()->TriggerEvent( EVENT_DIALOG_SHUTUP );
    GetSoundManager()->OnStoreScreenEnd();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMissionOver::SetFailureMessage( MissionCondition::ConditionTypeEnum conditionType )
{
    int failureMessageIndex = 0;

    switch( conditionType )
    {
        case MissionCondition::COND_VEHICLE_DAMAGE:
        {
            failureMessageIndex = 1;

            break;
        }
        case MissionCondition::COND_PLAYER_HIT:
        {
            failureMessageIndex = 2;

            break;
        }
        case MissionCondition::COND_TIME_OUT:
        {
            failureMessageIndex = 3;

            break;
        }
        case MissionCondition::COND_PLAYER_OUT_OF_VEHICLE:
        {
            failureMessageIndex = 4;

            break;
        }
        case MissionCondition::COND_FOLLOW_DISTANCE:
        {
            failureMessageIndex = 5;

            break;
        }
        case MissionCondition::COND_OUT_OF_BOUNDS:
        {
            failureMessageIndex = 6;

            break;
        }
        case MissionCondition::COND_RACE:
        {
            failureMessageIndex = 7;

            break;
        }
        case MissionCondition::COND_NOT_ABDUCTED:
        {
            failureMessageIndex = 8;

            break;
        }
        case MissionCondition::COND_POSITION:
        {
            failureMessageIndex = 9;

            break;
        }
        case MissionCondition::COND_HIT_AND_RUN_CAUGHT:
        {
            failureMessageIndex = 10;

            break;
        }
        case MissionCondition::COND_GET_COLLECTIBLES:
        {
            failureMessageIndex = 11;

            break;
        }
        default:
        {
            rAssertMsg( false, "Unknown failure reason!" );

            break;
        }
    }

    rAssert( m_failureReason != NULL );
    m_failureReason->SetIndex( failureMessageIndex );

    // set associated mission failure hint (randomly chosen)
    //
    int hintMessageIndex = 0;

    if( failureMessageIndex > 0 )
    {
        rAssert( GetGameplayManager()->GetCurrentMission() != NULL );

        int numValidFailureHints = GetGameplayManager()->GetCurrentMission()->GetNumValidFailureHints();
        if( numValidFailureHints < 0 )
        {
            numValidFailureHints = MAX_NUM_HINTS_PER_FAILURE;
        }

        int randomHintIndex = rand() % numValidFailureHints;

        hintMessageIndex = (failureMessageIndex - 1) * MAX_NUM_HINTS_PER_FAILURE + 1 + randomHintIndex;
    }

    rAssert( m_failureHint != NULL );
    m_failureHint->SetIndex( hintMessageIndex );
}

