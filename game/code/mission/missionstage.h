//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        missionstage.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef MISSIONSTAGE_H
#define MISSIONSTAGE_H

//========================================
// Nested Includes
//========================================

#include <radkey.hpp>
#include <events/eventlistener.h> 
#include <camera/supercam.h>
#include <ai/vehicle/vehicleai.h>
#include <ai/vehicle/waypointai.h>
#include <vector>
#include <memory/stlallocators.h>
#include <mission/haspresentationinfo.h>
#include <string.h>
#include <loading/loadingmanager.h>
//========================================
// Forward References
//========================================

class MissionObjective;
class MissionCondition;

class Character;
class Vehicle;
class VehicleAI;
class Locator;
class ZoneEventLocator;
class CarStartLocator;
class SafeZone;




//Const Declarations

//max of 1 chase data structs since we will only have 2 different generic chase vehicles per level
const int MAX_CHASE_STRUCTS = 1;
//max of 3 safezone per stage.
const int MAX_SAFEZONES = 3;

const unsigned int MAX_LOCK_REQUIREMENTS = 2;
//=============================================================================
//
// Synopsis:    Represents a single "task" for the player to perform, such as
//              getting something or chasing someone.  Each MissionStage has
//              a single objective (the action to perform) and zero or more
//              conditions for failure (such as wrecking the car).
//
//=============================================================================

