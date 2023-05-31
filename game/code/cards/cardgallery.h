//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CardGallery
//
// Description: Interface for the CardGallery class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/06      TChu        Created for SRR2
//
//===========================================================================

#ifndef CARDGALLERY_H
#define CARDGALLERY_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <cheats/cheatinputsystem.h>

#include <data/gamedata.h>
#include <p3d/p3dtypes.hpp>

//===========================================================================
// Forward References
//===========================================================================

const unsigned int NUM_LEVELS = 7;
const unsigned int NUM_CARDS_PER_LEVEL = 7;

class CardsDB;
class Card;

struct CardList
{
    Card* m_cards[ NUM_CARDS_PER_LEVEL ];
    int m_numCards;

    CardList();

    void Add( Card* card );
    bool Remove( unsigned int cardID );
    void Empty();
};

//===========================================================================
// Interface Definitions
//===========================================================================

class CardGallery : public GameDataHandler,
                    public ICheatEnteredCallback
{
public:
    // Static Methods for accessing this singleton.
    static CardGallery* CreateInstance();
    static void DestroyInstance();
    static CardGallery* GetInstance();

	CardGallery();
    virtual ~CardGallery();

    // Initialization (involves disc I/O)
    void Init();

    // Adding collected cards (either by UID name or by card ID)
    Card* AddCollectedCardByName( tUID cardName );
    Card* AddCollectedCardByID( unsigned int cardID );

    void AddAllCollectedCards();
    void RemoveAllCollectedCards();

    // Querying collected cards
    const CardList* GetCollectedCards( unsigned int level ) const;
    bool IsCardCollected( tUID cardName ) const;

    // Querying card decks
    //
    int GetNumCardDecksCompleted() const;
    bool IsCardDeckComplete( unsigned int level ) const;

    // Querying cards DB
    CardsDB* GetCardsDB() const { return m_cardsDB; }

    // Implements Game Data Handler
    virtual void LoadData( const GameDataByte* dataBuffer,
                           unsigned int numBytes );
    virtual void SaveData( GameDataByte* dataBuffer,
                           unsigned int numBytes );
    virtual void ResetData();

    virtual void OnCheatEntered( eCheatID cheatID, bool isEnabled );

private:

    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CardGallery( const CardGallery& );
    CardGallery& operator= ( const CardGallery& );

    void AddCollectedCard( Card* newCard );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    // Pointer to the one and only instance of this singleton.
    static CardGallery* spInstance;

    CardsDB* m_cardsDB;

    CardList m_collectedCards[ NUM_LEVELS ];
    int m_numCollectedCards;

};

// A little syntactic sugar for getting at this singleton.
inline CardGallery* GetCardGallery() { return( CardGallery::GetInstance() ); }

inline void CardGallery::ResetData()
{
    this->RemoveAllCollectedCards();
}

#endif // CARDGALLERY_H
