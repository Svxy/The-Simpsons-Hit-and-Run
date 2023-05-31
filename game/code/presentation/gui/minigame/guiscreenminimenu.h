//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniMenu
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/24      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMINIMENU_H
#define GUISCREENMINIMENU_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/utility/numerictext.h>

#include <supersprint/supersprintdata.h>

//#define MINI_MENU_SHOW_3D_CHARACTERS

//===========================================================================
// Forward References
//===========================================================================

class CGuiMenu;
class Reward;

struct PlayerMenu
{
    enum eSubMenu
    {
        MENU_CHARACTERS,
        MENU_VEHICLES,

        NUM_SUB_MENUS
    };

    CGuiMenu* m_pMenu[ NUM_SUB_MENUS ];
    int m_currentSubMenu;
    int m_controllerID;

    Scrooby::Text* m_pressStart;
    Scrooby::Sprite* m_vehicleRating;
    Scrooby::Drawable* m_characterSelectedIcon;
    Scrooby::Drawable* m_vehicleSelectedIcon;

    PlayerMenu();

    void HandleMessage( eGuiMessage message,
                        unsigned int param1 = 0,
                        unsigned int param2 = 0 );

    CGuiMenu* GetCurrentSubMenu() const;
    void SetActive( bool isActive, int controllerID = -1 );
    bool IsActive() const;
    bool IsDone() const;

};

inline bool PlayerMenu::IsActive() const
{
    return( m_controllerID != -1 );
}

inline bool PlayerMenu::IsDone() const
{
    return( m_currentSubMenu >= NUM_SUB_MENUS );
}

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMiniMenu : public CGuiScreen
{
public:
    CGuiScreenMiniMenu( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMiniMenu();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    virtual CGuiMenu* HasMenu() { return m_pTrackMenu; }
#ifdef RAD_WIN32
    virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y );
#endif

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void UpdateCharacterSlots();

    void SetTrackSelectionEnabled( bool enable );
    void MoveTrackCursor( int previousIndex, int nextIndex );
    void OnTrackSelected();

    void ActivateNewPlayer( int controllerID );

    void OnCharacterSelected( int playerID, bool isSelected );
    void OnVehicleSelected( int playerID, bool isSelected );

    void AddRewardVehicle( Reward* pReward );
    void PrepareVehicleSelections();
    void UpdateVehicleDisplayImages( int playerID, Scrooby::Sprite* vehicleImage );
    void UpdateVehicleRating( int playerID );

    void SetTimerEnabled( bool enable );

    int GetPlayerMenuID( int controllerID ) const;

    static const int NUM_TRACKS = 7;
    static const int MAX_NUM_VEHICLES = 64;

#ifdef RAD_DEBUG
    static const int TIMER_WAIT_TIME = 5000; // in msec
#else
    static const int TIMER_WAIT_TIME = 10000; // in msec
#endif

    enum eScreenState
    {
        SCREEN_STATE_NORMAL,
        SCREEN_STATE_OPTIONS,

        NUM_SCREEN_STATES
    };

    enum eCharacter
    {
        CHARACTER_LISA,
        CHARACTER_BART,
        CHARACTER_HOMER,
        CHARACTER_MARGE,
        CHARACTER_APU,

        NUM_CHARACTERS
    };

    enum eCharacterSelectionState
    {
        CHARACTER_NOT_SELECTED = 0,
        CHARACTER_SELECTED = 1,

        NUM_CHARACTER_SELECTION_STATES
    };

/*
    enum eOptionsMenuItem
    {
        MENU_ITEM_NUM_LAPS,
        MENU_ITEM_REVERSE_DIRECTION,

        NUM_OPTIONS_MENU_ITEMS
    };

    eScreenState m_screenState;
*/

    CGuiMenu* m_pTrackMenu;
    Scrooby::Sprite* m_trackDirection;
    Scrooby::Text* m_trackNumLaps;
    Scrooby::Sprite* m_trackNumLapsArrowU;
    Scrooby::Sprite* m_trackNumLapsArrowD;
    Scrooby::Polygon* m_trackCursorBgd;
    unsigned int m_elapsedTime;

    Scrooby::Text* m_backLabel;

    PlayerMenu m_playerMenus[ SuperSprintData::NUM_PLAYERS ];
    int m_numActivePlayers;
    int m_numActivePlayersDone;
    short m_characterSlots; // bit-mask

#ifdef MINI_MENU_SHOW_3D_CHARACTERS
    Scrooby::Pure3dObject* m_3dCharacters[ NUM_CHARACTERS ];
#endif

#ifdef RAD_WIN32
    int m_currentTrack;
    bool m_bTrackSelected;
#endif

    Reward* m_unlockedVehicles[ MAX_NUM_VEHICLES ];
    int m_numUnlockedVehicles;

    Scrooby::Group* m_optionsButton;
/*
    Scrooby::Layer* m_optionsOverlay;
    CGuiMenu* m_optionsMenu;
*/

    Scrooby::Group* m_characterSelectInfo;

    Scrooby::Group* m_timerOverlay;
    NumericText<Scrooby::Text> m_timer;
    int m_remainingTime;

};

#endif // GUISCREENMINIMENU_H
