//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CollectorCard
//
// Description: Implementation of the CollectorCard class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/06      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <cards/collectorcard.h>

//===========================================================================
// Local Constants
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CollectorCard::CollectorCard
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CollectorCard::CollectorCard
(
    unsigned int ID,
    unsigned int level,
    unsigned int levelID,
    tUID cardName,
    const eQuoteID* quotes,
    int numQuotes
)
:   Card( ID, level, levelID, cardName, quotes, numQuotes )
{
}

//===========================================================================
// CollectorCard::~CollectorCard
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CollectorCard::~CollectorCard()
{
}

//===========================================================================
// Private Member Functions
//===========================================================================

