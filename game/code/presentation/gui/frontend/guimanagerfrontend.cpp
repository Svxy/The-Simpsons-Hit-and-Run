//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerFrontEnd
//
// Description: Implementation of the CGuiManagerFrontEnd class.
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
#include <presentation/gui/frontend/guimanagerfrontend.h>
#include <presentation/gui/guiwindow.h> // for window IDs
#include <presentation/gui/guisystem.h>

#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guiscreenmemorycard.h>
#include <presentation/gui/guiscreenmemcardcheck.h>

#include <presentation/gui/frontend/guiscreensplash.h>
#include <presentation/gui/frontend/guiscreenmainmenu.h>
#include <presentation/gui/frontend/guiscreenloadgame.h>
#include <presentation/gui/frontend/guiscreenscrapbook.h>
#include <presentation/gui/frontend/guiscreenscrapbookcontents.h>
#include <presentation/gui/frontend/guiscreenscrapbookstats.h>
#include <presentation/gui/frontend/guiscreencardgallery.h>
#include <presentation/gui/frontend/guiscreenmissiongallery.h>
#include <presentation/gui/frontend/guiscreenskingallery.h>
#include <presentation/gui/frontend/guiscreenvehiclegallery.h>
#include <presentation/gui/frontend/guiscreenmultisetup.h>
#include <presentation/gui/frontend/guiscreenmultichoosechar.h>
#include <presentation/gui/frontend/guiscreenoptions.h>
#ifdef RAD_WIN32
#include <presentation/gui/frontend/guiscreencontrollerWin32.h>
#else
#include <presentation/gui/frontend/guiscreencontroller.h>
#endif
#include <presentation/gui/frontend/guiscreensound.h>
#include <presentation/gui/frontend/guiscreenviewcredits.h>
#include <presentation/gui/frontend/guiscreenviewmovies.h>
#include <presentation/gui/frontend/guiscreenplaymovie.h>
#include <presentation/gui/frontend/guiscreendisplay.h>

#include <presentation/presentation.h>
#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/fmvplayer/fmvuserinputhandler.h>

#include <data/gamedatamanager.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <input/inputmanager.h>
#include <mission/gameplaymanager.h>
//#include <mission/headtoheadmanager.h>
#include <mission/missionmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <supersprint/supersprintmanager.h>
#include <main/platform.h>
#include <main/game.h>
#include <sound/soundmanager.h>

#include <memory/srrmemory.h>

#include <raddebug.hpp> // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


#define SHOW_SPLASH_SCREEN

#ifdef RAD_RELEASE
    #ifndef RAD_E3
        #define SHOW_INTRO_MOVIE
    #endif
#endif

//#define SKIP_INTRO_MOVIE_IF_GAME_LOADED

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiManagerFrontEnd::CGuiManagerFrontEnd
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
CGuiManagerFrontEnd::CGuiManagerFrontEnd
(
    Scrooby::Project* pProject,
    CGuiEntity* pParent
)
:   CGuiManager( pProject, pParent ),
    m_levelNum( 1 ),
    m_disconnectedController(-1),
    m_quittingToDemo( false ),
    m_controllerPromptShown ( false ),
    m_quittingToMiniGame( false ),
    m_isControllerReconnected( false ),
    m_wasFMVInputHandlerEnabled( false )
{
#ifdef RAD_WIN32
    m_quittingGame = false;
#endif
}


//===========================================================================
// CGuiManagerFrontEnd::~CGuiManagerFrontEnd
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
CGuiManagerFrontEnd::~CGuiManagerFrontEnd()
{
}



