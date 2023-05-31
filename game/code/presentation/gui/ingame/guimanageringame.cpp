//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerInGame
//
// Description: Implementation of the CGuiManagerInGame class.
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
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/guiwindow.h> // for window IDs
#include <presentation/gui/guisystem.h>

#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guiscreenmemorycard.h>

#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreenmultihud.h>
#include <presentation/gui/ingame/guiscreenpausesunday.h>
#include <presentation/gui/ingame/guiscreenpausemission.h>
#include <presentation/gui/ingame/guiscreenmissionselect.h>
#include <presentation/gui/ingame/guiscreenhudmap.h>
#include <presentation/gui/ingame/guiscreenpauseoptions.h>
#ifdef RAD_WIN32
#include <presentation/gui/ingame/guiscreenpausedisplay.h>
#endif
#include <presentation/gui/ingame/guiscreenpausecontroller.h>
#include <presentation/gui/ingame/guiscreenpausesound.h>
#include <presentation/gui/ingame/guiscreenpausesettings.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>
#include <presentation/gui/ingame/guiscreenmissionover.h>
#include <presentation/gui/ingame/guiscreenmissionsuccess.h>
#include <presentation/gui/ingame/guiscreenlevelstats.h>
#include <presentation/gui/ingame/guiscreenlevelend.h>
#include <presentation/gui/ingame/guiscreenviewcards.h>
#include <presentation/gui/ingame/guiscreenletterbox.h>
#include <presentation/gui/ingame/guiscreeniriswipe.h>
#include <presentation/gui/ingame/guiscreenphonebooth.h>
#include <presentation/gui/ingame/guiscreenpurchaserewards.h>
#include <presentation/gui/ingame/guiscreensavegame.h>
#include <presentation/gui/ingame/guiscreentutorial.h>
#include <presentation/gui/ingame/guiscreencreditspostfmv.h>
#include <presentation/gui/backend/guimanagerbackend.h>
#include <presentation/gui/backend/guiscreenloadingfe.h>
#include <presentation/gui/utility/hudmap.h>

#include <contexts/context.h>
#include <contexts/pausecontext.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <data/memcard/memorycardmanager.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <input/inputmanager.h>
#include <interiors/interiormanager.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <mission/missionmanager.h>
#include <mission/objectives/missionobjective.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <presentation/presentation.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>
#include <sound/soundmanager.h>
#include <meta/eventlocator.h>

#include <p3d/fileftt.hpp>
#include <p3d/utility.hpp>
#include <p3d/view.hpp>

#include <main/platform.h>
#include <main/game.h>

#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#include <layer.h>
#include <page.h>
#include <screen.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const unsigned int MINIMUM_DYNA_LOAD_TIME = 250; // in msec

#ifdef DEBUGWATCH
    static const char* WATCHER_NAMESPACE = "GUI System - Ingame";
    bool g_wReversePauseMenus;
#endif

const char* INGAME_PROJECT_FILES[] =
{
    "art\\frontend\\scrooby\\ingame.p3d",
    "art\\frontend\\scrooby\\pause.p3d",
    "art\\frontend\\scrooby\\rewards.p3d",

    "" // dummy terminator
};

CGuiScreenHud* CGuiManagerInGame::s_currentHUD = NULL;
bool cGuiManagerInGameActive = false;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiManagerInGame::CGuiManagerInGame
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
CGuiManagerInGame::CGuiManagerInGame
(
    Scrooby::Project* pProject,
    CGuiEntity* pParent
)
:   CGuiManager( pProject, pParent ),
    m_nextLevelIndex( -1 ),
    m_nextMissionIndex( -1 ),
    m_isLoadingNewMission( false ),
    m_quitAndReload( false ),
    m_controllerPromptShown( false ),
    m_enteringPauseMenu( false ),
    m_exitingPauseMenu( false ),
    m_onHudEnterCommand( ON_HUD_ENTER_NO_COMMAND ),
    m_levelScreen( NULL ),
    m_levelLayer( NULL ),
    m_elapsedDynaLoadTime( 0 ),
    m_pRewardsProject( NULL ),
    m_unloadMemcardInfoWhenLoaded( false ),
    m_promptSaveBeforeQuit( true ),
    m_quitAfterSave( false ),
#ifdef RAD_WIN32
    m_quitToSystemAfterSave(false),
#endif
    m_isControllerReconnected( false ),
    m_RecieveIrisClosed( "Recieve Iris Closed" ),
    m_resumeGameScreenID( CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
{
    //
    // Set Up Transitions
    //
    m_RecieveIrisClosed.SetEvent( EVENT_GUI_IRIS_WIPE_CLOSED );

    m_RestartMissionTransition.SetNextTransition( m_RecieveIrisClosed );    
    m_RecieveIrisClosed.       SetNextTransition( m_RestartMission    );
    m_RestartMission.          SetNextTransition( NULL                );

    m_AbortMissionTransition.  SetNextTransition( m_AbortMission      );
    m_AbortMission.            SetNextTransition( NULL                );

    cGuiManagerInGameActive = true;
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_INIT );
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE_AND_FINISHED );
    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_TRANSITION_START );
    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_TRANSITION_END );
    GetEventManager()->AddListener( this, EVENT_EXIT_INTERIOR_START );
    GetEventManager()->AddListener( this, EVENT_EXIT_INTERIOR_END );
    GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::DEATH) );

    Scrooby::Project* pLevelProject = GetGuiSystem()->GetScroobyLevelProject();
    if( pLevelProject != NULL )
    {
        m_levelScreen = pLevelProject->GetCurrentScreen();
        rAssert( m_levelScreen );

        // get layer
        //
        Scrooby::Page* pPage = m_levelScreen->GetPage( "PauseBgd" );
        rAssert( pPage );
        m_levelLayer = pPage->GetLayerByIndex( 0 );
        rAssert( m_levelLayer );
        m_levelLayer->SetAlpha( 0.0f );
    }

