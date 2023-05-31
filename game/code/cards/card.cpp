//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Card
//
// Description: Implementation of the Card class.
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
#include <string.h>
#include <cards/card.h>

//===========================================================================
// Local Constants
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// Card::Card
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
Card::Card
(
    unsigned int ID,
    unsigned int level,
    unsigned int levelID,
    tUID cardName,
    const eQuoteID* quotes,
    int numQuotes
)
:   m_ID( ID ),
    m_level( level ),
    m_levelID( levelID ),
    m_cardName( cardName ),
    m_numQuotes( 0 )
{
    rAssert( quotes != NULL );

    for( int i = 0; i < numQuotes; i++ )
    {
        rAssert( quotes[ i ] < NUM_QUOTE_IDS );

        m_quotes[ i ] = quotes[ i ];

        if( m_quotes[ i ] != EMPTY_QUOTE )
        {
            m_numQuotes++;
        }
    }
}

//===========================================================================
// Card::~Card
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
Card::~Card()
{
}

eQuoteID
Card::GetQuoteID( int index ) const
{
    rAssert( static_cast<unsigned int>( index ) < MAX_NUM_QUOTES );

    return m_quotes[ index ];
}

//===========================================================================
// Private Member Functions
//===========================================================================

