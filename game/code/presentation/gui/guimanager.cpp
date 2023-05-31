//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManager
//
// Description: Implementation of the CGuiManager class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions        Date            Author      Revision
//                  2000/09/21      DChau       Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guisystem.h> // for loading callback
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guiscreenprompt.h>
#include <input/inputmanager.h>

#include <gameflow/gameflow.h>
#include <memory/srrmemory.h>

#include <raddebug.hpp> // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

CGuiManager::eMemoryCardCheckingState
CGuiManager::s_memcardCheckState = CGuiManager::MEM_CARD_CHECK_NOT_DONE;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiManager::CGuiManager
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
CGuiManager::CGuiManager
(
    Scrooby::Project* pProject,
    CGuiEntity* pParent
)
:   CGuiEntity( pParent ),
    m_numWindows( 0 ),
    m_pScroobyProject( pProject ),
    m_currentScreen( CGuiWindow::GUI_WINDOW_ID_UNDEFINED ),
    m_nextScreen( CGuiWindow::GUI_WINDOW_ID_UNDEFINED ),
    m_state( GUI_FE_UNINITIALIZED ),
    m_windowHistoryCount( 0 ),
    m_gotoScreenUserParam1( 0 ),
    m_gotoScreenUserParam2( 0 )
{
    for( unsigned int i = 0; i < sizeof( m_windows ) /
                                 sizeof( m_windows[ 0 ] ); i++ )
    {
        m_windows[ i ] = NULL;
    }

    for( unsigned int j = 0; j < sizeof( m_windowHistory ) /
                                 sizeof( m_windowHistory[ 0 ] ); j++ )
    {
        m_windowHistory[ j ] = CGuiWindow::GUI_WINDOW_ID_UNDEFINED;
    }
    //Load the mouse cursor.
#ifdef RAD_WIN32
    tDrawable* pMouseCursor = p3d::find<tDrawable>("mouse_cursor.png");
    GetInputManager()->GetFEMouse()->InitMouseCursor( pMouseCursor );
#endif
}


//===========================================================================
// CGuiManager::~CGuiManager
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
CGuiManager::~CGuiManager()
{
    // Remove all existing screens.
    //
    this->RemoveAllWindows();
}


//===========================================================================
// CGuiManager::HandleMessage
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
void CGuiManager::HandleMessage
(
    eGuiMessage message,
    unsigned int param1,
    unsigned int param2
)
{
    switch( message )
    {
        case GUI_MSG_SET_GOTO_SCREEN_PARAMETERS:
        {
            m_gotoScreenUserParam1 = param1;
            m_gotoScreenUserParam2 = param2;

            break;
        }

        case GUI_MSG_GOTO_SCREEN:
        {
            CGuiWindow::eGuiWindowID nextScreen = static_cast<CGuiWindow::eGuiWindowID>( param1 );

            if( this->FindWindowByID( nextScreen ) == NULL )
            {
                // ignore if screen not found
                //
                break;
            }

            if( (nextScreen == m_currentScreen) &&
                (param2 & FORCE_WINDOW_RELOAD) == 0 )
            {
                if( m_nextScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
                {
                    m_nextScreen = nextScreen;
                }

                // ignore if we are already on the requested screen,
                // unless we're trying to force a screen reload
                //
                break;
            }

            m_nextScreen = nextScreen;

            rDebugPrintf( "GUI GoTo Screen Request: (next screen ID = %d)\n", m_nextScreen );

            // push current screen onto history stack, unless requested
            // to keep existing history
            //
            if( (param2 & KEEP_WINDOW_HISTORY) == 0 )
            {
                this->PushScreenHistory( m_currentScreen );
            }

            // clear screen history, if requested
            //
            if( (param2 & CLEAR_WINDOW_HISTORY) > 0 )
            {
                this->ClearScreenHistory();
            }

            m_state = GUI_FE_CHANGING_SCREENS;

            // Tell the current screen to shut down.
            //
            if( m_currentScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
            {
                this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );
            }

            if( (param2 & FORCE_WINDOW_CHANGE_IMMEDIATE) > 0 )
            {
                m_currentScreen = m_nextScreen;

                CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
                rAssert( nextScreen != NULL );
                m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
            }

            break;
        }

        case GUI_MSG_BACK_SCREEN:
        {
            CGuiWindow::eGuiWindowID nextScreen = CGuiWindow::GUI_WINDOW_ID_UNDEFINED;

            // param1 = number of extra screens to go back
            //
            for( unsigned int numBackScreens = 0; numBackScreens < (param1 + 1); numBackScreens++ )
            {
                nextScreen = this->PopScreenHistory();
            }

            if( nextScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
            {
                m_nextScreen = nextScreen;

                m_state = GUI_FE_CHANGING_SCREENS;

                // Tell the current screen to shut down.
                //
                rAssert( m_currentScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED );
                this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );
            }

            break;
        }

        case GUI_MSG_MEMCARD_CHECK_COMPLETED:
        {
            s_memcardCheckState = MEM_CARD_CHECK_COMPLETED;

            break;
        }

        default:
        {
            break;
        }
    }
}


//===========================================================================
// CGuiManager::OnGotoScreenComplete
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
void CGuiManager::OnGotoScreenComplete()
{
    m_state = GUI_FE_SCREEN_RUNNING;

    rAssert( CGuiWindow::GUI_WINDOW_ID_UNDEFINED != m_nextScreen );

    m_currentScreen = m_nextScreen;

    this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_ENTER,
                                                            m_gotoScreenUserParam1,
                                                            m_gotoScreenUserParam2 );

    // reset goto screen user parameters
    //
    m_gotoScreenUserParam1 = 0;
    m_gotoScreenUserParam2 = 0;
}