#ifdef DEBUGWATCH
    radDbgWatchAddBoolean( &g_wReversePauseMenus,
                           "Reverse Pause Menus",
                           WATCHER_NAMESPACE );
#endif
}


//===========================================================================
// CGuiManagerInGame::~CGuiManagerInGame
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
CGuiManagerInGame::~CGuiManagerInGame()
{
    cGuiManagerInGameActive = false;
    GetEventManager()->RemoveAll( this );

#ifdef DEBUGWATCH
    radDbgWatchDelete( &g_wReversePauseMenus );
#endif
    s_currentHUD = NULL;
}


//===========================================================================
// CGuiManagerInGame::Popluate
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
void CGuiManagerInGame::Populate()
{
MEMTRACK_PUSH_GROUP( "CGUIManagerInGame" );
    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    Scrooby::Screen* pScroobyScreen = NULL;
    CGuiScreen* pScreen = NULL;

    pScroobyScreen = m_pScroobyProject->GetScreen( "Message" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMessage( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_GENERIC_MESSAGE, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Prompt" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPrompt( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "ErrorPrompt" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPrompt( pScroobyScreen, this,
                                        CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT );

        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_ERROR_PROMPT, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Hud" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenHud( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_HUD, pScreen );

        // store current HUD reference
        //
        s_currentHUD = dynamic_cast<CGuiScreenHud*>( pScreen );
        rAssert( s_currentHUD != NULL );
    }
/*
    pScroobyScreen = m_pScroobyProject->GetScreen( "MultiHud" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMultiHud( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MULTI_HUD, pScreen );
    }
*/
    pScroobyScreen = m_pScroobyProject->GetScreen( "MissionLoad" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMissionLoad( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MISSION_LOAD, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MissionOver" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMissionOver( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MISSION_OVER, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MissionLoad" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMissionSuccess( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MISSION_SUCCESS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "LevelStats" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLevelStats( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LEVEL_STATS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "LevelEnd" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLevelEnd( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LEVEL_END, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseViewCards" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenViewCards( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_VIEW_CARDS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "LetterBox" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLetterBox( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LETTER_BOX, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "IrisWipe" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenIrisWipe( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PhoneBooth" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPhoneBooth( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PHONE_BOOTH, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PurchaseRewards" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPurchaseRewards( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PURCHASE_REWARDS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseSunday" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPauseSunday( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PAUSE_SUNDAY, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseMission" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPauseMission( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_PAUSE_MISSION, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseOptions" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPauseOptions( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_OPTIONS, pScreen );
    }
#ifdef RAD_WIN32
    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseDisplay" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPauseDisplay( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_DISPLAY, pScreen );
    }
#endif

    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseController" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPauseController( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_CONTROLLER, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseSound" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPauseSound( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SOUND, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "PauseSettings" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenPauseSettings( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SETTINGS, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "MissionSelect" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMissionSelect( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MISSION_SELECT, pScreen );
    }
/*
    pScroobyScreen = m_pScroobyProject->GetScreen( "ViewMap" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenHudMap( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_HUD_MAP, pScreen );
    }
*/
    pScroobyScreen = m_pScroobyProject->GetScreen( "MemoryCard" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenMemoryCard( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_MEMORY_CARD, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "SaveGame" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenSaveGame( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_SAVE_GAME, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "Tutorial" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenTutorial( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_TUTORIAL, pScreen );
    }

    pScroobyScreen = m_pScroobyProject->GetScreen( "CreditsPostFMV" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenCreditsPostFMV( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_VIEW_CREDITS, pScreen );
    }

    // hide HUD if specified in commandline options
    //
    if( CommandLineOptions::Get( CLO_NO_HUD ) )
    {
        if( s_currentHUD != NULL )
        {
            s_currentHUD->SetVisible( false );
        }
    }

    HeapMgr()->PopHeap( GMA_LEVEL_HUD );
MEMTRACK_POP_GROUP("CGUIManagerInGame");
}

void
CGuiManagerInGame::Start( CGuiWindow::eGuiWindowID initialWindow )
{
    rAssert( GUI_FE_UNINITIALIZED == m_state || m_isLoadingNewMission );

    m_nextScreen = initialWindow != CGuiWindow::GUI_WINDOW_ID_UNDEFINED ?
                   initialWindow :
                   CGuiWindow::GUI_SCREEN_ID_HUD;

    m_state = GUI_FE_CHANGING_SCREENS; // must be set before calling GotoScreen()

    CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
    rAssert( nextScreen != NULL );
    m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
}

//===========================================================================
// CGuiManagerInGame::HandleMessage
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
void CGuiManagerInGame::HandleMessage
(
    eGuiMessage message, 
    unsigned int param1,
    unsigned int param2
)
{
    if( m_state == GUI_FE_DYNAMIC_LOADING )
    {
        rAssertMsg( false, "Dynamic loading of pause menu and HUD is no longer implemented!" );

        return;
    }

    if( m_isLoadingNewMission )
    {
        if( message == GUI_MSG_UPDATE )
        {
            this->UpdateDuringMissionLoading( param1 );
        }
        else if( message == GUI_MSG_RESUME_INGAME )
        {
            this->OnNewMissionLoadEnd();
        }

        CGuiManager::HandleMessage( message, param1, param2 );

        // and ignore all other messages
        //
        return;
    }

    if( m_state == GUI_FE_TERMINATED )
    {
        if( message == GUI_MSG_UPDATE )
        {
            this->UpdateWhileLoadingNotDone( param1 );
        }

        // and ignore all other messages
        //
        return;
    }

#ifdef RAD_DEMO
    if( this->IsControllerMessage( message ) )
    {
        GetGameplayManager()->ResetIdleTime();
    }
#endif

    switch( message )
    {
        case GUI_MSG_PROMPT_START_RESPONSE:
        {
            HandleMessage( GUI_MSG_UNPAUSE_INGAME );    
            break;
        }
        case GUI_MSG_QUIT_INGAME_FOR_RELOAD:
        {
            m_quitAndReload = true;
            m_nextLevelIndex = static_cast<int>( param1 );
            m_nextMissionIndex = static_cast<int>( param2 );

            if( GetGameplayManager()->GetLevelComplete() )
            {
                if( GetGameplayManager()->GetGameComplete() )
                {
                    // wrap back to level 1, mission 1 (not the tutorial mission)
                    //
                    GetCharacterSheetManager()->SetCurrentMission( RenderEnums::L1, RenderEnums::M2 ); // M2 = mission 1

                    // tell GUI system to show credits upon returning to FE
                    //
//                    GetGuiSystem()->ShowCreditsUponReturnToFE( true );

                    m_quitAndReload = false;
                }
                else
                {
                    GetCharacterSheetManager()->SetCurrentMission( static_cast<RenderEnums::LevelEnum>( m_nextLevelIndex ),
                                                                   static_cast<RenderEnums::MissionEnum>( m_nextMissionIndex ) );
                }

#ifdef RAD_DEMO
                m_promptSaveBeforeQuit = false;
                m_quitAndReload = false;
#else
                this->DisplayPrompt( PROMPT_CONFIRM_SAVE_BEFORE_QUIT,
                                     this,
                                     PROMPT_TYPE_YES_NO,
                                     false );

                break;
#endif // RAD_DEMO
            }
            else
            {
                // don't prompt to save unless we're advancing to a new level
                //
                m_promptSaveBeforeQuit = false;
            }

            // follow-thru ...
            //
        }

        case GUI_MSG_QUIT_INGAME:
        {
            if( GUI_FE_SCREEN_RUNNING == m_state )
            {
#ifndef RAD_DEMO
                if( m_promptSaveBeforeQuit && GetGameplayManager()->IsSundayDrive() )
                {
                    this->DisplayPrompt( PROMPT_CONFIRM_SAVE_BEFORE_QUIT,
                                         this,
                                         PROMPT_TYPE_YES_NO,
                                         false );
                }
                else
#endif
                {
                    m_state = GUI_FE_SHUTTING_DOWN;

                    // if paused in sunday drive mode, unload memory card info
                    //
                    if( GetGameplayManager()->IsSundayDrive() )
                    {
                        if( GetMemoryCardManager()->IsMemcardInfoLoaded() )
                        {
                            // unload it now
                            //
                            GetMemoryCardManager()->UnloadMemcardInfo();
                        }
                        else
                        {
                            // wait until loading is complete, then unload it
                            //
                            m_unloadMemcardInfoWhenLoaded = true;
                        }
                    }
/*
                    CGuiScreenPrompt* promptScreen = static_cast<CGuiScreenPrompt*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT ) );
                    rAssert( promptScreen != NULL );
                    promptScreen->RestoreScreenCover();
*/
                    // Tell the current screen to shut down.
                    //
                    this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );
                }
            }

            break;
        }
#ifdef RAD_WIN32
        case GUI_MSG_QUIT_TO_SYSTEM:
            {
                if( GUI_FE_SCREEN_RUNNING == m_state )
                {
                    if( m_promptSaveBeforeQuit && GetGameplayManager()->IsSundayDrive() )
                    {
                        this->DisplayPrompt( PROMPT_CONFIRM_SAVE_BEFORE_QUITTOSYSTEM,
                            this,
                            PROMPT_TYPE_YES_NO,
                            false );
                    }
                    else
                    {
                        m_state = GUI_FE_SHUTTING_DOWN;

                        // if paused in sunday drive mode, unload memory card info
                        //
                        if( GetGameplayManager()->IsSundayDrive() )
                        {
                            if( GetMemoryCardManager()->IsMemcardInfoLoaded() )
                            {
                                // unload it now
                                //
                                GetMemoryCardManager()->UnloadMemcardInfo();
                            }
                            else
                            {
                                // wait until loading is complete, then unload it
                                //
                                m_unloadMemcardInfoWhenLoaded = true;
                            }
                        }
                        /*
                        CGuiScreenPrompt* promptScreen = static_cast<CGuiScreenPrompt*>( this->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT ) );
                        rAssert( promptScreen != NULL );
                        promptScreen->RestoreScreenCover();
                        */
                        // Tell the current screen to shut down.
                        //
                        this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );

                        // let's begin the quit procedure
                        //
                        GetGameFlow()->SetContext( CONTEXT_EXIT );
                    }
                }

                break;
            }
#endif
        case GUI_MSG_MENU_PROMPT_RESPONSE:
        {
#ifdef RAD_WIN32
            if( param1 == PROMPT_CONFIRM_SAVE_BEFORE_QUITTOSYSTEM )
            {
                if( param2 == CGuiMenuPrompt::RESPONSE_YES )
                {
                    this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                        CGuiWindow::GUI_SCREEN_ID_SAVE_GAME );

                    m_quitToSystemAfterSave = true;
                }
                else
                {
                    rAssert( param2 == CGuiMenuPrompt::RESPONSE_NO );

                    m_promptSaveBeforeQuit = false;

                    this->HandleMessage( GUI_MSG_QUIT_TO_SYSTEM );
                }

            }
#endif
            if( param1 == PROMPT_CONFIRM_SAVE_BEFORE_QUIT )
            {
                if( param2 == CGuiMenuPrompt::RESPONSE_YES )
                {
#ifdef RAD_XBOX
                    // Xbox TCR Requirement: always prompt user to select memory
                    // device before loading/saving
                    //
                    CGuiScreenLoadSave::s_forceGotoMemoryCardScreen = true;
#endif
//                    this->PushScreenHistory( GetGameplayManager()->IsSundayDrive() ?
//                                             CGuiWindow::GUI_SCREEN_ID_PAUSE_SUNDAY :
//                                             CGuiWindow::GUI_SCREEN_ID_PAUSE_MISSION );

                    this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                         CGuiWindow::GUI_SCREEN_ID_SAVE_GAME );

                    m_quitAfterSave = true;
                }
                else
                {
                    rAssert( param2 == CGuiMenuPrompt::RESPONSE_NO );

                    m_promptSaveBeforeQuit = false;

                    this->HandleMessage( GUI_MSG_QUIT_INGAME );
                }
            }

            break;
        }
        case GUI_MSG_PAUSE_INGAME:
        {
            if( !this->IsPausingAllowed() )
            {
                rTuneWarningMsg( false, "*** Pause request ingored! ***" );
                break;
            }

            // switch to pause context
            //
            GetGameFlow()->SetContext( CONTEXT_PAUSE );

            bool inSundayDriveMode = GetGameplayManager()->IsSundayDrive();
#ifdef DEBUGWATCH
            if( g_wReversePauseMenus )
            {
                inSundayDriveMode = !inSundayDriveMode;
            }
#endif
            CGuiWindow::eGuiWindowID pauseWindow = inSundayDriveMode ?
                                                   CGuiWindow::GUI_SCREEN_ID_PAUSE_SUNDAY :
                                                   CGuiWindow::GUI_SCREEN_ID_PAUSE_MISSION;

            // remember current screen before going to pause menu
            //
            m_resumeGameScreenID = m_currentScreen;

            CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                        pauseWindow,
                                        CLEAR_WINDOW_HISTORY );

            // if pausing in sunday drive mode, load memory card info
            // for saving games
            //
            if( GetGameplayManager()->IsSundayDrive() )
            {
                m_unloadMemcardInfoWhenLoaded = false;
                GetMemoryCardManager()->LoadMemcardInfo( this );
            }

            GetSoundManager()->OnPauseStart();

            m_enteringPauseMenu = true;
            GetGameplayContext()->PauseAllButPresentation( true );

            m_promptSaveBeforeQuit = true;

            break;
        }
        case GUI_MSG_UNPAUSE_INGAME:
        {
            if( GetGameplayManager()->GetLevelComplete() || GetGameplayManager()->GetGameComplete() )
            {
                // can't un-pause the game if either level or game is just completed (i.e. just
                // finished the last mission of the level)
                //
                break;
            }

            GetSoundManager()->OnPauseEnd();

            // if paused in sunday drive mode, unload memory card info
            //
            if( GetGameplayManager()->IsSundayDrive() )
            {
                if( GetMemoryCardManager()->IsMemcardInfoLoaded() )
                {
                    // unload it now
                    //
                    GetMemoryCardManager()->UnloadMemcardInfo();
                }
                else
                {
                    // wait until loading is complete, then unload it
                    //
                    m_unloadMemcardInfoWhenLoaded = true;
                }
            }

            m_exitingPauseMenu = true;

            // follow-thru ...
            //
        }
        case GUI_MSG_RESUME_INGAME:
        {
            if( GetInteriorManager()->IsEntering() || GetInteriorManager()->IsExiting() )
            {
                // ignore this message, cuz if we're either entering or exiting an interior,
                // we're waiting for either the EVENT_ENTER_INTERIOR_TRANSITION_END or
                // EVENT_EXIT_INTERIOR_END event from the InteriorManager to resume the game
                // screen
                //
                break;
            }

            this->ResumeGame( param1, param2 );

            break;
        }
        case GUI_MSG_ON_SAVE_GAME_COMPLETE:
        {
            m_promptSaveBeforeQuit = false;

            if( m_quitAfterSave )
            {
                this->HandleMessage( GUI_MSG_QUIT_INGAME );
            }
#ifdef RAD_WIN32
            else if( m_quitToSystemAfterSave )
            {
                this->HandleMessage( GUI_MSG_QUIT_TO_SYSTEM );
            }
#endif
            else
            {
                this->HandleMessage( GUI_MSG_BACK_SCREEN );
            }

            break;
        }
        case GUI_MSG_BACK_SCREEN:
        {
            if( this->GetPreviousScreen( param1 ) == CGuiWindow::GUI_SCREEN_ID_GENERIC_PROMPT )
            {
                m_quitAfterSave = false;

                CGuiMenuPrompt::ePromptResponse responses[] =
                {
                    CGuiMenuPrompt::RESPONSE_YES,
                    CGuiMenuPrompt::RESPONSE_NO
                };

                CGuiScreenPrompt::Display( PROMPT_CONFIRM_SAVE_BEFORE_QUIT, this, 2, responses );
                CGuiScreenPrompt::EnableDefaultToNo( false );
            }

            break;
        }
        case GUI_MSG_WINDOW_FINISHED:
        {
            if( GUI_FE_CHANGING_SCREENS == m_state )
            {
                m_currentScreen = m_nextScreen;

                if( m_exitingPauseMenu )
                {
                    this->GotoHUDScreen();
                }
                else if( m_enteringPauseMenu )
                {
                    this->GotoPauseScreen();
                }
                else
                {
                    if( this->IsHudScreen( m_nextScreen ) )
                    {
                        this->GotoHUDScreen();
                    }
                    else
                    {
                        CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
                        rAssert( nextScreen != NULL );
                        m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
                    }
                }
            }
            else if( GUI_FE_SHUTTING_DOWN == m_state )
            {
                m_state = GUI_FE_TERMINATED;

                // set backend scrooby project as active project
                //
                GetGuiSystem()->SwitchToBackendProject();

                // pre-run backend loading screen
                //
                CGuiManagerBackEnd* backendManager = GetGuiSystem()->GetBackendManager();
                rAssert( backendManager != NULL );
                backendManager->HandleMessage( GUI_MSG_PRE_RUN_BACKEND,
                                               m_quitAndReload ? IS_LOADING_GAMEPLAY : 0 );

                // enable screen clearing
                //
                GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearMask( PDDI_BUFFER_ALL );
            }

            break;
        }
        case GUI_MSG_INGAME_MISSION_COMPLETE:
        {
            // param1 = 1 --> New Best Time!
            //
            if( s_currentHUD != NULL )
            {
                s_currentHUD->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY,
                                             HUD_MISSION_COMPLETE, param1 );
            }

            break;
        }
        case GUI_MSG_INGAME_MISSION_FAILED:
        {
            CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                        CGuiWindow::GUI_SCREEN_ID_MISSION_OVER,
                                        CLEAR_WINDOW_HISTORY );

            // switch to pause context
            GetGameFlow()->SetContext( CONTEXT_PAUSE );
            break;
        }
        case GUI_MSG_INGAME_MISSION_LOAD_BEGIN:
        {
            //
            // if we're on the iris screen, then we don't want to open the iris
            //
            CGuiWindow::eGuiWindowID current = CGuiManager::GetCurrentScreen();
            if( current == CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE )
            {
                CGuiScreenIrisWipe::DoNotOpenOnNextOutro();
            }
            CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                        CGuiWindow::GUI_SCREEN_ID_MISSION_LOAD,
                                        CLEAR_WINDOW_HISTORY );

            // switch to pause context
            GetGameFlow()->SetContext( CONTEXT_PAUSE );

            break;
        }
        case GUI_MSG_INGAME_MISSION_LOAD_END:
        {
            // notify MissionLoad screen that mission load has completed
            //
            if( !GetGameplayManager()->GetCurrentMission()->IsSundayDrive() )
            {
                GetEventManager()->TriggerEvent( EVENT_GUI_MISSION_LOAD_COMPLETE );
            }

            break;
        }
        case GUI_MSG_SHOW_HUD_OVERLAY:
        case GUI_MSG_HIDE_HUD_OVERLAY:
        {
            // send HUD overlay messages to current HUD
            //
            if( s_currentHUD != NULL )
            {
                s_currentHUD->HandleMessage( message, param1, param2 );
            }

            break;
        }
        case GUI_MSG_CONTROLLER_CONNECT:
        {
#ifndef RAD_GAMECUBE
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
        }
        case GUI_MSG_START_IRIS_WIPE_OPEN:
        {
            this->HandleMessage( GUI_MSG_RESUME_INGAME );

            break;
        }
        case GUI_MSG_START_IRIS_WIPE_CLOSE:
        {
            this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                 CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE,
                                 CLEAR_WINDOW_HISTORY );

            break;
        }
        case GUI_MSG_INGAME_DISPLAY_PROMPT:
        {
            int ingameMessageIndex = NUM_PROMPT_QUESTIONS + static_cast<int>( param1 );
            this->DisplayPrompt( ingameMessageIndex, s_currentHUD, PROMPT_TYPE_CONTINUE );

            rAssert( GetGameFlow()->GetCurrentContext() == CONTEXT_GAMEPLAY );
            GetGameFlow()->SetContext( CONTEXT_PAUSE );

            break;
        }
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
                rAssert( pScreen );

                pScreen->HandleMessage( message, param1, param2 );
            }

