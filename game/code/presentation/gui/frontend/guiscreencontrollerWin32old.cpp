//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenController
//
// Description: Implementation of the CGuiScreenController class.
//
// Authors:     Tony Chu,
//              Neil Haran
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreencontrollerWin32.h>
#include <presentation/gui/guimenu.h>

#include <data/config/gameconfigmanager.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <events/eventmanager.h>

#include <raddebug.hpp>     // Foundation
#include <group.h>
#include <layer.h>
#include <page.h>
#include <screen.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* szMainControllerPage = "ControllerPC";
const char* szKeyboardMousePage  = "PCKeyboardConfig";
const char* szGamepadPage        = "PCGamepadConfig";

const int NUM_DISPLAY_MODES = 2;
extern const char* gVirtualInputs[];


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
const tColour DEFAULT_SELECTED_ITEM_COLOUR( 255, 255, 0 );
const tColour DEFAULT_INPUTMAPPED_ITEM_COLOUR( 255, 0, 0 );   // the same as in guiscreenmemorycard.cpp


//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenController::CGuiScreenController
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
CGuiScreenController::CGuiScreenController
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
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
    SetSingleItemMenuPage( m_pMenuLabels, NUM_MAINMENU_LABELS, szMainControllerPage, "Menu", "Label", ALL_ATTRIBUTES_ON );
    m_numControllerGroups = 1; // The first page is the MENU_PAGE_MAIN with an index of 0.

    // Add master list items
    SetSingleItemMenuPage( m_pKMLabels[MASTER_LIST], NUM_KEYBOARDMOUSE_LABELS, szKeyboardMousePage, "Menu", "Label", ALL_ATTRIBUTES_OFF );
    SetSingleItemMenuPage( m_pGamepadLabels[MASTER_LIST], NUM_GAMEPAD_LABELS, szGamepadPage, "Menu", "Label", ALL_ATTRIBUTES_OFF );
    // Add map items
    SetSingleItemMenuPage( m_pKMLabels[MAP_PRIMARY], NUM_KEYBOARDMOUSE_LABELS, szKeyboardMousePage, "Map1", "Map1Label" );
    SetSingleItemMenuPage( m_pKMLabels[MAP_SECONDARY], NUM_KEYBOARDMOUSE_LABELS, szKeyboardMousePage, "Map2", "Map2Label" );
    SetSingleItemMenuPage( m_pGamepadLabels[MAP_PRIMARY], NUM_GAMEPAD_LABELS, szGamepadPage, "Map1", "Map1Label" );
    SetSingleItemMenuPage( m_pGamepadLabels[MAP_SECONDARY], NUM_GAMEPAD_LABELS, szGamepadPage, "Map2", "Map2Label" );


    //Turn off the controller page that shows up for the consoles.
    SetPageVisiblility( "Controller", false );
    SetPageVisiblility( szMainControllerPage, true );
}


