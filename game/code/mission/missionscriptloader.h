//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        missionscriptloader.h
//
// Description: Blahblahblah
//
// History:     12/06/2002 + Created -- NAME
//
//=============================================================================

#ifndef MISSIONSCRIPTLOADER_H
#define MISSIONSCRIPTLOADER_H

//========================================
// Nested Includes
//========================================

#include <loading/filehandlerenum.h>
#include <loading/loadingmanager.h>

#include <memory/srrmemory.h>

#include <mission/objectives/missionobjective.h>
#include <mission/conditions/missioncondition.h>
#include <worldsim/ped/pedestrianmanager.h>

#include <console/console.h>
#include <constants/directionalarrowenum.h>


//========================================
// Forward References
//========================================

class TrafficModelGroup;
class EventLocator;
class Mission;
class MissionStage;
class VehicleAI;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class MissionScriptLoader : public Console::ExecuteScriptCallback
{
public:
    // Static Methods for accessing this singleton.
    static MissionScriptLoader* CreateInstance();
    static MissionScriptLoader* GetInstance();
    static void DestroyInstance();

    void Register();

    void LoadScript( char* filename );
    void LoadScriptAsync( char* filename, LoadingManager::ProcessRequestsCallback* callback = NULL );

    void SetFileHander( FileHandlerEnum handler ) { mFileHandler = handler; }
    
    void OnExecuteScriptComplete( void* pUserData );

    //Making ptr to this Struct public cuz of dusits damn structs
    PedestrianManager::ModelGroup* mp_ModelGroup;
    int mPedGroupID;

    TrafficModelGroup* mp_TrafficGroup;

private:
    MissionScriptLoader();
    virtual ~MissionScriptLoader();

    //Prevent wasteful constructor creation.
    MissionScriptLoader( const MissionScriptLoader& missionscriptloader );
    MissionScriptLoader& operator=( const MissionScriptLoader& missionscriptloader );

    //
    // *** for GameCube only ***
    //
    static void SetLanguage( int argc, char** argv );

    //
    // Level functions
    //
    static void InitLevelPlayerVehicle( int argc, char** argv );
    static void PlacePlayerCar( int argc, char** argv );
    /*
	static void DisableTraffic (int argc, char** argv );
	static void EnableTraffic (int argc, char** argv );
    */
    static void AddPurchaseCarReward( int argc, char** argv );
    static void SetPostLevelFMV( int argc, char** argv );

    static void AddGlobalProp(int argc,char** argv );
    
    //Ped Group 
    static void CreatePedGroup(int argc, char** argv );
    static void AddPed(int argc, char** argv );
    static void ClosePedGroup (int argc, char** argv );
    static void UsePedGroup( int argc, char** argv );
    
    //Reward Binding
    static void BindReward(int argc, char** argv);
    static void SetCarAttributes(int argc, char** argv);

    //Wasps and Gags (in rewards script)
    static void SetTotalGags(int argc, char** argv);
    static void SetTotalWasps(int argc, char** argv);

    //Traffic Group
    static void CreateTrafficGroup(int argc, char** argv);
    static void AddTrafficModel(int argc, char** argv);
    static void CloseTrafficGroup(int argc, char** argv);

	//
	//Chase/Harrass AI Car functions
	//
	static void CreateChaseManager(int argc ,char** argv );
    static void DisableHitAndRun(int argc,char** argv );
    static void EnableHitAndRun(int argc,char** argv );
    static void SetHitAndRunMeter(int argc,char** argv );
    static void SetNumChaseCars(int argc,char** argv );
	static void SetChaseSpawnRate(int argc, char** argv );
	static void KillAllChaseAI(int argc,char** argv);
    static void ResetHitAndRun(int argc,char** argv );
    static void SetHitAndRunDecay(int argc,char** argv );
    static void SetHitAndRunDecayInterior(int argc,char** argv );

    //
    // Mission functions
    //
    static void AddMission( int argc, char** argv );
    static void AddBonusMission( int argc, char** argv );
    static void SelectMission( int argc, char** argv );
    static void SetMissionNameIndex( int argc, char** argv );
    //static void InitMissionPlayerVehicle( int argc, char** argv );
    static void SetMissionResetPlayerInCar( int argc, char** argv );
    static void SetMissionResetPlayerOutCar( int argc, char** argv );
    static void SetDynaLoadData( int argc, char** argv );
    static void AddBonusObjective( int argc, char** argv );
    static void SetForcedCar( int argc, char** argv );
    static void CloseMission( int argc, char** argv );
    static void SetDemoLoopTime( int argc, char** argv );  //This isn't really dependant on the mission.
    static void StreetRacePropsLoad(int argc, char** argv);
    static void StreetRacePropsUnload(int argc, char** argv);
    static void UseElapsedTime(int argc,char** argv);
    static void AttachStatePropCollectible(int argc,char** argv);
    static void ShowHUD(int argc,char** argv);
    static void SetNumValidFailureHints(int argc,char** argv);

