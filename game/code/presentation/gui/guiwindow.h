//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiWindow
//
// Description: Interface for the CGuiWindow class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions        Date            Author      Revision
//                  2000/09/20      DChau       Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUIWINDOW_H
#define GUIWINDOW_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guientity.h>
#include <presentation/gui/guimenu.h>
#ifdef RAD_WIN32
#include <input/FEMouse.h>
#endif

//===========================================================================
// Forward References
//===========================================================================
class CGuiManager;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiWindow : public CGuiEntity
{
    public:
        enum eGuiWindowID
        {
            GUI_WINDOW_ID_UNDEFINED = -1,

            // generic screens
            //
            GUI_SCREEN_ID_GENERIC_MESSAGE,
            GUI_SCREEN_ID_GENERIC_PROMPT,
            GUI_SCREEN_ID_ERROR_PROMPT,

            // bootup and backend screens
            //
            GUI_SCREEN_ID_BOOTUP_LOAD,
            GUI_SCREEN_ID_LICENSE,
            GUI_SCREEN_ID_LANGUAGE,
            GUI_SCREEN_ID_LOADING,
            GUI_SCREEN_ID_LOADING_FE,
            GUI_SCREEN_ID_DEMO,

            GUI_SCREEN_ID_MEMORY_CARD,
            GUI_SCREEN_ID_MEMORY_CARD_CHECK,
            GUI_SCREEN_ID_AUTO_LOAD,

            // front-end screens
            //
            GUI_SCREEN_ID_SPLASH,
            GUI_SCREEN_ID_INTRO_TRANSITION,
            GUI_SCREEN_ID_MAIN_MENU,
            GUI_SCREEN_ID_LOAD_GAME,
            GUI_SCREEN_ID_SCRAP_BOOK,
            GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS,
            GUI_SCREEN_ID_SCRAP_BOOK_STATS,
            GUI_SCREEN_ID_CARD_GALLERY,
            GUI_SCREEN_ID_MISSION_GALLERY,
            GUI_SCREEN_ID_SKIN_GALLERY,
            GUI_SCREEN_ID_VEHICLE_GALLERY,
//            GUI_SCREEN_ID_MULTIPLAYER_SETUP,
//            GUI_SCREEN_ID_MULTIPLAYER_CHOOSE_CHARACTER,
            GUI_SCREEN_ID_OPTIONS,
            GUI_SCREEN_ID_CONTROLLER,
            GUI_SCREEN_ID_SOUND,
            GUI_SCREEN_ID_VIEW_MOVIES,
            GUI_SCREEN_ID_VIEW_CREDITS,
#ifdef RAD_WIN32
            GUI_SCREEN_ID_DISPLAY,
#endif
            GUI_SCREEN_ID_PLAY_MOVIE,
            GUI_SCREEN_ID_PLAY_MOVIE_DEMO,
            GUI_SCREEN_ID_PLAY_MOVIE_INTRO,
            GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME,

            // in-game screens
            //
            GUI_SCREEN_ID_HUD,
            GUI_SCREEN_ID_MULTI_HUD,
			GUI_SCREEN_ID_PAUSE_SUNDAY,
			GUI_SCREEN_ID_PAUSE_MISSION,
            GUI_SCREEN_ID_MISSION_SELECT,
            GUI_SCREEN_ID_SETTINGS,
            GUI_SCREEN_ID_LEVEL_STATS,
            GUI_SCREEN_ID_VIEW_CARDS,
            GUI_SCREEN_ID_SAVE_GAME,
            GUI_SCREEN_ID_MISSION_LOAD,
            GUI_SCREEN_ID_MISSION_OVER,
            GUI_SCREEN_ID_MISSION_SUCCESS,
            GUI_SCREEN_ID_LETTER_BOX,
            GUI_SCREEN_ID_PHONE_BOOTH,
            GUI_SCREEN_ID_PURCHASE_REWARDS,
            GUI_SCREEN_ID_IRIS_WIPE,
            GUI_SCREEN_ID_LEVEL_END,
            GUI_SCREEN_ID_TUTORIAL,

            // mini-game screens
            //
            GUI_SCREEN_ID_MINI_MENU,
            GUI_SCREEN_ID_MINI_HUD,
            GUI_SCREEN_ID_MINI_PAUSE,
            GUI_SCREEN_ID_MINI_SUMMARY,

            NUM_GUI_WINDOW_IDS
        };

        CGuiWindow( eGuiWindowID id, CGuiEntity* pParent );
        virtual ~CGuiWindow();

        virtual void HandleMessage( eGuiMessage message, 
                                    unsigned int param1 = 0,
                                    unsigned int param2 = 0 );

        virtual CGuiMenu* HasMenu() { return NULL; }
        eGuiWindowID GetWindowID( void ) const { return( m_ID ); };
        void ForceClearTransitions();

        bool IsRunning(void) {return m_state == GUI_WINDOW_STATE_RUNNING;}

#ifdef RAD_WIN32
        virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y )
        {
            return HOTSPOT_NONE;
        }
#endif
    protected:
        
        //---------------------------------------------------------------------
        // Protected Functions
        //---------------------------------------------------------------------

        virtual void InitIntro()   = 0;
        virtual void InitRunning() = 0;
        virtual void InitOutro()   = 0;
        virtual void CleanUp();

        enum eGuiWindowState
        {
            GUI_WINDOW_STATE_UNITIALIZED,
            GUI_WINDOW_STATE_INTRO,
            GUI_WINDOW_STATE_RUNNING,
            GUI_WINDOW_STATE_PAUSED,
            GUI_WINDOW_STATE_IDLE,
            GUI_WINDOW_STATE_OUTRO,

            GUI_WINDOW_STATE_DISABLED,

            GUI_WINDOW_STATE_COUNT
        };

        eGuiWindowState m_state;
        eGuiWindowState m_prevState;

        eGuiWindowID m_ID;

        int m_numTransitionsPending;

        bool m_firstTimeEntered;

    private:

        //---------------------------------------------------------------------
        // Private Functions
        //---------------------------------------------------------------------

        // No copying or assignement. Declare but don't define.
        //
        CGuiWindow( const CGuiWindow& );
        CGuiWindow& operator= ( const CGuiWindow& );

};

#endif // GUIWINDOW_H
