//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supersprintmanager.h
//
// Description: Blahblahblah
//
// History:     2/3/2003 + Created -- Cary Brisebois    
//
//=============================================================================

#ifndef SUPERSPRINTMANAGER_H
#define SUPERSPRINTMANAGER_H

//========================================
// Nested Includes
//========================================
#include <events/eventlistener.h>
#include <input/mappable.h>
#include <mission/gameplaymanager.h>
#include <mission/animatedicon.h>
#include <loading/loadingmanager.h>
#include <supersprint/supersprintdata.h>
#include <supersprint/supersprintdrawable.h>

#include <roads/roadmanager.h>

//========================================
// Forward References
//========================================
class Vehicle;
class AnimCollisionEntityDSG;
class RoadSegment;
class EventLocator;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SuperSprintManager : public Mappable,
                           public GameplayManager,
                           public LoadingManager::ProcessRequestsCallback
{
public:

    enum
    {
        MAX_AI_WAYPOINTS = 20,
        MAX_CHECKPOINT_LOCATORS = 16
    };

    //-------------  S T A T E

    enum State
    {
        IDLE,               //idle, doing nothing

        COUNT_DOWN,         //Start of race 3, 2, 1 countdown

        RACING,             //Racing until everyone crosses the finish line.  Anyone who has crossed the finish 
        //line is disabled until the end of the race.  The last player has 30 seconds from the 
        //previous player finishing or they are DNF

        DNF_TIMEOUT,        //When this starts, anyone who does not finish in the remaining time gets a DNF

        WINNER_CIRCLE,      //Race stats and display of the winner and their time to finish

        PAUSED              //Get it?

        // 
        // Goes IDLE -> COUNT_DOWN -> RACING -> WINNER_CIRCLE
        //                                   -> DNF_TIMEOUT ->WINNER_CIRCLE
        //
        // At anytime we can quit back to the game through the pause menu.
        //
    };
    //From EventListener
    void HandleEvent( EventEnum id, void* pEventData );

    //From LoadingManager::ProcessRequestsCallback
    void OnProcessRequestsComplete( void* pUserData );

    //From GamePlayManager
    virtual void LoadLevelData();
    virtual void InitLevelData() {};
    virtual void CleanMissionData() {};
    virtual bool IsSundayDrive() { return true; };
    virtual bool IsSuperSprint();
    virtual void PerformLoading() {};
    virtual void Reset();

    //From Mappable
    virtual void OnButton( int controllerId, int id, const Button* pButton );
    virtual void OnButtonUp( int controllerId, int buttonId, const Button* pButton );
    virtual void OnButtonDown( int controllerId, int buttonId, const Button* pButton );
    virtual void LoadControllerMappings( unsigned int controllerId );
    virtual void OnControllerConnect( int id );
    virtual void OnControllerDisconnect( int id );

    char GetNumCheckpoints();

    //Local
    static SuperSprintManager* GetInstance();
    static void DestroyInstance();
    
    void Initialize();
    void Finalize();
    void Update( unsigned int milliseconds );

    void LoadScriptData();
    void LoadCars();
    void LoadCharacters();
    void StartRace();

    void SetCurrentLevel( int level );
    void SetNumLaps( int numLaps );
    void SetTrackDirection( bool reverse );
    bool IsTrackReversed() const;
    void SetCharacter( int playerID, int index );
    void SetVehicle( int playerID, const char* name );

    char FindLeader();  //Returns who's in the lead

    void CalculatePositions();

    const SuperSprintData::CarData* GetVehicleData( int playerID ) const;
    const SuperSprintData::PlayerData* GetPlayerData( int playerID ) const;

    int GetOnlyHumanPlayerID();

    State GetState() const {return mCurrentState;};

    void RestoreControllerState();

protected:
    void LoadMission() {};

private:



    enum ButtonMap
    {
        Start,
        Select,
        Back,
        Left,
        Right,
        StickX,
        CamSelect,
        L1,
        ShowPositions
    };

    static SuperSprintManager* spInstance;

    State mCurrentState;

    void SetState( State state );


    //-------------  P L A Y E R  C A R S

    SuperSprintData::CarData mVehicleSlots[ SuperSprintData::NUM_PLAYERS ];


    //-------------  P L A Y E R  D A T A

    SuperSprintData::PlayerData mPlayers[ SuperSprintData::NUM_PLAYERS ];

    //------------- D R A W A B L E

    SuperSprintDrawable* mDrawable;

    int mCountDownTime;
    char mTime[32];

    int mCurrentPosition;

    unsigned int mStartTime;

    int mNumActivePlayers;
    int mNumHumanPlayers;
    int mNumHumansFinished;

    int mCurrentLevel;
    int mNumLaps;

    // can either drive left around the track or right around the track
    // default to FALSE (i.e. going "left" is going backwards)
    bool mGoLeft;

    char mNumCheckPoints;  //Use the accessor please.
    char* mCheckPoints;

    AnimatedIcon* mFFlag;
    AnimatedIcon* mWFlag;
    unsigned int mFFlagTimeout;
    unsigned int mWFlagTimeout;

    AnimCollisionEntityDSG* mTrapController;
    bool mTrapTriggered;

    char mPositions[ SuperSprintData::NUM_PLAYERS ];  //This is an array of player IDs.
    AnimatedIcon* mPositionIcon[ SuperSprintData::NUM_PLAYERS ][ SuperSprintData::NUM_PLAYERS ];
    AnimatedIcon* mPlayerID[ SuperSprintData::NUM_PLAYERS ];
    bool mTogglePosition[ SuperSprintData::NUM_PLAYERS ];
    AnimatedIcon* mNitroEffect[ SuperSprintData::NUM_PLAYERS ];

    struct PathData
    {
        PathData() : seg( NULL ), segT( 0.0f ), roadT( 0.0f ), loc( NULL ) {};
        RoadSegment* seg;
        float segT;
        float roadT;
        RoadManager::PathElement closestElem;
        EventLocator* loc;
    };

    PathData mPathData[ MAX_AI_WAYPOINTS ];
    EventLocator* mCheckPointLocators[ MAX_CHECKPOINT_LOCATORS ];
    unsigned int mNumCheckPointLocators;

    void PositionCharacters();
    void SetUpCars();
    void CleanUpCars();
    void PositionCars();
    void PositionAI();
    void DisableAllAI();
    void DisableAllControllers();
    void InitRaceData();
    void ResetRaceData();
    void PlaceCharactersInCars();
    void EnumerateControllers();
    void SetupIcons();
    void InitCamera();
    void SetupTraps();
    void PlayIntroCam();
    void UpdatePositionIcons( unsigned int milliseconds );
    Locator* GetCheckpointWith( char data );
    int GetPathDataWith( Locator* loc );

    SuperSprintManager();
    virtual ~SuperSprintManager();

    //Prevent wasteful constructor creation.
    SuperSprintManager( const SuperSprintManager& supersprintmanager );
    SuperSprintManager& operator=( const SuperSprintManager& supersprintmanager );

};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

