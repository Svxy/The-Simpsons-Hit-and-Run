//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CardGallery
//
// Description: Implementation of the CardGallery class.
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
#include <cards/cardgallery.h>
#include <cards/cardsdb.h>
#include <cards/card.h>

#include <data/gamedatamanager.h>
#include <memory/srrmemory.h>

#include <raddebugwatch.hpp>

// Static pointer to instance of singleton.
CardGallery* CardGallery::spInstance = NULL;

#ifdef DEBUGWATCH
    static const char* WATCHER_NAMESPACE = "Card Gallery";
    static unsigned int s_wAddCollectedCard;
    static void AddCardToGallery()
    {
        GetCardGallery()->AddCollectedCardByID( s_wAddCollectedCard );
    }

    static void AddAllCardsToGallery()
    {
        GetCardGallery()->AddAllCollectedCards();
    }

    static void RemoveAllCardsFromGallery()
    {
        GetCardGallery()->RemoveAllCollectedCards();
    }
#endif

//===========================================================================
// Local Constants
//===========================================================================

const unsigned int NUM_BITS_PER_BYTE = 8;

//===========================================================================
// Public Member Functions
//===========================================================================

CardList::CardList()
:   m_numCards( 0 )
{
    memset( m_cards, 0, sizeof( m_cards ) );
}

void
CardList::Add( Card* card )
{
    rAssert( card != NULL );
    rAssert( m_numCards < static_cast<int>( NUM_CARDS_PER_LEVEL ) );

    unsigned int slot = card->GetLevelID() - 1;
    rAssert( slot < NUM_CARDS_PER_LEVEL );

    if( m_cards[ slot ] == NULL )
    {
        m_cards[ slot ] = card;
        m_numCards++;
    }
    else
    {
        rAssertMsg( 0, "WARNING: *** Card already collected!" );
    }
}

bool
CardList::Remove( unsigned int cardID )
{
    bool cardFound = false;

    // find card with cardID
    //
    for( int i = 0; i < m_numCards; i++ )
    {
        rAssert( m_cards[ i ] );

        if( m_cards[ i ]->GetID() == cardID )
        {
            // found it! now remove it
            m_cards[ i ] = NULL;
            m_numCards--;

            cardFound = true;
            break;
        }
    }

    return cardFound;
}

void
CardList::Empty()
{
    memset( m_cards, 0, sizeof( m_cards ) );
    m_numCards = 0;
}

//==============================================================================
// CardGallery::CreateInstance
//==============================================================================
//
// Description: - Creates the Card Gallery.
//
// Parameters:	None.
//
// Return:      Pointer to the CardGallery.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CardGallery* CardGallery::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "CardGallery" );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif
        spInstance = new(GMA_PERSISTENT) CardGallery;
    rAssert( spInstance != NULL );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
 MEMTRACK_POP_GROUP( "CardGallery" );
    return spInstance;
}

//==============================================================================
// CardGallery::DestroyInstance
//==============================================================================
//
// Description: Destroy the GUI system.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void CardGallery::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}

//==============================================================================
// CardGallery::GetInstance
//==============================================================================
//
// Description: - Access point for the CardGallery singleton.  
//              - Creates the CardGallery if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the CardGallery.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CardGallery* CardGallery::GetInstance()
{
    rAssert( spInstance != NULL );

    return spInstance;
}

//===========================================================================
// CardGallery::CardGallery
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
CardGallery::CardGallery()
:   m_cardsDB( NULL ),
    m_numCollectedCards( 0 )
{
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_PERSISTENT );
#endif

    m_cardsDB = new CardsDB();
    rAssert( m_cardsDB );

#ifdef RAD_GAMECUBE
    HeapMgr()->PopHeap( GMA_GC_VMM );
#else
    HeapMgr()->PopHeap( GMA_PERSISTENT );
#endif
}

//===========================================================================
// CardGallery::~CardGallery
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
CardGallery::~CardGallery()
{
    GetCheatInputSystem()->UnregisterCallback( this );

    if( m_cardsDB != NULL )
    {
        delete m_cardsDB;
        m_cardsDB = NULL;
    }
}

