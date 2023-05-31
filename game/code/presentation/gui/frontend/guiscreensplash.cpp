//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSplash
//
// Description: Implementation of the CGuiScreenSplash class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/23      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreensplash.h>
#include <presentation/gui/frontend/guiscreenplaymovie.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/frontend/guimanagerfrontend.h>

#include <cheats/cheatinputsystem.h>
#include <loading/loadingmanager.h>
#include <main/commandlineoptions.h>

#include <raddebug.hpp> // Foundation
#include <page.h>
#include <screen.h>
#include <sprite.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

short CGuiScreenSplash::s_demoPlaybackToggle = 0;

const unsigned int SPLASH_SCREEN_DEMO_WAIT_TIME = 30000; // in msec

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenSplash::CGuiScreenSplash
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
CGuiScreenSplash::CGuiScreenSplash
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SPLASH ),
    m_pMenu( NULL ),
    m_pressStart( NULL ),
    m_demoLoopTime( SPLASH_SCREEN_DEMO_WAIT_TIME ),
    m_elapsedTime( 0 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Splash" );
    rAssert( pPage != NULL );
/*
    Scrooby::Sprite* splashImage = pPage->GetSprite( "GameLogo" );
    if( splashImage != NULL )
    {
        const float SPLASH_SCREEN_IMAGE_CORRECTION_SCALE = 2.0f;
        splashImage->ScaleAboutCenter( SPLASH_SCREEN_IMAGE_CORRECTION_SCALE );
    }
*/
    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this );
    rAssert( m_pMenu != NULL );

    m_pressStart = pPage->GetText( "PressStart" );
    rAssert( m_pressStart != NULL );

    m_pMenu->AddMenuItem( m_pressStart,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          SELECTION_ENABLED | TEXT_OUTLINE_ENABLED );

    // set "press start" text to "loading ...", by default
    //
    m_pressStart->SetIndex( TEXT_LOADING );
    m_pressStart->SetColour( tColour( 255, 255, 255 ) );

    if( CommandLineOptions::Get( CLO_DEMO_TEST ) ||
        GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_DEMO_TEST ) )
    {
        m_demoLoopTime = 1000; // in msec
    }
}


//===========================================================================
// CGuiScreenSplash::~CGuiScreenSplash
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
CGuiScreenSplash::~CGuiScreenSplash()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenSplash::HandleMessage
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
void CGuiScreenSplash::HandleMessage
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
            case GUI_MSG_UPDATE:
            {
                if( !m_pMenu->HasSelectionBeenMade() )
                {
                    m_elapsedTime += param1;
                    if( m_elapsedTime > m_demoLoopTime )
                    {
                        if( s_demoPlaybackToggle != 0 )
                        {
                            if ( CommandLineOptions::Get( CLO_DEMO_TEST ) ||
                                 GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_DEMO_TEST ) )
                            {
                                this->StartDemoInRuntime();
                            }
                            else
                            {
                                this->StartDemoAsMovie();
                            }
                        }
                        else
                        {
                            if ( CommandLineOptions::Get( CLO_DEMO_TEST ) ||
                                 GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_DEMO_TEST ) )
                            {
                                this->StartDemoInRuntime();
                            }
                            else
                            {
                                this->StartDemoAsMovie();
                            }
                        }

                        // toggle demo playback format (btw. movie and runtime)
                        //
                        s_demoPlaybackToggle = 1 - s_demoPlaybackToggle;

                        m_elapsedTime = 0;
                    }
                }

                if( m_pressStart != NULL && !GetLoadingManager()->IsLoading() ) // sound loading is done
                {
                    rAssert( m_pMenu != NULL );

                    m_pressStart->SetIndex( TEXT_PRESS_START_GC +  PLATFORM_TEXT_INDEX);
                    m_pressStart->SetColour( m_pMenu->GetHighlightColour() );
                    m_pMenu->GetMenuItem( 0 )->m_attributes |= SELECTABLE;

                    // set this to NULL cuz we don't need to change it anymore
                    //
                    m_pressStart = NULL;
                }

                break;
            }
            case GUI_MSG_CONTROLLER_SELECT:
            {
                // ignore controller select inputs
                //
                return;

                break;
            }
            case GUI_MSG_CONTROLLER_START:
            {

                GetGuiSystem()->SetPrimaryController((int)param1);

                if( GetLoadingManager()->IsLoading() )
                {
                    // ignore start input until all loading is done
                    //
                    return;
                }

                if( !m_pMenu->HasSelectionBeenMade() )
                {
                    m_pMenu->MakeSelection();
                }

                break;
            }

            case GUI_MSG_MENU_SELECTION_MADE:
            {
                m_pParent->HandleMessage( GUI_MSG_SPLASH_SCREEN_DONE );

                break;
            }

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

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenSplash::InitIntro
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
void CGuiScreenSplash::InitIntro()
{
}


//===========================================================================
// CGuiScreenSplash::InitRunning
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
void CGuiScreenSplash::InitRunning()
{
    m_elapsedTime = 0;
}


//===========================================================================
// CGuiScreenSplash::InitOutro
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
void CGuiScreenSplash::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenSplash::StartDemoInRuntime()
{
    // Quit FE and start the demo in runtime.
    //
    m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 0 ); // 0 = demo mode
}

void
CGuiScreenSplash::StartDemoAsMovie()
{
#ifndef RAD_WIN32
    // Play demo movie.
    //
    rAssert( m_guiManager );
    CGuiScreenPlayMovie* playMovieScreen = static_cast<CGuiScreenPlayMovie*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_PLAY_MOVIE_DEMO ) );
    rAssert( playMovieScreen );
    playMovieScreen->SetMovieToPlay( MovieNames::DEMO );

    m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                              GUI_SCREEN_ID_PLAY_MOVIE_DEMO );
#endif
}

