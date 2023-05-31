//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenBootupLoad
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

#ifndef GUISCREENBOOTUPLOAD_H
#define GUISCREENBOOTUPLOAD_H

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
class CGuiScreenBootupLoad : public CGuiScreen
{
public:
    CGuiScreenBootupLoad( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenBootupLoad();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    Scrooby::Text* m_loadingText;
    unsigned int m_elapsedIdleTime;

};

#endif // GUISCREENBOOTUPLOAD_H
