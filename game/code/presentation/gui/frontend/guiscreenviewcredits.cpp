//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenViewCredits
//
// Description: Implementation of the CGuiScreenViewCredits class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/05/14      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenviewcredits.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guiuserinputhandler.h>

#include <cheats/cheatinputsystem.h>
#include <events/eventmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>

// Scrooby
#include <app.h>
#include <page.h>
#include <screen.h>
#include <group.h>
#include <text.h>
#include <sprite.h>

// ATG
#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float CREDITS_SCROLL_RATE = 20.0f; // in pixels per second
const float CREDITS_MANUAL_SCROLL_FACTOR = 5.0f;
const float CREDITS_FGD_CORRECTION_SCALE = 2.0f;

const int CREDITS_PARTITION_FOR_LOCALIZATION = 13;
const int CREDITS_PARTITION_FOR_LOGITECH = 30;

float CGuiScreenViewCredits::s_totalTranslateY = 0.0f;
float CGuiScreenViewCredits::s_numPixelsPerLine = 0.0f;
float CGuiScreenViewCredits::s_creditsOffsets[ CGuiScreenViewCredits::MAX_NUM_CREDITS_PARTITIONS ];

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenViewCredits::CGuiScreenViewCredits
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
CGuiScreenViewCredits::CGuiScreenViewCredits
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_VIEW_CREDITS ),
    m_playKKDialog( false ),
    m_creditsGroup( NULL ),
    m_currentTranslateY( 0.0f ),
    m_lastLineDisplayed( 0 ),
    m_elapsedIdleTime( 0 )
{
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "ViewCredits" );
    rAssert( pPage != NULL );

    // get credits group drawable
    //
    m_creditsGroup = pPage->GetGroup( "Credits" );
    rAssert( m_creditsGroup != NULL );

    bool isFirstTime = ( s_totalTranslateY == 0.0f );
    float currentTranslateY = 0.0f;

    // get all credits (partitioned) text
    //
    for( int i = 0; i < MAX_NUM_CREDITS_PARTITIONS; i++ )
    {
        char name[ 16 ];
        sprintf( name, "Credits%d", i );
        m_credits[ i ] = m_creditsGroup->GetText( name );

        if( m_credits[ i ] != NULL )
        {
#ifndef PAL
            // hide localization partition from credits for NTSC builds
            //
            if( i == CREDITS_PARTITION_FOR_LOCALIZATION )
            {
                m_credits[ i ]->SetVisible( false );

                continue;
            }
#endif // !PAL

#ifndef RAD_GAMECUBE
    #ifndef RAD_PS2
            // hide logitech line from credits for non-GC and non-PS2 platforms
            //
            if( i == CREDITS_PARTITION_FOR_LOGITECH )
            {
                m_credits[ i ]->SetVisible( false );

                continue;
            }
    #endif // !RAD_PS2
#endif // !RAD_GAMECUBE

            if( isFirstTime )
            {
                currentTranslateY = this->FormatTextWithLineBreaks( m_credits[ i ] );

                s_creditsOffsets[ i ] = -s_totalTranslateY;
                s_totalTranslateY += currentTranslateY;
            }

            // offset current credits partition by total translation so far
            //
            m_credits[ i ]->ResetTransformation();
            m_credits[ i ]->Translate( 0, (int)s_creditsOffsets[ i ] );

            // disable any text outlining or drop shadow effects
            //
            m_credits[ i ]->SetDisplayOutline( false );
            m_credits[ i ]->SetDisplayShadow( false );
        }
        else
        {
            // assuming there's no more partitions to follow
            //
            break;
        }
    }

    // increment total translation for the screen area
    //
    if( isFirstTime )
    {
        s_totalTranslateY += Scrooby::App::GetInstance()->GetScreenHeight();
    }

