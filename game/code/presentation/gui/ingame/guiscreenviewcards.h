//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenViewCards
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

#ifndef GUISCREENVIEWCARDS_H
#define GUISCREENVIEWCARDS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreencardgallery.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenViewCards : public CGuiScreenCardGallery
{
public:
    CGuiScreenViewCards( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenViewCards();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    Scrooby::Layer* m_pauseFgdLayer;
    Scrooby::Layer* m_bigBoardFgdLayer;

};

#endif // GUISCREENVIEWCARDS_H
