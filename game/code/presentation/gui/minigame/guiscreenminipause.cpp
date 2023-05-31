//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniPause
//
// Description: Implementation of the CGuiScreenMiniPause class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/24      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/minigame/guiscreenminipause.h>
#include <presentation/gui/minigame/guiscreenminihud.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guimenu.h>

#include <contexts/supersprint/supersprintcontext.h>
#include <gameflow/gameflow.h>
#include <input/inputmanager.h>
#include <sound/soundmanager.h>

#include <screen.h>
#include <page.h>
#include <group.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMiniPause::CGuiScreenMiniPause
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
CGuiScreenMiniPause::CGuiScreenMiniPause
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MINI_PAUSE ),
    m_pMenu( NULL )
{
    // Retrieve the Scrooby drawing elements (from MiniPause page).
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "MiniPause" );
	rAssert( pPage != NULL );

    // create menu
    //
    m_pMenu = new CGuiMenu( this, NUM_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    Scrooby::Group* pGroup = pPage->GetGroup( "Menu" );
    rAssert( pGroup != NULL );
    m_pMenu->AddMenuItem( pGroup->GetText( "Continue" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "Quit" ) );

    // Retrieve the Scrooby drawing elements (from PauseFgd page).
    //
    pPage = m_pScroobyScreen->GetPage( "PauseFgd" );
    if( pPage != NULL )
    {
        // Wrap "Press Start" help text
        //
        Scrooby::Text* pressStart = pPage->GetText( "PressStartResumePlay" );
        if( pressStart != NULL )
        {
            pressStart->SetTextMode( Scrooby::TEXT_WRAP );

            // add text outline
            //
            pressStart->SetDisplayOutline( true );

            // set platform-specific text
            //
            pressStart->SetIndex( PLATFORM_TEXT_INDEX );
        }
    }

    this->AutoScaleFrame( m_pScroobyScreen->GetPage( "XSmallBoard" ) );

    this->SetZoomingEnabled( true );
}


//===========================================================================
// CGuiScreenMiniPause::~CGuiScreenMiniPause
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
CGuiScreenMiniPause::~CGuiScreenMiniPause()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenMiniPause::HandleMessage
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
void CGuiScreenMiniPause::HandleMessage
(
	eGuiMessage message,
	unsigned int param1,
	unsigned int param2
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        if( this->IsControllerMessage( message ) &&
            static_cast<int>( param1 ) != CGuiScreenMiniHud::s_pausedControllerID )
        {
            // ignore controller messages if not from user who paused mini-game
            //
            return;
        }

        switch( message )
        {
            case GUI_MSG_CONTROLLER_START:
            {
                if( !m_pMenu->HasSelectionBeenMade() )
                {
                    this->ResumeGame();
                }

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
#ifdef RAD_WIN32
                    this->ResumeGame();
                    break;
#else
                // don't allow user to back out of pause menu
                //
                return;
#endif
            }
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                if( param1 == MENU_ITEM_CONTINUE )
                {
                    this->ResumeGame();
                }
                else if( param1 == MENU_ITEM_QUIT )
                {
                    this->QuitGame();
                }
                else
                {
                    rAssertMsg( false, "Invalid menu selection!" );
                }

                break;
            }
            default:
            {
                break;
            }
        }

        // relay message to menu
        //
        rAssert( m_pMenu != NULL );
        m_pMenu->HandleMessage( message, param1, param2 );
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenMiniPause::InitIntro
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
void CGuiScreenMiniPause::InitIntro()
{
    rAssert( m_pMenu != NULL );
    m_pMenu->Reset(); // default to 'Continue'

    GetSoundManager()->OnPauseStart();

#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( false );
#endif
}


//===========================================================================
// CGuiScreenMiniPause::InitRunning
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
void CGuiScreenMiniPause::InitRunning()
{
}


//===========================================================================
// CGuiScreenMiniPause::InitOutro
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
void CGuiScreenMiniPause::InitOutro()
{
    GetSoundManager()->OnPauseEnd();

#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( true );
#endif
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMiniPause::ResumeGame()
{
    m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_MINI_HUD, CLEAR_WINDOW_HISTORY );
}

void
CGuiScreenMiniPause::QuitGame()
{
    // quit mini-game and return to mini-game FE
    //
    m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                              GUI_SCREEN_ID_MINI_MENU,
                              CLEAR_WINDOW_HISTORY );

    GetGameFlow()->SetContext( CONTEXT_SUPERSPRINT_FE );
}