//===========================================================================
// CGuiScreenController::~CGuiScreenController
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

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
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
                case MENU_PAGE_KM:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on keyboard Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage] )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_KEYBOARDMOUSE_LABELS-1));
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+NUM_KEYBOARDMOUSE_LABELS )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_KEYBOARDMOUSE_LABELS*2)-1);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }
                    
                    break;
                case MENU_PAGE_GAMEPAD:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on gamepad Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage] )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_GAMEPAD_LABELS-1));
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+NUM_GAMEPAD_LABELS )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+(NUM_GAMEPAD_LABELS*2)-1);
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
                case MENU_PAGE_KM:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on keyboard Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_KEYBOARDMOUSE_LABELS-1) )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_KEYBOARDMOUSE_LABELS*2)-1 )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+NUM_KEYBOARDMOUSE_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }
                    
                    break;
                case MENU_PAGE_GAMEPAD:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On the first element on gamepad Map 1
                        if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_GAMEPAD_LABELS-1) )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else if( menuItem == m_menuGroupStartIndex[m_currentPage]+(NUM_GAMEPAD_LABELS*2)-1 )
                        {
                            m_pMenu->Reset(m_menuGroupStartIndex[m_currentPage]+NUM_GAMEPAD_LABELS);
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
                case MENU_PAGE_KM:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On Map 1
                        if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_KEYBOARDMOUSE_LABELS )
                        {
                            m_pMenu->Reset(menuItem+NUM_KEYBOARDMOUSE_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else // On Map 2
                        {
                            m_pMenu->Reset(menuItem-NUM_KEYBOARDMOUSE_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                    }

                    break;
                case MENU_PAGE_GAMEPAD:
                    {
                        int menuItem = m_pMenu->GetSelection();
                        // On Map 1
                        if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_GAMEPAD_LABELS )
                        {
                            m_pMenu->Reset(menuItem+NUM_GAMEPAD_LABELS);
                            GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                            return;
                        }
                        else // On Map 2
                        {
                            m_pMenu->Reset(menuItem-NUM_GAMEPAD_LABELS);
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
                switch( m_currentPage ) 
                {
                case MENU_PAGE_MAIN:
                    {
                        switch( param1 )
                        {
                        case MENU_ITEM_KEYMOUSECONFIG:
                            {
                                GetInputManager()->GetFEMouse()->SetClickStopMode( true );
                                SetPageVisiblility( szMainControllerPage, false );
                                SetPageVisiblility( szKeyboardMousePage, true );
                                m_currentPage = MENU_PAGE_KM;
                                break;
                            }
                        case MENU_ITEM_GAMEPADCONFIG:
                            {
                                GetInputManager()->GetFEMouse()->SetClickStopMode( true );
                                SetPageVisiblility( szMainControllerPage, false );
                                SetPageVisiblility( szGamepadPage, true );
                                m_currentPage = MENU_PAGE_GAMEPAD;
                                break;
                            }
                        default:
                            {
                                break;
                            }
                        }
                    } break;
                case MENU_PAGE_KM:
                    {
                        // if we're on Map 1 or Map 2
                        if( param1 >= (unsigned int)m_menuGroupStartIndex[m_currentPage] )
                        {
                            int menuItem = param1;
                            // On Map 1
                            if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_KEYBOARDMOUSE_LABELS )
                            {
                                // We know what map it is so just convert this into a value between 0 - NUM_KEYBOARDMOUSE_LABELS-1
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage];
                                RemapButton( MAP_PRIMARY, KEYBOARD, menuItem );
                                break;
                            }
                            else // On Map 2
                            {
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage] - NUM_KEYBOARDMOUSE_LABELS;
                                RemapButton( MAP_SECONDARY, KEYBOARD, menuItem );
                                break;
                            }
                        }
                    } break;
                case MENU_PAGE_GAMEPAD:
                    {
                        if( param1 >= (unsigned int)m_menuGroupStartIndex[m_currentPage] )
                        {
                            int menuItem = param1;
                            // On Map 1
                            if( menuItem < m_menuGroupStartIndex[m_currentPage] + NUM_GAMEPAD_LABELS )
                            {
                                // We know what map it is so just convert this into a value between 0 - NUM_KEYBOARDMOUSE_LABELS-1
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage];
                                RemapButton( MAP_PRIMARY, GAMEPAD, menuItem );
                                break;
                            }
                            else // On Map 2
                            {
                                menuItem = menuItem - m_menuGroupStartIndex[m_currentPage] - NUM_GAMEPAD_LABELS;
                                RemapButton( MAP_SECONDARY, GAMEPAD, menuItem );
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
                    case MENU_PAGE_KM:
                        GetInputManager()->GetFEMouse()->SetClickStopMode( false );
                        SetPageVisiblility( szKeyboardMousePage, false );
                        SetPageVisiblility( szMainControllerPage, true );
                        m_currentPage = MENU_PAGE_MAIN;
                        bRelayMessage = false;
                        break;
                    case MENU_PAGE_GAMEPAD:
                        GetInputManager()->GetFEMouse()->SetClickStopMode( false );
                        SetPageVisiblility( szGamepadPage, false );
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
    if( bRelayMessage)
	    CGuiScreen::HandleMessage( message, param1, param2 );
}

void CGuiScreenController::RemapButton( eControllerColumn column, eControllerType type, int menuItem )
{
    switch(m_currentPage) 
    {
    case MENU_PAGE_KM:
            m_currentTextLabel = m_pKMLabels[column][menuItem];
    	break;
    case MENU_PAGE_GAMEPAD:
            m_currentTextLabel = m_pGamepadLabels[column][menuItem];
    	break;
    default:
        break;
    }
    if( m_currentTextLabel )
        m_currentTextLabel->SetColour(DEFAULT_INPUTMAPPED_ITEM_COLOUR);
    GetInputManager()->GetController(m_currentControllerID)->RemapButton( type,
                                                                          (column == MAP_PRIMARY) ? SLOT_PRIMARY : SLOT_SECONDARY,
                                                                          GetVirtualKey(menuItem), 
                                                                          this );
    GetInputManager()->GetFEMouse()->SetSelectable( false );
    m_bMapInput = true;
}

//===========================================================================
// CGuiScreenController::CheckCursorAgainstHotspots
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
    eFEHotspotType hotSpotType = HOTSPOT_NONE;
    CGuiMenu* pCurrentMenu = HasMenu();
    int numMenuItems = 0;
    GuiMenuItem* pMenuItem = NULL;


    if( pCurrentMenu )
    {
        numMenuItems = pCurrentMenu->GetNumItems();

        for( int i = 0; i < numMenuItems; i++ )
        {
            bool bIsMenuItemEnabled = pCurrentMenu->IsMenuItemEnabled(i);
            pMenuItem = pCurrentMenu->GetMenuItem( i );

            if( pMenuItem && bIsMenuItemEnabled )
            {
                if( pMenuItem->GetItem()->IsVisible() )
                {
                    // Just tests if the point is in the bounding rect of the sprite.
                    if( pMenuItem->GetItem()->IsPointInBoundingRect( x, y ) )
                    {
                        //rDebugPrintf( "Cursor is inside Sprite %d rectangle!\n", i );
                        pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_OVER, i );
                        hotSpotType = HOTSPOT_BUTTON;

                        //After taking care of all the events for this menu item, just bail out.
                        break;
                    }
                }
            }
        }             
    }

    return hotSpotType;
}

void CGuiScreenController::OnButtonMapped( const char* szNewInput )
{
    if( szNewInput )
    {
        if( m_currentTextLabel )
            m_currentTextLabel->SetString(0, szNewInput);
    }
    else //user cancelled the mapper event.
    {
        m_bDisableBack = true;
    }

    // Reset flags we changed.
    if( m_currentTextLabel )
        m_currentTextLabel->SetColour(DEFAULT_SELECTED_ITEM_COLOUR);

    //Update the page labels.
    UpdatePageLabels(m_currentPage);
    GetInputManager()->GetFEMouse()->SetSelectable( true );
    m_bMapInput = false;
    m_currentTextLabel = NULL;
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
    UpdatePageLabels(MENU_PAGE_KM);
    UpdatePageLabels(MENU_PAGE_GAMEPAD);
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

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void CGuiScreenController::UpdatePageLabels( eMenuPages page )
{
    UserController* pController = GetInputManager()->GetController(m_currentControllerID);
    const char* szNotAssigned = "---";
    switch( page )
    {
    case MENU_PAGE_KM:
        {
            RealController* pKeyBoard = pController->GetRealController( KEYBOARD );
            
            int diMap[NUM_MAPS][NUM_KEYBOARDMOUSE_LABELS] = 
            {
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::MoveUp ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::MoveDown ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::MoveLeft ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::MoveRight ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::CameraFirstPerson ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::Attack ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::Jump ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::Sprint ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::DoAction ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::CameraZoom ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::Accelerate ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::HandBrake ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::Horn ),
                pKeyBoard->GetMap( SLOT_PRIMARY, InputManager::ResetCar ),

                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::MoveUp ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::MoveDown ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::MoveLeft ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::MoveRight ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::CameraFirstPerson ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::Attack ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::Jump ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::Sprint ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::DoAction ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::CameraZoom ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::Accelerate ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::HandBrake ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::Horn ),
                pKeyBoard->GetMap( SLOT_SECONDARY, InputManager::ResetCar ),
            };

            //Input::INVALID_CONTROLLERID

            for( int column = MAP_PRIMARY, mapCount = 0; column < NUM_COLUMNS; column++, mapCount++ )
            {
                for( int kmItem = 0; kmItem < NUM_KEYBOARDMOUSE_LABELS; kmItem++ )
                {
                    RADCONTROLLER pRadController = pKeyBoard->getController();
                    IRadControllerInputPoint* pInputPoint = NULL;
                    const char* szText = szNotAssigned;
                    int vKey = diMap[mapCount][kmItem];
                    if( vKey != Input::INVALID_CONTROLLERID )
                    {
                        pInputPoint = pRadController->GetInputPointByTypeAndIndex("Button", VirtualKeyToIndex[vKey]);
                        if( pInputPoint ) szText = pInputPoint->GetName();
                    }

                    m_pKMLabels[column][kmItem]->SetString(0, szText);
                }
            }
        } break;
    case MENU_PAGE_GAMEPAD:
        {
            RealController* pGamepad = pController->GetRealController( GAMEPAD );
            RADCONTROLLER pRadController = pGamepad->getController();
            if( pGamepad && pRadController )
            {
                int diMap[NUM_MAPS][NUM_GAMEPAD_LABELS] = 
                {
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::CameraFirstPerson ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::Attack ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::Jump ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::Sprint ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::DoAction ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::CameraZoom ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::Accelerate ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::HandBrake ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::Horn ),
                    pGamepad->GetMap( SLOT_PRIMARY, InputManager::ResetCar ),

                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::CameraFirstPerson ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::Attack ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::Jump ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::Sprint ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::DoAction ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::CameraZoom ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::Accelerate ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::HandBrake ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::Horn ),
                    pGamepad->GetMap( SLOT_SECONDARY, InputManager::ResetCar ),
                };

                //Input::INVALID_CONTROLLERID

                for( int column = MAP_PRIMARY, mapCount = 0; column < NUM_COLUMNS; column++, mapCount++ )
                {
                    for( int gamepadItem = 0; gamepadItem < NUM_GAMEPAD_LABELS; gamepadItem++ )
                    {
                        IRadControllerInputPoint* pInputPoint = NULL;
                        const char* szText = szNotAssigned;
                        int vKey = diMap[mapCount][gamepadItem];
                        if( vKey != Input::INVALID_CONTROLLERID )
                        {
                            pInputPoint = pRadController->GetInputPointByTypeAndIndex("Button", VirtualJoyKeyToIndex[vKey]);
                            if( pInputPoint ) szText = pInputPoint->GetName();
                        }

                        m_pGamepadLabels[column][gamepadItem]->SetString(0, szText);
                    }
                }
            }
            else
            {
                m_pMenu->SetMenuItemEnabled( MENU_ITEM_GAMEPADCONFIG, false );
            }
        } break;
    default:
        break;
    }

}

