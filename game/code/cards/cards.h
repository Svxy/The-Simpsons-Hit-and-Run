//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Cards
//
// Description: Collector Cards Database
//
// Authors:     Tony Chu (and Simpsons2 Designers)
//
// Revisions		Date			Author	    Revision
//                  2002/08/14      TChu        Created for SRR2
//
//===========================================================================

#ifndef CARDS_H
#define CARDS_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <cards/card.h>

struct CardData
{
    char name[ 32 ];
    unsigned short level;
    unsigned short levelID;
    unsigned short imageID;
    eCardType type;
    eQuoteID quotes[ MAX_NUM_QUOTES ];
};

//===========================================================================
// Cards Database
//===========================================================================

static const CardData s_cardsDB[] = 
{
    // SYNTAX: { <Card Name>, <Card Level>, <Card Level ID>, <Card Image ID>, <Card Type> },
    //
    //  - <Card Name> is the card name (max. 31 characters)
    //  - <Card Level> is the card level (1-7)
    //  - <Card Level ID> is the card level ID
    //  - <Card Image ID> is the ID reference for the associated Scrooby image
    //  - <Card Type> is the card type (either COLLECTOR_CARD or BONUS_CARD)
    //

/// Level 1
//
    { "card11", 1, 1, 0, COLLECTOR_CARD, { QUOTE_LENNY, QUOTE_CARL, EMPTY_QUOTE } },
    { "card12", 1, 2, 1, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card13", 1, 3, 2, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card14", 1, 4, 3, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card15", 1, 5, 4, COLLECTOR_CARD, { QUOTE_HOMER, QUOTE_HOMER, EMPTY_QUOTE } },
    { "card16", 1, 6, 5, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card17", 1, 7, 6, COLLECTOR_CARD, { QUOTE_MR_SPARKLE, EMPTY_QUOTE, EMPTY_QUOTE } },
//
/// Level 2
//
    { "card21", 2, 1, 8, COLLECTOR_CARD, { QUOTE_JIMBO, QUOTE_HOMER, EMPTY_QUOTE } },
    { "card22", 2, 2, 9, COLLECTOR_CARD, { QUOTE_WIGGUM, QUOTE_JASPER, EMPTY_QUOTE } },
    { "card23", 2, 3, 10, COLLECTOR_CARD, { QUOTE_BART, QUOTE_HOMER, EMPTY_QUOTE } },
    { "card24", 2, 4, 11, COLLECTOR_CARD, { QUOTE_SKINNER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card25", 2, 5, 12, COLLECTOR_CARD, { QUOTE_LISA, QUOTE_HOMER, EMPTY_QUOTE } },
    { "card26", 2, 6, 13, COLLECTOR_CARD, { QUOTE_MILHOUSE, QUOTE_BART, EMPTY_QUOTE } },
    { "card27", 2, 7, 14, COLLECTOR_CARD, { QUOTE_BART, EMPTY_QUOTE, EMPTY_QUOTE } },
//
/// Level 3
//
    { "card31", 3, 1, 16, COLLECTOR_CARD, { QUOTE_LISA, QUOTE_BROCKMAN, EMPTY_QUOTE } },
    { "card32", 3, 2, 17, COLLECTOR_CARD, { QUOTE_BART, QUOTE_HOMER, EMPTY_QUOTE } },
    { "card33", 3, 3, 18, COLLECTOR_CARD, { QUOTE_LISA, QUOTE_STACY, EMPTY_QUOTE } },
    { "card34", 3, 4, 19, COLLECTOR_CARD, { QUOTE_RALPH, QUOTE_LISA, EMPTY_QUOTE } },
    { "card35", 3, 5, 20, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card36", 3, 6, 21, COLLECTOR_CARD, { QUOTE_DR_WOLFF, QUOTE_LISA, EMPTY_QUOTE } },
    { "card37", 3, 7, 22, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
//
/// Level 4
//
    { "card41", 4, 1, 24, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card42", 4, 2, 25, COLLECTOR_CARD, { QUOTE_BURNS, QUOTE_MARGE, EMPTY_QUOTE } },
    { "card43", 4, 3, 26, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card44", 4, 4, 27, COLLECTOR_CARD, { QUOTE_HOMER, QUOTE_MARGE, EMPTY_QUOTE } },
    { "card45", 4, 5, 28, COLLECTOR_CARD, { QUOTE_GIL, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card46", 4, 6, 29, COLLECTOR_CARD, { QUOTE_HOMER, QUOTE_PHOTOGRAPHER, EMPTY_QUOTE } },
    { "card47", 4, 7, 30, COLLECTOR_CARD, { QUOTE_MARGE, QUOTE_HOMER, EMPTY_QUOTE } },
//
/// Level 5
//
    { "card51", 5, 1, 32, COLLECTOR_CARD, { QUOTE_LISA, QUOTE_HOMER, EMPTY_QUOTE } },
    { "card52", 5, 2, 33, COLLECTOR_CARD, { QUOTE_BROCKMAN, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card53", 5, 3, 34, COLLECTOR_CARD, { QUOTE_APU, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card54", 5, 4, 35, COLLECTOR_CARD, { QUOTE_MANJULA, QUOTE_HOMER, EMPTY_QUOTE } },
    { "card55", 5, 5, 36, COLLECTOR_CARD, { QUOTE_APU, QUOTE_MANJULA, EMPTY_QUOTE } },
    { "card56", 5, 6, 37, COLLECTOR_CARD, { QUOTE_BART, QUOTE_OTTO, EMPTY_QUOTE } },
    { "card57", 5, 7, 38, COLLECTOR_CARD, { QUOTE_APU, QUOTE_HOMER, EMPTY_QUOTE } },
//
/// Level 6
//
    { "card61", 6, 1, 40, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card62", 6, 2, 41, COLLECTOR_CARD, { QUOTE_CHILD, QUOTE_MOTHER, EMPTY_QUOTE } },
    { "card63", 6, 3, 42, COLLECTOR_CARD, { QUOTE_KRUSTY, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card64", 6, 4, 43, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card65", 6, 5, 44, COLLECTOR_CARD, { QUOTE_MEYERS, QUOTE_ANNOUNCER, EMPTY_QUOTE } },
    { "card66", 6, 6, 45, COLLECTOR_CARD, { QUOTE_KRUSTY, QUOTE_ANNOUNCER, EMPTY_QUOTE } },
    { "card67", 6, 7, 46, COLLECTOR_CARD, { QUOTE_BART, EMPTY_QUOTE, EMPTY_QUOTE } },
//
/// Level 7
//
    { "card71", 7, 1, 48, COLLECTOR_CARD, { QUOTE_BURNS, QUOTE_SMITHERS, EMPTY_QUOTE } },
    { "card72", 7, 2, 49, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card73", 7, 3, 50, COLLECTOR_CARD, { QUOTE_MARGE, QUOTE_KANG, EMPTY_QUOTE } },
    { "card74", 7, 4, 51, COLLECTOR_CARD, { QUOTE_WILLIE, QUOTE_MAGGIE, EMPTY_QUOTE } },
    { "card75", 7, 5, 52, COLLECTOR_CARD, { QUOTE_LISA, QUOTE_MARGE, EMPTY_QUOTE } },
    { "card76", 7, 6, 53, COLLECTOR_CARD, { QUOTE_HOMER, EMPTY_QUOTE, EMPTY_QUOTE } },
    { "card77", 7, 7, 54, COLLECTOR_CARD, { QUOTE_MARGE, QUOTE_HOMER, EMPTY_QUOTE } },
//
};

static const unsigned int s_numCardsInDB = sizeof( s_cardsDB ) / sizeof( s_cardsDB[ 0 ] );


#endif // CARDS_H
