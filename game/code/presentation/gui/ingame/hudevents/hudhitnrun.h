//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudHitNRun
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

#ifndef HUDHITNRUN_H
#define HUDHITNRUN_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>

#include <p3d/p3dtypes.hpp>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Page;
    class Text;
    class Sprite;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class HudHitNRun : public HudEventHandler
{
public:
    HudHitNRun( Scrooby::Page* pPage );
    virtual ~HudHitNRun();

    virtual void Start();
    virtual void Stop();
    virtual void Update( float elapsedTime );

    enum eMessage
    {
        MSG_HIT_N_RUN,
        MSG_BUSTED,

        NUM_MESSAGES
    };

    void SetMessage( eMessage message );

private:
    enum eSubState
    {
        STATE_MESSAGE_TRANSITION_IN,
        STATE_MESSAGE_DISPLAY_HOLD,
        STATE_MESSAGE_TRANSITION_OUT,

        NUM_SUB_STATES
    };

    unsigned int m_currentSubState;

    eMessage m_currentMessage;
    Scrooby::Text* m_hnrMessage;
    Scrooby::Sprite* m_hnrTicket;

    int m_messageTranslateX;
    int m_messageTranslateY;

    tColour m_defaultMessageColour;

};

#endif // HUDHITNRUN_H
