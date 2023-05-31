//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMainMenu
//
// Description: Implementation of the CGuiScreenMainMenu class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/11/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenmainmenu.h>
#include <presentation/gui/frontend/guiscreenplaymovie.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiscreenmemorycard.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guiscreenprompt.h>

#include <constants/movienames.h>
#include <data/gamedatamanager.h>
#include <data/persistentworldmanager.h>
#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>

#include <p3d/view.hpp>

#include <raddebug.hpp>     // Foundation
#include <radtime.hpp>
#include <radkey.hpp>
#include <radmath/random.hpp>
#include <p3d/anim/multicontroller.hpp>

#include <stdlib.h>
#include <group.h>
#include <layer.h>
#include <page.h>
#include <pure3dobject.h>
#include <screen.h>
#include <text.h>

#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//#define PLAY_GAGS_SEQUENTIALLY
//#define PLAY_ONLY_HOMER_GAGS

const float FIRST_TIME_FADE_IN_TIME = 500.0f;

const unsigned int MIN_GAGS_CYCLE_TIME = 15; // in seconds
const unsigned int MAX_GAGS_CYCLE_TIME = 30; // in seconds
const unsigned int GAGS_RANDOM_NUMBER_MODULO = 97;

const tColour FE_BLUE_SKY_COLOUR( 128, 255, 255 );

struct HomerGagAnimation
{
    float startFrame;
    float endFrame;
};

HomerGagAnimation HOMER_GAG_ANIMATION[] =
{
    {   0.0f,  75.0f },
    {  75.0f, 145.0f },
    { 145.0f, 215.0f },
    { 215.0f, 355.0f },
    { 355.0f, 475.0f },

    { -1.0f, -1.0f } // dummy terminator
};

int NUM_HOMER_GAG_ANIMATIONS =
    sizeof( HOMER_GAG_ANIMATION ) / sizeof( HOMER_GAG_ANIMATION[ 0 ] ) - 1;

const radKey32 FE_GAGS[] =
{
    ::radMakeKey32( "FE_Gag_Homer" ),
    ::radMakeKey32( "FE_Gag_Grandpa" ),
    ::radMakeKey32( "FE_Gag_Moleman" ),
    ::radMakeKey32( "FE_Gag_Frink" ),
    ::radMakeKey32( "FE_Gag_Barney" ),
    ::radMakeKey32( "FE_Gag_Nick" ),
    ::radMakeKey32( "FE_Gag_Snake" ),
    ::radMakeKey32( "FE_Gag_Maggie" ),

    ::radMakeKey32( "FE_Gag_UNKNOWN" )
};

const radKey32 FE_GAGS_FOR_HOMER[] =
{
    ::radMakeKey32( "FE_Gag_Homer_ScratchHead" ),
    ::radMakeKey32( "FE_Gag_Homer_ScratchBum" ),
    ::radMakeKey32( "FE_Gag_Homer_Yawn" ),
    ::radMakeKey32( "FE_Gag_Homer_Nightmare" ),
    ::radMakeKey32( "FE_Gag_Homer_Stretch" ),

    ::radMakeKey32( "FE_Gag_UNKNOWN" )
};

enum eMainMenuItem
{
    MENU_ITEM_MAIN_MENU,
/*
    MENU_ITEM_NEW_GAME,
    MENU_ITEM_RESUME_GAME,
    MENU_ITEM_LOAD_GAME,
    MENU_ITEM_COLLECTIBLE_CARDS,
    MENU_ITEM_OPTIONS,
*/

#ifdef SRR2_LEVEL_SELECTION
    MENU_ITEM_LEVEL,
#endif

    NUM_MAIN_MENU_ITEMS
};

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMainMenu::CGuiScreenMainMenu
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
CGuiScreenMainMenu::CGuiScreenMainMenu
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:
	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MAIN_MENU ),
    m_pMenu( NULL ),
    m_gags( NULL ),
    m_currentGagIndex( -1 ),
    m_nextGagIndex( -1 ),
    m_nextHomerGagIndex( -1 ),
    m_gagsElapsedTime( 0 ),
    m_gagsCycleTime( 1000 * MIN_GAGS_CYCLE_TIME ),
    m_homer( NULL ),
    m_raceCar( NULL ),
    m_homerIdleAnim( NULL ),
    m_tvFrame( NULL )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenMainMenu" );
    memset( m_glowingItems, 0, sizeof( m_glowingItems ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "MainMenu" );
    rAssert( pPage != NULL );

/*
    // get accept button icon
    //
    if( m_buttonIcons[ BUTTON_ICON_ACCEPT ] == NULL )
    {
        m_buttonIcons[ BUTTON_ICON_ACCEPT ] = pPage->GetGroup( "AcceptLabel" );
        rAssert( m_buttonIcons[ BUTTON_ICON_ACCEPT ] != NULL );
    }
*/

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this, NUM_MAIN_MENU_ITEMS, GUI_TEXT_MENU );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    Scrooby::Text* otherMainMenu = NULL;

#ifdef RAD_WIN32
    m_pMenu->AddMenuItem( pPage->GetText( "MainMenu_PC" ),
                          pPage->GetText( "MainMenu_PC" ),
                          NULL,
                          NULL,
                          pPage->GetSprite( "MainMenu_LArrow" ),
                          pPage->GetSprite( "MainMenu_RArrow" ) );

    otherMainMenu = pPage->GetText( "MainMenu" );
