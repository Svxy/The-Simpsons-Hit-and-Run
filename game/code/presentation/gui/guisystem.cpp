//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiSystem
//
// Description: Implementation of the CGuiSystem class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/09/21		DChau		Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/bootup/guimanagerlanguage.h>
#include <presentation/gui/bootup/guimanagerbootup.h>
#include <presentation/gui/bootup/guiscreenlanguage.h>
#include <presentation/gui/backend/guimanagerbackend.h>
#include <presentation/gui/frontend/guimanagerfrontend.h>
#include <presentation/gui/minigame/guimanagerminigame.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/guiuserinputhandler.h>
#include <presentation/tutorialmode.h>

#include <data/gamedatamanager.h>
#include <main/game.h>
#include <main/platform.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/missionscriptloader.h>
#include <input/inputmanager.h>
#include <gameflow/gameflow.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>

#include <p3d/view.hpp>
#include <raddebug.hpp>     // Foundation
#include <raddebugwatch.hpp>
#include <page.h>
#include <polygon.h>
#include <resourcemanager.h>
#include <screen.h>
#include <sprite.h>
#include <text.h>
#include <feloaders.h>

// Static pointer to instance of singleton.
CGuiSystem* CGuiSystem::spInstance = NULL;

// Scrooby simulation time, needed for DrawFrame() render loop
//
unsigned int g_scroobySimulationTime = 0;

//===========================================================================
// Local Constants
//===========================================================================

#define LOAD_LEVEL_SPECIFIC_PROJECT

/*
#ifdef RAD_GAMECUBE
    float g_gcnScreenScaleX = 1.1f;
    float g_gcnScreenScaleY = 1.0f;
#endif

#ifdef RAD_PS2
    float g_ps2screenScale = 1.096f;
#endif
*/

#ifdef DEBUGWATCH
    const char* GUI_WATCHER_NAMESPACE = "GUI System";
    float g_wGuiSimulationTimeFactor = 1.0f;
    int g_wGuiMessage = 0;
    int g_wGuiMessageParam1 = 0;
    int g_wGuiMessageParam2 = 0;

    static void SendMsgToGuiSystem()
    {
        GetGuiSystem()->HandleMessage( static_cast<eGuiMessage>( g_wGuiMessage ),
                                       g_wGuiMessageParam1,
                                       g_wGuiMessageParam2 );
    }

    namespace Scrooby
    {
        extern float g_CameraNearPlane;
        extern float g_CameraFarPlane;
    }

/*
#ifdef RAD_PS2
    static void UpdatePS2ScreenScale()
    {
        Scrooby::App::GetInstance()->EnableScreenScaling( true,
                                                          g_ps2screenScale,
                                                          g_ps2screenScale );
    }
#endif
*/

    // for Formatting Scrooby Drawables in Run-time
    //
    static const char* WATCHER_NAMESPACE_SCROOBY = "GUI System - Scrooby";

    static Scrooby::Page* g_wCurrentScroobyPage = NULL;
    static char g_wScroobyPageName[ 32 ];
    static void SetCurrentScroobyPage()
    {
        Scrooby::Project* currentProject = Scrooby::App::GetInstance()->GetProject();
        rAssert( currentProject );

        Scrooby::Screen* currentScreen = currentProject->GetCurrentScreen();
        rAssert( currentScreen );

        g_wCurrentScroobyPage = currentScreen->GetPage( g_wScroobyPageName );
        rWarning( g_wCurrentScroobyPage );
    }

    static Scrooby::Drawable* g_wCurrentScroobyDrawable = NULL;

    static int g_wScroobyPosX = 0;
    static int g_wScroobyPosY = 0;
    static void SetScroobyDrawablePosition()
    {
        if( g_wCurrentScroobyDrawable != NULL )
        {
            g_wCurrentScroobyDrawable->SetPosition( g_wScroobyPosX, g_wScroobyPosY );
        }
    }

    static float g_wScroobyAlpha = 0.0f;
    static void SetScroobyDrawableAlpha()
    {
        if( g_wCurrentScroobyDrawable != NULL )
        {
            g_wCurrentScroobyDrawable->SetAlpha( g_wScroobyAlpha );
        }
    }

    static void UpdateScroobyDrawableAttributes()
    {
        if( g_wCurrentScroobyDrawable != NULL )
        {
            g_wCurrentScroobyDrawable->GetOriginPosition( g_wScroobyPosX,
                                                          g_wScroobyPosY );
            g_wScroobyAlpha = g_wCurrentScroobyDrawable->GetAlpha();
        }
    }

    static char g_wScroobyTextName[ 32 ];
    static void SetCurrentScroobyDrawableAsText()
    {
        if( g_wCurrentScroobyPage != NULL )
        {
            g_wCurrentScroobyDrawable = g_wCurrentScroobyPage->GetText( g_wScroobyTextName );
            UpdateScroobyDrawableAttributes();
        }

        rWarning( g_wCurrentScroobyDrawable );
    }

    static char g_wScroobySpriteName[ 32 ];
    static void SetCurrentScroobyDrawableAsSprite()
    {
        if( g_wCurrentScroobyPage != NULL )
        {
            g_wCurrentScroobyDrawable = g_wCurrentScroobyPage->GetSprite( g_wScroobySpriteName );
            UpdateScroobyDrawableAttributes();
        }

        rWarning( g_wCurrentScroobyDrawable );
    }

    static char g_wScroobyPolygonName[ 32 ];
    static void SetCurrentScroobyDrawableAsPolygon()
    {
        if( g_wCurrentScroobyPage != NULL )
        {
            g_wCurrentScroobyDrawable = g_wCurrentScroobyPage->GetPolygon( g_wScroobyPolygonName );
            UpdateScroobyDrawableAttributes();
        }

        rWarning( g_wCurrentScroobyDrawable );
    }

    int g_currentLanguage = 0;

    static void ToggleNextLanguage()
    {
        g_currentLanguage = (g_currentLanguage + 1) % NUM_SRR2_LANGUAGES;
        CGuiTextBible::SetCurrentLanguage( SRR2_LANGUAGE[ g_currentLanguage ] );
    }

