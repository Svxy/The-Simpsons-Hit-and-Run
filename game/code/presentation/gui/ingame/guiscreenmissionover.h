//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionOver
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

#ifndef GUISCREENMISSIONOVER_H
#define GUISCREENMISSIONOVER_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

#include <events/eventlistener.h>
#include <mission/conditions/missioncondition.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

namespace Scrooby
{
    class Screen;
    class Text;
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMissionOver : public CGuiScreen,
                              public EventListener
{
public:
    CGuiScreenMissionOver( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMissionOver();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual void HandleEvent( EventEnum id, void* pEventData );
    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    static const int MAX_NUM_HINTS_PER_FAILURE = 8;

    void SetFailureMessage( MissionCondition::ConditionTypeEnum conditionType );

    CGuiMenu* m_pMenu;

    Scrooby::Text* m_failureReason;
    Scrooby::Text* m_failureHint;

};

#endif // GUISCREENMISSIONOVER_H