#else
    m_pMenu->AddMenuItem( pPage->GetText( "MainMenu" ),
                          pPage->GetText( "MainMenu" ),
                          NULL,
                          NULL,
                          pPage->GetSprite( "MainMenu_LArrow" ),
                          pPage->GetSprite( "MainMenu_RArrow" ) );

    otherMainMenu = pPage->GetText( "MainMenu_PC" );
#endif

    if( otherMainMenu != NULL )
    {
        otherMainMenu->SetVisible( false );
    }

    // wrap main menu text
    //
    Scrooby::Text* mainMenuText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_ITEM_MAIN_MENU )->GetItem() );
    rAssert( mainMenuText != NULL );
    mainMenuText->SetTextMode( Scrooby::TEXT_WRAP );

#ifdef RAD_DEMO
    m_pMenu->SetSelectionValueCount( MENU_ITEM_MAIN_MENU, NUM_MAIN_MENU_SELECTIONS );
    m_pMenu->SetSelectionValue( MENU_ITEM_MAIN_MENU, MAIN_MENU_PLAY_LEVEL_2 );

    HeapMgr()->PushHeap( GMA_LEVEL_FE );
    mainMenuText->SetString( MAIN_MENU_PLAY_LEVEL_2, "PLAY LEVEL 2" ); // TC: [TODO] localization??
    mainMenuText->SetString( MAIN_MENU_PLAY_LEVEL_7, "PLAY LEVEL 7" ); // TC: [TODO] localization??
    HeapMgr()->PopHeap( GMA_LEVEL_FE );
#endif

    // scale up main menu text a bit
    //
//    m_pMenu->GetMenuItem( MENU_ITEM_MAIN_MENU )->GetItem()->ScaleAboutCenter( 1.2f );

    Scrooby::Page* levelPage = m_pScroobyScreen->GetPage( "Level" );
    if( levelPage != NULL )
    {
#ifdef SRR2_LEVEL_SELECTION
        m_pMenu->AddMenuItem( levelPage->GetText( "Level" ),
                              levelPage->GetText( "Level" ),
                              NULL,
                              NULL,
                              NULL,
                              NULL,
                              SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );

    #ifndef RAD_DEBUG
        // hide level/car selection menu by default
        // in Tune and Release builds
        //
        this->ToggleLevelMenu();
    #endif
#else
        Scrooby::Group* levelMenu = levelPage->GetGroup( "Menu" );
        rAssert( levelMenu != NULL );
        levelMenu->SetVisible( false );
#endif
    }

    // Retrieve drawing elements from 3dFE page
    //
	pPage = m_pScroobyScreen->GetPage( "3dFE" );
	rAssert( pPage != NULL );
    m_homer = pPage->GetPure3dObject( "Homer" );
    m_raceCar = pPage->GetPure3dObject( "RaceCar" );

    // get glowing items
    //
#ifdef RAD_DEMO
    m_glowingItems[ MAIN_MENU_PLAY_LEVEL_2 ] = pPage->GetPure3dObject( "Glow4" );
    m_glowingItems[ MAIN_MENU_PLAY_LEVEL_7 ] = pPage->GetPure3dObject( "Glow4" );
    m_glowingItems[ MAIN_MENU_OPTIONS ] = pPage->GetPure3dObject( "Glow2" );

    Scrooby::Pure3dObject* glowingItem = NULL;

    glowingItem = pPage->GetPure3dObject( "Glow0" );
    rAssert( glowingItem != NULL );
    glowingItem->SetVisible( false );

    glowingItem = pPage->GetPure3dObject( "Glow1" );
    rAssert( glowingItem != NULL );
    glowingItem->SetVisible( false );

    glowingItem = pPage->GetPure3dObject( "Glow3" );
    rAssert( glowingItem != NULL );
    glowingItem->SetVisible( false );

    glowingItem = pPage->GetPure3dObject( "Glow5" );
    rAssert( glowingItem != NULL );
    glowingItem->SetVisible( false );
#else
    for( int i = 0; i < NUM_MAIN_MENU_SELECTIONS; i++ )
    {
        char name[ 16 ];
        sprintf( name, "Glow%d", i );
        m_glowingItems[ i ] = pPage->GetPure3dObject( name );
    }
#endif

    this->TurnOnGlowItems( 0 ); // all off, by default

    // 3D FE Gags
    //
    m_gags = m_pScroobyScreen->GetPage( "3dFEGags" );
    rAssert( m_gags );

    // Retrieve drawing elements from TVFrame page
    //
	pPage = m_pScroobyScreen->GetPage( "TVFrame" );
	rAssert( pPage );
    m_tvFrame = pPage->GetLayer( "TVFrame" );

    // correct TV frame proportion and size on GC and PS2 and pc
    //
    Scrooby::Sprite* tvFrame = pPage->GetSprite( "TVFrame" );
    if( tvFrame != NULL )
    {
        tvFrame->ResetTransformation();

#ifdef RAD_GAMECUBE
        tvFrame->ScaleAboutCenter( 1.1f, 1.0f, 1.0f );
#endif

#ifdef RAD_PS2
        tvFrame->ScaleAboutCenter( 1.07f );
#endif

#ifdef RAD_WIN32
        tvFrame->ScaleAboutCenter( 1.03f );
#endif
    }
MEMTRACK_POP_GROUP( "CGUIScreenMainMenu" );
}


