//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniMenu
//
// Description: Implementation of the CGuiScreenMiniMenu class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/24      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/minigame/guiscreenminimenu.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiuserinputhandler.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guiscreenprompt.h>

#include <cards/cardgallery.h>
#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <mission/rewards/reward.h>
#include <render/enums/renderenums.h>
#include <supersprint/supersprintmanager.h>
#include <supersprint/supersprintdata.h>

// Scrooby
//
#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <sprite.h>
#include <text.h>
#include <pure3dobject.h>
#include <polygon.h>

// ATG
//
#include <p3d/camera.hpp>
#include <p3d/utility.hpp>
#include <raddebug.hpp>

#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float NUM_LAPS_ARROW_ROTATION = 90.0f; // in degrees
const float NUM_LAPS_ARROW_GREY_OUT_ALPHA = 0.5f;

#ifdef RAD_WIN32
const float CHARACTER_SCALE = 0.67f; 
const float VEHICLE_SCALE = 0.5f;
const float CHARACTER_ARROW_SCALE = 0.45f;
const float VEHICLE_ARROW_SCALE = 0.85f;
const float NUM_LAPS_ARROW_SCALE = 0.375f; // scale arrows down a bit
#else
const float NUM_LAPS_ARROW_SCALE = 0.75f; // scale arrows down a bit
#endif

PlayerMenu::PlayerMenu()
:   m_currentSubMenu( 0 ),
    m_controllerID( -1 ),
    m_pressStart( NULL ),
    m_vehicleRating( NULL ),
    m_characterSelectedIcon( NULL ),
    m_vehicleSelectedIcon( NULL )
{
    memset( m_pMenu, 0, sizeof( m_pMenu ) );
}

void
PlayerMenu::HandleMessage( eGuiMessage message, unsigned int param1,
                                                unsigned int param2 )
{
    if( m_currentSubMenu < NUM_SUB_MENUS )
    {
        rAssert( m_currentSubMenu >= 0 );
        m_pMenu[ m_currentSubMenu ]->HandleMessage( message, param1, param2 );
    }
}

CGuiMenu*
PlayerMenu::GetCurrentSubMenu() const
{
    if( m_currentSubMenu < NUM_SUB_MENUS )
    {
        rAssert( m_currentSubMenu >= 0 );
        return m_pMenu[ m_currentSubMenu ];
    }

    return NULL;
}

void
PlayerMenu::SetActive( bool isActive, int controllerID )
{
    m_controllerID = isActive ? controllerID : -1;

    // hide/show press start text
    //
    rAssert( m_pressStart != NULL );
    m_pressStart->SetVisible( !isActive );

    // show/hide player cursor
    //
    rAssert( m_pMenu[ MENU_CHARACTERS ] != NULL );
    m_pMenu[ MENU_CHARACTERS ]->GetCursor()->SetVisible( isActive );

    // show/hide vehicle rating
    //
    if( m_vehicleRating != NULL )
    {
        m_vehicleRating->SetVisible( isActive );
    }

    // reset vehicle selection
    //
    rAssert( m_pMenu[ MENU_VEHICLES ] != NULL );
    m_pMenu[ MENU_VEHICLES ]->SetSelectionValue( 0, 0 );
    m_pMenu[ MENU_VEHICLES ]->SetMenuItemEnabled( 0, isActive, true );

    // turn off selected icons
    //
    rAssert( m_characterSelectedIcon != NULL );
    m_characterSelectedIcon->SetAlpha( 0.0f );

    rAssert( m_vehicleSelectedIcon != NULL );
    m_vehicleSelectedIcon->SetAlpha( 0.0f );

    // set controller ID for vehicle selection menu
    //
    m_pMenu[ MENU_VEHICLES ]->SetControllerID( controllerID );

    // reset current sub-menu
    //
    m_currentSubMenu = 0;


}

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMiniMenu::CGuiScreenMiniMenu
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
CGuiScreenMiniMenu::CGuiScreenMiniMenu
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MINI_MENU ),
//    m_screenState( SCREEN_STATE_NORMAL ),
    m_pTrackMenu( NULL ),
    m_trackDirection( NULL ),
    m_trackNumLaps( NULL ),
    m_trackNumLapsArrowU( NULL ),
    m_trackNumLapsArrowD( NULL ),
    m_trackCursorBgd( NULL ),
    m_elapsedTime( 0 ),
    m_backLabel( NULL ),
    m_numActivePlayers( 0 ),
    m_numActivePlayersDone( 0 ),
    m_characterSlots( 0 ),
    m_numUnlockedVehicles( 0 ),
    m_optionsButton( NULL ),
#ifdef RAD_WIN32
    m_currentTrack(0),
    m_bTrackSelected(false),
