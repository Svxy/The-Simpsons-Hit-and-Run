//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionSuccess
//
// Description: 
//              
//
// Authors:     Ian Gipson
//
// Revisions		Date			Author	    Revision
//                  2003/04/07      igipson     Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMISSIONSUCCESS_H
#define GUISCREENMISSIONSUCCESS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenmissionload.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMissionSuccess : 
    public CGuiScreenMissionBase
{
public:
    CGuiScreenMissionSuccess( Scrooby::Screen* pScreen, CGuiEntity* pParent );
	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    virtual void InitIntro();
protected:
    virtual void OutroDone();
private:
    Scrooby::Text* m_PattyAndSelmaTitle;
    Scrooby::Text* m_PattyAndSelmaInfo;
};

#endif // GUISCREENMISSIONSUCCESS_H
