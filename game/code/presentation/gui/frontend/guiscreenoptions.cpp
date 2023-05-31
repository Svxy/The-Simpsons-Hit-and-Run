//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenOptions
//
// Description: Implementation of the CGuiScreenOptions class.
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
#include <presentation/gui/frontend/guiscreenoptions.h>
#include <presentation/gui/frontend/guiscreenplaymovie.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guiscreenprompt.h>

#include <cheats/cheatinputsystem.h>
#include <cheats/cheats.h>
#include <memory/srrmemory.h>

#include <raddebug.hpp> // Foundation
#include <layer.h>
#include <page.h>
#include <screen.h>
#include <group.h>
#include <text.h>

#ifdef RAD_PS2
    #include <main/ps2platform.h>
#endif

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const unsigned int NUM_CHEAT_DISPLAY_LINES = 16;
const int PROGRESSIVE_MODE_TEST_TIME = 5000; // in msec

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenOptions::CGuiScreenOptions
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
CGuiScreenOptions::CGuiScreenOptions
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_OPTIONS ),
    m_pMenu( NULL ),
    m_cheatsPage( NULL ),
    m_cheatsOverlay( NULL ),
    m_startingCheatID( 0 ),
    m_elapsedProgressiveModeTestTime( -1 )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenOptions" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Options" );
    rAssert( pPage != NULL );

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this, NUM_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    Scrooby::Group* pGroup = pPage->GetGroup( "Menu" );
    rAssert( pGroup != NULL );

#ifdef RAD_WIN32
    m_pMenu->AddMenuItem( pGroup->GetText( "Display" ) );
#endif
    m_pMenu->AddMenuItem( pGroup->GetText( "Controller" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "Sound" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "ViewMovies" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "ViewCredits" ) );

#ifndef RAD_WIN32
    Scrooby::Text* display = pGroup->GetText( "Display" );
    if( display != NULL )
    {
        display->SetVisible( false );
    }
#endif

    m_pMenu->AddMenuItem( pGroup->GetText( "DisplayMode" ),
                          pGroup->GetText( "DisplayMode_Value" ),
                          NULL,
                          NULL,
                          pGroup->GetSprite( "DisplayMode_ArrowL" ),
                          pGroup->GetSprite( "DisplayMode_ArrowR" ),
                          SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );

#ifdef RAD_DEMO
    m_pMenu->SetMenuItemEnabled( MENU_ITEM_VIEW_MOVIES, false );
    m_pMenu->SetMenuItemEnabled( MENU_ITEM_VIEW_CREDITS, false );
#endif

/*
#ifdef RAD_RELEASE
    // hide 'view movies' in release build
    //
    m_pMenu->SetMenuItemEnabled( MENU_ITEM_VIEW_MOVIES, false, true );
#else
    // center menu items
    //
    pGroup->ResetTransformation();
    pGroup->Translate( 0, 30 );
#endif
*/

    // TC: no more display mode menu item on PS2, this is now done during bootup
    //     w/ specific button combo pressed and a display prompt
    //
//#ifndef RAD_PS2
    // hide display mode menu item for non-PS2 platforms
    //
    m_pMenu->SetMenuItemEnabled( MENU_ITEM_DISPLAY_MODE, false, true );

    #ifndef RAD_WIN32
    // re-center menu items
    //
    pGroup->ResetTransformation();
    pGroup->Translate( 0, -30 );
    #endif
//#endif // !RAD_PS2

    // get cheats page, if included
    //
    m_cheatsPage = m_pScroobyScreen->GetPage( "Cheats" );
    if( m_cheatsPage != NULL )
    {
        m_cheatsOverlay = m_cheatsPage->GetLayerByIndex( 0 );
    }

    // register self as cheat callback
    //
    GetCheatInputSystem()->RegisterCallback( this );
MEMTRACK_POP_GROUP( "CGUIScreenOptions" );
}


//===========================================================================
// CGuiScreenOptions::~CGuiScreenOptions
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
CGuiScreenOptions::~CGuiScreenOptions()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }

    // unregister self as cheat callback
    //
    GetCheatInputSystem()->UnregisterCallback( this );
}


