//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gameplaymgr.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef GAMEPLAYMGR_H
#define GAMEPLAYMGR_H

//========================================
// Nested Includes
//========================================

#include <constants/maxplayers.h>
#include <events/eventlistener.h>
#include <loading/loadingmanager.h>
#include <memory/srrmemory.h>
#include <render/enums/renderenums.h>
#include <mission/mission.h>
#include <mission/bonusmissioninfo.h>
#include <mission/respawnmanager/respawnmanager.h>
#include <presentation/presevents/presentationevent.h>
//========================================
// Forward References
//========================================

class RespawnManager;
class ChaseManager;
class Character;
class Vehicle;
class CarStartLocator;

//========================================
//Constants
//========================================



//=============================================================================
//
// Synopsis:    Base class for all gameplay types (e.g. H2H or Missions)
//
//=============================================================================


class GameplayManager : public EventListener,
                        public PresentationEvent::PresentationEventCallBack
{
public:
    static GameplayManager* GetInstance();
    static void SetInstance( GameplayManager* pInstance );

    static const int MAX_MISSIONS = 20;
    static const int MAX_BONUS_MISSIONS = 10;
    static const int MAX_LEVELS = 8;
	static const int MAX_CHASEMANAGERS = 1;
    static const int MAX_VDU_CARS = 10;

    // DEMO MODE YA!
    bool mIsDemo;

    enum GameTypeEnum
    {
        GT_NORMAL,
        GT_SUPERSPRINT,
        NUM_GAMETYPES
    };


    // Guess what?  No matter your rating, you suck!
    //
    enum RatingEnum 
    {
	    RATING_DNF,
	    RATING_BRONZE,
	    RATING_SILVER,
	    RATING_GOLD,
	    NUM_RATINGS
    };

 
	struct ChaseManagerStruct
	{
		ChaseManager* mp_chasemanager;
		char hostvehicle [16];
        char hostvehiclefilename[64];
	};


    //Chuck: use this to record the starting location of a player and Forced car prior
    //to player start the forced car mission, we save this so we can place player in safe
    //spot on mission cancel from Pause menu, so car is spawn on top of them.
    struct PlayerAndCarInfo
    {
        rmt::Vector mPlayerPosition;
        rmt::Vector mForceLocation;
        bool mbDirtyFlag; //we use this to signal that we just started a forced car mission and any retries and restarts do record the character position again
    };


    PlayerAndCarInfo mPlayerAndCarInfo;




    struct VDUStruct
    {
        Vehicle* mpVehicle[MAX_VDU_CARS];
        int mCounter;
    };

    // Data about the currently loaded level.
    //
    struct LevelDataEnum
    {
        LevelDataEnum() : level( (RenderEnums::LevelEnum)(RenderEnums::numLevels - 1) ), numMissions( 0 ), numBonusCollectibles( 0 ), numBonusCollected( 0 ), mission( (RenderEnums::MissionEnum)(RenderEnums::numMissions - 1) ) {};
        RenderEnums::LevelEnum level;

        // missions in this level
	    int numMissions;  

        // The single player rating of each mission
	    RatingEnum rating[ MAX_MISSIONS ];

        // Number of bonus collectibles available in this level
        int numBonusCollectibles;
        // Number the single player has collected
        int numBonusCollected;

        RenderEnums::MissionEnum mission;
    };

    /////////////////////////////////////////////////
    // Public accessor type stuff
    /////////////////////////////////////////////////

    int GetNumPlayers();

    //methods for disabling phonebooths since there are sometime 
    //like get into vehicle objective that we dont want user switching cars or in forced car missions
	void EnablePhoneBooths ();
	void DisablePhoneBooths ();
	bool QueryPhoneBoothsEnabled ();


	//getting access to the ChaseManagers

	//gets ChaseManager quering by the hostvehicle name
	ChaseManager* GetChaseManager( char* hostvehicle);
    ChaseManager* GetChaseManager(int index);

	//creates a chase manager, returns -1 if function fails
	int CreateChaseManager ( char* hostvehicle,char* confile, int spawnrate);
    
	//method to clear all chase cars that are active.
	void ClearAllChaseCars ( );
	
	//Kills the chase managers
	void KillAllChaseManagers ( );

    //Get the RespawnManager for wrenches and nitro etc.
    RespawnManager* GetRespawnManager();


	// the heap the current mission data uses
    GameMemoryAllocator GetCurrentMissionHeap();

    GameMemoryAllocator GetMissionHeap( int index );

    int GetNumMissions();
    int GetNumBonusMissions();

    Mission* GetMission( int index );

    virtual Mission* GetCurrentMission();
    const BonusMissionInfo* GetCurrentBonusMissionInfo() const;
    BonusMissionInfo* GetBonusMissionInfo( int missionNumber );
    int GetCurrentMissionIndex();
    int GetCurrentMissionNum() const;

    virtual void CleanMissionData() = 0;

    RenderEnums::LevelEnum GetCurrentLevelIndex();
    RenderEnums::MissionEnum GetCurrentMissionForDemo();

    virtual bool IsSundayDrive();
    virtual bool IsSuperSprint();

    bool TestPosInFrustrumOfPlayer( const rmt::Vector& pos, int playerID, float radius = 3.0f );

    //
    // Tells the manager that presentation stuff is done and it
    // should move on with gameplay
    //
    void ContinueGameplay();
    
    /////////////////////////////////////////////////
    // Load the next level
    // Use this once and only once for a gameplay session
    // The name is irrevelevant when the gameplay is head to head
    /////////////////////////////////////////////////

    virtual void SetLevelIndex( RenderEnums::LevelEnum level );
    virtual void SetMissionIndex( RenderEnums::MissionEnum mission );


    /////////////////////////////////////////////////
    // MS9: Temporarily set the from the main menu
    // -1 is the mission car, 0->n overrides
    //
    //int mVehicleIndex;
    //void SetVehicleIndex( int vehicleId ) { mVehicleIndex = vehicleId; }
    //int GetVehicleIndex() { return mVehicleIndex; }
    //Vehicle* InitVehicle( char* vehicleName );
    //void GetOverrideVehicleName( char* name );
    
    int mCharacterIndex;
    void SetCharacterIndex( int characterId ) { mCharacterIndex = characterId; }
    int GetCharacterIndex() { return mCharacterIndex; }
    void GetOverrideCharacterName( char* name );
    /////////////////////////////////////////////////

    /////////////////////////////////////////////////
    // tells the GPM to load a file and read a script defining which
    // files need to be loaded for this level
    virtual void LoadLevelData() = 0;
    virtual void InitLevelData() = 0;
    
    /////////////////////////////////////////////////
    // Service the GPM loading process for missions
    //
    virtual void PerformLoading() = 0;

    /////////////////////////////////////////////////
    // Allows you to set the position of a vehicle
    // TO DO: Extend this to place ANY DSG object
    /////////////////////////////////////////////////

    void PlaceCharacterAtLocator( Character* character, Locator* locator );

    void PlaceVehicleAtLocator( Vehicle* vehicle, CarStartLocator* locator );
    void PlaceVehicleAtLocation( Vehicle* vehicle, rmt::Vector pos, float rotation );
    void PlaceVehicleAtLocatorName( Vehicle* vehicle, const char* locatorName );

    /////////////////////////////////////////////////
    // Flow stuff, don't use this
    /////////////////////////////////////////////////
    
    // call this when all the other level data is done loading
    // and the gameplay manager will load the mission scripts
    virtual void LevelLoaded();

    // must be called to set shit up
	virtual void Initialize();
    // call this before destroying the GPM
    virtual void Finalize();
    // call this to start gameplay again
    virtual void Reset() = 0;

    // call Update every frame
    virtual void Update( int elapsedTime );

    void SetNumMissions( int num );
    void SetNumBonusMissions( int num );

    void SetMission( int index, Mission* mission );

    virtual void HandleEvent( EventEnum id, void* pEventData );
    virtual void RestartCurrentMission();
    virtual void RestartToMission( RenderEnums::MissionEnum mission );
    virtual void AbortCurrentMission();

    // SetBonusMissionInfo
    // Optional parameter isCompleted - bonus missions can be completed but still 
    // playable, but the animated icons they use are the alternative.
    void SetBonusMissionInfo( const char* NPCname, const char* missionName, const char* iconName, const char* dialogueName, bool isOneShot, const char* alternateIconName = NULL, bool wasCompletedAlready = false );
    int GetMissionNumByName( const char* name );
    int GetBonusMissionNumByName( const char* name );

    void EnableBonusMissions();
    void DisableBonusMissions();
    void DisableAllChaseAI();
    void EnableAllChaseAI();

    bool IsBonusMissionDesired() const;
    void CancelBonusMission();

    // return true when the last mission has been completed
    bool GetLevelComplete();
    bool GetGameComplete( void ) const { return mGameComplete; }
    //char mDefaultVehicle [32];
    
    enum eCarSlots{ eDefaultCar,eOtherCar,eAICar};   


    /////////////////////////////////////////////////
    // Adds a level vehicle (a vehicle that is always present)
    //
    Vehicle* AddLevelVehicle( char* vehicleName,eCarSlots slot, char* confile);

    //returns a pointer to vehicle, query by name
    //Vehicle* GetVehicle(char* name);

    //returns a pointer to the car that the PC last used/is using
    Vehicle* GetCurrentVehicle(void);
    
    //sets the mCurrentVehicle ptr.
    void SetCurrentVehicle(Vehicle* vehicle);
    void UnregisterVehicleHUDIcon(void);

    //Removes the current car from the inventory and releases the vehicle
    void DumpCurrentCar();
    void DumpCurrentCarIfInSphere( const rmt::Sphere& s );
    
    void ClearVehicleSlot(eCarSlots slot);
    void ClearVehicleSlotIfInSphere( eCarSlots slot, const rmt::Sphere& s );
    // Dumps the car in the source slot, then copies the vehicle
    // in the source slot into the dest slot
    // Used by LoadDisposableCarAsync
    void CopyVehicleSlot( eCarSlots sourceSlot, eCarSlots destSlot );
    char* GetVehicleSlotFilename(eCarSlots slot);
    char* GetVehicleSlotVehicleName(eCarSlots slot);
    Vehicle* GetVehicleInSlot(eCarSlots slot);

    // I guess we will need to call this afterall, in between each mission
    // call at the start of a new mission...
    void EmptyMissionVehicleSlots();

    int RemoveVehicleFromMissionVehicleSlots(Vehicle* pVehicle);
    
    struct CarDataStruct 
    {
        char filename [64];
        char name [32];
        rmt::Vector position;         //used to store the car position for unloading & reloading
        float heading;                      //also used to store cars facing for unloading & reloading

        Vehicle* mp_vehicle;
        
        Vehicle* pHuskVehicle;
        bool usingHusk;
    };

    static const int MAX_VEHICLE_SLOTS = 3;

    CarDataStruct mVehicleSlots [MAX_VEHICLE_SLOTS];   
    char mDefaultLevelVehicleName[64];
    char mDefaultLevelVehicleLocator[64];
    char mDefaultLevelVehicleConfile[64];
    bool mShouldLoadDefaultVehicle;

    // TODO greg - we don't actually need the AI slot in CarDataStruct


    // new
    // greg
    // jan 7, 2003
    
    // GameplayManager is now going to be the owner of "mission" cars.
    
    static const int MAX_MISSION_VEHICLE_SLOTS = 5;     // this has to have room for the "AI-heavyweight" car
    
    struct MissionCarDataStruct
    {
        Vehicle* vehicle;
        char name[32];      // for convenience
        //int vehicleCentralIndex;
        
        Vehicle* pHuskVehicle;
        bool usingHusk;
    };
    
    MissionCarDataStruct mMissionVehicleSlots[MAX_MISSION_VEHICLE_SLOTS];
  
    Vehicle* AddMissionVehicle(char* vehiclename, char* confile = 0, char* driver = 0);   

    // called from Mission::Reset
    void MakeSureHusksAreReverted(Vehicle* pvehicle = NULL );

    Vehicle* GetMissionVehicleByName( const char* name );
    Vehicle* GetUserVehicleByName(const char* name);
    int GetMissionVehicleIndex(Vehicle* vehicle);

    GameTypeEnum GetGameType();

    tColour GetControllerColour( int id );

    void PauseForIrisClose( float speedMod );
    void PauseForIrisOpen( float speedMod = 0.0f );
    void PauseForFadeToBlack( float speedMod );
    void PauseForFadeFromBlack( float speedMod = 0.0f );

    bool IsIrisClosed() { return mIrisClosed || mFadedToBlack; };

    void PutPlayerInCar( bool should ) { mPutPlayerInCar = should; };
    bool ShouldPutPlayerInCar() const { return mPutPlayerInCar; };


    

    //Chuck Moving this to public, bad design :-s but faster for missionscriptloader.
    ChaseManagerStruct m_ChaseManager_Array [ MAX_CHASEMANAGERS ];

    void SetPostLevelFMV( const char* FileName );
    const char* GetPostLevelFMV( void ) const { return mPostLevelFMV; }

    // advances sequentially to the next mission
    void NextMission();
    void PrevMission();

    void ResetIdleTime();
    void UpdateIdleTime( unsigned int elapsedTime );

     //Add cars to the Vehicle Disposal Unit for removal ,this prevents visual  popping.
    int AddToVDU(Vehicle* pvehicle);

    //Removes a Car slated for disposal, pass in a car name if the car is present it returns a ptr to that car else a NULL PTR
    void ReleaseFromVDU(char* carname, Vehicle**);

    //explicity removing cars that we are hangin on too, this should be called only in Finalize
    int MDKVDU();
    bool FadeInProgress() const
    {
        return mBlackScreenTimer > 0;
    }
    void AbortFade();
    void ManualControlFade(bool flag) 
    {
        mbManualControlFade = flag;
    }; 

    bool QueryManualControlFade()
    {
        return mbManualControlFade;
    };

    inline void SetLevelComplete()
    {
        mLevelComplete =true;
    }

    inline void SetGameComplete(void) { mGameComplete = true; }

    bool TestForContinuityErrorWithCar( Vehicle* v, bool aiSlot );  //If you are testing the ai slot you must be loading from phone.  Otherwise it will test the current car.

    int mBlackScreenTimer;

    //Try to break up the flow to allow for presentation.
    enum Message
    {
        NONE,
        NEXT_MISSION,
        PREV_MISSION
    };

    inline Message GetCurrentMessage() const { return mCurrentMessage; }

protected:
    GameplayManager();
    ~GameplayManager();

    virtual void LoadMission() = 0;

    virtual void OnPresentationEventBegin( PresentationEvent* pEvent ) {};
    virtual void OnPresentationEventLoadComplete( PresentationEvent* pEvent ) {};
    virtual void OnPresentationEventEnd( PresentationEvent* pEvent ) { mWaitingOnFMV = false; }

    void SetCurrentMission( int index );

    void RepairVehicle( CarDataStruct* carData );

    //
    // Only use this from SetLevelIndex in a subclass
    //
    void SetCurrentLevelIndex( RenderEnums::LevelEnum level );

    void SetNumPlayers( int numPlayers );

    char mSkipSunday;


    //Triage hack, only for demo mode. --dm 12/01/02
    void RemoveLevelVehicleController();
    int mAIIndex;

    GameTypeEnum mGameType;

    char mPostLevelFMV[ 13 ]; // Force 8.3 naming.
    bool mIrisClosed : 1;
    bool mFadedToBlack : 1;
    bool mWaitingOnFMV : 1;

private:
    GameplayManager( const GameplayManager& gameplayManager );
    GameplayManager& operator=( const GameplayManager& gameplayManager );

    // Pointer to the one and only instance of this singleton.
    static GameplayManager* spInstance;

    int mCurrentMission;

    int mNumPlayers;

    LevelDataEnum mLevelData;

    int mNumMissions;
    Mission* mMissions[ MAX_MISSIONS + MAX_BONUS_MISSIONS ];

    GameMemoryAllocator mCurrentMissionHeap;

    bool mLevelComplete : 1;
    bool mEnablePhoneBooths : 1;
    bool mGameComplete : 1;

    

    //static const int MAX_LEVEL_VEHICLES = 2;
    //Vehicle* mLevelVehicles[ MAX_LEVEL_VEHICLES ];
    //int miNumLevelVehicles;
    
    Vehicle* mCurrentVehicle; 
   
    
    //let the VDU release missions cars that arent in the players view, to avoid popping
    int UpdateVDU();   

    VDUStruct mVDU;

    
	

    //Bonus mission stuff.
    int mNumBonusMissions;
    int mCurrentBonusMission;
    int mDesiredBonusMission;
    bool mIsInBonusMission;
    bool mFireBonusMissionDialogue;
    bool mJumpToBonusMission;
    bool mUpdateBonusMissions;

    BonusMissionInfo mBonusMissions[ MAX_BONUS_MISSIONS ];

    Message mCurrentMessage;

    void DoNextMission();
    void DoPrevMission();

    //Chuck: Respawn ManagerStuff
    RespawnManager* mpRespawnManager;

    float mIrisSpeed;

    //This is a crappy way to force the player back in their car after a forced car mission.
    bool mPutPlayerInCar;

    //flag used to control the fader, used by the mission stage call back 
    //turn off once the loading is done
    bool mbManualControlFade;

    int mCurrentVehicleIconID;

    bool TestProximityToUsersCarAndNudgeUpIfNecessaryDamnUglyHack(rmt::Vector& pos, Vehicle* playersCar);

    unsigned int m_elapsedIdleTime;

};

