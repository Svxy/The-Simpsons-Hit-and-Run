//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseSettings
//
// Description: Implementation of the CGuiScreenPauseSettings class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenpausesettings.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guisystem.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <presentation/tutorialmanager.h>
#include <worldsim/avatarmanager.h>

#include <raddebug.hpp> // Foundation
#include <group.h>
#include <screen.h>
#include <page.h>
#include <text.h>
#include <strings/unicodestring.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

enum ePauseSettingsMenuItem
{
    MENU_ITEM_CAMERA,
    MENU_ITEM_JUMP_CAMERAS,
#ifndef RAD_WIN32
    MENU_ITEM_INVERT_CAM_CONTROL,
#endif
    MENU_ITEM_INTERSECT_NAV_SYSTEM,
    MENU_ITEM_RADAR,
#ifndef RAD_WIN32
    MENU_ITEM_VIBRATION,
#endif
    MENU_ITEM_TUTORIAL,

    NUM_PAUSE_SETTINGS_MENU_ITEMS
};


const char* PAUSE_SETTINGS_MENU_ITEMS[] =
{
    "Camera",
    "JumpCamera",
#ifndef RAD_WIN32
    "InvertCamControl",
#endif
    "IntersectNavSystem",
    "Radar",
#ifndef RAD_WIN32
    "Vibration",
#endif
    "Tutorial",

    ""
};

#ifdef RAD_WIN32
SuperCam::Type PC_CAMERAS_FOR_WALKING[] =
{
    SuperCam::WALKER_CAM,
    SuperCam::DEBUG_CAM,
    SuperCam::KULL_CAM
};

const int NUM_PC_CAMERAS_FOR_WALKING = sizeof(PC_CAMERAS_FOR_WALKING)/sizeof(SuperCam::Type);
const int NUM_PC_CAMERAS_FOR_WALKING_WITHOUT_CHEAT = 1;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPauseSettings::CGuiScreenPauseSettings
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
CGuiScreenPauseSettings::CGuiScreenPauseSettings
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SETTINGS ),
    m_pMenu( NULL ),
    m_currentCameraSelectionMode( CAMERA_SELECTION_NOT_AVAILABLE )
{
MEMTRACK_PUSH_GROUP( "CGuiScreenPauseSettings" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "PauseSettings" );
	rAssert( pPage );

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this, NUM_PAUSE_SETTINGS_MENU_ITEMS, GUI_TEXT_MENU, MENU_SFX_NONE );
    rAssert( m_pMenu != NULL );
    
    // Add menu items
    //
    char itemName[ 32 ];

    for( int i = 0; i < NUM_PAUSE_SETTINGS_MENU_ITEMS; i++ )
    {
        Scrooby::Group* group = pPage->GetGroup( PAUSE_SETTINGS_MENU_ITEMS[ i ] );
        rAssert( group != NULL );

        Scrooby::Text* pText = group->GetText( PAUSE_SETTINGS_MENU_ITEMS[ i ] );
        rAssert( pText != NULL );
        pText->SetTextMode( Scrooby::TEXT_WRAP );

        sprintf( itemName, "%s_Value", PAUSE_SETTINGS_MENU_ITEMS[ i ] );
        Scrooby::Text* pTextValue = group->GetText( itemName );
        rAssert( pTextValue != NULL );
        pTextValue->SetTextMode( Scrooby::TEXT_WRAP );

        sprintf( itemName, "%s_LArrow", PAUSE_SETTINGS_MENU_ITEMS[ i ] );
        Scrooby::Sprite* pLArrow = group->GetSprite( itemName );

        sprintf( itemName, "%s_RArrow", PAUSE_SETTINGS_MENU_ITEMS[ i ] );
        Scrooby::Sprite* pRArrow = group->GetSprite( itemName );

        m_pMenu->AddMenuItem( pText,
                              pTextValue,
                              NULL,
                              NULL,
                              pLArrow,
                              pRArrow,
                              SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );
    }