/*
    // get skip label
    //
    m_buttonIcons[ BUTTON_ICON_ACCEPT ] = pPage->GetGroup( "SkipLabel" );
    this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );
*/

    // apply correction scale to credits foreground overlay image (from CreditsFgd.pag)
    //
    pPage = m_pScroobyScreen->GetPage( "CreditsFgd" );
    if( pPage != NULL )
    {
        Scrooby::Sprite* creditsFgd = pPage->GetSprite( "CreditsFgd" );
        if( creditsFgd != NULL )
        {
            creditsFgd->ResetTransformation();
            creditsFgd->ScaleAboutCenter( CREDITS_FGD_CORRECTION_SCALE );
        }
    }
}


//===========================================================================
// CGuiScreenViewCredits::~CGuiScreenViewCredits
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
CGuiScreenViewCredits::~CGuiScreenViewCredits()
{
}


//===========================================================================
// CGuiScreenViewCredits::HandleMessage
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
void CGuiScreenViewCredits::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( message == GUI_MSG_WINDOW_ENTER )
    {
        this->SetFadingEnabled( !GetGuiSystem()->IsShowCreditsUponReturnToFE() );
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                float currentScrollAmount = param1 / 1000.0f * CREDITS_SCROLL_RATE;

                // auto-scrolling
                //
                bool isScrollingDone = this->ScrollCredits( currentScrollAmount );

                // check for up/down controller inputs for manual scroll control
                //
                const int NUM_USER_INPUT_HANDLERS = GetGuiSystem()->GetNumUserInputHandlers();

                for( int i = 0; i < NUM_USER_INPUT_HANDLERS; i++ )
                {
                    CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( i );
                    if( userInputHandler != NULL )
                    {
#ifdef RAD_WIN32
                        if( userInputHandler->IsYAxisOnUp() )
#else
                        if( userInputHandler->IsButtonDown( GuiInput::Up ) ||
                            userInputHandler->IsYAxisOnUp() )
#endif
                        {
                            // scroll upwards at opposite rate
                            //
                            isScrollingDone = this->ScrollCredits( -(1.0f + CREDITS_MANUAL_SCROLL_FACTOR) * currentScrollAmount );
                        }

                        if( !m_playKKDialog ) // only allow speed-up if dialog is not playing
                        {
#ifdef RAD_WIN32
                            if( userInputHandler->IsYAxisOnDown() )
#else
                            if( userInputHandler->IsButtonDown( GuiInput::Down ) ||
                                userInputHandler->IsYAxisOnDown() )
#endif
                            {
                                // double the scroll rate
                                //
                                isScrollingDone = this->ScrollCredits( (CREDITS_MANUAL_SCROLL_FACTOR - 1.0f) * currentScrollAmount );
                            }
                        }
                    }
                }

                // check if scroll idle time has expired
                //
                if( m_elapsedIdleTime > SCROLL_IDLE_TIME_BEFORE_RESET )
                {
                    this->OnScrollingDone();
                }
                else if( isScrollingDone )
                {
                    m_elapsedIdleTime += param1;
                }

                break;
            }
            case GUI_MSG_CONTROLLER_SELECT:
            {
                this->OnScrollingDone();

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                // ignore back controller inputs
                //
                return;
            }
            default:
            {
                break;
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenViewCredits::InitIntro
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
void CGuiScreenViewCredits::InitIntro()
{
    // only play K&K credits dialog if either cheat is enabled or user has completed
    // the last mission of the game
    //
    m_playKKDialog = GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_PLAY_CREDITS_DIALOG ) ||
                     GetCharacterSheetManager()->QueryMissionStatus( RenderEnums::L7, RenderEnums::M7 )->mCompleted;

    this->ResetScrolling();

    // hide back button label
    //
    this->SetButtonVisible( BUTTON_ICON_BACK, false );

    GetEventManager()->TriggerEvent( EVENT_PLAY_CREDITS );
}


//===========================================================================
// CGuiScreenViewCredits::InitRunning
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
void CGuiScreenViewCredits::InitRunning()
{
    if( GetGuiSystem()->IsShowCreditsUponReturnToFE() )
    {
        this->RestoreDefaultFadeTime();
    }
}