//===========================================================================
// CGuiScreenMainMenu::~CGuiScreenMainMenu
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
CGuiScreenMainMenu::~CGuiScreenMainMenu()
{
    if( m_nextGagIndex == -1 )
    {
        // stop any FE gag dialog that was triggered and may still be playing
        //
        GetEventManager()->TriggerEvent( EVENT_FE_GAG_STOP );
    }

    if( m_homerIdleAnim != NULL )
    {
        m_homerIdleAnim->Release();
        m_homerIdleAnim = NULL;
    }

    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }

    // restore black clear colour
    //
    GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearColour( tColour( 0, 0, 0 ) );
}


//===========================================================================
// CGuiScreenMainMenu::HandleMessage
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
void CGuiScreenMainMenu::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
#ifndef RAD_WIN32
        rAssert( param1 == PROMPT_CONFIRM_NEW_GAME );
#endif

        if( param1 == PROMPT_CONFIRM_NEW_GAME )
        {            
            switch( param2 )
            {
                case CGuiMenuPrompt::RESPONSE_YES:
                {
#ifdef SRR2_LEVEL_SELECTION
                    int level = m_pMenu->GetSelectionValue( MENU_ITEM_LEVEL );
                    this->OnNewGameSelected( static_cast<RenderEnums::LevelEnum>( level ) );
#else
                    this->OnNewGameSelected();
#endif // SRR2_LEVEL_SELECTION

                    CGuiScreen* pScreen = static_cast<CGuiScreen*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_GENERIC_PROMPT ) );
                    rAssert( pScreen != NULL );
                    pScreen->StartTransitionAnimation( 0, 53 );

                    break;
                }
                case CGuiMenuPrompt::RESPONSE_NO:
                {
                    this->ReloadScreen();

                    break;
                }
                default:
                {
                    rAssertMsg( false, "Invalid prompt response!" );

                    break;
                }
            }
        }
#ifdef RAD_WIN32
        else if( param1 == PROMPT_CONFIRM_QUIT )
        {
            switch( param2 )
            {
                case (CGuiMenuPrompt::RESPONSE_YES):
                {
                    // send message to front-end manager to quit front-end and
                    // exit the game.
                    //
                    m_pParent->HandleMessage( GUI_MSG_QUIT_GAME );

                    break;
                }

                case (CGuiMenuPrompt::RESPONSE_NO):
                {
                    this->ReloadScreen();

                    break;
                }

                default:
                {
                    rAssertMsg( 0, "WARNING: *** Invalid prompt response!\n" );

                    break;
                }
            }
        }
#endif
    }

    if( m_firstTimeEntered &&
        message == GUI_MSG_WINDOW_ENTER )
    {
        if( CGuiScreenIntroTransition::s_introTransitionPlayed )
        {
            this->SetFadingEnabled( false );
        }
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                this->UpdateRunning( param1 );

                break;
            }
#ifdef SRR2_LEVEL_SELECTION
            // show/hide level and car selection menu (for Tune and Release)
            //
            case GUI_MSG_CONTROLLER_L1:
            case GUI_MSG_CONTROLLER_R1:
            {
                this->ToggleLevelMenu();

                break;
            }
            case GUI_MSG_CONTROLLER_START:
            {
                if( m_pMenu->GetSelection() == MENU_ITEM_LEVEL )
                {
                    rAssert( m_pMenu != NULL );
                    m_pMenu->Reset();
                    m_pMenu->SetSelectionValue( 0, MAIN_MENU_NEW_GAME );
                }

                break;
            }
#endif
            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                if( param1 == MENU_ITEM_MAIN_MENU )
                {
                    // turn on new glow item
                    //
                    this->TurnOnGlowItems( (1 << param2) );

#ifndef RAD_DEMO
                    if( param2 == MAIN_MENU_MINI_GAME ) // special case for mini-game
                    {
                        if( !GetCharacterSheetManager()->IsMiniGameUnlocked() &&
                            !CommandLineOptions::Get( CLO_SKIP_FE ) )
                        {
                            // don't turn on glowing track
                            //
                            this->TurnOnGlowItems( 0 );

                            // grey out selection
                            //
                            m_pMenu->GetMenuItem( MENU_ITEM_MAIN_MENU )->GetItemValue()->SetColour( tColour( 128, 128, 128 ) );

                            // hide accept button icon
                            //
                            this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );
#ifdef RAD_WIN32
                            GetInputManager()->GetFEMouse()->SetClickable( false );
#endif
                            
                        }
                    }
                    else
                    {
                        // restore menu selection colour and accept button icon
                        //
                        tColour menuHighlightColour = m_pMenu->GetHighlightColour();
                        m_pMenu->GetMenuItem( MENU_ITEM_MAIN_MENU )->GetItemValue()->SetColour( menuHighlightColour );

                        this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );
#ifdef RAD_WIN32
                        GetInputManager()->GetFEMouse()->SetClickable( true );
#endif
                    }