#ifdef RAD_GAMECUBE
    // change "Vibration" text to "Rumble"
    //
    Scrooby::Text* vibrationText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_ITEM_VIBRATION )->GetItem() );
    rAssert( vibrationText != NULL );
    vibrationText->SetIndex( 1 );
#endif // RAD_GAMECUBE

    for( int j = 0; j < NUM_CAMERA_SELECTION_MODES; j++ )
    {
        m_cameraSelections[ j ] = NULL;
        m_numCameraSelections[ j ] = 0;
    }

    m_cameraSelections[ CAMERA_SELECTION_FOR_DRIVING ] = CAMERAS_FOR_DRIVING;
#ifdef RAD_WIN32
    m_cameraSelections[ CAMERA_SELECTION_FOR_WALKING ] = PC_CAMERAS_FOR_WALKING;
#else
    m_cameraSelections[ CAMERA_SELECTION_FOR_WALKING ] = CAMERAS_FOR_WALKING;
#endif

    if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_CAMERAS ) )
    {
        m_numCameraSelections[ CAMERA_SELECTION_FOR_DRIVING ] = NUM_CAMERAS_FOR_DRIVING;

#ifdef RAD_WIN32
        m_numCameraSelections[ CAMERA_SELECTION_FOR_WALKING ] = NUM_PC_CAMERAS_FOR_WALKING;
#else
        m_numCameraSelections[ CAMERA_SELECTION_FOR_WALKING ] = NUM_CAMERAS_FOR_WALKING;
#endif
    }
    else
    {
        m_numCameraSelections[ CAMERA_SELECTION_FOR_DRIVING ] = NUM_CAMERAS_FOR_DRIVING_WITHOUT_CHEAT;
#ifdef RAD_WIN32
        m_numCameraSelections[ CAMERA_SELECTION_FOR_WALKING ] = NUM_PC_CAMERAS_FOR_WALKING_WITHOUT_CHEAT;
#else
        m_numCameraSelections[ CAMERA_SELECTION_FOR_WALKING ] = NUM_CAMERAS_FOR_WALKING_WITHOUT_CHEAT;
#endif
    }

    GetCheatInputSystem()->RegisterCallback( this );
MEMTRACK_POP_GROUP("CGuiScreenPauseSettings");
}


//===========================================================================
// CGuiScreenPauseSettings::~CGuiScreenPauseSettings
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
CGuiScreenPauseSettings::~CGuiScreenPauseSettings()
{
    GetCheatInputSystem()->UnregisterCallback( this );

    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenPauseSettings::HandleMessage
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
void CGuiScreenPauseSettings::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_CONTROLLER_START:
            {
                // resume game
                //
                m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );

                break;
            }

            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                if( param1 == MENU_ITEM_CAMERA )
                {
                    // set new super cam
                    //
                    SuperCam::Type superCamType = m_cameraSelections[ m_currentCameraSelectionMode ][ param2 ];
                    GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( superCamType, SuperCamCentral::CUT, 0 );

                    // and apply it right away
                    //
                    GetSuperCamManager()->GetSCC( 0 )->Update( 0 );
                }
#ifndef RAD_WIN32
                else if( param1 == MENU_ITEM_VIBRATION )
                {
                    if( param2 == 1 ) // vibration turned ON
                    {
                        // send vibration pulse to controller
                        //
                        int controllerID = GetInputManager()->GetControllerIDforPlayer( 0 );
#ifdef RAD_PS2
                        if ( GetInputManager()->IsControllerInPort( Input::USB0 ) )
                        {
                            GetInputManager()->TriggerRumblePulse( Input::USB0 );
                        }
                        else if ( GetInputManager()->IsControllerInPort( Input::USB1 ) )
                        {
                            GetInputManager()->TriggerRumblePulse( Input::USB1 );
                        }
                        else
#endif
                        {
                            GetInputManager()->TriggerRumblePulse( controllerID );
                        }

                    }
                }
#endif

                break;
            }

            default:
            {
                break;
            }
        }

        // relay message to menu
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

