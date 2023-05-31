//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenScrapBookContents
//
// Description: Implementation of the CGuiScreenScrapBookContents class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/10      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenscrapbookcontents.h>
#include <presentation/gui/frontend/guiscreenplaymovie.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiuserinputhandler.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guitextbible.h>

#include <cards/cardgallery.h>
#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <render/enums/renderenums.h>

// Scrooby
//
#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <text.h>
#include <sprite.h>

// ATG
//
#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* MOVIES_INGAME[] =
{
    MovieNames::MOVIE2, // level 1
    MovieNames::MOVIE3, // level 2
    MovieNames::MOVIE8, // level 3 - I & S movie!
    MovieNames::MOVIE4, // level 4
    MovieNames::MOVIE5, // level 5
    MovieNames::MOVIE6, // level 6
    MovieNames::MOVIE7, // level 7

    "" // dummy terminator
};

const float LEVEL_BAR_CORRECTION_SCALE = 2.0f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenScrapBookContents::CGuiScreenScrapBookContents
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
CGuiScreenScrapBookContents::CGuiScreenScrapBookContents
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ),
    m_pMenu( NULL ),
    m_levelBarGroup( NULL ),
    m_levelSelection( NULL ),
    m_LTrigger( NULL ),
    m_RTrigger( NULL ),
    m_LTriggerBgd( NULL ),
    m_RTriggerBgd( NULL ),
    m_currentLevel( 0 ),
    m_sparkles( NULL )
{
    memset( m_menuImages, 0, sizeof( m_menuImages ) );

    // Retrieve the Scrooby drawing elements (from LevelBar.pag)
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "LevelBar" );
	rAssert( pPage != NULL );

    m_levelBarGroup = pPage->GetGroup( "LevelBar" );
    rAssert( m_levelBarGroup != NULL );

    // apply image correction scale
    //
    Scrooby::Sprite* levelBarBgd = m_levelBarGroup->GetSprite( "LevelBar" );
    if( levelBarBgd != NULL )
    {
        levelBarBgd->ResetTransformation();
        levelBarBgd->ScaleAboutCenter( LEVEL_BAR_CORRECTION_SCALE );
    }

    // get level bar elements
    //
    m_levelSelection = m_levelBarGroup->GetText( "Level" );
    rAssert( m_levelSelection != NULL );
    m_levelSelection->ResetTransformation();
    m_levelSelection->ScaleAboutCenter( 0.75f );

    m_LTrigger = m_levelBarGroup->GetGroup( "LTrigger" );
    rAssert( m_LTrigger != NULL );

    m_RTrigger = m_levelBarGroup->GetGroup( "RTrigger" );
    rAssert( m_RTrigger != NULL );

#ifdef RAD_PS2
    // PS2 only, scale up L1 and R1 button images
    //
    m_LTrigger->ResetTransformation();
    m_LTrigger->ScaleAboutCenter( 1.25f, 1.15f, 1.0f );
    m_RTrigger->ResetTransformation();
    m_RTrigger->ScaleAboutCenter( 1.25f, 1.15f, 1.0f );
#endif

#ifdef RAD_XBOX
    switch( CGuiTextBible::GetCurrentLanguage() )
    {
        case Scrooby::XL_FRENCH:
        {
            m_LTrigger->GetSprite( "LTrigger" )->SetIndex( 2 );
            m_RTrigger->GetSprite( "RTrigger" )->SetIndex( 1 );

            break;
        }
        case Scrooby::XL_SPANISH:
        {
            m_LTrigger->GetSprite( "LTrigger" )->SetIndex( 1 );
            m_RTrigger->GetSprite( "RTrigger" )->SetIndex( 1 );

            break;
        }
        default:
        {
            break;
        }
    }
#endif // RAD_XBOX

    m_LTriggerBgd = m_LTrigger->GetSprite( "LTriggerBgd" );
    m_RTriggerBgd = m_RTrigger->GetSprite( "RTriggerBgd" );

#ifdef RAD_WIN32
    m_LTriggerBgd->ScaleAboutCenter( 0.5 );
    m_RTriggerBgd->ScaleAboutCenter( 0.5 );