#endif // !RAD_DEMO
                }

                break;
            }
            case GUI_MSG_CONTROLLER_SELECT:
            {
                if( !this->IsButtonVisible( BUTTON_ICON_ACCEPT ) )
                {
                    return;
                }
#ifdef RAD_DEMO
                if( m_pMenu->GetSelectionValue( MENU_ITEM_MAIN_MENU ) == MAIN_MENU_PLAY_LEVEL_2 ||
                    m_pMenu->GetSelectionValue( MENU_ITEM_MAIN_MENU ) == MAIN_MENU_PLAY_LEVEL_7 )
#else
                if( m_pMenu->GetSelectionValue( MENU_ITEM_MAIN_MENU ) == MAIN_MENU_NEW_GAME ||
                    m_pMenu->GetSelectionValue( MENU_ITEM_MAIN_MENU ) == MAIN_MENU_RESUME_GAME )
#endif
                {
                    GetInputManager()->RegisterControllerID( 0, param1 );
                    GetGuiSystem()->SetPrimaryController(param1);
                }

#ifndef RAD_DEMO
                if( m_pMenu->GetSelectionValue( MENU_ITEM_MAIN_MENU ) == MAIN_MENU_MINI_GAME )
                {
                    GetGuiSystem()->SetPrimaryController(param1);
                }
#endif // !RAD_DEMO

                break;
            }
#ifdef RAD_WIN32
            case GUI_MSG_CONTROLLER_BACK:
            {
                this->OnQuitGameSelected();

                break;
            }
#endif
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                rAssert( param1 == MENU_ITEM_MAIN_MENU );
                rAssert( m_pMenu != NULL );
                int menuSelection = m_pMenu->GetSelectionValue( param1 );

                switch( menuSelection )
                {
#ifdef RAD_DEMO
                    case MAIN_MENU_PLAY_LEVEL_2:
                    {
                        this->OnNewGameSelected( RenderEnums::L2, RenderEnums::M1 );

                        break;
                    }
                    case MAIN_MENU_PLAY_LEVEL_7:
                    {
                        this->OnNewGameSelected( RenderEnums::L7, RenderEnums::M1 );

                        break;
                    }
#else
                    case MAIN_MENU_NEW_GAME:
                    {
                        if( GetGameDataManager()->IsGameLoaded() )
                        {
                            // prompt user to confirm first
                            //
                            m_guiManager->DisplayPrompt( PROMPT_CONFIRM_NEW_GAME, this );
                        }
                        else
                        {
#ifdef SRR2_LEVEL_SELECTION
                            int level = m_pMenu->GetSelectionValue( MENU_ITEM_LEVEL );
                            this->OnNewGameSelected( static_cast<RenderEnums::LevelEnum>( level ) );
#else
                            this->OnNewGameSelected();
#endif // SRR2_LEVEL_SELECTION
                        }

                        break;
                    }
                    case MAIN_MENU_RESUME_GAME:
                    {
                        this->OnResumeGameSelected();

                        break;
                    }
                    case MAIN_MENU_MINI_GAME:
                    {
                        this->OnMiniGameSelected();

                        break;
                    }
                    case MAIN_MENU_LOAD_GAME:
                    {
#ifdef RAD_XBOX
                        // Xbox TCR Requirement: always prompt user to select memory
                        // device before loading/saving
                        //
                        CGuiScreenLoadSave::s_forceGotoMemoryCardScreen = true;
#endif // RAD_XBOX
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_LOAD_GAME );

                        this->StartTransitionAnimation( 200, 230 );

                        break;
                    }
                    case MAIN_MENU_CARD_GALLERY:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_SCRAP_BOOK );

                        this->StartTransitionAnimation( 320, 350 );

                        break;
                    }
#endif // RAD_DEMO
                    case MAIN_MENU_OPTIONS:
                    {
                        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_OPTIONS );

                        this->StartTransitionAnimation( 80, 110 );

                        break;
                    }
#ifdef RAD_WIN32
                    case MAIN_MENU_QUIT_GAME:
                    {
                        this->OnQuitGameSelected();

                        break;
                    }
#endif
                    default:
                    {
                        rAssertMsg( false, "Invalid main menu selection!" );

                        break;
                    }
                }

                break;
            }
            default:
            {
                break;
            }
        }

        if( m_pMenu != NULL )
        {
            // relay message to menu
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }
    else if( m_state == GUI_WINDOW_STATE_INTRO )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                this->UpdateIntro( param1 );

                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                this->UpdateOutro( param1 );

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
	CGuiScreen::HandleMessage( message, param1, param2 );
}

void
CGuiScreenMainMenu::InitMenu()
{
#ifndef RAD_DEMO
    bool isGameLoaded = GetGameDataManager()->IsGameLoaded();

    rAssert( m_pMenu != NULL );
    m_pMenu->SetSelectionValueCount( MENU_ITEM_MAIN_MENU,
                                     isGameLoaded ? NUM_MAIN_MENU_SELECTIONS : NUM_MAIN_MENU_SELECTIONS - 1 );

    m_pMenu->SetSelectionValue( MENU_ITEM_MAIN_MENU,
                                isGameLoaded ? MAIN_MENU_RESUME_GAME : MAIN_MENU_NEW_GAME );
#endif // !RAD_DEMO
}

//===========================================================================
// CGuiScreenMainMenu::InitIntro
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
void CGuiScreenMainMenu::InitIntro()
{
    if( m_firstTimeEntered )
    {
        this->SetFadeTime( FIRST_TIME_FADE_IN_TIME );
        this->InitMenu();

        // set clear colour to a blue sky colour
        //
        GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearColour( FE_BLUE_SKY_COLOUR );
    }
}

//===========================================================================
// CGuiScreenMainMenu::InitRunning
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
void CGuiScreenMainMenu::InitRunning()
{
    if( m_firstTimeEntered )
    {
        this->SetFadingEnabled( true );
        this->RestoreDefaultFadeTime();

        m_screenCover = NULL;
    }

    // turn on current glow item
    //
    rAssert( m_pMenu != NULL );
    this->TurnOnGlowItems( (1 << m_pMenu->GetSelectionValue( MENU_ITEM_MAIN_MENU )) );
}