#ifndef RAD_GAMECUBE
            // poll controller connection status
            //
            if( message == GUI_MSG_UPDATE )
            {
                int controllerID = GetInputManager()->GetControllerIDforPlayer( 0 );
                if( !GetInputManager()->GetController( controllerID )->IsConnected() )
                {
                    bool setState = !m_controllerPromptShown;
                    this->OnControllerDisconnected( controllerID );

                    if ( setState )
                    {
                        m_oldControllerState = InputManager::GetInstance()->GetGameState();
                    }
                }
            }
#endif // !RAD_GAMECUBE

            break;      
        }
    }

    // propogate message up the hierarchy
    CGuiManager::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiManagerInGame::HandleEvent
//===========================================================================
// Description: This function translates events from to GUI messages
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void
CGuiManagerInGame::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_CONVERSATION_INIT:
        {
            this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                 CGuiWindow::GUI_SCREEN_ID_LETTER_BOX,
                                 CLEAR_WINDOW_HISTORY );

            GetInputManager()->SetGameState(Input::ACTIVE_FRONTEND);

            break;
        }
        case EVENT_CONVERSATION_DONE_AND_FINISHED:
        {
            //
            // Was this mission a patty and selma conversation
            //
            GameplayManager* gpm = GetGameplayManager();
            Mission* mission = gpm->GetCurrentMission();
            bool pattyAndSelma = mission->GetCurrentStage()->GetObjective()->IsPattyAndSelmaDialog();
            if( pattyAndSelma )
            {
               
                GetInputManager()->SetGameState(Input::ACTIVE_GAMEPLAY);
                this->HandleMessage( GUI_MSG_RESUME_INGAME );
                GetEventManager()->TriggerEvent( EVENT_GUI_TRIGGER_PATTY_AND_SELMA_SCREEN );

                /*
                GetGuiSystem()->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                            CGuiWindow::GUI_SCREEN_ID_MISSION_SUCCESS,
                                            CLEAR_WINDOW_HISTORY );
                GetEventManager()->TriggerEvent( EVENT_GUI_ENTERING_MISSION_SUCCESS_SCREEN );                
                GetGameFlow()->SetContext( CONTEXT_PAUSE );
                */
            }
            else
            {
                GetInputManager()->SetGameState(Input::ACTIVE_GAMEPLAY);
                this->HandleMessage( GUI_MSG_RESUME_INGAME );
            }

            break;
        }
        case EVENT_ENTER_INTERIOR_TRANSITION_START:
        case EVENT_EXIT_INTERIOR_START:
        {
/*
            if( m_currentScreen == CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE )
            {
                // ignore, if we're already on the iris wipe screen
                //
                break;
            }
*/
            if( id == EVENT_ENTER_INTERIOR_TRANSITION_START )
            {
                rReleasePrintf( "CGuiManagerInGame <= EVENT_ENTER_INTERIOR_TRANSITION_START.\n" );
            }
            else
            {
                rReleasePrintf( "CGuiManagerInGame <= EVENT_EXIT_INTERIOR_START.\n" );
            }

            this->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                 CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE,
                                 CLEAR_WINDOW_HISTORY );

            break;
        }
        case EVENT_ENTER_INTERIOR_TRANSITION_END:
        case EVENT_EXIT_INTERIOR_END:
        {
            if( id == EVENT_ENTER_INTERIOR_TRANSITION_END )
            {
                rReleasePrintf( "CGuiManagerInGame <= EVENT_ENTER_INTERIOR_TRANSITION_END.\n" );
            }
            else
            {
                rReleasePrintf( "CGuiManagerInGame <= EVENT_EXIT_INTERIOR_END.\n" );
            }

            this->ResumeGame();

            break;
        }
        case (EventEnum)(EVENT_LOCATOR + LocatorEvent::DEATH):
        {
            EventLocator* evtLoc = static_cast<EventLocator*>(pEventData);
            if ( evtLoc->GetPlayerID() < static_cast<unsigned int>(MAX_PLAYERS) )
            {
                //chuck: check if the current hud is active, if it's not then it may not generate the 
                //EVENT_DEATH_VOLUME_SCREEN_BLANK, so we should artifically trigger it
                if( s_currentHUD != NULL && s_currentHUD->IsActive() )
                {
                    //This is a player who triggered this.
                    s_currentHUD->HandleMessage( GUI_MSG_DEATH_VOLUME_START, reinterpret_cast< unsigned int >( pEventData ) );
                }
                else
                {
                    // current HUD is not active, so letz just fire this event back to whoever is
                    // waiting for it and ditch the presentation
                    //
                    GetEventManager()->TriggerEvent(EVENT_DEATH_VOLUME_SCREEN_BLANK,pEventData);
                }
            }

            break;
        }
        default:
        {
            rWarningMsg( false, "Why are we receiving messages we care nothing about?" );

            break;
        }
    }
}


