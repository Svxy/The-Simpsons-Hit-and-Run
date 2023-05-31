//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GameData
//
// Description: GameData Interface and Constants.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/10/04      TChu        Created for SRR2
//
//===========================================================================

#ifndef GAMEDATA_H
#define GAMEDATA_H

//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// Forward References
//===========================================================================

typedef unsigned char GameDataByte; // single byte of data

//===========================================================================
// Interface Definitions
//===========================================================================

#ifdef RAD_WIN32
const unsigned int NUM_GAME_SLOTS = 8; // number of available game slots
#else
const unsigned int NUM_GAME_SLOTS = 4; // number of available game slots
#endif

struct GameDataHandler
{
    virtual void LoadData( const GameDataByte* dataBuffer,
                           unsigned int numBytes ) = 0;

    virtual void SaveData( GameDataByte* dataBuffer,
                           unsigned int numBytes ) = 0;

    // reset data to defaults
    //
    virtual void ResetData() = 0;
};

struct GameData
{
    GameDataHandler* m_gdHandler;
    unsigned int m_numBytes;

#ifdef RAD_DEBUG
    char m_name[ 64 ];
#endif

};

#endif // GAMEDATA_H
