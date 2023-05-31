//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CheatsDB
//
// Description: Implementation of the CheatsDB class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/19      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <cheats/cheats.h>
#include <cheats/cheatinputhandler.h>

#include <memory/srrmemory.h>

#include <string.h>

//===========================================================================
// Local Constants
//===========================================================================

/* --------- Platform-Specific Button Mappings for Cheat Inputs ----------

[PLATFORM] [CHEAT_INPUT_0] [CHEAT_INPUT_1] [CHEAT_INPUT_2] [CHEAT_INPUT_3]

GameCube    A               B               X               Y 
PS2         X               Circle          Square          Triangle
Xbox        A               B               X               Y

 * ----------------------------------------------------------------------- */

static const Cheat REGISTERED_CHEATS[] = 
{
    // *** cheat name must be < 32 characters in length *** //
    //
    {
        CHEAT_ID_UNLOCK_VEHICLES,
        { CHEAT_INPUT_0, CHEAT_INPUT_1, CHEAT_INPUT_0, CHEAT_INPUT_1 },
        "Unlock All Reward Vehicles"
    },
    {
        CHEAT_ID_NO_TOP_SPEED,
        { CHEAT_INPUT_2, CHEAT_INPUT_2, CHEAT_INPUT_2, CHEAT_INPUT_2 },
        "No Top Speed"
    },
    {
        CHEAT_ID_HIGH_ACCELERATION, 
        { CHEAT_INPUT_3, CHEAT_INPUT_3, CHEAT_INPUT_3, CHEAT_INPUT_3 },
        "High Acceleration"
    },
    {
        CHEAT_ID_CAR_JUMP_ON_HORN, 
        { CHEAT_INPUT_2, CHEAT_INPUT_2, CHEAT_INPUT_2, CHEAT_INPUT_3 },
        "Car Jump on Horn"
    },
    {
        CHEAT_ID_ONE_TAP_TRAFFIC_DEATH,
        { CHEAT_INPUT_3, CHEAT_INPUT_3, CHEAT_INPUT_2, CHEAT_INPUT_2 },
        "One Tap Traffic Death"
    },
    {
        CHEAT_ID_UNLOCK_CAMERAS,
        { CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_0 },
        "Unlock All Cameras"
    },
    {
        CHEAT_ID_PLAY_CREDITS_DIALOG,
        { CHEAT_INPUT_0, CHEAT_INPUT_2, CHEAT_INPUT_2, CHEAT_INPUT_3 },
        "Play Credits Dialog"
    },
    {
        CHEAT_ID_SHOW_SPEEDOMETER,
        { CHEAT_INPUT_3, CHEAT_INPUT_3, CHEAT_INPUT_1, CHEAT_INPUT_2 },
        "Show Speedometer"
    },
    {
        CHEAT_ID_REDBRICK,
        { CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_3, CHEAT_INPUT_2 },
        "Red Brick"
    },
    {
        CHEAT_ID_INVINCIBLE_CAR,
        { CHEAT_INPUT_3, CHEAT_INPUT_0, CHEAT_INPUT_3, CHEAT_INPUT_0 },
        "Invincible Car"
    },
    {
        CHEAT_ID_SHOW_TREE,
        { CHEAT_INPUT_1, CHEAT_INPUT_0, CHEAT_INPUT_1, CHEAT_INPUT_3 },
        "Show Tree"
    },
    {
        CHEAT_ID_TRIPPY,
        { CHEAT_INPUT_3, CHEAT_INPUT_1, CHEAT_INPUT_3, CHEAT_INPUT_1 },
        "Trippy"
    },

    

#ifndef FINAL
    // register cheats that we don't want shipped in the final game here
    //
    {
        CHEAT_ID_UNLOCK_CARDS,
        { CHEAT_INPUT_0, CHEAT_INPUT_0, CHEAT_INPUT_0, CHEAT_INPUT_0 },
        "Unlock All Collectible Cards"
    },
    {
        CHEAT_ID_UNLOCK_SKINS,
        { CHEAT_INPUT_2, CHEAT_INPUT_3, CHEAT_INPUT_2, CHEAT_INPUT_3 },
        "Unlock All Character Clothing"
    },
    {
        CHEAT_ID_MOTHER_OF_ALL_CHEATS,
        { CHEAT_INPUT_0, CHEAT_INPUT_1, CHEAT_INPUT_2, CHEAT_INPUT_3 },
        "Display All Cheats"
    },
    {
        CHEAT_ID_UNLOCK_MISSIONS,
        { CHEAT_INPUT_0, CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_0 },
        "Unlock All Missions"
    },
    {
        CHEAT_ID_UNLOCK_MOVIES,
        { CHEAT_INPUT_1, CHEAT_INPUT_3, CHEAT_INPUT_2, CHEAT_INPUT_3 },
        "Unlock All Movies"
    },
    {
        CHEAT_ID_UNLOCK_EVERYTHING,
        { CHEAT_INPUT_0, CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_3 },
        "Unlock All (Unlockables)"
    },
    {
        CHEAT_ID_EXTRA_COINS,
        { CHEAT_INPUT_0, CHEAT_INPUT_2, CHEAT_INPUT_1, CHEAT_INPUT_3 },
        "Add Extra Coins"
    },
    {
        CHEAT_ID_KICK_TOGGLES_CHARACTER_MODEL,
        { CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_2 },
        "Kick Swaps Character Model"
    },
    {
        CHEAT_ID_SHOW_AVATAR_POSITION,
        { CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_1 },
        "Show Avatar Position"
    },
    {
        CHEAT_ID_FULL_DAMAGE_TO_CAR,
        { CHEAT_INPUT_2, CHEAT_INPUT_2, CHEAT_INPUT_3, CHEAT_INPUT_3 },
        "Apply Full Damage to Car"
    },
    {
        CHEAT_ID_EXTRA_TIME,
        { CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_1, CHEAT_INPUT_3 },
        "Extra Objective Time"
    },
    {
        CHEAT_ID_DEMO_TEST,
        { CHEAT_INPUT_0, CHEAT_INPUT_0, CHEAT_INPUT_0, CHEAT_INPUT_1 },
        "Enable Demotest mode"
    },
#endif

    // dummy terminator
    //
    {
        CHEAT_ID_UNREGISTERED,
        { UNKNOWN_CHEAT_INPUT, UNKNOWN_CHEAT_INPUT, UNKNOWN_CHEAT_INPUT, UNKNOWN_CHEAT_INPUT },
        ""
    }
};

