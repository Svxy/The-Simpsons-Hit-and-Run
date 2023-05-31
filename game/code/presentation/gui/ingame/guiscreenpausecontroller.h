//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseController
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

#ifndef GUISCREENPAUSECONTROLLER_H
#define GUISCREENPAUSECONTROLLER_H

//===========================================================================
// Nested Includes
//===========================================================================
#ifdef RAD_WIN32
#include <presentation/gui/frontend/guiscreencontrollerWin32.h>
#else
#include <presentation/gui/frontend/guiscreencontroller.h>
#endif

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPauseController : public CGuiScreenController
{
public:
    CGuiScreenPauseController( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPauseController();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

};

#endif // GUISCREENPAUSECONTROLLER_H