#endif

    // Retrieve the Scrooby drawing elements (from ScrapBookContents.pag)
    //
    pPage = m_pScroobyScreen->GetPage( "ScrapBookContents" );
	rAssert( pPage != NULL );

    // create a 2D menu
    //
    m_pMenu = new CGuiMenu2D( this, NUM_MENU_ITEMS, 3 );
    rAssert( m_pMenu != NULL );

    // add menu items
    //
    Scrooby::Group* pGroup = pPage->GetGroup( "Menu" );
    rAssert( pGroup != NULL );
    m_pMenu->AddMenuItem( pGroup->GetText( "StoryMissions" ),
                          pGroup->GetText( "StoryMissions_Unlocked" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "CharacterClothing" ),
                          pGroup->GetText( "CharacterClothing_Unlocked" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "Vehicles" ),
                          pGroup->GetText( "Vehicles_Unlocked" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "Dummy" ),
                          NULL, NULL, NULL, NULL, NULL, ALL_ATTRIBUTES_OFF );
    m_pMenu->AddMenuItem( pGroup->GetText( "CollectorCards" ),
                          pGroup->GetText( "CollectorCards_Unlocked" ) );
    m_pMenu->AddMenuItem( pGroup->GetText( "Dummy" ),
                          NULL, NULL, NULL, NULL, NULL, ALL_ATTRIBUTES_OFF );
//    m_pMenu->AddMenuItem( pGroup->GetText( "Movies" ),
//                          pGroup->GetText( "Movies_Unlocked" ) );

    // get menu images
    //
    m_menuImages[ MENU_STORY_MISSIONS ] = pGroup->GetSprite( "StoryMissions" );
    m_menuImages[ MENU_CHARACTER_CLOTHING ] = pGroup->GetSprite( "CharacterClothing" );
    m_menuImages[ MENU_VEHICLES ] = pGroup->GetSprite( "Vehicles" );
    m_menuImages[ MENU_COLLECTOR_CARDS ] = pGroup->GetSprite( "CollectorCards" );
//    m_menuImages[ MENU_MOVIES ] = pGroup->GetSprite( "Movies" );

    // shrink down bottom row images
    //
    const float MENU_IMAGE_SCALE = 0.75f;
    rAssert( m_menuImages[ MENU_COLLECTOR_CARDS ] != NULL );
    m_menuImages[ MENU_COLLECTOR_CARDS ]->ResetTransformation();
    m_menuImages[ MENU_COLLECTOR_CARDS ]->ScaleAboutCenter( MENU_IMAGE_SCALE );

    // get sparkles overlay
    //
    pPage = m_pScroobyScreen->GetPage( "3dFE" );
	rAssert( pPage != NULL );
    m_sparkles = pPage->GetLayer( "ScrapBookSparkles" );
}

//===========================================================================
// CGuiScreenScrapBookContents::~CGuiScreenScrapBookContents
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
CGuiScreenScrapBookContents::~CGuiScreenScrapBookContents()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}

#ifdef RAD_WIN32
//===========================================================================
// CGuiScreenScrapBookContents::CheckCursorAgainstHotspots
//===========================================================================
// Description: Checks cursor position against its list of hotspots.
//
// Constraints: None.
//
// Parameters:  float x - The x position of cursor in P3D coordinates.
//              float y - The y position of cursor in P3D coordinates.
//
// Return:      N/A.
//
//===========================================================================
eFEHotspotType CGuiScreenScrapBookContents::CheckCursorAgainstHotspots( float x, float y )
{
    eFEHotspotType hotSpotType = CGuiScreen::CheckCursorAgainstHotspots( x, y );
    if( hotSpotType == HOTSPOT_NONE )
    {
        if( m_LTriggerBgd )
        {
            if( m_LTriggerBgd->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_LTRIGGER;
            }
        }
        if( m_RTriggerBgd )
        {        
            if( m_RTriggerBgd->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_RTRIGGER;
            }
        }

    }
    return hotSpotType;
}
#endif