void
CGuiScreenPauseSettings::OnCheatEntered( eCheatID cheatID, bool isEnabled )
{
    if( cheatID == CHEAT_ID_UNLOCK_CAMERAS && isEnabled )
    {
        m_numCameraSelections[ CAMERA_SELECTION_FOR_DRIVING ] = NUM_CAMERAS_FOR_DRIVING;
        m_numCameraSelections[ CAMERA_SELECTION_FOR_WALKING ] = NUM_CAMERAS_FOR_WALKING;
    }
}

//===========================================================================
// CGuiScreenPauseSettings::InitIntro
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
void CGuiScreenPauseSettings::InitIntro()
{
    // update camera setting
    //
    bool allowCameraToggle = GetSuperCamManager()->GetSCC( 0 )->AllowCameraToggle();

    rAssert( m_pMenu );
    m_pMenu->SetMenuItemEnabled( MENU_ITEM_CAMERA, allowCameraToggle );

    if( allowCameraToggle )
    {
        // update camera selections
        //
        this->UpdateCameraSelections();
    }
    else
    {
        m_currentCameraSelectionMode = CAMERA_SELECTION_NOT_AVAILABLE;
    }

    // update other gameplay settings
    //
    bool isSettingOn = false;

#ifndef RAD_WIN32
    isSettingOn = GetSuperCamManager()->GetSCC( 0 )->IsInvertedCameraEnabled();
    m_pMenu->SetSelectionValue( MENU_ITEM_INVERT_CAM_CONTROL,
                                isSettingOn ? 1 : 0 );
#endif

    isSettingOn = GetSuperCamManager()->GetSCC( 0 )->JumpCamsEnabled();
    m_pMenu->SetSelectionValue( MENU_ITEM_JUMP_CAMERAS,
                                isSettingOn ? 1: 0 );

    isSettingOn = GetCharacterSheetManager()->QueryNavSystemSetting();
    m_pMenu->SetSelectionValue( MENU_ITEM_INTERSECT_NAV_SYSTEM,
                                isSettingOn ? 1 : 0 );

    isSettingOn = GetGuiSystem()->IsRadarEnabled();
    m_pMenu->SetSelectionValue( MENU_ITEM_RADAR,
                                isSettingOn ? 1 : 0 );
#ifndef RAD_WIN32
    isSettingOn = GetInputManager()->IsRumbleEnabled();
    m_pMenu->SetSelectionValue( MENU_ITEM_VIBRATION,
                                isSettingOn ? 1 : 0 );
#endif

    isSettingOn = GetTutorialManager()->IsTutorialEventsEnabled();
    m_pMenu->SetSelectionValue( MENU_ITEM_TUTORIAL,
                                isSettingOn ? 1 : 0 );

    // show/hide tutorial setting depending on whether or not tutorial mode is enabled
    //
    if( GetTutorialManager()->IsTutorialModeEnabled() )
    {
        CGuiManagerInGame* guiManagerIngame = static_cast<CGuiManagerInGame*>( m_pParent );
        rAssert( guiManagerIngame != NULL );

        bool isTutorialToggleAllowed = (guiManagerIngame->GetResumeGameScreenID() != GUI_SCREEN_ID_TUTORIAL);
        m_pMenu->SetMenuItemEnabled( MENU_ITEM_TUTORIAL, isTutorialToggleAllowed );
    }
    else
    {
        m_pMenu->SetMenuItemEnabled( MENU_ITEM_TUTORIAL, false, true );
    }
}


//===========================================================================
// CGuiScreenPauseSettings::InitRunning
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
void CGuiScreenPauseSettings::InitRunning()
{
}


