/******************************************************************************
    File:		GuiScreenControllerWin32.cpp
    Desc:		Defines the CGuiScreenController class.
    Author:		Neil Haran
    Date:		July 31, 2003
    History:
*****************************************************************************/
#include <presentation/gui/frontend/guiscreencontrollerWin32.h>
#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guimanager.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <data/config/gameconfigmanager.h>
#include <presentation/tutorialmanager.h>
#include <events/eventmanager.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <group.h>
#include <layer.h>
#include <page.h>
#include <screen.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* szMainControllerPage = "ControllerPC";
const char* szCCPage             = "CharacterControls";
const char* szVCPage             = "VehicleControls";
const char* szGSPage             = "GameSettings";
const char* szNotAssigned        = "---";

const int NUM_DISPLAY_MODES = 2;
const int MAX_INPUTNAME_LENGTH = 10;

const tColour DEFAULT_SELECTED_ITEM_COLOUR( 255, 255, 0 );
const tColour DEFAULT_INPUTMAPPED_ITEM_COLOUR( 255, 0, 0 );   // the same as in guiscreenmemorycard.cpp

const float SLIDER_ICON_SCALE = 0.5f;

/******************************************************************************
    Construction/Destruction
*****************************************************************************/
CGuiScreenController::CGuiScreenController( Scrooby::Screen* pScreen, CGuiEntity* pParent )
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_CONTROLLER ),
    m_pMenu( NULL ),
    m_currentPage( MENU_PAGE_MAIN ),
    m_currentControllerID( 0 ),
    m_bMapInput( false ),
    m_currentTextLabel(NULL),
    m_bDisableBack(false),
    m_numControllerGroups(0)
{
    memset( m_menuGroupStartIndex, 0 , sizeof( m_menuGroupStartIndex ) );
    m_pMenu = new(GMA_LEVEL_FE) CGuiMenu( this, NUM_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // Add main menu items
    SetGroups( m_pMenuLabels, NUM_MAINMENU_LABELS, szMainControllerPage, "Menu", "Label", ALL_ATTRIBUTES_ON );
    m_numControllerGroups = 1; // The first page is the MENU_PAGE_MAIN with an index of 0.

    // Add map items
    SetGroups( m_pCCLabels[MAP_PRIMARY], NUM_CHARACTERCONTROL_LABELS, szCCPage, "Map1", "Map1Label" );
    SetGroups( m_pCCLabels[MAP_SECONDARY], NUM_CHARACTERCONTROL_LABELS, szCCPage, "Map2", "Map2Label" );
    SetGroups( m_pVCLabels[MAP_PRIMARY], NUM_VEHICLECONTROL_LABELS, szVCPage, "Map1", "Map1Label" );
    SetGroups( m_pVCLabels[MAP_SECONDARY], NUM_VEHICLECONTROL_LABELS, szVCPage, "Map2", "Map2Label" );

    // Add game setting items
    SetGroups( m_pMSLabels, NUM_GAMESETTING_LABELS, szGSPage, "Menu", 
               "Label", (SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED) );

    //Turn off the controller page that shows up for the consoles.
    SetPageVisiblility( "Controller", false );
    SetPageVisiblility( szMainControllerPage, true );
    SetPageVisiblility( szCCPage, false );
    SetPageVisiblility( szVCPage, false );
    SetPageVisiblility( szGSPage, false );

    // Size the slider icons in the game settings page.
    Scrooby::Group* group = pScreen->GetPage( szGSPage )->GetGroup( "Menu" );
    if( group != NULL )
    {
        group->GetSprite( "MouseXSliderIcon" )->ScaleAboutCenter( SLIDER_ICON_SCALE );
        group->GetSprite( "MouseYSliderIcon" )->ScaleAboutCenter( SLIDER_ICON_SCALE );
        group->GetSprite( "WheelSliderIcon" )->ScaleAboutCenter( SLIDER_ICON_SCALE );
    }
}

CGuiScreenController::~CGuiScreenController()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}

