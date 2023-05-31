//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLoadGame
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/30      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENLOADGAME_H
#define GUISCREENLOADGAME_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/guiscreenmemorycard.h>
#include <data/gamedatamanager.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenLoadGame : public CGuiScreen,
                           public CGuiScreenLoadSave,
                           public GameDataLoadCallback
						   
{
public:
    CGuiScreenLoadGame( Scrooby::Screen* pScreen, CGuiEntity* pParent,
                        eGuiWindowID windowID = GUI_SCREEN_ID_LOAD_GAME );
    virtual ~CGuiScreenLoadGame();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    // Implements GameDataLoadCallback
    //
    virtual void OnLoadGameComplete( radFileError errorCode );
    virtual CGuiMenu* HasMenu() { return m_pMenu; }
	

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    void GotoMemoryCardScreen( bool isFromPrompt = false );
    virtual void HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response );
    virtual void LoadGame();

private:
    CGuiMenu* m_pMenu;
    Scrooby::Text* m_pFullText;
    bool m_StatusPromptShown;
};

class CGuiScreenAutoLoad : public CGuiScreenLoadGame
{
public:
    CGuiScreenAutoLoad( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenAutoLoad();

    virtual void OnLoadGameComplete( radFileError errorCode );

    static void SetGameSlot( int slot ) { s_autoLoadGameSlot = slot; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    virtual void HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response );
    virtual void LoadGame();

private:
    static int s_autoLoadGameSlot;

};

#endif // GUISCREENLOADGAME_H