//===========================================================================
// CGuiScreenPauseSettings::InitOutro
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
void CGuiScreenPauseSettings::InitOutro()
{
    // apply new gameplay settings
    //
    rAssert( m_pMenu != NULL );
    bool isSettingOn = false;

#ifndef RAD_WIN32
    isSettingOn = (m_pMenu->GetSelectionValue( MENU_ITEM_INVERT_CAM_CONTROL ) == 1);
    GetSuperCamManager()->GetSCC( 0 )->EnableInvertedCamera( isSettingOn );
#endif

    isSettingOn = (m_pMenu->GetSelectionValue( MENU_ITEM_JUMP_CAMERAS ) == 1);
    GetSuperCamManager()->GetSCC( 0 )->EnableJumpCams( isSettingOn );

    isSettingOn = (m_pMenu->GetSelectionValue( MENU_ITEM_INTERSECT_NAV_SYSTEM ) == 1);
    GetCharacterSheetManager()->SetNavSystemOn( isSettingOn );

    isSettingOn = (m_pMenu->GetSelectionValue( MENU_ITEM_RADAR ) == 1);
    GetGuiSystem()->SetRadarEnabled( isSettingOn );

#ifndef RAD_WIN32
    isSettingOn = (m_pMenu->GetSelectionValue( MENU_ITEM_VIBRATION ) == 1);
    GetInputManager()->SetRumbleEnabled( isSettingOn );
#endif

    isSettingOn = (m_pMenu->GetSelectionValue( MENU_ITEM_TUTORIAL ) == 1) 
#ifdef RAD_WIN32
                  && !(GetInputManager()->GetController(0)->IsTutorialDisabled())
#endif                  
                  ;
    GetTutorialManager()->EnableTutorialEvents( isSettingOn );
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenPauseSettings::UpdateCameraSelections()
{
    rAssert( m_pMenu );

    // set current camera selection mode
    //
    bool isPlayerInCar = GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar();
    m_currentCameraSelectionMode = isPlayerInCar ? CAMERA_SELECTION_FOR_DRIVING : CAMERA_SELECTION_FOR_WALKING;

    // get current active camera
    //
    SuperCam* currentSuperCam = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam();
    rAssert( currentSuperCam );
    SuperCam::Type currentSuperCamType = currentSuperCam->GetType();

    int currentCameraIndex = -1;

    // check to see if current camera is in the selection list
    //
    for( int i = 0; i < m_numCameraSelections[ m_currentCameraSelectionMode ]; i++ )
    {
        if( m_cameraSelections[ m_currentCameraSelectionMode ][ i ] == currentSuperCamType )
        {
            currentCameraIndex = i;

            break;
        }
    }

    if( currentCameraIndex == -1 )
    {
        // current camera not found in selection list, disable camera selection
        //
        m_pMenu->SetMenuItemEnabled( MENU_ITEM_CAMERA, false );

        m_currentCameraSelectionMode = CAMERA_SELECTION_NOT_AVAILABLE;

        return;
    }

    m_pMenu->SetMenuItemEnabled( MENU_ITEM_CAMERA, true );

    // set number of camera selections available
    //
    m_pMenu->SetSelectionValueCount( MENU_ITEM_CAMERA,
                                     m_numCameraSelections[ m_currentCameraSelectionMode ] );

    // update camera names
    //
    for( int i = 0; i < m_numCameraSelections[ m_currentCameraSelectionMode ]; i++ )
    {
        GuiMenuItem* menuItemCamera = m_pMenu->GetMenuItem( MENU_ITEM_CAMERA );
        rAssert( menuItemCamera != NULL );

        SuperCam* superCam = GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( m_cameraSelections[ m_currentCameraSelectionMode ][ i ] );
        rAssert( superCam );

        HeapMgr()->PushHeap( GMA_LEVEL_HUD );

        P3D_UNICODE* stringBuffer = GetTextBibleString( superCam->GetName() );
        rAssert( stringBuffer );

        UnicodeString unicodeString;
        unicodeString.ReadUnicode( static_cast<UnicodeChar*>( stringBuffer ) );

        Scrooby::Text* menuItemValue = dynamic_cast<Scrooby::Text*>( menuItemCamera->GetItemValue() );
        rAssert( menuItemValue != NULL );
        menuItemValue->SetString( i, unicodeString );

        HeapMgr()->PopHeap(GMA_LEVEL_HUD);
    }

    // set current camera selection
    //
    m_pMenu->SetSelectionValue( MENU_ITEM_CAMERA, currentCameraIndex );
}

