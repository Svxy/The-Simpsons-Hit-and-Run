//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiSystem
//
// Description: Interface for the CGuiSystem class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/09/20		DChau		Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUISYSTEM_H
#define GUISYSTEM_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guientity.h>
#include <data/gamedata.h>
#include <app.h>

//===========================================================================
// External Constants
//===========================================================================

extern unsigned int g_scroobySimulationTime;

static const char* SCROOBY_INVENTORY_LANGUAGE = "ScroobyLanguage";
static const char* SCROOBY_INVENTORY_BOOTUP = "ScroobyBootup";
static const char* SCROOBY_INVENTORY_BACKEND = "ScroobyBackend";
static const char* SCROOBY_INVENTORY_FRONTEND = "ScroobyFrontend";
static const char* SCROOBY_INVENTORY_MINIGAME = "ScroobyMiniGame";
static const char* SCROOBY_INVENTORY_INGAME = "ScroobyIngame";
static const char* SCROOBY_INVENTORY_INGAME_LEVEL = "ScroobyIngameLevel";

//===========================================================================
// Forward References
//===========================================================================

class CGuiTextBible;
class CGuiManager;
class CGuiManagerLanguage;
class CGuiManagerBootUp;
class CGuiManagerBackEnd;
class CGuiManagerFrontEnd;
class CGuiManagerMiniGame;
class CGuiManagerInGame;
class CGuiUserInputHandler;
class CGuiScreenMultiHud;

// Implement this interface to be called back when the
// async loading of the GUI system is complete.
//
struct IGuiLoadingCallback
{
    enum eGameMode
    {
        GUI_LOADED_FRONT_END,
        GUI_LOADED_IN_GAME,
        GUI_LOADED_MINI_GAME,

        NUM_GAME_MODES
    };

    virtual void OnGuiLoadComplete( eGameMode mode ) = 0;
};

//===========================================================================
// Interface Definitions
//===========================================================================