//===========================================================================
// CGuiScreenMainMenu::InitOutro
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
void CGuiScreenMainMenu::InitOutro()
{
    if( m_nextGagIndex == -1 )
    {
        // stop any FE gag dialog that was triggered and may still be playing
        //
        GetEventManager()->TriggerEvent( EVENT_FE_GAG_STOP );
    }

    // turn off all glow items
    //
    this->TurnOnGlowItems( 0 );
}

//===========================================================================
// CGuiScreenMainMenu::UpdateIntro
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
void CGuiScreenMainMenu::UpdateIntro( unsigned int elapsedTime )
{
    if( m_firstTimeEntered )
    {
        // hide duplicate homer
        //
        if( m_gags != NULL )
        {
            Scrooby::Pure3dObject* p3dGag = m_gags->GetPure3dObject( "Gag0" );
            rAssert( p3dGag );
            if( p3dGag->GetMultiController() != NULL )
            {
                p3dGag->SetVisible( false );
            }
        }
    }
}

//===========================================================================
// CGuiScreenMainMenu::UpdateRunning
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
void CGuiScreenMainMenu::UpdateRunning( unsigned int elapsedTime )
{
    GetMemoryCardManager()->Update( elapsedTime );
    this->UpdateGags( elapsedTime );
}

//===========================================================================
// CGuiScreenMainMenu::UpdateOutro
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
void CGuiScreenMainMenu::UpdateOutro( unsigned int elapsedTime )
{
    int currentMenuSelection = m_pMenu->GetSelectionValue( MENU_ITEM_MAIN_MENU );

#ifdef RAD_DEMO
    if( currentMenuSelection == MAIN_MENU_PLAY_LEVEL_2 ||
        currentMenuSelection == MAIN_MENU_PLAY_LEVEL_7 )
#else
    if( currentMenuSelection == MAIN_MENU_NEW_GAME ||
        currentMenuSelection == MAIN_MENU_RESUME_GAME ||
        currentMenuSelection == MAIN_MENU_MINI_GAME )
#endif
    {
        rAssert( m_p3dObject != NULL );

        tMultiController* multiController = m_p3dObject->GetMultiController();
        if( multiController != NULL )
        {
            const float NUM_FADE_OUT_FRAMES = 15.0f;
            float currentFrame = multiController->GetFrame();
            float numRemainingFrames = multiController->GetNumFrames() - currentFrame;

            if( numRemainingFrames < NUM_FADE_OUT_FRAMES )
            {
                // fade out TV frame
                //
                if( m_tvFrame != NULL )
                {
                    float alpha = numRemainingFrames / NUM_FADE_OUT_FRAMES;

                    // decrease fade rate for low alpha values
                    alpha *= alpha;

                    if( alpha > 0.0f && alpha < 1.0f )
                    {
                        m_tvFrame->SetAlpha( alpha );
                    }
                    else
                    {
                        m_tvFrame->SetAlpha( 0.0f );
                    }
                }

                // TC [HACK]: To prevent any clipping in homer's mouth
                //            in the last few frames.
                //
                if( numRemainingFrames < 1.0f )
                {
                    this->RestoreScreenCover();
                }
            }

#ifndef RAD_DEMO
            if( currentMenuSelection == MAIN_MENU_MINI_GAME )
            {
                const float NUM_RACE_CAR_FRAMES_FOR_BLENDING = 11.0f;

                rAssert( m_raceCar != NULL );
                if( currentFrame < 1.0f )
                {
                    // get number of frames remaining in race car animation
                    //
                    tMultiController* raceMultiController = m_raceCar->GetMultiController();
                    rAssert( raceMultiController != NULL );

                    int currentRaceFrame = (int)raceMultiController->GetFrame() % (int)raceMultiController->GetNumFrames();
                    float numRemainingRaceFrames = raceMultiController->GetNumFrames() - currentRaceFrame;
                    raceMultiController->SetRelativeSpeed( numRemainingRaceFrames / NUM_RACE_CAR_FRAMES_FOR_BLENDING );
                }
                else if( currentFrame > NUM_RACE_CAR_FRAMES_FOR_BLENDING )
                {
                    // hide duplicate race car
                    //
                    m_raceCar->SetVisible( false );
                }
            }
#endif // !RAD_DEMO
        }
    }
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

//===========================================================================
// CGuiScreenMainMenu::UpdateGags
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
void CGuiScreenMainMenu::UpdateGags( unsigned int elapsedTime )
{
    static float NUM_BLEND_FRAMES = 10.0f;

    // if the homer gag animation is running
    //
    if( m_homerIdleAnim != NULL && m_homer != NULL )
    {
        // check if homer gag animation is done
        //
        tMultiController* homerMultiController = m_homer->GetMultiController();
        if( homerMultiController != NULL )
        {
            tAnimationFrameController* homerFrameController =
                (tAnimationFrameController*)homerMultiController->GetTrack( 0 );
            rAssert( homerFrameController );

            if( homerFrameController->LastFrameReached() > 0 )
            {
                // ok, last frame has been reached on homer gag animation;
                // restore the homer idle animation
                //
                if( m_homerIdleAnim != NULL )
                {
                    homerFrameController->SetAnimation( m_homerIdleAnim, 0.0f, 0.0f );

                    m_homerIdleAnim->Release();
                    m_homerIdleAnim = NULL;
                }

                // restore homer idle animation frame range
                //
                homerMultiController->SetFrameRange( 0.0f, 60.0f );
                homerMultiController->Reset();
            }
        }
    }

    // advance gag timer
    //
    m_gagsElapsedTime += elapsedTime;

    const unsigned int FE_GAG_PREP_TIME = 2000; // in msec
    if( m_gagsElapsedTime > (m_gagsCycleTime - FE_GAG_PREP_TIME) )
    {
        if( m_nextGagIndex == -1 )
        {
            // prepare for next gag
            //
#ifdef PLAY_ONLY_HOMER_GAGS
            m_nextGagIndex = 0;
#else
    #ifdef PLAY_GAGS_SEQUENTIALLY
            m_nextGagIndex = (m_currentGagIndex + 1) % m_gags->GetNumberOfLayers();
    #else
            int random = (rand() / 10) % GAGS_RANDOM_NUMBER_MODULO;
            m_nextGagIndex = random % m_gags->GetNumberOfLayers();

            if( CommandLineOptions::Get( CLO_FE_GAGS_TEST ) )
            {
                rReleasePrintf( "Next FE Gag Index = %d (Random Number = %d)\n", m_nextGagIndex, random );
            }
    #endif
#endif
            if( m_nextGagIndex != 0 )
            {
                GetEventManager()->TriggerEvent( EVENT_FE_GAG_INIT, reinterpret_cast<void*>( FE_GAGS[ m_nextGagIndex ] ) );
            }
            else // multiple homer gags
            {
                // pick a random homer animation
                //
#ifdef PLAY_GAGS_SEQUENTIALLY
                m_nextHomerGagIndex = (m_nextHomerGagIndex + 1) % NUM_HOMER_GAG_ANIMATIONS;
#else
                int random = (rand() / 10) % GAGS_RANDOM_NUMBER_MODULO;
                m_nextHomerGagIndex = random % NUM_HOMER_GAG_ANIMATIONS;

                if( CommandLineOptions::Get( CLO_FE_GAGS_TEST ) )
                {
                    rReleasePrintf( "  Next Homer Gag Index = %d (Random Number = %d)\n", m_nextHomerGagIndex, random );
                }
#endif

                GetEventManager()->TriggerEvent( EVENT_FE_GAG_INIT, reinterpret_cast<void*>( FE_GAGS_FOR_HOMER[ m_nextHomerGagIndex ] ) );
            }
        }
    }

    if( m_gagsElapsedTime > m_gagsCycleTime )
    {
        if( m_gags != NULL )
        {
            Scrooby::Layer* currentGagLayer = NULL;

            // hide previous gag layer, if exists
            //
            if( m_currentGagIndex != -1 )
            {
                currentGagLayer = m_gags->GetLayerByIndex( m_currentGagIndex );
                currentGagLayer->SetVisible( false );
            }

            m_currentGagIndex = m_nextGagIndex;
            m_nextGagIndex = -1;

            // get the pure3d gag
            //
            char objectName[ 8 ];
            sprintf( objectName, "Gag%d", m_currentGagIndex );
            Scrooby::Pure3dObject* p3dGag = m_gags->GetPure3dObject( objectName );

            // reset gag animation
            //
            rAssert( p3dGag );
            tMultiController* multiController =  p3dGag->GetMultiController();
            if( multiController != NULL )
            {
                multiController->Reset();
            }

            // show current gag layer
            //
            currentGagLayer = m_gags->GetLayerByIndex( m_currentGagIndex );
            rAssert( currentGagLayer );
            currentGagLayer->SetVisible( true );

            // start FE gag dialog
            //
            GetEventManager()->TriggerEvent( EVENT_FE_GAG_START );

            if( m_currentGagIndex == 0 )
            {
                // special case for homer gag animation
                //
                if( m_homer != NULL && multiController != NULL )
                {
                    tAnimationFrameController* frameController =
                        (tAnimationFrameController*)multiController->GetTrack( 0 );
                    rAssert( frameController );

                    // get homer gag animation
                    tAnimation* homerAnim = frameController->GetAnimation();
                    rAssert( homerAnim );
                    homerAnim->SetCyclic( false ); // don't cycle when done

                    tMultiController* homerMultiController = m_homer->GetMultiController();
                    if( homerMultiController != NULL )
                    {
                        homerMultiController->SetFrameRange( HOMER_GAG_ANIMATION[ m_nextHomerGagIndex ].startFrame,
                                                             HOMER_GAG_ANIMATION[ m_nextHomerGagIndex ].endFrame );

                        tAnimationFrameController* homerFrameController =
                            (tAnimationFrameController*)homerMultiController->GetTrack( 0 );
                        rAssert( homerFrameController );

                        // save reference to homer idle animation
                        //
                        m_homerIdleAnim = homerFrameController->GetAnimation();
                        rAssert( m_homerIdleAnim != NULL );
                        m_homerIdleAnim->AddRef();

                        // switch to the homer gag animation
                        homerFrameController->SetAnimation( homerAnim, 0.0f, NUM_BLEND_FRAMES );
                    }
                }
            }
        }

        // set new random gag cycle time
        //
        if( CommandLineOptions::Get( CLO_FE_GAGS_TEST ) )
        {
            m_gagsCycleTime = 1000 * MIN_GAGS_CYCLE_TIME;
        }
        else
        {
            m_gagsCycleTime = 1000 * (MIN_GAGS_CYCLE_TIME + rand() % (MAX_GAGS_CYCLE_TIME - MIN_GAGS_CYCLE_TIME + 1));
        }

        // reset gag timer
        //
        m_gagsElapsedTime = 0;
    }
}

//===========================================================================
// CGuiScreenMainMenu::StopHomerIdleAnimation
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
void CGuiScreenMainMenu::StopHomerIdleAnimation()
{
/*
    if( m_homer != NULL )
    {
        // reset Homer's snoring animation so that the camera
        // can pan right into his mouth
        //
        tMultiController* multiController = m_homer->GetMultiController();
        if( multiController != NULL )
        {
            tAnimationFrameController* frameController = (tAnimationFrameController*)multiController->GetTrack( 0 );
            rAssert( frameController );

            if( m_homerIdleAnim == NULL )
            {
                m_homerIdleAnim = frameController->GetAnimation();
                rAssert( m_homerIdleAnim );
            }

            m_homerIdleAnim->SetCyclic( false );

            static float NUM_BLEND_FRAMES = 10.0f;
            frameController->SetAnimation( m_homerIdleAnim,
                                           m_homerIdleAnim->GetNumFrames() - NUM_BLEND_FRAMES,
                                           NUM_BLEND_FRAMES );

            m_homerIdleAnim = NULL;
        }
    }
*/
}

//===========================================================================
// CGuiScreenMainMenu::TurnOnGlowItems
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	items - bitmask representing which ones to turn on glowing
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMainMenu::TurnOnGlowItems( unsigned int items )
{
#ifdef RAD_DEMO
    if( items > 0 )
    {
        bool isOptionsSelected = ( (items & (1 << MAIN_MENU_OPTIONS)) > 0 );
        m_glowingItems[ MAIN_MENU_PLAY_LEVEL_2 ]->SetVisible( !isOptionsSelected );
        m_glowingItems[ MAIN_MENU_OPTIONS ]->SetVisible( isOptionsSelected );

        return;
    }
#endif

#ifdef RAD_WIN32
    for( int i = 0; i < NUM_MAIN_MENU_SELECTIONS; i++ )
    {
        bool isOn = (items & (1 << i)) > 0;
        int which = i < MAIN_MENU_QUIT_GAME ? i : i - 1;
        if( m_glowingItems[ which ] != NULL && i != MAIN_MENU_QUIT_GAME )
        {
            m_glowingItems[ which ]->SetVisible( isOn );
        }
    }
#else
    for( int i = 0; i < NUM_MAIN_MENU_SELECTIONS; i++ )
    {
        bool isOn = (items & (1 << i)) > 0;
        if( m_glowingItems[ i ] != NULL )
        {
            m_glowingItems[ i ]->SetVisible( isOn );
        }
    }
#endif
}

void
CGuiScreenMainMenu::OnNewGameSelected( RenderEnums::LevelEnum level,
                                       RenderEnums::MissionEnum mission )
{
    bool showNewGameMovie = false;

#ifdef RAD_RELEASE
  #ifndef RAD_DEMO
    showNewGameMovie = !CommandLineOptions::Get( CLO_SKIP_MOVIE );
  #endif
#endif

    if( showNewGameMovie )
    {
        // play new game movie
        //
        rAssert( m_guiManager );
        CGuiScreenPlayMovie* playMovieScreen = static_cast<CGuiScreenPlayMovie*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME ) );
        rAssert( playMovieScreen );

        playMovieScreen->SetMovieToPlay( MovieNames::MOVIE1, false, true );

        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME );
    }
    else
    {
        // send message to front-end manager to quit front-end and
        // start single player story mode
        //
        m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 1 ); // 1 = one player
    }

    this->StartTransitionAnimation( 0, 53 );

    this->StopHomerIdleAnimation();

    if( GetGameDataManager()->IsGameLoaded() )
    {
        // reset all game data
        //
        GetGameDataManager()->ResetGame();

        //reset the rewards manager and relock the rewards
        //
        GetRewardsManager()->ClearRewards();
    }

    GetCharacterSheetManager()->SetCurrentMission( level, mission );

    // restore black clear colour
    //
    GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearColour( tColour( 0, 0, 0 ) );
}

