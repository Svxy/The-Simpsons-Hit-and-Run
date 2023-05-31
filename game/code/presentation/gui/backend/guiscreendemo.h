//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenDemo
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

#ifndef GUISCREENDEMO_H
#define GUISCREENDEMO_H

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
class CGuiScreenDemo : public CGuiScreen
{
public:
    CGuiScreenDemo( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenDemo();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    Scrooby::Text* m_demoText;
    unsigned int m_elapsedTime;

};

#endif // GUISCREENDEMO_H
