//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenScrapBookContents
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

#ifndef GUISCREENSCRAPBOOKCONTENTS_H
#define GUISCREENSCRAPBOOKCONTENTS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu2D;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenScrapBookContents : public CGuiScreen
{
public:
    CGuiScreenScrapBookContents( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenScrapBookContents();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

    unsigned int GetCurrentLevel() const;
    void SetLevelBarVisible( bool isVisible );
#ifdef RAD_WIN32
    virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y );
#endif

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    enum eMenuItem
    {
        MENU_STORY_MISSIONS,
        MENU_CHARACTER_CLOTHING,
        MENU_VEHICLES,
        MENU_DUMMY_3,
        MENU_COLLECTOR_CARDS,
        MENU_DUMMY_5,
//        MENU_MOVIES,

        NUM_MENU_ITEMS
    };

    void OnLevelSelectionChange( int delta );
    void OnMenuSelectionMade( eMenuItem selection );

    CGuiMenu2D* m_pMenu;

    Scrooby::Sprite* m_menuImages[ NUM_MENU_ITEMS ];

    Scrooby::Group* m_levelBarGroup;
    Scrooby::Text* m_levelSelection;
    Scrooby::Group* m_LTrigger;
    Scrooby::Group* m_RTrigger;
    Scrooby::Sprite* m_LTriggerBgd;
    Scrooby::Sprite* m_RTriggerBgd;
    unsigned int m_currentLevel;

    Scrooby::Layer* m_sparkles;

};

//===========================================================================
// Inlines
//===========================================================================

inline unsigned int CGuiScreenScrapBookContents::GetCurrentLevel() const
{
    return m_currentLevel;
}

#endif // GUISCREENSCRAPBOOKCONTENTS_H
