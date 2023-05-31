//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenViewMovies
//
// Description: Implementation of the CGuiScreenViewMovies class.
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
#include <presentation/gui/frontend/guiscreenviewmovies.h>
#include <presentation/gui/frontend/guiscreenplaymovie.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guimanager.h>

#include <cheats/cheatinputsystem.h>
#include <constants/movienames.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <raddebug.hpp> // Foundation
#include <page.h>
#include <screen.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* MOVIES_MENU_ITEMS[] = 
{
    "IntroMovie",
    "Movie1",
    "Movie2",
    "Movie3",
    "Movie4",
    "Movie5",
    "Movie6",
    "Movie7",
    "Movie8",

    "" // dummy terminator
};

const char* MOVIES_SELECTION[] = 
{
    MovieNames::INTROFMV,
    MovieNames::MOVIE1,
    MovieNames::MOVIE2,
    MovieNames::MOVIE3,
    MovieNames::MOVIE4,
    MovieNames::MOVIE5,
    MovieNames::MOVIE6,
    MovieNames::MOVIE7,
    MovieNames::MOVIE8,

    "" // dummy terminator
};

const int NUM_MOVIE_SELECTIONS = sizeof( MOVIES_SELECTION ) / sizeof( MOVIES_SELECTION[ 0 ] ) - 1;

const int MOVIE_INDEX_FOR_LEVEL[] =
{
    2, // level 1
    3, // level 2
    8, // level 3
    4, // level 4
    5, // level 5
    6, // level 6
    7, // level 7

    -1
};

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenViewMovies::CGuiScreenViewMovies
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
CGuiScreenViewMovies::CGuiScreenViewMovies
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_VIEW_MOVIES ),
    m_pMenu( NULL )
{
MEMTRACK_PUSH_GROUP( "CGuiScreenViewMovies" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "ViewMovies" );
	rAssert( pPage );

    // Create a menu.
    //
    m_pMenu = new(GMA_LEVEL_FE) CGuiMenu( this, NUM_MOVIE_SELECTIONS );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    for( int i = 0; i < NUM_MOVIE_SELECTIONS; i++ )
    {
        m_pMenu->AddMenuItem( pPage->GetText( MOVIES_MENU_ITEMS[ i ] ) );
    }

    this->AutoScaleFrame( m_pScroobyScreen->GetPage( "BigBoard" ) );
MEMTRACK_POP_GROUP("CGuiScreenViewMovies");
}


//===========================================================================
// CGuiScreenViewMovies::~CGuiScreenViewMovies
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
CGuiScreenViewMovies::~CGuiScreenViewMovies()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenViewMovies::HandleMessage
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
void CGuiScreenViewMovies::HandleMessage
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
                rAssert( m_guiManager );
                CGuiScreenPlayMovie* playMovieScreen = static_cast<CGuiScreenPlayMovie*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_PLAY_MOVIE ) );
                rAssert( playMovieScreen );

                rAssert( static_cast<int>( param1 ) < NUM_MOVIE_SELECTIONS );
                if( param1 == 0 ) // 0 = Intro FMV
                {
                    // Intro FMV does not have localized audio.
                    //
                    playMovieScreen->SetMovieToPlay( MOVIES_SELECTION[ param1 ], true, false, false );
                }
                else
                {
                    playMovieScreen->SetMovieToPlay( MOVIES_SELECTION[ param1 ] );
                }

                m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_PLAY_MOVIE );
                this->StartTransitionAnimation( 975, 1005 );

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 944, 974 );

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
// CGuiScreenViewMovies::InitIntro
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
void CGuiScreenViewMovies::InitIntro()
{
    // update movies unlocked status
    //
    rAssert( m_pMenu != NULL );
    for( int i = 0; i < RenderEnums::numLevels; i++ )
    {
        bool isUnlocked = GetCharacterSheetManager()->QueryFMVUnlocked( static_cast<RenderEnums::LevelEnum>( i ) ) ||
                          GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_MOVIES );

        m_pMenu->SetMenuItemEnabled( MOVIE_INDEX_FOR_LEVEL[ i ], isUnlocked );
    }
}


//===========================================================================
// CGuiScreenViewMovies::InitRunning
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
void CGuiScreenViewMovies::InitRunning()
{
}


//===========================================================================
// CGuiScreenViewMovies::InitOutro
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
void CGuiScreenViewMovies::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