//===========================================================================
// CardGallery::Init
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
void
CardGallery::Init()
{
    // load cards into database
    //
    rAssert( m_cardsDB );
    m_cardsDB->LoadCards();

    // determine number of bytes needed for loading/saving
    //
    unsigned int numDataBytes = m_cardsDB->GetNumCards() / NUM_BITS_PER_BYTE;
    if( m_cardsDB->GetNumCards() % NUM_BITS_PER_BYTE > 0 )
    {
        numDataBytes++; // round up number of bytes
    }

    // register collected cards data for loading/saving
    //
    GetGameDataManager()->RegisterGameData( this, numDataBytes, "Card Gallery" );

    // register callback for "Collect All Cards" cheat
    //
    GetCheatInputSystem()->RegisterCallback( this );

#ifdef DEBUGWATCH
    radDbgWatchAddUnsignedInt( &s_wAddCollectedCard,
                               "Add Collectible Card",
                               WATCHER_NAMESPACE,
                               (RADDEBUGWATCH_CALLBACK)AddCardToGallery,
                               NULL,
                               0,
                               MAX_NUM_CARDS - 1 );

    radDbgWatchAddFunction( "Add All Collectible Cards",
                            (RADDEBUGWATCH_CALLBACK)AddAllCardsToGallery,
                            NULL,
                            WATCHER_NAMESPACE );

    radDbgWatchAddFunction( "Remove All Collectible Cards",
                            (RADDEBUGWATCH_CALLBACK)RemoveAllCardsFromGallery,
                            NULL,
                            WATCHER_NAMESPACE );
#endif
}

//===========================================================================
// CardGallery::AddCollectedCardByID
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
Card*
CardGallery::AddCollectedCardByID( unsigned int cardID )
{
    rAssert( m_cardsDB );

    // get card by ID from DB, and add it to collected cards
    //
    Card* collectedCard = m_cardsDB->GetCardByID( cardID );
    if( collectedCard != NULL )
    {
        this->AddCollectedCard( collectedCard );
    }

    return collectedCard;
}

//===========================================================================
// CardGallery::AddCollectedCardByName
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
Card*
CardGallery::AddCollectedCardByName( tUID cardName )
{
    rAssert( m_cardsDB );

    // get card by name from DB, and add it to collected cards
    //
    Card* collectedCard = m_cardsDB->GetCardByName( cardName );
    if( collectedCard != NULL )
    {
        this->AddCollectedCard( collectedCard );
    }
    else
    {
        rAssertMsg( false, "ERROR: *** Card not found!" );
    }

    return collectedCard;
}

//===========================================================================
// CardGallery::AddAllCollectedCards
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
void
CardGallery::AddAllCollectedCards()
{
    for( unsigned int i = 0; i < MAX_NUM_CARDS; i++ )
    {
        rAssert( m_cardsDB != NULL );
        Card* card = m_cardsDB->GetCardByID( i );
        if( card != NULL )
        {
            this->AddCollectedCard( card );
        }
    }
}

//===========================================================================
// CardGallery::RemoveAllCollectedCards
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
void
CardGallery::RemoveAllCollectedCards()
{
    for( unsigned int i = 0; i < NUM_LEVELS; i++ )
    {
        m_collectedCards[ i ].Empty();
    }

    m_numCollectedCards = 0;
}

//===========================================================================
// CardGallery::GetCollectedCards
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
const CardList*
CardGallery::GetCollectedCards( unsigned int level ) const
{
    rAssert( level < NUM_LEVELS );
    return( &(m_collectedCards[ level ]) );
}

//=============================================================================
// CardGallery::IsCardCollected
//=============================================================================
// Description: Comment
//
// Parameters:  ( tUID cardName )
//
// Return:      bool 
//
//=============================================================================
bool CardGallery::IsCardCollected( tUID cardName ) const
{
    bool cardCollected = false;

    rAssert( m_cardsDB );
    Card* card = m_cardsDB->GetCardByName( cardName );
    if( card != NULL )
    {
        int level = card->GetLevel() - 1;
        int levelID = card->GetLevelID() - 1;
        if( m_collectedCards[ level ].m_cards[ levelID ] != NULL )
        {
            cardCollected = true;
        }
    }

    return cardCollected;
}

