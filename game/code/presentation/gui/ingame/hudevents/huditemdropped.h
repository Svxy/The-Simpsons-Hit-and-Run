//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudItemDropped
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

#ifndef HUDITEMDROPPED_H
#define HUDITEMDROPPED_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Page;
    class Text;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class HudItemDropped : public HudEventHandler
{
public:
    HudItemDropped( Scrooby::Page* pPage );
    virtual ~HudItemDropped();

    virtual void Start();
    virtual void Stop();
    virtual void Update( float elapsedTime );

private:
    enum eSubState
    {
        STATE_TRANSITION_IN,
        STATE_DISPLAY_HOLD,
        STATE_TRANSITION_OUT,

        NUM_SUB_STATES
    };

    unsigned int m_currentSubState;

    Scrooby::Text* m_itemDropped;

};

#endif // HUDITEMDROPPED_H
