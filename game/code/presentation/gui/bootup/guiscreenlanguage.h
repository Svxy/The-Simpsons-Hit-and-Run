//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLanguage
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

#ifndef GUISCREENLANGUAGE_H
#define GUISCREENLANGUAGE_H

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
class CGuiScreenLanguage : public CGuiScreen
{
public:
    CGuiScreenLanguage( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenLanguage();

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

#endif // GUISCREENLANGUAGE_H
