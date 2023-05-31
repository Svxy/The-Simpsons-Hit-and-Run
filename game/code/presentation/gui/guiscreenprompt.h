//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPrompt
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENPROMPT_H
#define GUISCREENPROMPT_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/guimenu.h>

#include <strings/unicodestring.h>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Group;
}

enum ePromptQuestion
{
    PROMPT_CONFIRM_QUIT,
    PROMPT_CONFIRM_SAVE_BEFORE_QUIT,
    PROMPT_CONFIRM_RESTART,
    PROMPT_CONFIRM_ABORT,
    PROMPT_CONFIRM_RACE_AGAIN,
    PROMPT_CONFIRM_NEW_GAME,
    PROMPT_CONFIRM_START_MISSION,
    PROMPT_LOAD_CONFIRM_GC,
    PROMPT_LOAD_CONFIRM_PS2,
    PROMPT_LOAD_CONFIRM_XBOX,

    PROMPT_SAVE_CONFIRM_GC,
    PROMPT_SAVE_CONFIRM_PS2,
    PROMPT_SAVE_CONFIRM_XBOX,
    PROMPT_SAVE_CONFIRM_OVERWRITE_GC,
    PROMPT_SAVE_CONFIRM_OVERWRITE_PS2,
    PROMPT_SAVE_CONFIRM_OVERWRITE_XBOX,

    PROMPT_LOAD_SUCCESSFUL,
    PROMPT_SAVE_SUCCESSFUL,

	PROMPT_FORMAT_CONFIRM_GC,
	PROMPT_FORMAT_CONFIRM_PS2,
	PROMPT_FORMAT_CONFIRM_XBOX,

	PROMPT_FORMAT_CONFIRM2_GC,
	PROMPT_FORMAT_CONFIRM2_PS2,
	PROMPT_FORMAT_CONFIRM2_XBOX,

    PROMPT_FORMAT_SUCCESS_GC,
    PROMPT_FORMAT_SUCCESS_PS2,
    PROMPT_FORMAT_SUCCESS_XBOX,

    PROMPT_FORMAT_FAIL_GC,
    PROMPT_FORMAT_FAIL_PS2,
    PROMPT_FORMAT_FAIL_XBOX,

    PROMPT_DELETE_CORRUPT_SUCCESS_GC,
    PROMPT_DELETE_CORRUPT_SUCCESS_PS2,
    PROMPT_DELETE_CORRUPT_SUCCESS_XBOX,

    PROMPT_DELETE_CORRUPT_FAIL_GC,
    PROMPT_DELETE_CORRUPT_FAIL_PS2,
    PROMPT_DELETE_CORRUPT_FAIL_XBOX,

	PROMPT_LOAD_CARD_EMPTY_GC,
	PROMPT_LOAD_CARD_EMPTY_PS2,
	PROMPT_LOAD_CARD_EMPTY_XBOX,
	PROMPT_LOAD_CARD_EMPTY_XBOX_HD,
#ifdef RAD_WIN32
    PROMPT_LOAD_CARD_EMPTY_PC,
#endif

    PROMPT_DISPLAY_PROGRESSIVE_SCAN,
    PROMPT_DISPLAY_PROGRESSIVE_SCAN_PS2,
    PROMPT_DISPLAY_PROGRESSIVE_SCAN_CONFIRM,
    PROMPT_DISPLAY_PROGRESSIVE_SCAN_TEST,

	PROMPT_LOAD_DELETE_CORRUPT_GC,
	PROMPT_LOAD_DELETE_CORRUPT_PS2,
	PROMPT_LOAD_DELETE_CORRUPT_XBOX,
    PROMPT_LOAD_DELETE_CORRUPT_XBOX_HD,
    
    PROMPT_HD_FULL_XBOX,
#ifdef RAD_WIN32
    PROMPT_CONFIRM_RESTOREALLDEFAULTS,
    PROMPT_CONFIRM_QUIT_TO_SYSTEM,
    PROMPT_CONFIRM_SAVE_BEFORE_QUITTOSYSTEM,
#endif
    NUM_PROMPT_QUESTIONS
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPrompt : public CGuiScreen
{
public:
    CGuiScreenPrompt( Scrooby::Screen* pScreen,
                      CGuiEntity* pParent,
                      eGuiWindowID windowID = GUI_SCREEN_ID_GENERIC_PROMPT );

    virtual ~CGuiScreenPrompt();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

    static void Display( int messageIndex,
                         CGuiEntity* pCallback,
                         int numResponses,
                         CGuiMenuPrompt::ePromptResponse* responses );

    static void EnableDefaultToNo( bool enable );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    CGuiMenuPrompt* m_pMenu;

    Scrooby::Sprite* m_messageIcon;
    unsigned int m_elapsedTime;

    Scrooby::Group* m_xboxDashboardLabel;

    Scrooby::Text* m_promptMessage;
    static int s_messageIndex;
    static int s_numResponses;
    static CGuiMenuPrompt::ePromptResponse s_responses[ CGuiMenuPrompt::MAX_NUM_RESPONSES ];
    static CGuiEntity* s_pPromptCallback;
    static bool s_defaultToNo;

    UnicodeChar m_originalStringBuffer[ 256 ];

    Scrooby::Layer* m_tvFrame;

};

inline void CGuiScreenPrompt::EnableDefaultToNo( bool enable )
{
    s_defaultToNo = enable;
}

#endif // GUISCREENPROMPT_H
