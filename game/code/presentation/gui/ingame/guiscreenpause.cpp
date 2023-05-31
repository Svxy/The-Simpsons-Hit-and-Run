//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPause
//
// Description: Implementation of the CGuiScreenPause class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/11/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenpause.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guihudtextbox.h>
#include <presentation/gui/ingame/hudevents/hudmissionobjective.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <data/memcard/memorycardmanager.h>

#include <cards/card.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <worldsim/coins/coinmanager.h>

// Pure3D
#include <p3d/utility.hpp>
#include <p3d/sprite.hpp>

#include <raddebug.hpp> // Foundation
#include <radtime.hpp>

#include <stdlib.h>
#include <string.h>

#include <app.h>
#include <group.h>
#include <layer.h>
#include <page.h>
#include <screen.h>
#include <sprite.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPause::CGuiScreenPause
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
CGuiScreenPause::CGuiScreenPause
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent,
    eGuiWindowID id
)
:   CGuiScreen( pScreen, pParent, id ),
    m_pMenu( NULL ),
    m_MissionObjectiveBox( NULL ),
    m_pressStart( NULL ),
    m_missionObjective( NULL ),
    m_objectiveIcon( NULL ),
    m_numVisibleCards( NUM_CARDS_PER_LEVEL ),
    m_elapsedTime( 0 )
{
    memset( m_collectedCards, 0, sizeof( m_collectedCards ) );
/*
    // Retrieve the Scrooby drawing elements (from Pause Cards page).
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "PauseCards" );
	rAssert( pPage );

    Scrooby::Group* collectableCards = pPage->GetGroup( "CollectedCards" );
    rAssert( collectableCards != NULL );

    char cardName[ 32 ];
    for( unsigned int i = 0; i < sizeof( m_collectedCards ) /
                                 sizeof( m_collectedCards[ 0 ] ); i++ )
    {
        sprintf( cardName, "Card%d", i );
        m_collectedCards[ i ] = collectableCards->GetSprite( cardName );
        rAssert( m_collectedCards[ i ] );
    }
*/
    // Retrieve the Scrooby drawing elements (from Coins page).
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Coins" );
    rAssert( pPage != NULL );
    CGuiScreenHud::SetNumCoinsDisplay( pPage->GetSprite( "NumCoins" ) );

    // Retrieve the Scrooby drawing elements (from PauseMissionObjective page).
    //
    pPage = m_pScroobyScreen->GetPage( "PauseMissionObjective" );
    rAssert( pPage != NULL );

    Scrooby::Group* pGroup = pPage->GetGroup( "MissionObjective" );
    m_MissionObjectiveBox = pGroup;
    rAssert( pGroup != NULL );

    m_missionObjective = pGroup->GetText( "MissionObjective" );
    rAssert( m_missionObjective != NULL );
    m_missionObjective->SetTextMode( Scrooby::TEXT_WRAP );
    m_missionObjective->ResetTransformation();
    m_missionObjective->Translate( 0, MESSAGE_TEXT_VERTICAL_TRANSLATION );
    m_missionObjective->ScaleAboutPoint( MESSAGE_TEXT_SCALE * MESSGAE_TEXT_HORIZONTAL_STRETCH,
                                         MESSAGE_TEXT_SCALE,
                                         1.0f, 0, 0 );

    m_objectiveIcon = pGroup->GetSprite( "ObjectiveIcon" );
    rAssert( m_objectiveIcon != NULL );
    m_objectiveIcon->SetVisible( false );
    m_objectiveIcon->ResetTransformation();
    m_objectiveIcon->ScaleAboutCenter( MISSION_ICON_SCALE );

    // apply correction scale to message box
    //
    Scrooby::Sprite* messageBox = pGroup->GetSprite( "MessageBox" );
    rAssert( messageBox != NULL );
    messageBox->ResetTransformation();
    messageBox->ScaleAboutCenter( MESSAGE_BOX_CORRECTION_SCALE * MESSAGE_BOX_HORIZONTAL_STRETCH,
                                  MESSAGE_BOX_CORRECTION_SCALE * MESSAGE_BOX_VERTICAL_STRETCH,
                                  1.0f );

    // Retrieve the Scrooby drawing elements (from Pause Foreground page).
    //
    pPage = m_pScroobyScreen->GetPage( "PauseFgd" );
    if( pPage != NULL )
    {
#ifdef RAD_WIN32
        pPage->SetVisible( false );
#endif

        // Wrap "Press Start" help text
        //
        m_pressStart = pPage->GetText( "PressStartResumePlay" );
        if( m_pressStart != NULL )
        {
            m_pressStart->SetTextMode( Scrooby::TEXT_WRAP );

            // add text outline
            //
            m_pressStart->SetDisplayOutline( true );

            // set platform-specific text
            //
            m_pressStart->SetIndex( PLATFORM_TEXT_INDEX );
        }
    }

    this->AutoScaleFrame( m_pScroobyScreen->GetPage( "XSmallBoard" ) );

    this->SetZoomingEnabled( true );

    #ifdef DEBUGWATCH
        const char* screenName = GetWatcherName();
        m_MissionObjectiveBox->WatchAll( screenName );
        m_objectiveIcon->      WatchAll( screenName );
        m_missionObjective->   WatchAll( screenName );
        m_pressStart->         WatchAll( screenName );
    #endif
}


