//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerInGame
//
// Description: Interface for the CGuiManagerInGame class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/09/20		DChau		Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUIMANAGERINGAME_H
#define GUIMANAGERINGAME_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guimanager.h>
#include <presentation/gui/utility/transitions.h>
#include <data/memcard/memorycardmanager.h>
#include <input/inputmanager.h>

#include <app.h>

//===========================================================================
// Forward References
//===========================================================================

class CGuiScreenHud;
namespace GuiSFX
{
    class RecieveEvent;

    class AbortCurrentMission : public GuiSFX::Chainable1
    {
        public:
            virtual void Activate();
    }; //class AbortCurrentMission;

    class OpenIris : public GuiSFX::Chainable1
    {
        public:
            virtual void Activate();
    }; //class RestartCurrentMission;

    class RestartCurrentMission : public GuiSFX::Chainable1
    {
        public:
            virtual void Activate();
    }; //class RestartCurrentMission;
}; //namespace GuiSFX

namespace Scrooby
{
    class Layer;
}

enum eOnHudEnterCommand
{
    ON_HUD_ENTER_NO_COMMAND = 0,

    ON_HUD_ENTER_RESTART_MISSION,
    ON_HUD_ENTER_ABORT_MISSION,
    ON_HUD_ENTER_SKIP_MISSION,

    NUM_ON_HUD_ENTER_COMMANDS
};

//===========================================================================
// Interface Definitions
//===========================================================================

class CGuiManagerInGame : public CGuiManager,
                          public Scrooby::LoadProjectCallback,
                          public IMemoryCardInfoLoadCallback
{
public:
    CGuiManagerInGame( Scrooby::Project* pProject, CGuiEntity* pParent );
    virtual ~CGuiManagerInGame();

    virtual void Populate();
    virtual void Start( CGuiWindow::eGuiWindowID initialWindow = CGuiWindow::GUI_WINDOW_ID_UNDEFINED );
	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    
    virtual void HandleEvent( EventEnum id, void* pEventData );

    int GetNextLevelToLoad() const { return m_nextLevelIndex; }
    int GetNextMissionToLoad() const { return m_nextMissionIndex; }

    bool IsEnteringPauseMenu() const { return m_enteringPauseMenu; }
    bool isExitingPauseMenu() const { return m_exitingPauseMenu; }

    bool IsLoadingNewMissionInSundayDrive() const;  // newspaper loading screen displayed
    bool IsLoadingNewMission() const;               // mission briefing screen displayed

    CGuiWindow::eGuiWindowID GetResumeGameScreenID() const { return m_resumeGameScreenID; }

    //
    // Implements Scrooby::LoadProjectCallback interface.
    //
    virtual void OnProjectLoadComplete( Scrooby::Project* pProject );

    // Implements IMemoryCardInfoLoadCallback interface.
    //
    virtual void OnMemcardInfoLoadComplete();

    // Pointer to current HUD.
    //
    static CGuiScreenHud* s_currentHUD;

private:

    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CGuiManagerInGame( const CGuiManagerInGame& );
    CGuiManagerInGame& operator= ( const CGuiManagerInGame& );

    bool IsHudScreen( CGuiWindow::eGuiWindowID windowID ) const;
    bool IsRewardsScreen( CGuiWindow::eGuiWindowID windowID ) const;

    enum eInGameProject
    {
        PROJECT_INGAME,
        PROJECT_PAUSE,
        PROJECT_REWARDS,

        NUM_INGAME_PROJECTS
    };

	void PopulateInGame( eInGameProject project = PROJECT_INGAME );

    void UpdateDuringDynamicLoading( unsigned int elapsedTime );
    void UpdateDuringMissionLoading( unsigned int elapsedTime );
    void UpdateWhileLoadingNotDone( unsigned int elapsedTime );

    void GotoPauseScreen();
    void GotoHUDScreen();

    void ResumeGame( unsigned int param1 = 0, unsigned int param2 = 0 );
    void QuitGame();

    void OnControllerDisconnected( int controllerID );
    void OnControllerConnected( int controllerID );

    void OnNewMissionLoadBegin();
    void OnNewMissionLoadEnd();

    bool IsPausingAllowed() const;

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    int m_nextLevelIndex;
    int m_nextMissionIndex;

    bool m_isLoadingNewMission : 1;
    bool m_quitAndReload : 1;
    bool m_controllerPromptShown : 1;
    bool m_enteringPauseMenu : 1;
    bool m_exitingPauseMenu : 1;

    unsigned int m_onHudEnterCommand;

    Scrooby::Screen* m_levelScreen;
    Scrooby::Layer* m_levelLayer;
    unsigned int m_elapsedDynaLoadTime;

    Scrooby::Project* m_pRewardsProject;

    bool m_unloadMemcardInfoWhenLoaded : 1;
    bool m_promptSaveBeforeQuit : 1;
    bool m_quitAfterSave : 1;
#ifdef RAD_WIN32
    bool m_quitToSystemAfterSave : 1;
#endif
    bool m_isControllerReconnected : 1;

    GuiSFX::Dummy                  m_RestartMissionTransition;
    GuiSFX::RecieveEvent           m_RecieveIrisClosed;
    GuiSFX::RestartCurrentMission  m_RestartMission;

    GuiSFX::Dummy                  m_AbortMissionTransition;
    GuiSFX::AbortCurrentMission    m_AbortMission;

    Input::ActiveState m_oldControllerState;

    CGuiWindow::eGuiWindowID m_resumeGameScreenID;

};

inline CGuiScreenHud* GetCurrentHud()
{
    // return reference to current in-game HUD
    //
    return CGuiManagerInGame::s_currentHUD;
}

inline bool CGuiManagerInGame::IsLoadingNewMissionInSundayDrive() const
{
    return m_isLoadingNewMission;
}

inline bool CGuiManagerInGame::IsLoadingNewMission() const
{
    return ( m_currentScreen == CGuiWindow::GUI_SCREEN_ID_MISSION_LOAD );
}

inline bool CGuiManagerInGame::IsHudScreen( CGuiWindow::eGuiWindowID windowID ) const
{
    return( windowID == CGuiWindow::GUI_SCREEN_ID_HUD ||
            windowID == CGuiWindow::GUI_SCREEN_ID_MULTI_HUD );
}

inline bool CGuiManagerInGame::IsRewardsScreen( CGuiWindow::eGuiWindowID windowID ) const
{
    return( windowID == CGuiWindow::GUI_SCREEN_ID_PURCHASE_REWARDS );
}

#endif // GUIMANAGERINGAME_H