//===========================================================================
// CGuiManagerFrontEnd::Populate
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
void CGuiManagerFrontEnd::Populate()
{
MEMTRACK_PUSH_GROUP( "CGUIManagerFrontEnd" );
    HeapMgr()->PushHeap( GMA_LEVEL_FE );

    Scrooby::Screen* pScroobyScreen;
    CGuiScreen* pScreen;

    pScroobyScreen = m_pScroobyProject->GetScreen( "Message3D" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMessage( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_GENERIC_MESSAGE, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Prompt3D" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPrompt( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ErrorPrompt3D" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPrompt( pScroobyScreen, this,
                                        CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT );

        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MemoryCard3D" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMemoryCard( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Splash" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenSplash( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SPLASH, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MemCardCheck" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMemCardCheck( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD_CHECK, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MainMenu" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenIntroTransition( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_INTRO_TRANSITION, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MainMenu" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMainMenu( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MAIN_MENU, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "LoadGame" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLoadGame( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LOAD_GAME, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Blank" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenAutoLoad( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_AUTO_LOAD, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ScrapBook" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenScrapBook( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SCRAP_BOOK, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ScrapBookContents" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenScrapBookContents( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ScrapBookStats" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenScrapBookStats( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SCRAP_BOOK_STATS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "CardGallery" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenCardGallery( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_CARD_GALLERY, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MissionGallery" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMissionGallery( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MISSION_GALLERY, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "SkinGallery" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenSkinGallery( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SKIN_GALLERY, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "VehicleGallery" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenVehicleGallery( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_VEHICLE_GALLERY, pScreen );
    }
/*
    pScroobyScreen = m_pScroobyProject->GetScreen( "MultiSetup" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMultiSetup( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MULTIPLAYER_SETUP, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MultiChooseCharacter" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMultiChooseChar( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MULTIPLAYER_CHOOSE_CHARACTER, pScreen );
    }
*/
    pScroobyScreen = m_pScroobyProject->GetScreen( "Options" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenOptions( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_OPTIONS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Controller" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenController( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_CONTROLLER, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Sound" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenSound( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SOUND, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ViewCredits" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenViewCredits( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_VIEW_CREDITS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ViewMovies" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenViewMovies( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_VIEW_MOVIES, pScreen );
    }

#ifdef RAD_WIN32
    pScroobyScreen = m_pScroobyProject->GetScreen( "Display" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenDisplay( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_DISPLAY, pScreen );
    }
#endif

    pScroobyScreen = m_pScroobyProject->GetScreen( "PlayMovie" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPlayMovie( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Demo" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPlayMovie( pScroobyScreen, this,
                                           CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_DEMO );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_DEMO, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Blank" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPlayMovie( pScroobyScreen, this,
                                           CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_INTRO );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_INTRO, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Blank" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPlayMovie( pScroobyScreen, this,
                                           CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME, pScreen );
    }

    HeapMgr()->PopHeap( GMA_LEVEL_FE );
MEMTRACK_POP_GROUP( "CGUIManagerFrontEnd" );
}

void
CGuiManagerFrontEnd::Start( CGuiWindow::eGuiWindowID initialWindow )
{
    rAssert( GUI_FE_UNINITIALIZED == m_state );

    m_nextScreen = initialWindow != CGuiWindow::GUI_WINDOW_ID_UNDEFINED ?
                   initialWindow :
                   CGuiWindow::GUI_SCREEN_ID_MAIN_MENU;

// On PC never show the splash screen.. it is very console-ish...
#if defined(SHOW_SPLASH_SCREEN) && !defined(RAD_WIN32)
    bool skipSplashScreen = CommandLineOptions::Get( CLO_NO_SPLASH );
#else
    bool skipSplashScreen = true;
#endif
    if( skipSplashScreen &&
        initialWindow == CGuiWindow::GUI_SCREEN_ID_SPLASH )
    {
        if( s_memcardCheckState != MEM_CARD_CHECK_COMPLETED )
        {
            m_nextScreen = CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD_CHECK;
        }
        else
        {
            m_nextScreen = CGuiWindow::GUI_SCREEN_ID_INTRO_TRANSITION;
        }

        GetGuiSystem()->SetSplashScreenFinished();
    }

    if( s_memcardCheckState != MEM_CARD_CHECK_COMPLETED )
    {
        CGuiScreen* messageScreen = static_cast<CGuiScreen*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_GENERIC_MESSAGE ) );
        if( messageScreen != NULL )
        {
            messageScreen->SetScroobyScreen( m_pScroobyProject->GetScreen( "Message" ) );
        }

		CGuiScreen* errorPromptScreen = static_cast<CGuiScreen*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT ) );
		if( errorPromptScreen != NULL )
		{
			errorPromptScreen->SetScroobyScreen( m_pScroobyProject->GetScreen( "ErrorPrompt" ) );
		}

		CGuiScreen* genericPromptScreen = static_cast<CGuiScreen*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT ) );
		if( genericPromptScreen != NULL )
		{
			genericPromptScreen->SetScroobyScreen( m_pScroobyProject->GetScreen( "Prompt" ) );
		}

	}

    m_state = GUI_FE_CHANGING_SCREENS; // must be set before calling GotoScreen()

    CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
    rAssert( nextScreen != NULL );
    m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
}

void
CGuiManagerFrontEnd::OnControllerConnected( int controllerID )
{
#ifdef RAD_PS2 
    if( m_controllerPromptShown 
        && m_disconnectedController == controllerID )
#else
    if( m_controllerPromptShown 
        && GetGuiSystem()->GetPrimaryController() == controllerID )
#endif
    {
        m_isControllerReconnected = true;
        m_disconnectedController = -1;

        GetGame()->GetPlatform()->ClearControllerError();
    }
}

void
CGuiManagerFrontEnd::OnControllerDisconnected( int controllerID )
{
    if( !m_controllerPromptShown )
    {
        m_controllerPromptShown = true;

        // disable FMV user input handler
        //
        FMVUserInputHandler* userInputHandler = GetPresentationManager()->GetFMVPlayer()->GetUserInputHandler();
        rAssert( userInputHandler != NULL );
        m_wasFMVInputHandlerEnabled = userInputHandler->IsEnabled();
        userInputHandler->SetEnabled( false );

 //       this->DisplayMessage( CGuiScreenMessage::MSG_ID_CONTROLLER_DISCONNECTED_GC + PLATFORM_TEXT_INDEX );
        char str_buffer[256];
        CGuiScreenMessage::GetControllerDisconnectedMessage(controllerID, str_buffer,  255);
        GetGame()->GetPlatform()->OnControllerError(str_buffer);
    }
}

//===========================================================================
// CGuiManagerFrontEnd::HandleMessage
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
void CGuiManagerFrontEnd::HandleMessage
(
    eGuiMessage message,
    unsigned int param1,
    unsigned int param2
)
{
    switch( message )
    {
        case GUI_MSG_WINDOW_FINISHED:
        {
            if( GUI_FE_CHANGING_SCREENS == m_state )
            {
                m_currentScreen = m_nextScreen;

                CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
                rAssert( nextScreen != NULL );
                m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
            }
/*
            else if( GUI_FE_HIDING_POPUP == m_state )
            {
                m_state = GUI_FE_SCREEN_RUNNING;

                // Tell the current screen to resume.
                //
                this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_RESUME );
            }
*/
            else if( GUI_FE_SHUTTING_DOWN == m_state )
            {
                m_state = GUI_FE_TERMINATED;

                CGuiScreen::Reset3dFEMultiController();

                if( m_quittingToDemo )
                {
                    if ( CommandLineOptions::Get( CLO_SEQUENTIAL_DEMO ) )
                    {
                        RenderEnums::LevelEnum oldLevel = GetGameplayManager()->GetCurrentLevelIndex();
                        
                        unsigned int whichLevel = (oldLevel + 1) % RenderEnums::numLevels;
                        GetGameplayManager()->SetLevelIndex( (RenderEnums::LevelEnum)whichLevel );

                        if ( whichLevel == 0 )
                        {
                            RenderEnums::MissionEnum oldMisssion = GetGameplayManager()->GetCurrentMissionForDemo();
                            unsigned int whichMission = (oldMisssion + 1) % RenderEnums::numMissions;
                            GetGameplayManager()->SetMissionIndex( (RenderEnums::MissionEnum)whichMission );
                        }
                    }
                    else
                    {

#ifdef NEALL_DEMOTEST
                        unsigned int whichLevel = RenderEnums::L1;  //All seven levels
                        GetGameplayManager()->SetLevelIndex( (RenderEnums::LevelEnum)whichLevel );

                        unsigned int whichMission = RenderEnums::M1; //Why not.
                        GetGameplayManager()->SetMissionIndex( (RenderEnums::MissionEnum)whichMission );
#else
                        unsigned int whichLevel = rand() % RenderEnums::numLevels;  //All seven levels
                        GetGameplayManager()->SetLevelIndex( (RenderEnums::LevelEnum)whichLevel );

                        unsigned int whichMission = rand() % RenderEnums::numMissions; //Why not.
                        GetGameplayManager()->SetMissionIndex( (RenderEnums::MissionEnum)whichMission );
#endif
                    }

                    GetInputManager()->RegisterControllerID( 0, 0 );

                    // let's go to demo mode
                    //
                    GetGameFlow()->SetContext( CONTEXT_LOADING_DEMO );
                }
                else if( m_quittingToMiniGame )
                {
                    // let's go to mini-game mode
                    //
                    GetGameFlow()->SetContext( CONTEXT_SUPERSPRINT_FE );
                }
#ifdef RAD_WIN32
                else if( m_quittingGame )
                {
                    // let's begin the quit procedure
                    //
                    GetGameFlow()->SetContext( CONTEXT_EXIT );
                }
#endif
                else
                {
                    CurrentMissionStruct currentMission = GetCharacterSheetManager()->QueryCurrentMission();

                    // Set the current level and mission to load
                    //
                    GetGameplayManager()->SetLevelIndex( currentMission.mLevel );
                    GetGameplayManager()->SetMissionIndex( static_cast<RenderEnums::MissionEnum>( currentMission.mMissionNumber ) );

                    // let's go to normal gameplay mode
                    //
                    GetGameFlow()->SetContext( CONTEXT_LOADING_GAMEPLAY );
                }
            }

            break;
        }

        case GUI_MSG_SPLASH_SCREEN_DONE:
        {
            GetGuiSystem()->SetSplashScreenFinished();

            if( s_memcardCheckState == CGuiManager::MEM_CARD_CHECK_NOT_DONE )
            {
                CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                            CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD_CHECK,
                                            CLEAR_WINDOW_HISTORY );
            }
            else
            {
                rAssert( s_memcardCheckState == CGuiManager::MEM_CARD_CHECK_COMPLETED );

                this->StartIntroMovie();
            }

            break;
        }

        case GUI_MSG_MEMCARD_CHECK_COMPLETED:
        {
            CGuiScreen* messageScreen = static_cast<CGuiScreen*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_GENERIC_MESSAGE ) );
            if( messageScreen != NULL )
            {
                messageScreen->SetScroobyScreen( m_pScroobyProject->GetScreen( "Message3D" ) );
            }

            CGuiScreen* errorPromptScreen = static_cast<CGuiScreen*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT ) );
            if( errorPromptScreen != NULL )
            {
                errorPromptScreen->SetScroobyScreen( m_pScroobyProject->GetScreen( "ErrorPrompt3D" ) );
            }

			CGuiScreen* genericPromptScreen = static_cast<CGuiScreen*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT ) );
			if( genericPromptScreen != NULL )
			{
				genericPromptScreen->SetScroobyScreen( m_pScroobyProject->GetScreen( "Prompt3D" ) );
			}

			this->StartIntroMovie();

            break;
        }

        case GUI_MSG_QUIT_FRONTEND:
        {
            rAssert( GUI_FE_SCREEN_RUNNING == m_state );

            GetEventManager()->TriggerEvent( EVENT_FE_START_GAME_SELECTED );
            GetSoundManager()->DuckEverythingButMusicBegin();

            // Set appropriate gameplay manager depending on number of players
            //
            switch( param1 )
            {
                case 0: // single player demo mode
                {
                    m_quittingToDemo = true;

                    // follow-through
                }
                case 1: // single player story mode
                {
                    SetGameplayManager( MissionManager::GetInstance() );

                    break;
                }
                case 2:
                {
                    m_quittingToMiniGame = true;

                    SetGameplayManager( SuperSprintManager::GetInstance() );

                    break;
                }
                /*
                case 2: // two players head-to-head
                {
                    SetGameplayManager( HeadToHeadManager::GetInstance() );

                    break;
                }
                */
                default:
                {
                    rAssertMsg( 0, "ERROR: *** Invalid Number of Players!\n" );

                    break;
                }
            }

            m_state = GUI_FE_SHUTTING_DOWN;

            // Tell the current screen to shut down.
            //
            this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );

            break;
        }

        case GUI_MSG_QUIT_GAME:
        {
#ifdef RAD_WIN32
            rAssert( GUI_FE_SCREEN_RUNNING == m_state );

            m_state = GUI_FE_SHUTTING_DOWN;

            m_quittingGame = true;

            // Tell the current screen to shut down.
            //
            this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );
