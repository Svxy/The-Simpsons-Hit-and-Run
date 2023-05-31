//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSaveGame
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

#ifndef GUISCREENSAVEGAME_H
#define GUISCREENSAVEGAME_H

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
class CGuiScreenSaveGame : public CGuiScreen,
                           public CGuiScreenLoadSave,
                           public GameDataSaveCallback,
                           public GameDataDeleteCallback
{
public:
    CGuiScreenSaveGame( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenSaveGame();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    // Implements GameDataSaveCallback and GameDataDeleteCallback
    //
    virtual void OnSaveGameComplete( radFileError errorCode );
    virtual void OnDeleteGameComplete( radFileError errorCode );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    void GotoMemoryCardScreen( bool isFromPrompt = false );
    void HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response );

private:
    void SaveGame();

    CGuiMenu* m_pMenu;
	Scrooby::Text* m_pFullText;
    bool m_StatusPromptShown;
    unsigned short m_nonEmptySlots;

};

#endif // GUISCREENSAVEGAME_H
