//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerBootUp
//
// Description: Implementation of the CGuiManagerBootUp class.
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
#include <presentation/gui/bootup/guimanagerbootup.h>
#include <presentation/gui/bootup/guiscreenbootupload.h>
#include <presentation/gui/bootup/guiscreenlicense.h>
#include <presentation/gui/bootup/guiscreenlanguage.h>
#include <presentation/gui/frontend/guiscreenloadgame.h>
#include <presentation/gui/guiscreenmemcardcheck.h>
#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiwindow.h> // for window IDs

#include <contexts/bootupcontext.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>

#ifdef RAD_PS2
    #include <main/ps2platform.h>
#endif

#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#ifdef RAD_PS2
  #ifndef PAL
    #define ENABLE_PROGRESSIVE_SCAN_PROMPT
  #endif
#endif

#ifdef RAD_PS2
    #define ENABLE_MEMCARD_CHECK_DURING_BOOTUP
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiManagerBootUp::CGuiManagerBootUp
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
CGuiManagerBootUp::CGuiManagerBootUp
(
    Scrooby::Project* pProject,
    CGuiEntity* pParent
)
:   CGuiManager( pProject, pParent )
{
    if( CommandLineOptions::Get( CLO_SKIP_MEMCHECK ) )
    {
        s_memcardCheckState = MEM_CARD_CHECK_COMPLETED;
    }

#ifdef RAD_DEMO
    // no memory card checking for demos
    //
    s_memcardCheckState = MEM_CARD_CHECK_COMPLETED;
#endif
}


//===========================================================================
// CGuiManagerBootUp::~CGuiManagerBootUp
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
CGuiManagerBootUp::~CGuiManagerBootUp()
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
// CGuiManagerBootUp::Populate
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
void CGuiManagerBootUp::Populate()
{
MEMTRACK_PUSH_GROUP( "CGuiManagerBootUp" );
    Scrooby::Screen* pScroobyScreen;
    CGuiScreen* pScreen;

#ifdef RAD_PS2
    pScroobyScreen = m_pScroobyProject->GetScreen( "BootupLoad" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenBootupLoad( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_BOOTUP_LOAD, pScreen );
    }
#endif

    pScroobyScreen = m_pScroobyProject->GetScreen( "License" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLicense( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LICENSE, pScreen );
    }
/*
    pScroobyScreen = m_pScroobyProject->GetScreen( "Language" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLanguage( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LANGUAGE, pScreen );
    }
*/
    pScroobyScreen = m_pScroobyProject->GetScreen( "MemCardCheck" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMemCardCheck( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD_CHECK, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Blank" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenAutoLoad( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_AUTO_LOAD, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Message" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMessage( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_GENERIC_MESSAGE, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Prompt" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPrompt( pScroobyScreen, this,
                                        CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT );

        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ErrorPrompt" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPrompt( pScroobyScreen, this,
                                        CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT );

        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT, pScreen );
    }
MEMTRACK_POP_GROUP("CGuiManagerBootUp");
}

