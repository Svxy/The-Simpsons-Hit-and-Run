//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenController
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/30      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENCONTROLLER_H
#define GUISCREENCONTROLLER_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenController : public CGuiScreen
{
public:
    CGuiScreenController( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenController();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void UpdateLabels();

    static const int MAX_NUM_LABELS = 32;

    CGuiMenu* m_pMenu;

    Scrooby::Text* m_labels[ MAX_NUM_LABELS ];
    int m_numLabels;

    int m_currentConfig;
    int m_currentDisplay;
    int m_currentControllerID;

};

#endif // GUISCREENCONTROLLER_H
