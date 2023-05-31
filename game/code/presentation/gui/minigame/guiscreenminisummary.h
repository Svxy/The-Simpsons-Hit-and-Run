//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniSummary
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

#ifndef GUISCREENMINISUMMARY_H
#define GUISCREENMINISUMMARY_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

#include <supersprint/supersprintdata.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMiniSummary : public CGuiScreen
{
public:
    CGuiScreenMiniSummary( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMiniSummary();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void UpdateDisplayInfo( int rankIndex, int playerID );

    void ResetCurrentRankings();
    void InsertPlayerRanking( int playerID );

    struct PlayerDisplayInfo
    {
        Scrooby::Sprite* m_face;
        Scrooby::Text* m_ranking;
        Scrooby::Text* m_playerIndicator;
        Scrooby::Text* m_numWins;
        Scrooby::Text* m_numPoints;
        Scrooby::Text* m_totalTime;
        Scrooby::Text* m_bestLap;
        Scrooby::Polygon* m_rowBgd;

        void SetVisible( bool isVisible );
    };

    PlayerDisplayInfo m_displayInfo[ SuperSprintData::NUM_PLAYERS ];

    struct PlayerRanking
    {
        int m_playerID;
        int m_playerPoints;
        int m_playerWins;

        static int CompareScores( int points1, int wins1,
                                  int points2, int wins2 );
    };

    PlayerRanking m_currentRankings[ SuperSprintData::NUM_PLAYERS ];

    Scrooby::Sprite* m_totalTimeKingIcon;
    Scrooby::Sprite* m_bestLapKingIcon;

};

#endif // GUISCREENMINISUMMARY_H
