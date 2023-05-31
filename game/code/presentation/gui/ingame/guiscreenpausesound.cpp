//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseSound
//
// Description: Implementation of the CGuiScreenPauseSound class.
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
#include <presentation/gui/ingame/guiscreenpausesound.h>

#include <memory/srrmemory.h>

#include <raddebug.hpp> // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPauseSound::CGuiScreenPauseSound
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
CGuiScreenPauseSound::CGuiScreenPauseSound
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:	
	CGuiScreenSound( pScreen, pParent )
{
}


//===========================================================================
// CGuiScreenPauseSound::~CGuiScreenPauseSound
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
CGuiScreenPauseSound::~CGuiScreenPauseSound()
{
}


//===========================================================================
// CGuiScreenPauseSound::HandleMessage
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
void CGuiScreenPauseSound::HandleMessage
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
	CGuiScreenSound::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenPauseSound::InitIntro
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
void CGuiScreenPauseSound::InitIntro()
{
    CGuiScreenSound::InitIntro();
}


//===========================================================================
// CGuiScreenPauseSound::InitRunning
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
void CGuiScreenPauseSound::InitRunning()
{
    CGuiScreenSound::InitRunning();
}


//===========================================================================
// CGuiScreenPauseSound::InitOutro
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
void CGuiScreenPauseSound::InitOutro()
{
    CGuiScreenSound::InitOutro();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