void
CGuiManagerBootUp::Start( CGuiWindow::eGuiWindowID initialWindow )
{
    rAssert( m_state == GUI_FE_UNINITIALIZED );

    if( initialWindow != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
    {
        m_bootupScreenQueue.push( initialWindow );
    }

#ifdef ENABLE_PROGRESSIVE_SCAN_PROMPT
    // add progressive scan screen to bootup screen queue
    //
    if( GetInputManager()->IsProScanButtonsPressed() )
    {
        m_bootupScreenQueue.push( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT );
    }
#endif

#ifdef ENABLE_MEMCARD_CHECK_DURING_BOOTUP
    // add memcard checking screen to bootup screen queue
    //
    if( s_memcardCheckState == MEM_CARD_CHECK_NOT_DONE )
    {
        m_bootupScreenQueue.push( CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD_CHECK );
    }
#endif

#ifdef RAD_PS2
    // for PS2 only, start off w/ a loading screen
    //
    m_bootupScreenQueue.push( CGuiWindow::GUI_SCREEN_ID_BOOTUP_LOAD );
#endif

    // add license screen to bootup screen queue
    //
    m_bootupScreenQueue.push( CGuiWindow::GUI_SCREEN_ID_LICENSE );


    // alright, letz bring up the first screen of the game!!
    //
    m_nextScreen = this->PopNextScreenInQueue();
    if( m_nextScreen == CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT )
    {
        // special case for progressive scan prompt
        //
        CGuiMenuPrompt::ePromptResponse responses[] =
        {
            CGuiMenuPrompt::RESPONSE_YES,
            CGuiMenuPrompt::RESPONSE_NO
        };

        CGuiScreenPrompt::Display( PROMPT_DISPLAY_PROGRESSIVE_SCAN, this, 2, responses );
        CGuiScreenPrompt::EnableDefaultToNo( false );
    }

    m_state = GUI_FE_CHANGING_SCREENS; // must be set before calling GotoScreen()

    CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
    rAssert( nextScreen != NULL );
    m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
}

//===========================================================================
// CGuiManagerBootUp::HandleMessage
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
void CGuiManagerBootUp::HandleMessage
(
    eGuiMessage message,
    unsigned int param1,
    unsigned int param2
)
{
    switch( message )
    {
        case GUI_MSG_QUIT_BOOTUP:
        {
            rAssert( GUI_FE_SCREEN_RUNNING == m_state );

            rAssertMsg( m_bootupScreenQueue.empty(),
                        "Why are we quitting when there's still screens in the bootup queue??" );

            m_state = GUI_FE_SHUTTING_DOWN;

#ifdef RAD_WIN32
            GetBootupContext()->LoadConfig();
#endif

            // Tell the current screen to shut down.
            //
            this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );

            break;
        }
        case GUI_MSG_WINDOW_FINISHED:
        {
            if( GUI_FE_CHANGING_SCREENS == m_state )
            {
                m_currentScreen = m_nextScreen;
                CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
                rAssert( nextScreen != NULL );
                m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
            }
            else if( GUI_FE_SHUTTING_DOWN == m_state )
            {
                m_state = GUI_FE_TERMINATED;

                // start intro movies
                //
                GetBootupContext()->StartMovies();
            }

            break;
        }
        case GUI_MSG_MENU_PROMPT_RESPONSE:
        {
            rAssert( param1 == PROMPT_DISPLAY_PROGRESSIVE_SCAN );

            if( param2 == CGuiMenuPrompt::RESPONSE_YES )
            {
                // enable progressive scan
                //
#ifdef RAD_PS2
                PS2Platform::GetInstance()->SetProgressiveMode( true );
#endif
            }

            // follow-thru
            //
        }
        case GUI_MSG_BOOTUP_LOAD_COMPLETED:
        case GUI_MSG_LANGUAGE_SELECTION_DONE:
        case GUI_MSG_MEMCARD_CHECK_COMPLETED:
        {
            CGuiWindow::eGuiWindowID nextScreen = this->PopNextScreenInQueue();

            if( nextScreen == CGuiWindow::GUI_SCREEN_ID_BOOTUP_LOAD &&
                GetGuiSystem()->GetCurrentState() == CGuiSystem::BOOTUP_ACTIVE )
            {
                // FE is already loaded, no need to display extra loading screen
                //
                nextScreen = this->PopNextScreenInQueue(); // pop off next screen in queue
            }

            if( nextScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
            {
                CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                            nextScreen,
                                            CLEAR_WINDOW_HISTORY );
            }
            else
            {
                this->HandleMessage( GUI_MSG_QUIT_BOOTUP );
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

CGuiWindow::eGuiWindowID
CGuiManagerBootUp::PopNextScreenInQueue()
{
    CGuiWindow::eGuiWindowID nextScreen = CGuiWindow::GUI_WINDOW_ID_UNDEFINED;

    if( !m_bootupScreenQueue.empty() )
    {
        nextScreen = m_bootupScreenQueue.front();
        m_bootupScreenQueue.pop();

        if( nextScreen == CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD_CHECK )
        {
            s_memcardCheckState = MEM_CARD_CHECK_IN_PROGRESS;
        }
    }

    return nextScreen;
}