//===========================================================================
// CGuiScreenController::HandleMessage
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
void CGuiScreenController::HandleMessage
(
    eGuiMessage message, 
    unsigned int param1,
    unsigned int param2 
)
{
    static bool bRelayMessage = true;

    if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        switch( param1 )
        {
        case PROMPT_CONFIRM_RESTOREALLDEFAULTS:
            {
                switch( param2 )
                {
                case (CGuiMenuPrompt::RESPONSE_YES):
                    {
                        GetInputManager()->GetController(m_currentControllerID)->LoadDefaults();
                        this->InitIntro();
                        this->ReloadScreen();

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
            break;
        default: break;
        }
    }
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
        case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                rAssert( m_pMenu );
                GuiMenuItem* currentItem = m_pMenu->GetMenuItem( param1 );
                rAssert( currentItem );

                switch( m_currentPage ) 
                {
                case MENU_PAGE_GAMESETTINGS:
                    {
                        // convert to Gamesetting specific items.
                        int menuItem = param1-NUM_CHARACTERCONTROL_LABELS-NUM_VEHICLECONTROL_LABELS;
                        UserController* pController = GetInputManager()->GetController( m_currentControllerID );

                        switch( menuItem ) 
                        {
                        case GS_MOUSELOOK:
                            {
                                pController->SetMouseLook( m_pMenu->GetSelectionValue( param1 ) == PROMPT_YES ); 

                                GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::ON_FOOT_CAM, SuperCamCentral::CUT, 0 );
                                GetSuperCamManager()->GetSCC( 0 )->Update( 0 );
                            }
                        	break;
                        case GS_MOUSESENSITIVITY_X:
                            {
                                pController->SetMouseSensitivityX( m_pMenu->GetMenuItem( param1 )->m_slider.m_value );
                            }
                            break;
                        case GS_MOUSESENSITIVITY_Y:
                            {
                                pController->SetMouseSensitivityY( m_pMenu->GetMenuItem( param1 )->m_slider.m_value );
                            }
                            break;
                        case GS_INVERTMOUSEX:
                            {
                                pController->SetInvertMouseX( m_pMenu->GetSelectionValue( param1 ) == PROMPT_YES ); 
                            }
                            break;
                        case GS_INVERTMOUSEY:
                            {
                                pController->SetInvertMouseY( m_pMenu->GetSelectionValue( param1 ) == PROMPT_YES ); 
                            }
                            break;
                        case GS_FORCEFEEDBACK:
                            {
                                pController->SetForceFeedback( m_pMenu->GetSelectionValue( param1 ) == PROMPT_YES ); 
                            }
                            break;
                        case GS_WHEELSENSITIVITY:
                            {
                                pController->SetWheelSensitivityX( m_pMenu->GetMenuItem( param1 )->m_slider.m_value );
                            }
                            break;
                        default:
                            break;
                        }
                    }
                	break;
                default:
                    break;
                }

                break;
            }
        case GUI_MSG_CONTROLLER_UP:
            {
                // Basically a wrap around system when you reach the top.
                // Just jumps to the last element.
                switch(m_currentPage) 
                {
                case MENU_PAGE_MAIN:
                    if( m_pMenu->GetSelection() == m_menuGroupStartIndex[m_currentPage] )
                    {
                        m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_MAINMENU_LABELS-1));
                        GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                        return;
                    }
                    break;
                case MENU_PAGE_CHARACTERCONTROLS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on keyboard Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage] )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_CHARACTERCONTROL_LABELS-1));
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+NUM_CHARACTERCONTROL_LABELS )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_CHARACTERCONTROL_LABELS*2)-1);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }

                    break;
                case MENU_PAGE_VEHICLECONTROLS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on gamepad Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage] )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_VEHICLECONTROL_LABELS-1));
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+NUM_VEHICLECONTROL_LABELS )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_VEHICLECONTROL_LABELS*2)-1);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }
                case MENU_PAGE_GAMESETTINGS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on gamepad Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage] )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_GAMESETTING_LABELS-1));
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        case GUI_MSG_CONTROLLER_DOWN:
            {
                // Basically a wrap around system when you reach the bottom.
                // Just jumps to the first element.
                switch(m_currentPage) 
                {
                case MENU_PAGE_MAIN:
                    if( m_pMenu->GetSelection() == m_menuGroupStartIndex[m_currentPage]+(NUM_MAINMENU_LABELS-1) )
                    {
                        m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]);
                        GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                        return;
                    }
                    break;
                case MENU_PAGE_CHARACTERCONTROLS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on keyboard Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_CHARACTERCONTROL_LABELS-1) )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_CHARACTERCONTROL_LABELS*2)-1 )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+NUM_CHARACTERCONTROL_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }

                    break;
                case MENU_PAGE_VEHICLECONTROLS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on gamepad Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_VEHICLECONTROL_LABELS-1) )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_VEHICLECONTROL_LABELS*2)-1 )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+NUM_VEHICLECONTROL_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }

                    break;

                case MENU_PAGE_GAMESETTINGS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on gamepad Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage]+NUM_GAMESETTING_LABELS-1 )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        case GUI_MSG_CONTROLLER_LEFT:
        case GUI_MSG_CONTROLLER_RIGHT:
            {
                // Basically a wrap around system when you go left or right.
                switch(m_currentPage) 
                { 
                case MENU_PAGE_CHARACTERCONTROLS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On Map 1
                        if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_CHARACTERCONTROL_LABELS )
                        {
                            m_pMenu->Reset(menuItem+NUM_CHARACTERCONTROL_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else // On Map 2
                        {
                            m_pMenu->Reset(menuItem-NUM_CHARACTERCONTROL_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }

                    break;
                case MENU_PAGE_VEHICLECONTROLS:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On Map 1
                        if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_VEHICLECONTROL_LABELS )
                        {
                            m_pMenu->Reset(menuItem+NUM_VEHICLECONTROL_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else // On Map 2
                        {
                            m_pMenu->Reset(menuItem-NUM_VEHICLECONTROL_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }

                    break;
                default:
                    break;
                }                
                break;
            }
        case GUI_MSG_MENU_SELECTION_MADE:
            {
                // guiscreencontrollerwin32 is a circumvention of normal ENTER/EXIT
                // WINDOW front-end scheme. Since we are disabling the mouse 
                // each time a selection has been made, we need to re-enable
                // it for this situation, when you transit from some screen to 
                // one of these screens, so the mouse continues to function normally.
                //
                GetInputManager()->GetFEMouse()->SetSelectable( true );

                switch( m_currentPage ) 
                {
                case MENU_PAGE_MAIN:
                    {
                        switch( param1 )
                        {
                        case MENU_ITEM_CHARACTERCONTROLS:
                            {
                                GetInputManager()->GetFEMouse()->SetClickStopMode( true );
                                SetPageVisiblility( szMainControllerPage, false );
                                SetPageVisiblility( szCCPage, true );
                                m_currentPage = MENU_PAGE_CHARACTERCONTROLS;
                                break;
                            }
                        case MENU_ITEM_VEHICLECONTROLS:
                            {
                                GetInputManager()->GetFEMouse()->SetClickStopMode( true );
                                SetPageVisiblility( szMainControllerPage, false );
                                SetPageVisiblility( szVCPage, true );
                                m_currentPage = MENU_PAGE_VEHICLECONTROLS;
                                break;
                            }
                        case MENU_ITEM_GAMESETTINGS:
                            {
                                SetPageVisiblility( szMainControllerPage, false );
                                SetPageVisiblility( szGSPage, true );
                                m_currentPage = MENU_PAGE_GAMESETTINGS;
                                break;
                            }
                        case MENU_ITEM_RESTOREALLDEFAULTS:
                            {
                                rAssert( m_guiManager );
                                GetInputManager()->GetFEMouse()->SetSelectable( true );
                                m_guiManager->DisplayPrompt( PROMPT_CONFIRM_RESTOREALLDEFAULTS, this );
                                break;
                            }
                        default:
                            {
                                break;
                            }
                        }
                    } break;
                case MENU_PAGE_CHARACTERCONTROLS:
                    {
                        // if we're on Map 1 or Map 2
                        if( param1 >= (unsigned int)m_menuGroupStartIndex[m_currentPage] )
                        {
                            int menuItem = param1;
                            // On Map 1
                            if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_CHARACTERCONTROL_LABELS )
                            {
                                // We know what map it is so just convert this into a value between 0 - NUM_CHARACTERCONTROL_LABELS-1
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage];
                                RemapButton( MAP_PRIMARY, menuItem );
                                break;
                            }
                            else // On Map 2
                            {
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage] - NUM_CHARACTERCONTROL_LABELS;
                                RemapButton( MAP_SECONDARY, menuItem );
                                break;
                            }
                        }
                    } break;
                case MENU_PAGE_VEHICLECONTROLS:
                    {
                        if( param1 >= (unsigned int)m_menuGroupStartIndex[m_currentPage] )
                        {
                            int menuItem = param1;
                            // On Map 1
                            if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_VEHICLECONTROL_LABELS )
                            {
                                // We know what map it is so just convert this into a value between 0 - NUM_CHARACTERCONTROL_LABELS-1
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage];
                                RemapButton( MAP_PRIMARY, menuItem );
                                break;
                            }
                            else // On Map 2
                            {
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage] - NUM_VEHICLECONTROL_LABELS;
                                RemapButton( MAP_SECONDARY, menuItem );
                                break;
                            }
                        }
                    } break;
                default: break;
                }
                break;
            }
        case GUI_MSG_CONTROLLER_BACK:
            {
                // mapping an input, so any key to trigger this event should be ignored.
                if( !m_bDisableBack )
                {
                    switch( m_currentPage ) 
                    {
                    case MENU_PAGE_CHARACTERCONTROLS:
                        GetInputManager()->GetFEMouse()->SetClickStopMode( false );
                        SetPageVisiblility( szCCPage, false );
                        SetPageVisiblility( szMainControllerPage, true );
                        m_currentPage = MENU_PAGE_MAIN;
                        bRelayMessage = false;
                        break;
                    case MENU_PAGE_VEHICLECONTROLS:
                        GetInputManager()->GetFEMouse()->SetClickStopMode( false );
                        SetPageVisiblility( szVCPage, false );
                        SetPageVisiblility( szMainControllerPage, true );
                        m_currentPage = MENU_PAGE_MAIN;
                        bRelayMessage = false;
                        break;
                    case MENU_PAGE_GAMESETTINGS:
                        SetPageVisiblility( szGSPage, false );
                        SetPageVisiblility( szMainControllerPage, true );
                        m_currentPage = MENU_PAGE_MAIN;
                        bRelayMessage = false;
                        break;

                    default:
                        this->StartTransitionAnimation( 560, 590 );
                        bRelayMessage = true; 
                    }
                }
                else
                {
                    m_bDisableBack = false;
                    bRelayMessage = false;
                }

                break;
            }

        default:
            {
                break;
            }
        }

        // relay message to menu
        if( m_pMenu != NULL && !m_bMapInput )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

    // Propogate the message up the hierarchy.
    //
    if( bRelayMessage )
        CGuiScreen::HandleMessage( message, param1, param2 );
    else
        bRelayMessage = true;
}