inline SuperSprintManager* GetSSM() { return SuperSprintManager::GetInstance(); };

//*****************************************************************************
//
//Inline Private Member Functions
//
//*****************************************************************************
inline void SuperSprintManager::SetState( SuperSprintManager::State state )
{
    mCurrentState = state;
}

//=============================================================================
// SuperSprintManager::IsSuperSprint
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline bool SuperSprintManager::IsSuperSprint()
{
    return( true );
}

//=============================================================================
// SuperSprintManager::SetCurrentLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ( int level )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintManager::SetCurrentLevel( int level )
{
    mCurrentLevel = level;
}

//=============================================================================
// SuperSprintManager::SetNumLaps
//=============================================================================
// Description: Comment
//
// Parameters:  ( int level )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintManager::SetNumLaps( int numLaps )
{
    mNumLaps = numLaps;
}

//=============================================================================
// SuperSprintManager::SetTrackDirection
//=============================================================================
// Description: Comment
//
// Parameters:  ( int level )
//
// Return:      void 
//
//=============================================================================
inline void SuperSprintManager::SetTrackDirection( bool reverse )
{
    mGoLeft = reverse;
}

//=============================================================================
// SuperSprintManager::IsTrackReversed
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool SuperSprintManager::IsTrackReversed() const
{
    return mGoLeft;
}

//=============================================================================
// SuperSprintManager::GetPlayerData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:       
//
//=============================================================================
inline const SuperSprintData::PlayerData*
SuperSprintManager::GetPlayerData( int playerID ) const
{
    rAssert( playerID >= 0 && playerID < SuperSprintData::NUM_PLAYERS );

    return &( mPlayers[ playerID ] );
}

//=============================================================================
// SuperSprintManager::GetVehicleData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:       
//
//=============================================================================
inline const SuperSprintData::CarData*
SuperSprintManager::GetVehicleData( int playerID ) const
{
    rAssert( playerID >= 0 && playerID < SuperSprintData::NUM_PLAYERS );

    return &( mVehicleSlots[ playerID ] );
}

#endif //SUPERSPRINTMANAGER_H