void CGuiScreenController::SetSingleItemMenuPage( Scrooby::Text** pLabels,
                                                  int numMenuItems,
                                                  const char* strPage,
                                                  char* strGroup,
                                                  char* szLabel,
                                                  int attributes )
{
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( strPage );
    rAssert( pPage );

    
    // if this is a controller page set the start index of its menu items.
    if( !strcmp( strGroup, "Map1" ) && m_numControllerGroups < NUM_MENU_PAGES )
    {
        m_menuGroupStartIndex[m_numControllerGroups++] = m_pMenu->GetNumItems();
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
            m_pMenu->AddMenuItem( pMenuItemText,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  attributes );
            pLabels[itemCount] = pMenuItemText;
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
    if( !strcmp( strPage, szMainControllerPage ) )
    {
        for( int i = 0; i < NUM_MAINMENU_LABELS; i++ )
            m_pMenuLabels[i]->SetVisible( bVisible );
        if( bVisible )
            m_pMenu->Reset( MENU_ITEM_KEYMOUSECONFIG );
    }
    else if( !strcmp( strPage, szKeyboardMousePage ) )
    {
        for( int columnCount = 0; columnCount < NUM_COLUMNS; columnCount++ )
            for( int labelCount = 0; labelCount < NUM_KEYBOARDMOUSE_LABELS; labelCount++ )
                m_pKMLabels[columnCount][labelCount]->SetVisible( bVisible );
        if( bVisible )
            m_pMenu->Reset( m_menuGroupStartIndex[MENU_PAGE_KM] );
    }
    else if( !strcmp( strPage, szGamepadPage ) )
    {
        for( int columnCount = 0; columnCount < NUM_COLUMNS; columnCount++ )
             for( int labelCount = 0; labelCount < NUM_GAMEPAD_LABELS; labelCount++ )
                m_pGamepadLabels[columnCount][labelCount]->SetVisible( bVisible );
        if( bVisible )
            m_pMenu->Reset( m_menuGroupStartIndex[MENU_PAGE_GAMEPAD] );
    }
}

int CGuiScreenController::GetVirtualKey( int menuItem )
{
    // We have to start the menuitem off at the correct index.
    // This is simple, just add the menuitem with the correct start index.
    switch(m_currentPage) 
    {
    case MENU_PAGE_KM:
        menuItem += KM_MOVEUP;
        break;
    case MENU_PAGE_GAMEPAD:
        menuItem += GAMEPAD_FIRSTPERSON;
        break;
    default:
        break;
    }

    switch(menuItem) 
    {
    case KM_MOVEUP:
        return InputManager::MoveUp;
    case KM_MOVEDOWN:
        return InputManager::MoveDown;
    case KM_MOVELEFT:
        return InputManager::MoveLeft;
    case KM_MOVERIGHT:
        return InputManager::MoveRight;
    case KM_FIRSTPERSON: 
    case GAMEPAD_FIRSTPERSON:
        return InputManager::CameraFirstPerson;
    case KM_ATTACK: 
    case GAMEPAD_ATTACK:
        return InputManager::Attack;
    case KM_JUMP: 
    case GAMEPAD_JUMP:
        return InputManager::Jump;
    case KM_SPRINT: 
    case GAMEPAD_SPRINT:
        return InputManager::Sprint;
    case KM_DOACTION: 
    case GAMEPAD_DOACTION:
        return InputManager::DoAction;
    case KM_CAMERAZOOM: 
    case GAMEPAD_CAMERAZOOM:
        return InputManager::CameraZoom;
    case KM_ACCELERATE: 
    case GAMEPAD_ACCELERATE:
        return InputManager::Accelerate;
    case KM_EBRAKE: 
    case GAMEPAD_EBRAKE:
        return InputManager::HandBrake;
    case KM_HORN: 
    case GAMEPAD_HORN:
        return InputManager::Horn;
    case KM_RESETCAR: 
    case GAMEPAD_RESETCAR:
        return InputManager::ResetCar;
    default:
        return 0;
    }
}