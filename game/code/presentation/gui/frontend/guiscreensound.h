//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSound
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

#ifndef GUISCREENSOUND_H
#define GUISCREENSOUND_H

#if defined( RAD_GAMECUBE ) || defined( RAD_PS2 )
    #define INCLUDE_SOUND_MODE
#endif

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

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
class CGuiScreenSound : public CGuiScreen
{
public:
    CGuiScreenSound( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenSound();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    virtual CGuiMenu* HasMenu() { return m_pMenu; }

#ifdef RAD_WIN32
    //virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y );
#endif

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    enum eMenuItem
    {
        MENU_ITEM_MUSIC,
        MENU_ITEM_EFFECTS,
        MENU_ITEM_ENGINE,
        MENU_ITEM_VOICE,

#ifdef INCLUDE_SOUND_MODE
        MENU_ITEM_SURROUND_SOUND,
#endif

        NUM_MENU_ITEMS
    };

#ifdef INCLUDE_SOUND_MODE
    enum eMenuItemSurroundSound
    {
        MONO_SOUND,
        STEREO_SOUND,
        SURROUND_SOUND,

        NUM_SOUND_SETTINGS
    };
#endif

    CGuiMenu* m_pMenu;

    Scrooby::Sprite* m_soundOffIcons[ NUM_MENU_ITEMS ];
    bool m_hasSliderValueChanged[ NUM_MENU_ITEMS ];

};

#endif // GUISCREENSOUND_H
