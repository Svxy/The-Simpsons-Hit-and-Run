//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CollectorCard
//
// Description: Interface for the CollectorCard class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/06      TChu        Created for SRR2
//
//===========================================================================

#ifndef COLLECTORCARD_H
#define COLLECTORCARD_H

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

class CollectorCard : public Card
{
public:
	CollectorCard( unsigned int ID,
                   unsigned int level,
                   unsigned int levelID,
                   tUID cardName,
                   const eQuoteID* quotes,
                   int numQuotes );

    virtual ~CollectorCard();

    virtual eCardType GetType() const { return COLLECTOR_CARD; }

private:

    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CollectorCard( const CollectorCard& );
    CollectorCard& operator= ( const CollectorCard& );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

};

#endif // COLLECTORCARD_H
