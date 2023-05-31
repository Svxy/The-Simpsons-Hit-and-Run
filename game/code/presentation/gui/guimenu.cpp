//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiMenu
//
// Description: Implementation of the CGuiMenu class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions    Date            Author      Revision
//              2000/12/4       DChau       Created
//              2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiuserinputhandler.h>
#include <presentation/gui/utility/specialfx.h>

#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <events/eventmanager.h>

#include <raddebug.hpp>     // Foundation
#include <radmath/trig.hpp> // RadMath

// Scrooby
//
#include <group.h>
#include <page.h>
#include <sprite.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
const tColour DEFAULT_SELECTED_ITEM_COLOUR( 255, 255, 0 );
const tColour DEFAULT_DISABLED_ITEM_COLOUR( 128, 128, 128 );   // the same as in guiscreenmemorycard.cpp

const tColour DEFAULT_OUTLINE_COLOUR( 0, 0, 0, 192 );

const float SLIDER_FULL_RANGE_TIME = 2000; // in milliseconds
const int SELECTION_MADE_DURATION = 250; // in milliseconds

#ifdef RAD_WIN32
const float ARROW_SCALE = 0.5f;
#endif

//===========================================================================
// Public Member Functions - CGuiMenu
//===========================================================================

//===========================================================================
// CGuiMenu::CGuiMenu
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
CGuiMenu::CGuiMenu( CGuiEntity* pParent, int maxNumItems,
                                         eMenuType menuType,
                                         int specialEffects )
:   CGuiEntity( pParent ),
    m_menuType( menuType ),
    m_specialEffects( static_cast<short>( specialEffects ) ),
    m_menuItems( NULL ),
    m_numItems( 0 ),
    m_selection( NO_SELECTION ),
    m_pCursor( NULL ),
    m_isSelectionMade( false ),
    m_isHighlightEnabled( true ),
    m_highlightColour( DEFAULT_SELECTED_ITEM_COLOUR ),
    m_selectionMadeOutlineColour( tColour( 255, 0, 0, 192 ) ),
    m_isGreyOutEnabled( true ),
    m_elapsedTime( 0 ),
    m_selectionMadeElapsedTime( 0 ),
#ifdef RAD_WIN32
    m_bIsSelectionOutlined( false ),
    m_selectionOutlineColour( tColour( 0, 0, 255, 192 ) ),
#endif
    m_controllerID( -1 )
{
MEMTRACK_PUSH_GROUP( "GUIMenu" );
    rAssertMsg( ( specialEffects & 0xFFFF0000 ) == 0, "Bitmask Overflow!" );

    // create array of 'maxNumItems' menu items
    //
    rAssert( maxNumItems > 0 );
    m_menuItems = new GuiMenuItem*[ maxNumItems ];
    rAssert( m_menuItems != NULL );

    for( int i = 0; i < maxNumItems; i++ )
    {
        m_menuItems[ i ] = NULL;
    }
MEMTRACK_POP_GROUP( "GUIMenu" );
}

//===========================================================================
// CGuiMenu::~CGuiMenu
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
CGuiMenu::~CGuiMenu()
{
    if( m_menuItems != NULL )
    {
        for( int i = 0; i < m_numItems; i++ )
        {
            if( m_menuItems[ i ] != NULL )
            {
                delete m_menuItems[ i ];
                m_menuItems[ i ] = NULL;
            }
        }

        delete [] m_menuItems;
        m_menuItems = NULL;
    }
}

//===========================================================================
// CGuiMenu::HandleMessage
//===========================================================================
// Description: Message handler for this class.
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::HandleMessage( eGuiMessage message, unsigned int param1,
                                                   unsigned int param2 )
{
    if( m_isSelectionMade && this->IsControllerMessage( message ) )
    {
        // selection has already been made, ignore all controller messages
        //
        return;
    }

    switch( message )
    {
        case GUI_MSG_UPDATE:
        {
            if( m_selection != NO_SELECTION )
            {
                this->UpdateCurrentSelection( param1 );
            }
            
            break;
        }
        case GUI_MSG_CONTROLLER_UP:
        {
            this->ChangeSelection( -1 );

            break;
        }
        case GUI_MSG_CONTROLLER_DOWN:
        {
            this->ChangeSelection( +1 );

            break;
        }
        case GUI_MSG_CONTROLLER_LEFT:
        {
            this->DecrementSelectionValue();

            break;
        }
        case GUI_MSG_CONTROLLER_RIGHT:
        {
            this->IncrementSelectionValue();

            break;
        }
        case GUI_MSG_CONTROLLER_SELECT:
        {
            this->MakeSelection();

            break;
        }
#ifdef RAD_WIN32
        case GUI_MSG_MOUSE_OVER:
        {
            this->SetNewSelection(param1);
            
            break;
        }
        case GUI_MSG_MOUSE_LCLICKHOLD:
        {
            this->OutlineSelection( (param1!=0) );
            break;
        }
#endif
        default:
        {
            break;
        }
    }
}

