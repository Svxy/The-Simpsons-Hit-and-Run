//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   BonusCard
//
// Description: Interface for the BonusCard class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/06      TChu        Created for SRR2
//
//===========================================================================

#ifndef BONUSCARD_H
#define BONUSCARD_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <cards/card.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

class BonusCard : public Card
{
public:
	BonusCard( unsigned int ID,
               unsigned int level,
               unsigned int levelID,
               tUID cardName,
               const eQuoteID* quotes,
               int numQuotes );

    virtual ~BonusCard();

    virtual eCardType GetType() const { return BONUS_CARD; }

private:

    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    BonusCard( const BonusCard& );
    BonusCard& operator= ( const BonusCard& );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

};

#endif // BONUSCARD_H
