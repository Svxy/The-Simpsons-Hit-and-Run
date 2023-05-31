//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLevelStats
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

#ifndef GUISCREENLEVELSTATS_H
#define GUISCREENLEVELSTATS_H

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
class CGuiScreenLevelStats : public CGuiScreen
{
public:
    CGuiScreenLevelStats( Scrooby::Screen* pScreen, CGuiEntity* pParent,
                          eGuiWindowID windowID = GUI_SCREEN_ID_LEVEL_STATS );
    virtual ~CGuiScreenLevelStats();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    void UpdateLevelStats();

    enum eLevelStats
    {
        STAT_STORY_MISSIONS,
        STAT_BONUS_MISSIONS,
        STAT_STREET_RACES,
        STAT_CARDS,
        STAT_CLOTHING,
        STAT_VEHICLES,
        STAT_WASPS,
        STAT_GAGS,
        STAT_LEVEL_COMPLETE,

        NUM_LEVEL_STATS
    };

    Scrooby::Text* m_levelStatsHeading;
    Scrooby::Text* m_levelStats[ NUM_LEVEL_STATS ];

    Scrooby::Text* m_useMissionSelect;

};

#endif // GUISCREENLEVELSTATS_H