// A little syntactic sugar for getting at this pseudo-singleton.
inline GameplayManager* GetGameplayManager() { return( GameplayManager::GetInstance() ); }
inline void SetGameplayManager( GameplayManager* pInstance ) { GameplayManager::SetInstance( pInstance ); }

//=============================================================================
// GameplayManager::GetNumPlayers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline int GameplayManager::GetNumPlayers() 
{ 
    return( mNumPlayers ); 
}

//=============================================================================
// GameplayManager::GetCurrentMissionHeap
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      GameMemoryAllocator 
//
//=============================================================================
inline GameMemoryAllocator GameplayManager::GetCurrentMissionHeap() 
{ 
    return( mCurrentMissionHeap ); 
}

//=============================================================================
// GameplayManager::GetNumMissions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline int GameplayManager::GetNumMissions() 
{ 
    return( mNumMissions ); 
}

//=============================================================================
// GameplayManager::GetNumBonusMissions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline int GameplayManager::GetNumBonusMissions() 
{ 
    return( mNumBonusMissions ); 
}

//=============================================================================
// GameplayManager::GetMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int index )
//
// Return:      Mission
//
//=============================================================================
inline Mission* GameplayManager::GetMission( int index ) 
{ 
    if ( (index >= 0) && (index < (MAX_MISSIONS + MAX_BONUS_MISSIONS)))
    {
        return( mMissions[ index ] );
    }
    else
    {
        rTuneAssertMsg(0,"Illegal array access in GameplayManager::GetMission()\n");
        return NULL;
    }

}