//===========================================================================
// CGuiManager::FindWindowByID
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
CGuiWindow* CGuiManager::FindWindowByID
(
    CGuiWindow::eGuiWindowID id
)
{
/*
    for( int i = 0; i < m_windows.mSize; ++i )
    {
        if( m_windows[i]->GetWindowID() == id )
        {
            return( m_windows[i] );
        }
    }
*/
    CGuiWindow* window = NULL;

    if( this->IsValidWindowID( id ) )
    {
        window = m_windows[ id ];
    }

    return window;
}

//===========================================================================
// CGuiManager::GetCurrentWindow
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
CGuiWindow* CGuiManager::GetCurrentWindow()
{
    return FindWindowByID( m_currentScreen );
}


//===========================================================================
// CGuiManager::DisplayMessage
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
void CGuiManager::DisplayMessage
(
    int messageIndex,
    CGuiEntity* pCallback
)
{
    CGuiScreenMessage::Display( messageIndex, pCallback );

    this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                         CGuiWindow::GUI_SCREEN_ID_GENERIC_MESSAGE,
                         KEEP_WINDOW_HISTORY | FORCE_WINDOW_RELOAD );
}


//===========================================================================
// CGuiManager::DisplayPrompt
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
void CGuiManager::DisplayPrompt
(
    int messageIndex,
    CGuiEntity* pCallback,
    eGenericPromptType promptType,
    bool enableDefaultToNo
)
{
    switch( promptType )
    {
        case PROMPT_TYPE_YES_NO:
        {
            CGuiMenuPrompt::ePromptResponse responses[] =
            {
                CGuiMenuPrompt::RESPONSE_YES,
                CGuiMenuPrompt::RESPONSE_NO
            };

            CGuiScreenPrompt::Display( messageIndex, pCallback, 2, responses );
            CGuiScreenPrompt::EnableDefaultToNo( enableDefaultToNo );

            break;
        }
        case PROMPT_TYPE_CONTINUE:
        {
            CGuiMenuPrompt::ePromptResponse responses[] =
            {
                CGuiMenuPrompt::RESPONSE_CONTINUE
            };

            CGuiScreenPrompt::Display( messageIndex, pCallback, 1, responses );

            break;
        }
        case PROMPT_TYPE_OK:
        {
            CGuiMenuPrompt::ePromptResponse responses[] =
            {
                CGuiMenuPrompt::RESPONSE_OK
            };

            CGuiScreenPrompt::Display( messageIndex, pCallback, 1, responses );

            break;
        }
        case PROMPT_TYPE_DELETE:
        {
            CGuiMenuPrompt::ePromptResponse responses[] =
            {
                CGuiMenuPrompt::RESPONSE_DELETE
            };

            CGuiScreenPrompt::Display( messageIndex, pCallback, 1, responses );

            break;
        }
        case PROMPT_TYPE_LOAD:
        {
            CGuiMenuPrompt::ePromptResponse responses[] =
            {
                CGuiMenuPrompt::RESPONSE_LOAD
            };

            CGuiScreenPrompt::Display( messageIndex, pCallback, 1, responses );

            break;
        }
        case PROMPT_TYPE_SAVE:
        {
            CGuiMenuPrompt::ePromptResponse responses[] =
            {
                CGuiMenuPrompt::RESPONSE_SAVE
            };

            CGuiScreenPrompt::Display( messageIndex, pCallback, 1, responses );

            break;
        }
        default:
        {
            rAssertMsg( 0, "*** ERROR: Invalid prompt type!" );

            break;
        }
    }

    this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                         CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT,
                         KEEP_WINDOW_HISTORY | FORCE_WINDOW_RELOAD );
}


