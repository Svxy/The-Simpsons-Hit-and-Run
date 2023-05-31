//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerMiniGame
//
// Description: Implementation of the CGuiManagerMiniGame class.
//
// Authors:     Tony Chu
//
// Revisions        Date            Author      Revision
//                  2003/03/24      TChu        Created
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/minigame/guimanagerminigame.h>
#include <presentation/gui/guiwindow.h> // for window IDs

#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/minigame/guiscreenminimenu.h>
#include <presentation/gui/minigame/guiscreenminihud.h>
#include <presentation/gui/minigame/guiscreenminipause.h>
#include <presentation/gui/minigame/guiscreenminisummary.h>
#include <presentation/gui/guiscreenmessage.h>

#include <gameflow/gameflow.h>
#include <memory/srrmemory.h>
#include <loading/loadingmanager.h>

#include <p3d/utility.hpp>
#include <p3d/inventory.hpp>
#include <raddebug.hpp> // Foundation

#include <input/inputmanager.h>

#include <main/platform.h>
#include <main/game.h>
#include <contexts/supersprint/supersprintcontext.h>
#include <supersprint/supersprintmanager.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* MINIGAME_CHARACTERS_INVENTORY = "FE_MiniGameCharacters";

const char* CHARACTER_FILES[] =
{
    "art\\chars\\homer_m.p3d",
    "art\\chars\\bart_m.p3d",
    "art\\chars\\lisa_m.p3d",
    "art\\chars\\marge_m.p3d",
    "art\\chars\\apu_m.p3d",

    ""
};

const int NUM_CHARACTER_FILES = sizeof( CHARACTER_FILES ) / sizeof( CHARACTER_FILES[ 0 ] ) - 1;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiManagerMiniGame::CGuiManagerMiniGame
//===========================================================================
// Description: Constructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiManagerMiniGame::CGuiManagerMiniGame( Scrooby::Project* pProject,
                                          CGuiEntity* pParent )
:   CGuiManager( pProject, pParent ),
    m_isQuittingToSupersprint( false ),
    m_controllerPromptShown( false ),
    m_isControllerReconnected( false ),
    m_lastControllerDisconnectedId( -1 )
{
    p3d::inventory->AddSection( MINIGAME_CHARACTERS_INVENTORY );
}

//===========================================================================
// CGuiManagerMiniGame::~CGuiManagerMiniGame
//===========================================================================
// Description: Destructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiManagerMiniGame::~CGuiManagerMiniGame()
{
    p3d::inventory->DeleteSection( MINIGAME_CHARACTERS_INVENTORY );
}