#endif // DEBUGWATCH

const char* PROJECT_FILE_LANGUAGE = "art\\frontend\\scrooby\\language.p3d";
const char* PROJECT_FILE_BOOTUP = "art\\frontend\\scrooby\\bootup.p3d";
const char* PROJECT_FILE_BACKEND = "art\\frontend\\scrooby\\backend.p3d";
const char* PROJECT_FILE_FRONTEND = "art\\frontend\\scrooby\\frontend.p3d";
const char* PROJECT_FILE_MINIGAME = "art\\frontend\\scrooby\\minigame.p3d";
const char* PROJECT_FILE_INGAME = "art\\frontend\\scrooby\\ingame.p3d";

const char* LICENSE_SCREEN_IMAGE_DIR = "art\\frontend\\dynaload\\images\\license\\";
#ifdef RAD_WIN32
const char* MOUSE_CURSOR_DIR = "art\\frontend\\dynaload\\images\\";
#endif

const char* INGAME_LEVEL_PROJECT_FILES[] = 
{
    "art\\frontend\\scrooby\\ingamel1.p3d",
    "art\\frontend\\scrooby\\ingamel2.p3d",
    "art\\frontend\\scrooby\\ingamel3.p3d",
    "art\\frontend\\scrooby\\ingamel4.p3d",
    "art\\frontend\\scrooby\\ingamel5.p3d",
    "art\\frontend\\scrooby\\ingamel6.p3d",
    "art\\frontend\\scrooby\\ingamel7.p3d"
};

const char* TEXT_BIBLE_NAME = "srr2";

//===========================================================================
// Public Member Functions
//===========================================================================

//==============================================================================
// CGuiSystem::CreateInstance
//==============================================================================
//
// Description: - Creates the CGuiSystem.
//
// Parameters:	None.
//
// Return:      Pointer to the CGuiSystem.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CGuiSystem* CGuiSystem::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "CGUISystem" );
    spInstance = new CGuiSystem;
    rAssert( spInstance != NULL );
MEMTRACK_POP_GROUP( "CGUISystem" );

    return spInstance;
}

//==============================================================================
// CGuiSystem::DestroyInstance
//==============================================================================
//
// Description: Destroy the GUI system.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void CGuiSystem::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}

//==============================================================================
// CGuiSystem::GetInstance
//==============================================================================
//
// Description: - Access point for the CGuiSystem singleton.  
//              - Creates the CGuiSystem if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the CGuiSystem.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CGuiSystem* CGuiSystem::GetInstance()
{
    rAssert( spInstance != NULL );

    return spInstance;
}

//===========================================================================
// CGuiSystem::CGuiSystem
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CGuiSystem::CGuiSystem()
:   CGuiEntity( NULL ),
    m_state( GUI_UNINITIALIZED ),
    m_pTextBible( NULL ),
    m_pManagerLanguage( NULL ),
    m_pManagerBootUp( NULL ),
    m_pManagerBackEnd( NULL ),
	m_pManagerFrontEnd( NULL ),
	m_pManagerMiniGame( NULL ),
	m_pManagerInGame( NULL ),
    m_pUserInputHandlers( NULL ),
    m_numUserInputHandlers( 0 ),
    m_registeredUserInputHandlers( 0 ),
    m_primaryController(-1),
    m_pApp( NULL ),
    m_pProject( NULL ),
    m_pLevelProject( NULL ),
    m_pBackendProject( NULL ),
    m_pLoadingCallback( NULL ),
    m_isSplashScreenFinished( false ),
    m_isRadarEnabled( true ),
    m_isShowCreditsUponReturnToFE( false )
{
    m_pTextBible = new CGuiTextBible;
    rAssert( m_pTextBible != NULL );
}

//===========================================================================
// CGuiSystem::~CGuiSystem
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CGuiSystem::~CGuiSystem()
{
    // unload backend Scrooby project
    //
    if( m_pBackendProject != NULL )
    {
        m_pApp->UnloadProject( m_pBackendProject );
        m_pBackendProject = NULL;
    }

    // delete text bible
    //
    if( m_pTextBible != NULL )
    {
        delete m_pTextBible;
        m_pTextBible = NULL;
    }

    // delete all GUI managers (if not already deleted)
    //
    if( m_pManagerFrontEnd != NULL )
	{
		delete m_pManagerFrontEnd;
        m_pManagerFrontEnd = NULL;
	}

    if( m_pManagerMiniGame != NULL )
	{
		delete m_pManagerMiniGame;
        m_pManagerMiniGame = NULL;
	}

    if( m_pManagerInGame != NULL )
	{
		delete m_pManagerInGame;
        m_pManagerInGame = NULL;
	}

    if( m_pManagerBackEnd != NULL )
	{
		delete m_pManagerBackEnd;
        m_pManagerBackEnd = NULL;
	}

    if( m_pManagerBootUp != NULL )
	{
		delete m_pManagerBootUp;
        m_pManagerBootUp = NULL;
	}

    if( m_pManagerLanguage != NULL )
	{
		delete m_pManagerLanguage;
        m_pManagerLanguage = NULL;
	}

    // delete all GUI user input handlers
    //
    if(m_pUserInputHandlers != NULL )
	{
        for( int i = 0; i < m_numUserInputHandlers; i++ )
        {
            if( m_pUserInputHandlers[ i ] != NULL )
            {
		        m_pUserInputHandlers[ i ]->Release();
                m_pUserInputHandlers[ i ] = NULL;
            }
        }

		delete [] m_pUserInputHandlers;
        m_pUserInputHandlers = NULL;
	}

    // delete Scrooby App singleton
    //
    if( m_pApp != NULL )
    {
        Scrooby::App::DeleteInstance();
        m_pApp = NULL;
    }
}

