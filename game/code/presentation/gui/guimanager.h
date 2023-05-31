//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManager
//
// Description: Interface for the CGuiManager class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions        Date            Author      Revision
//                  2000/09/20      DChau       Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guientity.h>
#include <presentation/gui/guiwindow.h>
#include <events/eventlistener.h>
#include <project.h>

//===========================================================================
// Forward References
//===========================================================================
struct IGuiLoadingCallback;

//===========================================================================
// Constants
//===========================================================================

const int MAX_WINDOW_HISTORY = 8;

enum eWindowCommand
{
    CLEAR_WINDOW_HISTORY = 1,
    KEEP_WINDOW_HISTORY = 2,
    FORCE_WINDOW_CHANGE_IMMEDIATE = 4,
    FORCE_WINDOW_RELOAD = 8,

    NUM_WINDOW_COMMANDS
};

enum eGenericPromptType
{
    PROMPT_TYPE_YES_NO,
    PROMPT_TYPE_CONTINUE,
    PROMPT_TYPE_OK,
    PROMPT_TYPE_DELETE,
    PROMPT_TYPE_LOAD,
    PROMPT_TYPE_SAVE,

    NUM_GENERIC_PROMPT_TYPES
};

enum eErrorPromptResponse
{
    ERROR_RESPONSE_NONE = 0,

    ERROR_RESPONSE_CONTINUE = 1,
    ERROR_RESPONSE_RETRY = 2,
    ERROR_RESPONSE_YES = 4,
	ERROR_RESPONSE_NO = 8,
	ERROR_RESPONSE_FORMAT = 16,
    ERROR_RESPONSE_MANAGE = 32,
    ERROR_RESPONSE_CONTINUE_WITHOUT_SAVE = 64,
    ERROR_RESPONSE_DELETE = 128,
    ERROR_RESPONSE_ALL = ~0
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiManager : public CGuiEntity,
                    public Scrooby::GotoScreenCallback,
                    public EventListener
{
    public:

        CGuiManager( Scrooby::Project* pProject,
                     CGuiEntity* pParent );

        virtual ~CGuiManager();

        virtual void Populate() = 0;
        virtual void Start( CGuiWindow::eGuiWindowID initialWindow = CGuiWindow::GUI_WINDOW_ID_UNDEFINED ) = 0;

		virtual void HandleMessage( eGuiMessage message, 
			                        unsigned int param1 = 0,
									unsigned int param2 = 0 );

        Scrooby::Project* GetScroobyProject() const { return m_pScroobyProject; }

		virtual void OnGotoScreenComplete();

        CGuiWindow* FindWindowByID( CGuiWindow::eGuiWindowID id );

        CGuiWindow* GetCurrentWindow();

        void DisplayMessage( int messageIndex,
                             CGuiEntity* pCallback = NULL );

        void DisplayPrompt( int messageIndex,
                            CGuiEntity* pCallback,
                            eGenericPromptType promptType = PROMPT_TYPE_YES_NO,
                            bool enableDefaultToNo = true );

        void DisplayErrorPrompt( int messageIndex,
                                 CGuiEntity* pCallback,
                                 int promptResponses = ERROR_RESPONSE_NONE );

        CGuiWindow::eGuiWindowID GetPreviousScreen( int fromCurrentScreen = 0 ) const;
        CGuiWindow::eGuiWindowID GetCurrentScreen() const;

        // Implements EventListener
        virtual void HandleEvent( EventEnum id, void* pEventData ) {};

        enum eMemoryCardCheckingState
        {
            MEM_CARD_CHECK_NOT_DONE,
            MEM_CARD_CHECK_IN_PROGRESS,
            MEM_CARD_CHECK_COMPLETED,

            NUM_MEM_CARD_CHECK_STATES
        };

        static eMemoryCardCheckingState s_memcardCheckState;

    protected:

        //---------------------------------------------------------------------
        // Protected Functions
        //---------------------------------------------------------------------

        void AddWindow( CGuiWindow::eGuiWindowID windowID, CGuiWindow* pWindow );
        void RemoveWindow( CGuiWindow::eGuiWindowID windowID );
        void RemoveAllWindows();

        void PushScreenHistory( CGuiWindow::eGuiWindowID windowID );
        CGuiWindow::eGuiWindowID PopScreenHistory();
        void ClearScreenHistory();

        //---------------------------------------------------------------------
        // Protected Data
        //---------------------------------------------------------------------

        CGuiWindow* m_windows[ CGuiWindow::NUM_GUI_WINDOW_IDS ];
        int m_numWindows;
        
        Scrooby::Project* m_pScroobyProject;

        CGuiWindow::eGuiWindowID m_currentScreen;
		CGuiWindow::eGuiWindowID m_nextScreen;

		enum eGuiFrontEndState
		{
			GUI_FE_UNINITIALIZED,
			GUI_FE_SCREEN_RUNNING,
			GUI_FE_CHANGING_SCREENS,
            GUI_FE_DYNAMIC_LOADING,
			GUI_FE_SHUTTING_DOWN,
			GUI_FE_TERMINATED
		};

		eGuiFrontEndState m_state;

    private:

        //---------------------------------------------------------------------
        // Private Functions
        //---------------------------------------------------------------------

        // No copying or assignment. Declare but don't define.
        //
        CGuiManager( const CGuiManager& );
        CGuiManager& operator= ( const CGuiManager& );

        bool IsValidWindowID( CGuiWindow::eGuiWindowID windowID ) const;

        //---------------------------------------------------------------------
        // Private Data
        //---------------------------------------------------------------------
        CGuiWindow::eGuiWindowID m_windowHistory[ MAX_WINDOW_HISTORY ];
        int m_windowHistoryCount;

        unsigned int m_gotoScreenUserParam1;
        unsigned int m_gotoScreenUserParam2;
};

inline void
CGuiManager::ClearScreenHistory()
{
    m_windowHistoryCount = 0;
}

inline bool
CGuiManager::IsValidWindowID( CGuiWindow::eGuiWindowID windowID ) const
{
    return (windowID >= 0 && windowID < CGuiWindow::NUM_GUI_WINDOW_IDS);
}

#endif // GUIMANAGER_H