#endif // RAD_WIN32

            break;
        }


        case GUI_MSG_CONTROLLER_DISCONNECT:
        {
#ifdef RAD_PS2
            if( !m_controllerPromptShown )
            {
                // show controller disconnected message on PS2 only if:
                //
                // - primary controller hasn't been established yet and ANY controller is disconnected; OR
                // - the controller disconnected is the primary controller
                //
                int primaryControllerID = GetGuiSystem()->GetPrimaryController();
                if( primaryControllerID == -1 || primaryControllerID == static_cast<int>( param1 ) )
                {
                    m_disconnectedController = static_cast<int>( param1 );
                }
            }
#endif // RAD_PS2

            break;
        }

        default:
        {
            if( message == GUI_MSG_UPDATE && m_isControllerReconnected )
            {
                m_isControllerReconnected = false;
                m_controllerPromptShown = false;

                // re-enable FMV user input handler
                //
                FMVUserInputHandler* userInputHandler = GetPresentationManager()->GetFMVPlayer()->GetUserInputHandler();
                rAssert( userInputHandler != NULL );
                userInputHandler->SetEnabled( m_wasFMVInputHandlerEnabled );
            }

            if( m_controllerPromptShown )
            {
                if (message==GUI_MSG_CONTROLLER_START)  // start trigger reconnection
                {
                    this->OnControllerConnected( static_cast<int>( param1 ) );
                }
                break;  // don't pass event if controller error
            }

            if( m_state != GUI_FE_UNINITIALIZED && m_currentScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
            {
                // Send the messages down to the current screen.
                //
                CGuiScreen* pScreen = static_cast<CGuiScreen*>(this->FindWindowByID( m_currentScreen ));
                rAssert( pScreen );
                if (pScreen->IsIgnoringControllerInputs() && pScreen->IsControllerMessage(message))
                    ; // don't pass to screen
                else
                    pScreen->HandleMessage( message, param1, param2 );
            }

            if( message == GUI_MSG_UPDATE )
            {
#ifndef RAD_GAMECUBE
#ifdef RAD_PS2 
                int controllerID = m_disconnectedController;
#else
                int controllerID = GetGuiSystem()->GetPrimaryController();
#endif
                if( controllerID >=0 && !GetInputManager()->GetController( controllerID )->IsConnected() )
                {
                    this->OnControllerDisconnected( controllerID );
                }
#endif
            }

            break;
        }
    }

    // propogate message up the hierarchy
    CGuiManager::HandleMessage( message, param1, param2 );
}


