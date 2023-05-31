//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionSelect
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/20      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMISSIONSELECT_H
#define GUISCREENMISSIONSELECT_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/utility/numerictext.h>

const int MAX_NUM_REGULAR_MISSIONS = 7;
const int MAX_NUM_MISSIONS_PER_LEVEL = MAX_NUM_REGULAR_MISSIONS + 4 + 1; // +4 street races, +1 bonus

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;
struct MissionRecord;

struct MissionDisplayInfo
{
    static const unsigned int MAX_NUM_INITIALS = 3;

    Scrooby::Text* m_number;
    Scrooby::Text* m_title;
    Scrooby::Sprite* m_status;
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMissionSelect : public CGuiScreen
{
public:
    CGuiScreenMissionSelect( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMissionSelect();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }
#ifdef RAD_WIN32
    virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y );
#endif

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void OnLevelSelectionChange( int currentLevel );
    void UpdateMissionStatus( int index, MissionRecord* missionRecord );

    CGuiMenu* m_pMenuLevel;
    CGuiMenu* m_pMenu;
    int m_numLevelSelections;

#ifdef RAD_WIN32
    Scrooby::Sprite* m_leftArrow;
    Scrooby::Sprite* m_rightArrow;
#endif

    MissionDisplayInfo m_missionInfo[ MAX_NUM_REGULAR_MISSIONS ];

};

#endif // GUISCREENMISSIONSELECT_H