#endif
//    m_optionsOverlay( NULL ),
//    m_optionsMenu( NULL ),
    m_characterSelectInfo( NULL ),
    m_timerOverlay( NULL ),
    m_remainingTime( TIMER_WAIT_TIME )
{
    memset( m_unlockedVehicles, 0, sizeof( m_unlockedVehicles ) );

#ifdef MINI_MENU_SHOW_3D_CHARACTERS
    memset( m_3dCharacters, 0, sizeof( m_3dCharacters ) );
#endif

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "MiniMenu" );
	rAssert( pPage != NULL );

    int i = 0;
    char name[ 32 ];
    Scrooby::Group* pGroup = NULL;

    // create sub menu for tracks (only one needed for all players)
    //
    m_pTrackMenu = new CGuiMenu2D( this, NUM_TRACKS, NUM_TRACKS, GUI_SPRITE_MENU, MENU_SFX_NONE );
    rAssert( m_pTrackMenu != NULL );

    // add tracks to menu
    //
    pGroup = pPage->GetGroup( "Tracks" );
    rAssert( pGroup != NULL );
    for( i = 0; i < NUM_TRACKS; i++ )
    {
        sprintf( name, "Track%d", i );
        m_pTrackMenu->AddMenuItem( pGroup->GetSprite( name ) );
    }

    if( this->IsWideScreenDisplay() )
    {
        ApplyWideScreenCorrectionScale( pGroup );
    }

    // add track cursor
    //
    Scrooby::Group* pGroupCursor = pGroup->GetGroup( "TrackCursor" );
    rAssert( pGroupCursor != NULL );
    m_pTrackMenu->SetCursor( pGroupCursor );

    // get track cursor background
    //
    m_trackCursorBgd = pGroup->GetPolygon( "TrackBgd" );

    // get track num laps and direction
    //
    m_trackNumLaps = pGroupCursor->GetText( "NumLaps" );
    m_trackDirection = pGroupCursor->GetSprite( "Direction" );

    rAssert( m_trackNumLaps != NULL );
    m_trackNumLaps->SetIndex( SuperSprintData::DEFAULT_NUM_LAPS );

    // rotate num laps arrows (90 deg CW) so they point up and down, respectively
    //
    m_trackNumLapsArrowU = pGroupCursor->GetSprite( "NumLaps_LArrow" );
    if( m_trackNumLapsArrowU != NULL )
    {
        m_trackNumLapsArrowU->ResetTransformation();
        m_trackNumLapsArrowU->ScaleAboutCenter( NUM_LAPS_ARROW_SCALE ); 
        m_trackNumLapsArrowU->RotateAboutCenter( NUM_LAPS_ARROW_ROTATION );
    }

    m_trackNumLapsArrowD = pGroupCursor->GetSprite( "NumLaps_RArrow" );
    if( m_trackNumLapsArrowD != NULL )
    {
        m_trackNumLapsArrowD->ResetTransformation();
        m_trackNumLapsArrowD->ScaleAboutCenter( NUM_LAPS_ARROW_SCALE );
        m_trackNumLapsArrowD->RotateAboutCenter( NUM_LAPS_ARROW_ROTATION );
    }

    // prepare vehicle selections (only unlocked vehicles are selectable by players)
    //
    this->PrepareVehicleSelections();

    // if no vehicles are unlocked, just add the L1 default vehicle so there's at least one vehicle
    // to choose from
    //
    if( m_numUnlockedVehicles == 0 )
    {
        m_unlockedVehicles[ 0 ] = GetRewardsManager()->GetReward( RenderEnums::L1, Reward::eDefaultCar );
        m_numUnlockedVehicles++;
    }

    rAssertMsg( m_numUnlockedVehicles > 0, "There aren't any unlocked vehicles to choose from!" );

    // get back label
    //
    rAssert( m_buttonIcons[ BUTTON_ICON_BACK ] != NULL );
    m_backLabel = m_buttonIcons[ BUTTON_ICON_BACK ]->GetText( "Back" );
    rAssert( m_backLabel != NULL );

    // create player-specific menus
    //
    for( int p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
    {
        // create sub menu for characters
        //
        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ] = new CGuiMenu2D( this,
                                                                                    NUM_CHARACTERS,
                                                                                    NUM_CHARACTERS,
                                                                                    GUI_SPRITE_MENU,
                                                                                    MENU_SFX_NONE );

        rAssert( m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ] != NULL );
        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->SetGreyOutEnabled( false );

        // add character slots to menu
        //
        pGroup = pPage->GetGroup( "Characters" );
        rAssert( pGroup != NULL );
        for( i = 0; i < NUM_CHARACTERS; i++ )
        {
            sprintf( name, "Character%d", i );
            Scrooby::Sprite* characterImage = pGroup->GetSprite( name );
#ifdef RAD_WIN32
            characterImage->ResetTransformation();
            characterImage->ScaleAboutCenter( CHARACTER_SCALE );
#endif
            m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->AddMenuItem( characterImage,
                                                                                    characterImage );
        }

        // get player cursor
        //
        sprintf( name, "Player%d", p );
        pGroupCursor = pGroup->GetGroup( name );
        rAssert( pGroupCursor != NULL );
        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->SetCursor( pGroupCursor );

        // set player-specific colour to cursor
        //
        pGroupCursor->GetText( name )->SetColour( SuperSprintData::PLAYER_COLOURS[ p ] );

        // get character selected icon
        //
        sprintf( name, "Player%d_Selected", p );
        m_playerMenus[ p ].m_characterSelectedIcon = pGroupCursor->GetSprite( name );

        // create sub menu for vehicles
        //
        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ] = new CGuiMenu( this,
                                                                                1,
                                                                                GUI_SPRITE_MENU,
                                                                                MENU_SFX_NONE );

        rAssert( m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ] != NULL );
        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->SetHighlightColour( false, tColour( 0, 0, 0 ) );
//        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->SetSelectionMadeOutlineColour( tColour( 0, 0, 0, 192 ) );

        // add vehicle selections to menu
        //
        pGroup = pPage->GetGroup( "Vehicles" );
        rAssert( pGroup != NULL );

        // get press start text
        //
        sprintf( name, "PressStart%d", p );
        m_playerMenus[ p ].m_pressStart = pGroup->GetText( name );
        rAssert( m_playerMenus[ p ].m_pressStart != NULL );
        m_playerMenus[ p ].m_pressStart->SetTextMode( Scrooby::TEXT_WRAP );
        m_playerMenus[ p ].m_pressStart->ScaleAboutCenter( 0.9f );
        m_playerMenus[ p ].m_pressStart->SetVisible( false ); // hide by default

        sprintf( name, "Vehicle%d_Value", p );
        Scrooby::Sprite* vehicleImage = pGroup->GetSprite( name );
        sprintf( name, "Vehicle%d_LArrow", p );
        Scrooby::Sprite* vehicleImageLArrow = pGroup->GetSprite( name );
        sprintf( name, "Vehicle%d_RArrow", p );
        Scrooby::Sprite* vehicleImageRArrow = pGroup->GetSprite( name );

#ifdef RAD_WIN32
        vehicleImage->ScaleAboutCenter( VEHICLE_SCALE );
        vehicleImageLArrow->ResetTransformation();
        vehicleImageRArrow->ResetTransformation();
        vehicleImageLArrow->ScaleAboutCenter( VEHICLE_ARROW_SCALE );
        vehicleImageRArrow->ScaleAboutCenter( VEHICLE_ARROW_SCALE );
