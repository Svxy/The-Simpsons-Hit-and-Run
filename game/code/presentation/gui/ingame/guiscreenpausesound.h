//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseSound
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

#ifndef GUISCREENPAUSESOUND_H
#define GUISCREENPAUSESOUND_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreensound.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPauseSound : public CGuiScreenSound
{
public:
    CGuiScreenPauseSound( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPauseSound();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

};

#endif // GUISCREENPAUSESOUND_H