//===========================================================================
// CGuiScreenOptions::HandleMessage
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
void CGuiScreenOptions::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_MOTHER_OF_ALL_CHEATS ) )
    {
        if( m_cheatsOverlay != NULL )
        {
            // show/hide cheats overlay, depending on whether or not
            // cheat input is activated on any of the controllers
            //
            bool isCheatInputActivated = false;
            int numUserInputHandlers = GetGuiSystem()->GetNumUserInputHandlers();
            for( int i = 0; i < numUserInputHandlers; i++ )
            {
                if( GetGuiSystem()->GetUserInputHandler( i ) != NULL &&
                    GetCheatInputSystem()->IsActivated( i ) )
                {
                    isCheatInputActivated = true;

                    break;
                }
            }

            m_cheatsOverlay->SetVisible( isCheatInputActivated );
        }

        if( message == GUI_MSG_CONTROLLER_RIGHT &&
            GetCheatInputSystem()->IsActivated( param1 ) )
        {
            // toggle cheat list
            //
            m_startingCheatID += NUM_CHEAT_DISPLAY_LINES;
            if( m_startingCheatID >= NUM_CHEATS )
            {
                // wrap to the beginning
                m_startingCheatID = 0;
            }

            this->UpdateCheatsDisplay( m_startingCheatID );
        }
    }

    if( this->IsControllerMessage( message ) &&
        GetCheatInputSystem()->IsActivated( param1 ) )
    {
        // ignore all controller inputs when cheat input system is activated
        return;
    }

    if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        if( param1 == PROMPT_DISPLAY_PROGRESSIVE_SCAN_PS2 )
        {
            if( param2 == CGuiMenuPrompt::RESPONSE_YES )
            {
                // begin progressive scan test
                //
                this->ProgressiveModeTestBegin();
            }
            else
            {
                // don't switch display mode and return to options screen
                //
                this->ReloadScreen();
            }
        }
        else if( param1 == PROMPT_DISPLAY_PROGRESSIVE_SCAN_CONFIRM )
        {
            if( param2 == CGuiMenuPrompt::RESPONSE_YES )
            {
#ifdef RAD_PS2
                PS2Platform::GetInstance()->SetProgressiveMode( true );
#endif
                rTunePrintf( "Current display mode confirmed: PROGRESSIVE.\n" );
            }
            else
            {
                rTunePrintf( "Current display mode confirmed: INTERLACED.\n" );
            }

            // return to options screen
            //
            this->ReloadScreen();
        }
		else
			rTuneAssert(!"not handled");
    }
    else if( message == GUI_MSG_MESSAGE_UPDATE || message == GUI_MSG_PROMPT_UPDATE ) // update from message/prompt screen
    {
        if( m_elapsedProgressiveModeTestTime != -1 )
        {
            m_elapsedProgressiveModeTestTime += static_cast<int>( param1 );

            if( m_elapsedProgressiveModeTestTime > PROGRESSIVE_MODE_TEST_TIME )
            {
                this->OnProgressiveModeTestEnd();
            }
        }
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_MENU_SELECTION_CHANGED:
            {
                this->SetButtonVisible( BUTTON_ICON_ACCEPT, param1 != MENU_ITEM_DISPLAY_MODE );

                break;
            }
            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                if( param1 == MENU_ITEM_DISPLAY_MODE )
                {
                    if( param2 == 1 ) // progressive mode
                    {
                        // display confirmation prompt
                        //
                        m_guiManager->DisplayPrompt( PROMPT_DISPLAY_PROGRESSIVE_SCAN_PS2, this );
                    }
                    else // interlaced mode
                    {
#ifdef RAD_PS2
                        PS2Platform::GetInstance()->SetProgressiveMode( false );
#endif
                    }
                }

                break;
            }
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                switch( param1 )
                {
                    case MENU_ITEM_CONTROLLER:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_CONTROLLER );

                        this->StartTransitionAnimation( 530, 560 );

                        break;
                    }
                    case MENU_ITEM_SOUND:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_SOUND );

                        this->StartTransitionAnimation( 660, 690 );

                        break;
                    }
                    case MENU_ITEM_VIEW_MOVIES:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_VIEW_MOVIES );

                        this->StartTransitionAnimation( 914, 944 );

                        break;
                    }
                    case MENU_ITEM_VIEW_CREDITS:
                    {
/*
                        rAssert( m_guiManager );
                        CGuiScreenPlayMovie* playMovieScreen = static_cast<CGuiScreenPlayMovie*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_VIEW_CREDITS ) );
                        rAssert( playMovieScreen );
                        playMovieScreen->SetMovieToPlay( MovieNames::CREDITS );
*/
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_VIEW_CREDITS );

                        this->StartTransitionAnimation( 600, 630 );

                        break;
                    }
