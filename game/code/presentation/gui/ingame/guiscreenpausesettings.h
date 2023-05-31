//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPauseSettings
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

#ifndef GUISCREENPAUSESETTINGS_H
#define GUISCREENPAUSESETTINGS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

#include <camera/supercam.h>
#include <cheats/cheatinputsystem.h>

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
class CGuiScreenPauseSettings : public CGuiScreen,
                                public ICheatEnteredCallback
{
public:
    CGuiScreenPauseSettings( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenPauseSettings();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }
    virtual void OnCheatEntered( eCheatID cheatID, bool isEnabled );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void UpdateCameraSelections();

    CGuiMenu* m_pMenu;

    enum eCameraSelectionMode
    {
        CAMERA_SELECTION_NOT_AVAILABLE = -1,

        CAMERA_SELECTION_FOR_DRIVING,
        CAMERA_SELECTION_FOR_WALKING,

        NUM_CAMERA_SELECTION_MODES
    };

    eCameraSelectionMode m_currentCameraSelectionMode;

    SuperCam::Type* m_cameraSelections[ NUM_CAMERA_SELECTION_MODES ];
    int m_numCameraSelections[ NUM_CAMERA_SELECTION_MODES ];

};

#endif // GUISCREENPAUSEOPTIONS_H
