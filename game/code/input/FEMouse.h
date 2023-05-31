/******************************************************************************
    File:		FEMouse.h
    Desc:		Interface for the Frontend Mouse class.
                - Uses the windows mouse functionality.

    Date:		June 13, 2003
    History:
*****************************************************************************/

#ifndef FEMOUSE_H
#define FEMOUSE_H

#include <input/MouseCursor.h>

enum eFEMouseButton
{
    BUTTON_LEFT,
    BUTTON_RIGHT,
    NUM_BUTTON_TYPES
};

enum eFEMouseHorzDir
{
    MDIR_LEFT,
    MDIR_RIGHT,
    NUM_MOUSE_HORZ_DIRECTIONS,
    NO_HORZ_MOVEMENT
};

enum eFEHotspotType
{
    HOTSPOT_BUTTON,
    HOTSPOT_ARROWLEFT,
    HOTSPOT_ARROWRIGHT,
    HOTSPOT_ARROWUP,
    HOTSPOT_ARROWDOWN,
    HOTSPOT_SLIDER,
    HOTSPOT_LTRIGGER,
    HOTSPOT_RTRIGGER,
    NUM_HOTSPOT_TYPES,
    HOTSPOT_NONE
};

enum eFEButtonStates
{
    BUTTON_IDLE,
    BUTTON_CLICKHOLD,
    BUTTON_CLICKED,
    NUM_BUTTON_STATES
};

class FEMouse
{
public:
    FEMouse();
    ~FEMouse();

    MouseCursor* getCursor()            const { return m_pCursor; }
    bool Moved()                        const { return m_bMoved; }
    bool IsClickable()                  const { return m_bClickable; }
    eFEMouseHorzDir MovedHorizontally() const { return m_horzDir; }
    bool IsLeftButtonDown()             const { return (m_button[BUTTON_LEFT]==BUTTON_CLICKHOLD); }
    eFEHotspotType LeftButtonDownOn()   const 
    { 
        return ((m_button[BUTTON_LEFT]==BUTTON_CLICKHOLD) ? m_hotSpotType : HOTSPOT_NONE);
    }
    bool IsRightButtonDown()            const { return (m_button[BUTTON_RIGHT]==BUTTON_CLICKHOLD); }

    void InitMouseCursor( tDrawable* pCursor );
    void SetClickable( bool bClickable ) { m_bClickable = bClickable; }
    void SetSelectable( bool bSelectable ) { m_bSelectable = bSelectable; }
    void SetClickStopMode( bool bClickAndStop ) { m_bClickAndStop = bClickAndStop; }
    void ButtonDown( eFEMouseButton buttonType );
    void ButtonUp( eFEMouseButton buttonType );

    eFEMouseHorzDir OnSliderHorizontalClickDrag() const;    //Is on a slider clicking and dragging horizontally.
    bool            DidWeMove( int newX, int newY ) const;
    void            Move( int mouseX, int mouseY, long screenWidth, long screenHeight );
    void            Update();

    void SetInGameMode( bool ingame );
    void SetInGameOverride( bool override );

private:
    void SetupInGameMode();

private:
    MouseCursor*    m_pCursor;
    bool            m_bMoved;
    bool            m_bClickable;
    bool            m_bSelectable;
    bool            m_bMovable;
    bool            m_bClickAndStop; // if this is on, if a button is clicked, mouse processing stops.
    char            m_button[ NUM_BUTTON_TYPES ];
    eFEHotspotType  m_hotSpotType;
    eFEMouseHorzDir m_horzDir;

    int             m_oldPositionX, //We save the last position of the mouse.
                    m_oldPositionY;

    bool            m_bInGame;
    bool            m_bInGameOverride;
    int             m_inGamePosX;
    int             m_inGamePosY;

    int             m_buttonDownSelection;
};

#endif