//===========================================================================
// CGuiSystem::Init
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::Init()
{
MEMTRACK_PUSH_GROUP( "CGUISystem" );
    rAssert( m_state == GUI_UNINITIALIZED );

    m_pApp = Scrooby::App::GetInstance();
    rAssert( m_pApp != NULL );

    // set Scrooby to use full image names depending on whether or not
    // resource files are joined together into single P3D project file
    //
    m_pApp->SetFullImageNames( CommandLineOptions::Get( CLO_FE_UNJOINED ) );

    // set FE text bible loader to only load one string table for the
    // current language
    //
    FeTextBibleLoader::SetOnlyLoadCurrentLanguage( true );

#if defined( PAL ) && defined( RAD_XBOX ) && !defined( RAD_RELEASE )
    // TC: disable this for Xbox non-release PAL builds so that I can
    //     change languages on-the-fly w/ the Watcher
    //
    FeTextBibleLoader::SetOnlyLoadCurrentLanguage( false );
#endif

    // setup the GUI render layer
    //
    GetRenderManager()->mpLayer(RenderEnums::GUI)->AddGuts( m_pApp );
    GetRenderManager()->mpLayer(RenderEnums::GUI)->FreezeCorpse();
    GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearColour( tColour( 0, 0, 0 ) );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    // create array of user input handlers
    //
    m_numUserInputHandlers = GetInputManager()->GetMaxControllers();
    m_pUserInputHandlers = new CGuiUserInputHandler*[ m_numUserInputHandlers ];
    rAssert( m_pUserInputHandlers != NULL );

    // create each user input handler
    //
    for( int i = 0; i < m_numUserInputHandlers; i++ )
    {
        m_pUserInputHandlers[ i ] = new CGuiUserInputHandler;
        rAssert( m_pUserInputHandlers[ i ] != NULL );
        m_pUserInputHandlers[ i ]->AddRef();
    }

    HeapMgr()->PopHeap( GMA_PERSISTENT );

    // register GUI as game data handler
    //
    GetGameDataManager()->RegisterGameData( this, 1, "GUI System" );

#if defined( PAL ) && !defined( RAD_DEMO )
/*
  #ifdef RAD_GAMECUBE
    // GC ONLY: load language configuration file
    //
    HeapMgr()->PushHeap( GMA_TEMP );
    GetMissionScriptLoader()->LoadScriptAsync( "language.ini" );
    HeapMgr()->PopHeap( GMA_TEMP );
  #endif
*/
    // load language Scrooby project (into FE heap)
    //
    GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                     PROJECT_FILE_LANGUAGE,
                                     GMA_LEVEL_FE,
                                     SCROOBY_INVENTORY_LANGUAGE,
                                     SCROOBY_INVENTORY_LANGUAGE );

    m_state = LANGUAGE_LOADING;
#else
    this->OnInitBootUp();
#endif // PAL

#ifdef DEBUGWATCH
    this->RegisterWatcherStuff();
#endif
    MEMTRACK_POP_GROUP( "CGUISystem" );
}

//===========================================================================
// CGuiSystem::Update
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::Update( unsigned int elapsedTime )
{
#ifdef RAD_RELEASE
    // TC [HACK]: To avoid any choppiness in FE animations
    //
    const unsigned int MAX_ELAPSED_TIME = 100; // in msec
    if( elapsedTime > MAX_ELAPSED_TIME )
    {
        rTunePrintf( "*** WARNING: GUI System elapsed time (%d ms) exceeded MAX_ELAPSED_TIME (%d ms)!\n",
                     elapsedTime, MAX_ELAPSED_TIME );

        elapsedTime = 20;
    }
#endif

#ifdef DEBUGWATCH
    elapsedTime = static_cast<unsigned int>( g_wGuiSimulationTimeFactor *
                                             elapsedTime );
#endif

    // check for repeated inputs
    for( int i = 0; i < m_numUserInputHandlers; i++ )
    {
        m_pUserInputHandlers[ i ]->Update( elapsedTime, i );
    }

    // set scrooby simulation time for use in the rendering loop
    //
    g_scroobySimulationTime = elapsedTime;

    // send update message to current GUI manager
    //
    CGuiManager* currentManager = this->GetCurrentManager();
    if( currentManager != NULL )
    {
        currentManager->HandleMessage( GUI_MSG_UPDATE, elapsedTime );
    }

    if( m_state == DEMO_ACTIVE )
    {
        if( m_pManagerInGame != NULL )
        {
            m_pManagerInGame->HandleMessage( GUI_MSG_UPDATE, elapsedTime );
        }
    }
}