class CGuiSystem : public CGuiEntity,
                   public Scrooby::LoadProjectCallback,
                   public GameDataHandler
{
public:
    // Static Methods for accessing this singleton.
    static CGuiSystem* CreateInstance();
    static void DestroyInstance();
    static CGuiSystem* GetInstance();

    void Init();
    void Update( unsigned int elapsedTime );

    virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    // Implements Scrooby::LoadProjectCallback interface.
    //
    virtual void OnProjectLoadComplete( Scrooby::Project* pProject );

    void GotoScreen( unsigned int screenID,
                     unsigned int param1 = 0,
                     unsigned int param2 = 0,
                     unsigned int windowOptions = 0 );

    Scrooby::Project* GetCurrentScroobyProject() const { return m_pProject; }
    void SetCurrentScroobyProject( Scrooby::Project* pProject ) { m_pProject = pProject; }
    Scrooby::Project* GetScroobyLevelProject() const { return m_pLevelProject; }

    void SwitchToCurrentProject();
    void SwitchToLevelProject();
    void SwitchToBackendProject();

    // Registration of Client Loading Callback Routine
    //
    void RegisterLoadingCallback( IGuiLoadingCallback* pCallback );
    void UnregisterLoadingCallback();

    // Accessors to GUI System Managers
    //
    CGuiManager* GetCurrentManager() const;

    inline CGuiManagerBootUp* GetBootupManager() const { return m_pManagerBootUp; }
    inline CGuiManagerBackEnd* GetBackendManager() const { return m_pManagerBackEnd; }
    inline CGuiManagerFrontEnd* GetFrontEndManager() const { return m_pManagerFrontEnd; }
    inline CGuiManagerInGame* GetInGameManager() const { return m_pManagerInGame; }
    inline CGuiManagerMiniGame* GetMiniGameManager() const { return m_pManagerMiniGame; }

    // Registration of GUI User Input Handlers
    //
    void RegisterUserInputHandlers( int controllerIDs = ~0 );
    void UnregisterUserInputHandlers( int controllerIDs = ~0 );

    // Accessor to GUI User Input Handlers
    //
    CGuiUserInputHandler* GetUserInputHandler( int controllerId ) const;
    int GetNumUserInputHandlers() const { return m_numUserInputHandlers; }

    // Splash Screen
    //
    inline void SetSplashScreenFinished() { m_isSplashScreenFinished = true; }
    inline bool IsSplashScreenFinished() const { return m_isSplashScreenFinished; }

    // Credits Screen
    //
    void ShowCreditsUponReturnToFE( bool enable );
    bool IsShowCreditsUponReturnToFE() const;

    // Enabling/Disabling HUD Radar
    //
    void SetRadarEnabled( bool isEnabled );
    bool IsRadarEnabled() const;

    // Implements GameDataHandler
    //
    virtual void LoadData( const GameDataByte* dataBuffer, unsigned int numBytes );
    virtual void SaveData( GameDataByte* dataBuffer, unsigned int numBytes );
    virtual void ResetData();

    int GetPrimaryController() const;
    void SetPrimaryController(int id);

    enum eGuiSystemState
	{
		GUI_UNINITIALIZED,
        GUI_IDLE,

        LANGUAGE_LOADING,
        LANGUAGE_ACTIVE,

        BOOTUP_LOADING,
        BOOTUP_ACTIVE,

        BACKEND_LOADING,
        FRONTEND_LOADING_DURING_BOOTUP,

		FRONTEND_LOADING,
		FRONTEND_ACTIVE,

		MINIGAME_LOADING,
		MINIGAME_ACTIVE,

        INGAME_LOADING,
		INGAME_ACTIVE,

        DEMO_ACTIVE,

        NUM_GUI_STATES
	};

    eGuiSystemState GetCurrentState() const { return m_state; }

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No public access; use singleton interface.
    //
	CGuiSystem();
    virtual ~CGuiSystem();

    // No copying or assignment. Declare but don't define.
    //
    CGuiSystem( const CGuiSystem& );
    CGuiSystem& operator= ( const CGuiSystem& );

    void OnInitBootUp();
    void OnReleaseBootUp();

	void OnInitFrontEnd();
    void OnReleaseFrontEnd();

	void OnInitMiniGame();
    void OnReleaseMiniGame();

    void OnInitInGame();
    void OnReleaseInGame();

    void FormatTutorialTextWithLineBreaks();

#ifdef DEBUGWATCH
    void RegisterWatcherStuff();
#endif

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    // Pointer to the one and only instance of this singleton.
    static CGuiSystem* spInstance;

	eGuiSystemState m_state;

    CGuiTextBible* m_pTextBible;

    CGuiManagerLanguage* m_pManagerLanguage;
    CGuiManagerBootUp* m_pManagerBootUp;
    CGuiManagerBackEnd* m_pManagerBackEnd;
	CGuiManagerFrontEnd* m_pManagerFrontEnd;
    CGuiManagerMiniGame* m_pManagerMiniGame;
	CGuiManagerInGame* m_pManagerInGame;

    CGuiUserInputHandler** m_pUserInputHandlers;
    int m_numUserInputHandlers;
    int m_registeredUserInputHandlers;
    int m_primaryController;

	Scrooby::App* m_pApp;
    Scrooby::Project* m_pProject;
    Scrooby::Project* m_pLevelProject;
    Scrooby::Project* m_pBackendProject;

    IGuiLoadingCallback* m_pLoadingCallback;

    bool m_isSplashScreenFinished : 1;
    bool m_isRadarEnabled : 1;
    bool m_isShowCreditsUponReturnToFE : 1;

};

//===========================================================================
// Inline Methods
//===========================================================================

// A little syntactic sugar for getting at this singleton.
//
inline CGuiSystem* GetGuiSystem() { return( CGuiSystem::GetInstance() ); }

inline void CGuiSystem::SwitchToCurrentProject()
{
    rAssert( m_pProject != NULL );
    rAssert( m_pApp != NULL );
    m_pApp->SetProject( m_pProject );
}

inline void CGuiSystem::SwitchToLevelProject()
{
    if( m_pLevelProject != NULL )
    {
        rAssert( m_pApp != NULL );
        m_pApp->SetProject( m_pLevelProject );
    }
}

inline void CGuiSystem::SwitchToBackendProject()
{
    rAssert( m_pBackendProject != NULL );
    rAssert( m_pApp != NULL );
    m_pApp->SetProject( m_pBackendProject );
}

inline void CGuiSystem::RegisterLoadingCallback( IGuiLoadingCallback* pCallback )
{
    m_pLoadingCallback = pCallback;
}

inline void CGuiSystem::UnregisterLoadingCallback()
{
    m_pLoadingCallback = NULL;
}

inline void CGuiSystem::SetRadarEnabled( bool isEnabled )
{
    m_isRadarEnabled = isEnabled;
}

inline bool CGuiSystem::IsRadarEnabled() const
{
    return m_isRadarEnabled;
}

inline void CGuiSystem::ShowCreditsUponReturnToFE( bool enable )
{
    m_isShowCreditsUponReturnToFE = enable;
}

inline bool CGuiSystem::IsShowCreditsUponReturnToFE() const
{
    return m_isShowCreditsUponReturnToFE;
}

#endif // GUISYSTEM_H
