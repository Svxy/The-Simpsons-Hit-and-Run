//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        mission.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef MISSION_H
#define MISSION_H

//========================================
// Nested Includes
//========================================

#include <events/eventlistener.h>

#include <events/eventenum.h>

#include <memory/srrmemory.h>

#include <mission/missionstage.h>

#include <string.h>


//========================================
// Forward References
//========================================

class Vehicle;
class CarStartLocator;
class ZoneEventLocator;
class BonusObjective;
class StatePropCollectible;
class AnimatedIcon;

//=============================================================================
//
// Synopsis:    Controls the flow through a mission by sequentially leading
//              the player through one or more stages.  Upon the completion
//              of a stage, the next stage becomes active, and so on until
//              all stages (and thus the mission) are complete.
//
//=============================================================================

class Mission : public EventListener
{
public:

    enum { MAX_BONUS_OBJECTIVES = 3 };

    Mission();
    ~Mission();

    // Gets the index in the text bible of the name of this mission
    char* GetName() { return( &mcName[ 0 ] ); }
    void SetName( char* name );

    // Stages in this mission
    void SetNumStages( int num ) { mNumMissionStages = num; }
    int GetNumStages() { return( mNumMissionStages ); }

    MissionStage* GetStage( int index );
    void SetStage( int index, MissionStage* stage );

	
    MissionStage* GetCurrentStage();
    int GetCurrentStageIndex() const { return( mCurrentStage ); }

    // loads and initializes all the stages
	virtual void Initialize( GameMemoryAllocator heap );

    // finalizes and dumps all the stages
	virtual void Finalize();


    // These two methods finalize the current stage then move
    // to and initialize some other stage
	void SetToStage( int index, bool resetting = false );
	virtual void NextStage();
	virtual void PrevStage();

    // Resets the current mission stage
	void ResetStage();

    // fake event listener
	virtual void HandleEvent( EventEnum id, void* pEventData );

    // The GamePlayManager should call this for every frame
    // the mission is active
	void Update( unsigned int elapsedTime );
    
    //
    // Adds a vehicle for this mission only
    //
    //void AddVehicle( Vehicle* vehicle );
    //Vehicle* GetVehicleByName( char* name );

    //
    // Accessors for the time left in the mission
    //
    int GetMissionTimeLeftInSeconds();
    int GetMissionTimeLeftInMilliSeconds();

    // Resets the entire mission. Use with caution!
    void Reset( bool JumpStage = false );
    void ResetPlayer();
    void InitDynaLoad();

    bool DialogueCharactersTeleported();

    enum MissionState
    {
        STATE_WAITING,
        STATE_INPROGRESS,
        STATE_FAILED,
        STATE_SUCCESS,
        NUM_STATES
    };

    MissionState GetState() { return( mState ); }

    // Returns true iff every stage in this mission is complete
	bool IsComplete();
    
    //methods called from the missionscriptloader these shouldnt be called by any thing other missionscriptloader
    //since are for setup.
    void SetVehicleRestart( CarStartLocator* loc ) { mVehicleRestart = loc; };
    void SetPlayerRestart( Locator* loc ) { mPlayerRestart = loc; };
    void SetRestartDynaload( const char* loadString, const char* interior = NULL ) ;
    void LoadStreetRaceProps(const char* loadString );
    void UnloadStreetRaceProps (const char* loadString);

    
    //higher level methods for code use
    void InitStreetRacePropLoad();
    void InitStreetRacePropUnload();
   
   

    void SetSundayDrive() { mSundayDrive = true; };
    bool IsSundayDrive() const { return mSundayDrive; };
    void SetResetToStage( int stage ) { mResetToStage = stage; };
    void SetBonusMission();
    bool IsBonusMission();
    bool IsRaceMission(); //determines if this mission is a special road race
    bool IsWagerMission();

    void AddBonusObjective( BonusObjective* bo );
    void StartBonusObjectives();

 
    void SetForcedCar( bool isForced );
    bool IsForcedCar() const { return mIsForcedCar; };

    void SetMissionTime( int timeMilliseconds );

    void CreateStatePropCollectible( const char* statepropname, const char* locator, int collisionattributes );
    void AttachStatePropCollectible( const char* statepropname, const char* vehicleName, int collisionattributes );

    bool GetSwappedCarsFlag();
    void SetSwappedCarsFlag(bool flag);

    void SpecialCaseStageBackup( unsigned int num ) { mChangingStages = true; mJumpBackStage = true; mJumpBackBy = num; };

    void SetInitPedGroup( int initGroupId );

    void ShowHUD( bool isShowHUD );
    bool IsHUDVisible() const;
    bool CanMDKCar(Vehicle* pVehicle,MissionStage* pStage);