void
CGuiScreenMainMenu::OnResumeGameSelected()
{
    // send message to front-end manager to quit front-end and
    // start single player story mode
    //
    m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 1 ); // 1 = one player

    this->StartTransitionAnimation( 140, 170 );
}

void
CGuiScreenMainMenu::OnMiniGameSelected()
{
    // send message to front-end manager to quit front-end and
    // start mini-game mode
    //
    m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 2 ); // 2 = mini-game

    this->StartTransitionAnimation( 880, 913 );
}

#ifdef RAD_WIN32
void
CGuiScreenMainMenu::OnQuitGameSelected()
{
    rAssert( m_guiManager != NULL );
    m_guiManager->DisplayPrompt( PROMPT_CONFIRM_QUIT, this );
}
#endif

#ifdef SRR2_LEVEL_SELECTION
void
CGuiScreenMainMenu::ToggleLevelMenu()
{
    Scrooby::Page* levelPage = m_pScroobyScreen->GetPage( "Level" );
    if( levelPage != NULL )
    {
        Scrooby::Group* levelMenu = levelPage->GetGroup( "Menu" );
        rAssert( levelMenu );

        bool toggle = !levelMenu->IsVisible();

        // display menu on/off
        levelMenu->SetVisible( toggle );

        // enable/disable menu selections
        for( int i = MENU_ITEM_LEVEL; i <= MENU_ITEM_LEVEL; i++ )
        {
            m_pMenu->SetMenuItemEnabled( i, toggle );
        }
    }
}
#endif // SRR2_LEVEL_SELECTION

