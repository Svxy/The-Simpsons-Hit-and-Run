//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniHud
//
// Description: Implementation of the CGuiScreenMiniHud class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/24      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/minigame/guiscreenminihud.h>
#include <presentation/gui/guimanager.h>

#include <contexts/supersprint/supersprintcontext.h>
#include <gameflow/gameflow.h>

#ifdef RAD_WIN32
#include <input/inputmanager.h>
#endif

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

int CGuiScreenMiniHud::s_pausedControllerID = -1;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMiniHud::CGuiScreenMiniHud
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
CGuiScreenMiniHud::CGuiScreenMiniHud
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MINI_HUD )
{
}


//===========================================================================
// CGuiScreenMiniHud::~CGuiScreenMiniHud
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
CGuiScreenMiniHud::~CGuiScreenMiniHud()
{
}


//===========================================================================
// CGuiScreenMiniHud::HandleMessage
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
void CGuiScreenMiniHud::HandleMessage
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
#ifdef RAD_WIN32
            case GUI_MSG_CONTROLLER_BACK:
            {
                if( GetInputManager()->GetValue( 0, InputManager::KeyboardEsc ) > 0.0f )
                {
                    //GOOD!  Fall through.
                }
                else
                {
                    break;
                }
            }
#endif
            case GUI_MSG_CONTROLLER_START:
            {
                // pause mini-game
                //
                GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->Suspend();

                // go to pause menu
                //
                s_pausedControllerID = static_cast<int>( param1 );
                m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_MINI_PAUSE );

                break;
            }
/*
            // TC: *** for testing only ***
            //
            case GUI_MSG_CONTROLLER_AUX_X:
            case GUI_MSG_CONTROLLER_AUX_Y:
            {
                // go to Race Summary screen
                //
                GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->Suspend();

                m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_MINI_SUMMARY );

                break;
            }
*/
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
// CGuiScreenMiniHud::InitIntro
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
void CGuiScreenMiniHud::InitIntro()
{
#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( true );
#endif
}


//===========================================================================
// CGuiScreenMiniHud::InitRunning
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
void CGuiScreenMiniHud::InitRunning()
{
    if( GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->IsSuspended() )
    {
        // resume mini-game
        //
        GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->Resume();
    }
}


//===========================================================================
// CGuiScreenMiniHud::InitOutro
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
void CGuiScreenMiniHud::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