//===========================================================================
// Private Member Functions
//===========================================================================

void
CGuiManagerFrontEnd::StartIntroMovie()
{
#ifdef SKIP_INTRO_MOVIE_IF_GAME_LOADED
    if( GetGameDataManager()->IsGameLoaded() )
    {
        // skip intro fmv and go straight to main menu
        //
        CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                    CGuiWindow::GUI_SCREEN_ID_INTRO_TRANSITION,
                                    CLEAR_WINDOW_HISTORY );
    }
    else
#endif // SKIP_INTRO_MOVIE_IF_GAME_LOADED
    {
#ifdef SHOW_INTRO_MOVIE
        // start the intro fmv
        //
        CGuiScreenPlayMovie* playMovieScreen = static_cast<CGuiScreenPlayMovie*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_INTRO ) );
        rAssert( playMovieScreen != NULL );
        playMovieScreen->SetMovieToPlay( MovieNames::INTROFMV, true, false, false );

        CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                    CGuiWindow::GUI_SCREEN_ID_PLAY_MOVIE_INTRO,
                                    CLEAR_WINDOW_HISTORY );
#else
        CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                    CGuiWindow::GUI_SCREEN_ID_INTRO_TRANSITION,
                                    CLEAR_WINDOW_HISTORY );
#endif // SHOW_INTRO_MOVIE
    }
}

