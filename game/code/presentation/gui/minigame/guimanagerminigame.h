//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerMiniGame
//
// Description: Interface for the CGuiManagerMiniGame class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/24		TChu		Created
//
//===========================================================================

#ifndef GUIMANAGERMINIGAME_H
#define GUIMANAGERMINIGAME_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guimanager.h>
#include <input/inputmanager.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiManagerMiniGame : public CGuiManager
{
public:
    CGuiManagerMiniGame( Scrooby::Project* pProject, CGuiEntity* pParent );
    virtual ~CGuiManagerMiniGame();

	virtual void Populate();
    virtual void Start( CGuiWindow::eGuiWindowID initialWindow = CGuiWindow::GUI_WINDOW_ID_UNDEFINED );
	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    static void LoadCharacters();
    static void UnloadCharacters();

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CGuiManagerMiniGame( const CGuiManagerMiniGame& );
    CGuiManagerMiniGame& operator= ( const CGuiManagerMiniGame& );

    void OnControllerDisconnected( int controllerID, bool force_display = false );
    void OnControllerConnected( int controllerID );
    bool PollControllers();
    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    bool m_isQuittingToSupersprint : 1;
    bool m_controllerPromptShown : 1;
    bool m_isControllerReconnected : 1;
    int m_lastControllerDisconnectedId;
    Input::ActiveState m_oldControllerState;

};

#endif // GUIMANAGERMINIGAME_H