//===========================================================================
// CGuiScreenViewCredits::InitOutro
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
void CGuiScreenViewCredits::InitOutro()
{
    if( GetGuiSystem()->IsShowCreditsUponReturnToFE() )
    {
        GetGuiSystem()->ShowCreditsUponReturnToFE( false );
    }

    GetEventManager()->TriggerEvent( EVENT_DIALOG_SHUTUP );
    GetEventManager()->TriggerEvent( EVENT_PLAY_FE_MUSIC );
}


void
CGuiScreenViewCredits::ResetScrolling()
{
    rAssert( m_creditsGroup != NULL );
    m_creditsGroup->ResetTransformation();

    m_currentTranslateY = 0.0f;

    m_lastLineDisplayed = 0;
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

float
CGuiScreenViewCredits::FormatTextWithLineBreaks( Scrooby::Text* pText )
{
    rAssert( pText != NULL );

    float totalTranslateY = 0.0f;

    int width = 0;
    int height = 0;
    pText->GetBoundingBoxSize( width, height );

#ifdef RAD_WIN32
    const float SPACING_FUDGE_FACTOR = 1.20f;
#else
    const float SPACING_FUDGE_FACTOR = 1.15f;
#endif

    s_numPixelsPerLine = height * SPACING_FUDGE_FACTOR;

    P3D_UNICODE* textBuffer = static_cast<P3D_UNICODE*>( pText->GetStringBuffer() );
    rAssert( textBuffer != NULL );

    for( int i = 0; textBuffer[ i ] != '\0'; i++ )
    {
        if( textBuffer[ i ] == '\\' ) // replace backslashes with line breaks
        {
            textBuffer[ i ] = '\n';

            // increment total translation by number of pixels per line
            //
            totalTranslateY += s_numPixelsPerLine;
        }
    }

    return totalTranslateY;
}

bool
CGuiScreenViewCredits::ScrollCredits( float pixels )
{
    float newTranslateY = m_currentTranslateY + pixels;

    bool isScrollingDone = ( newTranslateY > s_totalTranslateY );

    if( newTranslateY >= 0 && newTranslateY <= s_totalTranslateY )
    {
        m_currentTranslateY = newTranslateY;

        rAssert( m_creditsGroup != NULL );
        m_creditsGroup->ResetTransformation();
        m_creditsGroup->Translate( 0, (int)m_currentTranslateY );

        // check if new line is displayed
        //
        const float BOTTOM_OFFSET = 80.0f; // in pixels
        if( (m_currentTranslateY - BOTTOM_OFFSET) >= (m_lastLineDisplayed * s_numPixelsPerLine) )
        {
            m_lastLineDisplayed++;

            this->OnNewLineDisplayed( m_lastLineDisplayed );
        }

        m_elapsedIdleTime = 0; // reset idle time
    }
    else
    {
        rDebugPrintf( "Credits not scrolling.\n" );
    }

    return isScrollingDone;
}

void
CGuiScreenViewCredits::OnScrollingDone()
{
    if( GetGuiSystem()->IsShowCreditsUponReturnToFE() )
    {
        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                  GUI_SCREEN_ID_MAIN_MENU,
                                  CLEAR_WINDOW_HISTORY );
    }
    else if( m_guiManager->GetPreviousScreen() != GUI_WINDOW_ID_UNDEFINED )
    {
        this->StartTransitionAnimation( 630, 660 );

        m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );
    }
}

void
CGuiScreenViewCredits::OnNewLineDisplayed( int lineNumber )
{
    rTunePrintf( "Credits Line #%d\n", lineNumber );

    if( m_playKKDialog )
    {
        // trigger this event for the sound manager to play line-specific dialog
        //
        GetEventManager()->TriggerEvent( EVENT_FE_CREDITS_NEW_LINE, (void*)lineNumber );
    }
}