//===========================================================================
// CGuiManagerInGame::OnProjectLoadComplete
//===========================================================================
void
CGuiManagerInGame::OnProjectLoadComplete( Scrooby::Project* pProject )
{
    m_pScroobyProject = pProject;
    GetGuiSystem()->SetCurrentScroobyProject( m_pScroobyProject );

    rReleasePrintf( "Scrooby dynamic loading completed. (%d msec)\n", m_elapsedDynaLoadTime );
}

void
CGuiManagerInGame::OnMemcardInfoLoadComplete()
{
    if( m_unloadMemcardInfoWhenLoaded )
    {
        GetMemoryCardManager()->UnloadMemcardInfo();
    }
}

//===========================================================================
// Private Member Functions
//===========================================================================

void
CGuiManagerInGame::UpdateDuringMissionLoading( unsigned int elapsedTime )
{
    // update backend loading screen
    //
    CGuiManagerBackEnd* backendManager = GetGuiSystem()->GetBackendManager();
    rAssert( backendManager != NULL );
    backendManager->HandleMessage( GUI_MSG_UPDATE, elapsedTime );
}

void
CGuiManagerInGame::UpdateWhileLoadingNotDone( unsigned int elapsedTime )
{
    // update backend loading screen
    //
    CGuiManagerBackEnd* backendManager = GetGuiSystem()->GetBackendManager();
    rAssert( backendManager != NULL );
    backendManager->HandleMessage( GUI_MSG_UPDATE, elapsedTime );

    // waiting for loading manger queue to empty before quitting
    //
    if( GetLoadingManager()->IsLoading() )
    {
        rTunePrintf( ">> Waiting for loading manager to finish loading ... ...\n" );
    }
    else
    {
        // ok, let's quit outa here!
        //
        this->QuitGame();
    }
}

