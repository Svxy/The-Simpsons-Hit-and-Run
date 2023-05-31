/******************************************************************************

    File:        GuiScreenControllerWin32.h
    Desc:        Interface for the CGuiScreenController class.

    Date:        July 31, 2003
    Author:      Neil Haran
    History:     

*****************************************************************************/

#ifndef GUISCREENCONTROLLER_H
#define GUISCREENCONTROLLER_H

#include <presentation/gui/guiscreen.h>
#include <input/usercontrollerWin32.h>

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
    virtual void OnButtonMapped( const char* InputName, eControllerType cont, int num_dirs, eDirectionType direction );

protected:
    void InitIntro();
    void InitRunning();
    void InitOutro();

private:
    enum eControllerPromptResponse
    {
        PROMPT_NO,
        PROMPT_YES,
        NUM_CONTROLLER_PROMPT_RESPONSES
    };

    enum eControllerColumn
    {
        MAP_PRIMARY = 0,
        MAP_SECONDARY,
        NUM_COLUMNS,
    };

    enum eMenuPages
    {
        MENU_PAGE_MAIN,
        MENU_PAGE_CHARACTERCONTROLS,
        MENU_PAGE_VEHICLECONTROLS,
        MENU_PAGE_GAMESETTINGS,
        NUM_MENU_PAGES,
        NUM_CONTROLLER_PAGES = NUM_MENU_PAGES - 1,
        MENU_PAGE_NONE
    };

    enum eMenuItem
    {
        // MAIN CONTROLLER MENU ITEMS
        MENU_ITEM_CHARACTERCONTROLS = 0,
        MENU_ITEM_VEHICLECONTROLS,
        MENU_ITEM_GAMESETTINGS,
        MENU_ITEM_RESTOREALLDEFAULTS,
        MENU_ITEM_NONE,
        NUM_MAINMENU_LABELS = MENU_ITEM_NONE,

        // Character Control ITEMS
        CC_MOVEUP = MENU_ITEM_NONE,
        CC_MOVEDOWN,
        CC_MOVELEFT,
        CC_MOVERIGHT,
        CC_ATTACK,
        CC_JUMP,
        CC_SPRINT,
        CC_ACTION,
        CC_CAMERALEFT,
        CC_CAMERARIGHT,
        CC_CAMERAMOVEIN,
        CC_CAMERAMOVEOUT,
        CC_ZOOM,
        CC_LOOKUP,
        CC_NONE,
        NUM_CHARACTERCONTROL_LABELS = CC_NONE - MENU_ITEM_NONE,

        // Vehicle Control ITEMS
        VC_ACCELERATE = CC_NONE,
        VC_REVERSE,
        VC_STEERLEFT,
        VC_STEERRIGHT,
        VC_EBRAKE,
        VC_ACTION,
        VC_HORN,
        VC_RESET,
        VC_LOOKLEFT,
        VC_LOOKRIGHT,
        VC_LOOKUP,
        VC_LOOKBACK,
        VC_CHANGECAMERA,

        VC_NONE,
        NUM_VEHICLECONTROL_LABELS = VC_NONE - CC_NONE,

        // Game Setting Items
        GS_MOUSELOOK = VC_NONE,
        GS_MOUSESENSITIVITY_X,
        GS_MOUSESENSITIVITY_Y,
        GS_INVERTMOUSEX,
        GS_INVERTMOUSEY,
        GS_FORCEFEEDBACK,
        GS_WHEELSENSITIVITY,
        GS_NONE,
        NUM_GAMESETTING_LABELS = GS_NONE - VC_NONE,

        NUM_MENU_ITEMS = NUM_MAINMENU_LABELS + 
                         NUM_CHARACTERCONTROL_LABELS*NUM_COLUMNS + 
                         NUM_VEHICLECONTROL_LABELS*NUM_COLUMNS +
                         NUM_GAMESETTING_LABELS
    };

private:
    void InitPageLabels( eMenuPages page );
    void UpdatePageLabels( eMenuPages page, const char* szNewInput );
    void SetGroups( Scrooby::Text** pLabels,
                    int numMenuItems,
                    const char* strPage,
                    char* strGroup = "Menu",
                    char* szLabel = "Label",
                    int attributes = SELECTION_ENABLED | SELECTABLE | VALUES_WRAPPED );

    void SetPageVisiblility( const char* strPage, bool bVisible );
    int  GetVirtualKey( eMenuPages page, int menuItem );
    void RemapButton( eControllerColumn column, int menuItem );
    void GetAppropriateInputName( char* szInputName, 
                                  eControllerType controllerType, 
                                  eDirectionType direction, 
                                  int numDirections );
    
private:
    CGuiMenu*       m_pMenu;
    Scrooby::Text*  m_pMenuLabels[ NUM_MAINMENU_LABELS ];
    Scrooby::Text*  m_pCCLabels[ NUM_COLUMNS ][ NUM_CHARACTERCONTROL_LABELS ];
    Scrooby::Text*  m_pVCLabels[ NUM_COLUMNS ][ NUM_VEHICLECONTROL_LABELS ];
    Scrooby::Text*  m_pMSLabels[ NUM_GAMESETTING_LABELS ];
    Scrooby::Text*  m_currentTextLabel;

    // These arrays are for the screen mappings.
    eMenuPages      m_currentPage;
    int             m_currentControllerID;
    bool            m_bMapInput;
    bool            m_bDisableBack;
    int             m_menuGroupStartIndex[NUM_MENU_PAGES];
    int             m_numControllerGroups;
};

#endif
