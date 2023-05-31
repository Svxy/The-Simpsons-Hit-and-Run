//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenTutorial
//
// Description: 
//              
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/04/15      TChu        Created
//
//===========================================================================

#ifndef GUISCREENTUTORIAL_H
#define GUISCREENTUTORIAL_H

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
class CGuiScreenTutorial : public CGuiScreen
{
public:
    CGuiScreenTutorial( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenTutorial();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    void SetTutorialMessage( int index );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    static const int MAX_TUTORIAL_MESSAGE_LENGTH = 256;
    Scrooby::Sprite* m_tutorialMessage;
    Scrooby::Sprite* m_bartsHead;
    Scrooby::Group* m_disableTutorial;

    static const unsigned int MAX_TUTORIAL_DIALOG_TIME = 10000; // in msec
    unsigned int m_elapsedDialogTime;

};


#endif // GUISCREENTUTORIAL_H