void
CGuiManagerInGame::GotoPauseScreen()
{
    m_enteringPauseMenu = false;
    GetGameplayContext()->PauseAllButPresentation( false );

    rAssert( m_nextScreen == CGuiWindow::GUI_SCREEN_ID_PAUSE_SUNDAY ||
             m_nextScreen == CGuiWindow::GUI_SCREEN_ID_PAUSE_MISSION );

    CGuiScreen* pauseScreen = (CGuiScreen*)CGuiManager::FindWindowByID( m_nextScreen );
    rAssert( pauseScreen );
    pauseScreen->SetZoomingEnabled( true );

    m_pScroobyProject->GotoScreen( pauseScreen->GetScroobyScreen(), this );
}

void
CGuiManagerInGame::GotoHUDScreen()
{
    m_exitingPauseMenu = false;

    GetEventManager()->TriggerEvent( EVENT_GUI_LEAVING_PAUSE_MENU );

    CGuiScreen* hudScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
    rAssert( hudScreen != NULL );
    m_pScroobyProject->GotoScreen( hudScreen->GetScroobyScreen(), this );

    // handle any commands specified for upon entering HUD
    //
    switch( m_onHudEnterCommand )
    {
        case ON_HUD_ENTER_RESTART_MISSION:
        {
            GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_CLOSE, 0x00 );
            m_RestartMissionTransition.Activate();

            // stop any dialog that may still be in progress
            //
            GetEventManager()->TriggerEvent( EVENT_DIALOG_SHUTUP );

            break;
        }
        case ON_HUD_ENTER_ABORT_MISSION:
        {
            m_AbortMissionTransition.Activate();

            // stop any dialog that may still be in progress
            //
            GetEventManager()->TriggerEvent( EVENT_DIALOG_SHUTUP );

            break;
        }
        case ON_HUD_ENTER_SKIP_MISSION:
        {
            GetGameFlow()->SetContext( CONTEXT_GAMEPLAY );
            GetGameplayManager()->AbortCurrentMission();
            GetGameplayManager()->NextMission();

            RenderEnums::LevelEnum currentLevel = GetCharacterSheetManager()->QueryCurrentMission().mLevel;
            RenderEnums::MissionEnum currentMission = GetCharacterSheetManager()->QueryCurrentMission().mMissionNumber;
            GetCharacterSheetManager()->SetMissionSkipped( currentLevel, currentMission );

            break;
        }
        default:
        {
            // don't switch to gameplay context if returning to tutorial screen
            //
            if(m_nextScreen == CGuiWindow::GUI_SCREEN_ID_HUD)
            {
                GetGameFlow()->SetContext( CONTEXT_GAMEPLAY );
            }

            break;
        }
    }

    m_onHudEnterCommand = ON_HUD_ENTER_NO_COMMAND;
}