static const unsigned int NUM_REGISTERED_CHEATS =
    sizeof( REGISTERED_CHEATS ) / sizeof( REGISTERED_CHEATS[ 0 ] ) - 1;

unsigned int CheatsDB::s_maxNumPossibleCheats = 1;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CheatsDB::CheatsDB
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
CheatsDB::CheatsDB()
:   m_cheats( NULL )
{
MEMTRACK_PUSH_GROUP( "CheatsDB" );
    unsigned int i = 0;

    // determine maximum number of possible cheat patterns
    //
    for( i = 0; i < NUM_CHEAT_SEQUENCE_INPUTS; i++ )
    {
        s_maxNumPossibleCheats *= NUM_CHEAT_INPUTS;
    }

    // create and initialize cheats database
    //
    m_cheats = new( GMA_PERSISTENT ) eCheatID[ s_maxNumPossibleCheats ];
    for( i = 0; i < s_maxNumPossibleCheats; i++ )
    {
        m_cheats[ i ] = CHEAT_ID_UNREGISTERED;
    }

    rTunePrintf( "\n---=[ Registered Simpsons Cheats Begin ]=---\n\n" );

    // add registered cheats to database
    //
    for( i = 0; i < NUM_REGISTERED_CHEATS; i++ )
    {
        int cheatIndex = this->ConvertSequenceToIndex( REGISTERED_CHEATS[ i ].m_cheatInputs );

        rAssertMsg( m_cheats[ cheatIndex ] == CHEAT_ID_UNREGISTERED,
                    "WARNING: *** Duplicate cheat input sequence found! Clobbering previously registered cheat." );

        rAssert( REGISTERED_CHEATS[ i ].m_cheatID < static_cast<int>( MAX_NUM_CHEATS ) );
        m_cheats[ cheatIndex ] = REGISTERED_CHEATS[ i ].m_cheatID;

#ifndef RAD_RELEASE
        char buffer[ 256 ];
        this->PrintCheatInfo( &(REGISTERED_CHEATS[ i ]), buffer );

        rTunePrintf( "Cheat ID [%02d]: %s\n",
                     REGISTERED_CHEATS[ i ].m_cheatID, buffer );
#endif
    }

    rTunePrintf( "\n---=[ Registered Simpsons Cheats End   ]=---\n\n" );
MEMTRACK_POP_GROUP( "CheatsDB" );
}