    void SetNumValidFailureHints( int numHints );
    int GetNumValidFailureHints() const;
    CarStartLocator* GetVehicleRestart () { return mVehicleRestart;};

    inline bool IsChangingStages() const { return mChangingStages; }

    bool GetCarryOverOutOfCarCondition() { return mbCarryOverOutOfCarCondition;};

    void SetCarryOverOutOfCarCondition(bool flag) { mbCarryOverOutOfCarCondition = flag;};

public:
    bool mIsStreetRace1Or2;

protected:
    void SetCurrentStageIndex( int index ) { mCurrentStage = index; }

    virtual void DoUpdate( int elapsedTime );
private:

    bool UnlockStage( MissionStage* stage );
    void SetupStageChange();
    void DoStageChange();


    static const int MAX_STAGES = 25;

    int mNumMissionStages;
    MissionStage* mMissionStages[ MAX_STAGES ];
    int mCurrentStage;
    int mResetMission;

    char mcName[16];

    GameMemoryAllocator mHeap;

    bool mbComplete;
    bool mbIsLastStage;
    
    int mMissionTimer;

    int mElapsedTimems;

    MissionState mState;
    MissionStage::MissionStageState mLastStageState;

    // TODO greg
    // this is soon to be history
    
    //static const int MAX_VEHICLES = 4;
    //int mNumVehicles;
    //Vehicle* mVehicles[ MAX_VEHICLES ];

    CarStartLocator* mVehicleRestart;
    Locator* mPlayerRestart;
    ZoneEventLocator* mDynaloadLoc;

    ZoneEventLocator* mStreetRacePropsLoad;    //Chuck: adding  these so we can load and unload street race barriers.
    ZoneEventLocator* mStreetRacePropsUnload;


    int mResetToStage;

    bool mSundayDrive  : 1;
    bool mBonusMisison : 1;

    BonusObjective* mBonusObjectives[ MAX_BONUS_OBJECTIVES ];
    unsigned int mNumBonusObjectives;

    bool mIsForcedCar;
    //Chuck Adding this so on retry the car gets auto repaired 
    bool mbAutoRepairCar;

    //Chuck: this variable used for forced missions so that the designers can swap
    // the forced car for the default vehicle.
    //if true that means that we have used a scripted sequence switched from the forced to our default 
    //and dont need to  swap in the default car at the end of the mission
    bool mbSwappedCars;

    //flag for carrying over get out of car condition from stage to stage
    bool mbCarryOverOutOfCarCondition;



    //Flag to trigger the patty and selma screen after street race
    bool mbTriggerPattyAndSelmaScreen;

    int mFinalDelay;
    int mCompleteDelay;

    bool mChangingStages;

    bool mNoTimeUpdate;

    bool mJumpBackStage;
    char mJumpBackBy;

    int mNumStatePropCollectibles;
    StatePropCollectible** mStatePropCollectibles;

    AnimatedIcon* mDoorStars;
    int mInitPedGroupId;

    bool mShowHUD : 1;

    int mNumValidFailureHints;

};

//=============================================================================
// Mission::SetName
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* name )
//
// Return:      void 
//
//=============================================================================
inline void Mission::SetName( char* name ) 
{ 
    strcpy( &mcName[ 0 ], name ); 
}

//=============================================================================
// Mission::SetStage
//=============================================================================
// Description: Comment
//
// Parameters:  ( int index, MissionStage* stage )
//
// Return:      inline 
//
//=============================================================================
inline void Mission::SetStage( int index, MissionStage* stage )
{ 
    rTuneAssertMsg( index < MAX_STAGES, "Too many stages, get Cary to increase the max!\n" );
    mMissionStages[ index ] = stage; 
}

//=============================================================================
// Mission::GetStage
//=============================================================================
// Description: Comment
//
// Parameters:  ( int index )
//
// Return:      inline 
//
//=============================================================================
inline MissionStage* Mission::GetStage( int index )
{ 
    if( index >= 0 && index < mNumMissionStages )
    {
        return( mMissionStages[ index ] );
    }
    else
    {
        return( NULL );
    }
}

//=============================================================================
// Mission::IsHUDVisible
//=============================================================================
// Description: Comment
//
// Parameters:  
//
// Return:      inline 
//
//=============================================================================
inline bool Mission::IsHUDVisible() const
{
    return mShowHUD;
}

inline void Mission::SetNumValidFailureHints( int numHints )
{
    mNumValidFailureHints = numHints;
}

inline int Mission::GetNumValidFailureHints() const
{
    return mNumValidFailureHints;
}

#endif //MISSION_H