//===========================================================================
// CGuiSystem::HandleMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	
//
// Return:      N/A.
//
//===========================================================================
void CGuiSystem::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2
)
{
	switch( message )
	{
        case GUI_MSG_UPDATE:
		{
            rAssertMsg( 0, "*** Call GuiSystem::Update() instead! ***" );

			break;		
		}

		case GUI_MSG_INIT_FRONTEND:
		{
			this->OnInitFrontEnd();

			break;		
		}
		case GUI_MSG_RELEASE_FRONTEND:
		{
            this->OnReleaseFrontEnd();

			break;		
		}
        case GUI_MSG_RUN_FRONTEND:
        {
            // thaw frontend render layer
            GetRenderManager()->mpLayer(RenderEnums::GUI)->Thaw();

            m_pApp->SetProject( m_pProject );

            m_state = FRONTEND_ACTIVE;

            // start the frontend manager
            rAssert( m_pManagerFrontEnd );
            if( param1 != 0 )
            {
                m_pManagerFrontEnd->Start( static_cast<CGuiWindow::eGuiWindowID>( param1 ) );
            }
            else
            {
                m_pManagerFrontEnd->Start();
            }

/*
#ifdef RAD_GAMECUBE
            // enable screen scaling (for GameCube)
            m_pApp->EnableScreenScaling( true, g_gcnScreenScaleX, g_gcnScreenScaleY );
#endif

#ifdef RAD_PS2
            // enable screen scaling (for PS2)
            m_pApp->EnableScreenScaling( true, g_ps2screenScale, g_ps2screenScale );
#endif
*/

            break;
        }

		case GUI_MSG_INIT_MINIGAME:
		{
			this->OnInitMiniGame();

			break;		
		}
		case GUI_MSG_RELEASE_MINIGAME:
		{
            this->OnReleaseMiniGame();

			break;		
		}
        case GUI_MSG_RUN_MINIGAME:
        {
            m_pApp->SetProject( m_pProject );

            m_state = MINIGAME_ACTIVE;

            // start the minigame manager
            //
            rAssert( m_pManagerMiniGame != NULL );
            if( param1 != 0 )
            {
                m_pManagerMiniGame->Start( static_cast<CGuiWindow::eGuiWindowID>( param1 ) );
            }
            else
            {
                m_pManagerMiniGame->Start();
            }

            break;
        }

		case GUI_MSG_INIT_INGAME:
		{
			this->OnInitInGame();

			break;		
		}
		case GUI_MSG_RELEASE_INGAME:
		{
            this->OnReleaseInGame();

			break;		
		}
        case GUI_MSG_RUN_INGAME:
        {
            m_pApp->SetProject( m_pProject );

            m_state = INGAME_ACTIVE;

            // start the in-game manager
            rAssert( m_pManagerInGame );
            m_pManagerInGame->Start();

            break;
        }

		case GUI_MSG_INIT_BOOTUP:
		{
            this->OnInitBootUp();

			break;		
		}
		case GUI_MSG_RELEASE_BOOTUP:
		{
            this->OnReleaseBootUp();

			break;		
		}

        case GUI_MSG_RUN_BACKEND:
        {
            m_pApp->SetProject( m_pBackendProject );

            m_state = GUI_IDLE;

            rAssert( m_pManagerBackEnd );
            m_pManagerBackEnd->HandleMessage( message, param1, param2 );

            break;
        }
        case GUI_MSG_RUN_DEMO:
        {
            m_state = DEMO_ACTIVE;

            rAssert( m_pManagerBackEnd );
            m_pManagerBackEnd->HandleMessage( message, param1, param2 );

            break;
        }
        case GUI_MSG_PROJECT_LOAD_COMPLETE:
        {
            this->OnProjectLoadComplete( (Scrooby::Project*)param1 );

            break;
        }
		default:
		{
            // relay message to current GUI manager
            //
            CGuiManager* currentManager = this->GetCurrentManager();
            if( currentManager != NULL )
            {
                currentManager->HandleMessage( message, param1, param2 );
            }

			break;		
		} 
	}
}

//===========================================================================
// CGuiSystem::OnProjectLoadComplete
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnProjectLoadComplete( Scrooby::Project* pProject )
{
MEMTRACK_PUSH_GROUP( "CGUISystem" );
	rAssert( pProject != NULL );
    m_pProject = pProject;

    // update reference to text bible
    //
    rAssert( m_pTextBible != NULL );
    m_pTextBible->SetTextBible( TEXT_BIBLE_NAME );

    switch( m_state )
    {
        case LANGUAGE_LOADING:
        {
            HeapMgr()->PushHeap( GMA_LEVEL_FE );

			m_pManagerLanguage = new CGuiManagerLanguage( m_pProject, this );
			rAssert( m_pManagerLanguage != NULL );

			// Populate screens	
			//
			m_pManagerLanguage->Populate();

            HeapMgr()->PopHeap( GMA_LEVEL_FE );

            m_state = LANGUAGE_ACTIVE;

            // Start it up!
            //
            m_pManagerLanguage->Start();

            break;
        }
        case BOOTUP_LOADING:
        {
            HeapMgr()->PushHeap( GMA_LEVEL_FE );

			// Create bootup manager (for license screen, etc.)
			//
			m_pManagerBootUp = new CGuiManagerBootUp( m_pProject, this );
			rAssert( m_pManagerBootUp );

			// Populate screens	
			//
			m_pManagerBootUp->Populate();

            // Start it up!
            //
            m_pManagerBootUp->Start();

            m_pApp->SetProject( m_pProject );

            // thaw frontend render layer
            GetRenderManager()->mpLayer(RenderEnums::GUI)->Thaw();

            m_state = BACKEND_LOADING;

            HeapMgr()->PopHeap( GMA_LEVEL_FE );

            break;
        }
        case BACKEND_LOADING:
        {
            HeapMgr()->PushHeap( GMA_PERSISTENT );

            // now that the text bible loaded from the backend project is residing
            // in the persistend heap, there is no need to load any more instances
            // of the text bible from subsequent scrooby project loads
            //
            FeTextBibleLoader::SetUseLastLoadedTextBible( true );

            m_pBackendProject = pProject;

			// Create backend manager (for loading screens)
			//
			m_pManagerBackEnd = new CGuiManagerBackEnd( m_pProject, this );
			rAssert( m_pManagerBackEnd );

			// Populate screens	
			//
			m_pManagerBackEnd->Populate();

            m_pManagerBackEnd->Start();

            if( !CommandLineOptions::Get( CLO_SKIP_FE ) )
            {
                m_state = FRONTEND_LOADING_DURING_BOOTUP;
            }
            else
            {
                m_state = BOOTUP_ACTIVE;
            }

            HeapMgr()->PopHeap(GMA_PERSISTENT);

            break;
        }
        case FRONTEND_LOADING_DURING_BOOTUP:
        {
            m_state = BOOTUP_ACTIVE;

            // follow-through
        }
        case FRONTEND_LOADING:
        {
            HeapMgr()->PushHeap( GMA_LEVEL_FE );

			// Create FE screen manager
			//
			m_pManagerFrontEnd = new CGuiManagerFrontEnd( m_pProject, this );
			rAssert( m_pManagerFrontEnd );
			
			// Populate screens	
			//
			m_pManagerFrontEnd->Populate();

            // notify callback
            //
            if( m_pLoadingCallback != NULL )
            {
                m_pLoadingCallback->OnGuiLoadComplete( IGuiLoadingCallback::GUI_LOADED_FRONT_END );
            }

            HeapMgr()->PopHeap(GMA_LEVEL_FE);

            break;
        }
        case MINIGAME_LOADING:
        {
            HeapMgr()->PushHeap( GMA_LEVEL_HUD );

			// create mini-game manager
			//
			m_pManagerMiniGame = new CGuiManagerMiniGame( m_pProject, this );
			rAssert( m_pManagerMiniGame != NULL );
			
			// Populate screens	
			//
			m_pManagerMiniGame->Populate();

            // notify callback
            //
            if( m_pLoadingCallback != NULL )
            {
                m_pLoadingCallback->OnGuiLoadComplete( IGuiLoadingCallback::GUI_LOADED_MINI_GAME );
            }

            HeapMgr()->PopHeap( GMA_LEVEL_HUD );

            break;
        }
        case INGAME_LOADING:
        {
            HeapMgr()->PushHeap( GMA_LEVEL_HUD );

			// Create InGame manager
			//
#ifdef LOAD_LEVEL_SPECIFIC_PROJECT
            if( m_pLevelProject == NULL )
            {
                m_pLevelProject = m_pProject;
            }
            else
#endif // LOAD_LEVEL_SPECIFIC_PROJECT
            {
			    m_pManagerInGame = new CGuiManagerInGame( m_pProject, this );
			    rAssert( m_pManagerInGame );

			    // Populate screens	
			    //
			    m_pManagerInGame->Populate();

                // notify callback
                //
                if( m_pLoadingCallback != NULL )
                {
                    m_pLoadingCallback->OnGuiLoadComplete( IGuiLoadingCallback::GUI_LOADED_IN_GAME );
                }
            }

            HeapMgr()->PopHeap( GMA_LEVEL_HUD );

            break;
        }
        default:
        {
            rAssertMsg( false, "Invalid state!" );

            break;
        }
    }
MEMTRACK_POP_GROUP("CGUISystem");
}