//=============================================================================
// GameplayManager::GetCurrentLevelIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      RenderEnums
//
//=============================================================================
inline RenderEnums::LevelEnum GameplayManager::GetCurrentLevelIndex() 
{ 
    if ( mLevelData.level >= RenderEnums::numLevels )
    {
        //This is a bonus game.
        return (RenderEnums::LevelEnum)(mLevelData.level - RenderEnums::numLevels);
    }

    return( mLevelData.level); 
}

//=============================================================================
// GameplayManager::SetNumMissions
//=============================================================================
// Description: Comment
//
// Parameters:  ( int num )
//
// Return:      void 
//
//=============================================================================
inline void GameplayManager::SetNumMissions( int num ) 
{ 
    mNumMissions = num; 
}

//=============================================================================
// GameplayManager::SetNumBonusMissions
//=============================================================================
// Description: Comment
//
// Parameters:  ( int num )
//
// Return:      inline 
//
//=============================================================================
inline void GameplayManager::SetNumBonusMissions( int num )
{
    mNumBonusMissions = num;
}

//=============================================================================
// GameplayManager::SetMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int index, Mission* mission )
//
// Return:      void 
//
//=============================================================================
inline void GameplayManager::SetMission( int index, Mission* mission ) 
{ 
    mMissions[ index ] = mission; 
}

