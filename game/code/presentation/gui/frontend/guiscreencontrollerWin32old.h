//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenController
//
// Description: 
//              
//
// Authors:     Tony Chu,
//              Neil Haran
//
// Revisions		Date			Author	    Revision
//                  2002/07/30      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENCONTROLLER_H
#define GUISCREENCONTROLLER_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <input/usercontrollerWin32.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenController : public CGuiScreen,
                             public ButtonMappedCallback
{
public:
    CGuiScreenController( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenController();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

    virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y );
    virtual void OnButtonMapped( const char* szNewInput );

protected:
    void InitIntro();
    void InitRunning();
    void InitOutro();

private:
    enum eControllerColumn
    {
        MASTER_LIST = 0,
        MAP_PRIMARY,
        MAP_SECONDARY,
        NUM_COLUMNS,
        NUM_MAPS = NUM_COLUMNS - 1
    };

    enum eMenuPages
    {
        MENU_PAGE_MAIN,
        MENU_PAGE_KM,
        MENU_PAGE_GAMEPAD,
        NUM_MENU_PAGES,
        NUM_CONTROLLER_PAGES = NUM_MENU_PAGES - 1,
        MENU_PAGE_NONE
    };

    enum eMenuItem
    {
        // MAIN CONTROLLER MENU ITEMS
        MENU_ITEM_KEYMOUSECONFIG = 0,
        MENU_ITEM_GAMEPADCONFIG,
        MENU_ITEM_NONE,
        NUM_MAINMENU_LABELS = MENU_ITEM_NONE,

        // KEYBOARD/MOUSE ITEMS
        KM_MOVEUP = MENU_ITEM_NONE,
        KM_MOVEDOWN,
        KM_MOVELEFT,
        KM_MOVERIGHT,
        KM_FIRSTPERSON,
        KM_ATTACK,
        KM_JUMP,
        KM_SPRINT,
        KM_DOACTION,
        KM_CAMERAZOOM,
        KM_ACCELERATE,
        KM_EBRAKE,
        KM_HORN,
        KM_RESETCAR,
        KM_NONE,
        NUM_KEYBOARDMOUSE_LABELS = KM_NONE - MENU_ITEM_NONE,

        // GAMEPAD ITEMS
        GAMEPAD_FIRSTPERSON = KM_NONE,
        GAMEPAD_ATTACK,
        GAMEPAD_JUMP,
        GAMEPAD_SPRINT,
        GAMEPAD_DOACTION,
        GAMEPAD_CAMERAZOOM,
        GAMEPAD_ACCELERATE,
        GAMEPAD_EBRAKE,
        GAMEPAD_HORN,
        GAMEPAD_RESETCAR,
        GAMEPAD_NONE,
        NUM_GAMEPAD_LABELS = GAMEPAD_NONE - KM_NONE,

        NUM_LABELS = NUM_MAINMENU_LABELS + NUM_KEYBOARDMOUSE_LABELS + NUM_GAMEPAD_LABELS,
        NUM_MENU_ITEMS = NUM_MAINMENU_LABELS + NUM_KEYBOARDMOUSE_LABELS*NUM_COLUMNS + NUM_GAMEPAD_LABELS*NUM_COLUMNS
    };

private:
    void UpdatePageLabels( eMenuPages page );
    void SetSingleItemMenuPage( Scrooby::Text** pLabels,
                                int numMenuItems,
                                const char* strPage,
                                char* strGroup = "Menu",
                                char* szLabel = "Label",
                                int attributes = SELECTION_ENABLED | SELECTABLE | VALUES_WRAPPED );

    void SetPageVisiblility( const char* strPage, bool bVisible );
    int  GetVirtualKey( int menuItem );
    void RemapButton( eControllerColumn column, eControllerType type, int menuItem );
    
private:
    CGuiMenu*       m_pMenu;
    Scrooby::Text*  m_pMenuLabels[ NUM_MAINMENU_LABELS ];
    Scrooby::Text*  m_pKMLabels[ NUM_COLUMNS ][ NUM_KEYBOARDMOUSE_LABELS ];
    Scrooby::Text*  m_pGamepadLabels[ NUM_COLUMNS ][ NUM_GAMEPAD_LABELS ];
    Scrooby::Text*  m_currentTextLabel;

    eMenuPages      m_currentPage;
    int             m_currentControllerID;
    bool            m_bMapInput;
    bool            m_bDisableBack;
    int             m_menuGroupStartIndex[NUM_MENU_PAGES];
    int             m_numControllerGroups;
};

#endif // GUISCREENCONTROLLER_H