//===========================================================================
// CGuiManagerMiniGame::Populate
//===========================================================================
// Description:
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:
//
//===========================================================================
void CGuiManagerMiniGame::Populate()
{
MEMTRACK_PUSH_GROUP( "CGuiManagerMiniGame" );
    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    Scrooby::Screen* pScroobyScreen = NULL;

    pScroobyScreen = m_pScroobyProject->GetScreen( "Prompt" );
    if( pScroobyScreen != NULL )
    {
        CGuiScreen* pScreen = new CGuiScreenPrompt( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MiniMenu" );
    if( pScroobyScreen != NULL )
    {
        CGuiScreen* pScreen = new CGuiScreenMiniMenu( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MINI_MENU, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MiniHud" );
    if( pScroobyScreen != NULL )
    {
        CGuiScreen* pScreen = new CGuiScreenMiniHud( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MINI_HUD, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MiniPause" );
    if( pScroobyScreen != NULL )
    {
        CGuiScreen* pScreen = new CGuiScreenMiniPause( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MINI_PAUSE, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MiniSummary" );
    if( pScroobyScreen != NULL )
    {
        CGuiScreen* pScreen = new CGuiScreenMiniSummary( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MINI_SUMMARY, pScreen );
    }

    HeapMgr()->PopHeap( GMA_LEVEL_HUD );
MEMTRACK_POP_GROUP("CGuiManagerMiniGame");
}

void
CGuiManagerMiniGame::Start( CGuiWindow::eGuiWindowID initialWindow )
{
    rAssert( GUI_FE_UNINITIALIZED == m_state );

    m_nextScreen = initialWindow != CGuiWindow::GUI_WINDOW_ID_UNDEFINED ?
                   initialWindow :
                   CGuiWindow::GUI_SCREEN_ID_MINI_MENU;

    m_state = GUI_FE_CHANGING_SCREENS; // must be set before calling GotoScreen()

    CGuiScreen* nextScreen = static_cast<CGuiScreen*>( this->FindWindowByID( m_nextScreen ) );
    rAssert( nextScreen != NULL );
    m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
}

//===========================================================================
// CGuiManagerMiniGame::HandleMessage
//===========================================================================
// Description:
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:
//
//===========================================================================
void CGuiManagerMiniGame::HandleMessage( eGuiMessage message,
                                         unsigned int param1,
                                         unsigned int param2 )
{
    switch( message )
    {
        case GUI_MSG_WINDOW_FINISHED:
        {
            if( GUI_FE_CHANGING_SCREENS == m_state )
            {
                m_currentScreen = m_nextScreen;

                CGuiScreen* nextScreen = static_cast<CGuiScreen*>( this->FindWindowByID( m_nextScreen ) );
                rAssert( nextScreen != NULL );
                m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
            }
            else if( GUI_FE_SHUTTING_DOWN == m_state )
            {
                if( m_isQuittingToSupersprint )
                {
                    // quit and start loading supersprint mini-game
                    //
                    GetGameFlow()->SetContext( CONTEXT_LOADING_SUPERSPRINT );

                    m_state = GUI_FE_UNINITIALIZED;
                }
                else
                {
                    // quit mini-game mode and return to front-end
                    //
                    GetGameFlow()->SetContext( CONTEXT_FRONTEND );

                    m_state = GUI_FE_TERMINATED;
                }
            }

            break;
        }
        case GUI_MSG_QUIT_MINIGAME:
        {
            rAssert( GUI_FE_SCREEN_RUNNING == m_state );

            m_state = GUI_FE_SHUTTING_DOWN;

            m_isQuittingToSupersprint = (param1 > 0 );

            // Tell the current screen to shut down.
            //
            this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );

            break;
        }

        case GUI_MSG_CONTROLLER_DISCONNECT:
#ifndef RAD_GAMECUBE
            this->OnControllerDisconnected( static_cast<int>( param1 ) );
#endif // !RAD_GAMECUBE
        break;
        
        case GUI_MSG_CONTROLLER_CONNECT:
#ifndef RAD_GAMECUBE
            m_oldControllerState = InputManager::GetInstance()->GetGameState();
            if( m_oldControllerState == Input::ACTIVE_ANIM_CAM )
            {
                // deactivate anim cam state first, since the input manager
                // won't let us set the game state to anything else prior
                // to that
                //
                GetInputManager()->SetGameState( Input::DEACTIVE_ANIM_CAM );
            }
            GetInputManager()->SetGameState( Input::ACTIVE_FRONTEND );
#endif // !RAD_GAMECUBE
        break;
        default:
        {
            if( message == GUI_MSG_UPDATE && m_isControllerReconnected )
            {
                m_isControllerReconnected = false;
                m_controllerPromptShown = false;
            }

            if (m_controllerPromptShown) // don't pass event if controller error
            { 
                if (message==GUI_MSG_CONTROLLER_START)  // start trigger reconnection
                {
                    this->OnControllerConnected( static_cast<int>( param1 ) );
                }

                break;
            }

            if( m_state != GUI_FE_UNINITIALIZED &&
                m_currentScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
            {
                // Send the messages down to the current screen.
                //
                CGuiWindow* pScreen = this->FindWindowByID( m_currentScreen );
                rAssert( pScreen != NULL );
                pScreen->HandleMessage( message, param1, param2 );
            }
#ifndef RAD_GAMECUBE
            if ( message == GUI_MSG_UPDATE)
                PollControllers();
#endif
            break;
        }
    }

    // propogate message up the hierarchy
    //
    CGuiManager::HandleMessage( message, param1, param2 );
}

void
CGuiManagerMiniGame::OnControllerDisconnected( int controllerID, bool force_display )
{
    if (force_display) // update message
    {
        m_lastControllerDisconnectedId = controllerID;
        char str_buffer[256];
        CGuiScreenMessage::GetControllerDisconnectedMessage(controllerID, str_buffer,  255);
        GetGame()->GetPlatform()->OnControllerError(str_buffer);
        m_controllerPromptShown = true;
    }
    else if (m_controllerPromptShown) // check if this disconnection id is for the player already registered
    {
        for (int i = 0; i < SuperSprintData::NUM_PLAYERS; i++) 
        {
            int player_controllerID = GetInputManager()->GetControllerIDforPlayer( i );
            if (controllerID == player_controllerID)
            {
                m_lastControllerDisconnectedId = controllerID;
                char str_buffer[256];
                CGuiScreenMessage::GetControllerDisconnectedMessage(controllerID, str_buffer,  255);
                GetGame()->GetPlatform()->OnControllerError(str_buffer);
                m_controllerPromptShown = true;
            }
        }
    }

}
bool
CGuiManagerMiniGame::PollControllers()
{
    bool all_ok = true;
    bool has_registered = false;

    if ( GetGameFlow()->GetCurrentContext()==CONTEXT_SUPERSPRINT 
         && GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->IsSuspended() ) // minigame pause menu
    {
        int controllerID = CGuiScreenMiniHud::s_pausedControllerID;
        if (controllerID >= 0)
        {
            has_registered = true;
            if( !GetInputManager()->GetController( controllerID )->IsConnected() )
            {
                OnControllerDisconnected(controllerID, true);
                all_ok = false;
            }
        }
    }
    else  
    { // minigame game, or minigame menu
        for (int i = 0; i < SuperSprintData::NUM_PLAYERS; i++) 
        {
            int controllerID = GetInputManager()->GetControllerIDforPlayer( i );
            if (controllerID >= 0)
            {
                has_registered = true;
                if( !GetInputManager()->GetController( controllerID )->IsConnected() )
                {
                    OnControllerDisconnected(controllerID, true);
                    all_ok = false;
                    break;
                }
            }
        }
    }
    if (all_ok) {
        if (has_registered == false) // minigame menu but no one registered yet
        {
            int controllerID = GetGuiSystem()->GetPrimaryController( );
            if (controllerID >= 0)
            {
                if( !GetInputManager()->GetController( controllerID )->IsConnected() )
                {
                    OnControllerDisconnected(controllerID, true);
                    all_ok = false;
                }
            }
        }
    }

    if (all_ok) 
    {
        m_isControllerReconnected = true;
//        m_controllerPromptShown = false;
    }
    return all_ok;

}

void
CGuiManagerMiniGame::OnControllerConnected( int controllerID )
{
    if (controllerID==m_lastControllerDisconnectedId)
    {    // poll
        bool all_ok = PollControllers();
        if (all_ok)
        {
            GetGame()->GetPlatform()->ClearControllerError();
            GetSSM()->RestoreControllerState();

            if ( GetGameFlow()->GetCurrentContext()==CONTEXT_SUPERSPRINT )
            {
#ifdef RAD_XBOX
                CGuiScreenMiniHud::s_pausedControllerID  = controllerID;

                if( GetSSM()->GetState() != SuperSprintManager::IDLE )
                {
                    this->HandleMessage( GUI_MSG_GOTO_SCREEN, CGuiWindow::GUI_SCREEN_ID_MINI_PAUSE );
                }

                GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->Suspend();

                if (GetSSM()->GetState()==SuperSprintManager::COUNT_DOWN)
                {
                    InputManager::GetInstance()->SetGameState( Input::DEACTIVE_ANIM_CAM );
                    InputManager::GetInstance()->SetGameState( Input::ACTIVE_FRONTEND );
                }
                else
                {
                    InputManager::GetInstance()->SetGameState( Input::DEACTIVE_ANIM_CAM );
                    InputManager::GetInstance()->SetGameState( Input::ACTIVE_ALL );
                }
#endif
            }

            m_isControllerReconnected = true;
//            m_controllerPromptShown = false;
            m_lastControllerDisconnectedId = -1;
        }
    }
}

//===========================================================================
// CGuiManagerMiniGame::LoadCharacters
//===========================================================================
// Description:
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:
//
//===========================================================================
void
CGuiManagerMiniGame::LoadCharacters()
{
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                     "art\\chars\\global.p3d",
                                     GMA_LEVEL_HUD,
                                     MINIGAME_CHARACTERS_INVENTORY,
                                     MINIGAME_CHARACTERS_INVENTORY );

    for( int i = 0; i < NUM_CHARACTER_FILES; i++ )
    {
        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                         CHARACTER_FILES[ i ],
                                         GMA_LEVEL_HUD,
                                         MINIGAME_CHARACTERS_INVENTORY,
                                         MINIGAME_CHARACTERS_INVENTORY );
    }
}

//===========================================================================
// CGuiManagerMiniGame::UnloadCharacters
//===========================================================================
// Description:
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:
//
//===========================================================================
void
CGuiManagerMiniGame::UnloadCharacters()
{
    p3d::pddi->DrawSync();
    p3d::inventory->RemoveSectionElements( MINIGAME_CHARACTERS_INVENTORY );
}

//===========================================================================
// Private Member Functions
//===========================================================================

