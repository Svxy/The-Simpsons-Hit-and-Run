//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenScrapBookStats
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

#ifndef GUISCREENSCRAPBOOKSTATS_H
#define GUISCREENSCRAPBOOKSTATS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenScrapBookStats : public CGuiScreen
{
public:
    CGuiScreenScrapBookStats( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenScrapBookStats();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    enum eGameStats
    {
        STAT_STORY_MISSIONS,
        STAT_BONUS_MISSIONS,
        STAT_STREET_RACES,
        STAT_CARDS,
        STAT_CLOTHING,
        STAT_VEHICLES,
        STAT_WASPS,
        STAT_GAGS,
        STAT_MOVIES,

        NUM_GAME_STATS
    };

    Scrooby::Text* m_gameStats[ NUM_GAME_STATS ];
    Scrooby::Text* m_percentGameComplete;

};

#endif // GUISCREENSCRAPBOOKSTATS_H
