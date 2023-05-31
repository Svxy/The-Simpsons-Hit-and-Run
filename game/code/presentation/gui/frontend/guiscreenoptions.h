//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenOptions
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

#ifndef GUISCREENOPTIONS_H
#define GUISCREENOPTIONS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <cheats/cheatinputsystem.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenOptions : public CGuiScreen,
                          public ICheatEnteredCallback
{
public:
    CGuiScreenOptions( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenOptions();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual void OnCheatEntered( eCheatID cheatID, bool isEnabled );
    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void UpdateCheatsDisplay( int cheatID );

    void ProgressiveModeTestBegin();
    void OnProgressiveModeTestEnd();

    enum eMenuItem
    {
#ifdef RAD_WIN32
        MENU_ITEM_DISPLAY,
#endif
        MENU_ITEM_CONTROLLER,
        MENU_ITEM_SOUND,
        MENU_ITEM_VIEW_MOVIES,
        MENU_ITEM_VIEW_CREDITS,
        MENU_ITEM_DISPLAY_MODE,

        NUM_MENU_ITEMS
    };

    CGuiMenu* m_pMenu;

    Scrooby::Page* m_cheatsPage;
    Scrooby::Layer* m_cheatsOverlay;
    int m_startingCheatID;

    int m_elapsedProgressiveModeTestTime;

};

#endif // GUISCREENOPTIONS_H