//===========================================================================
// CGuiMenu::AddMenuItem
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
GuiMenuItem* CGuiMenu::AddMenuItem( Scrooby::BoundedDrawable* pItem,
                                    Scrooby::BoundedDrawable* pItemValue,
                                    Scrooby::Polygon* pSlider,
                                    Scrooby::Sprite* pSliderImage,
                                    Scrooby::Sprite* pItemValueArrowL,
                                    Scrooby::Sprite* pItemValueArrowR,
                                    int attributes )
{
    rAssert( m_menuItems[ m_numItems ] == NULL );

    // create a new menu item based on menu type
    //
    switch( m_menuType )
    {
        case GUI_TEXT_MENU:
        {
            m_menuItems[ m_numItems ] = new GuiMenuItemText;

            break;
        }
        case GUI_SPRITE_MENU:
        {
            m_menuItems[ m_numItems ] = new GuiMenuItemSprite;

            break;
        }
        default:
        {
            rAssertMsg( false, "Invalid menu type!" );

            break;
        }
    }

    rAssert( m_menuItems[ m_numItems ] != NULL );

    GuiMenuItem* pMenuItem = m_menuItems[ m_numItems ];

    m_menuItems[ m_numItems ]->SetItem( pItem );

    // for items w/ value toggle
    //
    if( pItemValue != NULL )
    {
        m_menuItems[ m_numItems ]->SetItemValue( pItemValue );

        // hide arrows by default, if exists
        //
        if( pItemValueArrowL != NULL )
        {
            pItemValueArrowL->SetVisible( false );
#ifdef RAD_WIN32
            pItemValueArrowL->ScaleAboutCenter( ARROW_SCALE );
#endif
            m_menuItems[ m_numItems ]->m_itemValueArrowL = pItemValueArrowL;
        }
        if( pItemValueArrowR != NULL )
        {
            pItemValueArrowR->SetVisible( false );
#ifdef RAD_WIN32
            pItemValueArrowR->ScaleAboutCenter( ARROW_SCALE );
#endif
            m_menuItems[ m_numItems ]->m_itemValueArrowR = pItemValueArrowR;
        }
    }

    if( m_menuType == GUI_TEXT_MENU )
    {
        if( (attributes & TEXT_OUTLINE_ENABLED) > 0 )
        {
            // turn on text outlining
            //
            m_menuItems[ m_numItems ]->SetDisplayOutline( true );
        }
#ifdef RAD_WIN32
        pItem->SetVerticalJustification( Scrooby::Top );
#endif
    }

    // for items w/ sliders
    //
    if( pSliderImage != NULL )
    {
        m_menuItems[ m_numItems ]->m_slider.SetScroobyImage( pSliderImage );
    }

    m_menuItems[ m_numItems ]->m_attributes = attributes;
    m_menuItems[ m_numItems ]->m_defaultColour = pItem->GetColour();
    m_numItems++;

    // select first item by default
    //
    if( m_numItems == 1 )
    {
        SelectItem( 0 );
    }
    return pMenuItem;
}