#endif

        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->AddMenuItem( vehicleImage,
                                                                              vehicleImage,
                                                                              NULL,
                                                                              NULL,
                                                                              vehicleImageLArrow,
                                                                              vehicleImageRArrow );

        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->SetSelectionValueCount( 0, m_numUnlockedVehicles );

        // get vehicle cursor
        //
        sprintf( name, "Vehicle%d", p );
        pGroupCursor = pGroup->GetGroup( name );
        rAssert( pGroupCursor != NULL );
        m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->SetCursor( pGroupCursor );

        // set player-specific colour to cursor
        //
        pGroupCursor->GetText( name )->SetColour( SuperSprintData::PLAYER_COLOURS[ p ] );

        // get vehicle selected icon
        //
        sprintf( name, "Vehicle%d_Selected", p );
        m_playerMenus[ p ].m_vehicleSelectedIcon = pGroupCursor->GetSprite( name );

        // get vehicle rating
        //
        sprintf( name, "Vehicle%d_Rating", p );
        m_playerMenus[ p ].m_vehicleRating = pGroup->GetSprite( name );

#ifdef RAD_WIN32
        m_playerMenus[ p ].m_vehicleRating->ScaleAboutCenter( 0.5f );
#endif

        this->UpdateVehicleDisplayImages( p, vehicleImage );
    }

    // get 3D character models
    //
#ifdef MINI_MENU_SHOW_3D_CHARACTERS
    pGroup = pPage->GetGroup( "Characters" );
    rAssert( pGroup != NULL );
    for( i = 0; i < NUM_CHARACTERS; i++ )
    {
        sprintf( name, "Character%d", i );
        m_3dCharacters[ i ] = pGroup->GetPure3dObject( name );
        rAssert( m_3dCharacters[ i ] != NULL );
        m_3dCharacters[ i ]->SetClearDepthBuffer( true );
    }
#endif // MINI_MENU_SHOW_3D_CHARACTERS

    // get options overlay stuff
    //
    m_optionsButton = pPage->GetGroup( "Options" );
    rAssert( m_optionsButton != NULL );

    Scrooby::Text* optionsText = m_optionsButton->GetText( "Options" );
    if( optionsText != NULL )
    {
        optionsText->SetTextMode( Scrooby::TEXT_WRAP );
    }

    if( this->IsWideScreenDisplay() )
    {
        ApplyWideScreenCorrectionScale( m_optionsButton );
    }
/*
    m_optionsOverlay = pPage->GetLayer( "Options" );
    rAssert( m_optionsOverlay != NULL );

    // create options menu
    //
    m_optionsMenu = new CGuiMenu( this, NUM_OPTIONS_MENU_ITEMS );
    rAssert( m_optionsMenu != NULL );

    // add menu items to options menu
    //
    pGroup = pPage->GetGroup( "OptionsMenu" );
    rAssert( pGroup != NULL );

    m_optionsMenu->AddMenuItem( pGroup->GetText( "NumLaps" ),
                                pGroup->GetText( "NumLaps_Value" ),
                                NULL,
                                NULL,
                                pGroup->GetSprite( "NumLaps_ArrowL" ),
                                pGroup->GetSprite( "NumLaps_ArrowR" ),
                                SELECTION_ENABLED | TEXT_OUTLINE_ENABLED );

    m_optionsMenu->AddMenuItem( pGroup->GetText( "ReverseDirection" ),
                                pGroup->GetText( "ReverseDirection_Value" ),
                                NULL,
                                NULL,
                                pGroup->GetSprite( "ReverseDirection_ArrowL" ),
                                pGroup->GetSprite( "ReverseDirection_ArrowR" ),
                                SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );
*/

    // get character select info
    //
    m_characterSelectInfo = pPage->GetGroup( "CharacterSelectInfo" );
    if( m_characterSelectInfo != NULL )
    {
        m_characterSelectInfo->SetVisible( false ); // hide by default
    }

#ifdef RAD_WIN32
    Scrooby::Sprite* arrow = m_characterSelectInfo->GetSprite( "CharacterLArrow" );
    arrow->ResetTransformation();
    arrow->ScaleAboutCenter( CHARACTER_ARROW_SCALE );
    arrow = m_characterSelectInfo->GetSprite( "CharacterRArrow" );
    arrow->ResetTransformation();
    arrow->ScaleAboutCenter( CHARACTER_ARROW_SCALE );
#endif

    // get timer overlay stuff
    //
    m_timerOverlay = pPage->GetGroup( "Timer" );
    rAssert( m_timerOverlay != NULL );
    m_timerOverlay->SetVisible( false ); // hide by default

    Scrooby::Text* waitingForOtherPlayers = m_timerOverlay->GetText( "WaitingForOtherPlayers" );
    if( waitingForOtherPlayers != NULL )
    {
        waitingForOtherPlayers->SetTextMode( Scrooby::TEXT_WRAP );
    }

    m_timer.SetScroobyText( m_timerOverlay, "Timer" );
    m_timer.m_digits[ 0 ]->ResetTransformation();
    m_timer.m_digits[ 0 ]->ScaleAboutPoint( 1.5f, 0, 0 );
}

//===========================================================================
// CGuiScreenMiniMenu::~CGuiScreenMiniMenu
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
CGuiScreenMiniMenu::~CGuiScreenMiniMenu()
{
#ifdef MINI_MENU_SHOW_3D_CHARACTERS
    // release drawables for 3D characters
    //
    for( int c = 0; c < NUM_CHARACTERS; c++ )
    {
        if( m_3dCharacters[ c ] != NULL )
        {
            m_3dCharacters[ c ]->SetDrawable( NULL );
        }
    }
#endif // MINI_MENU_SHOW_3D_CHARACTERS

    // destroy track menu
    //
    if( m_pTrackMenu != NULL )
    {
        delete m_pTrackMenu;
        m_pTrackMenu = NULL;
    }

    // destroy all player sub-menus
    //
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        for( int j = 0; j < PlayerMenu::NUM_SUB_MENUS; j++ )
        {
            if( m_playerMenus[ i ].m_pMenu[ j ] != NULL )
            {
                delete m_playerMenus[ i ].m_pMenu[ j ];
                m_playerMenus[ i ].m_pMenu[ j ] = NULL;
            }
        }
    }

