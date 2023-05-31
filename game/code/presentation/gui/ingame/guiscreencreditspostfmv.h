//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenCreditsPostFMV
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/06/02      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENCREDITSPOSTFMV_H
#define GUISCREENCREDITSPOSTFMV_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenviewcredits.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenCreditsPostFMV : public CGuiScreenViewCredits
{
public:
    CGuiScreenCreditsPostFMV( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenCreditsPostFMV();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    virtual void OnScrollingDone();

    Scrooby::Sprite* m_kang;
    Scrooby::Sprite* m_kodos;

};

#endif // GUISCREENCREDITSPOSTFMV_H
