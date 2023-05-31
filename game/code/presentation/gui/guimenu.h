//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiMenu
//
// Description: 
//              
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions    Date            Author      Revision
//              2000/12/4       DChau       Created
//              2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUIMENU_H
#define GUIMENU_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guientity.h>
#include <presentation/gui/guimenuitem.h>

namespace Scrooby
{
    class Page;
    class Drawable;
    class BoundedDrawable;
    class Sprite;
}

enum eMenuType
{
    GUI_TEXT_MENU,
    GUI_SPRITE_MENU,

    NUM_MENU_TYPES
};

enum eMenuSpecialEffect
{
    MENU_SFX_NONE = 0,

    MENU_SFX_COLOUR_PULSE = 1,
    MENU_SFX_SIZE_PULSE = 2,

    MENU_SFX_ALL = ~0
};

//===========================================================================
// Interface Definitions - CGuiMenu
//===========================================================================
class CGuiMenu : public CGuiEntity
{
public:
    CGuiMenu( CGuiEntity* pParent,
              int maxNumItems = 1,
              eMenuType menuType = GUI_TEXT_MENU,
              int specialEffects = MENU_SFX_SIZE_PULSE );

    virtual ~CGuiMenu();

    void HandleMessage( eGuiMessage message, 
                        unsigned int param1 = 0,
                        unsigned int param2 = 0 );

    GuiMenuItem* AddMenuItem( Scrooby::BoundedDrawable* pItem,
                              Scrooby::BoundedDrawable* pItemValue = NULL,
                              Scrooby::Polygon* pSlider = NULL,
                              Scrooby::Sprite* pSliderImage = NULL,
                              Scrooby::Sprite* pItemValueArrowL = NULL,
                              Scrooby::Sprite* pItemValueArrowR = NULL,
                              int attributes = ALL_ATTRIBUTES_ON );

    void SetMenuItemEnabled( int index, bool enabled,
                             bool changeVisibility = false );

    bool IsMenuItemEnabled( int index );

    void SetCursor( Scrooby::Drawable* pCursor ) { m_pCursor = pCursor; }
    Scrooby::Drawable* GetCursor() const { return m_pCursor; }

    // Reset Menu
    //
    void Reset( int defaultSelection = 0 );

    // Menu Highlight Colour
    //
    void SetHighlightColour( bool isEnabled, tColour colour );
    tColour GetHighlightColour() const { return m_highlightColour; }

    void SetGreyOutEnabled( bool isEnabled ) { m_isGreyOutEnabled = isEnabled; }

    // Selection Made Outline Colour
    //
    void SetSelectionMadeOutlineColour( tColour colour ) { m_selectionMadeOutlineColour = colour; }

    // Set Selection Values and Value Counts
    //
    void SetSelectionValue( int index, int value );
    void SetSelectionValueCount( int index, int count );

    // Make Menu Selection and Query Menu Selection Made
    //
    void MakeSelection( bool isSelectionMade = true );
    bool HasSelectionBeenMade() const { return m_isSelectionMade; }

    // Set Specific Controller ID
    //
    void SetControllerID( int controllerID );

    // Menu Accessors
    //
    GuiMenuItem* GetMenuItem( int index ) const;
    int GetNumItems() const { return m_numItems; }
    int GetSelection() const { return m_selection; }
    int GetSelectionValue( int index ) const;

protected:
    enum eSelection { NO_SELECTION = -1 };

    void SelectItem( int index ); 
    void UnselectItem( int index );

    void ChangeSelection( int deltaItems, bool isUserInput = true );
#ifdef RAD_WIN32
    void SetNewSelection( int newSelection, bool isUserInput = false );
    void OutlineSelection( bool bOutline = true );
#endif
    void IncrementSelectionValue( bool isUserInput = true );
    void DecrementSelectionValue( bool isUserInput = true );

    eMenuType m_menuType;
    short m_specialEffects; // bitmask

    GuiMenuItem** m_menuItems;
    int m_numItems;

    int m_selection;
#ifdef RAD_WIN32
    bool m_bIsSelectionOutlined;
    tColour m_selectionOutlineColour;
#endif
    Scrooby::Drawable* m_pCursor;
    bool m_isSelectionMade : 1;

private:
    void UpdateCurrentSelection( int elapsedTime );
    void MoveCursor( int previousIndex, int nextIndex );

    bool m_isHighlightEnabled : 1;
    tColour m_highlightColour;
    tColour m_selectionMadeOutlineColour;

    bool m_isGreyOutEnabled : 1;

    unsigned int m_elapsedTime;
    int m_selectionMadeElapsedTime;

    short m_controllerID;
};

inline void CGuiMenu::SetControllerID( int controllerID )
{
    m_controllerID = static_cast<short>( controllerID );
}

inline GuiMenuItem* CGuiMenu::GetMenuItem( int index ) const
{
    rAssert( index >= 0 && index < m_numItems );
    return m_menuItems[ index ];
}

inline int CGuiMenu::GetSelectionValue( int index ) const
{
    rAssert( index >= 0 && index < m_numItems );
    return m_menuItems[ index ]->GetItemValueIndex();
}

//===========================================================================
// Interface Definitions - CGuiMenu2D
//===========================================================================
class CGuiMenu2D : public CGuiMenu
{
public:
    CGuiMenu2D( CGuiEntity* pParent,
                int numItems,
                int numColumns,
                eMenuType menuType = GUI_TEXT_MENU,
                int specialEffects = MENU_SFX_SIZE_PULSE );

    virtual ~CGuiMenu2D();

    void HandleMessage( eGuiMessage message, 
                        unsigned int param1 = 0,
                        unsigned int param2 = 0 );

private:
    short m_numColumns;

};

//===========================================================================
// Interface Definitions - CGuiMenuPrompt
//===========================================================================
class CGuiMenuPrompt : public CGuiMenu
{
public:
    enum ePromptResponse
    {
        RESPONSE_NO,
        RESPONSE_YES,
        RESPONSE_CONTINUE,
        RESPONSE_CONTINUE_WITHOUT_SAVE,
        RESPONSE_RETRY,
        RESPONSE_MANAGE_MEMCARDS,
        RESPONSE_FORMAT_GC,
        RESPONSE_FORMAT_PS2,
        RESPONSE_FORMAT_XBOX,
        RESPONSE_CONTINUE_WITHOUT_FORMAT,
        RESPONSE_DELETE,
        RESPONSE_OK,
        RESPONSE_LOAD,
        RESPONSE_SAVE,

        NUM_PROMPT_RESPONSES
    };

    static const int MAX_NUM_RESPONSES = 3;

    CGuiMenuPrompt( CGuiEntity* pParent,
                    Scrooby::Page* pPage,
                    int numResponses = MAX_NUM_RESPONSES,
                    int specialEffects = MENU_SFX_SIZE_PULSE );

    virtual ~CGuiMenuPrompt();

    void SetNumResponses( int numResponses );
    void SetResponse( int index, ePromptResponse response );
    ePromptResponse GetResponse( int index ) const;
    ePromptResponse GetCurrentResponse() const;

private:
    short m_numResponses;

};

#endif // GUIMENU_H