//=============================================================================
// GameplayManager::GetLevelComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool GameplayManager::GetLevelComplete() 
{ 
    return( mLevelComplete );
}

//=============================================================================
// GameplayManager::SetNumPlayers
//=============================================================================
// Description: Comment
//
// Parameters:  ( int numPlayers )
//
// Return:      void 
//
//=============================================================================
inline void GameplayManager::SetNumPlayers( int numPlayers ) 
{ 
    mNumPlayers = numPlayers; 
}


//=============================================================================
// GameplayManager::GetCurrentMissionIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline int GameplayManager::GetCurrentMissionIndex()
{
    return( ( mCurrentMission - ( mCurrentMission & 0x00000001 ) ) ) >> 1; //Heh...
}

//=============================================================================
// GameplayManager::SetCurrentLevelIndex
//=============================================================================
// Description: Only use this in the subclass implementation of SetLevelIndex
//
// Parameters:  ( RenderEnums::LevelEnum level )
//
// Return:      void 
//
//=============================================================================
inline void GameplayManager::SetCurrentLevelIndex( RenderEnums::LevelEnum level )
{
    mLevelData.level = level;
}

//=============================================================================
// GameplayManager::GetCurrentMissionForDemo
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      RenderEnums
//
//=============================================================================
inline RenderEnums::MissionEnum GameplayManager::GetCurrentMissionForDemo()
{
    return mLevelData.mission;
}

//=============================================================================
// GameplayManager::IsSundayDrive
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool GameplayManager::IsSundayDrive()
{
    bool isSunday = false;

    if ( mCurrentMission == 0 )
    {
        isSunday = true;
    }
    
    return false;
}

//=============================================================================
// GameplayManager::IsSuperSprint
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline bool GameplayManager::IsSuperSprint()
{
    return false;
}

#endif //GAMEPLAYMGR_H
