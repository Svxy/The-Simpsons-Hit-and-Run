//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenScrapBook
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/10      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENSCRAPBOOK_H
#define GUISCREENSCRAPBOOK_H

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
class CGuiScreenScrapBook : public CGuiScreen
{
public:
    CGuiScreenScrapBook( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenScrapBook();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    enum eMenuItem
    {
        MENU_OPEN_BOOK,
        MENU_GAME_STATS,

        NUM_MENU_ITEMS
    };

    void OnMenuSelectionMade( eMenuItem selection );

    CGuiMenu* m_pMenu;
    Scrooby::Group* m_krustySticker;

};

#endif // GUISCREENSCRAPBOOK_H