//===========================================================================
// CGuiSystem::GotoScreen
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::GotoScreen( unsigned int screenID,
                             unsigned int param1,
                             unsigned int param2,
                             unsigned int windowOptions )
{
    CGuiManager* currentManager = this->GetCurrentManager();
    rAssert( currentManager != NULL );

    if( param1 != 0 || param2 != 0 )
    {
        currentManager->HandleMessage( GUI_MSG_SET_GOTO_SCREEN_PARAMETERS,
                                       param1, param2 );
    }

    currentManager->HandleMessage( GUI_MSG_GOTO_SCREEN, screenID, windowOptions );
}

//===========================================================================
// CGuiSystem::GetCurrentManager
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CGuiManager* CGuiSystem::GetCurrentManager() const
{
    CGuiManager* currentManager = NULL;

    switch( m_state )
    {
        case FRONTEND_ACTIVE:
        {
            currentManager = m_pManagerFrontEnd;
            
            break;
        }
        case MINIGAME_ACTIVE:
        {
            currentManager = m_pManagerMiniGame;
            
            break;
        }
        case INGAME_ACTIVE:
        {
            currentManager = m_pManagerInGame;

			break;
        }
        case BOOTUP_ACTIVE:
        case BACKEND_LOADING:
        case FRONTEND_LOADING_DURING_BOOTUP:
        {
            currentManager = m_pManagerBootUp;

            break;
        }
        case LANGUAGE_ACTIVE:
        {
            currentManager = m_pManagerLanguage;

            break;
        }
        case GUI_IDLE:
        case DEMO_ACTIVE:
        case FRONTEND_LOADING:
        case MINIGAME_LOADING:
        case INGAME_LOADING:
        {
            currentManager = m_pManagerBackEnd;

            break;
        }
        default:
        {
            break;
        }
    }

    return currentManager;
}

//===========================================================================
// CGuiSystem::RegisterUserInputHandlers
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::RegisterUserInputHandlers( int controllerIDs )
{
    for( int i = 0; i < m_numUserInputHandlers; i++ )
    {
        if( (controllerIDs & (1 << i)) > 0 )
        {
            GetInputManager()->RegisterMappable( i, m_pUserInputHandlers[ i ] );

            m_registeredUserInputHandlers |= (1 << i);
        }
    }
}

//===========================================================================
// CGuiSystem::UnregisterUserInputHandlers
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::UnregisterUserInputHandlers( int controllerIDs )
{
    for( int i = 0; i < m_numUserInputHandlers; i++ )
    {
        if( (controllerIDs & (1 << i)) > 0 )
        {
            GetInputManager()->UnregisterMappable( i, m_pUserInputHandlers[ i ] );

            m_registeredUserInputHandlers &= ~(1 << i);
        }
    }

    rAssertMsg( m_registeredUserInputHandlers == 0,
                "*** WARNING: Not all GUI user input handlers were un-registered!" );
}

//===========================================================================
// CGuiSystem::GetUserInputHandler
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CGuiUserInputHandler*
CGuiSystem::GetUserInputHandler( int controllerId ) const
{
    rAssert( controllerId >= 0 && controllerId < m_numUserInputHandlers );
    rAssert( m_pUserInputHandlers != NULL );

    if( (m_registeredUserInputHandlers & (1 << controllerId)) > 0 )
    {
        return m_pUserInputHandlers[ controllerId ];
    }
    else
    {
        return NULL;
    }
}

//===========================================================================
// CGuiSystem::LoadData
//===========================================================================
// Description: Load GUI data from memory card.
//
//===========================================================================
void
CGuiSystem::LoadData( const GameDataByte* dataBuffer, unsigned int numBytes )
{
    rAssert( dataBuffer != NULL && numBytes == 1 );
    m_isRadarEnabled = ( dataBuffer[ 0 ] != 0 );
}

//===========================================================================
// CGuiSystem::SaveData
//===========================================================================
// Description: Save GUI data to memory card.
//
//===========================================================================
void
CGuiSystem::SaveData( GameDataByte* dataBuffer, unsigned int numBytes )
{
    rAssert( dataBuffer != NULL && numBytes == 1 );
    dataBuffer[ 0 ] = m_isRadarEnabled ? 1 : 0;
}

