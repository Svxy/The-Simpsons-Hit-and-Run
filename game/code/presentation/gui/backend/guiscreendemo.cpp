//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenDemo
//
// Description: Implementation of the CGuiScreenDemo class.
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
#include <presentation/gui/backend/guiscreendemo.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guisystem.h>

#include <contexts/demo/democontext.h>

#include <raddebug.hpp> // Foundation
#include <layer.h>
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
// CGuiScreenDemo::CGuiScreenDemo
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
CGuiScreenDemo::CGuiScreenDemo
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_DEMO ),
    m_demoText( NULL ),
    m_elapsedTime( 0 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "Demo" );
	rAssert( pPage );

    Scrooby::Layer* foreground = pPage->GetLayer( "Foreground" );
    rAssert( foreground != NULL );

    m_demoText = foreground->GetText( "Demo" );
    rAssert( m_demoText );
}


//===========================================================================
// CGuiScreenDemo::~CGuiScreenDemo
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
CGuiScreenDemo::~CGuiScreenDemo()
{
}


//===========================================================================
// CGuiScreenDemo::HandleMessage
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
void CGuiScreenDemo::HandleMessage
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
                m_elapsedTime += param1;

                const unsigned int BLINK_PERIOD = 250;
                bool blinked = GuiSFX::Blink( m_demoText,
                                              (float)m_elapsedTime,
                                              (float)BLINK_PERIOD );
                if( blinked )
                {
                    m_elapsedTime %= BLINK_PERIOD;
                }

                break;
            }

            case GUI_MSG_CONTROLLER_CONNECT:
            case GUI_MSG_CONTROLLER_DISCONNECT:
            case GUI_MSG_CONTROLLER_START:
            case GUI_MSG_CONTROLLER_SELECT:
            {
                GetDemoContext()->EndDemo();
//                m_pParent->HandleMessage( GUI_MSG_QUIT_DEMO );

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
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenDemo::InitIntro
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
void CGuiScreenDemo::InitIntro()
{
    rAssert( m_demoText );
    m_demoText->SetVisible( true );

    m_elapsedTime = 0;
}

//===========================================================================
// CGuiScreenDemo::InitRunning
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
void CGuiScreenDemo::InitRunning()
{
}


//===========================================================================
// CGuiScreenDemo::InitOutro
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
void CGuiScreenDemo::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

