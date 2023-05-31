//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudCoinCollected
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

#ifndef HUDCOINCOLLECTED_H
#define HUDCOINCOLLECTED_H

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
    class Group;
    class Sprite;
    class Text;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class HudCoinCollected : public HudEventHandler
{
public:
    HudCoinCollected( Scrooby::Page* pPage );
    virtual ~HudCoinCollected();

    virtual void Start();
    virtual void Stop();
    virtual void Update( float elapsedTime );

    void SetCurrentItemCount( int itemCount );

private:
    void SetItemCount( unsigned int numCollected, unsigned int numCollectibles );

    static const unsigned int BITMAP_TEXT_BUFFER_SIZE = 8;

    enum eSubState
    {
        STATE_COIN_TRANSITION_IN,
        STATE_COIN_DISPLAY_HOLD,
        STATE_COIN_TRANSITION_OUT,

        NUM_SUB_STATES
    };

    unsigned int m_currentSubState;

    Scrooby::Sprite* m_cardImage;
    Scrooby::Group* m_cardText;

    Scrooby::Group* m_itemsCount;
    Scrooby::Sprite* m_numCoins;
    int m_currentItemCount;

    Scrooby::Group* m_itemsComplete;
    bool m_isFullCount : 1;

    float m_elapsedCoinDecrementTime;

};

inline void HudCoinCollected::SetCurrentItemCount( int itemCount )
{
    m_currentItemCount = itemCount;
}

#endif // HUDCOINCOLLECTED_H
