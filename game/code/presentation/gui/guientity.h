//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiEntity
//
// Description: Interface for the CGuiEntity class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/09/20		DChau		Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUIENTITY_H
#define GUIENTITY_H

//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// Forward References
//===========================================================================

#ifndef NULL
	#define NULL 0
#endif

#ifdef RAD_GAMECUBE
    const int PLATFORM_TEXT_INDEX = 0;
#endif
#ifdef RAD_PS2
    const int PLATFORM_TEXT_INDEX = 1;
#endif
#ifdef RAD_XBOX
    const int PLATFORM_TEXT_INDEX = 2;
#endif
#ifdef RAD_WIN32 // parallel the xbox for now.
    const int PLATFORM_TEXT_INDEX = 2;
#endif

enum eGuiMessage
{
	GUI_MSG_UPDATE,                         // GUI system -> Manager -> current screen
    GUI_MSG_CONTROLLER_ALPHA,               // begin controller messages
	GUI_MSG_CONTROLLER_UP,                  //
	GUI_MSG_CONTROLLER_DOWN,
	GUI_MSG_CONTROLLER_LEFT,
	GUI_MSG_CONTROLLER_RIGHT,
	GUI_MSG_CONTROLLER_AUX_UP,              // on right analog stick
	GUI_MSG_CONTROLLER_AUX_DOWN,            // on right analog stick
	GUI_MSG_CONTROLLER_AUX_LEFT,            // on right analog stick
	GUI_MSG_CONTROLLER_AUX_RIGHT,           // on right analog stick
    GUI_MSG_CONTROLLER_START,
    GUI_MSG_CONTROLLER_SELECT,
    GUI_MSG_CONTROLLER_BACK,
    GUI_MSG_CONTROLLER_AUX_X,
    GUI_MSG_CONTROLLER_AUX_Y,
    GUI_MSG_CONTROLLER_L1,
    GUI_MSG_CONTROLLER_R1,
    GUI_MSG_CONTROLLER_CONNECT,
    GUI_MSG_CONTROLLER_DISCONNECT,          //
    GUI_MSG_CONTROLLER_OMEGA,               // end controller messages
    GUI_MSG_PROJECT_LOAD_COMPLETE,          // Scrooby File Handler -> GUI System
	GUI_MSG_INIT_BOOTUP,     	            // client -> System
	GUI_MSG_RELEASE_BOOTUP, 	            // client -> System
    GUI_MSG_INIT_FRONTEND,                  // client -> System
	GUI_MSG_RELEASE_FRONTEND,	            // client -> System
    GUI_MSG_INIT_MINIGAME,                  // client -> System
	GUI_MSG_RELEASE_MINIGAME,	            // client -> System
	GUI_MSG_INIT_INGAME,	                // client -> System
	GUI_MSG_RELEASE_INGAME,                 // client -> System
	GUI_MSG_PRE_RUN_BACKEND,                // ManagerInGame -> ManagerBackEnd
	GUI_MSG_RUN_BACKEND,	                // client -> System
	GUI_MSG_RUN_FRONTEND,	                // client -> System
	GUI_MSG_RUN_MINIGAME,	                // client -> System
	GUI_MSG_RUN_INGAME,		                // client -> System
    GUI_MSG_RUN_DEMO,                       // client -> System
    GUI_MSG_PAUSE_INGAME,	                // System -> ManagerInGame
    GUI_MSG_UNPAUSE_INGAME,                 // Pause Menu Screen -> ManagerInGame
	GUI_MSG_RESUME_INGAME,                  // System -> ManagerInGame
    GUI_MSG_QUIT_BOOTUP,                    // client -> ManagerBootUp
    GUI_MSG_QUIT_BACKEND,                   // client -> ManagerBackEnd
    GUI_MSG_QUIT_FRONTEND,                  // Screen -> ManagerFrontEnd
    GUI_MSG_QUIT_MINIGAME,                  // Screen -> ManagerMiniGame
    GUI_MSG_QUIT_INGAME,                    // PauseMenu -> ManagerInGame
    GUI_MSG_QUIT_INGAME_FOR_RELOAD,         // MissionSelect -> ManagerInGame
    GUI_MSG_QUIT_DEMO,                      // client/screen -> ManagerBackEnd
    GUI_MSG_QUIT_GAME,                      // pc.  Screen -> ManagerFrontEnd
    GUI_MSG_BOOTUP_LOAD_COMPLETED,          // screen -> ManagerBootUp
    GUI_MSG_LANGUAGE_SELECTION_DONE,        // screen -> ManagerBootUp
    GUI_MSG_MEMCARD_CHECK_COMPLETED,        // screen -> ManagerBootUp / ManagerFrontEnd
    GUI_MSG_SPLASH_SCREEN_DONE,             // screen -> ManagerFrontEnd
	GUI_MSG_GOTO_SCREEN,	                // screen -> ManagerFrontEnd (param1 = screen id)
    GUI_MSG_BACK_SCREEN,                    // screen -> Manager
    GUI_MSG_SHOW_HUD_OVERLAY,               // client -> In-game HUD (param1 = overlay id)
    GUI_MSG_HIDE_HUD_OVERLAY,               // client -> In-game HUD (param1 = overlay id)
    GUI_MSG_INGAME_MISSION_COMPLETE,        // client -> ManagerInGame
    GUI_MSG_INGAME_MISSION_FAILED,          // client -> ManagerInGame
    GUI_MSG_INGAME_MISSION_LOAD_BEGIN,      // client -> ManagerInGame
    GUI_MSG_INGAME_MISSION_LOAD_END,        // client -> ManagerInGame
    GUI_MSG_INGAME_DISPLAY_PROMPT,          // client -> ManagerInGame
    GUI_MSG_WINDOW_ENTER,		            // Manager -> Window
	GUI_MSG_WINDOW_EXIT,		            // Manager -> Window
	GUI_MSG_WINDOW_FINISHED,	            // Window -> Manager
	GUI_MSG_WINDOW_PAUSE,                   // client -> Window
	GUI_MSG_WINDOW_RESUME,                  // client -> Window
    GUI_MSG_DEMOLOOP_BEGIN,                 // ManagerFrontEnd->Screen
    GUI_MSG_DEMOLOOP_END,                   // ManagerFrontEnd->Screen
    GUI_MSG_RESTART_GAME,                   // Pause Menu -> ManagerInGame
    GUI_MSG_MENU_SELECTION_CHANGED,         // Menu -> Screen
    GUI_MSG_MENU_SELECTION_VALUE_CHANGED,   // Menu -> Screen
    GUI_MSG_MENU_SELECTION_MADE,            // Menu -> Screen
    GUI_MSG_MENU_SLIDER_NOT_CHANGING,       // Menu -> Screen
    GUI_MSG_ON_DISPLAY_MESSAGE,             // Message Screen -> Screen
    GUI_MSG_MENU_PROMPT_RESPONSE,           // Prompt Screen -> Screen
    GUI_MSG_ERROR_PROMPT_RESPONSE,           // Prompt Screen -> Screen
    GUI_MSG_PROMPT_START_RESPONSE,          // Prompt Screen -> Screen, only on ps2
    GUI_MSG_DEATH_VOLUME_START,             // Play the death volume transition
    GUI_MSG_MANUAL_RESET,                   // Play the death volume transition
    GUI_MSG_START_IRIS_WIPE_CLOSE,          // Do an iris wipe closing
    GUI_MSG_START_IRIS_WIPE_OPEN,           // Do an iris wipe opening
    GUI_MSG_INGAME_FADE_OUT,                // Fade from IN-GAME to BLACK
    GUI_MSG_INGAME_FADE_IN,                 // Fade from BLACK to IN-GAME
    GUI_MSG_INGAME_FADE_OUT_CANCEL,                // Cancel fading from IN-GAME to BLACK
    GUI_MSG_INGAME_FADE_IN_CANCEL,                 // Cancel fading from BLACK to IN-GAME
    GUI_MSG_LOAD_RESOURCES,                 // GUI manager -> Screen
    GUI_MSG_SET_GOTO_SCREEN_PARAMETERS,     // GUI system -> GUI manager
	GUI_MSG_ON_SAVE_GAME_COMPLETE,          // save game screen -> GUI in-game manager
	GUI_MSG_PROMPT_UPDATE,                  // Prompt Screen -> Screen   called during CGuiScreenPrompt::HandleMessage GUI_MSG_UPDATE
	GUI_MSG_MESSAGE_UPDATE,                  // Message Screen -> Screen called during CGuiScreenMessage::HandleMessage GUI_MSG_UPDATE
#ifdef RAD_WIN32
    GUI_MSG_MOUSE_OVER,                     // Called when a mouse cursor is on a hotspot.
    GUI_MSG_MOUSE_LCLICK,                   // Called when a mouse clicks the left button and releases.
    GUI_MSG_MOUSE_RCLICK,                   // Called when a mouse clicks the right button and releases.
    GUI_MSG_MOUSE_LCLICKHOLD,               // Called when a mouse clicks a hotspot and holds with the left button.
    GUI_MSG_MOUSE_RCLICKHOLD,               // Called when a mouse clicks a hotspot and holds with the right button.
    GUI_MSG_MOUSE_LCLICKDRAG,               // Called when a mouse left clicks on a hotspot and drags.
    GUI_MSG_QUIT_TO_SYSTEM,
#endif

	GUI_MSG_TOTAL
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiEntity
{
public:
	CGuiEntity( CGuiEntity* pParent );
    virtual ~CGuiEntity();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 ) = 0;

    static bool IsControllerMessage( eGuiMessage message );

protected:		
	CGuiEntity* m_pParent;		

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignement. Declare but don't define.
    //
    CGuiEntity( const CGuiEntity& );
    CGuiEntity& operator= ( const CGuiEntity& );

};

inline bool
CGuiEntity::IsControllerMessage( eGuiMessage message )
{
    return (message > GUI_MSG_CONTROLLER_ALPHA && message < GUI_MSG_CONTROLLER_OMEGA);
}

#endif // GUIENTITY_H
