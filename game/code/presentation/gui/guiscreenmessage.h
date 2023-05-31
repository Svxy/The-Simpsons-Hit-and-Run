//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMessage
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/10/21      TChu        Created for SRR2
//
//===========================================================================
 
#ifndef GUISCREENMESSAGE_H
#define GUISCREENMESSAGE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

#include <strings/unicodestring.h>

#ifdef RAD_WIN32
const float MESSAGE_ICON_CORRECTION_SCALE = 0.875f;
#else
const float MESSAGE_ICON_CORRECTION_SCALE = 1.75f;
#endif

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMessage : public CGuiScreen
{
public:
    enum eMessageID
    {
		MSG_ID_CONTROLLER_DISCONNECTED_GC,
		MSG_ID_CONTROLLER_DISCONNECTED_PS2,
		MSG_ID_CONTROLLER_DISCONNECTED_XBOX,
        MSG_ID_AUTO_LOADING_GAME_GC,
        MSG_ID_AUTO_LOADING_GAME_PS2,
        MSG_ID_AUTO_LOADING_GAME_XBOX,
        MSG_ID_AUTO_LOADING_GAME_XBOX_HD,
        MSG_ID_LOADING_GAME_GC,
        MSG_ID_LOADING_GAME_PS2,
        MSG_ID_LOADING_GAME_XBOX,
        MSG_ID_LOADING_GAME_XBOX_HD,
        MSG_ID_SAVING_GAME_GC,
        MSG_ID_SAVING_GAME_PS2,
        MSG_ID_SAVING_GAME_XBOX,
        MSG_ID_SAVING_GAME_XBOX_HD,
		MSG_ID_FORMATTING_GC,
		MSG_ID_FORMATTING_PS2,
		MSG_ID_FORMATTING_XBOX,
		MSG_ID_DELETING_GC,
		MSG_ID_DELETING_PS2,
		MSG_ID_DELETING_XBOX,
        MSG_ID_LOADING_MISSION,
        MSG_ID_PROGRESSIVE_SCAN_TEST,
#ifdef RAD_WIN32
        MSG_ID_LOADING_GAME_PC,
        MSG_ID_SAVING_GAME_PC,
        MSG_ID_AUTO_LOADING_GAME_PC,
#endif

        NUM_MESSAGE_ID
    };

    CGuiScreenMessage( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMessage();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

    static void Display( int messageIndex, CGuiEntity* pCallback = NULL );

    static void FormatMessage( Scrooby::Text* pText,
                               UnicodeChar* originalStringBuffer = NULL,
                               int stringBufferLength = -1 );
    static void GetControllerDisconnectedMessage(int controllerId, char *str, int max_char);

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    CGuiMenu* m_pMenu;

    Scrooby::Text* m_messageText;
    static int s_messageIndex;
    static CGuiEntity* s_pMessageCallback;
    static int s_ControllerDisconnectedPort;

    Scrooby::Sprite* m_messageIcon;
    unsigned int m_elapsedTime;

    UnicodeChar m_originalStringBuffer[ 256 ];
    static void ConvertUnicodeToChar(char *str, P3D_UNICODE* uni_str, int max_char);

};

#endif // GUISCREENMESSAGE_H
