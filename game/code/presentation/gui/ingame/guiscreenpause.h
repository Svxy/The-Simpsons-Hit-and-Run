//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPause
//
// Description: 
//              
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/11/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUISCREENPAUSE_H
#define GUISCREENPAUSE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <cards/cardgallery.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPause : public CGuiScreen
{
public:
    CGuiScreenPause( Scrooby::Screen* pScreen,
                     CGuiEntity* pParent,
                     eGuiWindowID id );
    virtual ~CGuiScreenPause();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    void HandleResumeGame( unsigned int param1 = 0,
                           unsigned int param2 = 0 );
    void HandleQuitGame();
#ifdef RAD_WIN32
    void HandleQuitToSystem();
#endif
    #ifdef DEBUGWATCH
        virtual const char* GetWatcherName() const;
    #endif


    CGuiMenu* m_pMenu;
    Scrooby::Group* m_MissionObjectiveBox;
    Scrooby::Text* m_pressStart;
    Scrooby::Text* m_missionObjective;
    Scrooby::Sprite* m_objectiveIcon;

private:
/*
    void HideCards();
    void ShowNextCard();
*/

    Scrooby::Sprite* m_collectedCards[ NUM_CARDS_PER_LEVEL ];
    int m_numVisibleCards;
    unsigned int m_elapsedTime;

};

#endif // GUISCREENPAUSE_H
