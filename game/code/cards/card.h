//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Card
//
// Description: Interface for the Card class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/06      TChu        Created for SRR2
//
//===========================================================================

#ifndef CARD_H
#define CARD_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/p3dtypes.hpp>

//===========================================================================
// Forward References
//===========================================================================

enum eCardType
{
    COLLECTOR_CARD,
    BONUS_CARD,

    NUM_CARD_TYPES
};

enum eQuoteID
{
    EMPTY_QUOTE = -1,

    QUOTE_ANNOUNCER,
    QUOTE_APU,
    QUOTE_BART,
    QUOTE_BROCKMAN,
    QUOTE_BURNS,
    QUOTE_CARL,
    QUOTE_CHILD,
    QUOTE_DR_WOLFF,
    QUOTE_GIL,
    QUOTE_HOMER,
    QUOTE_JASPER,
    QUOTE_JIMBO,
    QUOTE_KANG,
    QUOTE_KRUSTY,
    QUOTE_LENNY,
    QUOTE_LISA,
    QUOTE_MAGGIE,
    QUOTE_MANJULA,
    QUOTE_MARGE,
    QUOTE_MEYERS,
    QUOTE_MILHOUSE,
    QUOTE_MOTHER,
    QUOTE_MR_SPARKLE,
    QUOTE_OTTO,
    QUOTE_PHOTOGRAPHER,
    QUOTE_RALPH,
    QUOTE_SKINNER,
    QUOTE_SMITHERS,
    QUOTE_STACY,
    QUOTE_WIGGUM,
    QUOTE_WILLIE,

    NUM_QUOTE_IDS
};

const unsigned int MAX_NUM_QUOTES = 3; // max. number of quotes per card

//===========================================================================
// Interface Definitions
//===========================================================================

class Card
{
public:
	Card( unsigned int ID,
          unsigned int level,
          unsigned int levelID,
          tUID cardName,
          const eQuoteID* quotes,
          int numQuotes );

    virtual ~Card();

    // Accessors to card info
    unsigned int GetID() const { return m_ID; }
    unsigned int GetLevel() const { return m_level; }
    unsigned int GetLevelID() const { return m_levelID; }
    tUID GetCardName() const { return m_cardName; }
    eQuoteID GetQuoteID( int index ) const;
    int GetNumQuotes() const { return m_numQuotes; }

    // Pure virtual function to be implemented by derived classes
    virtual eCardType GetType() const = 0;

private:

    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    Card( const Card& );
    Card& operator= ( const Card& );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    unsigned int m_ID;          // ID >= 0 (for card image)
    unsigned int m_level;       // level >= 1
    unsigned int m_levelID;     // levelID >= 1

    tUID m_cardName;            // hashed ID for card name

    eQuoteID m_quotes[ MAX_NUM_QUOTES ];
    int m_numQuotes;

};

#endif // CARD_H