//===========================================================================
// CGuiManager::DisplayErrorPrompt
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
void CGuiManager::DisplayErrorPrompt
(
    int messageIndex,
    CGuiEntity* pCallback,
    int promptResponses
)
{
    int numResponses = 0;
    CGuiMenuPrompt::ePromptResponse responses[ CGuiMenuPrompt::MAX_NUM_RESPONSES ];


	if( promptResponses & ERROR_RESPONSE_CONTINUE )
    {
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_CONTINUE;
    }

	if( promptResponses & ERROR_RESPONSE_CONTINUE_WITHOUT_SAVE )
    {
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_CONTINUE_WITHOUT_SAVE;
    }

    if( promptResponses & ERROR_RESPONSE_RETRY )
    {
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_RETRY;
    }

    if( promptResponses & ERROR_RESPONSE_DELETE )
    {
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_DELETE;
    }

    if( promptResponses & ERROR_RESPONSE_YES )
    {
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_YES;
    }

	if( promptResponses & ERROR_RESPONSE_NO )
	{
		responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_NO;
	}

    if( promptResponses & ERROR_RESPONSE_FORMAT )
    {
#ifdef RAD_XBOX
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_FORMAT_XBOX;
#endif
#ifdef RAD_WIN32
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_FORMAT_XBOX;
#endif
#ifdef RAD_GAMECUBE
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_FORMAT_GC;
#endif
#ifdef RAD_PS2
        responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_FORMAT_PS2;
#endif
    }

    if( promptResponses & ERROR_RESPONSE_MANAGE )
	{
		responses[ numResponses++ ] = CGuiMenuPrompt::RESPONSE_MANAGE_MEMCARDS;
	}

    CGuiScreenPrompt::Display( messageIndex, pCallback, numResponses, responses );
    CGuiScreenPrompt::EnableDefaultToNo( false );

    this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                         CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT,
                         KEEP_WINDOW_HISTORY | FORCE_WINDOW_RELOAD );
}


CGuiWindow::eGuiWindowID
CGuiManager::GetPreviousScreen( int fromCurrentScreen ) const
{
    CGuiWindow::eGuiWindowID previousScreenID = CGuiWindow::GUI_WINDOW_ID_UNDEFINED;

    int windowHistoryCount = m_windowHistoryCount - fromCurrentScreen;
    if( windowHistoryCount > 0 )
    {
        previousScreenID = m_windowHistory[ windowHistoryCount - 1 ];
    }

    return previousScreenID;
}

CGuiWindow::eGuiWindowID CGuiManager::GetCurrentScreen() const
{
    return m_currentScreen;
}

//===========================================================================
// Protected Member Functions
//===========================================================================

//===========================================================================
// CGuiManager::AddWindow
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
void CGuiManager::AddWindow
(
    CGuiWindow::eGuiWindowID windowID,
    CGuiWindow* pWindow 
)
{
    rAssert( this->IsValidWindowID( windowID ) && pWindow != NULL );

    rAssert( m_windows[ windowID ] == NULL );
    m_windows[ windowID ] = pWindow;

    m_numWindows++;
}

void
CGuiManager::RemoveWindow( CGuiWindow::eGuiWindowID windowID )
{
    rAssert( this->IsValidWindowID( windowID ) );

    if( m_windows[ windowID ] != NULL )
    {
        delete m_windows[ windowID ];
        m_windows[ windowID ] = NULL;

        m_numWindows--;
    }
}

void
CGuiManager::RemoveAllWindows()
{
    for( int i = 0; i < CGuiWindow::NUM_GUI_WINDOW_IDS; i++ )
    {
        if( m_windows[ i ] != NULL )
        {
            delete m_windows[ i ];
            m_windows[ i ] = NULL;
        }
    }

    m_numWindows = 0;
}

void
CGuiManager::PushScreenHistory( CGuiWindow::eGuiWindowID windowID )
{
    if( this->IsValidWindowID( windowID ) )
    {
        rAssert( m_windowHistoryCount < MAX_WINDOW_HISTORY );

        m_windowHistory[ m_windowHistoryCount ] = windowID;
        m_windowHistoryCount++;
    }
}

CGuiWindow::eGuiWindowID
CGuiManager::PopScreenHistory()
{
    CGuiWindow::eGuiWindowID windowID = CGuiWindow::GUI_WINDOW_ID_UNDEFINED;

    if( m_windowHistoryCount > 0 )
    {
        m_windowHistoryCount--;

        windowID = m_windowHistory[ m_windowHistoryCount ];
    }

    return windowID;
}

//===========================================================================
// Private Member Functions
//===========================================================================

