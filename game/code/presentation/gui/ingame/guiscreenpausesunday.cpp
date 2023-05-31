//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseSunday
//
// Description: Implementation of the CGuiScreenPauseSunday class.
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
#include <presentation/gui/ingame/guiscreenpausesunday.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guiscreenmemorycard.h>

#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <raddebug.hpp> // Foundation
#include <group.h>
#include <page.h>
#include <screen.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

enum ePauseSundayMenuItem
{
    MENU_ITEM_PAUSE_SUNDAY_CONTINUE,
    MENU_ITEM_MISSION_SELECT,
    MENU_ITEM_PAUSE_SUNDAY_LEVEL_PROGRESS,
    MENU_ITEM_PAUSE_SUNDAY_VIEW_CARDS,
    MENU_ITEM_PAUSE_SUNDAY_OPTIONS,
    MENU_ITEM_SAVE_GAME,
    MENU_ITEM_PAUSE_SUNDAY_QUIT_GAME,
#ifdef RAD_WIN32
    MENU_ITEM_PAUSE_SUNDAY_EXIT_GAME,
#endif

    NUM_PAUSE_SUNDAY_MENU_ITEMS
};

static const char* PAUSE_SUNDAY_MENU_ITEMS[] = 
{
    "Continue",
    "MissionSelect",
    "LevelProgress",
    "ViewCards",
    "Options",
    "SaveGame",
    "QuitGame"
#ifdef RAD_WIN32
    ,"ExitToSystem"
#endif
};

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPauseSunday::CGuiScreenPauseSunday
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
CGuiScreenPauseSunday::CGuiScreenPauseSunday
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:	
	CGuiScreenPause( pScreen, pParent, GUI_SCREEN_ID_PAUSE_SUNDAY )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenPauseSunday" );
    // Create a menu.
    //
    m_pMenu = new(GMA_LEVEL_HUD) CGuiMenu( this, NUM_PAUSE_SUNDAY_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "PauseSunday" );
    rAssert( pPage != NULL );

    // Add menu items
    //
    Scrooby::Group* menu = pPage->GetGroup( "Menu" );
    rAssert( menu != NULL );
    Scrooby::Text* pText = NULL;
    for( int i = 0; i < NUM_PAUSE_SUNDAY_MENU_ITEMS; i++ )
    {
        pText = menu->GetText( PAUSE_SUNDAY_MENU_ITEMS[ i ] );
        rAssert( pText );

        m_pMenu->AddMenuItem( pText );
    }

#ifdef RAD_DEMO
    // disable certain menu items for demos
    //
    m_pMenu->SetMenuItemEnabled( MENU_ITEM_SAVE_GAME, false );
#endif
MEMTRACK_POP_GROUP("CGUIScreenPauseSunday");
}


//===========================================================================
// CGuiScreenPauseSunday::~CGuiScreenPauseSunday
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
CGuiScreenPauseSunday::~CGuiScreenPauseSunday()
{
}


//===========================================================================
// CGuiScreenPauseSunday::HandleMessage
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
void CGuiScreenPauseSunday::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
/*
        if( !GetMemoryCardManager()->IsMemcardInfoLoaded() )
        {
            if( message == GUI_MSG_CONTROLLER_SELECT ||
                message == GUI_MSG_CONTROLLER_START )
            {
                // ignore all menu selection inputs and start input
                // until memcard info is loaded
                //
                return;
            }
        }
*/
        switch( message )
        {
            case GUI_MSG_MENU_SELECTION_MADE:	
            {
                switch( param1 )
                {
                    case MENU_ITEM_PAUSE_SUNDAY_CONTINUE:
                    {
                        this->HandleResumeGame();

                        break;
                    }
                    case MENU_ITEM_MISSION_SELECT:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_MISSION_SELECT );

                        break;
                    }
                    case MENU_ITEM_PAUSE_SUNDAY_LEVEL_PROGRESS:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_LEVEL_STATS );

                        break;
                    }
                    case MENU_ITEM_PAUSE_SUNDAY_VIEW_CARDS:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_VIEW_CARDS );

                        break;
                    }
                    case MENU_ITEM_PAUSE_SUNDAY_OPTIONS:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_OPTIONS );

                        break;
                    }
                    case MENU_ITEM_SAVE_GAME:
                    {
#ifdef RAD_XBOX
                        // Xbox TCR Requirement: always prompt user to select memory
                        // device before loading/saving
                        //
                        CGuiScreenLoadSave::s_forceGotoMemoryCardScreen = true;
#endif
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_SAVE_GAME );

                        break;
                    }
                    case MENU_ITEM_PAUSE_SUNDAY_QUIT_GAME:
                    {
                        this->HandleQuitGame();

                        break;
                    }
#ifdef RAD_WIN32
                    case MENU_ITEM_PAUSE_SUNDAY_EXIT_GAME:
                    {
                        this->HandleQuitToSystem();

                        break;
                    }
#endif
                    default:
                    {
                        rAssertMsg( 0, "WARNING: Invalid case for switch statement!\n" );

                        break;
                    }
                }

                break;
            }

            default:
            {
                break;
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreenPause::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenPauseSunday::InitIntro
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
void CGuiScreenPauseSunday::InitIntro()
{
    if( GetCharacterSheetManager()->QueryHighestMission().mLevel > 0 ||
        GetCharacterSheetManager()->QueryHighestMission().mMissionNumber > 0 ||
        GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_MISSIONS ) )
    {
        m_pMenu->SetMenuItemEnabled( MENU_ITEM_MISSION_SELECT, true );
    }
    else
    {
        // this means the user's just started a new game and is
        // currently on the level 1 tutorial mission; hence, cannot
        // select any other missions
        //
        m_pMenu->SetMenuItemEnabled( MENU_ITEM_MISSION_SELECT, false );
    }

    CGuiScreenPause::InitIntro();
}


//===========================================================================
// CGuiScreenPauseSunday::InitRunning
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
void CGuiScreenPauseSunday::InitRunning()
{
    CGuiScreenPause::InitRunning();
}


//===========================================================================
// CGuiScreenPauseSunday::InitOutro
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
void CGuiScreenPauseSunday::InitOutro()
{
    CGuiScreenPause::InitOutro();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------