void
CGuiManagerInGame::ResumeGame( unsigned int param1, unsigned int param2 )
{
    if( m_resumeGameScreenID != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
    {
        CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                    m_resumeGameScreenID,
                                    CLEAR_WINDOW_HISTORY );
    }
    else
    {
        CGuiManager::HandleMessage( GUI_MSG_GOTO_SCREEN,
                                    CGuiWindow::GUI_SCREEN_ID_HUD,
                                    CLEAR_WINDOW_HISTORY );
    }

    m_resumeGameScreenID = CGuiWindow::GUI_WINDOW_ID_UNDEFINED;

    m_onHudEnterCommand = param1;
    m_nextMissionIndex = param2;
}

void
CGuiManagerInGame::QuitGame()
{
    if( m_quitAndReload )
    {
        rAssert( m_nextLevelIndex != -1 && m_nextMissionIndex != -1 );

        // switch to loading context
        //
        if( m_nextLevelIndex != GetGameplayManager()->GetCurrentLevelIndex() )
        {
            if( GetGameplayManager()->GetGameComplete() ) // game finished
            {
                GetGameFlow()->SetContext( CONTEXT_FRONTEND );
            }
            else
            {
                GetGameFlow()->SetContext( CONTEXT_LOADING_GAMEPLAY );
            }
        }
        else
        {
            this->OnNewMissionLoadBegin();
        }

        m_quitAndReload = false;
    }
    else
    {
        // switch to frontend context
        GetGameFlow()->SetContext( CONTEXT_FRONTEND );
    }
}

