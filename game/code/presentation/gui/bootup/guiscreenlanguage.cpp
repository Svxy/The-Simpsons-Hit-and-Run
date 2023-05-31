//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLanguage
//
// Description: Implementation of the CGuiScreenLanguage class.
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
#include <presentation/gui/bootup/guiscreenlanguage.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guiscreenmessage.h>

#include <raddebug.hpp> // Foundation

#include <app.h>
#include <group.h>
#include <page.h>
#include <screen.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLanguage::CGuiScreenLanguage
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
CGuiScreenLanguage::CGuiScreenLanguage
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:	
	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_LANGUAGE ),
    m_pMenu( NULL )
{
    // retrieve Scrooby drawing elements
    //
    rAssert( m_pScroobyScreen != NULL );
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Language" );
    rAssert( pPage != NULL );

    m_pMenu = new CGuiMenu( this, NUM_SRR2_LANGUAGES );
    rAssert( m_pMenu != NULL );

    Scrooby::Group* menu = pPage->GetGroup( "Menu" );
    rAssert( menu != NULL );

    for( int i = 0; i < NUM_SRR2_LANGUAGES; i++ )
    {
        char name[ 32 ];
        sprintf( name, "Language%d", i );
        m_pMenu->AddMenuItem( menu->GetText( name ) );
    }

#ifdef RAD_GAMECUBE
    m_pMenu->SetMenuItemEnabled( 3, false, true ); // no Spanish on GC PAL
#endif // RAD_GAMECUBE

    Scrooby::Page* messageBoxPage = m_pScroobyScreen->GetPage( "MessageBox" );
    if( messageBoxPage != NULL )
    {
        this->AutoScaleFrame( messageBoxPage );

        Scrooby::Sprite* messageIcon = messageBoxPage->GetSprite( "ErrorIcon" );
        if( messageIcon != NULL )
        {
            messageIcon->ResetTransformation();
            messageIcon->ScaleAboutCenter( MESSAGE_ICON_CORRECTION_SCALE );
        }
    }
}


//===========================================================================
// CGuiScreenLanguage::~CGuiScreenLanguage
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
CGuiScreenLanguage::~CGuiScreenLanguage()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenLanguage::HandleMessage
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
void CGuiScreenLanguage::HandleMessage
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
                rAssert( static_cast<int>( param1 ) < NUM_SRR2_LANGUAGES );
                CGuiTextBible::SetCurrentLanguage( SRR2_LANGUAGE[ param1 ] );

                m_pParent->HandleMessage( GUI_MSG_LANGUAGE_SELECTION_DONE );

                break;
            }
            default:
            {
                break;
            }
        }

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
// CGuiScreenLanguage::InitIntro
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
void CGuiScreenLanguage::InitIntro()
{
}


//===========================================================================
// CGuiScreenLanguage::InitRunning
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
void CGuiScreenLanguage::InitRunning()
{
}


//===========================================================================
// CGuiScreenLanguage::InitOutro
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
void CGuiScreenLanguage::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

