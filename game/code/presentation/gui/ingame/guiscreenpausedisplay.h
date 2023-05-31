/******************************************************************************

    File:        CGuiScreenPauseDisplay.h
    Desc:        Interface for the CGuiScreenController class.

    Date:        August 14, 2003
    Authors:     Tony Chu, Neil Haran
    History:     

*****************************************************************************/

#ifndef GUISCREENPAUSEDISPLAY_H
#define GUISCREENPAUSEDISPLAY_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreendisplay.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPauseDisplay : public CGuiScreenDisplay
{
public:
    CGuiScreenPauseDisplay( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPauseDisplay();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

};

#endif // GUISCREENPAUSEDISPLAY_H
