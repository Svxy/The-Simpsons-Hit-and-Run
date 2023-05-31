//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniPause
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/24      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMINIPAUSE_H
#define GUISCREENMINIPAUSE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================

class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMiniPause : public CGuiScreen
{
public:
    CGuiScreenMiniPause( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMiniPause();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void ResumeGame();
    void QuitGame();

    enum eMenuItem
    {
        MENU_ITEM_CONTINUE,
        MENU_ITEM_QUIT,

        NUM_MENU_ITEMS
    };

    CGuiMenu* m_pMenu;

};

#endif // GUISCREENMINIPAUSE_H