//===========================================================================
// CGuiScreenScrapBookContents::HandleMessage
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
void CGuiScreenScrapBookContents::HandleMessage( eGuiMessage message,
                                                 unsigned int param1,
                                                 unsigned int param2 )
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                // toggle L/R trigger icon backgrounds
                //
                if( m_LTriggerBgd != NULL && m_RTriggerBgd != NULL )
                {
                    m_LTriggerBgd->SetIndex( 0 );
                    m_RTriggerBgd->SetIndex( 0 );

                    if( !m_pMenu->HasSelectionBeenMade() )
                    {
                        int numUserInputHandlers = GetGuiSystem()->GetNumUserInputHandlers();
                        for( int i = 0; i < numUserInputHandlers; i++ )
                        {
                            CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( i );
                            if( userInputHandler != NULL )
                            {
                                if( userInputHandler->IsButtonDown( GuiInput::L1 )
#ifdef RAD_WIN32
                                    || GetInputManager()->GetFEMouse()->LeftButtonDownOn() == HOTSPOT_LTRIGGER
#endif  
                                    )
                                {
                                    rAssert( m_LTriggerBgd->GetNumOfImages() > 1 );
                                    m_LTriggerBgd->SetIndex( 1 );
                                }

                                if( userInputHandler->IsButtonDown( GuiInput::R1 )
#ifdef RAD_WIN32
                                    || GetInputManager()->GetFEMouse()->LeftButtonDownOn() == HOTSPOT_RTRIGGER
#endif  
                                    )
                                {
                                    rAssert( m_RTriggerBgd->GetNumOfImages() > 1 );
                                    m_RTriggerBgd->SetIndex( 1 );
                                }
                            }
                        }
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_L1:
            {
                if( !m_pMenu->HasSelectionBeenMade() )
                {
                    // decrement level selection
                    //
                    this->OnLevelSelectionChange( -1 );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                }

                break;
            }
            case GUI_MSG_CONTROLLER_R1:
            {
                if( !m_pMenu->HasSelectionBeenMade() )
                {
                    // increment level selection
                    //
                    this->OnLevelSelectionChange( +1 );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                }

                break;
            }
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                this->OnMenuSelectionMade( static_cast<eMenuItem>( param1 ) );

                break;
            }
/*
            case GUI_MSG_CONTROLLER_SELECT:
            {
                if( m_currentLevel == RenderEnums::L3 &&
                    m_pMenu->GetSelection() == MENU_MOVIES )
                {
                    GetInputManager()->RegisterControllerID( 0, param1 );
                }

                break;
            }
*/
            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 830, 850 );

                // hide sparkles
                //
                if( m_sparkles != NULL )
                {
                    m_sparkles->SetVisible( false );
                }

                break;
            }
            default:
            {
                break;
            }
        }

        // relay message to menu
        //
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenScrapBookContents::SetLevelBarVisible
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
void CGuiScreenScrapBookContents::SetLevelBarVisible( bool isVisible )
{
    rAssert( m_levelBarGroup != NULL );
    m_levelBarGroup->SetVisible( isVisible );
}

//===========================================================================
// CGuiScreenScrapBookContents::InitIntro
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
void CGuiScreenScrapBookContents::InitIntro()
{
    // this is needed here to update all the unlocked n/N values
    //
    this->OnLevelSelectionChange( 0 );

    // show L/R trigger buttons
    //
    rAssert( m_LTrigger != NULL && m_RTrigger != NULL );
    m_LTrigger->SetVisible( true );
    m_RTrigger->SetVisible( true );
}

//===========================================================================
// CGuiScreenScrapBookContents::InitRunning
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
void CGuiScreenScrapBookContents::InitRunning()
{
    // show sparkles only if game percent complete is 100%
    //
    float percentComplete = GetCharacterSheetManager()->QueryPercentGameCompleted();
    if( m_sparkles != NULL )
    {
        // TC: [TODO] verify sparkles look OK
        //
//        m_sparkles->SetVisible( percentComplete == 100.0f );
    }
}