//===========================================================================
// CheatsDB::~CheatsDB
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
CheatsDB::~CheatsDB()
{
    // clean-up memory
    //
    if( m_cheats != NULL )
    {
        delete [] m_cheats;
        m_cheats = NULL;
    }
}

//===========================================================================
// CheatsDB::GetCheatID
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
eCheatID
CheatsDB::GetCheatID( unsigned int cheatIndex ) const
{
    rAssert( m_cheats );
    rAssert( cheatIndex < s_maxNumPossibleCheats );

    // return cheat ID associated with specified cheat index
    //
    return m_cheats[ cheatIndex ];
}

//===========================================================================
// CheatsDB::GetNumRegisteredCheats
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
unsigned int
CheatsDB::GetNumRegisteredCheats() const
{
    return NUM_REGISTERED_CHEATS;
}

//===========================================================================
// CheatsDB::GetCheat
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
const Cheat*
CheatsDB::GetCheat( eCheatID cheatID ) const
{
    // search for cheat
    //
    for( unsigned int i = 0; i < NUM_REGISTERED_CHEATS; i++ )
    {
        if( REGISTERED_CHEATS[ i ].m_cheatID == cheatID )
        {
            // found it!
            //
            return &(REGISTERED_CHEATS[ i ]);
        }
    }

    // cheat not found, return NULL
    //
    return NULL;
}

//===========================================================================
// CheatsDB::ConvertSequenceToIndex
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
unsigned int
CheatsDB::ConvertSequenceToIndex( const eCheatInput* cheatInputs,
                                  int numInputs )
{
    unsigned int cheatIndex = 0;

    // convert cheat sequence inputs to an index number
    //
    // cheatIndex = 8 bit value w/ 2 bits allocated per cheat input
    //   bits [0,1] = cheat input 0
    //   bits [2,3] = cheat input 1
    //   bits [4,5] = cheat input 2
    //   bits [6,7] = cheat input 3
    //
    rAssert( cheatInputs != NULL );
    for( int i = 0; i < numInputs; i++ )
    {
        cheatIndex |= (cheatInputs[ i ] << (i * 2));
    }

    rAssertMsg( cheatIndex < s_maxNumPossibleCheats,
                "ERROR: *** Invalid cheat index computed!" );

    return cheatIndex;
}

//===========================================================================
// CheatsDB::PrintCheatInfo
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
CheatsDB::PrintCheatInfo( const Cheat* cheat, char* buffer )
{
    rAssert( cheat != NULL );
    rAssert( buffer != NULL );

    sprintf( buffer, "%s: { ", cheat->m_cheatName );

    const int MAX_CHEAT_INPUT_NAME_LENGTH = 4; // truncate if necessary
    char inputNames[ NUM_CHEAT_SEQUENCE_INPUTS ][ MAX_CHEAT_INPUT_NAME_LENGTH ];

    for( unsigned int i = 0; i < NUM_CHEAT_SEQUENCE_INPUTS; i++ )
    {
        strncpy( inputNames[ i ],
                 CheatInputHandler::GetInputName( cheat->m_cheatInputs[ i ] ),
                 MAX_CHEAT_INPUT_NAME_LENGTH - 1 );
        inputNames[ i ][ MAX_CHEAT_INPUT_NAME_LENGTH - 1 ] = '\0';

        if( i != 0 ) // insert comma, except for the first one
        {
            strcat( buffer, ", " );
        }

        strcat( buffer, inputNames[ i ] );
    }

    strcat( buffer, " }" );
/*
    sprintf( buffer, "%s: { %s, %s, %s, %s }",
             cheat->m_cheatName,
             CheatInputHandler::GetInputName( cheat->m_cheatInputs[ 0 ] ),
             CheatInputHandler::GetInputName( cheat->m_cheatInputs[ 1 ] ),
             CheatInputHandler::GetInputName( cheat->m_cheatInputs[ 2 ] ),
             CheatInputHandler::GetInputName( cheat->m_cheatInputs[ 3 ] ) );
*/
}

//===========================================================================
// Private Member Functions
//===========================================================================

