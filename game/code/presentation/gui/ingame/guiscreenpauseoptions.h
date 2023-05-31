//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseOptions
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

#ifndef GUISCREENPAUSEOPTIONS_H
#define GUISCREENPAUSEOPTIONS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

namespace Scrooby
{
    class Screen;
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPauseOptions : public CGuiScreen
{
public:
    CGuiScreenPauseOptions( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPauseOptions();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    CGuiMenu* m_pMenu;

};

#endif // GUISCREENPAUSEOPTIONS_H