//===========================================================================
// CGuiSystem::ResetData
//===========================================================================
// Description: Reset GUI data to defaults.
//
//===========================================================================
void
CGuiSystem::ResetData()
{
    m_isRadarEnabled = true;
}

//===========================================================================
// Private Member Functions
//===========================================================================

//===========================================================================
// CGuiSystem::OnInitBootUp
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnInitBootUp()
{
    char languageDir[ 16 ];
    languageDir[ 0 ] = '\0';

#ifdef PAL
    switch( CGuiTextBible::GetCurrentLanguage() )
    {
        case Scrooby::XL_FRENCH:
        {
            strcpy( languageDir, "french\\" );

            break;
        }
        case Scrooby::XL_GERMAN:
        {
            strcpy( languageDir, "german\\" );

            break;
        }
        case Scrooby::XL_SPANISH:
        {
            strcpy( languageDir, "spanish\\" );

            break;
        }
        default:
        {
            rAssert( CGuiTextBible::GetCurrentLanguage() == Scrooby::XL_ENGLISH );

            break;
        }
    }
#endif // PAL

    char licenseImageFile[ 256 ];
    sprintf( licenseImageFile,
#ifdef RAD_GAMECUBE
             "%s%slicenseG.p3d",
#endif
#ifdef RAD_PS2
             "%s%slicenseP.p3d",
#endif
#ifdef RAD_XBOX
             "%s%slicenseX.p3d",
#endif
#ifdef RAD_WIN32
             "%s%slicensePC.p3d",
#endif
             LICENSE_SCREEN_IMAGE_DIR,
             languageDir );

    //Load the mouse cursor
#ifdef RAD_WIN32
    char cursorImageFile[256];
    sprintf( cursorImageFile, "%smouse_cursor.p3d", MOUSE_CURSOR_DIR );

    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                     cursorImageFile, 
                                     GMA_PERSISTENT,
                                     "Default",
                                     "Default" );
#endif

    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                     licenseImageFile,
                                     GMA_LEVEL_FE,
                                     SCROOBY_INVENTORY_BOOTUP,
                                     SCROOBY_INVENTORY_BOOTUP );

    // load bootup Scrooby project (into FE heap)
    //
    GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                     PROJECT_FILE_BOOTUP,
                                     GMA_LEVEL_FE,
                                     SCROOBY_INVENTORY_BOOTUP,
                                     SCROOBY_INVENTORY_BOOTUP );

    // load backend Scrooby project (into PERSISTENT heap)
    //
    GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                     PROJECT_FILE_BACKEND,
                                     GMA_PERSISTENT,        // This is because the loading screen always stays around
                                     SCROOBY_INVENTORY_BACKEND,
                                     SCROOBY_INVENTORY_BACKEND );

    if( !CommandLineOptions::Get( CLO_SKIP_FE ) )
    {
        // load frontend Scrooby project (into FE heap)
        //
        GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                         PROJECT_FILE_FRONTEND,
                                         GMA_LEVEL_FE,
                                         SCROOBY_INVENTORY_FRONTEND,
                                         SCROOBY_INVENTORY_FRONTEND );
    }

    m_state = BOOTUP_LOADING;
}

//===========================================================================
// CGuiSystem::OnReleaseBootUp
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnReleaseBootUp()
{
    HeapMgr()->PushHeap( GMA_LEVEL_FE );

    Scrooby::Project* pScroobyProject = NULL;

    if( m_pManagerLanguage != NULL )
    {
        pScroobyProject = m_pManagerLanguage->GetScroobyProject();

        // destroy language manager
        //
        delete m_pManagerLanguage;
        m_pManagerLanguage = NULL;

        if( pScroobyProject != NULL )
        {
            m_pApp->UnloadProject( pScroobyProject );
        }
    }

    if( m_pManagerBootUp != NULL )
    {
        pScroobyProject = m_pManagerBootUp->GetScroobyProject();

        // destroy bootup manager
        delete m_pManagerBootUp;
        m_pManagerBootUp = NULL;

        if( pScroobyProject != NULL )
        {
            m_pApp->UnloadProject( pScroobyProject );
        }
    }

    // update reference to text bible
    //
    rAssert( m_pTextBible != NULL );
    m_pTextBible->SetTextBible( TEXT_BIBLE_NAME );

    HeapMgr()->PopHeap(GMA_LEVEL_FE);

#ifdef PAL
    this->FormatTutorialTextWithLineBreaks();
#endif

    m_state = GUI_IDLE;
}

//===========================================================================
// CGuiSystem::OnInitFrontEnd
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnInitFrontEnd()
{
    // load frontend Scrooby project (into FE heap)
    GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                     PROJECT_FILE_FRONTEND,
                                     GMA_LEVEL_FE,
                                     SCROOBY_INVENTORY_FRONTEND,
                                     SCROOBY_INVENTORY_FRONTEND );

    m_state = FRONTEND_LOADING;
}

//===========================================================================
// CGuiSystem::OnReleaseFrontEnd
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnReleaseFrontEnd()
{
    HeapMgr()->PushHeap( GMA_LEVEL_FE );

    if( m_pManagerFrontEnd != NULL )
    {
        // destroy frontend manager
        delete m_pManagerFrontEnd;
        m_pManagerFrontEnd = NULL;
    }

    // unload frontend project resources
    if( m_pProject != NULL )
    {
        m_pApp->UnloadProject( m_pProject );
        m_pProject = NULL;
    }

/*
#ifdef RAD_GAMECUBE
    // disable screen scaling (for GameCube)
    m_pApp->EnableScreenScaling( false );
#endif

#ifdef RAD_PS2
    // disable screen scaling (for PS2)
    m_pApp->EnableScreenScaling( false );
#endif
*/

    HeapMgr()->PopHeap(GMA_LEVEL_FE);

    m_state = GUI_IDLE;
}

//===========================================================================
// CGuiSystem::OnInitMiniGame
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnInitMiniGame()
{
    // load mini-game Scrooby project
    //
    GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                     PROJECT_FILE_MINIGAME,
                                     GMA_LEVEL_HUD,
                                     SCROOBY_INVENTORY_MINIGAME,
                                     SCROOBY_INVENTORY_MINIGAME );

    // load 3D characters
    //
