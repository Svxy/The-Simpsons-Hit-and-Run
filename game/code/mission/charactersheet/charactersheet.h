//CharacterSheet struct for saving data
#ifndef CHARACTERSHEET_H
#define CHARACTERSHEET_H


#include <render/enums/renderenums.h>
#include <camera/supercam.h>

const int MAX_MISSIONS = 8; //this is set to 8 since there is a Stupid tutorial mission in level 1
const int MAX_CARDS = 7;
const int MAX_STREETRACES = 3;
const int MAX_CAMERAS = 12;
const int MAX_COINS = 100;
const int MAX_LEVELS = 7;
const int MAX_PURCHASED_ITEMS = 12; //the maximum number of things that can be buy with tokens.
const int MAX_CARS_OWNED =60;
const unsigned int MAX_LEVEL_GAGS = 32; // stored as a bitmask, so if you need to make it bigger, 
                               // change the type in the level struct

// on PS2, always use a byte for a boolean, so that this data type size
// is the same for all builds (debug/tune/release)
//
#ifdef RAD_PS2
    typedef unsigned char CS_BOOL;
#else
    typedef bool CS_BOOL;
#endif

    enum CarDamageStateEnum
    {
        eNull,
        eNoDamage,
        eDamaged,
        eHusk,
        eNumOfStates
    };

    static const int NUM_PERSISTENT_SECTORS = 82; // Add 7 'sectors' on the end of global level objects.
    static const int NUM_PERSISTENT_OBJECTS = 128; // Number of persistent objects we'll track per zone/rail.
    static const int NUM_BYTES_PER_SECTOR = ( NUM_PERSISTENT_OBJECTS + 7 ) >> 3; // We'll bit pack the object states so this is the number of bytes we'll need per zone/rail.
    static const int NUM_BYTES_FOR_PERSISTENT_STATES = NUM_BYTES_PER_SECTOR * NUM_PERSISTENT_SECTORS; // And this is the number of bytes we'll need for the entire game.
    static const int NUM_BYTES_FOR_STATES = 1;

    struct CarCharacterSheet
    {
        char mName[16];
        float mCurrentHealth; // 0.0f = hust, 1.0f = no damage;
        float mMaxHealth;
    };

    struct CarInventoryStruct
    {
        CarCharacterSheet mCars [MAX_CARS_OWNED];
        int mCounter;
    };


    struct CurrentMissionStruct
        {
            RenderEnums::LevelEnum mLevel;
            RenderEnums::MissionEnum mMissionNumber;
        };
    struct Record
        {
            char mName [16];
            CS_BOOL mCompleted;
        };

    struct MissionRecord
        {
            char mName [16];
            CS_BOOL mCompleted;
            CS_BOOL mBonusObjective;
            unsigned int mNumAttempts;
            CS_BOOL mSkippedMission;
            int mBestTime;
        };


    struct CharCardList
        {
            Record mList [MAX_CARDS];
        };

    struct MissionList
        {
            MissionRecord mList [MAX_MISSIONS];
        };
    struct StreetRaceList
        {
            MissionRecord mList [MAX_STREETRACES];
        };


    struct LevelRecord
        {
            //collectable items
            CharCardList mCard;                      

            //Mission items
            MissionList mMission;
            StreetRaceList mStreetRace;
            MissionRecord mBonusMission;
            MissionRecord mGambleRace;


            CS_BOOL mFMVunlocked;
            int mNumCarsPurchased;
            int mNumSkinsPurchased;
            int mWaspsDestroyed;
            char mCurrentSkin [16];
            
            int mGagsViewed;
            unsigned mGagMask;

            CS_BOOL mGags[MAX_LEVEL_GAGS];

            //stuff that player has bought with tokens
            CS_BOOL mPurchasedRewards [MAX_PURCHASED_ITEMS];

        };
    
/*
    //struct for saving player preferences For FE options etc.
    struct PlayerOptions
    {
        CS_BOOL mRumble;
        float mSFXVolume;
        float mMusicVolume;
        float mDialogVolume;
		CS_BOOL mInvertCamera;
		CS_BOOL mRadarOn;
		CS_BOOL mTutorialOn;
		CS_BOOL mNavSystemOn;

        SuperCam::Type mDriverCam;

    };

    struct TutorialsSeen
    {
        int mBitfield;
    };
*/

    //this is the master character sheet that we will be saving to the memory card
    struct CharacterSheet
        {
            char mPlayerName [16];
//            PlayerOptions mPlayerOptions;
            LevelRecord mLevelList [MAX_LEVELS];
            CurrentMissionStruct mCurrentMissionInfo;
            CurrentMissionStruct mHighestMissionPlayed;
//            TutorialsSeen mTutoiralsSeen;
            CS_BOOL mIsNavSystemEnabled;
            int mNumberOfTokens; // used to track the number of coins/token whatever used to buy things
            CarInventoryStruct mCarInventory;
            unsigned char mPersistentObjectStates[ NUM_BYTES_FOR_PERSISTENT_STATES ]; // The broken state for objects in the world.
            unsigned char mStates[NUM_BYTES_FOR_STATES]; // Flags used for the state of the game, such as if the player has the IS movie ticket.
        };
    


       



#endif //CHARACTER_SHEET.h

