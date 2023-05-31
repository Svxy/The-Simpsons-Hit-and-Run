//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionLoad
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

#ifndef GUISCREENMISSIONLOAD_H
#define GUISCREENMISSIONLOAD_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenmissionbase.h>
#include <presentation/gui/utility/transitions.h>

//===========================================================================
// Forward References
//===========================================================================

class CGuiMenu;
class tSprite;
namespace Scrooby
{
    class Group;
}

const int MAX_NUM_BULLETS = 2;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMissionLoad : 
    public CGuiScreenMissionBase,
    public EventListener
{
public:
    CGuiScreenMissionLoad( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMissionLoad();

    // Implements EventListener
	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    virtual void HandleEvent( EventEnum id, void* pEventData );
    static  void InitializePermanentVariables();
    virtual void InitIntro();
	virtual void InitOutro();

protected:
    virtual void AddListeners();
    void ExtractNormalPositions();
    void InitPositionsNormal();
    void InitPositionsWager();
    virtual void RemoveListeners();

private:
    bool m_LoadIsDone        : 1;  //the mission load is done

    Scrooby::Text* m_loadingText;
    unsigned int m_elapsedIdleTime;

};

#endif // GUISCREENMISSIONLOAD_H
