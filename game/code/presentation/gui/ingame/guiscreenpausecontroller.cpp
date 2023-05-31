//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseController
//
// Description: Implementation of the CGuiScreenPauseController class.
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
#include <presentation/gui/ingame/guiscreenpausecontroller.h>

#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/gameplaymanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>

#include <raddebug.hpp> // Foundation

#include <screen.h>
#include <layer.h>
#include <page.h>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float CONTROLLER_IMAGE_CORRECTION_SCALE = 2.0f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPauseController::CGuiScreenPauseController
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
CGuiScreenPauseController::CGuiScreenPauseController
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreenController( pScreen, pParent )
{
#ifndef RAD_WIN32
    rAssert( pScreen != NULL );
    Scrooby::Page* pPage = pScreen->GetPage( "ControllerImageS" );
    rAssert( pPage != NULL );
    Scrooby::Layer* foreground = pPage->GetLayer( "Foreground" );
    rAssert( foreground != NULL );

    Scrooby::Sprite* controllerImage = foreground->GetSprite( "Controller" );
    if( controllerImage != NULL )
    {
        controllerImage->ScaleAboutCenter( CONTROLLER_IMAGE_CORRECTION_SCALE );
    }
#endif
}


//===========================================================================
// CGuiScreenPauseController::~CGuiScreenPauseController
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
CGuiScreenPauseController::~CGuiScreenPauseController()
{
}


//===========================================================================
// CGuiScreenPauseController::HandleMessage
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
void CGuiScreenPauseController::HandleMessage
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
	CGuiScreenController::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenPauseController::InitIntro
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
void CGuiScreenPauseController::InitIntro()
{
    CGuiScreenController::InitIntro();
}


//===========================================================================
// CGuiScreenPauseController::InitRunning
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
void CGuiScreenPauseController::InitRunning()
{
    CGuiScreenController::InitRunning();
}


//===========================================================================
// CGuiScreenPauseController::InitOutro
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
void CGuiScreenPauseController::InitOutro()
{
    CGuiScreenController::InitOutro();

    // apply vibration/rumble setting right away
    //
    int controllerID = GetInputManager()->GetControllerIDforPlayer( 0 );
    bool vibrationOn = GetInputManager()->IsRumbleEnabled();

    if ( GetGameplayManager()->GetGameType() == GameplayManager::GT_NORMAL &&
         GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle() )
    {
#ifdef RAD_PS2
        if ( GetInputManager()->GetController( Input::USB0 )->IsConnected() )
        {
            GetInputManager()->SetRumbleForDevice( Input::USB0, vibrationOn );
        }
        else if ( GetInputManager()->GetController( Input::USB1 )->IsConnected() )
        {
            GetInputManager()->SetRumbleForDevice( Input::USB1, vibrationOn );
        }
        else
        {
            GetInputManager()->SetRumbleForDevice( controllerID, vibrationOn );
        }
#else
        GetInputManager()->SetRumbleForDevice( controllerID, vibrationOn );
#endif

    }
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

