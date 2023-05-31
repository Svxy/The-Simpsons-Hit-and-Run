//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLevelEnd
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/10      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENLEVELEND_H
#define GUISCREENLEVELEND_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenlevelstats.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenLevelEnd : public CGuiScreenLevelStats
{
public:
    CGuiScreenLevelEnd( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenLevelEnd();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

};

#endif // GUISCREENLEVELEND_H