//===========================================================================
// CGuiMenu::SetMenuItemEnabled
//===========================================================================
// Description: Enable/disable menu item.
//
// Constraints: None.
//
// Parameters:  
//
// Return:      
//
//===========================================================================
void CGuiMenu::SetMenuItemEnabled( int index, bool enabled, bool changeVisibility )
{
    GuiMenuItem* currentMenuItem = this->GetMenuItem( index );
    rAssert( currentMenuItem != NULL );

    Scrooby::BoundedDrawable* currentItem = currentMenuItem->GetItem();
    rAssert( currentItem != NULL );

    Scrooby::BoundedDrawable* currentItemValue = currentMenuItem->GetItemValue();

    if( enabled )
    {
        currentMenuItem->m_attributes |= SELECTION_ENABLED;

        if( index != m_selection )
        {
            if( m_isGreyOutEnabled )
            {
                // restore default item colour
                //
                currentItem->SetColour( currentMenuItem->m_defaultColour );
                if( currentItemValue != NULL )
                {
                    currentItemValue->SetColour( currentMenuItem->m_defaultColour );
                }
            }

            // show cursor
            //
            if( m_pCursor != NULL )
            {
                m_pCursor->SetVisible( true );
            }

            if( m_selection == NO_SELECTION )
            {
                m_selection = 0;
                this->SelectItem( index );
            }
        }
    }
    else // !enabled
    {
        currentMenuItem->m_attributes &= ~SELECTION_ENABLED;

        // if item disabled is current selection, select next enabled item
        //
        if( index == m_selection )
        {
            this->ChangeSelection( +1, false );

            if( index == m_selection )
            {
                // reset cursor to first selection, and hide it
                //
                this->MoveCursor( m_selection, 0 );
                if( m_pCursor != NULL )
                {
                    m_pCursor->SetVisible( false );
                }

                // this means all selections have been disabled
                //
                m_selection = NO_SELECTION;
            }
        }

        if( m_isGreyOutEnabled )
        {
            // grey out item colour
            //
            currentItem->SetColour( DEFAULT_DISABLED_ITEM_COLOUR );

            if( currentItemValue != NULL )
            {
                currentItemValue->SetColour( DEFAULT_DISABLED_ITEM_COLOUR );
            }
        }
    }

    if( changeVisibility )
    {
        currentItem->SetVisible( enabled );

        if( currentItemValue != NULL )
        {
            currentItemValue->SetVisible( enabled );
        }
    }
}

bool CGuiMenu::IsMenuItemEnabled( int index )
{
    GuiMenuItem* currentMenuItem = this->GetMenuItem( index );
    rAssert( currentMenuItem != NULL );
    return (currentMenuItem->m_attributes & SELECTION_ENABLED);
}

//===========================================================================
// CGuiMenu::Reset
//===========================================================================
// Description: Unselect all menu items.
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::Reset( int defaultSelection )
{
    // un-select current item
    if( m_selection != NO_SELECTION )
    {
        this->UnselectItem( m_selection );
    }

    // select first item
    this->SelectItem( defaultSelection );
}

//===========================================================================
// CGuiMenu::SetHighlightColour
//===========================================================================
// Description: Sets the menu highlight colour.
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::SetHighlightColour( bool isEnabled, tColour colour )
{
    m_isHighlightEnabled = isEnabled;
    m_highlightColour = colour;
}

//===========================================================================
// CGuiMenu::SetSelectionValue
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::SetSelectionValue( int index, int value )
{
    if( value >= 0 && value < m_menuItems[ index ]->m_itemValueCount )
    {
        m_menuItems[ index ]->SetItemValueIndex( value );

        // Notify screen that selection value has changed
        m_pParent->HandleMessage( GUI_MSG_MENU_SELECTION_VALUE_CHANGED, index, value );
    }
}

//===========================================================================
// CGuiMenu::SetSelectionValueCount
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::SetSelectionValueCount( int index, int count )
{
    m_menuItems[ index ]->m_itemValueCount = count;

//    if( index == m_selection )
    {
        // show arrows if more than one value to toggle; otherwise, hide them (partially)
        //
        bool showArrows = (m_menuItems[ index ]->m_itemValueCount > 1);
        if( m_menuItems[ index ]->m_itemValueArrowL != NULL &&
            m_menuItems[ index ]->m_itemValueArrowR != NULL )
        {
            m_menuItems[ index ]->m_itemValueArrowL->SetAlpha( showArrows ? 1.0f : 0.4f );
            m_menuItems[ index ]->m_itemValueArrowR->SetAlpha( showArrows ? 1.0f : 0.4f );
        }
    }
}