//===========================================================================
// CGuiScreenPause::~CGuiScreenPause
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
CGuiScreenPause::~CGuiScreenPause()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenPause::HandleMessage
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
void CGuiScreenPause::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
#ifdef RAD_WIN32
    if( message == GUI_MSG_MENU_PROMPT_RESPONSE && 
        param1 == PROMPT_CONFIRM_QUIT_TO_SYSTEM )
    {
        switch( param2 )
        {
        case (CGuiMenuPrompt::RESPONSE_YES):
            {
                // Tell the current screen to shut down.
                //
                m_pParent->HandleMessage( GUI_MSG_QUIT_TO_SYSTEM );

                break;
            }

        case (CGuiMenuPrompt::RESPONSE_NO):
            {
                this->ReloadScreen();

                break;
            }

        default:
            {
                rAssertMsg( 0, "WARNING: *** Invalid prompt response!\n" );

                break;
            }
        }
    }
#endif

    if( message == GUI_MSG_MENU_PROMPT_RESPONSE &&
        param1 == PROMPT_CONFIRM_QUIT )
    {
        switch( param2 )
        {
            case (CGuiMenuPrompt::RESPONSE_YES):
            {
                m_pParent->HandleMessage( GUI_MSG_QUIT_INGAME );

                break;
            }

            case (CGuiMenuPrompt::RESPONSE_NO):
            {
                this->ReloadScreen();

                break;
            }

            default:
            {
                rAssertMsg( 0, "WARNING: *** Invalid prompt response!\n" );

                break;
            }
        }
    }
    
    if ( message==GUI_MSG_PROMPT_START_RESPONSE )
    {
        m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );
    }


    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {

            case GUI_MSG_UPDATE:
            {
                GetMemoryCardManager()->Update( param1 );
                HudMissionObjective* hudMissionObjective = static_cast<HudMissionObjective*>( GetCurrentHud()->GetEventHandler( CGuiScreenHud::HUD_EVENT_HANDLER_MISSION_OBJECTIVE ) );
                hudMissionObjective->UpdateIcon();
                break;
            }

            case GUI_MSG_CONTROLLER_START:
            {
                if( !m_pMenu->HasSelectionBeenMade() )
                {
                    this->HandleResumeGame();
                }

                break;
            }

#ifdef RAD_WIN32
            case GUI_MSG_CONTROLLER_BACK:
            {
                // This is our start button for PC.
                this->HandleResumeGame();
                break;
            }
#endif

            default:
            {
                break;
            }
        }

        // relay message to menu
        //
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }
    else if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        if( m_numTransitionsPending <= 0 )
        {
            // restore press start text
            //
            if( m_pressStart != NULL )
            {
                m_pressStart->ResetTransformation();
                m_pressStart->SetHorizontalJustification( Scrooby::Left );
            }

            rAssert( m_objectiveIcon != NULL );
            m_objectiveIcon->SetRawSprite( NULL );
            m_objectiveIcon->SetVisible( false );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenPause::InitIntro
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
void CGuiScreenPause::InitIntro()
{
/*
    // update collect cards display for current level
    //
    unsigned int currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
    const CardList* collectedCards = GetCardGallery()->GetCollectedCards( currentLevel );
    rAssert( collectedCards );

    for( unsigned int i = 0; i < sizeof( m_collectedCards ) /
                                 sizeof( m_collectedCards[ 0 ] ); i++ )
    {
        rAssert( m_collectedCards[ i ] );

        if( collectedCards->m_cards[ i ] != NULL )
        {
            unsigned int cardID = collectedCards->m_cards[ i ]->GetID();
            rAssert( cardID < static_cast<unsigned int>( m_collectedCards[ i ]->GetNumOfImages() ) );
            m_collectedCards[ i ]->SetIndex( cardID + 1 );
        }
        else
        {
            m_collectedCards[ i ]->SetIndex( 0 );
        }
    }

    this->HideCards();
*/
    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
    if( currentMission != NULL )
    {
        MissionStage* currentStage = currentMission->GetCurrentStage();
        if( currentStage != NULL )
        {
            // update mission objective text
            //
            rAssert( m_missionObjective != NULL );

            int currentMissionObjective = currentStage->GetStartMessageIndex();
            if( currentMissionObjective >= 0 ) // && currentStage->GetMissionLocked() )
            {
                rAssert( currentMissionObjective < m_missionObjective->GetNumOfStrings() );
                m_missionObjective->SetIndex( currentMissionObjective );
                m_missionObjective->SetVisible( true );
                m_MissionObjectiveBox->SetVisible( true );
            }
            else
            {
                rWarningMsg( false, "Invalid mission objective message index!" );
                m_missionObjective->SetVisible( false );
                m_MissionObjectiveBox->SetVisible( false );
            }

            // update mission objective icon
            //
            tSprite* pSprite = NULL;
            const char* iconName = currentStage->GetHUDIcon();
            if( iconName[ 0 ] != '\0' )
            {
                pSprite = p3d::find<tSprite>( iconName );
                rTuneWarningMsg( pSprite != NULL, "Can't find mission objective icon!" );
            }

            rAssert( m_objectiveIcon != NULL );
            m_objectiveIcon->SetVisible( pSprite != NULL );
            m_objectiveIcon->SetRawSprite( pSprite, true );
        }
        else
        {
            m_MissionObjectiveBox->SetVisible( false );
        }
    }
    else
    {
        m_MissionObjectiveBox->SetVisible( false );
    }

    // move press start text to bottom center of pause menu box
    //
    if( m_pressStart != NULL )
    {
        int centerX, centerY;
        m_pressStart->GetBoundingBoxCenter( centerX, centerY );

        int screenCenterX = static_cast<int>( Scrooby::App::GetInstance()->GetScreenWidth() / 2 );

        m_pressStart->ResetTransformation();
        m_pressStart->Translate( screenCenterX - centerX, 85 );
        m_pressStart->SetHorizontalJustification( Scrooby::Centre );
    }

    // reset pause menu to default selection
    if( m_pMenu != NULL )
    {
        m_pMenu->Reset();
    }
}


//===========================================================================
// CGuiScreenPause::InitRunning
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
void CGuiScreenPause::InitRunning()
{
    // show coins
    //
    CGuiScreenHud::UpdateNumCoinsDisplay( GetCoinManager()->GetBankValue() );

    // disable screen zooming
    //
    this->SetZoomingEnabled( false );
}


//===========================================================================
// CGuiScreenPause::InitOutro
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
void CGuiScreenPause::InitOutro()
{
    // hide coins
    //
    CGuiScreenHud::UpdateNumCoinsDisplay( 0, false );
}


//===========================================================================
// CGuiScreenPause::HandleResumeGame
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
void CGuiScreenPause::HandleResumeGame( unsigned int param1,
                                        unsigned int param2 )
{
    // enable zoom-out when returning to game
    //
    this->SetZoomingEnabled( true );

    // resume game
    m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME, param1, param2 );
}


