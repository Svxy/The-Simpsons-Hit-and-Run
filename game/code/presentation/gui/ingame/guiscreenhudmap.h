//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenHudMap
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

#ifndef GUISCREENHUDMAP_H
#define GUISCREENHUDMAP_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================
class CHudMap;
class tPointCamera;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenHudMap : public CGuiScreen
{
public:
    CGuiScreenHudMap( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenHudMap();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    CHudMap* m_largeHudMap;
    int m_posX;
    int m_posY;
    int m_width;
    int m_height;

    tPointCamera* m_camera;

};

#endif // GUISCREENHUDMAP_H