//===========================================================================
// CGuiMenu::MakeSelection
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::MakeSelection( bool isSelectionMade )
{
    if (m_selection == NO_SELECTION) return;//------------->

    if( (m_menuItems[ m_selection ]->m_attributes & SELECTABLE) > 0 )
    {
        if( isSelectionMade )
        {
            if( m_selection != NO_SELECTION )
            {
                if( m_menuType == GUI_TEXT_MENU )
                {
                    m_menuItems[ m_selection ]->SetOutlineColour( m_selectionMadeOutlineColour );
#ifdef RAD_WIN32
                    if( m_bIsSelectionOutlined )
                        m_menuItems[ m_selection ]->GetItem()->SetColour( DEFAULT_SELECTED_ITEM_COLOUR );
#endif
                }

                m_selectionMadeElapsedTime = 0;

                GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );

                // tell parent screen to ignore all controller inputs until
                // selection-made effect is completed
                //
                CGuiScreen* parentScreen = static_cast<CGuiScreen*>( m_pParent );
                rAssert( parentScreen );
                parentScreen->SetIngoreControllerInputs( true );

                m_isSelectionMade = true;
            }
        }
        else
        {
            CGuiScreen* parentScreen = static_cast<CGuiScreen*>( m_pParent );
            rAssert( parentScreen );
            parentScreen->SetIngoreControllerInputs( false );

            if( m_menuType == GUI_TEXT_MENU )
            {
                m_menuItems[ m_selection ]->SetOutlineColour( DEFAULT_OUTLINE_COLOUR );
            }

            // restore current item's regular L/R arrows, if exist
            //
            if( m_menuItems[ m_selection ]->m_itemValueArrowL != NULL &&
                m_menuItems[ m_selection ]->m_itemValueArrowR != NULL )
            {
                m_menuItems[ m_selection ]->m_itemValueArrowL->SetIndex( 0 );
                m_menuItems[ m_selection ]->m_itemValueArrowR->SetIndex( 0 );
            }

            m_pParent->HandleMessage( GUI_MSG_MENU_SELECTION_MADE, m_selection );

            m_isSelectionMade = false;
        }
    }
}

//===========================================================================
// Protected Member Functions - CGuiMenu
//===========================================================================

//===========================================================================
// CGuiMenu::SelectItem
//===========================================================================
// Description: Select specified item.
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::SelectItem( int index )
{
    rAssert( index >= 0 && index < m_numItems );
    rAssert( m_menuItems[ index ] != NULL );

    // move cursor
    if( index != m_selection )
    {
        this->MoveCursor( m_selection, index );
    }

    // reset elapsed time
    m_elapsedTime = 0;

    if( m_isHighlightEnabled && m_menuType == GUI_TEXT_MENU  )
    {
        m_menuItems[ index ]->GetItem()->SetColour( m_highlightColour );
    }

    // apply to item value also, if exists
    if( m_menuItems[ index ]->GetItemValue() != NULL )
    {
        if( m_isHighlightEnabled && m_menuType == GUI_TEXT_MENU )
        {
            m_menuItems[ index ]->GetItemValue()->SetColour( m_highlightColour );
        }

        // show arrows, if exists
        //
        if( m_menuItems[ index ]->m_itemValueArrowL != NULL &&
            m_menuItems[ index ]->m_itemValueArrowR != NULL )
        {
            m_menuItems[ index ]->m_itemValueArrowL->SetVisible( true );
            m_menuItems[ index ]->m_itemValueArrowR->SetVisible( true );
        }
    }
/*
    if( m_specialEffects & MENU_SFX_DROP_SHADOW )
    {
        // turn on drop shadow
        m_menuItems[ index ]->m_item->SetDisplayShadow( true );

        // apply to item value also, if exists
        if( m_menuItems[ index ]->m_itemValue != NULL )
        {
            m_menuItems[ index ]->m_itemValue->SetDisplayShadow( true );
        }
    }
*/
    m_selection = index;
}

//===========================================================================
// CGuiMenu::UnselectItem
//===========================================================================
// Description: Unselect specified item.
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::UnselectItem( int index )
{
    rAssert( index >= 0 && index < m_numItems );
    rAssert( m_menuItems[ index ] != NULL );

    if( m_menuType == GUI_TEXT_MENU )
    {
        m_menuItems[ index ]->GetItem()->SetColour( m_menuItems[ index ]->m_defaultColour );
    }

    // apply to item value also, if exists
    if( m_menuItems[ index ]->GetItemValue() != NULL )
    {
        if( m_menuType == GUI_TEXT_MENU )
        {
            m_menuItems[ index ]->GetItemValue()->SetColour( m_menuItems[ index ]->m_defaultColour );
        }

        // hide arrows until item is selected
        //
        if( m_menuItems[ index ]->m_itemValueArrowL != NULL &&
            m_menuItems[ index ]->m_itemValueArrowR != NULL )
        {
            m_menuItems[ index ]->m_itemValueArrowL->SetVisible( false );
            m_menuItems[ index ]->m_itemValueArrowR->SetVisible( false );
        }
    }

    if( m_specialEffects & MENU_SFX_SIZE_PULSE )
    {
        // reset scale
        m_menuItems[ index ]->GetItem()->ResetTransformation();

        // apply to item value also, if exists
        if( m_menuItems[ index ]->GetItemValue() != NULL )
        {
            m_menuItems[ index ]->GetItemValue()->ResetTransformation();
        }
    }
/*
    if( m_specialEffects & MENU_SFX_DROP_SHADOW )
    {
        // turn off drop shadow
        m_menuItems[ index ]->m_item->SetDisplayShadow( false );

        // apply to item value also, if exists
        if( m_menuItems[ index ]->m_itemValue != NULL )
        {
            m_menuItems[ index ]->m_itemValue->SetDisplayShadow( false );
        }
    }
*/
}

