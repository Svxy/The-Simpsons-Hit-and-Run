//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniHud
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

#ifndef GUISCREENMINIHUD_H
#define GUISCREENMINIHUD_H

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
class CGuiScreenMiniHud : public CGuiScreen
{
public:
    CGuiScreenMiniHud( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMiniHud();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    // controller ID of user that paused mini-game
    //
    static int s_pausedControllerID;

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

};

#endif // GUISCREENMINIHUD_H
