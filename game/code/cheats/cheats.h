//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CheatsDB
//
// Description: Interface for the CheatsDB class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/19      TChu        Created for SRR2
//
//===========================================================================

#ifndef CHEATS_H
#define CHEATS_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <cheats/cheatinputs.h>

//===========================================================================
// Forward References
//===========================================================================

typedef unsigned int CHEATBITMASK;

const unsigned int MAX_NUM_CHEATS = sizeof( CHEATBITMASK ) * 8;

enum eCheatID
{
    CHEAT_ID_UNREGISTERED = -1,

    CHEAT_ID_MOTHER_OF_ALL_CHEATS,  // display all cheats on screen

    // this is not actually a cheat, but is merely to indicate that
    // all cheat id's following this will be enabled whenever the
    // CHEAT_ID_UNLOCK_EVERYTHING cheat is enabled
    //
    CHEAT_ID_UNLOCK_BEGIN,

    CHEAT_ID_UNLOCK_CARDS = CHEAT_ID_UNLOCK_BEGIN,
    CHEAT_ID_UNLOCK_SKINS,
    CHEAT_ID_UNLOCK_MISSIONS,
    CHEAT_ID_UNLOCK_MOVIES,
    CHEAT_ID_UNLOCK_VEHICLES,
    CHEAT_ID_UNLOCK_EVERYTHING,     // unlock everything!!!

    CHEAT_ID_NO_TOP_SPEED,
    CHEAT_ID_HIGH_ACCELERATION,
    CHEAT_ID_CAR_JUMP_ON_HORN,
    CHEAT_ID_FULL_DAMAGE_TO_CAR,
    CHEAT_ID_ONE_TAP_TRAFFIC_DEATH,
    CHEAT_ID_EXTRA_TIME,
    CHEAT_ID_SHOW_AVATAR_POSITION,
    CHEAT_ID_KICK_TOGGLES_CHARACTER_MODEL,
    CHEAT_ID_EXTRA_COINS,
    CHEAT_ID_UNLOCK_CAMERAS,
    CHEAT_ID_SPEED_CAM = CHEAT_ID_UNLOCK_CAMERAS,
    CHEAT_ID_DEMO_TEST,
    CHEAT_ID_PLAY_CREDITS_DIALOG,
    CHEAT_ID_SHOW_SPEEDOMETER,
    CHEAT_ID_REDBRICK,
    CHEAT_ID_INVINCIBLE_CAR,
    CHEAT_ID_SHOW_TREE,
    CHEAT_ID_TRIPPY,

    NUM_CHEATS
};

struct Cheat
{
    eCheatID m_cheatID;
    eCheatInput m_cheatInputs[ NUM_CHEAT_SEQUENCE_INPUTS ];
    const char* m_cheatName;
};

//===========================================================================
// Interface Definitions
//===========================================================================

class CheatsDB
{
public:
	CheatsDB();
    virtual ~CheatsDB();

    eCheatID GetCheatID( unsigned int cheatIndex ) const;

    unsigned int GetNumRegisteredCheats() const;
    const Cheat* GetCheat( eCheatID cheatID ) const;

    static unsigned int ConvertSequenceToIndex( const eCheatInput* cheatInputs,
                                                int numInputs = NUM_CHEAT_SEQUENCE_INPUTS );

    static void PrintCheatInfo( const Cheat* cheat, char* buffer );

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CheatsDB( const CheatsDB& );
    CheatsDB& operator= ( const CheatsDB& );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    static unsigned int s_maxNumPossibleCheats;
    eCheatID* m_cheats;

};

#endif // CHEATS_H