//    CGuiManagerMiniGame::LoadCharacters();

    m_state = MINIGAME_LOADING;
}

//===========================================================================
// CGuiSystem::OnReleaseMiniGame
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnReleaseMiniGame()
{
    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    // unload 3D characters
    //
//    CGuiManagerMiniGame::UnloadCharacters();

    // destroy MiniGame manager
    //
    if( m_pManagerMiniGame != NULL )
    {
        delete m_pManagerMiniGame;
        m_pManagerMiniGame = NULL;
    }

    // unload mini-game project resources
    //
    if( m_pProject != NULL )
    {
        m_pApp->UnloadProject( m_pProject );
        m_pProject = NULL;
    }

    HeapMgr()->PopHeap( GMA_LEVEL_HUD );

    m_state = GUI_IDLE;
}

//===========================================================================
// CGuiSystem::OnInitInGame
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnInitInGame()
{
    int currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
    rAssert( currentLevel >= 0 );

    // substitute levels > 7 with level 1
    //
    if( static_cast<unsigned int>( currentLevel ) >= sizeof( INGAME_LEVEL_PROJECT_FILES ) /
                                                     sizeof( INGAME_LEVEL_PROJECT_FILES[ 0 ] ) )
    {
        currentLevel = 0;
    }

#ifdef LOAD_LEVEL_SPECIFIC_PROJECT
    // load level-specific ingame Scrooby project (into HUD heap)
    GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                     INGAME_LEVEL_PROJECT_FILES[ currentLevel ],
                                     GMA_LEVEL_HUD,
                                     SCROOBY_INVENTORY_INGAME_LEVEL,
                                     SCROOBY_INVENTORY_INGAME_LEVEL );
#endif // LOAD_LEVEL_SPECIFIC_PROJECT

    // load ingame Scrooby project (into HUD heap)
    GetLoadingManager()->AddRequest( FILEHANDLER_SCROOBY,
                                     PROJECT_FILE_INGAME,
                                     GMA_LEVEL_HUD,
                                     SCROOBY_INVENTORY_INGAME,
                                     SCROOBY_INVENTORY_INGAME );

    rAssert( m_pApp );
    m_pApp->GetResourceManager().SetSecondaryInventorySection( SCROOBY_INVENTORY_INGAME_LEVEL );

    m_state = INGAME_LOADING;
}


//===========================================================================
// CGuiSystem::OnReleaseInGame
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::OnReleaseInGame()
{
    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    if( m_pManagerInGame != NULL )
    {
        // destroy ingame manager
        delete m_pManagerInGame;
        m_pManagerInGame = NULL;
    }

    // unload ingame project resources
    if( m_pProject != NULL )
    {
        m_pApp->UnloadProject( m_pProject );
        m_pProject = NULL;
    }

    // as well as level-specific ingame resources
    if( m_pLevelProject != NULL )
    {
        m_pApp->UnloadProject( m_pLevelProject );
        m_pLevelProject = NULL;
    }

    // update reference to text bible
    //
    rAssert( m_pTextBible != NULL );
    m_pTextBible->SetTextBible( TEXT_BIBLE_NAME );

    rAssert( m_pApp );
    m_pApp->GetResourceManager().SetSecondaryInventorySection( NULL );

    HeapMgr()->PopHeap(GMA_LEVEL_HUD);

    m_state = GUI_IDLE;
}


//===========================================================================
// CGuiSystem::FormatTutorialTextWithLineBreaks
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiSystem::FormatTutorialTextWithLineBreaks()
{
#ifdef PAL
    const int MAX_NUM_CHARS_PER_LINE = 35;
    const P3D_UNICODE TEXT_LINE_BREAK = '\\';

    for( int tutorialID = 0; tutorialID < TUTORIAL_MAX; tutorialID++ )
    {
        char textBibleID[ 32 ];
        sprintf( textBibleID, "TUTORIAL_%03d", tutorialID );
        P3D_UNICODE* tutorialText = GetTextBibleString( textBibleID );

        // check for platform-specific text; if found, override default text
        //
        P3D_UNICODE* platformText = NULL;

#ifdef RAD_GAMECUBE
        strcat( textBibleID, "_(GC)" );
        platformText = GetTextBibleString( textBibleID );
#endif
#ifdef RAD_PS2
        strcat( textBibleID, "_(PS2)" );
        platformText = GetTextBibleString( textBibleID );
#endif
#ifdef RAD_XBOX
        strcat( textBibleID, "_(XBOX)" );
        platformText = GetTextBibleString( textBibleID );
#endif
        if( platformText != NULL )
        {
            tutorialText = platformText;
        }

        // wrap tutorial text by inserting line breaks into text
        //
        int numCharsInCurrentLine = 0;
        int lastSpaceCharIndex = -1;

#ifdef RAD_DEBUG
        const int MAX_NUM_LINES_PER_MESSAGE = 5;
        int numLineBreaks = 0;
#endif

        int tutorialTextLength = p3d::UnicodeStrLen( tutorialText );
        for( int i = 0; i < tutorialTextLength; i++ )
        {
            if( tutorialText[ i ] == TEXT_LINE_BREAK ) // line break character
            {
#ifdef RAD_DEBUG
                numLineBreaks++;
                rAssert( numLineBreaks < MAX_NUM_LINES_PER_MESSAGE );
#endif
                numCharsInCurrentLine = 0;

                continue;
            }

            if( tutorialText[ i ] == ' ' ) // space character
            {
                lastSpaceCharIndex = i;
            }

            numCharsInCurrentLine++;

            if( numCharsInCurrentLine > MAX_NUM_CHARS_PER_LINE )
            {
                rAssertMsg( lastSpaceCharIndex != -1, "We might encounter this w/ Japanese text!" );

                // replace last space character w/ line break
                //
                tutorialText[ lastSpaceCharIndex ] = TEXT_LINE_BREAK;
                numCharsInCurrentLine = i - lastSpaceCharIndex;

#ifdef RAD_DEBUG
                numLineBreaks++;
                rAssertMsg( numLineBreaks < MAX_NUM_LINES_PER_MESSAGE, "Too many lines in tutorial message! Tell Tony." );
#endif

                rAssertMsg( numCharsInCurrentLine <= MAX_NUM_CHARS_PER_LINE, "We might encounter this w/ Japanese text!" );
            }
        }
    }
#endif // PAL
}