    //
    // Mission Stage functions
    //
    static void AddStage( int argc, char** argv );
    static void SetStageMessageIndex( int argc, char** argv );
    static void SetStageTime( int argc, char** argv );
    static void AddStageTime( int argc, char** argv );
    static void AddStageVehicle( int argc, char** argv );	
    static void AddStageWaypoint( int argc, char** argv );
    static void AddStageCharacter( int argc, char** argv );
    static void AddStageMusicChange( int argc, char** argv );
    static void SetStageMusicAlwaysOn( int argc, char** argv );
    static void SetStageStartMusicEvent( int argc, char** argv );
    static void SetCompletionDialog( int argc, char** argv );
    static void SetMusicState( int argc, char** argv );
    static void SetStageCamera( int argc, char** argv );
	static void MoveStageVehicle( int argc, char** argv );
	static void ActivateVehicle(int argc, char** argv );
    static void ResetToThisStage( int argc, char** argv );
    static void SetTrafficDensity( int argc, char** argv );
    static void AddSafeZone ( int argc, char** argv ); 
    static void SetBonusMissionStart( int argc, char** argv );
    static void ShowStageComplete( int argc, char** argv );
    static void SetHUDIcon( int argc, char** argv );
    static void SetIrisWipe( int argc, char** argv );
    static void SetFadeOut( int argc, char** argv );
    static void CloseStage( int argc, char** argv );
    static void SetVehicleAIParams( int argc, char** argv );
    static void SetCoinFee(int argc,char** argv);
    static void PutMFPlayerInCar(int argc,char** argv);
    static void SetStatepropShadow(int argc,char** argv);
    static void SwapInDefaultCar(int argc,char** argv);
    static void SetSwapPlayerRespawnLocatorName(int argc,char** argv);
    static void SetSwapDefaultCarRespawnLocatorName(int argc,char** argv);
    static void SetSwapForcedCarRespawnLocatorName(int argc,char** argv);
    static void NoTrafficForStage(int argc,char** argv);
    static void ClearTrafficForStage(int argc,char** argv);
    static void PlacePlayerAtLocatorName(int argc,char** argv);
    static void msPlacePlayerCarAtLocatorName(int argc,char** argv);
    static void SetCharacterToHide(int argc,char** argv);
    static void SetLevelOver(int argc,char** argv);
    static void SetGameOver(int argc, char** argv);
    static void StayInBlack(int argc,char** argv);
    static void AllowMissionAbort(int argc,char** argv);

    static void SetStageAIRaceCatchupParams( int argc, char** argv );
    static void SetStageAIEvadeCatchupParams( int argc, char** argv );
    static void SetStageAITargetCatchupParams( int argc, char** argv );


	//Gamble Race Specific Commands
	static void SetParTime(int argc, char** argv);
    static void SetRaceEnteryFee(int argc, char** argv);

    //
    // Objective methods
    //
    static void AddObjective( int argc, char** argv );
    static void AddCollectible( int argc, char** argv );
    static void AddCollectibleStateProp( int argc, char** argv );
    static void SetCollectibleEffect( int argc, char** argv );
    static void SetDestination( int argc, char** argv );
    static void SetObjTargetVehicle( int argc, char** argv );
    static void SetObjTargetBoss( int argc, char** argv );
    static void SetPickupTarget( int argc, char** argv );
    static void SetObjDistance( int argc, char** argv );
    static void AddNPC( int argc, char** argv );
    static void RemoveNPC( int argc, char** argv );
    static void AddDriver( int argc, char** argv );
    static void RemoveDriver( int argc, char** argv );
    static void SetTalkToTarget( int argc, char** argv );
    static void SetDialogueInfo( int argc, char** argv );
    static void SetDialoguePositions( int argc, char** argv );
    static void SetFMVInfo( int argc, char** argv );
    static void SetRaceLaps( int argc, char** argv );
    static void BindCollectibleToWaypoint( int argc, char** argv );
    static void AllowUserDump( int argc, char** argv );
    static void SetVehicleToLoad( int argc, char** argv );
    static void AllowRockOut( int argc, char** argv );
    static void TurnGotoDialogOff( int argc, char** argv );
    static void SetPauseDuration(int argc,char** argv );
    static void MustActionTrigger( int argc, char** argv );
    static void CloseObjective( int argc, char** argv );