void
CGuiManagerInGame::OnControllerDisconnected( int controllerID )
{
/*
        if( s_currentHUD != NULL )
        {
            s_currentHUD->SetFadingEnabled( false );
        }
*/
 
        m_controllerPromptShown = true;
        char str_buffer[256];
        CGuiScreenMessage::GetControllerDisconnectedMessage(controllerID, str_buffer,  255);
        GetGame()->GetPlatform()->OnControllerError(str_buffer);

}

void
CGuiManagerInGame::OnControllerConnected( int controllerID )
{
    if( m_controllerPromptShown 
        && GetInputManager()->GetControllerIDforPlayer( 0 ) == controllerID )
    {

#ifdef RAD_XBOX
// go to pause screen after disconnection      
        rAssert(s_currentHUD);
        if( s_currentHUD && s_currentHUD->IsActive() )
            this->HandleMessage( GUI_MSG_PAUSE_INGAME );
#endif
        GetGame()->GetPlatform()->ClearControllerError();
        
        InputManager::GetInstance()->SetGameState(m_oldControllerState);
       
        m_isControllerReconnected = true;

    }
}

//===========================================================================
// CGuiManagerInGame::OnNewMissionLoadBegin
//===========================================================================
// Description: Starting point for loading a new mission (within the same
//              level) from the Mission Select screen.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void
CGuiManagerInGame::OnNewMissionLoadBegin()
{
    GetGameplayManager()->RestartToMission( static_cast<RenderEnums::MissionEnum>( m_nextMissionIndex ) );

    GetPauseContext()->SetWaitingForContextSwitch( true );

    m_resumeGameScreenID = CGuiWindow::GUI_WINDOW_ID_UNDEFINED;
 
    // run backend loading screen
    //
    CGuiManagerBackEnd* backendManager = GetGuiSystem()->GetBackendManager();
    rAssert( backendManager != NULL );
    backendManager->HandleMessage( GUI_MSG_RUN_BACKEND, IS_LOADING_GAMEPLAY );

    m_state = GUI_FE_UNINITIALIZED;

    m_isLoadingNewMission = true;
}

