/******************************************************************************
    File:		CGuiScreenPauseDisplay.cpp
    Desc:		Defines the CGuiScreenPauseDisplay class.
    Authors:	Tony Chu, Neil Haran
    Date:		August 14, 2003
    History:
*****************************************************************************/

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenpausedisplay.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float CONTROLLER_IMAGE_CORRECTION_SCALE = 2.0f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPauseDisplay::CGuiScreenPauseDisplay
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
CGuiScreenPauseDisplay::CGuiScreenPauseDisplay
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreenDisplay( pScreen, pParent )
{
}


//===========================================================================
// CGuiScreenPauseDisplay::~CGuiScreenPauseDisplay
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
CGuiScreenPauseDisplay::~CGuiScreenPauseDisplay()
{
}


//===========================================================================
// CGuiScreenPauseDisplay::HandleMessage
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
void CGuiScreenPauseDisplay::HandleMessage
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
            case GUI_MSG_CONTROLLER_START:
            {
                // resume game
                m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );

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
	CGuiScreenDisplay::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenPauseDisplay::InitIntro
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
void CGuiScreenPauseDisplay::InitIntro()
{
    CGuiScreenDisplay::InitIntro();
}


//===========================================================================
// CGuiScreenPauseDisplay::InitRunning
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
void CGuiScreenPauseDisplay::InitRunning()
{
    CGuiScreenDisplay::InitRunning();
}


//===========================================================================
// CGuiScreenPauseDisplay::InitOutro
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
void CGuiScreenPauseDisplay::InitOutro()
{
    CGuiScreenDisplay::InitOutro();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