//=============================================================================
// CardGallery::GetNumCardDecksCompleted
//=============================================================================
// Description: Get number of card decks completed.  Collecting all cards in
//              a given level constitutes a complete deck.
//
// Parameters:  none
//
// Return:      number of card decks completed
//
//=============================================================================
int CardGallery::GetNumCardDecksCompleted() const
{
    int numDecksCompleted = 0;

    for( unsigned int i = 0; i < NUM_LEVELS; i++ )
    {
        if( this->IsCardDeckComplete( i ) )
        {
            numDecksCompleted++;
        }
    }

    return numDecksCompleted;
}

//=============================================================================
// CardGallery::IsCardDeckComplete
//=============================================================================
// Description: Query whether or not a card deck is complete for a given level.
//
// Parameters:  level index
//
// Return:      true/false
//
//=============================================================================
bool CardGallery::IsCardDeckComplete( unsigned int level ) const
{
    rAssert( level < NUM_LEVELS );

    return( m_collectedCards[ level ].m_numCards == static_cast<int>( NUM_CARDS_PER_LEVEL ) );
}

//===========================================================================
// CardGallery::LoadData
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
void
CardGallery::LoadData( const GameDataByte* dataBuffer,
                       unsigned int numBytes )
{
    // remove all collected cards first
    //
    this->RemoveAllCollectedCards();

    // add new set of collected cards from loaded data
    //
    for( unsigned int i = 0; i < numBytes * NUM_BITS_PER_BYTE; i++ )
    {
        unsigned int dataBufferIndex = i / NUM_BITS_PER_BYTE;
        if( (dataBuffer[ dataBufferIndex ] & (1 << i % NUM_BITS_PER_BYTE)) > 0 )
        {
            this->AddCollectedCardByID( i );
        }
    }
}

//===========================================================================
// CardGallery::SaveData
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
void
CardGallery::SaveData( GameDataByte* dataBuffer,
                       unsigned int numBytes )
{
    if( CommandLineOptions::Get( CLO_MEMCARD_CHEAT ) )
    {
        // turn on all bits, and save
        //
        memset( dataBuffer, ~0, numBytes );
    }
    else
    {
        // clear data buffer first
        //
        for( unsigned int byteIndex = 0; byteIndex < numBytes; byteIndex++ )
        {
            dataBuffer[ byteIndex ] = 0;
        }

        // save collected cards using bit-masking algorithm
        //
        for( unsigned int i = 0; i < NUM_LEVELS; i++ )
        {
            for( unsigned int j = 0; j < NUM_CARDS_PER_LEVEL; j++ )
            {
                if( m_collectedCards[ i ].m_cards[ j ] != NULL )
                {
                    unsigned int cardID = m_collectedCards[ i ].m_cards[ j ]->GetID();

                    unsigned int dataBufferIndex = cardID / NUM_BITS_PER_BYTE;
                    dataBuffer[ dataBufferIndex ] |= (1 << (cardID % NUM_BITS_PER_BYTE));
                }
            }
        }
    }
}

//===========================================================================
// CardGallery::OnCheatEntered
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
void
CardGallery::OnCheatEntered( eCheatID cheatID, bool isEnabled )
{
    if( cheatID == CHEAT_ID_UNLOCK_CARDS )
    {
        // remove all cards from gallery first
        //
        this->RemoveAllCollectedCards();

        // now add all collectible cards to gallery
        //
        this->AddAllCollectedCards();
    }
}

//===========================================================================
// Private Member Functions
//===========================================================================

//===========================================================================
// CardGallery::AddCollectedCard
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
void
CardGallery::AddCollectedCard( Card* newCard )
{
    rAssert( newCard != NULL );
    unsigned int level = newCard->GetLevel() - 1;
    rAssert( level < NUM_LEVELS );
    m_collectedCards[ level ].Add( newCard );

    rDebugPrintf( "Card Gallery: Collected Card %d-%d\n",
                    newCard->GetLevel(), newCard->GetLevelID() );

    m_numCollectedCards++;
}