int CGuiSystem::GetPrimaryController() const
{
    return m_primaryController;
}

void CGuiSystem::SetPrimaryController(int id)
{
    m_primaryController = id;
}


#ifdef DEBUGWATCH

void CGuiSystem::RegisterWatcherStuff()
{
    static bool s_watcherRegistered = false;

    if( s_watcherRegistered )
    {
        // delete previously registered stuff
        //
        radDbgWatchDelete( &g_wGuiSimulationTimeFactor );
        radDbgWatchDelete( &g_wGuiMessage );
        radDbgWatchDelete( &g_wGuiMessageParam1 );
        radDbgWatchDelete( &g_wGuiMessageParam2 );
        radDbgWatchDelete( &(Scrooby::g_CameraNearPlane) );
        radDbgWatchDelete( &(Scrooby::g_CameraFarPlane) );
/*
#ifdef RAD_PS2
        radDbgWatchDelete( &g_ps2screenScale );
#endif
*/
        radDbgWatchDelete( &g_wScroobyPageName );
        radDbgWatchDelete( &g_wScroobyPosX );
        radDbgWatchDelete( &g_wScroobyPosY );
        radDbgWatchDelete( &g_wScroobyAlpha );
        radDbgWatchDelete( &g_wScroobyTextName );
        radDbgWatchDelete( &g_wScroobySpriteName );
        radDbgWatchDelete( &g_wScroobyPolygonName );
    }

    // and re-register again
    //
    radDbgWatchAddFloat( &g_wGuiSimulationTimeFactor,
                         "GUI Simulation Time Factor",
                         GUI_WATCHER_NAMESPACE,
                         NULL,
                         NULL,
                         0.0f,
                         2.0f );

    radDbgWatchAddInt( &g_wGuiMessage,
                       "GUI Message",
                       GUI_WATCHER_NAMESPACE,
                       (RADDEBUGWATCH_CALLBACK)SendMsgToGuiSystem );

    radDbgWatchAddInt( &g_wGuiMessageParam1,
                       "GUI Message Param1",
                       GUI_WATCHER_NAMESPACE );

    radDbgWatchAddInt( &g_wGuiMessageParam2,
                       "GUI Message Param2",
                       GUI_WATCHER_NAMESPACE );

    radDbgWatchAddFloat( &(Scrooby::g_CameraNearPlane),
                         "Override Camera Near Plane",
                         GUI_WATCHER_NAMESPACE,
                         NULL, NULL, 0.01f, 1.0f );

    radDbgWatchAddFloat( &(Scrooby::g_CameraFarPlane),
                         "Override Camera Far Plane",
                         GUI_WATCHER_NAMESPACE,
                         NULL, NULL, 10.0f, 1000.0f );

    radDbgWatchAddFunction( "Toggle Next Language",
                            (RADDEBUGWATCH_CALLBACK)ToggleNextLanguage,
                            NULL,
                            GUI_WATCHER_NAMESPACE );
/*
#ifdef RAD_PS2
    radDbgWatchAddFloat( &g_ps2screenScale,
                         "PS2 Screen Scale",
                         GUI_WATCHER_NAMESPACE,
                         (RADDEBUGWATCH_CALLBACK)UpdatePS2ScreenScale,
                         NULL, 0.5f, 1.5f );
#endif
*/
    radDbgWatchAddString( g_wScroobyPageName,
                          sizeof( g_wScroobyPageName ),
                          "Scrooby Page",
                          WATCHER_NAMESPACE_SCROOBY,
                          (RADDEBUGWATCH_CALLBACK)SetCurrentScroobyPage );

    radDbgWatchAddInt( &g_wScroobyPosX,
                       "X-Position",
                       WATCHER_NAMESPACE_SCROOBY,
                       (RADDEBUGWATCH_CALLBACK)SetScroobyDrawablePosition,
                       NULL,
                       0,
                       (int)Scrooby::App::GetInstance()->GetScreenWidth() );

    radDbgWatchAddInt( &g_wScroobyPosY,
                       "Y-Position",
                       WATCHER_NAMESPACE_SCROOBY,
                       (RADDEBUGWATCH_CALLBACK)SetScroobyDrawablePosition,
                       NULL,
                       0,
                       (int)Scrooby::App::GetInstance()->GetScreenHeight() );

    radDbgWatchAddFloat( &g_wScroobyAlpha,
                         "Alpha",
                         WATCHER_NAMESPACE_SCROOBY,
                         (RADDEBUGWATCH_CALLBACK)SetScroobyDrawableAlpha );

    radDbgWatchAddString( g_wScroobyTextName,
                          sizeof( g_wScroobyTextName ),
                          "Scrooby Text",
                          WATCHER_NAMESPACE_SCROOBY,
                          (RADDEBUGWATCH_CALLBACK)SetCurrentScroobyDrawableAsText );

    radDbgWatchAddString( g_wScroobySpriteName,
                          sizeof( g_wScroobySpriteName ),
                          "Scrooby Sprite",
                          WATCHER_NAMESPACE_SCROOBY,
                          (RADDEBUGWATCH_CALLBACK)SetCurrentScroobyDrawableAsSprite );

    radDbgWatchAddString( g_wScroobyPolygonName,
                          sizeof( g_wScroobyPolygonName ),
                          "Scrooby Polygon",
                          WATCHER_NAMESPACE_SCROOBY,
                          (RADDEBUGWATCH_CALLBACK)SetCurrentScroobyDrawableAsPolygon );

    s_watcherRegistered = true;
}

#endif // DEBUGWATCH
