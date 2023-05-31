//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supersprintdata.h
//
// Description: Blahblahblah
//
// History:     2/8/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SUPERSPRINTDATA_H
#define SUPERSPRINTDATA_H

//========================================
// Nested Includes
//========================================
#include <constants/maxplayers.h>

#include <p3d/p3dtypes.hpp>

//========================================
// Forward References
//========================================

class Vehicle;
class WaypointAI;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

namespace SuperSprintData
{
    enum 
    {
#ifdef RAD_PS2
        NUM_PLAYERS = 4,
#else
        NUM_PLAYERS = 4,
#endif
        DEFAULT_TURBO_NUM = 3,
        DEFAULT_NUM_LAPS = 3,
        MIN_NUM_LAPS = 1,
        MAX_NUM_LAPS = 5,
        FLAG_TIMEOUT = 4000,
        MAX_CHARACTER_NAME_LEN = 32
    };

    struct CarData
    {
        enum State
        {
            WAITING,
            SELECTING,
            SELECTED
        };

        CarData() : 
            mVehicle( NULL ), 
            mVehicleAI( NULL ), 
            mState( WAITING ),
            mActiveListIndex( -1 ),
            mIsHuman(false)
        {
            mCarName[ 0 ] = '\0';
        };

        Vehicle*        mVehicle;
        WaypointAI*      mVehicleAI;
        State           mState;
        char            mCarName[ 16 ];
        int             mActiveListIndex;
        bool            mIsHuman;
    };

    struct PlayerData
    {
        PlayerData() : 
            mLapTime( 0 ), 
            mBestLap( 0xffffffff ), 
            mRaceTime( 0 ), 
            mNumLaps( 0 ), 
            mPosition( 0 ),
            mPoints( 0 ),
            mWins( 0 ),
            mNextCheckPoint( 0 ), 
            mBestTimeEntry( -1 ),
            mBestLapEntry( -1 ),
            mRacing( false ),
            mCheated( false ),
            mCharacterIndex( -1 ),
            mDistToCheckpoint( 10000000.0f ) { mCharacterName[0] = '\0'; };

        unsigned int mLapTime;
        unsigned int mBestLap;
        unsigned int mRaceTime;
        unsigned char mNumLaps;
        unsigned char mPosition;
        unsigned char mPoints;
        unsigned char mWins;
        char mNextCheckPoint;
        int mBestTimeEntry;
        int mBestLapEntry;
        bool mRacing;
        bool mCheated;
        char mCharacterName[MAX_CHARACTER_NAME_LEN];
        int mCharacterIndex;
        float mDistToCheckpoint;
    };

    struct DisplayNames
    {
        const char* name;
        const char* text;
    };

    extern const DisplayNames VEHICLE_NAMES[];
    extern const unsigned int NUM_NAMES;

    extern const char* CHARACTER_NAMES[];
    extern const unsigned int NUM_CHARACTER_NAMES;

    const tColour PLAYER_COLOURS[] = 
    {
        tColour( 213, 74, 33 ),
        tColour( 36, 232, 255 ),
        tColour( 246, 255, 5 ),
        tColour( 35, 209, 14 ),

        tColour( 0, 0, 0 ) // dummy terminator
    };

    struct HighScore
    {
        char name[4];
        unsigned int carNum;
        unsigned int score;
        enum { NUM_HIGH_SCORE = 10 };
    };
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //SUPERSPRINTDATA_H
