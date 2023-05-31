//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseSunday
//
// Description: 
//              
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/11/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUISCREENPAUSESUNDAY_H
#define GUISCREENPAUSESUNDAY_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenpause.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPauseSunday : public CGuiScreenPause
{
public:
    CGuiScreenPauseSunday( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPauseSunday();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

};

#endif // GUISCREENPAUSESUNDAY_H
