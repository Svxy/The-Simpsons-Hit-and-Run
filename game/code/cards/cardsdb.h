//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CardsDB
//
// Description: Interface for the CardsDB class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/06      TChu        Created for SRR2
//
//===========================================================================

#ifndef CARDSDB_H
#define CARDSDB_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/p3dtypes.hpp>

//===========================================================================
// Forward References
//===========================================================================

const unsigned int MAX_NUM_CARDS = 64;

class Card;

//===========================================================================
// Interface Definitions
//===========================================================================

class CardsDB
{
public:
	CardsDB();
    virtual ~CardsDB();

    // Load cards from config file into DB
    void LoadCards();

    // Accessing cards DB
    Card* GetCardByID( unsigned int cardID );
    Card* GetCardByName( tUID cardName );

    int GetNumCards() const { return m_numCards; }

private:

    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CardsDB( const CardsDB& );
    CardsDB& operator= ( const CardsDB& );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    Card** m_cards;
    int m_numCards;

};

#endif // CARDSDB_H