//===========================================================================
// CGuiScreen::CheckCursorAgainstHotspots
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
eFEHotspotType CGuiScreenController::CheckCursorAgainstHotspots( float x, float y )
{
    if( m_bMapInput ) return HOTSPOT_NONE;
    else return CGuiScreen::CheckCursorAgainstHotspots( x, y );
}

void CGuiScreenController::RemapButton( eControllerColumn column, int menuItem )
{
    switch(m_currentPage) 
    {
    case MENU_PAGE_CHARACTERCONTROLS:
        m_currentTextLabel = m_pCCLabels[column][menuItem];
        break;
    case MENU_PAGE_VEHICLECONTROLS:
        m_currentTextLabel = m_pVCLabels[column][menuItem];
        break;
    default:
        break;
    }
    if( m_currentTextLabel )
        m_currentTextLabel->SetColour(DEFAULT_INPUTMAPPED_ITEM_COLOUR);

    GetInputManager()->GetController(m_currentControllerID)->RemapButton( column,
                                                                          GetVirtualKey(m_currentPage, menuItem),
                                                                          this );
    GetInputManager()->GetFEMouse()->SetSelectable( false );
    m_bMapInput = true;
}

//===========================================================================
// CGuiScreenController::InitIntro
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
void CGuiScreenController::InitIntro()
{
    // Init the page labels.
    InitPageLabels(MENU_PAGE_CHARACTERCONTROLS);
    InitPageLabels(MENU_PAGE_VEHICLECONTROLS);

    UserController* pController = GetInputManager()->GetController( m_currentControllerID );
    GuiMenuItem* pMenuItem = NULL;
    // This is the real menuItem index.
    int menuItem = m_menuGroupStartIndex[MENU_PAGE_GAMESETTINGS];

    // This is relative to the enum section for GAME SETTINGS.
    int relMenuItem = m_menuGroupStartIndex[MENU_PAGE_GAMESETTINGS] - 
                      NUM_CHARACTERCONTROL_LABELS -
                      NUM_VEHICLECONTROL_LABELS;

    rAssert( m_pMenu );

    for( int i = 0; i < NUM_GAMESETTING_LABELS; i++, menuItem++, relMenuItem++ )
    {
        switch( relMenuItem ) 
        {
        case GS_MOUSELOOK:
            {
                m_pMenu->SetSelectionValue( menuItem, pController->IsMouseLookOn() == PROMPT_YES );
            }
            break;
        case GS_MOUSESENSITIVITY_X:
            {
                pMenuItem = m_pMenu->GetMenuItem( menuItem );
                rAssert( pMenuItem );
                pMenuItem->m_slider.SetValue( pController->GetMouseSensitivityX() );
            }
            break;
        case GS_MOUSESENSITIVITY_Y:
            {
                pMenuItem = m_pMenu->GetMenuItem( menuItem );
                rAssert( pMenuItem );
                pMenuItem->m_slider.SetValue( pController->GetMouseSensitivityY() );
            }
            break;
        case GS_INVERTMOUSEX:
            {
                m_pMenu->SetSelectionValue( menuItem, pController->IsMouseXInverted() == PROMPT_YES );
            }
            break;
        case GS_INVERTMOUSEY:
            {
                m_pMenu->SetSelectionValue( menuItem, pController->IsMouseYInverted() == PROMPT_YES );
            }
            break;
        case GS_FORCEFEEDBACK:
            {
                m_pMenu->SetSelectionValue( menuItem, pController->IsForceFeedbackOn() == PROMPT_YES );
            }
            break;
        case GS_WHEELSENSITIVITY:
            {
                pMenuItem = m_pMenu->GetMenuItem( menuItem );
                rAssert( pMenuItem );
                pMenuItem->m_slider.SetValue( pController->GetWheelSensitivityX() );
            }
            break;
        default:
            break;
        }
    }
}


