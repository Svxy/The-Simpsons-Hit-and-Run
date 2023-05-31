//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudMissionObjective
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

#ifndef HUDMISSIONOBJECTIVE_H
#define HUDMISSIONOBJECTIVE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>
#include <presentation/gui/utility/transitions.h>

//===========================================================================
// Forward References
//===========================================================================

#ifdef RAD_WIN32 // temporary.. for art testing.
const float MISSION_ICON_SCALE = 0.78f;
#else
const float MISSION_ICON_SCALE = 1.5f;
#endif

namespace Scrooby
{
    class Page;
    class Sprite;
}

class tSprite;

//===========================================================================
// Interface Definitions
//===========================================================================

class HudMissionObjective : public HudEventHandler
{
public:
    HudMissionObjective( Scrooby::Page* pPage );
    virtual ~HudMissionObjective();

    virtual void Start();
    virtual void Stop();
    virtual void Update( float elapsedTime );

    void SetMessageID( unsigned int messageID );
    bool UpdateIcon();

private:
    enum eSubState
    {
        STATE_ICON_POP_UP,
        STATE_ICON_DISPLAY_HOLD,
        STATE_ICON_SLIDE_UP,
        STATE_IDLE,

        NUM_SUB_STATES
    };

    unsigned int m_currentSubState;

    Scrooby::Sprite* m_missionIcon;
    tSprite* m_missionIconImage;
    unsigned int m_messageID;

    GuiSFX::UnderdampedTranslator m_iconTranslator;

};

inline void
HudMissionObjective::SetMessageID( unsigned int messageID )
{
    m_messageID = messageID;
}

#endif // HUDMISSIONOBJECTIVE_H