class MissionStage : 
    public EventListener, 
    public LoadingManager::ProcessRequestsCallback,
    public HasPresentationInfo
{
public:

    MissionStage();
    ~MissionStage();
		
    //
    // AI and waypoints
    //
    /*
    void AddVehicle(Vehicle* vehicle, 
                    int vehicleCentralIndex,  
                    CarStartLocator* spawnlocator, 
                    VehicleAI* vehicleAI );
    */
    
    void AddVehicle(Vehicle* vehicle, 
                    int vehicleCentralIndex,  
                    CarStartLocator* spawnlocator, 
                    char* ainame);
    
    void AddWaypoint( Locator* locator );
    void AddCharacter( char* name, 
                       CarStartLocator* spawnlocator,
                       CarStartLocator* carlocator,
                       const char* dynaloadString,
                       Vehicle* vehicle );
    //chuck overloaded function 
    void AddCharacter(char* name,                        
                       CarStartLocator* spawnlocator,
                       CarStartLocator* carlocator,
                       const char* dynaloadString,
                       char* VehicleName);

                        

    struct AIParams
    {
        int minShortcutSkill;
        int maxShortcutSkill;

        AIParams()
        {
            minShortcutSkill = 15;
            maxShortcutSkill = 25;
        };
    };
    void SetAIParams( Vehicle* vehicle, const AIParams& params );
    void SetAIRaceCatchupParams( Vehicle* vehicle, const VehicleAI::RaceCatchupParams& params );
    void SetAIEvadeCatchupParams( Vehicle* vehicle, const VehicleAI::EvadeCatchupParams& params );
    void SetAITargetCatchupParams( Vehicle* vehicle, const VehicleAI::TargetCatchupParams& params );

    //
    // The objective of this mission
    //
    MissionObjective* GetObjective();
    void SetObjective( MissionObjective* objective );

    //
    // The conditions for failure of this mission
    //
    unsigned int GetNumConditions();
    void SetNumConditions( unsigned int num );

    MissionCondition* GetCondition( int index );
    void SetCondition( int index, MissionCondition* condition );

    // Finds the first failure condition and returns it.
    //
    MissionCondition* GetFailureCondition() const;

    //
    // Activates all the stuff this stage uses
    //
	void Initialize();
    //
    // Cleans up all the stuff for this stage
    //
	void Finalize();
    void VehicleFinalize();
    
    //
    // Resets all the objects and stuff to the default positions
    //
	void Reset();
    void VehicleInfoInitialize();

    void Start();

    // Tells the dynamic loading system to start loading the area
    // the mission starts in
    //
    bool LoadMissionStart();

    //
    // Update should call this every frame when this stage
    // is active.
    //
	virtual void Update( unsigned int elapsedTime );

	virtual void HandleEvent( EventEnum id, void* pEventData );

    //
    // Time for this stage
    //
    enum StageTimeType
    {
        STAGETIME_NOT_TIMED,
        STAGETIME_ADD,
        STAGETIME_SET,
        NUM_STAGETIME_TYPES
    };

    void SetStageTime( StageTimeType type, unsigned int seconds );
    void GetStageTime( StageTimeType &type, unsigned int &seconds );

    void SetFinalStage( bool bIsFinal );
    bool GetFinalStage();

    //
    // Progress in this stage
    //
    enum MissionStageState
    {
        STAGE_IDLE,
        STAGE_INPROGRESS,
        STAGE_COMPLETE,
        STAGE_FAILED,
        STAGE_BACKUP,  //This tells us that something went wrong, we wanna go bak one stage.
        STAGE_ALL_COMPLETE,
        NUM_STATES
    };

    //
    // Returns the progress of this mission stage
    //
    MissionStageState GetProgress() { return ( mState ); }

    //
    // Index in the bible of this mission stage's name
    //
    void SetNameIndex( int index );
	int GetNameIndex();

    //
    // Returns the index in the text bible of the starting 
    // or success message
    //
    void SetStartMessageIndex( int index );
	int GetStartMessageIndex();
    void ShowStartMessageIndex( bool show ) { mbShowMessage = show; };

    void SetCameraInfo( SuperCam::Type type, bool cut, bool quickTransition );

    int GetNumVehicles() const { return mNumVehicles; };
    Vehicle* GetVehicle(  int which );

    bool GetMusicChangeFlag() { return( mMusicChange ); }
    void SetMusicChangeFlag() { mMusicChange = true; }

    bool GetMusicAlwaysOnFlag() { return( mKeepMusicOn ); }
    void SetMusicAlwaysOnFlag() { mKeepMusicOn = true; }

    void SetStageStartMusicEvent( radKey32 key ) { mMusicEventKey = key; }
    radKey32 GetStageStartMusicEvent() { return( mMusicEventKey ); }

    void SetStageMusicState( radKey32 stateKey, radKey32 stateEventKey );
    void GetStageMusicState( radKey32& stateKey, radKey32& stateEventKey );

    void SetDialogKey( radKey32 key ) { mDialogEventKey = key; }
    void SetConversationCharacterKey( tUID uid ) { mConversationCharacterKey = uid; }

    void SetTrafficDensity( char density ) { mTrafficDensity = density; };


	//Setting the chase vehicle spawnrate
	void SetChaseSpawnRate (char* vehiclename,unsigned int spawnrate);

	//Enable Traffic etc.
	void EnableTraffic ();
	void DisableTraffic ();
    void ClearTrafficForStage();
    void PutAllAisInLimbo( bool inLimbo );
    void DisableHitAndRun();

    //Add safezones
    int AddSafeZone (CarStartLocator* locator,unsigned int radius);

    //Make time into an elasped timer
    void UseElapsedTime();
    bool QueryUseElapsedTime();
    
    //Chuck: Gambling Race Stuff
    void SetRaceEnteryFee(int coins);
    void PutMFPlayerInCar();

    //Chuck:Adding these for Forced Car presentation clean up.
    void SwapInDefaultCar();
    void SwapInDefaultCarStart();
    void OnProcessRequestsComplete( void* pUserData );
    void SetSwapPlayerRespawnLocatorName(char* locatorName);
    void SetSwapDefaultCarRespawnLocatorName(char* locatorName);
    void SetSwapForcedCarRespawnLocatorName(char* locatorName);

    //Call this to reset a player to this spawn pt once per stage.
    void SetPlayerRespawnLocatorName(char* locatorName);
    void SetmsPlayerCarRespawnLocatorName(char* locatorName);

    //used to hide one character per stage, used for the bart abuduction on l1m7.
    void SetCharacterToHide(char* charactername);
    //script to trigger leaving the level
    void SetLevelOver();
    void SetGameOver(bool IsGameOver = true) { mbGameOver = IsGameOver; }

    struct CountdownSequenceUnit
    {
        char textID[ 64 ];
        int durationTime;
    };

    void SetCountdownEnabled( radKey32 dialogID, tUID secondSpeakerUID );
    void AddCountdownSequenceUnit( const char* textID, int durationTime = 1000 );
    CountdownSequenceUnit* GetCountdownSequenceUnit( int index ) const;
    radKey32 GetCountdownDialogID() const { return mCountdownDialogKey; };
    tUID GetCountdownSecondSpeakerUID() const { return mSecondSpeakerUID; };

    inline bool StartBonusObjective() const { return mStartBonusObjectives;  };
    inline void MakeBonusObjectiveStart() { mIsBonusObjectiveStart = true; };

    struct LockRequirement
    {
        enum Type { SKIN, CAR, NONE };
        enum { MAX_NAME_LEN = 32 };
        LockRequirement() : mType( NONE ) { mName[0] = '\0'; };
        Type mType;
        char mName[MAX_NAME_LEN + 1];
    };

    void SetLockRequirement( unsigned int which, LockRequirement::Type type, const char* name );
    const LockRequirement& GetLockRequirement( unsigned int which );
    bool GetMissionLocked() const { return mMissionLocked; };

    void DisplayMissionStageIndexMessage();

    // stage complete presentation
    //
    void ShowStageComplete( bool enabled ) { mShowStageComplete = enabled; }
    bool IsShowStageComplete() const { return mShowStageComplete; }

    // HUD icon image
    //
    void SetHUDIcon( const char* name );
    const char* GetHUDIcon() const;

    void SetIrisAtEnd() { mIrisAtEnd = true; };
    void SetFadeOutAtEnd() { mFadeOutAtEnd = true; };
    void SetIrisSpeed( float speed ) { mIrisSpeed = speed; };

    enum Transition { IRIS, FADE, NONE };
    Transition GetTransition() { if ( mIrisAtEnd ) { return IRIS; } else if ( mFadeOutAtEnd ) { return FADE; } else { return NONE; } };
    void DoTransition();

    void SetMissionAbortEnabled( bool isEnabled );
    bool IsMissionAbortAllowed() const;

    //Chuck dirty pulbic var
    bool mbStayBlackForStage;
    bool mbDisablePlayerControlForCountDown;
    
    Vehicle* GetMainAIVehicleForThisStage();
    
    static const int MAX_VEHICLES = 4;

protected:
    void ActivateVehicle( Vehicle* vehicle, bool bIsActive );
    void OnStageCompleteSuccessful();
private:
#ifdef RAD_DEBUG
    int m_Id;
#endif
    MissionStageState mState;
	int DestroyStageVehicleAI();
    void DestroyAllSafeZones ();

    //
    // Play dialog associated with stage completion if it exists
    //
    void triggerStageDialog();

    //
    // Objective (always only one per stage)
    //
    MissionObjective* mObjective;

    //
    // Conditions
    //
    static const int MAX_CONDITIONS = 8;

    int mNumConditions;
    MissionCondition* mConditions[ MAX_CONDITIONS ];

    //
    // Time for stage
    //
    StageTimeType mStageTimeType;
    int mStageTime;

    //
    // AI Vehicles
    //
    struct VehicleInfo
    {
        VehicleInfo() : vehicle( NULL ), spawn( NULL ), vehicleAINum( -1 ), vehicleAI( NULL ) {};
        Vehicle* vehicle;
        CarStartLocator* spawn;
        int vehicleAINum;//Stupid.
        VehicleAI* vehicleAI;
    };


    int mNumVehicles;
    VehicleInfo mVehicles[ MAX_VEHICLES ];

    //
    // Waypoints
    //
    int mNumWaypoints;
    Locator* mWaypoints[ WaypointAI::MAX_WAYPOINTS ];

    //
    // Characters
    //
    struct CharacterInfo
    {
        CharacterInfo() : character( NULL ), locator( NULL ), carLocator( NULL ), pZoneEventLocator( NULL ), vehicle( NULL ) { name[0] = '\0'; };
        char name[16];
        Character* character;
        CarStartLocator* locator;
        CarStartLocator* carLocator;
        ZoneEventLocator* pZoneEventLocator;
        Vehicle* vehicle;
        char VehicleName[16];
    };

    enum { MAX_CHARACTERS_IN_STAGE = 6 };

    int mNumCharacters;
    CharacterInfo mCharacters[ MAX_CHARACTERS_IN_STAGE ];

    char mCharacterToHide [16];
    bool mbLevelOver : 1;
    bool mbGameOver : 1;
    bool mbFinalStage : 1;

    int miNameIndex;
    int miStartMessageIndex;
    bool mbShowMessage;

    struct CameraInfo
    {
        CameraInfo() : type ( SuperCam::FOLLOW_CAM ), cut( false ), quickTransition( false ), active( false ) {};
        SuperCam::Type type;
        bool cut;
        bool quickTransition;
        bool active;
    };

    CameraInfo mCamInfo;

    //
    // Interactive music properties
    //
    bool mMusicChange;
    bool mKeepMusicOn;
    radKey32 mMusicEventKey;
    radKey32 mMusicStateKey;
    radKey32 mMusicStateEventKey;

    //
    // Dialog properties
    //
    radKey32 mDialogEventKey;
    tUID mConversationCharacterKey;
	
	//used to store any chase vehicle data that is for this stage
	// more harrass AI etc
	struct ChaseVehicleStruct
	{
		unsigned int mChaseVehicleSpawnRate;
		char vehiclename [64] ;
	};

	ChaseVehicleStruct m_ChaseData_Array [MAX_CHASE_STRUCTS] ;

    SafeZone* m_SafeZone_Array [MAX_SAFEZONES];
    
    //Chuck Traffic Variable

    bool mbClearTrafficForStage;
    bool mbNoTrafficForStage;

    bool mb_DisableHitAndRun;
    bool mb_InsideSafeZone;
    
    bool mb_UseElapsedTime; //used to make the time count up rather than down
    int mRaceEnteryFee;
    bool mbPutPlayerInCar;
    bool mbRacePaidOut;


    //Chuck: Variables for Forced car clean up
    bool mbSwapInDefaultCar;
    char mSwapDefaultCarRespawnLocatorName [32];
    char mSwapForcedCarRespawnLocatorName [32];
    char mSwapPlayerRespawnLocatorName [32];
         
    //variable used for placing a player per stage basis
    char mPlayerRespawnLocatorName [32];
    char mPlayerCarRespawnLocatorName [32];

    char mTrafficDensity;
    bool mIsBonusObjectiveStart;
    bool mStartBonusObjectives;

    LockRequirement mRequirement[ MAX_LOCK_REQUIREMENTS ];
    bool mMissionLocked;

    bool mShowStageComplete : 1;
    bool mCountdownEnabled  : 1;
    char mHUDIconImage[ 16 ];

    bool mIrisAtEnd;
    bool mFadeOutAtEnd;
    float mIrisSpeed;

    static const int MAX_NUM_COUNTDOWN_SEQUENCE_UNITS = 8;

    CountdownSequenceUnit* mCountdownSequenceUnits;
    int mNumCountdownSequenceUnits;

    radKey32 mCountdownDialogKey;
    tUID mSecondSpeakerUID;

    bool mAllowMissionAbort : 1;
    int mResetCounter;

};