//===========================================================================
// CGuiMenu::ChangeSelection
//===========================================================================
// Description: Select the next menu item.
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::ChangeSelection( int deltaItems, bool isUserInput )
{
    if( m_selection != NO_SELECTION )
    {
        rAssert( m_numItems > 0 );

        // Increment and check for wrap around.
        //
        int newSelection = (m_selection + deltaItems + m_numItems) % m_numItems;

        // Skip to next enabled selection if new selection is disabled
        while( newSelection != m_selection )
        {
            if( m_menuItems[ newSelection ]->m_attributes & SELECTION_ENABLED )
            {
                // Unselect the current item.
                //
                this->UnselectItem( m_selection );

                // Notify screen that menu selection has changed
                // (param1 = new selection, param2 = old selection)
                //
                m_pParent->HandleMessage( GUI_MSG_MENU_SELECTION_CHANGED, newSelection, m_selection );

                // Make the new selection.
                //
                this->SelectItem( newSelection );

                if( isUserInput )
                {
                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                }

                break;
            }

            newSelection = (newSelection + deltaItems + m_numItems) % m_numItems;
        }
    }
}

#ifdef RAD_WIN32
//===========================================================================
// CGuiMenu::ChangeSelection
//===========================================================================
// Description: Select a specific selection.
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::SetNewSelection( int newSelection, bool isUserInput )
{
    if( m_selection != NO_SELECTION )
    {
        rAssert( m_numItems > 0 );

        if( newSelection != m_selection )
        {
            if( m_menuItems[ newSelection ]->m_attributes & SELECTION_ENABLED )
            {
                // Unselect the current item.
                //
                this->UnselectItem( m_selection );

                // Notify screen that menu selection has changed
                // (param1 = new selection, param2 = old selection)
                //
                m_pParent->HandleMessage( GUI_MSG_MENU_SELECTION_CHANGED, newSelection, m_selection );

                // Make the new selection.
                //
                this->SelectItem( newSelection );

                if( isUserInput )
                {
                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
                }
            }
        }
    }
}
//===========================================================================
// CGuiMenu::OutlineSelection
//===========================================================================
// Description: Outlines a specific selection.
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::OutlineSelection( bool bOutline )
{
    if (m_selection == NO_SELECTION) return;//------------->

    if( (m_menuItems[ m_selection ]->m_attributes & SELECTABLE) > 0 )
    {
        if( m_menuType == GUI_TEXT_MENU )
        {
            if( bOutline )
                m_menuItems[ m_selection ]->GetItem()->SetColour( m_selectionOutlineColour );
            else
                m_menuItems[ m_selection ]->GetItem()->SetColour( DEFAULT_SELECTED_ITEM_COLOUR );

            m_bIsSelectionOutlined = bOutline;
        }
    }
}

#endif

//===========================================================================
// CGuiMenu::IncrementSelectionValue
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::IncrementSelectionValue( bool isUserInput )
{
    if( m_selection != NO_SELECTION &&
        m_menuItems[ m_selection ]->GetItemValue() != NULL )
    {
        int currentIndex = m_menuItems[ m_selection ]->GetItemValueIndex();
        int newIndex = currentIndex;

        if( currentIndex < m_menuItems[ m_selection ]->m_itemValueCount - 1 )
        {
            newIndex = currentIndex + 1;
        }
        else if( m_menuItems[ m_selection ]->m_attributes & VALUES_WRAPPED )
        {
            // wrap around to beginning
            newIndex = 0;
        }

        if( newIndex != currentIndex )
        {
            m_menuItems[ m_selection ]->SetItemValueIndex( newIndex );

            // Notify screen that selection value has changed
            m_pParent->HandleMessage( GUI_MSG_MENU_SELECTION_VALUE_CHANGED, m_selection, newIndex );

            if( isUserInput )
            {
                GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
            }
        }
    }
}