//===========================================================================
// CGuiScreenPause::HandleQuitGame
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
void CGuiScreenPause::HandleQuitGame()
{
    rAssert( m_guiManager );
    m_guiManager->DisplayPrompt( PROMPT_CONFIRM_QUIT, this );
}

#ifdef RAD_WIN32
//===========================================================================
// CGuiScreenPause::HandleQuitToSystem
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
void CGuiScreenPause::HandleQuitToSystem()
{
    rAssert( m_guiManager );
    m_guiManager->DisplayPrompt( PROMPT_CONFIRM_QUIT_TO_SYSTEM, this );
}
#endif

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

/*
void
CGuiScreenPause::HideCards()
{
    // hide all cards
    //
    for( unsigned int i = 0; i < NUM_CARDS_PER_LEVEL; i++ )
    {
        rAssert( m_collectedCards[ i ] );
        m_collectedCards[ i ]->SetVisible( false );
    }

    // set num visible cards to zero
    //
    m_numVisibleCards = 0;
}

void
CGuiScreenPause::ShowNextCard()
{
    // randomize next card to show
    //
    int nextCard = 0;

    int numHiddenCards = NUM_CARDS_PER_LEVEL - m_numVisibleCards;
    if( numHiddenCards > 1 )
    {
        nextCard = rand() % numHiddenCards;
    }

    // show next card
    //
    for( unsigned int i = 0; i < NUM_CARDS_PER_LEVEL; i++ )
    {
        rAssert( m_collectedCards[ i ] );

        if( m_collectedCards[ i ]->IsVisible() )
        {
            continue;
        }
        else
        {
            if( nextCard == 0 )
            {
                m_collectedCards[ i ]->SetVisible( true );
                break;
            }
            else
            {
                nextCard--;
            }
        }
    }

    m_numVisibleCards++;
}
*/

#ifdef DEBUGWATCH
const char* CGuiScreenPause::GetWatcherName() const
{
    return "GuiScreenPause";
}
#endif