//===========================================================================
// CGuiScreenController::InitRunning
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
void CGuiScreenController::InitRunning()
{
}


//===========================================================================
// CGuiScreenController::InitOutro
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
void CGuiScreenController::InitOutro()
{
    GetInputManager()->GetFEMouse()->SetClickStopMode( false );

    // Save the new controller mappings to the config file.
    GetGameConfigManager()->SaveConfigFile();
}

//===========================================================================
// CGuiScreenController::OnButtonMapped
//===========================================================================
// Description: Gets called back by the UserController when a button is
//              mapped.
//
// Constraints: None.
//
// Parameters:  szNewInput - The new input that just got mapped.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenController::OnButtonMapped( const char* szNewInput, eControllerType cont, int num_dirs, eDirectionType direction )
{
    char szText[255];
    memset( szText, 0, sizeof( szText) );

    if( !szNewInput ) //user cancelled the mapper event.
    {
        m_bDisableBack = true;
    }
    else
    {
        // Reset flags we changed.
        if( m_currentTextLabel )
            m_currentTextLabel->SetColour(DEFAULT_SELECTED_ITEM_COLOUR);

        strcpy( szText, szNewInput );
        GetAppropriateInputName( szText, cont, direction, num_dirs );

        //Update the page labels.
        UpdatePageLabels( m_currentPage, szText ); 
        
        //Disable the tutorials if controls are changed.
        GetInputManager()->GetController(0)->SetTutorialDisabled( true );
        GetTutorialManager()->EnableTutorialMode( false );
    }

    GetInputManager()->GetFEMouse()->SetSelectable( true );
    m_bMapInput = false;
    m_currentTextLabel = NULL;
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------
void CGuiScreenController::UpdatePageLabels( eMenuPages page, const char* szNewInput )
{
    switch( page ) 
    {
    case MENU_PAGE_CHARACTERCONTROLS:
        if( szNewInput )
        {
            //Search for this text in the array of texts to see if it already exists.
            for( int mapCount = 0; mapCount < NUM_COLUMNS; mapCount++ )
            {
                for( int itemCount = 0; itemCount < NUM_CHARACTERCONTROL_LABELS; itemCount++ )
                {
                    UnicodeString uniStrTemp = m_pCCLabels[mapCount][itemCount]->GetString();
                    char tempStr[255];
                    memset( tempStr, 0, sizeof( tempStr ) );
                    uniStrTemp.MakeAscii( tempStr, uniStrTemp.Length() + 1 );
                    if( strcmp( szNewInput, tempStr ) == 0 )
                    {
                        m_pCCLabels[mapCount][itemCount]->SetString(0, szNotAssigned);
                        break;
                    }
                }
            }
        }

    	break;
    case MENU_PAGE_VEHICLECONTROLS:
        if( szNewInput )
        {
            //Search for this text in the array of texts to see if it already exists.
            for( int mapCount = 0; mapCount < NUM_COLUMNS; mapCount++ )
            {
                for( int itemCount = 0; itemCount < NUM_VEHICLECONTROL_LABELS; itemCount++ )
                {
                    UnicodeString uniStrTemp = m_pVCLabels[mapCount][itemCount]->GetString();
                    char tempStr[255];
                    memset( tempStr, 0, sizeof( tempStr ) );
                    uniStrTemp.MakeAscii( tempStr, uniStrTemp.Length() + 1 );
                    if( strcmp( szNewInput, tempStr ) == 0 )
                    {
                        m_pVCLabels[mapCount][itemCount]->SetString(0, szNotAssigned);
                        break;
                    }
                }
            }
        }

    	break;
    default:
        break;
    }
    if( m_currentTextLabel )
        m_currentTextLabel->SetString(0, szNewInput);
}

void CGuiScreenController::InitPageLabels( eMenuPages page )
{
    UserController* pController = GetInputManager()->GetController(m_currentControllerID);

    switch( page )
    {
    case MENU_PAGE_CHARACTERCONTROLS:
        {            
            // Go through the elements and set the maps.
            for( int mapCount = 0; mapCount < NUM_COLUMNS; mapCount++ )
            {
                eControllerType controllerType;
                int ndirections;
                eDirectionType dir;

                for( int i = 0; i < NUM_CONTROLLERTYPES; i++ )
                {
                    for( int ccItem = 0; ccItem < NUM_CHARACTERCONTROL_LABELS; ccItem++ )
                    {
                        const char* szName = pController->GetMap( mapCount, 
                                                                  GetVirtualKey(MENU_PAGE_CHARACTERCONTROLS,ccItem), 
                                                                  ndirections, controllerType, dir );
                        if( szName )
                        {
                            char szText[255];
                            memset( szText, 0, sizeof( szText) );

                            strcpy( szText, szName );
                            GetAppropriateInputName( szText, controllerType, dir, ndirections );
                            m_pCCLabels[mapCount][ccItem]->SetString(0, szText);
                        } 
                        else
                        {
                            m_pCCLabels[mapCount][ccItem]->SetString(0, szNotAssigned);
                        }
                    }
                }
            }
        } break;
    case MENU_PAGE_VEHICLECONTROLS:
        {
            // Go through the elements and set the maps.
            for( int mapCount = 0; mapCount < NUM_COLUMNS; mapCount++ )
            {
                eControllerType controllerType;
                int ndirections;
                eDirectionType dir;

                for( int i = 0; i < NUM_CONTROLLERTYPES; i++ )
                {
                    for( int ccItem = 0; ccItem < NUM_VEHICLECONTROL_LABELS; ccItem++ )
                    {
                        const char* szName = pController->GetMap( mapCount, 
                                                                  GetVirtualKey(MENU_PAGE_VEHICLECONTROLS,ccItem), 
                                                                  ndirections, controllerType, dir );
                        if( szName )
                        {
                            char szText[255];
                            memset( szText, 0, sizeof( szText) );

                            strcpy( szText, szName );
                            GetAppropriateInputName( szText, controllerType, dir, ndirections );
                            m_pVCLabels[mapCount][ccItem]->SetString(0, szText);
                        } 
                        else
                        {
                            m_pVCLabels[mapCount][ccItem]->SetString(0, szNotAssigned);
                        }
                    }
                }
            }
        } break;
    default:
        break;
    }

}

void CGuiScreenController::SetGroups( Scrooby::Text** pLabels,
                                      int numMenuItems,
                                      const char* strPage,
                                      char* strGroup,
                                      char* szLabel,
                                      int attributes )
{
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( strPage );
    rAssert( pPage );


    // if this is a controller page set the start index of its menu items.
    if( m_numControllerGroups < NUM_MENU_PAGES )
    {
        if( strcmp( strGroup, "Map1" ) == 0 || strcmp( strPage, szGSPage ) == 0 )
        {
            m_menuGroupStartIndex[m_numControllerGroups++] = m_pMenu->GetNumItems();
        }
    }

    // Add main menu items
    Scrooby::Group* pMenuGroup = pPage->GetGroup( strGroup );
    for( int itemCount = 0; itemCount < numMenuItems; itemCount++ )
    {
        char objectName[ 32 ];
        sprintf( objectName, "%s%02d", szLabel, itemCount );
        Scrooby::Text* pMenuItemText = pMenuGroup->GetText( objectName );
        if( pMenuItemText != NULL )
        {
            pMenuItemText->SetTextMode( Scrooby::TEXT_WRAP );

            char itemName[32];
            sprintf( itemName, "%s%02d_Value", szLabel, itemCount );
            Scrooby::Text* pTextValue = pMenuGroup->GetText( itemName );

            sprintf( itemName, "%s%02d_ArrowL", szLabel, itemCount );
            Scrooby::Sprite* pLArrow = pMenuGroup->GetSprite( itemName );

            sprintf( itemName, "%s%02d_ArrowR", szLabel, itemCount );
            Scrooby::Sprite* pRArrow = pMenuGroup->GetSprite( itemName );

            sprintf( itemName, "%s%02d_Slider", szLabel, itemCount );
            Scrooby::Sprite* pSlider = pMenuGroup->GetSprite( itemName );

            GuiMenuItem* pMenuItem = m_pMenu->AddMenuItem( pMenuItemText,
                                                           pTextValue,
                                                           NULL,
                                                           pSlider,
                                                           pLArrow,
                                                           pRArrow,
                                                           attributes );
            pLabels[itemCount] = pMenuItemText;

            if( pSlider )
                pMenuItem->m_slider.m_type = Slider::HORIZONTAL_SLIDER_ABOUT_CENTER;
        }
        else
        {
            break;
        }
    }
}

void CGuiScreenController::SetPageVisiblility( const char* strPage, bool bVisible )
{
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( strPage );
    rAssert( pPage );
    pPage->GetLayerByIndex( 0 )->SetVisible( bVisible );

    //if it is any of our pages, set all their menu items to bVisible.
    if( strcmp( strPage, szMainControllerPage ) == 0 )
    {
        for( int i = 0; i < NUM_MAINMENU_LABELS; i++ )
            m_pMenuLabels[i]->SetVisible( bVisible );
        if( bVisible )
            m_pMenu->Reset( MENU_ITEM_CHARACTERCONTROLS );
    }
    else if( strcmp( strPage, szGSPage ) == 0 )
    {
        for( int i = 0; i < NUM_GAMESETTING_LABELS; i++ )
            m_pMSLabels[i]->SetVisible( bVisible );
        if( bVisible )
            m_pMenu->Reset( m_menuGroupStartIndex[MENU_PAGE_GAMESETTINGS] );
    }
    else if( strcmp( strPage, szCCPage ) == 0 )
    {
        for( int columnCount = 0; columnCount < NUM_COLUMNS; columnCount++ )
            for( int labelCount = 0; labelCount < NUM_CHARACTERCONTROL_LABELS; labelCount++ )
                m_pCCLabels[columnCount][labelCount]->SetVisible( bVisible );
        if( bVisible )
            m_pMenu->Reset( m_menuGroupStartIndex[MENU_PAGE_CHARACTERCONTROLS] );
    }
    else if( strcmp( strPage, szVCPage ) == 0 )
    {
        for( int columnCount = 0; columnCount < NUM_COLUMNS; columnCount++ )
            for( int labelCount = 0; labelCount < NUM_VEHICLECONTROL_LABELS; labelCount++ )
                m_pVCLabels[columnCount][labelCount]->SetVisible( bVisible );
        if( bVisible )
            m_pMenu->Reset( m_menuGroupStartIndex[MENU_PAGE_VEHICLECONTROLS] );
    }
}

int CGuiScreenController::GetVirtualKey( eMenuPages page, int menuItem )
{
    // We have to start the menuitem off at the correct index.
    // This is simple, just add the menuitem with the correct start index.
    switch(page) 
    {
    case MENU_PAGE_CHARACTERCONTROLS:
        menuItem += CC_MOVEUP;
        break;
    case MENU_PAGE_VEHICLECONTROLS:
        menuItem += VC_ACCELERATE;
        break;
    default:
        break;
    }

    switch(menuItem)                                            
    {                                                           
    case CC_MOVEUP:                                             
        return InputManager::MoveUp;                            
    case CC_MOVEDOWN:                                           
        return InputManager::MoveDown;  
    case CC_MOVELEFT:                                           
        return InputManager::MoveLeft;
    case CC_MOVERIGHT:                                         
        return InputManager::MoveRight;                                                             
    case CC_ATTACK:                                                                               
        return InputManager::Attack;                                        
    case CC_JUMP:
        return InputManager::Jump;                                          
    case CC_SPRINT:
        return InputManager::Sprint;                                                     
    case CC_ACTION:
        return InputManager::DoAction;                                      
    case CC_CAMERALEFT:                                                 
        return InputManager::CameraLeft;                                    
    case CC_CAMERARIGHT:
        return InputManager::CameraRight;
    case CC_CAMERAMOVEIN:
        return InputManager::CameraMoveIn;
    case CC_CAMERAMOVEOUT:
        return InputManager::CameraMoveOut;
    case CC_LOOKUP: 
        return InputManager::CameraLookUp;
    case CC_ZOOM:                                                                
        return InputManager::CameraZoom;

    case VC_ACCELERATE:
        return InputManager::Accelerate;  
    case VC_REVERSE:                      
        return InputManager::Reverse;  
    case VC_STEERLEFT:
        return InputManager::SteerLeft;
    case VC_STEERRIGHT:
        return InputManager::SteerRight;
    case VC_EBRAKE:                       
        return InputManager::HandBrake;   
    case VC_ACTION:
        return InputManager::GetOutCar; 
    case VC_HORN:                         
        return InputManager::Horn;  
    case VC_RESET:                        
        return InputManager::ResetCar;  
    case VC_LOOKLEFT:
        return InputManager::CameraCarLeft;  
    case VC_LOOKRIGHT:
        return InputManager::CameraCarRight;  
    case VC_LOOKUP:                       
        return InputManager::CameraCarLookUp;
    case VC_LOOKBACK:
        return InputManager::CameraCarLookBack;  
    case VC_CHANGECAMERA:
        return InputManager::CameraToggle;    
    default:                              
        return 0;                         
    }
}

void CGuiScreenController::GetAppropriateInputName( char* szInputName, 
                                                    eControllerType controllerType, 
                                                    eDirectionType direction, 
                                                    int numDirections )
{
    char szText[255];
    memset( szText, 0, sizeof( szText) );

    if( szInputName )
    {
        // Do some hacky special casing here.
        // Only done for the keyboard atm.
        switch( controllerType ) 
        {
        case KEYBOARD:
            {
                // Special cases.
                if( strcmp( szInputName, "Right Shift" ) == 0 )
                    strcpy( szInputName, "R. Shift" );
                else if( strcmp( szInputName, "Left Shift" ) == 0 )
                    strcpy( szInputName, "L. Shift" );
            }

            break;
        }
        // truncate the name to the max allowed letters.
        if( strlen(szInputName) > MAX_INPUTNAME_LENGTH )
        {
            szInputName[MAX_INPUTNAME_LENGTH-1] = '\0';
        }
        switch( controllerType ) 
        {
        case GAMEPAD:
        case STEERINGWHEEL:
            {
                switch( numDirections ) 
                {
                case 2:
                    switch( direction )
                    {
                    case DIR_UP:
                        strcat( szInputName, " +");
                        break;
                    case DIR_DOWN:
                        strcat( szInputName, " -");
                        break;
                    default:
                        break;
                    }
                	break;
                case 4: // It is a POV hat...
                    switch( direction )
                    {
                    case DIR_UP:
                        strcpy( szInputName,"POV Up" );
                        break;
                    case DIR_DOWN:
                        strcpy( szInputName,"POV Down" );
                        break;
                    case DIR_LEFT:
                        strcpy( szInputName,"POV Left" );
                        break;
                    case DIR_RIGHT:
                        strcpy( szInputName,"POV Right" );
                        break;
                    }
                	break;
                }

                switch( controllerType ) 
                {
                case GAMEPAD:
                    sprintf( szText,"J %s", szInputName );
                    strcpy( szInputName, szText );
                    break;
                case STEERINGWHEEL:
                    sprintf( szText,"S %s", szInputName );
                    strcpy( szInputName, szText );
                    break;
                }

                break;
            }
        case MOUSE:
            {
                if( strchr( szInputName, '0' ) )
                    strcpy( szInputName,"Left Mouse" );
                else if( strchr( szInputName, '1' ) )
                    strcpy( szInputName,"Right Mouse" );
                else if( strchr( szInputName, '2' ) )
                    strcpy( szInputName,"Mid Mouse" );
                else if( strchr( szInputName, '3' ) )
                    strcpy( szInputName,"Shoulder L" );
                else if( strchr( szInputName, '4' ) )
                    strcpy( szInputName,"Shoulder R" );
                else if( strchr( szInputName, '5' ) )
                    strcpy( szInputName,"Side Left" );
                else if( strchr( szInputName, '6' ) )
                    strcpy( szInputName,"Side Right" );
            }
            
            break;
        default:
            break;
        }
    }
}