//===========================================================================
// CGuiScreenScrapBookContents::InitOutro
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
void CGuiScreenScrapBookContents::InitOutro()
{
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenScrapBookContents::OnLevelSelectionChange( int delta )
{
    m_currentLevel = (m_currentLevel + RenderEnums::numLevels + delta) % RenderEnums::numLevels;
    rAssert( m_levelSelection );

    // update level selection bar
    //
    m_levelSelection->SetIndex( m_currentLevel );

    // update menu images and unlocked values
    //
    for( int i = 0; i < NUM_MENU_ITEMS; i++ )
    {
        if( m_menuImages[ i ] != NULL )
        {
            m_menuImages[ i ]->SetIndex( m_currentLevel );
        }
    }

    // update unlocked values
    //
    HeapMgr()->PushHeap( GMA_LEVEL_FE );

    char buffer[ 8 ];
    Scrooby::Text* unlockedText = NULL;
    rAssert( m_pMenu != NULL );

    // missions
    int numMissionsCompleted = GetCharacterSheetManager()->QueryNumMissionsCompleted( static_cast<RenderEnums::LevelEnum>( m_currentLevel ) );
    if( GetCharacterSheetManager()->QueryBonusMissionCompleted( static_cast<RenderEnums::LevelEnum>( m_currentLevel ) ) )
    {
        numMissionsCompleted++;
    }

    sprintf( buffer, "%d / %d",
             numMissionsCompleted,
             8 ); // m_currentLevel == 6 ? 5 : 7 );
    unlockedText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_STORY_MISSIONS )->GetItemValue() );
    rAssert( unlockedText != NULL );
    unlockedText->SetString( 0, buffer );

    // clothing
    sprintf( buffer, "%d / %d",
             GetCharacterSheetManager()->QueryNumSkinsUnlocked( static_cast<RenderEnums::LevelEnum>( m_currentLevel ) ),
             3 );
    unlockedText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_CHARACTER_CLOTHING )->GetItemValue() );
    rAssert( unlockedText != NULL );
    unlockedText->SetString( 0, buffer );

    // vehicles
    int numVehiclesUnlocked = GetCharacterSheetManager()->QueryNumCarUnlocked( static_cast<RenderEnums::LevelEnum>( m_currentLevel ) );
    if( GetRewardsManager()->GetReward( m_currentLevel, Reward::eDefaultCar )->RewardStatus() )
    {
        numVehiclesUnlocked++;
    }
    sprintf( buffer, "%d / %d",
             numVehiclesUnlocked,
             6 );
    unlockedText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_VEHICLES )->GetItemValue() );
    rAssert( unlockedText != NULL );
    unlockedText->SetString( 0, buffer );

    // cards
    sprintf( buffer, "%d / %d",
             GetCharacterSheetManager()->QueryNumCardsCollected( static_cast<RenderEnums::LevelEnum>( m_currentLevel ) ),
             NUM_CARDS_PER_LEVEL );
    unlockedText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_COLLECTOR_CARDS )->GetItemValue() );
    rAssert( unlockedText != NULL );
    unlockedText->SetString( 0, buffer );

/*
    // movies
    bool isMovieUnlocked = GetCharacterSheetManager()->QueryFMVUnlocked( static_cast<RenderEnums::LevelEnum>( m_currentLevel ) );
    if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_MOVIES ) )
    {
        isMovieUnlocked = true;
    }

    sprintf( buffer, "%d / %d",
             isMovieUnlocked ? 1 : 0,
             1 );
    unlockedText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_MOVIES )->GetItemValue() );
    rAssert( unlockedText != NULL );
    unlockedText->SetString( 0, buffer );

    m_pMenu->SetMenuItemEnabled( MENU_MOVIES, isMovieUnlocked );
*/

    HeapMgr()->PopHeap( GMA_LEVEL_FE );
}

void
CGuiScreenScrapBookContents::OnMenuSelectionMade( eMenuItem selection )
{
    switch( selection )
    {
        case MENU_STORY_MISSIONS:
        {
            m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_MISSION_GALLERY );

            break;
        }
        case MENU_CHARACTER_CLOTHING:
        {
            m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_SKIN_GALLERY );

            break;
        }
        case MENU_VEHICLES:
        {
            m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_VEHICLE_GALLERY );

            break;
        }
        case MENU_COLLECTOR_CARDS:
        {
            m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_CARD_GALLERY );

            break;
        }
/*
        case MENU_MOVIES:
        {
            if( m_currentLevel != RenderEnums::L3 )
            {
                rAssert( m_guiManager );
                CGuiScreenPlayMovie* playMovieScreen = static_cast<CGuiScreenPlayMovie*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_PLAY_MOVIE ) );
                rAssert( playMovieScreen );

                rAssert( MOVIES_INGAME[ m_currentLevel ] != "" );
                playMovieScreen->SetMovieToPlay( MOVIES_INGAME[ m_currentLevel ] );

                m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_PLAY_MOVIE );
            }
            else // special case for level 3
            {
                // TC: [TODO] Need to find out from D.Evenson which mission in L3 to start
                //
                GetCharacterSheetManager()->SetCurrentMission( static_cast<RenderEnums::LevelEnum>( m_currentLevel ),
                                                               static_cast<RenderEnums::MissionEnum>( RenderEnums::M1 ) );

                // send message to front-end manager to quit front-end and
                // start single player story mode
                //
                m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 1 ); // 1 = one player
            }

            break;
        }
*/
        default:
        {
            rAssertMsg( false, "Invalid menu selection!" );

            break;
        }
    }

    // hide L/R trigger buttons
    //
    rAssert( m_LTrigger != NULL && m_RTrigger != NULL );
    m_LTrigger->SetVisible( false );
    m_RTrigger->SetVisible( false );
}

