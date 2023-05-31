//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenBootupLoad
//
// Description: Implementation of the CGuiScreenBootupLoad class.
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
#include <presentation/gui/bootup/guiscreenbootupload.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guisystem.h>

#include <raddebug.hpp> // Foundation

#include <screen.h>
#include <page.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenBootupLoad::CGuiScreenBootupLoad
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
CGuiScreenBootupLoad::CGuiScreenBootupLoad
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_BOOTUP_LOAD ),
    m_loadingText( NULL ),
    m_elapsedIdleTime( 0 )
{
    // get loading text
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "LoadingText" );
    if( pPage != NULL )
    {
        m_loadingText = pPage->GetText( "Loading" );
        rAssert( m_loadingText != NULL );
        m_loadingText->SetVisible( false ); // hide by default
    }
}


//===========================================================================
// CGuiScreenBootupLoad::~CGuiScreenBootupLoad
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
CGuiScreenBootupLoad::~CGuiScreenBootupLoad()
{
}


//===========================================================================
// CGuiScreenBootupLoad::HandleMessage
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
void CGuiScreenBootupLoad::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        if( message == GUI_MSG_UPDATE )
        {
            m_elapsedIdleTime += param1;

            if( m_loadingText != NULL )
            {
                // blink loading text if idling here on this screen to satisfy
                // TRC/TCR requirements
                //
                const unsigned int BLINKING_PERIOD = 250;
                bool isBlinked = GuiSFX::Blink( m_loadingText,
                                                static_cast<float>( m_elapsedIdleTime ),
                                                static_cast<float>( BLINKING_PERIOD ) );
                if( isBlinked )
                {
                    m_elapsedIdleTime %= BLINKING_PERIOD;
                }
            }

            if( GetGuiSystem()->GetCurrentState() == CGuiSystem::BOOTUP_ACTIVE )
            {
                m_pParent->HandleMessage( GUI_MSG_BOOTUP_LOAD_COMPLETED );
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenBootupLoad::InitIntro
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
void CGuiScreenBootupLoad::InitIntro()
{
}


//===========================================================================
// CGuiScreenBootupLoad::InitRunning
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
void CGuiScreenBootupLoad::InitRunning()
{
    m_elapsedIdleTime = 0;
}


//===========================================================================
// CGuiScreenBootupLoad::InitOutro
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
void CGuiScreenBootupLoad::InitOutro()
{
    if( m_loadingText != NULL )
    {
        // hide loading text
        //
        m_loadingText->SetVisible( false );
    }
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