//=============================================================================
// MissionStage::SetFinalStage
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool bIsFinal )
//
// Return:      void 
//
//=============================================================================
inline void MissionStage::SetFinalStage( bool bIsFinal )
{
    mbFinalStage = bIsFinal;
}

//=============================================================================
// MissionStage::GetFinalStage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool MissionStage::GetFinalStage()
{
    return mbFinalStage;
}

//=============================================================================
// MissionStage::GetObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MissionObjective
//
//=============================================================================
inline MissionObjective* MissionStage::GetObjective() 
{ 
    return ( mObjective ); 
}

//=============================================================================
// MissionStage::SetObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( MissionObjective* objective )
//
// Return:      void 
//
//=============================================================================
inline void MissionStage::SetObjective( MissionObjective* objective ) 
{ 
    mObjective = objective; 
}

//=============================================================================
// MissionStage::GetNumConditions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline unsigned int MissionStage::GetNumConditions() 
{ 
    return ( mNumConditions ); 
}

//=============================================================================
// MissionStage::SetNumConditions
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int num )
//
// Return:      inline 
//
//=============================================================================
inline void MissionStage::SetNumConditions( unsigned int num ) 
{ 
    mNumConditions = num; 
}

//=============================================================================
// MissionStage::GetCondition
//=============================================================================
// Description: Comment
//
// Parameters:  (unsigned int index)
//
// Return:      inline 
//
//=============================================================================
inline MissionCondition* MissionStage::GetCondition( int index )
{ 
    rAssert( index < MAX_CONDITIONS );
    return ( mConditions[ index ] ); 
}