    //
    // Condition methods
    //
    static void AddCondition( int argc, char** argv );
    static void SetCondTargetVehicle( int argc, char** argv );
    static void SetFollowDistances( int argc, char** argv );
    static void SetCondMinHealth( int argc, char** argv );
    static void SetConditionPosition( int argc, char** argv );
    static void SetCondTime( int argc, char** argv );
    static void SetHitNRun( int argc, char** argv );
    static void CloseCondition( int argc, char** argv );
    
    //
    // Conversation/Presentation methods
    //
    static void CharacterIsChild          ( int argc, char** argv );
    static void SetConversationCamName    ( int argc, char** argv );
    static void SetConversationCamPcName  ( int argc, char** argv );
    static void SetConversationCamNpcName ( int argc, char** argv );
    static void SetConversationCam        ( int argc, char** argv );
    static void SetConversationCamDistance( int argc, char** argv );
    static void SetConversationCamAngle   ( int argc, char** argv );
    static void SetPresentationBitmap     ( int argc, char** argv );
    static void EnableTutorialMode        ( int argc, char** argv );
    static void AmbientAnimationRandomize ( int argc, char** argv );
    static void ClearAmbientAnimations    ( int argc, char** argv );
    static void AddAmbientNpcAnimation    ( int argc, char** argv );
    static void AddAmbientPcAnimation     ( int argc, char** argv );
    static void SetAnimatedCameraMulticontrollerName( int argc, char** argv );
    static void SetAnimatedCameraName     ( int argc, char** argv );
    static void SetMissionStartMulticontrollerName( int argc, char** argv );
    static void SetMissionStartCameraName ( int argc, char** argv );
    static void SetCamBestSide            ( int argc, char** argv );
    static void StartCountdown            ( int argc, char** argv );
    static void AddToCountdownSequence    ( int argc, char** argv );
    static void SetCarStartCamera         ( int argc, char** argv );
    static void GoToPattyAndSelmaScreenWhenDone( int argc, char** argv );

    //SuperSprint methods
    static void SetPlayerCarName( int argc, char** argv );

    //
    // Loading methods
    //

public:
    //Use to load cars that can be unloaded from memory easily. like the Default Car and Forced cars, and AI cars.
    static void LoadDisposableCar(int argc, char** argv );

     //spew text on the screen in for Ps2 
    static void TreeOfWoeErrorMsg(const char* outputbuffer);


private:
    static void LoadP3DFile( int argc, char** argv );
    static void AddCharacter( int argc, char** argv );
    static void AddNPCCharacterBonusMission( int argc, char** argv );
    static void SetBonusMissionDialoguePositions( int argc, char** argv );
    static void AddAmbientCharacter( int argc, char** argv );

    // adding waypoints for NPCs to walk on
    static void AddBonusMissionNPCWaypoint( int argc, char** argv );
    static void AddObjectiveNPCWaypoint( int argc, char** argv ); // syntax: name locatorName 
    static void AddAmbientNPCWaypoint( int argc, char** argv );
    static void AddPurchaseCarNPCWaypoint( int argc, char** argv );

    static void ActivateTrigger( int argc, char** argv );
    static void DeactivateTrigger( int argc, char** argv );

    static void CreateAnimPhysObject( int argc, char** argv );
    static void CreateActionEventTrigger( int argc, char** argv );
    static void LinkActionToObjectJoint( int argc, char** argv );
    static void LinkActionToObject( int argc, char** argv );
    static void SetCoinDrawable( int argc, char** argv );
    static void SetParticleTexture( int argc, char** argv );

    //used to set respawnrate of wrenches and nitro etc. 
    static void SetRespawnRate (int argc,char** argv );
    
   

    //
    // Helper methods
    //
    static bool AddNPCWaypoint( const char* npcName, const char* locName );
    Locator* GetLocator( char* name );
    Vehicle* GetVehicleByName( const char* name );
    // Return the type of directional arrow from the given string
    // returns true if the arrow is valid, false if not
    static bool GetDirectionalArrowType( const char* arg, DirectionalArrowEnum::TYPE* outArrowType );

    GameMemoryAllocator mMissionHeap;

    Mission*            mpMission;
    MissionStage*       mpStage;
    MissionObjective*   mpObjective;
    MissionObjective::ObjectiveTypeEnum mObjType;
    MissionCondition*   mpCondition;
    MissionCondition::ConditionTypeEnum mCondType;

    FileHandlerEnum     mFileHandler;
    
    bool mFirstObjective;


    static MissionScriptLoader* spInstance;
};

// A little syntactic sugar for getting at this singleton.
inline MissionScriptLoader* GetMissionScriptLoader() { return( MissionScriptLoader::GetInstance() ); }


#endif //MISSIONSCRIPTLOADER_H