#ifdef RAD_WIN32
                    case MENU_ITEM_DISPLAY:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_DISPLAY );

                        break;
                    }
#endif
                    default:
                    {
                        break;
                    }
                }

                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 110, 140 );

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


//===========================================================================
// CGuiScreenOptions::OnCheatEntered
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
void
CGuiScreenOptions::OnCheatEntered( eCheatID cheatID, bool isEnabled )
{
    this->UpdateCheatsDisplay( m_startingCheatID );

    if( cheatID == CHEAT_ID_MOTHER_OF_ALL_CHEATS && !isEnabled )
    {
        if( m_cheatsOverlay != NULL )
        {
            m_cheatsOverlay->SetVisible( false );
        }
    }
}


//===========================================================================
// CGuiScreenOptions::InitIntro
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
void CGuiScreenOptions::InitIntro()
{
#ifndef RAD_E3
    GetCheatInputSystem()->SetEnabled( true );
#endif

    rAssert( m_pMenu != NULL );
    if( m_pMenu->GetSelection() == MENU_ITEM_DISPLAY_MODE )
    {
        this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );
    }

#ifdef RAD_PS2
    // update current display mode
    //
    m_pMenu->SetSelectionValue( MENU_ITEM_DISPLAY_MODE,
                                PS2Platform::GetInstance()->GetProgressiveMode() ? 1 : 0 );
#endif
}


//===========================================================================
// CGuiScreenOptions::InitRunning
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
void CGuiScreenOptions::InitRunning()
{
}


//===========================================================================
// CGuiScreenOptions::InitOutro
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
void CGuiScreenOptions::InitOutro()
{
#ifndef RAD_E3
    GetCheatInputSystem()->SetEnabled( false );
#endif
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenOptions::UpdateCheatsDisplay( int cheatID )
{
    if( m_cheatsPage != NULL )
    {
        // update cheats info
        //
        CheatsDB* cheatsDB = GetCheatInputSystem()->GetCheatsDB();
        rAssert( cheatsDB != NULL );

        for( unsigned int i = 0; i < NUM_CHEAT_DISPLAY_LINES; i++ )
        {
            char name[ 32 ];
            sprintf( name, "Cheat%d", i );
            Scrooby::Text* pText = m_cheatsPage->GetText( name );
            rAssert( pText != NULL );

            HeapMgr()->PushHeap( GMA_LEVEL_FE );

            if( cheatID < NUM_CHEATS )
            {
                char cheatInfo[ 64 ];

                const Cheat* cheat = cheatsDB->GetCheat( static_cast<eCheatID>( cheatID ) );
                if( cheat != NULL )
                {
                    CheatsDB::PrintCheatInfo( cheat, cheatInfo );

                    if( GetCheatInputSystem()->IsCheatEnabled( static_cast<eCheatID>( cheatID ) ) )
                    {
                        strcat( cheatInfo, "  (!)" ); // exclamation mark denotes cheat enabled
                    }
                }
                else
                {
                    // display un-registered cheat
                    //
                    sprintf( cheatInfo,
                             "Cheat ID [%02d]: *** Not Registered! ***", cheatID );
/*
                    // ignore/skip un-registered cheat and don't display anything
                    //
                    pText->SetString( 0, " " );
                    i--;
*/
                }

                pText->SetString( 0, cheatInfo );
            }
            else
            {
                pText->SetString( 0, " " );
            }

            HeapMgr()->PopHeap( GMA_LEVEL_FE );

            cheatID++;
        }
    }
}

void
CGuiScreenOptions::ProgressiveModeTestBegin()
{
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_PROGRESSIVE_SCAN_TEST, this );
    m_elapsedProgressiveModeTestTime = 0;

    rTunePrintf( "Testing progressive scan display ... ...\n" );

#ifdef RAD_PS2
    PS2Platform::GetInstance()->SetProgressiveMode( true );
#endif
}

void
CGuiScreenOptions::OnProgressiveModeTestEnd()
{
    m_guiManager->DisplayPrompt( PROMPT_DISPLAY_PROGRESSIVE_SCAN_CONFIRM, this );
    m_elapsedProgressiveModeTestTime = -1;

    rTunePrintf( "Progressive scan test completed.\n" );

#ifdef RAD_PS2
    PS2Platform::GetInstance()->SetProgressiveMode( false );
#endif
}

