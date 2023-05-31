//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenController
//
// Description: Implementation of the CGuiScreenController class.
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
#include <presentation/gui/frontend/guiscreencontroller.h>
#include <presentation/gui/guimenu.h>

#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <raddebug.hpp>     // Foundation
#include <group.h>
#include <layer.h>
#include <page.h>
#include <screen.h>
#include <text.h>

#include <strings/unicodestring.h>
#include <p3d/unicode.hpp>
#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

enum eMenuItem
{
    MENU_ITEM_CONFIGURATION,
    MENU_ITEM_DISPLAY,
    MENU_ITEM_VIBRATION,

    NUM_MENU_ITEMS
};

static const char* MENU_ITEMS[] = 
{
    "Configuration",
    "Display",
    "Vibration"
};

const int NUM_DISPLAY_MODES = 2;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenController::CGuiScreenController
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
CGuiScreenController::CGuiScreenController
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_CONTROLLER ),
    m_pMenu( NULL ),
    m_numLabels( 0 ),
    m_currentConfig( 0 ),
    m_currentDisplay( 0 ),
    m_currentControllerID( 0 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "Controller" );
	rAssert( pPage );

    // create a menu
    //
    m_pMenu = new(GMA_LEVEL_FE) CGuiMenu( this, NUM_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // add menu items
    //
    Scrooby::Group* menu = pPage->GetGroup( "Menu" );
    for( int j = 0; j < NUM_MENU_ITEMS; j++ )
    {
        char objectName[ 32 ];
        sprintf( objectName, "%s_Value", MENU_ITEMS[ j ] );
        m_pMenu->AddMenuItem( menu->GetText( MENU_ITEMS[ j ] ),
                              menu->GetText( objectName ),
                              NULL,
                              NULL,
                              NULL,
                              NULL,
                              SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );
    }

#ifdef RAD_GAMECUBE
    // change "Vibration" text to "Rumble"
    //
    Scrooby::Text* vibrationText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_ITEM_VIBRATION )->GetItem() );
    rAssert( vibrationText );
    vibrationText->SetIndex( 1 );
#endif // RAD_GAMECUBE

    // disable configuration until there are multiple configurations from
    // which to choose
    //
    m_pMenu->SetMenuItemEnabled( MENU_ITEM_CONFIGURATION, false, true );

    // get the platform-specific Controller page
    //
#ifdef RAD_GAMECUBE
    pPage = m_pScroobyScreen->GetPage( "ControllerGC" );
    rAssert( pPage );
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
    pPage = m_pScroobyScreen->GetPage( "ControllerPS2" );
    rAssert( pPage );
#endif // RAD_PS2

#ifdef RAD_XBOX
    pPage = m_pScroobyScreen->GetPage( "ControllerXBOX" );
    rAssert( pPage );
#endif // RAD_XBOX

#ifdef RAD_WIN32
    pPage = m_pScroobyScreen->GetPage( "ControllerPC" );
    rAssert( pPage );
#endif // RAD_WIN32

    // and make it visible
    //
    pPage->GetLayerByIndex( 0 )->SetVisible( true );

    // get text labels
    //
    Scrooby::Group* textLabels = pPage->GetGroup( "TextLabels" );
    rAssert( textLabels != NULL );
    for( int i = 0; i < MAX_NUM_LABELS; i++ )
    {
        char objectName[ 32 ];
        sprintf( objectName, "Label%02d", i );

        Scrooby::Text* textLabel = textLabels->GetText( objectName );
        if( textLabel != NULL )
        {
#ifdef PAL
            textLabel->StretchBoundingBox( 30, 0 );
#endif // PAL

            textLabel->SetTextMode( Scrooby::TEXT_WRAP );
            m_labels[ i ] = textLabel;
            m_numLabels++;

            // TC: this is now done in the text bible
            //
/*
            // convert text label strings to all caps
            //
            for( int j = 0; j < NUM_DISPLAY_MODES; j++ )
            {
                rAssert( j < textLabel->GetNumOfStrings() );

                p3d::UnicodeStrUpr( textLabel->GetStringBuffer( j ) );
            }
*/
        }
        else
        {
            // assume no more labels, which means they all must have
            // been numbered sequentially without any gaps
            //
            break;
        }
    }

#ifdef PAL
    // scale down controller image and text labels a bit so that
    // all localized versions will fit on screen
    //
  #ifdef RAD_PS2
    const float SCALE_FACTOR = 0.95f;
  #else
    const float SCALE_FACTOR = 0.9f;
  #endif

    textLabels->ResetTransformation();
    textLabels->ScaleAboutCenter( SCALE_FACTOR );

    pPage = m_pScroobyScreen->GetPage( "ControllerImage" );
    if( pPage != NULL )
    {
        Scrooby::Sprite* controllerImage = pPage->GetSprite( "Controller" );
        if( controllerImage != NULL )
        {
            controllerImage->ResetTransformation();
            controllerImage->ScaleAboutCenter( SCALE_FACTOR );
        }
    }
#endif // PAL
}


//===========================================================================
// CGuiScreenController::~CGuiScreenController
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
CGuiScreenController::~CGuiScreenController()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenController::HandleMessage
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
void CGuiScreenController::HandleMessage
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
            case GUI_MSG_CONTROLLER_LEFT:
            case GUI_MSG_CONTROLLER_RIGHT:
            {
                if( m_pMenu->GetSelection() == MENU_ITEM_VIBRATION )
                {
                    m_currentControllerID = static_cast<int>( param1 );
                }

                break;
            }
            
            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                if( param1 == MENU_ITEM_CONFIGURATION )
                {
                    m_currentConfig = param2;

                    this->UpdateLabels();
                }
                else if( param1 == MENU_ITEM_DISPLAY )
                {
                    m_currentDisplay = param2;

                    this->UpdateLabels();
                }
                else if( param1 == MENU_ITEM_VIBRATION )
                {
                    if( param2 == 0 ) // vibration turned ON
                    {
                        // send vibration pulse to controller
                        //
                        GetInputManager()->TriggerRumblePulse( m_currentControllerID );
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 560, 590 );

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
// CGuiScreenController::InitIntro
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
void CGuiScreenController::InitIntro()
{
    // update controller settings on menu
    //
    bool vibrationOn = GetInputManager()->IsRumbleEnabled();
    m_pMenu->SetSelectionValue( MENU_ITEM_VIBRATION,
                                vibrationOn ? 0 : 1 );
}


//===========================================================================
// CGuiScreenController::InitRunning
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
void CGuiScreenController::InitRunning()
{
}


//===========================================================================
// CGuiScreenController::InitOutro
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
void CGuiScreenController::InitOutro()
{
    bool vibrationOn = (m_pMenu->GetSelectionValue( MENU_ITEM_VIBRATION ) == 0);
    GetInputManager()->SetRumbleEnabled( vibrationOn );
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenController::UpdateLabels()
{
    int labelIndex = m_currentConfig * NUM_DISPLAY_MODES + m_currentDisplay;
    for( int i = 0; i < m_numLabels; i++ )
    {
        rAssert( m_labels[ i ] );
        m_labels[ i ]->SetIndex( labelIndex );
    }
}

