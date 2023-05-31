//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMultiSetup
//
// Description: Implementation of the CGuiScreenMultiSetup class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/16      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenmultisetup.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guisystem.h>

#include <memory/srrmemory.h>

#include <raddebug.hpp> // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

static const char* MULTI_SETUP_MENU_ITEMS[] = 
{
    "NumFlags",
    "Base"
};

static const int NUM_MULTI_SETUP_MENU_ITEMS = sizeof( MULTI_SETUP_MENU_ITEMS ) / sizeof( MULTI_SETUP_MENU_ITEMS[ 0 ] );

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMultiSetup::CGuiScreenMultiSetup
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
CGuiScreenMultiSetup::CGuiScreenMultiSetup
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:
	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MULTIPLAYER_SETUP ),
    m_pMenu( NULL )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenMultiSetup" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "MultiSetup" );
	rAssert( pPage );

    // Create a menu.
    //
    m_pMenu = new(GMA_LEVEL_FE) CGuiMenu( this, NUM_MULTI_SETUP_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    Scrooby::Text* pText = NULL;
    Scrooby::Text* pTextValue = NULL;
    char itemValue[ 32 ];

    for( int i = 0; i < NUM_MULTI_SETUP_MENU_ITEMS; i++ )
    {
        pText = pPage->GetText( MULTI_SETUP_MENU_ITEMS[ i ] );
        rAssert( pText );

        // if text value exists
        sprintf( itemValue, "%s_Value", MULTI_SETUP_MENU_ITEMS[ i ] );
        pTextValue = pPage->GetText( itemValue );

        m_pMenu->AddMenuItem( pText, pTextValue );
    }
MEMTRACK_POP_GROUP();
}


//===========================================================================
// CGuiScreenMultiSetup::~CGuiScreenMultiSetup
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
CGuiScreenMultiSetup::~CGuiScreenMultiSetup()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenMultiSetup::HandleMessage
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
void CGuiScreenMultiSetup::HandleMessage
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
            case GUI_MSG_CONTROLLER_SELECT:	
            {
                m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_MULTIPLAYER_CHOOSE_CHARACTER );

                this->StartTransitionAnimation( 390, 420 );

                // register controller ID for player 1
                GetGuiSystem()->RegisterControllerId( PLAYER_1, param1 );

                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 290, 320 );

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
// CGuiScreenMultiSetup::InitIntro
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
void CGuiScreenMultiSetup::InitIntro()
{
}


//===========================================================================
// CGuiScreenMultiSetup::InitRunning
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
void CGuiScreenMultiSetup::InitRunning()
{
    // unregister all player controller ID's
    GetGuiSystem()->UnregisterControllerId( ALL_PLAYERS );
}


//===========================================================================
// CGuiScreenMultiSetup::InitOutro
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
void CGuiScreenMultiSetup::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

