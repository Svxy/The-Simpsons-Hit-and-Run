//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenScrapBook
//
// Description: Implementation of the CGuiScreenScrapBook class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/10      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenscrapbook.h>
#include <presentation/gui/guimenu.h>

#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>

// Scrooby
//
#include <screen.h>
#include <page.h>
#include <group.h>
#include <text.h>

// ATG
//
#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenScrapBook::CGuiScreenScrapBook
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
CGuiScreenScrapBook::CGuiScreenScrapBook
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SCRAP_BOOK ),
    m_pMenu( NULL ),
    m_krustySticker( NULL )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "ScrapBook" );
	rAssert( pPage != NULL );

    // get krusty sticker overlay
    //
    m_krustySticker = pPage->GetGroup( "KrustySticker" );
    rAssert( m_krustySticker != NULL );
    m_krustySticker->SetVisible( false ); // hide by default

#ifdef PAL
    m_krustySticker->ScaleAboutPoint( 0.9f, 0, 0 );
#endif

    // create menu
    //
    m_pMenu = new CGuiMenu( this, NUM_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // add menu items
    //
    Scrooby::Group* pGroup = pPage->GetGroup( "Menu" );
    rAssert( pGroup != NULL );
    m_pMenu->AddMenuItem( pGroup->GetText( "OpenBook" ) );
    Scrooby::Text* gameStats = pGroup->GetText( "GameStats" );
    m_pMenu->AddMenuItem( gameStats );

#ifdef PAL
    rAssert( gameStats != NULL );
    gameStats->SetTextMode( Scrooby::TEXT_WRAP );
#endif
}


//===========================================================================
// CGuiScreenScrapBook::~CGuiScreenScrapBook
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
CGuiScreenScrapBook::~CGuiScreenScrapBook()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenScrapBook::HandleMessage
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
void CGuiScreenScrapBook::HandleMessage
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
                this->OnMenuSelectionMade( static_cast<eMenuItem>( param1 ) );

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 350, 380 );

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
// CGuiScreenScrapBook::InitIntro
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
void CGuiScreenScrapBook::InitIntro()
{
    // update percent game complete
    //
    float percentComplete = GetCharacterSheetManager()->QueryPercentGameCompleted();

    // show krusty sticker only if game percent complete is 100%
    //
    rAssert( m_krustySticker != NULL );
    m_krustySticker->SetVisible( percentComplete > 99.999f );
}


//===========================================================================
// CGuiScreenScrapBook::InitRunning
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
void CGuiScreenScrapBook::InitRunning()
{
}


//===========================================================================
// CGuiScreenScrapBook::InitOutro
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
void CGuiScreenScrapBook::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenScrapBook::OnMenuSelectionMade( eMenuItem selection )
{
    switch( selection )
    {
        case MENU_OPEN_BOOK:
        {
            m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS );

            break;
        }
        case MENU_GAME_STATS:
        {
            m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_SCRAP_BOOK_STATS );

            break;
        }
        default:
        {
            rAssertMsg( false, "Invalid menu selection!" );

            break;
        }
    }

    this->StartTransitionAnimation( 810, 830 );
}