//===========================================================================
// CGuiManagerInGame::OnNewMissionLoadEnd
//===========================================================================
// Description: Finishing point for loading a new mission (within the same
//              level) from the Mission Select screen.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void
CGuiManagerInGame::OnNewMissionLoadEnd()
{
    this->Start();
    this->ClearScreenHistory();

    // quit backend loading screen
    //
    CGuiManagerBackEnd* backendManager = GetGuiSystem()->GetBackendManager();
    rAssert( backendManager != NULL );
    backendManager->HandleMessage( GUI_MSG_QUIT_BACKEND );

    GetGuiSystem()->SwitchToCurrentProject();

    m_isLoadingNewMission = false;
}

//===========================================================================
// CGuiManagerInGame::IsPausingAllowed
//===========================================================================
// Description: returns FALSE if in-game pausing is not allowed due to
//              at least one condition; otherwise, returns TRUE
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
bool
CGuiManagerInGame::IsPausingAllowed() const
{
    if( m_state == GUI_FE_CHANGING_SCREENS )
    {
        // don't allow pausing if we're currently in the middle of changing screens
        //
        return false;
    }

    if( GameplayContext::GetInstance()->IsPaused() || GetPresentationManager()->IsBusy() )
    {
        return false;
    }

    if( GetGameplayManager()->GetCurrentMessage() != GameplayManager::NONE )
    {
        // don't allow pausing if the gameplay manager is in the middle of changing missions
        //
        rAssert( GetGameplayManager()->GetCurrentMessage() == GameplayManager::PREV_MISSION || 
                 GetGameplayManager()->GetCurrentMessage() == GameplayManager::NEXT_MISSION );

        return false;
    }

    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
    if( currentMission->IsChangingStages() )
    {
        // don't allow pausing if the current mission is in the middle of changing stages
        //
        return false;
    }

    if( GetGameplayManager()->GetLevelComplete() || GetGameplayManager()->GetGameComplete() )
    {
        // don't allow pausing if level/game has just been completed
        //
        return false;
    }

    return true;
}


//===========================================================================
// AbortCurrentMission::Activate
//===========================================================================
// Description: cancels a mission
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void GuiSFX::AbortCurrentMission::Activate()
{
    GetGameFlow()->SetContext( CONTEXT_GAMEPLAY );
    GetGameplayManager()->AbortCurrentMission();
    ContinueChain();
};

//===========================================================================
// RestartCurrentMission::OpenIris
//===========================================================================
// Description: reactivates a paused mission
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void GuiSFX::OpenIris::Activate()
{
    bool irisClosed = CGuiScreenIrisWipe::IsIrisClosed();
    if( irisClosed )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_OPEN, 0x00 );
    }
    ContinueChain();
};

//===========================================================================
// RestartCurrentMission::Activate
//===========================================================================
// Description: reactivates a paused mission
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void GuiSFX::RestartCurrentMission::Activate()
{
    GetGameplayManager()->RestartCurrentMission();
    ContinueChain();
};