//===========================================================================
// Public Member Functions (for CGuiScreenIntroTransition)
//===========================================================================

bool CGuiScreenIntroTransition::s_introTransitionPlayed = false;

//===========================================================================
// CGuiScreenIntroTransition::CGuiScreenIntroTransition
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
CGuiScreenIntroTransition::CGuiScreenIntroTransition
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_INTRO_TRANSITION ),
    m_introState( WAITING_FOR_MULTICONTROLLER ),
    m_homer( NULL ),
    m_tvFrame( NULL ),
    m_gags( NULL )
{
    this->SetFadingEnabled( false );

    // Retrieve drawing elements from 3dFE page
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "3dFE" );
    rAssert( pPage );
    m_homer = pPage->GetPure3dObject( "Homer" );

    // Retrieve drawing elements from TVFrame page
    //
    pPage = m_pScroobyScreen->GetPage( "TVFrame" );
    rAssert( pPage );
    m_tvFrame = pPage->GetLayer( "TVFrame" );

    // 3D FE Gags
    //
    m_gags = m_pScroobyScreen->GetPage( "3dFEGags" );
    rAssert( m_gags );

    s_introTransitionPlayed = false;
}


//===========================================================================
// CGuiScreenIntroTransition::~CGuiScreenIntroTransition
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
CGuiScreenIntroTransition::~CGuiScreenIntroTransition()
{
}


