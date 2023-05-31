//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMultiChooseChar
//
// Description: Implementation of the CGuiScreenMultiChooseChar class.
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
#include <presentation/gui/frontend/guiscreenmultichoosechar.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guisystem.h>

#include <memory/srrmemory.h>

#include <raddebug.hpp>     // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

/*
static const char* MENU_ITEMS[] =
{
    "Character"
};

static const int NUM_MENU_ITEMS = sizeof( MENU_ITEMS ) / sizeof( MENU_ITEMS[ 0 ] );

/*
static const char* MENU_CURSOR = "Cursor";
*/

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMultiChooseChar::CGuiScreenMultiChooseChar
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
CGuiScreenMultiChooseChar::CGuiScreenMultiChooseChar
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:
    CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MULTIPLAYER_CHOOSE_CHARACTER )
{
    memset( m_pMenu, 0, sizeof( m_pMenu ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "MultiChooseCharacter" );
	rAssert( pPage );

    // Get character selection for all players
    //
    char itemName[ 32 ];
    for( int i = 0; i < MAX_PLAYERS; i++ )
    {
        sprintf( itemName, "Character%d", i );
        m_pCharacter[ i ] = pPage->GetSprite( itemName );

        rAssert( m_pCharacter[ i ] );
    }

/*
    Scrooby::Text* pText = NULL;
    char menuItemName[ 32 ];

    // Create and add menu items for all players
    //
    for( int i = 0; i < MAX_PLAYERS; i++ )
    {
        m_pMenu[ i] = new(GMA_LEVEL_FE) CGuiMenu( this, NUM_MENU_ITEMS );
        rAssert( m_pMenu[ i ] != NULL );

        for( int j = 0; j < NUM_MENU_ITEMS; j++ )
        {
            sprintf( menuItemName, "%s%d", MENU_ITEMS[ j ], i );
            pText = pPage->GetText( menuItemName );
            rAssert( pText );

            m_pMenu[ i ]->AddMenuItem( pText );
        }
    }

/*
    // Set menu cursor
    //
    Scrooby::Sprite* pSprite = pPage->GetSprite( MENU_CURSOR );
    rAssert( pSprite != NULL );
    m_pMenu->SetCursor( pSprite );
*/
}


//===========================================================================
// CGuiScreenMultiChooseChar::~CGuiScreenMultiChooseChar
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
CGuiScreenMultiChooseChar::~CGuiScreenMultiChooseChar()
{
    for( int i = 0; i < MAX_PLAYERS; i++ )
    {
        if( m_pMenu[ i ] != NULL )
        {
            delete m_pMenu[ i ];
            m_pMenu[ i ] = NULL;
        }
    }
}


//===========================================================================
// CGuiScreenMultiChooseChar::HandleMessage
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
void CGuiScreenMultiChooseChar::HandleMessage
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
                // send message to front-end manager to quit front-end and
                // start multiplayer head-to-head
                //
                m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 2 ); // 2 = two players

                this->StartTransitionAnimation( 600, 640 );

                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 420, 450 );

                break;
            }

            default:
            {
                break;
            }
        }

        // relay message to menu
        if( this->IsControllerMessage( message ) )
        {
            int controllerId = param1;
/*
            // register player 2, if not already done so and if controller ID different from player 1
            if( GetGuiSystem()->GetControllerId( PLAYER_2 ) == -1 &&
                GetGuiSystem()->GetControllerId( PLAYER_1 ) != controllerId )
            {
                GetGuiSystem()->RegisterControllerId( PLAYER_2, controllerId );
            }
*/
            // send controller messages only to corresponding player menu
            for( int i = 0; i < MAX_PLAYERS; i++ )
            {
                if( GetGuiSystem()->GetControllerId( static_cast<ePlayer>( i ) ) == controllerId )
                {
/*
                    rAssert( m_pMenu[ i ] != NULL );
                    m_pMenu[ i ]->HandleMessage( message, param1, param2 );
*/
                    this->HandleControllerMessage( i, message );

                    break;
                }
            }
        }
/*
        else
        {
            // send all other messages to all menus
            for( int i = 0; i < MAX_PLAYERS; i++ )
            {
                rAssert( m_pMenu[ i ] != NULL );
                m_pMenu[ i ]->HandleMessage( message, param1, param2 );
            }
        }
*/
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenMultiChooseChar::InitIntro
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
void CGuiScreenMultiChooseChar::InitIntro()
{
    // get character selection (for all players)
    for( int i = 0; i < MAX_PLAYERS; i++ )
    {
    }
}


//===========================================================================
// CGuiScreenMultiChooseChar::InitRunning
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
void CGuiScreenMultiChooseChar::InitRunning()
{
}


//===========================================================================
// CGuiScreenMultiChooseChar::InitOutro
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
void CGuiScreenMultiChooseChar::InitOutro()
{
    // set character selection (for all players)
    for( int i = 0; i < MAX_PLAYERS; i++ )
    {
    }
}


//===========================================================================
// CGuiScreenMultiChooseChar::HandleControllerMessage
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
void CGuiScreenMultiChooseChar::HandleControllerMessage( int player, eGuiMessage message )
{
    switch( message )
    {
        case GUI_MSG_CONTROLLER_LEFT:
        {
            // decrement character selection
            rAssert( m_pCharacter[ player ] );

            int newIndex = m_pCharacter[ player ]->GetIndex() - 1;
            if( newIndex < 0 )
            {
                newIndex = m_pCharacter[ player ]->GetNumOfImages() - 1;
            }

            m_pCharacter[ player ]->SetIndex( newIndex );

            break;
        }
        
        case GUI_MSG_CONTROLLER_RIGHT:
        {
            // increment character selection
            rAssert( m_pCharacter[ player ] );

            int newIndex = (m_pCharacter[ player ]->GetIndex() + 1) % m_pCharacter[ player ]->GetNumOfImages();

            m_pCharacter[ player ]->SetIndex( newIndex );

            break;
        }
        
        default:
        {
            break;
        }
    }
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

