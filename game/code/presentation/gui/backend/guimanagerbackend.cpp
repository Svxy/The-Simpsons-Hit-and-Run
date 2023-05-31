//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerBackEnd
//
// Description: Implementation of the CGuiManagerBackEnd class.
//
// Authors:     Tony Chu
//
// Revisions        Date            Author      Revision
//                  2002/07/15      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/backend/guimanagerbackend.h>
#include <presentation/gui/backend/guiscreenloading.h>
#include <presentation/gui/backend/guiscreenloadingfe.h>
#include <presentation/gui/backend/guiscreendemo.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiwindow.h> // for window IDs

#include <contexts/bootupcontext.h>

#include <gameflow/gameflow.h>

#include <memory/srrmemory.h>

#include <raddebug.hpp>     // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiManagerBackEnd::CGuiManagerBackEnd
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
CGuiManagerBackEnd::CGuiManagerBackEnd
(
    Scrooby::Project* pProject,
    CGuiEntity* pParent
)
:   CGuiManager( pProject, pParent ),
    m_isQuittingDemo( false ),
    m_isBackendPreRun( false )
{
}


//===========================================================================
// CGuiManagerBackEnd::~CGuiManagerBackEnd
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
CGuiManagerBackEnd::~CGuiManagerBackEnd()
{
    for( int i = 0; i < CGuiWindow::NUM_GUI_WINDOW_IDS; i++ )
    {
        if( m_windows[ i ] != NULL )
        {
            delete m_windows[ i ];
            m_windows[ i ] = NULL;
        }
    }
}


//===========================================================================
// CGuiManagerBackEnd::Populate
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
void CGuiManagerBackEnd::Populate()
{
MEMTRACK_PUSH_GROUP( "CGUIManagerBackEnd" );
    Scrooby::Screen* pScroobyScreen;
    CGuiScreen* pScreen;

    pScroobyScreen = m_pScroobyProject->GetScreen( "Loading" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLoading( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LOADING, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "LoadingFE" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLoadingFE( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LOADING_FE, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Demo" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenDemo( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_DEMO, pScreen );
    }
MEMTRACK_POP_GROUP( "CGUIManagerBackEnd" );
}

void
CGuiManagerBackEnd::Start( CGuiWindow::eGuiWindowID initialWindow )
{
    rAssert( m_state == GUI_FE_UNINITIALIZED );
    m_state = GUI_FE_SCREEN_RUNNING;
/*
    m_nextScreen = initialWindow != CGuiWindow::GUI_WINDOW_ID_UNDEFINED ?
                   initialWindow :
                   CGuiWindow::GUI_SCREEN_ID_LOADING;

    m_state = GUI_FE_CHANGING_SCREENS; // must be set before calling GotoScreen()

    CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
    rAssert( nextScreen != NULL );
    m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
*/
}

//===========================================================================
// CGuiManagerBackEnd::HandleMessage
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
void CGuiManagerBackEnd::HandleMessage
(
    eGuiMessage message,
    unsigned int param1,
    unsigned int param2
)
{
    switch( message )
    {
        case GUI_MSG_PRE_RUN_BACKEND:
        {
            this->GotoLoadingScreen( param1 );

            m_isBackendPreRun = true;

            break;
        }

        case GUI_MSG_RUN_BACKEND:
        {
            if( !m_isBackendPreRun )
            {
                this->GotoLoadingScreen( param1 );
            }

            // load dynamic resources for loading screen
            //
            CGuiWindow* loadingScreen = this->FindWindowByID( m_nextScreen );
            rAssert( loadingScreen != NULL );
            loadingScreen->HandleMessage( GUI_MSG_LOAD_RESOURCES );

            break;
        }

        case GUI_MSG_QUIT_BACKEND:
        {
            m_isBackendPreRun = false;

            // Tell the current screen to shut down.
            //
            this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );

            // Go to a blank screen.
            //
            m_pScroobyProject->GotoScreen( "Blank", NULL );

            break;
        }

        case GUI_MSG_RUN_DEMO:
        {
            CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                        CGuiWindow::GUI_SCREEN_ID_DEMO,
                                        CLEAR_WINDOW_HISTORY | FORCE_WINDOW_RELOAD | FORCE_WINDOW_CHANGE_IMMEDIATE );

            break;
        }

        case GUI_MSG_QUIT_DEMO:
        {
            m_isQuittingDemo = true;

            CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                        CGuiWindow::GUI_SCREEN_ID_LOADING_FE,
                                        CLEAR_WINDOW_HISTORY );

            break;
        }

        case GUI_MSG_WINDOW_FINISHED:
        {
            if( GUI_FE_CHANGING_SCREENS == m_state )
            {
                if( m_isQuittingDemo )
                {
                    m_isQuittingDemo = false;

                    // switch to frontend context
                    //
                    GetGameFlow()->SetContext( CONTEXT_FRONTEND );
                }

                m_currentScreen = m_nextScreen;
                CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
                rAssert( nextScreen != NULL );
                m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
            }

            break;
        }

        default:
        {
            if( m_state != GUI_FE_UNINITIALIZED &&
                m_currentScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
            {
                // Send the messages down to the current screen.
                //
                CGuiWindow* pScreen = this->FindWindowByID( m_currentScreen );
                rAssert( pScreen );

                pScreen->HandleMessage( message, param1, param2 );
            }
        }
    }

    // propogate message up the hierarchy
    CGuiManager::HandleMessage( message, param1, param2 );
}


//===========================================================================
// Private Member Functions
//===========================================================================

void
CGuiManagerBackEnd::GotoLoadingScreen( unsigned int param1 )
{
    bool isLoadingGameplay = (param1 == IS_LOADING_GAMEPLAY ||
                              GetGameFlow()->GetNextContext() == CONTEXT_LOADING_GAMEPLAY);

    unsigned int loadingScreenID = isLoadingGameplay ?
                                   CGuiWindow::GUI_SCREEN_ID_LOADING :
                                   CGuiWindow::GUI_SCREEN_ID_LOADING_FE;
/*
#ifdef RAD_E3
    // always show I&S loading screen for E3 build
    //
    loadingScreenID = CGuiWindow::GUI_SCREEN_ID_LOADING_FE;
#endif
*/
    CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                loadingScreenID,
                                CLEAR_WINDOW_HISTORY | FORCE_WINDOW_RELOAD | FORCE_WINDOW_CHANGE_IMMEDIATE );
}