//===========================================================================
// CGuiScreenIntroTransition::HandleMessage
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
void CGuiScreenIntroTransition::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );

    if( m_state == GUI_WINDOW_STATE_INTRO &&
        message == GUI_MSG_UPDATE )
    {
        switch( m_introState )
        {
            case WAITING_FOR_MULTICONTROLLER:
            {
                if( s_p3dMultiController != NULL )
                {
                    m_numTransitionsPending--;

                    this->StartTransitionAnimation( 721, 770, false );

                    if( m_screenCover != NULL )
                    {
                        m_screenCover->SetVisible( false );
                    }

                    m_introState = RUNNING_INTRO_ANIMATION;
                }
                else
                {
                    if( m_screenCover != NULL )
                    {
                        m_screenCover->SetAlpha( 1.0f );
                    }
                }

                break;
            }

            default:
            {
                if( m_tvFrame != NULL )
                {
                    rAssert( m_p3dObject );

                    tMultiController* multiController = m_p3dObject->GetMultiController();
                    rAssert( multiController );

                    const float NUM_FADE_IN_FRAMES = 20.0f;
                    float numRemainingFrames = multiController->GetNumFrames() -
                                               multiController->GetFrame();

                    if( numRemainingFrames < NUM_FADE_IN_FRAMES )
                    {
                        float alpha = 1.0f - numRemainingFrames / NUM_FADE_IN_FRAMES;

                        // decrease fade rate for low alpha values
                        alpha *= alpha;

                        if( alpha < 1.0f )
                        {
                            // fade in TV frame
                            //
                            if( m_tvFrame != NULL )
                            {
                                m_tvFrame->SetAlpha( alpha );
                            }

                            // fade in foreground layers
                            //
                            this->SetAlphaForLayers( alpha,
                                                     m_foregroundLayers,
                                                     m_numForegroundLayers );
                        }
                    }
                }

                break;
            }
        }

        // hide duplicate homer
        //
        if( m_gags != NULL )
        {
            Scrooby::Pure3dObject* p3dGag = m_gags->GetPure3dObject( "Gag0" );
            rAssert( p3dGag );
            if( p3dGag->GetMultiController() != NULL )
            {
                p3dGag->SetVisible( false );
            }
        }
    }
}


//===========================================================================
// CGuiScreenIntroTransition::InitIntro
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
void CGuiScreenIntroTransition::InitIntro()
{
    // wait for retrieval of pure3d multicontroller
    m_numTransitionsPending++;

    if( m_tvFrame != NULL )
    {
        m_tvFrame->SetAlpha( 0.0f );

        this->SetAlphaForLayers( 0.0f,
                                 m_foregroundLayers,
                                 m_numForegroundLayers );
    }

    CGuiScreenMainMenu* pScreen = static_cast<CGuiScreenMainMenu*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_MAIN_MENU ) );
    rAssert( pScreen != NULL );
    pScreen->InitMenu();
}


//===========================================================================
// CGuiScreenIntroTransition::InitRunning
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
void CGuiScreenIntroTransition::InitRunning()
{
    s_introTransitionPlayed = true;

    if( m_tvFrame != NULL )
    {
        m_tvFrame->SetAlpha( 1.0f );

        this->SetAlphaForLayers( 1.0f,
                                 m_foregroundLayers,
                                 m_numForegroundLayers );
    }

    // goto main menu
    //
    rAssert( m_pParent );
    m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                              GUI_SCREEN_ID_MAIN_MENU,
                              CLEAR_WINDOW_HISTORY );
}


//===========================================================================
// CGuiScreenIntroTransition::InitOutro
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
void CGuiScreenIntroTransition::InitOutro()
{
}

