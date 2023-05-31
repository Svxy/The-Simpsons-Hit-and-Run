//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMultiSetup
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/16      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMULTISETUP_H
#define GUISCREENMULTISETUP_H

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
class CGuiScreenMultiSetup : public CGuiScreen
{
public:
    CGuiScreenMultiSetup( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMultiSetup();

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

#endif // GUISCREENMULTISETUP_H