//===========================================================================
// CGuiMenu::DecrementSelectionValue
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  N/A.
//
// Return:      N/A.
//
//===========================================================================
void CGuiMenu::DecrementSelectionValue( bool isUserInput )
{
    if( m_selection != NO_SELECTION &&
        m_menuItems[ m_selection ]->GetItemValue() != NULL )
    {
        int currentIndex = m_menuItems[ m_selection ]->GetItemValueIndex();
        int newIndex = currentIndex;

        if( currentIndex > 0 )
        {
            newIndex = currentIndex - 1;
        }
        else if( m_menuItems[ m_selection ]->m_attributes & VALUES_WRAPPED )
        {
            // wrap around to end
            newIndex = m_menuItems[ m_selection ]->m_itemValueCount - 1;
        }

        if( newIndex != currentIndex )
        {
            rAssertMsg( newIndex != -1, "This means item value count is zero!" );

            m_menuItems[ m_selection ]->SetItemValueIndex( newIndex );

            // Notify screen that selection value has changed
            m_pParent->HandleMessage( GUI_MSG_MENU_SELECTION_VALUE_CHANGED, m_selection, newIndex );

            if( isUserInput )
            {
                GetEventManager()->TriggerEvent( EVENT_FE_MENU_UPORDOWN );
            }
        }
    }
}

//===========================================================================
// Private Member Functions - CGuiMenu
//===========================================================================

