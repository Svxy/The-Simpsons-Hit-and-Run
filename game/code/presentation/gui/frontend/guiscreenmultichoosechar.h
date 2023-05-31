//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMultiChooseChar
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

#ifndef GUISCREENMULTICHOOSECHAR_H
#define GUISCREENMULTICHOOSECHAR_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

#include <constants/maxplayers.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

namespace Scrooby
{
    class Screen;
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMultiChooseChar : public CGuiScreen
{
public:
    CGuiScreenMultiChooseChar( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMultiChooseChar();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    void HandleControllerMessage( int player, eGuiMessage message );

private:
    CGuiMenu* m_pMenu[ MAX_PLAYERS ];
    Scrooby::Sprite* m_pCharacter[ MAX_PLAYERS ];

};

#endif // GUISCREENMULTICHOOSECHAR_H
