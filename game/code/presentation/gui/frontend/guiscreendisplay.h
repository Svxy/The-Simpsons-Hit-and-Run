//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenDisplay
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/06/16      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENDISPLAY_H
#define GUISCREENDISPLAY_H

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
class CGuiScreenDisplay : public CGuiScreen
{
public:
    CGuiScreenDisplay( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenDisplay();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void ApplySettings();

private:
    enum eMenuItem
    {
        MENU_ITEM_RESOLUTION,
        MENU_ITEM_COLOUR_DEPTH,
        MENU_ITEM_DISPLAY_MODE,
        MENU_ITEM_GAMMA,
        MENU_ITEM_APPLY_CHANGES,

        NUM_MENU_ITEMS
    };

    CGuiMenu* m_pMenu;
    bool      m_changedGamma;
};

#endif // GUISCREENDISPLAY_H