//===========================================================================
// CGuiMenu::UpdateCurrentSelection
//===========================================================================
// Description: Updates current selection.
//
// Constraints: None.
//
// Parameters:  
//
// Return:      
//
//===========================================================================
void CGuiMenu::UpdateCurrentSelection( int elapsedTime )
{
    rAssert( m_selection >= 0 && m_selection < m_numItems );

    Scrooby::BoundedDrawable* currentItem = m_menuItems[ m_selection ]->GetItem();
    rAssert( currentItem != NULL );

    if( m_isSelectionMade ) // update "selection made" effect
    {
        m_selectionMadeElapsedTime += elapsedTime;

        if( m_selectionMadeElapsedTime > SELECTION_MADE_DURATION )
        {
            this->MakeSelection( false );
        }
    }
    else // update current selection
    {
        const unsigned int ITEM_PULSE_PERIOD = 600; // in milliseconds

        if( m_specialEffects & MENU_SFX_COLOUR_PULSE )
        {
            // pulse text colour
            //
            tColour textColour;
            GuiSFX::ModulateColour( &textColour,
                                    (float)m_elapsedTime,
                                    (float)ITEM_PULSE_PERIOD,
                                    tColour( 255, 224, 32 ), // m_menuItems[ m_selection ]->m_defaultColour,
                                    m_highlightColour );

            currentItem->SetColour( textColour );
        }

        if( m_specialEffects & MENU_SFX_SIZE_PULSE )
        {
            // pulse text size
            //
            const float ITEM_SCALE_FACTOR = 0.10f;

            currentItem->ResetTransformation();

//            if( m_elapsedTime < ITEM_PULSE_PERIOD )
            {
                // TC: [IMPROVE] Man, this looks ugly!
                //
                static const float THETA_OFFSET = -rmt::ASin( -0.5f ); // = -ASin( (1 - center) / amplitude )

                float scale = GuiSFX::Pulse( (float)m_elapsedTime,
                                             (float)ITEM_PULSE_PERIOD,
                                             1.0f + ITEM_SCALE_FACTOR / 2,
                                             ITEM_SCALE_FACTOR,
                                             THETA_OFFSET );

                int width = 0;
                int height = 0;
                currentItem->GetBoundingBoxSize( width, height );

                // scale text about (left center, right center, or middle center)
                //
                if( currentItem->GetHorizontalJustification() == Scrooby::Left )
                {
                    currentItem->ScaleAboutPoint( scale, 0, height / 2 );
                }
                else if( currentItem->GetHorizontalJustification() == Scrooby::Right )
                {
                    currentItem->ScaleAboutPoint( scale, width, height / 2 );
                }
                else
                {
                    currentItem->ScaleAboutCenter( scale );
                }
            }
        }

        // update slider, if exists on current selection
        //
        ImageSlider* currentItemSlider = &(m_menuItems[ m_selection ]->m_slider);
        if( currentItemSlider->m_pImage != NULL )
        {
            bool hasSliderValueChanged = false;

            int numUserInputHandlers = GetGuiSystem()->GetNumUserInputHandlers();
            for( int i = 0; i < numUserInputHandlers; i++ )
            {
                if( m_controllerID != -1 && m_controllerID != static_cast<short>( i ) )
                {
                    // ignore other controller inputs
                    //
                    continue;
                }

                CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( i );
                if( userInputHandler != NULL )
                {
#ifdef RAD_WIN32
                    if( userInputHandler->IsXAxisOnLeft() || 
                        GetInputManager()->GetFEMouse()->OnSliderHorizontalClickDrag() == MDIR_LEFT )
#else
                    if( userInputHandler->IsButtonDown( GuiInput::Left ) ||
                        userInputHandler->IsXAxisOnLeft() )
#endif
                    {
                        float newValue = currentItemSlider->m_value - elapsedTime / SLIDER_FULL_RANGE_TIME;
                        if( newValue < 0.0f )
                        {
                            newValue = 0.0f; // clamp lower-end at 0.0
                        }

                        if( newValue != currentItemSlider->m_value )
                        {
                            currentItemSlider->SetValue( newValue );
                            hasSliderValueChanged = true;
                        }
                    }

#ifdef RAD_WIN32
                    if( userInputHandler->IsXAxisOnRight() ||
                        GetInputManager()->GetFEMouse()->OnSliderHorizontalClickDrag() == MDIR_RIGHT )
#else
                    if( userInputHandler->IsButtonDown( GuiInput::Right ) ||
                        userInputHandler->IsXAxisOnRight() )
#endif
                    {
                        float newValue = currentItemSlider->m_value + elapsedTime / SLIDER_FULL_RANGE_TIME;
                        if( newValue > 1.0f )
                        {
                            newValue = 1.0f; // clamp higher-end at 1.0
                        }

                        if( newValue != currentItemSlider->m_value )
                        {
                            currentItemSlider->SetValue( newValue );
                            hasSliderValueChanged = true;
                        }
                    }
                }
            }

            if( hasSliderValueChanged )
            {
                // notify screen that slider value has changed
                //
                m_pParent->HandleMessage( GUI_MSG_MENU_SELECTION_VALUE_CHANGED, m_selection );
            }
            else
            {
                // otherwise, notify screen that slider value is currently not changing
                //
                m_pParent->HandleMessage( GUI_MSG_MENU_SLIDER_NOT_CHANGING, m_selection );
            }
        }

        // update L/R arrows (if exists for current selection)
        //
        if( m_menuItems[ m_selection ]->m_itemValueArrowL != NULL &&
            m_menuItems[ m_selection ]->m_itemValueArrowR != NULL )
        {
            m_menuItems[ m_selection ]->m_itemValueArrowL->SetIndex( 0 );
            m_menuItems[ m_selection ]->m_itemValueArrowR->SetIndex( 0 );

            // if there are more than one item to choose from
            //
            if( m_menuItems[ m_selection ]->m_itemValueCount > 1 )
            {
                int numUserInputHandlers = GetGuiSystem()->GetNumUserInputHandlers();
                for( int i = 0; i < numUserInputHandlers; i++ )
                {
                    if( m_controllerID != -1 && m_controllerID != static_cast<short>( i ) )
                    {
                        // ignore other controller inputs
                        //
                        continue;
                    }

                    CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( i );
                    if( userInputHandler != NULL )
                    {
#ifdef RAD_WIN32
                        if( userInputHandler->IsXAxisOnLeft() || 
                            GetInputManager()->GetFEMouse()->LeftButtonDownOn() == HOTSPOT_ARROWLEFT )
#else
                        if( userInputHandler->IsButtonDown( GuiInput::Left ) ||
                            userInputHandler->IsXAxisOnLeft() )
#endif
                        {
                            rAssert( m_menuItems[ m_selection ]->m_itemValueArrowL->GetNumOfImages() > 1 );
                            m_menuItems[ m_selection ]->m_itemValueArrowL->SetIndex( 1 );
                        }

#ifdef RAD_WIN32
                        if( userInputHandler->IsXAxisOnRight() || 
                            GetInputManager()->GetFEMouse()->LeftButtonDownOn() == HOTSPOT_ARROWRIGHT )
#else
                        if( userInputHandler->IsButtonDown( GuiInput::Right ) ||
                            userInputHandler->IsXAxisOnRight() )
#endif
                        {
                            rAssert( m_menuItems[ m_selection ]->m_itemValueArrowR->GetNumOfImages() > 1 );
                            m_menuItems[ m_selection ]->m_itemValueArrowR->SetIndex( 1 );
                        }
                    }
                }
            }
        }

        // update elapsed time
        //
        m_elapsedTime = (m_elapsedTime + elapsedTime) % ITEM_PULSE_PERIOD;
    }
}

