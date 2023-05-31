//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSplash
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/23      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENSPLASH_H
#define GUISCREENSPLASH_H

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
class CGuiScreenSplash : public CGuiScreen
{
public:
    CGuiScreenSplash( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenSplash();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void StartDemoAsMovie();
    void StartDemoInRuntime();

    enum ePressStartText
    {
        TEXT_LOADING,
        TEXT_PRESS_START_GC,
        TEXT_PRESS_START_PS2,
        TEXT_PRESS_START_XBOX,

        NUM_PRESS_START_TEXTS
    };

    CGuiMenu* m_pMenu;
    Scrooby::Text* m_pressStart;

    unsigned int m_demoLoopTime;
    unsigned int m_elapsedTime;

    static short s_demoPlaybackToggle;

};

#endif // GUISCREENSPLASH_H
