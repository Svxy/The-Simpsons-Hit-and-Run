//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudCardCollected
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

#ifndef HUDCARDCOLLECTED_H
#define HUDCARDCOLLECTED_H

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

class HudCardCollected : public HudEventHandler
{
public:
    HudCardCollected( Scrooby::Page* pPage );
    virtual ~HudCardCollected();

    virtual void Start();
    virtual void Stop();
    virtual void Update( float elapsedTime );

    void SetCurrentCard( unsigned int cardID );

private:
    void SetCardCount( unsigned int numCollected, unsigned int numCollectibles );

    static const unsigned int BITMAP_TEXT_BUFFER_SIZE = 8;

    enum eSubState
    {
        STATE_CARD_TRANSITION_IN,
        STATE_CARD_DISPLAY_HOLD,
        STATE_CARD_TRANSITION_OUT,

        // extra state when last card per level is collected
        //
        STATE_CARD_DECK_COMPLETE_DISPLAY_HOLD,
        STATE_CARD_DECK_COMPLETE_TRANSITION_OUT,

        NUM_SUB_STATES
    };

    unsigned int m_currentSubState;

    Scrooby::Sprite* m_cardImage;

    Scrooby::Group* m_cardText;
    Scrooby::Text* m_cardGet;
    Scrooby::Text* m_cardTitle;

    Scrooby::Group* m_itemsCount;
    Scrooby::Sprite* m_numCards;

    Scrooby::Group* m_itemsComplete;

    enum eCardDeckCompleteMessage
    {
        LEVEL_CARDS_COMPLETE,
        CARD_DECK_COMPLETE
    };

    Scrooby::Text* m_cardDeckComplete;
    bool m_isCardDeckComplete : 1;

    enum eItemUnlockedMessage
    {
        UNLOCKED_MINI_GAME_TRACK,
        UNLOCKED_IS_MOVIE
    };

    Scrooby::Text* m_itemsUnlocked;

};

#endif // HUDCARDCOLLECTED_H