//===========================================================================
// CGuiMenu::MoveCursor
//===========================================================================
// Description: Move cursor from previous selection to next selection.
//
// Constraints: None.
//
// Parameters:  
//
// Return:      
//
//===========================================================================
void CGuiMenu::MoveCursor( int previousIndex, int nextIndex )
{
    // if cursor exists
    //
    if( m_pCursor != NULL && previousIndex != NO_SELECTION )
    {
        Scrooby::BoundedDrawable* item = NULL;
        int x1, y1, x2, y2;

        // get location of previous item
        item = m_menuItems[ previousIndex ]->GetItem();
        rAssert( item != NULL );
        item->GetOriginPosition( x1, y1 );

        // get location of next item
        item = m_menuItems[ nextIndex ]->GetItem();
        rAssert( item != NULL );
        item->GetOriginPosition( x2, y2 );

        // translate cursor
        m_pCursor->Translate( x2 - x1, y2 - y1 );
    }
}


//===========================================================================
// Public Member Functions - CGuiMenu2D
//===========================================================================

CGuiMenu2D::CGuiMenu2D( CGuiEntity* pParent,
                        int numItems,
                        int numColumns,
                        eMenuType menuType,
                        int specialEffects )
:   CGuiMenu( pParent, numItems, menuType, specialEffects ),
    m_numColumns( static_cast<short>( numColumns ) )
{
}

CGuiMenu2D::~CGuiMenu2D()
{
}

void
CGuiMenu2D::HandleMessage( eGuiMessage message, unsigned int param1,
                                                unsigned int param2 )
{
    if( m_isSelectionMade && this->IsControllerMessage( message ) )
    {
        // selection has already been made, ignore all controller messages
        //
        return;
    }

    switch( message )
    {
        case GUI_MSG_CONTROLLER_LEFT:
        {
            this->ChangeSelection( -1 );

            break;
        }
        case GUI_MSG_CONTROLLER_RIGHT:
        {
            this->ChangeSelection( +1 );

            break;
        }
        case GUI_MSG_CONTROLLER_UP:
        {
            this->ChangeSelection( -m_numColumns );

            break;
        }
        case GUI_MSG_CONTROLLER_DOWN:
        {
            this->ChangeSelection( +m_numColumns );

            break;
        }
        default:
        {
            CGuiMenu::HandleMessage( message, param1, param2 );

            break;
        }
    }
}


//===========================================================================
// Public Member Functions - CGuiMenuPrompt
//===========================================================================

CGuiMenuPrompt::CGuiMenuPrompt( CGuiEntity* pParent,
                                Scrooby::Page* pPage,
                                int numResponses,
                                int specialEffects )
:
    CGuiMenu( pParent, numResponses, GUI_TEXT_MENU, specialEffects ),
    m_numResponses( static_cast<short>( numResponses ) )
{
    rAssert( pPage != NULL );

    Scrooby::Group* menu = pPage->GetGroup( "Menu" );
    rAssert( menu != NULL );
    // add response menu items
    //
    for( short i = 0; i < m_numResponses; i++ )
    {
        char textName[ 32 ];
        sprintf( textName, "Response%d", i );
        this->AddMenuItem( menu->GetText( textName ) );
    }
}

CGuiMenuPrompt::~CGuiMenuPrompt()
{
}

void
CGuiMenuPrompt::SetNumResponses( int numResponses )
{
    for( short i = 0; i < m_numResponses; i++ )
    {
        this->SetMenuItemEnabled( i, (i < numResponses), true );
    }
}

void
CGuiMenuPrompt::SetResponse( int index, ePromptResponse response )
{
    rAssert( static_cast<short>( index ) >= 0 && static_cast<short>( index ) < m_numResponses );

    GuiMenuItem* menuItem = this->GetMenuItem( index );
    rAssert( menuItem != NULL );

    Scrooby::Text* textItem = dynamic_cast<Scrooby::Text*>( menuItem->GetItem() );
    rAssert( textItem != NULL );
    textItem->SetIndex( response );
}

CGuiMenuPrompt::ePromptResponse
CGuiMenuPrompt::GetResponse( int index ) const
{
    rAssert( static_cast<short>( index ) >= 0 && static_cast<short>( index ) < m_numResponses );

    GuiMenuItem* menuItem = this->GetMenuItem( index );
    rAssert( menuItem != NULL );

    Scrooby::Text* textItem = dynamic_cast<Scrooby::Text*>( menuItem->GetItem() );
    rAssert( textItem != NULL );

    return static_cast<ePromptResponse>( textItem->GetIndex() );
}

CGuiMenuPrompt::ePromptResponse
CGuiMenuPrompt::GetCurrentResponse() const
{
    return this->GetResponse( m_selection );
}