//=============================================================================
// MissionStage::SetCondition
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index, MissionCondition* condition )
//
// Return:      void 
//
//=============================================================================
inline void MissionStage::SetCondition( int index, MissionCondition* condition ) 
{ 
    rAssert( index < MAX_CONDITIONS );
    mConditions[ index ] = condition; 
}

//=============================================================================
// MissionStage::SetNameIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      void 
//
//=============================================================================
inline void MissionStage::SetNameIndex( int index )
{
    miNameIndex = index;
}

//=============================================================================
// MissionStage::SetStartMessageIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      void 
//
//=============================================================================
inline void MissionStage::SetStartMessageIndex( int index )
{
    miStartMessageIndex = index;
}

//=============================================================================
// MissionStage::GetStartMessageIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline int MissionStage::GetStartMessageIndex()
{
    return miStartMessageIndex;
}

//=============================================================================
// MissionStage::SetHUDIcon
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      void 
//
//=============================================================================
inline void MissionStage::SetHUDIcon( const char* name )
{
    sprintf( mHUDIconImage, "%s.png", name );
    rAssert( strlen( mHUDIconImage ) < sizeof( mHUDIconImage ) );
}

//=============================================================================
// MissionStage::GetHUDIcon
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline const char* MissionStage::GetHUDIcon() const
{
    return mHUDIconImage;
}

//=============================================================================
// MissionStage::SetMissionAbortEnabled
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void MissionStage::SetMissionAbortEnabled( bool isEnabled )
{
    mAllowMissionAbort = isEnabled;
}

//=============================================================================
// MissionStage::IsMissionAbortAllowed
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool MissionStage::IsMissionAbortAllowed() const
{
    return mAllowMissionAbort;
}

#endif //MISSIONSTAGE_H