/*
    // destroy options menu
    //
    if( m_optionsMenu != NULL )
    {
        delete m_optionsMenu;
        m_optionsMenu = NULL;
    }
*/
}

//===========================================================================
// CGuiScreenMiniMenu::HandleMessage
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
void CGuiScreenMiniMenu::HandleMessage(	eGuiMessage message,
                                        unsigned int param1,
                                        unsigned int param2 )
{
    if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        rAssert( param1 == PROMPT_CONFIRM_QUIT );

        switch( param2 )
        {
            case (CGuiMenuPrompt::RESPONSE_YES):
            {
                m_pParent->HandleMessage( GUI_MSG_QUIT_MINIGAME );

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

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        int controllerID = -1;
        int playerMenuID = -1;

        if( this->IsControllerMessage( message ) )
        {
            controllerID = static_cast<int>( param1 );
            playerMenuID = this->GetPlayerMenuID( controllerID );

            this->UpdateCharacterSlots();
        }

        if( m_numActivePlayers == 0 )
        {
            // relay message to track menu
            //
            rAssert( m_pTrackMenu != NULL );
            m_pTrackMenu->HandleMessage( message, param1, param2 );
        }
        else
        {
            if( this->IsControllerMessage( message ) )
            {
                // relay message ONLY to associated player's current sub-menu
                //
                if( playerMenuID != -1 )
                {
                    m_playerMenus[ playerMenuID ].HandleMessage( message, param1, param2 );

                    if( m_playerMenus[ playerMenuID ].m_currentSubMenu == PlayerMenu::MENU_VEHICLES )
                    {
                        this->UpdateVehicleRating( playerMenuID );
                    }
                }
            }
            else
            {
                // relay message to all players' current sub-menu
                //
                for( int p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
                {
                    m_playerMenus[ p ].HandleMessage( message, param1, param2 );
                }
            }
        }

        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                this->UpdateCharacterSlots();

                const unsigned int PULSE_PERIOD = 500;

                float alpha = GuiSFX::Pulse( (float)m_elapsedTime,
                                             (float)PULSE_PERIOD,
                                             0.6f,
                                             0.4f,
                                             -rmt::PI_BY2 );

                if( m_numActivePlayers == 0 ) // track selection state
                {
                    // pulse cursor bgd alpha
                    //
                    if( m_trackCursorBgd != NULL )
                    {
                        m_trackCursorBgd->SetAlpha( alpha );
                    }

                    // update U/D arrows on num laps
                    //
                    if( m_trackNumLapsArrowU != NULL && m_trackNumLapsArrowD != NULL )
                    {
                        m_trackNumLapsArrowU->SetIndex( 0 );
                        m_trackNumLapsArrowD->SetIndex( 0 );

                        int numUserInputHandlers = GetGuiSystem()->GetNumUserInputHandlers();
                        for( int i = 0; i < numUserInputHandlers; i++ )
                        {
                            CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( i );
                            if( userInputHandler != NULL )
                            {
#ifdef RAD_WIN32
                                if( userInputHandler->IsYAxisOnUp() ||
                                    GetInputManager()->GetFEMouse()->LeftButtonDownOn() == HOTSPOT_ARROWUP )
#else
                                if( userInputHandler->IsButtonDown( GuiInput::Up ) ||
                                    userInputHandler->IsYAxisOnUp() )
#endif
                                {
                                    rAssert( m_trackNumLapsArrowU->GetNumOfImages() > 1 );
                                    m_trackNumLapsArrowU->SetIndex( 1 );
                                }

#ifdef RAD_WIN32
                                if( userInputHandler->IsYAxisOnDown() ||
                                    GetInputManager()->GetFEMouse()->LeftButtonDownOn() == HOTSPOT_ARROWDOWN )
#else
                                if( userInputHandler->IsButtonDown( GuiInput::Down ) ||
                                    userInputHandler->IsYAxisOnDown() )
#endif
                                {
                                    rAssert( m_trackNumLapsArrowD->GetNumOfImages() > 1 );
                                    m_trackNumLapsArrowD->SetIndex( 1 );
                                }
                            }
                        }
                    }
                }
                else // not in track selection state
                {
#ifdef RAD_XBOX
                    // re-enable Start-to-Select button mapping
                    //
                    for( int i = 0; i < GetGuiSystem()->GetNumUserInputHandlers(); i++ )
                    {
                        GetGuiSystem()->GetUserInputHandler( i )->EnableStartToSelectMapping( true );
                    }
#endif // RAD_XBOX

                    for( int p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
                    {
                        // pulse player's current sub-menu cursor
                        //
                        CGuiMenu* currentSubMenu = m_playerMenus[ p ].GetCurrentSubMenu();
                        if( currentSubMenu != NULL )
                        {
                            Scrooby::Drawable* pCursor = currentSubMenu->GetCursor();
                            rAssert( pCursor != NULL );
                            pCursor->SetAlpha( alpha );
                        }
                    }
                }

                m_elapsedTime += param1;
                m_elapsedTime %= PULSE_PERIOD;

                rAssert( m_timerOverlay != NULL );
                if( m_timerOverlay->IsVisible() )
                {
                    // update timer
                    //
                    m_remainingTime -= static_cast<int>( param1 );
                    if( m_remainingTime > 0 )

                    {
                        m_timer.SetValue( static_cast<unsigned int>( m_remainingTime ) / 1000 );
                    }
                    else
                    {
                        this->SetTimerEnabled( false );

                        // timer expired, letz start the mini-game!
                        //
                        m_pParent->HandleMessage( GUI_MSG_QUIT_MINIGAME, m_numActivePlayersDone );
                    }
                }

                break;
            }
            case GUI_MSG_MENU_SELECTION_CHANGED: // param1 = old, param2 = new
            {
                if( m_numActivePlayers == 0 ) // track selection state
                {
                    this->MoveTrackCursor( static_cast<int>( param2 ), static_cast<int>( param1 ) );
                }

                break;
            }
            case GUI_MSG_CONTROLLER_UP:
            {
                if( m_numActivePlayers == 0 ) // track selection state
                {
                    // increment number of laps
                    //
                    rAssert( m_trackNumLaps != NULL );
                    int currentNumLaps = m_trackNumLaps->GetIndex();
                    if( currentNumLaps < SuperSprintData::MAX_NUM_LAPS )
                    {
                        currentNumLaps++;
                        m_trackNumLaps->SetIndex( currentNumLaps );

                        if( currentNumLaps == SuperSprintData::MAX_NUM_LAPS &&
                            m_trackNumLapsArrowU != NULL )
                        {
                            m_trackNumLapsArrowU->SetAlpha( NUM_LAPS_ARROW_GREY_OUT_ALPHA ); // grey out arrow
                        }

                        if( m_trackNumLapsArrowD != NULL )
                        {
                            m_trackNumLapsArrowD->SetAlpha( 1.0f ); // restore normal arrow
                        }

                        GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_DOWN:
            {
                if( m_numActivePlayers == 0 ) // track selection state
                {
                    // decrement number of laps
                    //
                    rAssert( m_trackNumLaps != NULL );
                    int currentNumLaps = m_trackNumLaps->GetIndex();
                    if( currentNumLaps > SuperSprintData::MIN_NUM_LAPS )
                    {
                        currentNumLaps--;
                        m_trackNumLaps->SetIndex( currentNumLaps );

                        if( currentNumLaps == SuperSprintData::MIN_NUM_LAPS &&
                            m_trackNumLapsArrowD != NULL )
                        {
                            m_trackNumLapsArrowD->SetAlpha( NUM_LAPS_ARROW_GREY_OUT_ALPHA ); // grey out arrow
                        }

                        if( m_trackNumLapsArrowU != NULL )
                        {
                            m_trackNumLapsArrowU->SetAlpha( 1.0f ); // restore normal arrow
                        }

                        GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_START:
            {
                if( m_numActivePlayers == 0 )
                {
                    this->OnTrackSelected();
                }

                // active new player menu
                //
                if( playerMenuID == -1 ) // means this player's not active yet
                {
#ifdef RAD_XBOX
                    // disable Start-to-Select button mapping to prevent duplicate SELECT inputs;
                    // it will be re-enabled in the next update
                    //
                    GetGuiSystem()->GetUserInputHandler( controllerID )->EnableStartToSelectMapping( false );
#endif // RAD_XBOX

                    this->ActivateNewPlayer( controllerID );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );
                }

                break;
            }
            case GUI_MSG_CONTROLLER_SELECT:
            {
                if( m_numActivePlayers == 0 )
                {
                    this->OnTrackSelected();

                    // track selected, activate the first player
                    //
                    this->ActivateNewPlayer( controllerID );
                }
                else 
                {
                    if( playerMenuID != -1 && !m_playerMenus[ playerMenuID ].IsDone() )
                    {
                        Scrooby::Drawable* pCursor = m_playerMenus[ playerMenuID ].GetCurrentSubMenu()->GetCursor();
                        rAssert( pCursor != NULL );
                        pCursor->SetAlpha( 1.0f ); // restore cursor alpha

                        m_playerMenus[ playerMenuID ].m_currentSubMenu++;

                        if( m_playerMenus[ playerMenuID ].IsDone() )
                        {
                            // player is done selecting from all sub-menus
                            //
                            m_numActivePlayersDone++;

                            this->SetTimerEnabled( true );

                            this->OnVehicleSelected( playerMenuID, true );
                        }
                        else
                        {
                            this->OnCharacterSelected( playerMenuID, true );
                        }

                        // if everyone's done selecting, start the game!
                        //
                        if( m_numActivePlayersDone == SuperSprintData::NUM_PLAYERS )
                        {
                            this->SetTimerEnabled( false );

                            m_pParent->HandleMessage( GUI_MSG_QUIT_MINIGAME, m_numActivePlayersDone );
                        }
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                if( m_numActivePlayers == 0 )
                {
                    // display quit confirmation screen
                    //
                    m_guiManager->DisplayPrompt( PROMPT_CONFIRM_QUIT, this );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK );
                }
                else
                {
                    if( playerMenuID != -1 )
                    {
                        if( m_playerMenus[ playerMenuID ].m_currentSubMenu > 0 )
                        {
                            if( !m_playerMenus[ playerMenuID ].IsDone() )
                            {
                                Scrooby::Drawable* pCursor = m_playerMenus[ playerMenuID ].GetCurrentSubMenu()->GetCursor();
                                rAssert( pCursor != NULL );
                                pCursor->SetAlpha( 1.0f ); // restore cursor alpha

                                this->OnCharacterSelected( playerMenuID, false );
                            }
                            else
                            {
                                this->OnVehicleSelected( playerMenuID, false );

                                m_numActivePlayersDone--;

                                // if no one's done selecting all their sub-menus, disable timer
                                //
                                if( m_numActivePlayersDone == 0 )
                                {
                                    this->SetTimerEnabled( false );
                                }
                            }

                            m_playerMenus[ playerMenuID ].m_currentSubMenu--;
                        }
                        else
                        {
                            // de-activate player menu
                            //
                            m_playerMenus[ playerMenuID ].SetActive( false );
                            GetInputManager()->UnregisterControllerID( playerMenuID );
                            
                            m_numActivePlayers--;

                            if( m_numActivePlayers == 0 )
                            {
                                // re-enable track selection
                                //
                                this->SetTrackSelectionEnabled( true );
                            }
                        }

                        GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK );
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_AUX_X:
            {
                if( m_numActivePlayers == 0 ) // track selection state
                {
                    // toggle track direction
                    //
                    rAssert( m_trackDirection != NULL );
                    m_trackDirection->SetIndex( 1 - m_trackDirection->GetIndex() );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
/*
                    // show options overlay
                    //
                    rAssert( m_optionsOverlay != NULL );
                    m_optionsOverlay->SetVisible( true );

                    // hide buttons
                    //
                    rAssert( m_optionsButton != NULL );
                    m_optionsButton->SetVisible( false );
                    this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );

                    // reset options menu
                    //
                    rAssert( m_optionsMenu != NULL );
                    m_optionsMenu->Reset();

                    m_screenState = SCREEN_STATE_OPTIONS;
*/
                }

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

#ifdef RAD_WIN32
//===========================================================================
// CGuiScreenMiniMenu::CheckCursorAgainstHotspots
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
eFEHotspotType CGuiScreenMiniMenu::CheckCursorAgainstHotspots( float x, float y )
{
    if( m_bTrackSelected ) return HOTSPOT_NONE;
    eFEHotspotType hotSpotType = HOTSPOT_NONE;
    CGuiMenu* pCurrentMenu = HasMenu();
    int numMenuItems = 0;
    GuiMenuItem* pMenuItem = NULL;


    if( pCurrentMenu )
    {
        numMenuItems = pCurrentMenu->GetNumItems();
        for( int i = 0; i < numMenuItems; i++ )
        {
            pMenuItem = pCurrentMenu->GetMenuItem( i );

            if( pMenuItem )
            {
                if( pMenuItem->GetItem()->IsVisible() )
                {
                    // Just tests if the point is in the bounding rect of the sprite.
                    if( pMenuItem->GetItem()->IsPointInBoundingRect( x, y ) )
                    {
                        //rDebugPrintf( "Cursor is inside Sprite %d rectangle!\n", i );
                        pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_OVER, i );
                        m_currentTrack = i;
                        hotSpotType = HOTSPOT_BUTTON;

                        //After taking care of all the events for this menu item, just bail out.
                        break;
                    }
                }
            }
        }           
    }

    if( hotSpotType == HOTSPOT_NONE )
    {
        // Since there's only one set of up and down arrows, when they get translated,
        // their bounding box info doesn't change.. so I just offset it depending on
        // their current selection.
        float xOffset = 0.25f * m_currentTrack;
        x -= xOffset;

        if( m_trackNumLapsArrowU )
        {
            if( m_trackNumLapsArrowU->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_ARROWUP;
            }
        }
        if( m_trackNumLapsArrowD )
        {        
            if( m_trackNumLapsArrowD->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_ARROWDOWN;
            }
        }
    }

    return hotSpotType;
}
#endif

//===========================================================================
// CGuiScreenMiniMenu::InitIntro
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
void CGuiScreenMiniMenu::InitIntro()
{
    // disable track(s) that are not unlocked
    //
    rAssert( m_pTrackMenu != NULL );
    if( !CommandLineOptions::Get( CLO_SKIP_FE ) )
    {
        for( int i = 0; i < NUM_TRACKS; i++ )
        {
            bool isTrackUnlocked = GetCardGallery()->IsCardDeckComplete( static_cast<unsigned int>( i ) );
            m_pTrackMenu->SetMenuItemEnabled( i, isTrackUnlocked );
        }
    }

    rAssertMsg( m_pTrackMenu->GetSelection() != -1, "Why isn't there at least one track enabled??" );

    if( m_firstTimeEntered )
    {
        this->MoveTrackCursor( 0, m_pTrackMenu->GetSelection() );
    }

    // set default track direction
    //
    rAssert( m_trackDirection != NULL );
    m_trackDirection->SetIndex( GetSSM()->IsTrackReversed() ? 0 : 1 );

    // de-activate all player menus
    //
    for( int p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
    {
        m_playerMenus[ p ].SetActive( false );
        GetInputManager()->UnregisterControllerID( p );
    }

    // restore all normal character bitmaps
    //
    for( int j = 0; j < NUM_CHARACTERS; j++ )
    {
        rAssert( m_playerMenus[ 0 ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ] != NULL );
        m_playerMenus[ 0 ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->SetSelectionValue( j, 0 );
    }

    m_numActivePlayers = 0;
    m_numActivePlayersDone = 0;

    this->SetTrackSelectionEnabled( true );
}

//===========================================================================
// CGuiScreenMiniMenu::InitRunning
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
void CGuiScreenMiniMenu::InitRunning()
{
#ifdef MINI_MENU_SHOW_3D_CHARACTERS
    // set drawables for 3D characters
    //
    tDrawable* pDrawable = NULL;

    pDrawable = p3d::find<tDrawable>( "homer_h" );
    if( pDrawable != NULL )
    {
        m_3dCharacters[ CHARACTER_HOMER ]->SetDrawable( pDrawable );
    }

    pDrawable = p3d::find<tDrawable>( "bart_h" );
    if( pDrawable != NULL )
    {
        m_3dCharacters[ CHARACTER_BART ]->SetDrawable( pDrawable );
    }

    pDrawable = p3d::find<tDrawable>( "lisa_h" );
    if( pDrawable != NULL )
    {
        m_3dCharacters[ CHARACTER_LISA ]->SetDrawable( pDrawable );
    }

    pDrawable = p3d::find<tDrawable>( "marge_h" );
    if( pDrawable != NULL )
    {
        m_3dCharacters[ CHARACTER_MARGE ]->SetDrawable( pDrawable );
    }

    pDrawable = p3d::find<tDrawable>( "apu_h" );
    if( pDrawable != NULL )
    {
        m_3dCharacters[ CHARACTER_APU ]->SetDrawable( pDrawable );
    }

    // adjust camera aspect ratio to match viewport dimensions
    //
    for( int i = 0; i < NUM_CHARACTERS; i++ )
    {
        rAssert( m_3dCharacters[ i ] != NULL );

        int width = 0;
        int height = 0;
        m_3dCharacters[ i ]->GetBoundingBoxSize( width, height );

        tCamera* pCamera = m_3dCharacters[ i ]->GetCamera();
        rAssert( pCamera != NULL );
        pCamera->SetFOV( pCamera->GetFieldOfView(), (float)width / (float)height );

        // show 3D characters
        //
        m_3dCharacters[ i ]->SetVisible( false ); // true );
    }
#endif // MINI_MENU_SHOW_3D_CHARACTERS
}

//===========================================================================
// CGuiScreenMiniMenu::InitOutro
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
void CGuiScreenMiniMenu::InitOutro()
{
#ifdef MINI_MENU_SHOW_3D_CHARACTERS
    // hide 3D characters
    //
    for( int i = 0; i < NUM_CHARACTERS; i++ )
    {
        rAssert( m_3dCharacters[ i ] != NULL );
        m_3dCharacters[ i ]->SetVisible( false );
    }
#endif // MINI_MENU_SHOW_3D_CHARACTERS

    // setup supersprint stuff
    //
    for( int p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
    {
        if( m_playerMenus[ p ].IsActive() )
        {

            int character = m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->GetSelection();
            GetSSM()->SetCharacter( p, character );

            int vehicle = m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->GetSelectionValue( 0 );
            rAssert( m_unlockedVehicles[ vehicle ] != NULL );
            GetSSM()->SetVehicle( p, m_unlockedVehicles[ vehicle ]->GetName() );
        }
    }
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMiniMenu::UpdateCharacterSlots()
{
    // update character slots availability
    //
    int p = 0;

    m_characterSlots = 0;
    for( p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
    {
        if( m_playerMenus[ p ].IsActive() )
        {
            int currentSelection = m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->GetSelection();
            m_characterSlots |= (1 << currentSelection );
        }
    }

    // enable/disable character selections for all player menus
    //
    for( p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
    {
        if( m_playerMenus[ p ].IsActive() )
        {
            for( int i = 0; i < NUM_CHARACTERS; i++ )
            {
                int currentSelection = m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->GetSelection();
                if( i != currentSelection )
                {
                    bool isEnabled = ( ((1 << i) & m_characterSlots) == 0 );
                    m_playerMenus[ p ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->SetMenuItemEnabled( i, isEnabled );
                }
            }
        }
    }
}

void
CGuiScreenMiniMenu::SetTrackSelectionEnabled( bool enable )
{
    // show/hide options button
    //
    rAssert( m_optionsButton != NULL );
    m_optionsButton->SetVisible( enable );

    // set back label to "quit" if enabled
    //
    rAssert( m_backLabel != NULL );
    m_backLabel->SetIndex( enable ? 1 : 0 );

    // hide/show all players' "press start"
    //
    for( int p = 0; p < SuperSprintData::NUM_PLAYERS; p++ )
    {
        rAssert( m_playerMenus[ p ].m_pressStart != NULL );
        m_playerMenus[ p ].m_pressStart->SetVisible( !enable );
    }

    // hide/show character select info
    //
    if( m_characterSelectInfo != NULL )
    {
        m_characterSelectInfo->SetVisible( !enable );
    }
#ifdef RAD_WIN32
    m_bTrackSelected = !enable;
#endif
}

void
CGuiScreenMiniMenu::MoveTrackCursor( int previousIndex, int nextIndex )
{
    // if cursor exists
    //
    if( m_trackCursorBgd != NULL )
    {
        Scrooby::BoundedDrawable* item = NULL;
        int x1, y1, x2, y2;

        rAssert( m_pTrackMenu != NULL );

        // get location of previous item
        item = m_pTrackMenu->GetMenuItem( previousIndex )->GetItem();
        rAssert( item != NULL );
        item->GetOriginPosition( x1, y1 );

        // get location of next item
        item = m_pTrackMenu->GetMenuItem( nextIndex )->GetItem();
        rAssert( item != NULL );
        item->GetOriginPosition( x2, y2 );

        // translate cursor
        m_trackCursorBgd->Translate( x2 - x1, y2 - y1 );
    }
}

void
CGuiScreenMiniMenu::OnTrackSelected()
{
    this->SetTrackSelectionEnabled( false );

    // highlight track cursor bgd
    //
    if( m_trackCursorBgd != NULL )
    {
        m_trackCursorBgd->SetAlpha( 1.0f );
    }

    // restore U/D num laps arrows
    //
    if( m_trackNumLapsArrowU != NULL && m_trackNumLapsArrowD != NULL )
    {
        m_trackNumLapsArrowU->SetIndex( 0 );
        m_trackNumLapsArrowD->SetIndex( 0 );
    }

    // get current track selection
    //
    rAssert( m_pTrackMenu != NULL );
    int currentTrack = m_pTrackMenu->GetSelection();

    // disable selection made delay
    //
    m_pTrackMenu->MakeSelection( false );

    // tell gameplay manager which track to load
    //
    RenderEnums::LevelEnum levelIndex = static_cast<RenderEnums::LevelEnum>( RenderEnums::B01 + currentTrack );
    GetGameplayManager()->SetLevelIndex( levelIndex );

    // set track options
    //
    rAssert( m_trackNumLaps != NULL );
    GetSSM()->SetNumLaps( m_trackNumLaps->GetIndex() );

    rAssert( m_trackDirection != NULL );
    GetSSM()->SetTrackDirection( m_trackDirection->GetIndex() == 0 );
}

void
CGuiScreenMiniMenu::ActivateNewPlayer( int controllerID )
{
    if( m_numActivePlayers >= SuperSprintData::NUM_PLAYERS )
    {
        // can't activate anymore new players
        //
        return;
    }

    int player_slot = 0;
#ifdef RAD_XBOX
    player_slot = controllerID;
#elif defined ( RAD_WIN32 )
    player_slot = controllerID;
#else
    // look for non empty slot
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        // find first non-active menu to activate
        //
        if( !m_playerMenus[ i ].IsActive() )
        {
            player_slot = i;
            break;
        }
    }
#endif

    // ok, found it!
    //
    m_playerMenus[ player_slot ].SetActive( true, controllerID );
    GetInputManager()->RegisterControllerID( player_slot , controllerID);

    m_numActivePlayers++;

    // default character selection to first empty slot
    //
    for( int s = 0; s < NUM_CHARACTERS; s++ )
    {
        if( ((1 << s) & m_characterSlots) == 0 )
        {
            m_playerMenus[ player_slot ].GetCurrentSubMenu()->Reset( s );

            break;
        }
    }

    // update vehicle rating
    //
    this->UpdateVehicleRating( player_slot );
}

void
CGuiScreenMiniMenu::OnCharacterSelected( int playerID, bool isSelected )
{
    rAssert( playerID != -1 );
    rAssert( m_playerMenus[ playerID ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ] != NULL );

    // show/hide character selection done indicator
    //
    rAssert( m_playerMenus[ playerID ].m_characterSelectedIcon != NULL );
    m_playerMenus[ playerID ].m_characterSelectedIcon->SetAlpha( isSelected ? 1.0f : 0.0f );

    // swap selected character bitmap
    //
    int characterIndex = m_playerMenus[ playerID ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->GetSelection();
    m_playerMenus[ playerID ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->SetSelectionValue( characterIndex, isSelected ? CHARACTER_SELECTED : CHARACTER_NOT_SELECTED );

    // disable selection made delay
    //
    if( isSelected )
    {
        m_playerMenus[ playerID ].m_pMenu[ PlayerMenu::MENU_CHARACTERS ]->MakeSelection( false );
    }
}

void
CGuiScreenMiniMenu::OnVehicleSelected( int playerID, bool isSelected )
{
    rAssert( playerID != -1 );
    rAssert( m_playerMenus[ playerID ].m_pMenu[ PlayerMenu::MENU_VEHICLES ] != NULL );

    // show/hide vehicle selection done indicator
    //
    rAssert( m_playerMenus[ playerID ].m_vehicleSelectedIcon != NULL );
    m_playerMenus[ playerID ].m_vehicleSelectedIcon->SetAlpha( isSelected ? 1.0f : 0.0f );

    // disable selection made delay
    //
    if( isSelected )
    {
        m_playerMenus[ playerID ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->MakeSelection( false );
    }
}

void
CGuiScreenMiniMenu::AddRewardVehicle( Reward* pReward )
{
    rAssert( pReward != NULL );
    if( pReward->RewardStatus() || // if reward is unlocked
        CommandLineOptions::Get( CLO_SKIP_FE ) ||
//        GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_CARDS ) ||
        GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_VEHICLES ) )
    {
        rAssert( m_numUnlockedVehicles < MAX_NUM_VEHICLES );

        m_unlockedVehicles[ m_numUnlockedVehicles ] = pReward;
        m_numUnlockedVehicles++;
    }
}

void
CGuiScreenMiniMenu::PrepareVehicleSelections()
{
    for( int level = 0; level < RenderEnums::numLevels; level++ )
    {
        Reward* pReward = NULL;

        for( int i = Reward::eBlank + 1; i < Reward::NUM_QUESTS; i++ )
        {
            pReward = GetRewardsManager()->GetReward( level, static_cast<Reward::eQuestType>( i ) );
            if( pReward != NULL )
            {
                if( pReward->GetRewardType() == Reward::ALT_PLAYERCAR )
                {
                    this->AddRewardVehicle( pReward );
                }
            }
        }

        for( pReward = GetRewardsManager()->FindFirstMerchandise( level, Merchandise::SELLER_GIL );
             pReward != NULL;
             pReward = GetRewardsManager()->FindNextMerchandise( level, Merchandise::SELLER_GIL ) )
        {
            this->AddRewardVehicle( pReward );
        }

        for( pReward = GetRewardsManager()->FindFirstMerchandise( level, Merchandise::SELLER_SIMPSON );
             pReward != NULL;
             pReward = GetRewardsManager()->FindNextMerchandise( level, Merchandise::SELLER_SIMPSON ) )
        {
            this->AddRewardVehicle( pReward );
        }
    }
}

void
CGuiScreenMiniMenu::UpdateVehicleDisplayImages( int playerID, Scrooby::Sprite* vehicleImage )
{
    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    rAssert( vehicleImage != NULL );
    for( int i = 0; i < m_numUnlockedVehicles; i++ )
    {
        rAssert( m_unlockedVehicles[ i ] != NULL );
        vehicleImage->SetImage( i, m_unlockedVehicles[ i ]->GetName() );
    }

    HeapMgr()->PopHeap( GMA_LEVEL_HUD );
}

void
CGuiScreenMiniMenu::UpdateVehicleRating( int playerID )
{
    if( m_playerMenus[ playerID ].m_vehicleRating != NULL )
    {
        int vehicleIndex = m_playerMenus[ playerID ].m_pMenu[ PlayerMenu::MENU_VEHICLES ]->GetSelectionValue( 0 );

        rAssert( m_unlockedVehicles[ vehicleIndex ] != NULL );
        CarAttributeRecord* carStats = GetRewardsManager()->GetCarAttributeRecord( m_unlockedVehicles[ vehicleIndex ]->GetName() );
        if( carStats != NULL )
        {
            int rating = (int)( GetRewardsManager()->ComputeOverallCarRating( carStats ) );

            rAssertMsg( rating >= 0 && rating < m_playerMenus[ playerID ].m_vehicleRating->GetNumOfImages(),
                        "Invalid vehicle rating (either negative or too high)!" );

            m_playerMenus[ playerID ].m_vehicleRating->SetIndex( rating );
        }
    }
}

void
CGuiScreenMiniMenu::SetTimerEnabled( bool enable )
{
    // show/hide timer overlay
    //
    rAssert( m_timerOverlay != NULL );
    m_timerOverlay->SetVisible( enable );

    if( !enable )
    {
        // reset timer
        //
        m_remainingTime = TIMER_WAIT_TIME;
    }
}

int
CGuiScreenMiniMenu::GetPlayerMenuID( int controllerID ) const
{
    // search for player menu w/ associated controller ID
    //
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        if( m_playerMenus[ i ].m_controllerID == controllerID )
        {
            // found it! return player menu ID
            //
            return i;
        }
    }

    // player menu not found, return -1
    //
    return -1;
}

