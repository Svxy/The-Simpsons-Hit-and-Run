//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement MissionScriptLoader
//
// History:     12/06/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <math.h>
#include <string.h>
#include <radtextdisplay.hpp>

//========================================
// Project Includes
//========================================

#include <console/console.h>

#include <ai/vehicle/vehicleai.h>
#include <ai/vehicle/waypointai.h>
#include <ai/vehicle/chaseai.h>
#include <camera/animatedcam.h>
#include <camera/conversationcam.h>
#include <camera/supercam.h>
#include <camera/supercammanager.h>
#include <meta/carstartlocator.h>
#include <meta/eventlocator.h>
#include <meta/locator.h>

#include <mission/mission.h>
#include <mission/missionmanager.h>
#include <mission/missionscriptloader.h>
#include <mission/missionstage.h>
#include <mission/nodamagebonusobjective.h>
#include <mission/nocopbonusobjective.h>
#include <mission/timeremainbonusobjective.h>
#include <mission/racepositionbonusobjective.h>

#include <mission/conditions/vehiclecarryingstateprop.h>
#include <mission/conditions/damagecondition.h>
#include <mission/conditions/followcondition.h>
#include <mission/conditions/missioncondition.h>
#include <mission/conditions/outofboundscondition.h>
#include <mission/conditions/timeoutcondition.h>
#include <mission/conditions/racecondition.h>
#include <mission/conditions/leaveinteriorcondition.h>
#include <mission/conditions/positioncondition.h>
#include <mission/conditions/getoutofcarcondition.h>
#include <mission/conditions/notabductedcondition.h>
#include <mission/conditions/keepbarrelcondition.h>

#include <mission/objectives/loadvehicleobjective.h>
#include <mission/objectives/deliveryobjective.h>
#include <mission/objectives/destroyobjective.h>
#include <mission/objectives/followobjective.h>
#include <mission/objectives/gotoobjective.h>
#include <mission/objectives/loseobjective.h>
#include <mission/objectives/missionobjective.h>
#include <mission/objectives/raceobjective.h>
#include <mission/objectives/talktoobjective.h>
#include <mission/objectives/dialogueobjective.h>
#include <mission/objectives/getinobjective.h>
#include <mission/objectives/collectdumpedobjective.h>
#include <mission/objectives/fmvobjective.h>
#include <mission/objectives/interiorobjective.h>
#include <mission/objectives/coinobjective.h>
#include <mission/objectives/destroybossobjective.h>
#include <mission/objectives/pickupitemobjective.h>
#include <mission/objectives/timerobjective.h>
#include <mission/objectives/buycarobjective.h>
#include <mission/objectives/buyskinobjective.h>
#include <mission/objectives/gooutsideobjective.h>

#include <presentation/tutorialmanager.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/backend/guimanagerbackend.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>

#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/rewards/rewardsmanager.h>

#include <mission/animatedicon.h>

#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/rendermanager/renderlayer.h>

#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>

#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <ai/actionbuttonmanager.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/harass/chasemanager.h>
#include <worldsim/hitnrunmanager.h>

#include <presentation/presentation.h>
#include <mission/statepropcollectible.h>

#include <events/eventmanager.h>
#include <events/eventenum.h>

#include <camera/supercam.h>

#include <ai/actor/actormanager.h>

#include <stdlib.h>

#include <memory/srrmemory.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/traffic/trafficmodelgroup.h>
#include <worldsim/ped/pedestrianmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>

#include <loading/loadingmanager.h>
#include <mission/ufo/ufo.h>
#include <mission/ufo/tractorbeam.h>

#include <render/Loaders/AnimDynaPhysLoader.h>

#include <gameflow/gameflow.h>
#include <contexts/demo/democontext.h>
#include <contexts/gameplay/gameplaycontext.h>

#include <worldsim/parkedcars/parkedcarmanager.h>
#include <supersprint/supersprintmanager.h>

#include <meta/spheretriggervolume.h>
#include <meta/actioneventlocator.h>
#include <meta/triggervolumetracker.h>

#include <atc/atcmanager.h>
#include <stateprop/statepropdata.hpp>

#include <interiors/interiormanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
MissionScriptLoader* MissionScriptLoader::spInstance = 0;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// MissionScriptLoader::CreateInstance
//==============================================================================
//
// Description: Creates the MissionScriptLoader.
//
// Parameters:	None.
//
// Return:      Pointer to the MissionScriptLoader.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
MissionScriptLoader* MissionScriptLoader::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "MissionScriptLoader" );
    rAssert( spInstance == 0 );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

    spInstance = new MissionScriptLoader;
    rAssert( spInstance );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
    
    spInstance->Register();
    
MEMTRACK_POP_GROUP( "MissionScriptLoader" );
    return spInstance;
}

//==============================================================================
// MissionScriptLoader::GetInstance
//==============================================================================
//
// Description: - Access point for the MissionScriptLoader singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the MissionScriptLoader.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
MissionScriptLoader* MissionScriptLoader::GetInstance()
{
    rAssert( spInstance != 0 );
    
    return spInstance;
}


//==============================================================================
// MissionScriptLoader::DestroyInstance
//==============================================================================
//
// Description: Destroy the MissionScriptLoader.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void MissionScriptLoader::DestroyInstance()
{
    rAssert( spInstance != 0 );

    delete spInstance;
    spInstance = 0;
}

//==============================================================================
// MissionScriptLoader::MissionScriptLoader
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionScriptLoader::MissionScriptLoader() :
    mp_ModelGroup( NULL ),    
    mPedGroupID( -1 ),
    mp_TrafficGroup( NULL),
    mpMission( NULL ),
    mpStage( NULL ),
    mpObjective( NULL ),
    mpCondition( NULL ),
    mFirstObjective( true )
    
{

}

//==============================================================================
// MissionScriptLoader::~MissionScriptLoader
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionScriptLoader::~MissionScriptLoader()
{
}

//=============================================================================
// MissionScriptLoader::Register
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::Register()
{
    Console* console = GetConsole();

/*
 * TC: removing this, since it's not needed anymore; plus, there's no more room
 *     in the console to add any more functions (it's already maxed out)
 *
#ifdef PAL
    console->AddFunction( "SetLanguage", SetLanguage, "", 1, 1 );
#endif // PAL
*/

    //
    // Level methods
    //
    console->AddFunction( "InitLevelPlayerVehicle", InitLevelPlayerVehicle, "", 3, 4 ); //Inits a player vehicle and puts it at the locator ( carname, locatorname, slot, [.con] )
    console->AddFunction( "PlacePlayerCar", PlacePlayerCar, "", 2, 2 ); //Places a Player car at the locator (carname, locatorname)
    /*
	console->AddFunction( "EnableTraffic", EnableTraffic,
		"Traffic", 0, 0 );
	console->AddFunction( "DisableTraffic", DisableTraffic,
		"No Traffic", 0, 0 );
    */
    console->AddFunction( "AddPurchaseCarReward", AddPurchaseCarReward, "", 5, 6 ); //Add purchasable car reward (name), (character name), (choreo), (location), (float scale), [carstart]
    console->AddFunction( "SetPostLevelFMV", SetPostLevelFMV, "", 1, 1 ); //FMV to play before loading next level.

    console->AddFunction( "CreatePedGroup",CreatePedGroup, "",1,1); //Creates a ped group
    console->AddFunction( "AddPed",AddPed, "",2,2); //Adds a ped to a group
    console->AddFunction( "ClosePedGroup",ClosePedGroup, "",0,0); //Closes a group
    console->AddFunction( "UsePedGroup",UsePedGroup, "", 1,1 );  //Sets current ped group to a group

    console->AddFunction("BindReward",BindReward,"", 5, 7); //Binds a reward to a Quest

    console->AddFunction("CreateTrafficGroup",CreateTrafficGroup,"",1,1); //Creates a Traffic Group
    console->AddFunction("AddTrafficModel",AddTrafficModel, "",2,3); //Add a Traffic Model to a Group
    console->AddFunction("CloseTrafficGroup",CloseTrafficGroup, "",0,0); //Closes a Traffic Group

    console->AddFunction("SetCarAttributes",SetCarAttributes, "",5,5); //Set the Car Attributes displayed in the Phone Booth
    console->AddFunction("SetTotalGags",SetTotalGags, "",2,2); //Set the total number of gags for a level
    console->AddFunction("SetTotalWasps",SetTotalWasps, "",2,2); //Set the total number of wasps for a level
    console->AddFunction("AddGlobalProp",AddGlobalProp, "",1,1); //Adds a Prop the Master Global Prop Hash Table

    //Chase/HitnRun Methods
    console->AddFunction( "CreateChaseManager",CreateChaseManager, "",3,3); //Creates a ChaseManager for a host car
    console->AddFunction("DisableHitAndRun",DisableHitAndRun, "",0,0); //Disables Hit and Run Feature
    console->AddFunction("EnableHitAndRun",EnableHitAndRun, "",0,0); //Enables Hit and Run Feature
    console->AddFunction("SetHitAndRunMeter",SetHitAndRunMeter, "",1,1); //Set the value of the HitnRun meter
    console->AddFunction("SetNumChaseCars",SetNumChaseCars, "",1,1); //Set the Max Number of Chase Cars
    console->AddFunction( "SetChaseSpawnRate",SetChaseSpawnRate, "",2,2); //Sets the Spawn rate  for a host car
    console->AddFunction( "KillAllChaseAI",KillAllChaseAI, "",1,1); //Kills all Chase AI for a host car
    console->AddFunction( "ResetHitAndRun",ResetHitAndRun, "",0,0); //Resets HitnRun system
    console->AddFunction( "SetHitAndRunDecay",SetHitAndRunDecay, "",1,1); //Sets decay rate for HitnRun meter
    console->AddFunction( "SetHitAndRunDecayInterior",KillAllChaseAI, "",1,1); //Sets interior decay rate for HitnRun meter
    //
    // Mission methods
    //
    console->AddFunction( "AddMission", AddMission, "", 1, 1 ); //Adds a new mission and names it (Name)

    console->AddFunction( "AddBonusMission", AddBonusMission, "", 1, 1 ); //Adds a new bonus mission and names it (Name)

    console->AddFunction( "SetMissionNameIndex", SetMissionNameIndex, "", 1, 1 ); //Sets the index in the text bible (index)
    
    console->AddFunction( "SelectMission", SelectMission, "", 1, 1 ); //Selects a mission by name (Name)
    //console->AddFunction( "InitMissionPlayerVehicle", InitMissionPlayerVehicle, "", 1, 2 ); //Adds a player vehicle to this mission (vehiclename), [.con]

    console->AddFunction( "SetMissionResetPlayerInCar", SetMissionResetPlayerInCar, "", 1, 1 ); //This is where the player will go (in car) when the mission resets due to failure [locator]

    console->AddFunction( "SetMissionResetPlayerOutCar", SetMissionResetPlayerOutCar, "", 2, 2 ); //This is where the player will go (out of car) when the mission resets due to failure [player locator, vehicle locator]
    console->AddFunction( "SetDynaLoadData", SetDynaLoadData, "", 1, 2 ); //This is the string that defines what to load and dump..

    console->AddFunction( "AddBonusObjective", AddBonusObjective, "", 1, 2 ); //This adds a bonus objective to the current mission (type).

    console->AddFunction( "SetForcedCar", SetForcedCar, "", 0, 0 ); //This makes a mission a forced car mission

    console->AddFunction( "CloseMission", CloseMission, "", 0, 0 ); //Closes the current mission

    console->AddFunction( "SetDemoLoopTime", SetDemoLoopTime, "", 1, 1 ); //Sets the demo loop time only for demo loops

    console->AddFunction("StreetRacePropsLoad",StreetRacePropsLoad, "",1,1); //String that defines which file that contains the props for streetraces to load

    console->AddFunction("StreetRacePropsUnload",StreetRacePropsUnload, "",1,1); //String that defines which file that contains the props for streetraces to unload

    console->AddFunction("UseElapsedTime",UseElapsedTime, "",0,0); //Make this Mission timer count UP

    console->AddFunction("AttachStatePropCollectible",AttachStatePropCollectible, "" , 2, 2 ); //Attaches a collectible to the vehicle (vehicle name , statepropname )

    console->AddFunction("ShowHUD",ShowHUD, "" , 1, 1 ); //Sets visibility of in-game HUD

    console->AddFunction( "SetNumValidFailureHints", SetNumValidFailureHints, "", 1, 1 ); //Sets number of valid failure hints to be randomly chosen

    //
    // Stage methods
    // 
    console->AddFunction( "AddStage", AddStage, "", 0, 7 ); //Adds a new stage to the current mission ([final], [locked, skin|car, name], [locked, skin|car, name])
    console->AddFunction( "SetStageMessageIndex", SetStageMessageIndex, "", 1, 2 ); //Sets the index in the text bible (index)

    console->AddFunction( "SetStageTime", SetStageTime, "", 1, 1 ); //Adds time to the current stage ()
    console->AddFunction( "AddStageTime", AddStageTime, "", 1, 1 ); //Sets the time of the current stage ()
    
    console->AddFunction( "AddStageVehicle", AddStageVehicle, "", //Adds an AI vehicle to the stage (name, startlocator, aitype, [.con], [driver name])
        3, 5 );
	console->AddFunction( "MoveStageVehicle", MoveStageVehicle, "", //Moves an AI vehicle in the stage (name, startlocator, aitype)
        3, 3 );
	console->AddFunction( "ActivateVehicle", ActivateVehicle, "", 3, 4 ); //Activates AI vehicle that has no AI in the stage (name, startlocator, aitype, [driver name])
    console->AddFunction( "AddStageWaypoint", AddStageWaypoint, "", 1, 1 ); //Adds a waypoint to the stage (locatorname)
    console->AddFunction( "AddStageCharacter", AddStageCharacter, "", 3, 5 ); //Adds a character to the stage (name, locatorname, dynaload string, [vehiclename], [carlocator])

    console->AddFunction( "AddStageMusicChange", AddStageMusicChange, "", 0, 0 ); //Triggers check for change of music tension
    console->AddFunction( "SetStageMusicAlwaysOn", SetStageMusicAlwaysOn, "", 0, 0 ); //Locks music on for walking out of car in stage
    console->AddFunction( "SetCompletionDialog", SetCompletionDialog, "", 1, 2 ); //Plays dialog with given event name at end of stage
    console->AddFunction( "StageStartMusicEvent", SetStageStartMusicEvent, "", 1, 1 ); //Triggers a music event when stage begins
    console->AddFunction( "SetMusicState", SetMusicState, "", 2, 2 ); //Sets a radMusic state using the given state and event names

    console->AddFunction( "SetStageCamera", SetStageCamera, "", 3, 3 ); //Sets the current camera mode for this stage (mode, cut, quick transition)

    console->AddFunction( "RESET_TO_HERE", ResetToThisStage, "", 0, 0 ); //Sets the desred stage for resseting to... hehe hehehshshehahrheh
    
    console->AddFunction( "SetMaxTraffic", SetTrafficDensity, "", 1, 1 ); //Sets the max traffic density.

    console->AddFunction( "AddSafeZone", AddSafeZone, "", 2, 2 ); //Adds a Safezone at locatorname,radius in meters ()

    console->AddFunction( "SetBonusMissionStart", SetBonusMissionStart, "", 0, 0 ); //Makes this stage the start of a bonus mission.

    console->AddFunction( "ShowStageComplete", ShowStageComplete, "", 0, 0 ); //Show stage complete presentation.

    console->AddFunction( "SetHUDIcon", SetHUDIcon, "", 1, 1 ); //Set HUD Icon Image.

    console->AddFunction( "SetIrisWipe", SetIrisWipe, "", 1, 1 ); //Turns on Iris wipe.

    console->AddFunction( "SetFadeOut", SetFadeOut, "", 1, 1 ); //Turns on Fade Out.

    console->AddFunction( "CloseStage", CloseStage, "", 0, 0 ); //Closes the current stage ()

    console->AddFunction( "SetVehicleAIParams", SetVehicleAIParams, "", 3, 3 ); //Sets params for an existing stage vehicle AI (STAGEVEHICLENAME, MINSHORTCUTSKILL, MAXSHORTCUTSKILL)

    console->AddFunction("PlacePlayerAtLocatorName",PlacePlayerAtLocatorName, "",1,1); //On Stage Reset/Init places the player at a Locator
    console->AddFunction("msPlacePlayerCarAtLocatorName",msPlacePlayerCarAtLocatorName, "",1,1); //Place the players current car at locator at the start of stage

    //Chuck:Commands for Swapping Default for forced cars As per the Agreed template
    console->AddFunction("SwapInDefaultCar",SwapInDefaultCar, "",0,0); //At the End of this Stage We Swap In the Default during the Iris Wipe
    console->AddFunction("SetSwapPlayerLocator",SetSwapPlayerRespawnLocatorName, "",1,1); //Sets the Locator to spawn the player after we swap cars
    console->AddFunction("SetSwapDefaultCarLocator",SetSwapDefaultCarRespawnLocatorName, "",1,1); //Set the Locator to spawn the default car after we swap cars
    console->AddFunction("SetSwapForcedCarLocator",SetSwapForcedCarRespawnLocatorName, "",1,1); //Set the Locator to spawn the forced car after we swap cars

    console->AddFunction("NoTrafficForStage",NoTrafficForStage, "",0,0); //Sets No Traffic For a Stage

    console->AddFunction("ClearTrafficForStage",ClearTrafficForStage, "",0,0); //Clear traffic at the beginning of the stage
        
    console->AddFunction( "SetStageAIRaceCatchupParams",SetStageAIRaceCatchupParams, "", 5,5 ); //Sets Race AI catchup params.

    console->AddFunction( "SetStageAIEvadeCatchupParams",SetStageAIEvadeCatchupParams, "", 3,3 ); //Sets Evade AI catchup params.

    console->AddFunction( "SetStageAITargetCatchupParams",SetStageAITargetCatchupParams, "", 3,3 ); //Sets Target AI catchup params.


    console->AddFunction("SetCharacterToHide",SetCharacterToHide, "",1,1); //Set a Character To Be Hidden for duration at start of stage

    console->AddFunction("SetLevelOver",SetLevelOver, "",0,0); //Explictly Command to End Level at Stage Finalize
    console->AddFunction("SetGameOver", SetGameOver, "", 0, 0); //Explicitly end the game.

    console->AddFunction("StayInBlack",StayInBlack, "",0,0); //Makes Scree black for duration of stage

    console->AddFunction("AllowMissionAbort",AllowMissionAbort,"",1,1);

    //
    //Gamble Race Commands
    //
    console->AddFunction("SetParTime",SetParTime, "",1,1); //Sets Par Time for Gambling Races ONLY!

    console->AddFunction("SetRaceEnteryFee",SetRaceEnteryFee, "",1,1); //Sets The Amount of Coins Required to Enter Race

    console->AddFunction("PutMFPlayerInCar",PutMFPlayerInCar, "",0,0); //Forces the player to respawn in their car at the beginning of this stage


    console->AddFunction("SetStatepropShadow",SetStatepropShadow, "", 2,2 ); //Sets which element of a stateprop is the shadow and projects it onto the ground


    //
    // Objective methods
    //
    console->AddFunction( "AddObjective", AddObjective, "", 1, 3 ); //Adds a new objective to the current stage ( objtype )
    console->AddFunction( "CloseObjective", CloseObjective, "", 0, 0 ); //Closes the current objective ()

    console->AddFunction( "SetDestination", SetDestination, "", 1, 3); //Sets the destination of the current objective (locator name, [p3d name], [float scale factor])

    console->AddFunction( "AddNPC", AddNPC, "",2, 3 ); //Adds an NPC (name) to the position (locator name) beginning at this objective

    console->AddFunction( "RemoveNPC", RemoveNPC, "",1, 1 ); //Remove an NPC (name) in this objective

    console->AddFunction( "AddDriver", AddDriver, "",2, 2 ); //Adds an driver (name) int the car (other name) in this objective

    console->AddFunction( "RemoveDriver", RemoveDriver, "",1, 1 ); //Remove an driver (name) in this objective

    console->AddFunction( "SetTalkToTarget", SetTalkToTarget, "", 1, 4 ); //Sets the target of the objective's conversation (name, [0 - exclamation (default), 1 - gift, 2 - Door], [y offset], [trigger radius]

    console->AddFunction( "SetDialogueInfo", SetDialogueInfo, "", 4, 4 ); //Sets the members of the dialogue, the name of the dialogue sound and the length of pause

    console->AddFunction( "SetDialoguePositions", SetDialoguePositions, "", 2, 4 ); //Sets the positions of the characters and the car for the dialogue

    console->AddFunction( "SetRaceLaps", SetRaceLaps, "", 1, 1 ); //Sets the number of laps in the race.

    console->AddFunction( "TurnGotoDialogOff", TurnGotoDialogOff, "", 0, 0 ); //Turns arrival dialog off for goto objectives.

    console->AddFunction( "MustActionTrigger", MustActionTrigger, "", 0, 0 ); //Must use action button for goto objectives.

    //Chuck Coin Objectives

    console->AddFunction("SetCoinFee",SetCoinFee, "",1,1); //Set The Amount of Coins to get past a Stage

    //Timer Objectives

    console->AddFunction("SetDurationTime",SetPauseDuration, "",1,1); //Set the amount of time for the timer objective

    //
    // Collection objective methods
    //
    console->AddFunction( "AddCollectible", AddCollectible, "", 1, 4 ); //Adds a new collectible to the current objective (locator name, [p3d name], [dialog event], [float scale factor])
    console->AddFunction( "SetCollectibleEffect", SetCollectibleEffect, "", 1, 1 ); //Sets the collectible effect (effect name)
    console->AddFunction( "BindCollectibleTo", BindCollectibleToWaypoint, "", 2, 2 ); //Binds the collectible <num> to the waypoint <num> (collect num, waypoint num)
    console->AddFunction( "AllowUserDump", AllowUserDump, "", 0, 0 ); //Allows the player to lose collected object when struck by ai vehicles.

    console->AddFunction( "SetVehicleToLoad", SetVehicleToLoad, "", 3,3); //Starts an async load of a new player vehicle

    //
    // Follow objective methods
    //
    console->AddFunction( "SetObjTargetVehicle", SetObjTargetVehicle, "", 1, 1 ); //Sets the target vehicle for a follow objective (name)
    console->AddFunction( "SetObjDistance", SetObjDistance, "", 1, 1 ); //Sets the distance needed to lose the tail (distance)

    // Boss objective
    console->AddFunction( "SetObjTargetBoss", SetObjTargetBoss, "", 1,1 ); //Sets the target boss for a destroy objective

    console->AddFunction( "AddCollectibleStateProp", AddCollectibleStateProp, "", 3,3 ); //Creates a new collectible stateprop into world

    console->AddFunction( "SetPickupTarget", SetPickupTarget, "", 1,1 ); //Sets the target for a pickupitem objective


    console->AddFunction( "AllowRockOut", AllowRockOut, "", 0, 0 ); //Have the PC 'rock out' when idling during this objective

    //
    // Mission Condition methods
    //
    console->AddFunction( "AddCondition", AddCondition,  "", 1, 2 ); //Adds a new condition to the stage (type)
    console->AddFunction( "CloseCondition", CloseCondition, "", 0, 0 ); //Closes the current condition

    console->AddFunction( "SetFollowDistances", SetFollowDistances, "", 2, 2 ); //Sets the follow condition distances (min, max)

    //
    // Follow Condition methods
    //
    console->AddFunction( "SetCondTargetVehicle", SetCondTargetVehicle, "", 1, 1 ); //Sets the target vehicle for a follow condition (name)

    //
    // Position Condition methods
    console->AddFunction("SetConditionPosition", SetConditionPosition, "", 1, 1 ); //Sets the position required for the position condition on a race.

    console->AddFunction("SetCondTime", SetCondTime, "", 1, 1 ); //Sets the timeout for the out of car condition.

    console->AddFunction("SetHitNRun", SetHitNRun, "", 0, 0 ); //Turns on Hit N Run on timeout condition.

    //
    // Presentation Functions 
    //
    console->AddFunction( "EnableTutorialMode", EnableTutorialMode, "", 1, 1 ); //Should tutorial mode be enabled?
    console->AddFunction( "SetConversationCamName", SetConversationCamName, "x", 1, 1 ); //Should tutorial mode be enabled?
    console->AddFunction( "SetConversationCamPcName", SetConversationCamPcName, "", 1, 1 ); //Sets which of the conversation cameras will be used when the PC is talking
    console->AddFunction( "SetConversationCamNpcName", SetConversationCamNpcName, "", 1, 1 ); //Sets which of the conversation cameras will be used when the NPC is talking
    console->AddFunction( "SetConversationCam", SetConversationCam, "", 2, 3 ); //Sets the conversation cmaera for a particular line of dialog
    console->AddFunction( "SetConversationCamDistance", SetConversationCamDistance, "", 2, 2 ); //Sets which of the conversation cameras will be used when the NPC is talking
    console->AddFunction( "AmbientAnimationRandomize", AmbientAnimationRandomize, "", 2, 2 ); //should the animation for a particular character be randomized?
    console->AddFunction( "ClearAmbientAnimations", ClearAmbientAnimations, "", 1, 1 );
    console->AddFunction( "AddAmbientPcAnimation", AddAmbientPcAnimation, "", 1, 2 ); //Adds an animation that will be chosen at random during conversations
    console->AddFunction( "AddAmbientNpcAnimation", AddAmbientNpcAnimation, "", 1, 2 ); //Adds an animation that will be chosen at random during conversations
    console->AddFunction( "CharacterIsChild", CharacterIsChild,  "", 1, 1 ); //Tells the conversation camera that this character is a child, and to look down
    console->AddFunction( "SetPresentationBitmap", SetPresentationBitmap, "", 1, 1 ); //this is the bitmap to use on the mission breifing screen
    console->AddFunction( "SetAnimCamMulticontName", SetAnimatedCameraMulticontrollerName, "", 1, 1 ); //This is the multicontroller for the animated camera that we should use
    console->AddFunction( "SetAnimatedCameraName", SetAnimatedCameraName, "", 1, 1 ); //This is the animated camera that we should use
    console->AddFunction( "SetMissionStartMulticontName", SetMissionStartMulticontrollerName, "", 1, 1 ); //This is the multicontroller for the animated camera that we should use
    console->AddFunction( "SetMissionStartCameraName", SetMissionStartCameraName, "", 1, 1 ); //This is the animated camera that we should use
    console->AddFunction( "SetCamBestSide", SetCamBestSide, "", 1, 2 ); //This is the name of the locator that corresponds to the best side for the camera
    console->AddFunction( "SetFMVInfo", SetFMVInfo, "", 1, 2 ); //Play the FMV specified by file name
    console->AddFunction( "StartCountdown", StartCountdown, "", 1, 2 ); //start the 321 go countdown, specify dialog
    console->AddFunction( "AddToCountdownSequence", AddToCountdownSequence, "", 1, 2 ); //Add message and dialog to countdown sequence
    console->AddFunction( "SetCarStartCamera", SetCarStartCamera, "", 1, 1 ); //Sets the camera that will be used when a car is loaded from a phone booth
    console->AddFunction( "GoToPsScreenWhenDone", GoToPattyAndSelmaScreenWhenDone, "", 0, 0 ); //go to patty and selma screen when stage done?

    //SuperSprint methods
    console->AddFunction( "SetPlayerCarName", SetPlayerCarName, "", 2, 2 ); //This sets the car name for a given player

    console->AddFunction( "SetCondMinHealth", SetCondMinHealth, "", 1, 1 ); //This sets the min health before failing the condition

    // 
    // Loading script methods
    //
    console->AddFunction( "LoadP3DFile", LoadP3DFile, "", 1, 3 ); //Loads a Pure3D file ( filename, [heap name], [inventory section name] )
    console->AddFunction( "LoadDisposableCar", LoadDisposableCar, "", 3, 3 ); //Loads a Pure3D CAR file ( filename,name,slot )


    //Setting the respawn time for wrenches and wasps and nitro etc

    console->AddFunction("SetRespawnRate",SetRespawnRate, "",2,2); //Set RespawnRate for Wrenches and Nitro in seconds,etc

    console->AddFunction( "AddCharacter", AddCharacter, "", 2, 2 ); //Adds a character to the game world.  All files must still be loaded ( p3d, choreo )
    console->AddFunction( "AddNPCCharacterBonusMission", AddNPCCharacterBonusMission, "", 7, 8 ); //Adds an NPC character to the game world.  All files must still be loaded ( p3d, choreo, loc, mission name, icon name, dialogue name, <bool>isRace, alternate icon name )
    console->AddFunction( "SetBonusMissionDialoguePos", SetBonusMissionDialoguePositions, "", 3, 4 ); //Sets the positions of the characters and the car for the bonus mission dialogue (mission name, char 1 loc, char 2 loc, car loc)

    console->AddFunction( "AddAmbientCharacter", AddAmbientCharacter, "", 2, 3 ); //Adds an ambient NPC character to the game world.  All files must still be loaded ( p3d, loc)

    console->AddFunction( "AddBonusMissionNPCWaypoint", AddBonusMissionNPCWaypoint, "", 2, 2 ); //Adds a waypoint for specified bonus mission NPC ( NPCNAME, LOCATOR )
    console->AddFunction( "AddObjectiveNPCWaypoint", AddObjectiveNPCWaypoint, "", 2, 2 ); //Adds a waypoint for specified mission objective NPC ( NPCNAME, LOCATOR )
    console->AddFunction( "AddAmbientNPCWaypoint", AddAmbientNPCWaypoint, "", 2, 2 ); //Adds a waypoint for specified ambient NPC ( NPCNAME, LOCATOR )
    console->AddFunction( "AddPurchaseCarNPCWaypoint", AddPurchaseCarNPCWaypoint, "", 2, 2 ); //Adds a waypoint for specified Purchase Car Reward NPC ( NPCNAME, LOCATOR )

    console->AddFunction( "ActivateTrigger", ActivateTrigger, "", 1, 1 ); //Activate a locator ( locator name )
    console->AddFunction( "DeactivateTrigger", DeactivateTrigger, "", 1, 1 ); //Deactivate a locator ( locator name )

    //
    // Oh, that's a creative name. Like, duh.
    //
    console->AddFunction( "CreateAnimPhysObject", CreateAnimPhysObject, "", 2, 2 ); //Creates an animated physics object (name, animname)
    console->AddFunction( "CreateActionEventTrigger", CreateActionEventTrigger, "", 5, 5 ); //Creates an action event trigger CreateActionEventTrigger(const char* triggerName, rmt::Vector& pos, float r)
     console->AddFunction( "LinkActionToObjectJoint", LinkActionToObjectJoint, "", 5, 5 ); //Links the Action Trigger to an object joint. LinkActionToObjectJoint(const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName )
    console->AddFunction( "LinkActionToObject", LinkActionToObject, "", 5, 5 ); //Links the Action Trigger to an object. LinkActionToObject(const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName)
    console->AddFunction( "SetCoinDrawable", SetCoinDrawable, "", 1, 1 ); //Name the drawable for the coins
    console->AddFunction( "SetParticleTexture", SetParticleTexture, "", 2, 2 ); //Name of texture used for particle

}
//=============================================================================
// MissionScriptLoader::LoadScript
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* filename )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::LoadScript( char* filename )
{
    mFirstObjective = true;

    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);

    //
    // NOTE: This has to stay sync because the scripts queue requests to the
    // loading manager and these do not get processed if the loadmanager is
    // already busy loading something
    //
    GetConsole()->ExecuteScriptSync( filename );

    HeapMgr()->PopHeap (GMA_LEVEL_MISSION);
}

void MissionScriptLoader::LoadScriptAsync( char* filename, LoadingManager::ProcessRequestsCallback* callback )
{
    mFirstObjective = true;
    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
    GetConsole()->ExecuteScript( filename, this, callback );
    HeapMgr()->PopHeap(GMA_LEVEL_MISSION);
}


void MissionScriptLoader::OnExecuteScriptComplete( void* pUserData )
{
    if ( pUserData != NULL )
    {
        LoadingManager::ProcessRequestsCallback* callback = static_cast<LoadingManager::ProcessRequestsCallback*>(pUserData);
        if ( callback )
        {
            callback->OnProcessRequestsComplete( NULL );
        }
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// MissionScriptLoader::SetLanguage
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetLanguage( int argc, char** argv )
{
#ifdef PAL
    const char languageID = argv[ 1 ][ 0 ];

    switch( languageID )
    {
        case 'F':
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_FRENCH );

            break;
        }
        case 'G':
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_GERMAN );

            break;
        }

#ifndef RAD_GAMECUBE
        case 'S':
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_SPANISH );

            break;
        }
#endif // !RAD_GAMECUBE

        default:
        {
            rTuneAssertMsg( languageID == 'E', "Invalid language specification!" );

            break;
        }
    }
#endif // PAL
}

//=============================================================================
// MissionScriptLoader::InitVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::InitLevelPlayerVehicle( int argc, char** argv )
{
    char locator[64], vehicleName[64], confile[64], slot[64];
    GameplayManager::eCarSlots eslot = GameplayManager::eDefaultCar;
    strcpy( vehicleName, argv[ 1 ]);
    strcpy( locator, argv[ 2 ]);
    strcpy(slot,argv[3]);

    
    //determining which slot init on. 
    if(strcmp (slot,"DEFAULT") == 0)
    {
        eslot = GameplayManager::eDefaultCar;
    }
    else if ( strcmp (slot,"OTHER") == 0 )
    {
        eslot = GameplayManager::eOtherCar;
    }
    else if (strcmp (slot,"AI") == 0 )
    {
        rTuneAssertMsg(0, "is this ever called - see greg \n");
        eslot = GameplayManager::eAICar;
    }
    else
    {
        printf(" Unknown Slot type, trying Init Level Car\n");
        rAssert (0);
    }

    //empty vehicle point case
    if ( GetGameplayManager()->mVehicleSlots[eslot].mp_vehicle == NULL)  
    {
        Vehicle* vehicle;
        
        if(argc == 5)
        {
            strcpy( confile, argv[ 4 ]);
            vehicle = GetGameplayManager()->AddLevelVehicle( vehicleName,eslot, confile );
        }
        else
        {
            vehicle = GetGameplayManager()->AddLevelVehicle( vehicleName,eslot, 0 );
        }        
        GetGameplayManager()->SetCurrentVehicle(vehicle);
        
        // the mDefaultVehicle member data is old daryll legacy bs
        //if(eslot == GameplayManager::eDefaultCar)
        //{
        //    strcpy(GetGameplayManager()->mDefaultVehicle,vehicleName);
        //}
        
        GetGameplayManager()->PlaceVehicleAtLocatorName( vehicle, locator );
    }
    //non empty ptr
    else
    {
        // greg
        // jan 4, 2003
        // i'm curious to see if we ever get here
        rAssert(0);
    
    
    
        if (strcmp(GetGameplayManager()->mVehicleSlots[eslot].mp_vehicle->GetName(), vehicleName) == 0)
        {
            //do nothing same vehicle exist
        }
        else
        {  
            //trying to add some vehicle to a slot thats occupied
            rAssert(0);
        }
    }

    if ( eslot == GameplayManager::eDefaultCar )
    {
        //Save this as the default level data.
        sprintf( GetGameplayManager()->mDefaultLevelVehicleName, vehicleName );
        if ( argc == 5 )
        {
            sprintf( GetGameplayManager()->mDefaultLevelVehicleConfile, confile );
        }
        sprintf( GetGameplayManager()->mDefaultLevelVehicleLocator, locator );
    }

}

//=============================================================================
// MissionScriptLoader::PlacePlayerCar
//=============================================================================
// Description: Moves a Player car to a locator.
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::PlacePlayerCar( int argc, char** argv )
{
    char locator[64], vehicleName[64];
    strcpy( vehicleName, argv[ 1 ]);

    strcpy( locator, argv[ 2 ]);    
    
    Vehicle* vehicle = NULL;
    

    //checking command to move current car.
    if (strcmp(vehicleName,"current")==0)
    {
        vehicle= GetGameplayManager()->GetCurrentVehicle();
    }
    else
    {
        rTuneAssertMsg(0, "was this ever getting called, 'cause it wouldn't have worked - see greg \n");
        //vehicle= GetGameplayManager()->GetVehicle(vehicleName);
         
    }
   
    rAssert(vehicle != NULL);   
    GetGameplayManager()->PlaceVehicleAtLocatorName( vehicle, locator );
    
  
}

/*
void MissionScriptLoader::EnableTraffic( int argc, char** argv )
{
    ITrafficSpawnController* p_TrafficController= TrafficManager::GetSpawnController();
    rAssert(p_TrafficController);
    p_TrafficController->EnableTraffic();    

}
void MissionScriptLoader::DisableTraffic( int argc, char** argv )
{

    ITrafficSpawnController* p_TrafficController= TrafficManager::GetSpawnController();
    rAssert(p_TrafficController);
    p_TrafficController->DisableTraffic();
}
*/

//=============================================================================
// MissionScriptLoader::AddPurchaseCarReward
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddPurchaseCarReward( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    ActionEventLocator* rewLoc = NULL;

    rewLoc = new ActionEventLocator();
    rAssert( rewLoc );

    rewLoc->SetName( argv[1] );
    rewLoc->SetActionNameSize( strlen("PurchaseCar") );
    rewLoc->SetActionName( "PurchaseCar" );

    rAssert( argc == 7 );

    //The obj name is the name of the carstart locator
    rewLoc->SetObjNameSize( strlen( argv[6] ) );
    rewLoc->SetObjName( argv[6] );

    //We use people for cars.
    char uniqueName[64];
    sprintf(uniqueName, "reward_%s", argv[2]);

    Character* c = GetCharacterManager()->AddCharacterDeferedLoad( CharacterManager::NPC, uniqueName, argv[2], argv[3], argv[4] );
    c->SetRole(Character::ROLE_REWARD);
    c->SetAmbient(argv[4], 0.0f);

    //This is a hack to tie the character name to the action event.
    rewLoc->SetJointNameSize( strlen( uniqueName ) );
    rewLoc->SetJointName( uniqueName );

    c->SetRenderLayer(RenderEnums::LevelSlot);
    c->AddToWorldScene();

    rewLoc->SetButtonInput( (CharacterController::DoAction ) );
    rewLoc->SetShouldTransform( false );

    Locator* posLoc = p3d::find<Locator>(argv[4]);
    rAssert( posLoc );

    rmt::Vector pos( 0.0f, 0.0f, 0.0f );
    if ( posLoc )
    {
        posLoc->GetLocation( &pos );
    }

    rewLoc->SetLocation( pos );

    // If AddToGame() fails, actLoc will be released.
    // Hold on to it here.
    //
    rewLoc->AddRef();
    bool added = rewLoc->AddToGame( NULL );
    rAssert( added );

    // If AddToGame() didn't addref the locator, 
    // this release will delete it.
    //
    rewLoc->Release( );

    if ( added )
    {
        //Create a trigger volume for the locator;
        float scale = static_cast<float>( atof( argv[5] ) );

        SphereTriggerVolume* newTrigger = new SphereTriggerVolume( pos, scale );
        rAssert( newTrigger );

        newTrigger->SetLocator( rewLoc );
        rewLoc->SetNumTriggers( 1, GMA_LEVEL_OTHER );
        rewLoc->AddTriggerVolume( newTrigger );

        GetTriggerVolumeTracker()->AddTrigger( newTrigger );
    }

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
}

void MissionScriptLoader::SetPostLevelFMV( int argc, char** argv )
{
    rAssert( argc > 1 );
    GetGameplayManager()->SetPostLevelFMV( argv[ 1 ] );
}

//=============================================================================
// MissionScriptLoader::CreateChaseManager
//=============================================================================
// Description: Creates a ChaseManager instance for the hostvehicle
//
// Parameters:  the hostvehicle (char* )and the spawn rate (int)
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::CreateChaseManager(int argc,char** argv)
{
	char vehiclename [64];
	char confile [64];
	int spawnrate =0;

	strcpy(vehiclename,argv[1]);
	strcpy(confile,argv[2]);
	spawnrate = atoi(argv[3]);
	
	if ( GetGameplayManager()->CreateChaseManager(vehiclename,confile,spawnrate) != 0)
	{
		//no room to create new chasemanager, WTF?
		rAssert(0);
	}

}


//=============================================================================
// MissionScriptLoader::SetChaseSpawnRate
//=============================================================================
// Description: Sets the spawn rate for the hostvehicle
//
// Parameters:  the hostvehicle (char* )and the spawn rate (int) 0-10 whole integer
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetChaseSpawnRate (int argc,char** argv)
{

	char vehiclename [64];
	int num_cars =0;
	ChaseManager* p_chasemanager = NULL;


	strcpy(vehiclename,argv[1]);
	num_cars = atoi(argv[2]);
	
	spInstance->mpStage->SetChaseSpawnRate(vehiclename,num_cars);
}


//=============================================================================
// MissionScriptLoader::KillAllChaseAI
//=============================================================================
// Description: Stop all chase AI vehicle for a hostvehicle
//
// Parameters:  the hostvehicle (char* )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::KillAllChaseAI(int argc,char** argv)
{

	char vehiclename [64];
	ChaseManager* p_chasemanager = NULL;
	strcpy(vehiclename,argv[1]);
	p_chasemanager=GetGameplayManager()->GetChaseManager(vehiclename);
	rAssert(p_chasemanager);
	p_chasemanager->DisableAllActiveVehicleAIs();
	
}

   



//=============================================================================
// MissionScriptLoader::AddMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddMission( int argc, char** argv )
{
    int index = GetGameplayManager()->GetNumMissions();
    
    spInstance->mMissionHeap = GetGameplayManager()->GetMissionHeap( index );

    HeapMgr()->PushHeap( spInstance->mMissionHeap );

MEMTRACK_PUSH_GROUP( "Mission" );
    spInstance->mpMission = new Mission();
    spInstance->mpMission->SetName( argv[ 1 ] );

    if ( !GetGameplayManager()->mIsDemo )
    {
        Mission* sdPart = new Mission();

        char name[64];
        sprintf( name, "%ssd", argv[ 1 ] );
        sdPart->SetName( name );
        spInstance->mpStage = NULL;

        //Add the SD part of the mission...
        index;
        GetGameplayManager()->SetNumMissions( index + 1 );
        GetGameplayManager()->SetMission( index, sdPart );
        sdPart->SetSundayDrive();
        index++;
    }

    //Add the Main part of the mission...
    GetGameplayManager()->SetNumMissions( index + 1 );
    GetGameplayManager()->SetMission( index, spInstance->mpMission );

    //chuck: when we are in demo we shouldn't add missions to the charactersheet.
    if (GetGameplayManager()->mIsDemo == false)
    {
        //check for m8, m8 is a bogus mission and should not get added to charactersheet.
        if (strcmp(argv[1],"m8") != 0)
        {

            if (GetCharacterSheetManager()->AddMission(GetGameplayManager()->GetCurrentLevelIndex(),spInstance->mpMission->GetName()) !=0 )
            {  
                //asserted cause something failed adding mission to charactersheet.
                rAssert(0);
            }
        }
    }

    HeapMgr()->PopHeap( spInstance->mMissionHeap );
MEMTRACK_POP_GROUP( "Mission" );
}

//=============================================================================
// MissionScriptLoader::AddBonusMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddBonusMission( int argc, char** argv )
{
    int index = GameplayManager::MAX_MISSIONS + GetGameplayManager()->GetNumBonusMissions();

    spInstance->mMissionHeap = GetGameplayManager()->GetMissionHeap( index );

    HeapMgr()->PushHeap( spInstance->mMissionHeap );

MEMTRACK_PUSH_GROUP( "Mission" );
    spInstance->mpMission = new Mission();
    spInstance->mpMission->SetName( argv[ 1 ] );
    spInstance->mpMission->SetBonusMission();

    //Add the Main part of the mission...
    int bonusMissionsCount = GetGameplayManager()->GetNumBonusMissions();
    GetGameplayManager()->SetNumBonusMissions( bonusMissionsCount + 1 );
    GetGameplayManager()->SetMission( index, spInstance->mpMission );
MEMTRACK_POP_GROUP( "Mission" );

//Chuck: a Code Design Shortcut.
//I'm going to ASSUME That all bonusmission will be name "bm1"

//its a bonus mission 
    if (strcmp(argv[1],"bm1") ==0)
    {
        if (GetCharacterSheetManager()->AddBonusMission(GetGameplayManager()->GetCurrentLevelIndex(),spInstance->mpMission->GetName()) !=0 )
                {  
                    //asserted cause something failed adding mission to charactersheet.
                    rAssert(0);
                }
    }
    else if (strcmp (argv[1],"gr1")==0)
    {
       //its a gamble race and we do nothing
        if(GetCharacterSheetManager()->AddGambleRace(GetGameplayManager()->GetCurrentLevelIndex(),spInstance->mpMission->GetName()) != 0)
        {
            rTuneAssert(0);
        }

    }

    else
    {  
        //add it the charactersheet since it must be a streetrace
         if(GetCharacterSheetManager()->AddStreetRace(GetGameplayManager()->GetCurrentLevelIndex(),spInstance->mpMission->GetName()) !=0 )
        {
            //problems adding the streetrace to the charactersheet
            //rAssert(0);
            rReleasePrintf("Warning: Could NOT add %S into the CharacterSheet, All 3 StreetRace Slots are Full, Double Check this Mission's Name\n",spInstance->mpMission->GetName());
        }
    }

    HeapMgr()->PopHeap( spInstance->mMissionHeap );
}


//=============================================================================
// MissionScriptLoader::CloseMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::CloseMission( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpMission != NULL, "Mission already closed!\n" );
    spInstance->mpMission = NULL;
}

//=============================================================================
// MissionScriptLoader::SetForcedCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetForcedCar( int argc, char** argv )
{
    spInstance->mpMission->SetForcedCar( true );
}

//=============================================================================
// MissionScriptLoader::SetDemoLoopTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetDemoLoopTime( int argc, char** argv )
{
    if ( ( CommandLineOptions::Get( CLO_DEMO_TEST ) ||
           GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_DEMO_TEST ) ) &&
         ( !CommandLineOptions::Get( CLO_SHORT_DEMO ) ) )
    {
        ContextEnum currctx = GetGameFlow()->GetCurrentContext();
        if ( currctx == CONTEXT_DEMO || currctx == CONTEXT_LOADING_DEMO )
        {
            DemoContext* dctx = static_cast<DemoContext*>(GetGameFlow()->GetContext( CONTEXT_DEMO ));
            rAssert( dctx != NULL );

            dctx->SetDemoLoopTime( atoi( argv[1] ) );
        }
    }
}


//=============================================================================
// MissionScriptLoader::SelectMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SelectMission( int argc, char** argv )
{
    GameplayManager* gpm = GetGameplayManager();
    int missionNum = gpm->GetMissionNumByName( argv[1] );

    if ( missionNum > -1 )
    {
        spInstance->mpMission = gpm->GetMission( missionNum );
        
        
        // new 
        // jan 8, 2003
        // greg
        
        // this is the very beginning of a brand new mission
        //
        // dump out old mission car list from GameplayManger
        
        // HUGE TODO!!!!
        //
        // only want to do this the first time we start a mission
        // NOT on a mission reset
        
        gpm->EmptyMissionVehicleSlots();    
                
        return;
    }

    spInstance->mpMission = NULL;

    // This really bad
    rAssert( false );
}

//=============================================================================
// MissionScriptLoader::SetMissionNameIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetMissionNameIndex( int argc, char** argv )
{
    rAssert( spInstance->mpMission != NULL );

    int index = atoi( argv[ 1 ] );
    //spInstance->mpMission->SetNameIndex( index );
}

//=============================================================================
// MissionScriptLoader::AddMissionPlayerVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
/*
void MissionScriptLoader::InitMissionPlayerVehicle( int argc, char** argv )
{
    // don't think this is ever called
    
    rAssert(0);



    rAssert( spInstance->mpMission != NULL );

    char confile[64];
    
    Vehicle* vehicle = NULL;
    if(argc == 2)
    {
        vehicle = GetVehicleCentral()->InitVehicle( argv[ 1 ], false, 0, VT_USER );
    }
    else if(argc == 3)
    {
        strcpy (confile, argv[2]);
        vehicle = GetVehicleCentral()->InitVehicle( argv[ 1 ], false, confile, VT_USER );
    }
    else
    {
        rTuneAssertMsg(0,"wrong number of arguments for InitMissionPlayerVehicle\n");
    }

    // now handled by VehicleCentral::AddVehicleToActiveList
    //GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->AddGuts((tDrawable*)( vehicle ) );
    
    //spInstance->mpMission->AddVehicle( vehicle );
}
*/

//=============================================================================
// MissionScriptLoader::SetMissionResetPlayerInCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetMissionResetPlayerInCar( int argc, char** argv )
{
    //Find the locator named argv[ 1 ]
    CarStartLocator* loc = p3d::find<CarStartLocator>( argv[ 1 ] );

    if ( loc != NULL )
    {
        spInstance->mpMission->SetVehicleRestart( loc );
    }
    else
    {
#ifdef RAD_DEBUG
        char error[256];
        sprintf( error, "Can not find locator: %s\n", argv[ 1 ] );
        rDebugString( error );
#endif
    }
}

//=============================================================================
// MissionScriptLoader::SetMissionResetPlayerOutCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetMissionResetPlayerOutCar( int argc, char** argv )
{
    //Find the locator named argv[ 1 ]
    Locator* playerLoc = p3d::find<Locator>( argv[ 1 ] );
    //Find the locator named argv[ 2 ]
    CarStartLocator* vehicleLoc = p3d::find<CarStartLocator>( argv[ 2 ] );

    if ( playerLoc != NULL && vehicleLoc != NULL )
    {
        spInstance->mpMission->SetVehicleRestart( vehicleLoc );
        spInstance->mpMission->SetPlayerRestart( playerLoc );
    }
    else
    {
#ifndef FINAL
        
        if(playerLoc == NULL)
        {
            char error[256];
            sprintf( error, "Can not find locator: %s, Designer Error!!!!\n", argv[ 1 ] );
            TreeOfWoeErrorMsg(error);
            rTuneAssertMsg(0, error );
        }
        if(vehicleLoc == NULL)
        {
            char error[256];
            sprintf( error, "Can not find locator: %s, Designer Error!!!!\n", argv[ 1 ] );
            TreeOfWoeErrorMsg(error);
            rTuneAssertMsg(0, error );
        }

#endif
    }

    if(playerLoc)
    {
        bool loading = ((GetGuiSystem()->GetBackendManager()->GetCurrentScreen() == CGuiWindow::GUI_SCREEN_ID_LOADING) &&
            GetGuiSystem()->GetBackendManager()->GetCurrentWindow()->IsRunning());

        if((GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_GAMEPLAY) || loading) 
        {
            rmt::Vector pos;
            playerLoc->GetLocation(&pos);
            GetInteriorManager()->LoadLevelGags(pos, true);
        }
    }
}

//=============================================================================
// MissionScriptLoader::SetDynaLoadData
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetDynaLoadData( int argc, char** argv )
{
    if(argc == 3)
    {
        spInstance->mpMission->SetRestartDynaload( argv[ 1 ], argv[ 2 ] );
    }
    else
    {
        spInstance->mpMission->SetRestartDynaload( argv[ 1 ] );
    }
}

//=============================================================================
// MissionScriptLoader::AddBonusObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddBonusObjective( int argc, char** argv )
{
    rAssert( spInstance->mpMission );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
    #endif

    if ( strncmp("no_damage", argv[1], 9 ) == 0 )
    {
        //This is a no damage type bonus objective;
        NoDamageBonusObjective* ndbo = new NoDamageBonusObjective();
        rAssert( ndbo );

        spInstance->mpMission->AddBonusObjective( ndbo );
    }
    else if ( strncmp("no_harass", argv[1], 9 ) == 0 )
    {
        //This is a no harass cars type bonus objective;
        NoCopBonusObjective* ncbo = new NoCopBonusObjective();
        rAssert( ncbo );

        spInstance->mpMission->AddBonusObjective( ncbo );
    }
    else if ( strncmp("time", argv[1], 4 ) == 0 )
    {
        //This is a minimum time remaining type bonus objective;
        TimeRemainBonusObjective* trbo = new TimeRemainBonusObjective();
        rAssert( trbo );

        trbo->SetTime( atoi(argv[2]) );
        trbo->SetMission( spInstance->mpMission );
        spInstance->mpMission->AddBonusObjective( trbo );
    }
    else if ( strncmp("position", argv[1], 8 ) == 0 )
    {
        //This is a minimum time remaining type bonus objective;
        RacePositionBonusObjective* rpbo = new RacePositionBonusObjective();
        rAssert( rpbo );

        rpbo->SetDesiredPosition( atoi(argv[2]) );
        spInstance->mpMission->AddBonusObjective( rpbo );
    }

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
    #endif
}


//=============================================================================
// MissionScriptLoader::AddObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddObjective( int argc, char** argv )
{
    // The AddObjective script command takes between 1 and 3 parameters
    // The first parameter is the objective type.
    // The 2nd depends is either the arrow type or , if it is a race, it could be "gamble"
    // If the 2nd parameter is gamble, the third is optionally the arrow type
    // Make the last parameter optionally be the directional arrow type
    // Default is BOTH, if it is not present
    DirectionalArrowEnum::TYPE darrowType;
    if ( GetDirectionalArrowType( argv[ argc - 1 ], &darrowType ) == false )
    {
        darrowType = DirectionalArrowEnum::BOTH;
    }

    HeapMgr()->PushHeap( spInstance->mMissionHeap );

MEMTRACK_PUSH_GROUP( "Mission - Objectives" );

    rAssert( spInstance->mpStage->GetObjective() == NULL );

    rAssert( spInstance->mpObjective == NULL );

    spInstance->mObjType = MissionObjective::OBJ_INVALID;

    bool found = false;
    for( unsigned int i = 0; i < MissionObjective::NUM_OBJECTIVES; i++ )
    {
        if( strcmp( MissionObjectiveNames::Name[ i ], argv[ 1 ] ) == 0 )
        {
            spInstance->mObjType = (MissionObjective::ObjectiveTypeEnum)( i );
            found = true;
            break;
        }
    }

    rTuneAssertMsg( found, "This is not a valid Objective type" );

    switch( spInstance->mObjType )
    {
    case MissionObjective::OBJ_DELIVERY:
        {
            DeliveryObjective* pDel = new DeliveryObjective();
                       
            spInstance->mpObjective = pDel;
            break;
        }
    case MissionObjective::OBJ_GOTO:
        {
            GoToObjective* pGoto = new GoToObjective();
                        
            spInstance->mpObjective = pGoto;
            break;
        }
    case MissionObjective::OBJ_FOLLOW:
        {
            FollowObjective* pFollow = new FollowObjective();
                        
            spInstance->mpObjective = pFollow;
            break;
        }
    case MissionObjective::OBJ_RACE:
        {
            RaceObjective* pRace = new RaceObjective();
            
            //Chuck: check if this race is a gamble race
            if(strcmp(argv[2],"gamble") ==0)
            {
                pRace->SetGambleRace(true);
            }
            spInstance->mpObjective = pRace;
            break;
        }
    case MissionObjective::OBJ_LOSETAIL:
        {
            LoseObjective* pLose = new LoseObjective();

            spInstance->mpObjective = pLose;
            break;
        }
    case MissionObjective::OBJ_DESTROY:
        {
            DestroyObjective* pDestroy = new DestroyObjective();

            spInstance->mpObjective = pDestroy;
            break;
        }
    case MissionObjective::OBJ_TALKTO:
        {
            TalkToObjective* pTalkTo = new TalkToObjective();

            spInstance->mpObjective = pTalkTo;
            break;
        }
    case MissionObjective::OBJ_DIALOGUE:
        {
            DialogueObjective* pDialogue = new DialogueObjective();

            spInstance->mpObjective = pDialogue;
            break;
        }
    case MissionObjective::OBJ_GETIN:
        {
            GetInObjective* pGetIn = new GetInObjective();

            if(argc == 3) //added another param for strict getin
            {
                pGetIn->SetStrict(argv[2]);
            }

            spInstance->mpObjective = pGetIn;

            break;
        }
    case MissionObjective::OBJ_DUMP:
        {
            CollectDumpedObjective* pDump = new CollectDumpedObjective();

            spInstance->mpObjective = pDump;
            break;
        }
    case MissionObjective::OBJ_FMV:
        {
            FMVObjective* fmvObjective = new FMVObjective();
            spInstance->mpObjective = fmvObjective;
            break;
        }
    case MissionObjective::OBJ_INTERIOR:
        {
            InteriorObjective* intObjective = new InteriorObjective();
            spInstance->mpObjective = intObjective;
            break;
        }
    case MissionObjective::OBJ_COIN:
        {
            CoinObjective* pCoinObjective = new CoinObjective();
            spInstance->mpObjective = pCoinObjective;
            break;
        }
    case MissionObjective::OBJ_DESTROY_BOSS:
        {
            DestroyBossObjective* pObjective = new DestroyBossObjective();
            spInstance->mpObjective = pObjective;
        break;
        }
    case MissionObjective::OBJ_LOAD_VEHICLE:
        {
            LoadVehicleObjective* objective = new LoadVehicleObjective();
            spInstance->mpObjective = objective;
          break;
        }           
    case MissionObjective::OBJ_PICKUP_ITEM:
        {
            PickupItemObjective* objective = new PickupItemObjective();
            spInstance->mpObjective = objective;
            break;
        }
    case MissionObjective::OBJ_TIMER:
        {
            TimerObjective* pTimerObjective = new TimerObjective();
            spInstance->mpObjective = pTimerObjective;
            break;
        }
    case MissionObjective::OBJ_BUY_CAR:
        {
            BuyCarObjective* bco = new BuyCarObjective();
            spInstance->mpObjective = bco;
            rTuneAssertMsg( argc == 3, "You need to specify which car to buy for BuyCar Objectives\n");

            if ( argc == 3 )
            {
                bco->SetVehicleName( argv[2] );
            }
            break;
        }
    case MissionObjective::OBJ_BUY_SKIN:
        {
            BuySkinObjective* bso = new BuySkinObjective();
            spInstance->mpObjective = bso;
            rTuneAssertMsg( argc == 3, "You need to specify which skin to buy for BuSkinr Objectives\n");

            if ( argc == 3 )
            {
                bso->SetSkinName( argv[2] );
            }
            break;
        }
    case MissionObjective::OBJ_GO_OUTSIDE:
        {

            GoOutsideObjective* pGoOutSide = new GoOutsideObjective();
                        
            spInstance->mpObjective = pGoOutSide;


            break;
        }
    default:
        {
            MissionObjective* objective = new MissionObjective();
            spInstance->mpObjective = objective;
            break;
        }
    }

    //GetRoadManager()->SetDirectionalArrowType( darrowType );
        
    rAssert( spInstance->mpObjective );
    spInstance->mpObjective->SetDirectionalArrowType( &darrowType );
    spInstance->mpObjective->SetObjectiveType( spInstance->mObjType );    
    spInstance->mpStage->SetObjective( spInstance->mpObjective );
MEMTRACK_POP_GROUP( "Mission - Objectives" );

    HeapMgr()->PopHeap( spInstance->mMissionHeap );
}

//=============================================================================
// MissionScriptLoader::AddNPC
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddNPC( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
    //Find the locator named argv[ 2 ]
    spInstance->mpObjective->AddNPC( argv[ 1 ], argv[ 2 ], false );

    if(spInstance->mFirstObjective)
    {
        GetCharacterManager()->PreloadCharacter(argv[1], "npd");
    }    

    HeapMgr()->PopHeap(GMA_LEVEL_MISSION);
}

//=============================================================================
void MissionScriptLoader::AddDriver( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_MISSION );

    spInstance->mpObjective->AddNPC( argv[ 1 ], argv[ 2 ], true );

    if(spInstance->mFirstObjective)
    {
        GetCharacterManager()->PreloadCharacter(argv[1], "npd");
    }    

    HeapMgr()->PopHeap(GMA_LEVEL_MISSION);
}

//=============================================================================
void MissionScriptLoader::RemoveNPC( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
    //Find the locator named argv[ 2 ]
    spInstance->mpObjective->RemoveNPC( argv[ 1 ], false);
    HeapMgr()->PopHeap(GMA_LEVEL_MISSION);
}

//=============================================================================
void MissionScriptLoader::RemoveDriver( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
    //Find the locator named argv[ 2 ]
    spInstance->mpObjective->RemoveNPC( argv[ 1 ], true);
    HeapMgr()->PopHeap(GMA_LEVEL_MISSION);
}

//=============================================================================
// MissionScriptLoader::SetTalkToTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetTalkToTarget( int argc, char** argv )
{
    if ( spInstance->mObjType != MissionObjective::OBJ_TALKTO )
    {
        rDebugString( "Can only set talk to targets in talk to objectives\n" );
        return;
    }

    TalkToObjective::IconType icon = TalkToObjective::EXCLAMATION;

    if ( argc > 2 )
    {
        icon = static_cast<TalkToObjective::IconType>(::atoi( argv[2] ));
    }

    float yOffset = 0.0f;

    if ( argc > 3 )
    {
        yOffset = static_cast<float>(::atof( argv[3] ));
    }

    float trigRad = 1.3f; 
    if ( argc > 4 )
    {
        trigRad = static_cast<float>(::atof( argv[4] ));
    }

    reinterpret_cast<TalkToObjective*>(spInstance->mpObjective)->SetTalkToTarget( argv[1], icon, yOffset, trigRad );
}

//=============================================================================
// MissionScriptLoader::SetDialogueInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetDialogueInfo( int argc, char** argv )
{
    if ( spInstance->mObjType != MissionObjective::OBJ_DIALOGUE )
    {
        rDebugString( "Can only set dialoge settings in dialogue objectives!" );
        return;
    }

    DialogueObjective* dialogObj = reinterpret_cast<DialogueObjective*>(spInstance->mpObjective);
    dialogObj->SetChar1Name( argv[1] );
    dialogObj->SetChar2Name( argv[2] );
    dialogObj->SetDialogueName( argv[3] );
}

//=============================================================================
// MissionScriptLoader::SetDialoguePositions
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetDialoguePositions( int argc, char** argv )
{
    if ( spInstance->mObjType != MissionObjective::OBJ_DIALOGUE )
    {
        rDebugString( "Can only set dialoge settings in dialogue objectives!" );
        return;
    }

    CarStartLocator* char1Pos = p3d::find<CarStartLocator>( argv[1] );

#ifndef RAD_RELEASE
    if ( !char1Pos )
    {
        char error[128];
        sprintf( error, "Could not find %s for dialogue position\n", argv[1] );
        rTuneAssertMsg( false, error );
    }
#endif

    CarStartLocator* char2Pos = p3d::find<CarStartLocator>( argv[2] );

#ifndef RAD_RELEASE
    if ( !char2Pos )
    {
        char error[128];
        sprintf( error, "Could not find %s for dialogue position\n", argv[2] );
        rTuneAssertMsg( false, error );
    }
#endif

    CarStartLocator* carPos = NULL;
    /*if ( argc >= 4 )
    {
        //Also need a car start
        carPos = p3d::find<CarStartLocator>( argv[3] );

#ifndef RAD_RELEASE
        if ( !carPos )
        {
            char error[128];
            sprintf( error, "Could not find %s for dialogue position\n", argv[3] );
            rTuneAssertMsg( false, error );
        }
#endif
    }*/

    bool dontReset = false;

    if ( argc >= 5 )
    {
        dontReset = (atoi( argv[ 4 ] ) == 1);
    }

    static_cast<DialogueObjective*>(spInstance->mpObjective)->SetPositions( char1Pos, char2Pos, carPos, dontReset );
}

//=============================================================================
// MissionScriptLoader::SetRaceLaps
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetRaceLaps( int argc, char** argv )
{
    rAssert( spInstance->mObjType == MissionObjective::OBJ_RACE );

    RaceObjective* ro = reinterpret_cast<RaceObjective*>(spInstance->mpObjective);
    ro->SetNumLaps( atoi(argv[1] ) );
}

//=============================================================================
// MissionScriptLoader::BindCollectibleToWaypoint
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::BindCollectibleToWaypoint( int argc, char** argv )
{
    rAssert( spInstance->mObjType == MissionObjective::OBJ_DUMP );
    CollectDumpedObjective* cdo = reinterpret_cast<CollectDumpedObjective*>(spInstance->mpObjective);
    cdo->BindCollectibleToWaypoint( atoi( argv[1] ), atoi( argv[2] ) );
}

//=============================================================================
// MissionScriptLoader::AllowUserDump
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AllowUserDump( int argc, char** argv )
{
    rAssert( spInstance->mObjType == MissionObjective::OBJ_DUMP ||
             spInstance->mObjType == MissionObjective::OBJ_DELIVERY );

    static_cast<CollectibleObjective*>(spInstance->mpObjective)->AllowUserDump();
}

//=============================================================================
// MissionScriptLoader::SetVehicleToLoad
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetVehicleToLoad( int argc, char** argv )
{
    rAssert( spInstance->mObjType == MissionObjective::OBJ_LOAD_VEHICLE );
    
    LoadVehicleObjective* objective = static_cast< LoadVehicleObjective* >( spInstance->mpObjective );

    const char* filename = argv[1];
    const char* vehiclename = argv[2];
    const char* locatorname = argv[3];

    objective->SetFilename( filename );
    objective->SetVehicleName( vehiclename );
    objective->SetLocatorName( locatorname );
}


//=============================================================================
// MissionScriptLoader::CloseObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::CloseObjective( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpObjective != NULL, "Objective already closed!\n" );
    spInstance->mpObjective = NULL;
    spInstance->mObjType = MissionObjective::OBJ_INVALID;
    spInstance->mFirstObjective = false;
}



//=============================================================================
// MissionScriptLoader::AddCollectible
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddCollectible( int argc, char** argv )
{
    bool bValid = spInstance->mObjType == MissionObjective::OBJ_DELIVERY ||
                  spInstance->mObjType == MissionObjective::OBJ_RACE ||
                  spInstance->mObjType == MissionObjective::OBJ_DUMP;
    rTuneAssertMsg( bValid,
        "The current objective cannot accept collectibles." );

    rAssert( dynamic_cast<CollectibleObjective*>( spInstance->mpObjective ) != NULL );
    CollectibleObjective* pCol = static_cast<CollectibleObjective*>( spInstance->mpObjective );
    rAssert( pCol );

    if ( argc == 2 )
    {
        pCol->AddCollectibleLocatorName( argv[ 1 ], "", 0, 0, 0.0f );
    }
    else if ( argc == 3 )
    {
        pCol->AddCollectibleLocatorName( argv[ 1 ], argv[ 2 ], 0, 0, 0.0f );
    }
    else if ( argc == 4 )
    {
        pCol->AddCollectibleLocatorName( argv[ 1 ], argv[ 2 ], ::radMakeKey32( argv[ 3 ] ), 0, 0.0f );
    }
    else if ( argc == 5 )
    {
        pCol->AddCollectibleLocatorName( argv[ 1 ], argv[ 2 ], ::radMakeKey32( argv[ 3 ] ), 
                                         tEntity::MakeUID( argv[ 4 ] ), 0.0f );
    }
    else
    {
        pCol->AddCollectibleLocatorName( argv[ 1 ], argv[ 2 ], ::radMakeKey32( argv[ 3 ] ), 
                                         tEntity::MakeUID( argv[ 4 ] ), static_cast<float>(::atof( argv[ 5 ] )) );
    }
}

//=============================================================================
// MissionScriptLoader::AddCollectibleStateProp
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddCollectibleStateProp( int argc, char** argv )
{
    rmt::Vector position;    
    const char* statepropName = argv[1];
    const char* locatorName = argv[2];
    int physID = atoi( argv[3] );
    spInstance->mpMission->CreateStatePropCollectible( statepropName, locatorName, physID );
}



//=============================================================================
// MissionScriptLoader::SetCollectibleEffect
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void  
//
//=============================================================================
void MissionScriptLoader::SetCollectibleEffect( int argc, char** argv )
{
    if ( spInstance->mpObjective->GetObjectiveType() == MissionObjective::OBJ_GOTO )
    {
        GoToObjective* gto = static_cast<GoToObjective*>( spInstance->mpObjective );
        gto->SetCollectEffectName( argv[ 1 ] );
    }
    else
    {
        rTuneAssertMsg( dynamic_cast< CollectibleObjective* >( spInstance->mpObjective ) != NULL, "The current objective cannot accept collectibles.");
        CollectibleObjective* pCol = static_cast<CollectibleObjective*>( spInstance->mpObjective );
        rAssert( pCol );

        pCol->SetCollectEffectName( argv[ 1 ] );
    }
}

//=============================================================================
// MissionScriptLoader::SetDestination
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetDestination( int argc, char** argv )
{
    MissionObjective::ObjectiveTypeEnum type = spInstance->mObjType;

    switch( type )
    {
    case MissionObjective::OBJ_GO_OUTSIDE:
    case MissionObjective::OBJ_GOTO:
        {
            GoToObjective* pObj = (GoToObjective*)(spInstance->mpObjective);

            //Test to see if there is no drawable, but a scale.
            if ( argc == 3 && static_cast<float>(::atof( argv[ 2 ] )) != 0.0f )
            {
                pObj->SetDestinationNames( argv[ 1 ], "", static_cast<float>(::atof( argv[ 2 ] )) );
            }
            else if ( argc == 3 )
            {
                //There is no scale factor
                pObj->SetDestinationNames( argv[ 1 ], argv[ 2 ], 0.0f );
            }
            else if ( argc == 4 )
            {
                pObj->SetDestinationNames( argv[ 1 ], argv[ 2 ], static_cast<float>(::atof( argv[ 3 ] )) );
            }
            else
            {
                pObj->SetDestinationNames( argv[ 1 ], "", 0.0f );
            }
            break;
        }
    case MissionObjective::OBJ_INTERIOR:
        {
            InteriorObjective* pObj = (InteriorObjective*)(spInstance->mpObjective);
            pObj->SetDestination(argv[1]);

            if ( argc >= 3 )
            {
                pObj->SetIcon( argv[2] );
            }
        }
        break;
    default:
        {
            // fook!
            rTuneAssertMsg( false, "The current objective cannot accept a destination." );
        }
    }
}

//=============================================================================
// MissionScriptLoader::TurnGotoDialogOff
//=============================================================================
// Description: Turn the arrival dialogue off for a goto objective 
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::TurnGotoDialogOff( int argc, char** argv )
{
    rAssert( spInstance->mpObjective->GetObjectiveType() == MissionObjective::OBJ_GOTO || spInstance->mpObjective->GetObjectiveType() == MissionObjective::OBJ_GO_OUTSIDE );

    static_cast<GoToObjective*>(spInstance->mpObjective)->SetGotoDialogOff();
}

//=============================================================================
// MissionScriptLoader::MustActionTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::MustActionTrigger( int argc, char** argv )
{
    rAssertMsg( spInstance->mpObjective->GetObjectiveType() == MissionObjective::OBJ_GOTO, "MustActionTrigger can only be applied to GOTO objectives!\n" );

    static_cast<GoToObjective*>(spInstance->mpObjective)->SetMustActionTrigger();
}

//=============================================================================
// MissionScriptLoader::AddStageVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddStageVehicle( int argc, char** argv )
{
MEMTRACK_PUSH_GROUP( "Mission - Stage Vehicle" );
    char vehiclename[32];
    char spawnname[32];
    char ainame[32];
    char confile[32];

    strcpy( vehiclename, argv[ 1 ] );
    strcpy( spawnname, argv[ 2 ] );
    
    if(argc >= 4)
    {
        strcpy( ainame, argv[ 3 ] );
    }
    else
    {
        // ever have this case?
        ainame[0] = 0;
    }

    rTuneAssertMsg( spInstance->mpMission != NULL, "No mission is selected\n" );

    // new
    // greg
    // jan 7, 2003
    
    // GameplayManager is now the owner of these vehilces
    //
    // mission stages can continue to reference them as before, but they do not own them
    // 
    // I don't think there is any use in having a reference in the mission also


    Vehicle* vehicle = NULL; 

    switch(argc)
    {
        case 4:
        {
            vehicle = GetGameplayManager()->AddMissionVehicle(vehiclename);                
        }
        break;

        case 5:
        {
            strcpy(confile, argv[4]);
            vehicle = GetGameplayManager()->AddMissionVehicle(vehiclename, confile);                
            // this method will return a pointer if the vehicle already exists in the list of mission cars,
            // or it will allocate a new one             
        }
        break;

        case 6:
        {
            strcpy(confile, argv[4]);
            vehicle = GetGameplayManager()->AddMissionVehicle(vehiclename, confile, argv[5]);                
            // this method will return a pointer if the vehicle already exists in the list of mission cars,
            // or it will allocate a new one             
        }
        break;
    }

    rTuneAssertMsg( vehicle != NULL, "Could not find or create the requested vehicle\n" );
   

    /*
    if(argc == 5)
    {
        strcpy(confile, argv[4]);
        vehicle = GetVehicleCentral()->InitVehicle( vehiclename, false, confile, VT_AI );
    }
    else
    {
        vehicle = GetVehicleCentral()->InitVehicle( vehiclename, false, 0, VT_AI );
    }
    */

    
    //spInstance->mpMission->AddVehicle( vehicle ); // useless

    
    // now handled by VehicleCentral::AddVehicleToActiveList
    //GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->AddGuts((tDrawable*)( vehicle ) );

    CarStartLocator* locator = NULL;
    if( argc >= 3 && strlen( spawnname ) != 0 )
    {
        locator = p3d::find<CarStartLocator>( spawnname );
#ifndef FINAL
        char error [255];
        sprintf(error,"AddStageVehicle: Could not find the requested Car Start locator %s\n",spawnname);
        rTuneAssertMsg( locator != NULL,error);
#endif
    }

    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

    // new
    // greg
    // jan 9, 2003
    
    // move the allocation of the vehicle AI into the MissionStage

/*
MEMTRACK_PUSH_GROUP( "Mission - AI " );
    VehicleAI* pAI = NULL;
    if( argc >= 4 )
    {
        if( strcmp( ainame, "chase") == 0 )
        {
            pAI = new ChaseAI( vehicle );
        }
        else if( strcmp( ainame, "waypoint" ) == 0 )
        {
            pAI = new WaypointAI( vehicle );
        }
        else if ( strcmp( ainame, "NULL" ) == 0 )
        {
			pAI = NULL;
		
		}

		else 
		{	
			char buffy[64];
            sprintf( buffy, "Unknown AI vehicle type: %s", argv[ 3 ] );
            rTuneAssertMsg( false, buffy );
        }
    }
MEMTRACK_POP_GROUP();
*/


    int vehicleCentralIndex = GetGameplayManager()->GetMissionVehicleIndex(vehicle);
    //rAssert(vehicleCentralIndex != -1);

    //spInstance->mpStage->AddVehicle( vehicle, vehicleCentralIndex, locator, pAI );
    spInstance->mpStage->AddVehicle( vehicle, vehicleCentralIndex, locator, ainame );


MEMTRACK_POP_GROUP( "Mission - Stage Vehicle" );
}

//=============================================================================
// MissionScriptLoader::MoveStageVehicle
//=============================================================================
// Description: Used to move a staged Vehicle from one location to another. This vehicle must have been 
// Initialized or the command to fail. and 
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::MoveStageVehicle( int argc, char** argv )
{
MEMTRACK_PUSH_GROUP( "Mission - Stage Vehicle" );
    char vehiclename[32];
    char spawnname[32];
    char ainame[32];
    
    strcpy( vehiclename, argv[ 1 ] );
    strcpy( spawnname, argv[ 2 ] );
    
    
    if(argc >= 4)
    {
        strcpy( ainame, argv[ 3 ] );
    }
    else
    {
        // ever have this case?
        ainame[0] = 0;
    }
    
    rTuneAssertMsg( spInstance->mpMission != NULL, "No mission is selected\n" );

    Vehicle* vehicle = NULL; 

	vehicle =  spInstance->GetVehicleByName(vehiclename);

    CarStartLocator* locator = NULL;
    if( argc >= 3 && strlen( spawnname ) != 0 )
    {
        locator = p3d::find<CarStartLocator>( spawnname );
        rTuneAssertMsg( locator != NULL, "Could not find the requested Car Start locator\n" );
    }

    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );
/*
MEMTRACK_PUSH_GROUP( "Mission - AI " );
    VehicleAI* pAI = NULL;
    if( argc >= 4 )
    {
        if( strcmp( ainame, "chase") == 0 )
        {
            pAI = new ChaseAI( vehicle );
        }
        else if( strcmp( ainame, "waypoint" ) == 0 )
        {
            pAI = new WaypointAI( vehicle );
        }
		else if ( strcmp( ainame, "NULL" ) == 0 )
        {
			pAI = NULL;
		
		}
        else
        {
            char buffy[64];
            sprintf( buffy, "Unknown AI vehicle type: %s", argv[ 3 ] );
            rTuneAssertMsg( false, buffy );
        }
    }
MEMTRACK_POP_GROUP();
*/
    int vehicleCentralIndex = GetGameplayManager()->GetMissionVehicleIndex(vehicle);
    rAssert(vehicleCentralIndex != -1);
    
    spInstance->mpStage->AddVehicle( vehicle, vehicleCentralIndex, locator, ainame );
    
MEMTRACK_POP_GROUP( "Mission - Stage Vehicle" );

    GetGameplayManager()->PlaceVehicleAtLocator( vehicle, locator );
}


//=============================================================================
// MissionScriptLoader::ActivateVehicle
//=============================================================================
// Description: Used to give a staged Vehicle a new AI type . Call this  on a vehicle that is idle.
// This vehicle must have been Initialized via AddStageVehicle or the command to fail.  
// Parameters:  ( int argc, char** argv ) vehicle,locator,AI
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::ActivateVehicle( int argc, char** argv )
{
MEMTRACK_PUSH_GROUP( "Mission - Stage Vehicle" );
#if (RAD_TUNE||RAD_DEBUG)
    char errMsg[256];
#endif

    char vehiclename[32];
    char spawnname[32];
    char ainame[32];
    
    strcpy( vehiclename, argv[ 1 ] );
    strcpy( spawnname, argv[ 2 ] );
    
    if(argc >= 4)
    {
        strcpy( ainame, argv[ 3 ] );
    }
    else
    {
        // ever have this case?
        ainame[0] = 0;
    }
        
    rTuneAssertMsg( spInstance->mpMission != NULL, "No mission is selected\n" );

    Vehicle* vehicle = NULL; 

	vehicle =  spInstance->GetVehicleByName(vehiclename);

#if (RAD_TUNE||RAD_DEBUG)
    sprintf( errMsg, "ActivateVehicle: Cannot find vehicle called \"%s\"", vehiclename );
    rTuneAssertMsg( vehicle, errMsg );
#endif

    if ( argc >= 5 )
    {
        vehicle->SetDriverName( argv[4] );
    }

    CarStartLocator* locator = NULL;
    if( argc >= 3 && strlen( spawnname ) != 0 )
    {
		if (strcmp (spawnname,"NULL") == 0)
		{
			//do nothing

		}
		else
		{
			//get a ptr to the locator
			locator = p3d::find<CarStartLocator>( spawnname );

#if (RAD_TUNE||RAD_DEBUG)
            sprintf( errMsg, "ActivateVehicle: Could not find the requested Car Start locator \"%s\"", spawnname );
			rTuneAssertMsg( locator, errMsg );
#endif
		}
    }

#if (RAD_TUNE||RAD_DEBUG)
    sprintf( errMsg, "ActivateVehicle: This command is invalid because no stage is active." );
    rTuneAssertMsg( spInstance->mpStage, errMsg );
#endif
/*
MEMTRACK_PUSH_GROUP( "Mission - AI " );
    VehicleAI* pAI = NULL;
    if( argc >= 4 )
    {
        if( strcmp( ainame, "chase") == 0 )
        {
            pAI = new ChaseAI( vehicle );
        }
        else if( strcmp( ainame, "waypoint" ) == 0 )
        {
            pAI = new WaypointAI( vehicle );
        }
		else if ( strcmp( ainame, "NULL" ) == 0 )
        {
			pAI = NULL;
		
		}
        else
        {
            char buffy[64];
            sprintf( buffy, "Unknown AI vehicle type: %s", argv[ 3 ] );
            rTuneAssertMsg( false, buffy );
        }
    }
MEMTRACK_POP_GROUP();
*/
    int vehicleCentralIndex = GetGameplayManager()->GetMissionVehicleIndex(vehicle);
    //rAssert(vehicleCentralIndex != -1);
    
    spInstance->mpStage->AddVehicle( vehicle, vehicleCentralIndex, locator, ainame );
    

MEMTRACK_POP_GROUP( "Mission - Stage Vehicle" );

}

//=============================================================================
// MissionScriptLoader::AddStageWaypoint
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddStageWaypoint( int argc, char** argv )
{
    Locator* locator = p3d::find<Locator>( argv[ 1 ] );
    rTuneAssertMsg( locator != NULL, "Could not find the requested waypoint locator\n" );

    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

    spInstance->mpStage->AddWaypoint( locator );
}

//=============================================================================
// MissionScriptLoader::AddStageCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddStageCharacter( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

/*
    // Characters not loaded until Gameplay Context starts
    //
    Character* character = GetCharacterManager()->GetCharacterByName( argv[ 1 ] );
    rTuneAssertMsg( character != NULL, "Could not find the requested character\n" );
*/

    CarStartLocator* locator = NULL;
    if ( strcmp( argv[ 2 ], "" ) != 0 )
    {
        locator = p3d::find<CarStartLocator>( argv[ 2 ] );
        rTuneAssertMsg( locator != NULL, "Could not find the requested car start locator\n" );
    }

    CarStartLocator* carlocator = NULL;
    if ( argc >= 6 )
    {
        carlocator = p3d::find<CarStartLocator>( argv[ 5 ] );
        rTuneAssertMsg( carlocator != NULL, "Could not find the requested car start locator\n" );
    }

    const char* dynaloadString = argv[3];

    Vehicle* vehicle = NULL;
    if( argc >= 5 )
    {
        //chuck: we will use the overloader AddCharacter function.
        if(strcmp(argv[4],"current") ==0)
        {
            spInstance->mpStage->AddCharacter( argv[ 1 ], locator, carlocator, dynaloadString, argv[4] );
            return;
        }

        vehicle = spInstance->GetVehicleByName( argv[ 4 ] );
        rTuneAssertMsg( vehicle != NULL, "Could not find the requested vehicle\n" );
    }

    ////////////////////////////////////////////////////////////////////////////
    // MS10: Hack to allow character to be selected.
//        char name[32];
//        GetMissionManager()->GetOverrideCharacterName( name );
//        spInstance->mpStage->AddCharacter( name, locator, dynaloadString, vehicle );
    //
    // Put this back!
    //
    spInstance->mpStage->AddCharacter( argv[ 1 ], locator, carlocator, dynaloadString, vehicle );
    //
    ////////////////////////////////////////////////////////////////////////////
}

//=============================================================================
// MissionScriptLoader::AddStageMusicChange
//=============================================================================
// Description: Sets an indicator used by the music player to tell it that
//              we should change up the music and make it more dramatic for
//              this stage.
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddStageMusicChange( int argc, char** argv )
{
    spInstance->mpStage->SetMusicChangeFlag();
}

//=============================================================================
// MissionScriptLoader::SetStageMusicAlwaysOn
//=============================================================================
// Description: Sets an indicator for the music player to say that we should
//              continue playing music out of the car for this stage.
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetStageMusicAlwaysOn( int argc, char** argv )
{
    spInstance->mpStage->SetMusicAlwaysOnFlag();
}

//=============================================================================
// MissionScriptLoader::SetCompletionDialog
//=============================================================================
// Description: Sets the event name of a dialog line to play when stage
//              is completed
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetCompletionDialog( int argc, char** argv )
{
    spInstance->mpStage->SetDialogKey( ::radMakeKey32( argv[1] ) );

    if( argc == 3 )
    {
        spInstance->mpStage->SetConversationCharacterKey( tEntity::MakeUID( argv[2] ) );
    }
}

//=============================================================================
// MissionScriptLoader::SetStageStartMusicEvent
//=============================================================================
// Description: Sets the name of a music event to trigger when the current
//              stage begins
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetStageStartMusicEvent( int argc, char** argv )
{
    spInstance->mpStage->SetStageStartMusicEvent( ::radMakeCaseInsensitiveKey32( argv[1] ) );
}

//=============================================================================
// MissionScriptLoader::SetMusicState
//=============================================================================
// Description: Set a state in that funky radMusic matrix stuff
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetMusicState( int argc, char** argv )
{
    spInstance->mpStage->SetStageMusicState( ::radMakeCaseInsensitiveKey32( argv[1] ),
                                             ::radMakeCaseInsensitiveKey32( argv[2] ) );
}

//=============================================================================
// MissionScriptLoader::SetStageCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetStageCamera( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );
    if ( spInstance->mpStage == NULL )
    {
        return;
    }

    bool cut = atoi( argv[2] ) == 1 ? true : false;
    bool qTrans = atoi( argv[3] ) == 1 ? true : false;

    if ( strcmp( argv[1], "follow" ) == 0 )
    {
        //Set to follow cam
        spInstance->mpStage->SetCameraInfo( SuperCam::FOLLOW_CAM, cut, qTrans );

    }
    else if ( strcmp( argv[1], "walker" ) == 0 )
    {
        //Set to walker cam
#ifdef RAD_WIN32
        spInstance->mpStage->SetCameraInfo( SuperCam::ON_FOOT_CAM, cut, qTrans );
#else
        spInstance->mpStage->SetCameraInfo( SuperCam::WALKER_CAM, cut, qTrans );
#endif
    }
    else
    {
        rTuneAssertMsg( false, "Unsupported camera type set in script!" );
    }
}

//=============================================================================
// MissionScriptLoader::ResetToThisStage
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::ResetToThisStage( int argc, char** argv )
{
    int stagNum = spInstance->spInstance->mpMission->GetNumStages() - 1;
    spInstance->mpMission->SetResetToStage( stagNum );
}

//=============================================================================
// MissionScriptLoader::SetTrafficDensity
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetTrafficDensity( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

    if ( spInstance->mpStage == NULL )
    {
        return;
    }

    spInstance->mpStage->SetTrafficDensity( static_cast<char>(atoi(argv[1])) );
}

//=============================================================================
// MissionScriptLoader::AddCondition
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddCondition( int argc, char** argv )
{
    HeapMgr()->PushHeap( spInstance->mMissionHeap );

MEMTRACK_PUSH_GROUP( "Mission - Conditions" );
    rAssert( spInstance->mpCondition == NULL );

    spInstance->mCondType = MissionCondition::COND_INVALID;

    for( unsigned int i = 0; i < MissionCondition::NUM_CONDITIONS; i++ )
    {
        if( strcmp( MissionConditionNames::Name[ i ], argv[ 1 ] ) == 0 )
        {
            spInstance->mCondType = (MissionCondition::ConditionTypeEnum)( i );
            break;
        }
    }

    rTuneAssertMsg( spInstance->mCondType != MissionCondition::COND_INVALID, "This is not a valid Condition type\n" );

    switch( spInstance->mCondType )
    {
    case MissionCondition::COND_FOLLOW_DISTANCE:
        {
            FollowCondition* pCond = new FollowCondition();
                        
            spInstance->mpCondition = pCond;
            break;
        }
    case MissionCondition::COND_TIME_OUT:
        {
            TimeOutCondition* pCond = new TimeOutCondition();

            spInstance->mpCondition = pCond;
            break;
        }
    case MissionCondition::COND_RACE:
        {
            RaceCondition* pCond = new RaceCondition();
            
            spInstance->mpCondition = pCond;
            break;
        }
    case MissionCondition::COND_OUT_OF_BOUNDS:
        {
            OutOfBoundsCondition* pCond = new OutOfBoundsCondition();
            
            spInstance->mpCondition = pCond;
            break;
        }
    case MissionCondition::COND_VEHICLE_DAMAGE:
        {
            DamageCondition* pCond = new DamageCondition();
            
            spInstance->mpCondition = pCond;
            break;
        }
    case MissionCondition::COND_LEAVE_INTERIOR:
        {
            LeaveInteriorCondition* lic = new LeaveInteriorCondition();

            spInstance->mpCondition = lic;
            break;
        }
    case MissionCondition::COND_POSITION:
        {
            PositionCondition* pc = new PositionCondition();

            spInstance->mpCondition = pc;
            break;
        }
    case MissionCondition::COND_CARRYING_STATEPROP_COLLECTIBLE:
        {
            VehicleCarryingStateProp* vcsp = new VehicleCarryingStateProp();
            spInstance->mpCondition = vcsp;
            if ( argc > 2 )
            {
                // grab the collectible name, if specified
                const char* collectiblename = argv[2];
                vcsp->SetDesiredProp( collectiblename );
            }
            break;
        }
    case MissionCondition::COND_PLAYER_OUT_OF_VEHICLE:
        {
            GetOutOfCarCondition* goocc = new GetOutOfCarCondition();
            spInstance->mpCondition = goocc;
            break;
        }
    case MissionCondition::COND_NOT_ABDUCTED:
        {
            NotAbductedCondition* condition = new NotAbductedCondition();
            spInstance->mpCondition = condition;
            break;
        }
    case MissionCondition::COND_KEEP_BARREL:
        {
            KeepBarrelCondition* kbc = new KeepBarrelCondition();
            spInstance->mpCondition = kbc;

            if ( argc == 3 )
            {
                kbc->JumpBackBy( atoi( argv[2] ) );
            }

            break;
        }
    default:
        {
            rTuneAssertMsg( false, "Unknown Condition type\n" );
            break;
        }
    }


    rAssert( spInstance->mpCondition != NULL );
    
    unsigned int num = spInstance->mpStage->GetNumConditions();
    spInstance->mpStage->SetCondition( num,
        spInstance->mpCondition );
    spInstance->mpStage->SetNumConditions( num + 1 );
MEMTRACK_POP_GROUP( "Mission - Conditions" );

    HeapMgr()->PopHeap( spInstance->mMissionHeap );
}

//=============================================================================
// MissionScriptLoader::CloseCondition
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::CloseCondition( int argc, char** argv )
{
    spInstance->mpCondition = NULL;
    spInstance->mCondType = MissionCondition::COND_INVALID;
}

//=============================================================================
// MissionScriptLoader::AmbientAnimationRandomize
//=============================================================================
// Description: should the animations for a particular character be randomized
//              or not?
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AmbientAnimationRandomize( int argc, char** argv )
{
    rAssert( argc == 3 );
    int characterNumber = atoi( argv[ 1 ] );
    int randomInt = atoi( argv[ 2 ] );
    bool random = ( randomInt != 0 );
    spInstance->mpStage->AmbientCharacterAnimationSetRandom( characterNumber, random );
}

//=============================================================================
// MissionScriptLoader::ClearAmbientAnimations
//=============================================================================
// Description: adds an ambient animation to be played during conversations
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::ClearAmbientAnimations( int argc, char** argv )
{
    rAssert( argc == 2 );
    char* bonusMissionName = argv[ 1 ];
    const int index = GetGameplayManager()->GetMissionNumByName( bonusMissionName ) - GameplayManager::MAX_MISSIONS;  //the bonus missions come after the regular missions
    rAssert( index >= 0 );
    BonusMissionInfo* bmi = GetGameplayManager()->GetBonusMissionInfo( index );
    rAssert( bmi != NULL );
    bmi->ClearAmbientAnimations();
}

//=============================================================================
// MissionScriptLoader::AddAmbientNpcAnimation
//=============================================================================
// Description: adds an ambient animation to be played during conversations
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddAmbientNpcAnimation( int argc, char** argv )
{
    if( argc == 2 )
    {
        spInstance->mpStage->AddAmbientCharacterAnimation( 1, argv[ 1 ] );
    }
    else
    {
        rAssert( argc == 3 );
        char* bonusMissionName = argv[ 2 ];
        const int index = GetGameplayManager()->GetMissionNumByName( bonusMissionName ) - GameplayManager::MAX_MISSIONS;  //the bonus missions come after the regular missions
        rAssert( index >= 0 );
        BonusMissionInfo* bmi = GetGameplayManager()->GetBonusMissionInfo( index );
        rAssert( bmi != NULL );
        bmi->AddAmbientCharacterAnimation( 1, argv[ 1 ] );
    }
}

//=============================================================================
// MissionScriptLoader::AddAmbientPcAnimation
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddAmbientPcAnimation( int argc, char** argv )
{
    if( argc == 2 )
    {
        HeapMgr()->PushHeap( GMA_TEMP );
        spInstance->mpStage->AddAmbientCharacterAnimation( 0, argv[ 1 ] );
        HeapMgr()->PopHeap(GMA_TEMP);
    }
    else
    {
        rAssert( argc == 3 );
        char* bonusMissionName = argv[ 2 ];
        const int index = GetGameplayManager()->GetMissionNumByName( bonusMissionName ) - GameplayManager::MAX_MISSIONS;  //the bonus missions come after the regular missions
        rAssert( index >= 0 );
        BonusMissionInfo* bmi = GetGameplayManager()->GetBonusMissionInfo( index );
        rAssert( bmi != NULL );
        bmi->AddAmbientCharacterAnimation( 0, argv[ 1 ] );
    }
}

//=============================================================================
// MissionScriptLoader::SetCamBestSide
//=============================================================================
// Description: Sets the best side locator name for the conversation cameras
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetCamBestSide( int argc, char** argv )
{
    char* name = argv[ 1 ];
    if( argc == 2 )
    {
        spInstance->mpStage->SetBestSideLocator( name );
    }
    else
    {
        rAssert( argc == 3 );
        char* bonusMissionName = argv[ 2 ];
        const int index = GetGameplayManager()->GetMissionNumByName( bonusMissionName ) - GameplayManager::MAX_MISSIONS;  //the bonus missions come after the regular missions
        rAssert( index >= 0 );
        BonusMissionInfo* bmi = GetGameplayManager()->GetBonusMissionInfo( index );
        rAssert( bmi != NULL );
        bmi->SetBestSideLocator( name );
    }
}

//=============================================================================
// MissionScriptLoader::SetConversationCamName
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetConversationCamName( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_TEMP );
    spInstance->mpStage->SetConversationCamName( argv[ 1 ] );
    HeapMgr()->PopHeap( GMA_TEMP );
}

//=============================================================================
// MissionScriptLoader::SetConversationCamPcName
//=============================================================================
// Description: sets up the conversation camera used when the PC is talking
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetConversationCamPcName( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_TEMP );
    spInstance->mpStage->SetConversationCamPcName( argv[ 1 ] );
    HeapMgr()->PopHeap( GMA_TEMP );
}
//=============================================================================
// MissionScriptLoader::SetConversationCamNpcName
//=============================================================================
// Description: sets up the conversation camera used when the NPC is talking
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetConversationCamNpcName( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_TEMP );
    spInstance->mpStage->SetConversationCamNpcName( argv[ 1 ] );
    HeapMgr()->PopHeap( GMA_TEMP );
}

//=============================================================================
// MissionScriptLoader::SetConversationCam
//=============================================================================
// Description: sets the conversation camera for a particular line of dialog
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetConversationCam( int argc, char** argv )
{
    HeapMgr()->PushHeap( GMA_TEMP );
    int lineOfDialog = atoi( argv[ 1 ] );
    tName name = argv[ 2 ];
    if( argc == 3 )
    {
        spInstance->mpStage->SetCameraForDialogLine( lineOfDialog, name );
    }
    else
    {
        rAssert( argc == 4 );
        char* bonusMissionName = argv[ 3 ];
        const int index = GetGameplayManager()->GetMissionNumByName( bonusMissionName ) - GameplayManager::MAX_MISSIONS;  //the bonus missions come after the regular missions
        rAssert( index >= 0 );
        BonusMissionInfo* bmi = GetGameplayManager()->GetBonusMissionInfo( index );
        rAssert( bmi != NULL );
        bmi->SetCameraForDialogLine( lineOfDialog, name );
    }
    HeapMgr()->PopHeap( GMA_TEMP );
}

//=============================================================================
// MissionScriptLoader::SetConversationCamAngle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetConversationCamAngle( int argc, char** argv )
{
    rAssert( argc == 2 );
    const char* cameraName = argv[ 1 ];
    float camAngle = static_cast< float >( ::atof( argv[ 2 ] ) );
}

//=============================================================================
// MissionScriptLoader::SetConversationCamDistance
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetConversationCamDistance( int argc, char** argv )
{
    rAssert( argc == 3 );
    const char* cameraName = argv[ 1 ];
    float camDistance = static_cast< float >( ::atof( argv[ 2 ] ) );
    ConversationCam::SetCameraDistanceByName( cameraName, camDistance );
}

//=============================================================================
// MissionScriptLoader::SetConversationCamDistance
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetPresentationBitmap( int argc, char** argv )
{
    bool changeRequired = !CGuiScreenMissionLoad::IsCurrentBitmap( argv[ 1 ] );
    if( changeRequired )
    {
        CGuiScreenMissionLoad::SetBitmapName( argv[ 1 ] );
        CGuiScreenMissionLoad::ReplaceBitmap();
    }
}

//=============================================================================
// MissionScriptLoader::SetAnimatedCameraMulticontrollerName
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetAnimatedCameraMulticontrollerName( int argc, char** argv )
{
    rAssert( argc > 1 );
    AnimatedCam::SetMulticontroller( argv[ 1 ] );
}

//=============================================================================
// MissionScriptLoader::SetAnimatedCameraName
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void
//
//=============================================================================
void MissionScriptLoader::SetAnimatedCameraName( int argc, char** argv )
{
    rAssert( argc > 1 );
    AnimatedCam::ClearCamera();
    AnimatedCam::SetCamera( argv[ 1 ] );
}

//=============================================================================
// MissionScriptLoader::SetMissionStartMulticontrollerName
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void
//
//=============================================================================
void MissionScriptLoader::SetMissionStartMulticontrollerName( int argc, char** argv )
{
    rAssert( argc > 1 );
    AnimatedCam::SetMissionStartMulticontroller( argv[ 1 ] );
}

//=============================================================================
// MissionScriptLoader::SetMissionStartCameraName
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void
//
//=============================================================================
void MissionScriptLoader::SetMissionStartCameraName ( int argc, char** argv )
{
    rAssert( argc > 1 );
    AnimatedCam::SetMissionStartCamera( argv[ 1 ] );
}

//=============================================================================
// MissionScriptLoader::SetPlayerCarName
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetPlayerCarName( int argc, char** argv )
{
    GetSSM()->SetVehicle( ::atoi(argv[1]), argv[2] );
}


/*=============================================================================
Description:    Plays a FMV, usually after a mission. Note that it causes the HUD
                to be un/reloaded.
=============================================================================*/
void MissionScriptLoader::SetFMVInfo( int argc, char** argv )
{
    rAssert( argc > 1 );

    if ( spInstance->mObjType != MissionObjective::OBJ_FMV )
    {
        rDebugString( "Can only set FMV information in fmv objectives!" );
        return;
    }

    FMVObjective* fmvObj = reinterpret_cast<FMVObjective*>(spInstance->mpObjective);
    fmvObj->SetFileName( argv[ 1 ] );

    if( argc > 2 )
    {
        fmvObj->SetMusicStop();
    }
}
//=============================================================================
// MissionScriptLoader::CharacterIsChild
//=============================================================================
// Description: marks a specific character as a child character - camera
//              will be set lower in this case
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::CharacterIsChild( int argc, char** argv )
{
    rAssert( argc > 1 );
    AnimatedCam::SetMulticontroller( argv[ 1 ] );
}
//=============================================================================
// MissionScriptLoader::SetFollowDistances
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetFollowDistances( int argc, char** argv )
{
    rAssert( spInstance->mpCondition != NULL );
    
    switch( spInstance->mCondType )
    {
    case MissionCondition::COND_FOLLOW_DISTANCE:
        {
            FollowCondition* pCond = (FollowCondition*)(spInstance->mpCondition);

            float dist;

            dist = static_cast<float>( atoi( argv[ 1 ] ));
            pCond->SetMinDistance( dist );

            dist = static_cast<float>( atoi( argv[ 2 ] ));
            pCond->SetMaxDistance( dist );
            break;
        }
    default:
        {
            // fook!
            rTuneAssertMsg( false, "The current Condition cannot accept a follow distance\n" );
        }
    }
}

//=============================================================================
// MissionScriptLoader::SetCondMinHealth
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetCondMinHealth( int argc, char** argv )
{
    rAssert( spInstance->mpCondition != NULL );
    
    switch( spInstance->mCondType )
    {
    case MissionCondition::COND_VEHICLE_DAMAGE:
        {
            DamageCondition* pCond = (DamageCondition*)(spInstance->mpCondition);

            float damage = static_cast<float>( atoi( argv[ 1 ] ));
            pCond->SetMinValue( damage );

            break;
        }
    default:
        {
            // fook!
            rTuneAssertMsg( false, "The current Condition cannot accept a min health value\n" );
        }
    }
}

//=============================================================================
// MissionScriptLoader::SetConditionPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetConditionPosition( int argc, char** argv )
{
    rAssert( spInstance->mpCondition != NULL );
    rAssert( spInstance->mCondType == MissionCondition::COND_POSITION );

    PositionCondition* pc = static_cast<PositionCondition*>(spInstance->mpCondition);
    pc->SetRequiredPosition( atoi(argv[1] ) );
}

//=============================================================================
// MissionScriptLoader::SetCondTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetCondTime( int argc, char** argv )
{
    rAssert( spInstance->mpCondition->GetType() == MissionCondition::COND_PLAYER_OUT_OF_VEHICLE );
    GetOutOfCarCondition* goocc = static_cast<GetOutOfCarCondition*>(spInstance->mpCondition);
    goocc->SetTime( static_cast<unsigned int>(::atoi( argv[1] )) );
}

//=============================================================================
// MissionScriptLoader::SetHitNRun
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetHitNRun( int argc, char** argv )
{
    rAssert( spInstance->mpCondition->GetType() == MissionCondition::COND_TIME_OUT );
    TimeOutCondition* toc = static_cast<TimeOutCondition*>(spInstance->mpCondition);
    //toc->SetHitNRun();
}

//=============================================================================
// MissionScriptLoader::SetObjTargetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetObjTargetVehicle( int argc, char** argv )
{
    rAssert( spInstance->mpObjective != NULL );
    
    Vehicle* vehicle = NULL;

    if (strcmp("current",argv[1]) ==0)
    {
        vehicle = GetGameplayManager()->GetCurrentVehicle();
    }
    else
    {
        vehicle =  spInstance->GetVehicleByName( argv[ 1 ] );
    }

    rTuneAssertMsg( vehicle != NULL, "Could not find the requested target vehicle\n" );

    switch( spInstance->mObjType )
    {
    case MissionObjective::OBJ_FOLLOW:
        {
            FollowObjective* pObj = (FollowObjective*)(spInstance->mpObjective);
            pObj->SetTargetVehicle( vehicle );
            break;
        }
    case MissionObjective::OBJ_LOSETAIL:
        {
            LoseObjective* pObj = (LoseObjective*)(spInstance->mpObjective);
            pObj->SetTargetVehicle( vehicle );
            break;
        }
    case MissionObjective::OBJ_DESTROY:
        {
            DestroyObjective* pObj = (DestroyObjective*)(spInstance->mpObjective);
            pObj->SetTargetVehicle( vehicle );
            break;
        }
    case MissionObjective::OBJ_GETIN:
        {
//            GetInObjective* pObj = (GetInObjective*)(spInstance->mpObjective);
//            pObj->SetTargetVehicle( vehicle );
            break;
        }
    case MissionObjective::OBJ_DUMP:
        {
            CollectDumpedObjective* pObj = (CollectDumpedObjective*)(spInstance->mpObjective);
            pObj->SetDumpVehicle( vehicle );
            break;
        }
    default:
        {
            // fook!
            rTuneAssertMsg( false, "The current objective cannot accept a target vehicle\n" );
        }
    }
}

//=============================================================================
// MissionScriptLoader::SetObjTargetBoss
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetObjTargetBoss( int argc, char** argv )
{
    rAssert( spInstance->mpObjective != NULL );

    // Only supporting destroying right now
    rAssert( spInstance->mObjType == MissionObjective::OBJ_DESTROY_BOSS );

    rAssert( dynamic_cast< DestroyBossObjective* >( spInstance->mpObjective ) != NULL );
    DestroyBossObjective* pObj = (DestroyBossObjective*)(spInstance->mpObjective);
    // Ok, we are looking for a boss, which means UFO, which means
    // actormanager
    const char* bossName = argv[1];
    Actor* actor = GetActorManager()->GetActorByName( bossName );
    pObj->SetTarget( actor );
}

//=============================================================================
// MissionScriptLoader::SetPickupTarget
//=============================================================================
// Description: Set an object that the user must pickup in his vehicle
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetPickupTarget( int argc, char** argv )
{
    rAssert( spInstance->mObjType == MissionObjective::OBJ_PICKUP_ITEM );
    rAssert( dynamic_cast< PickupItemObjective* >( spInstance->mpObjective ) != NULL );
    PickupItemObjective* pObj = (PickupItemObjective*)(spInstance->mpObjective);   
    const char* targetname = argv[1];
    pObj->SetTarget( targetname );
}

//=============================================================================
// MissionScriptLoader::AllowRockOut
//=============================================================================
// Description: The PC character will rock out during this objective
//=============================================================================
void MissionScriptLoader::AllowRockOut( int argc, char** argv )
{
    spInstance->mpObjective->SetRockOut(true);
}



//=============================================================================
// MissionScriptLoader::SetObjDistance
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetObjDistance( int argc, char** argv )
{
    rAssert( spInstance->mpObjective != NULL );

    switch( spInstance->mObjType )
    {
    case MissionObjective::OBJ_LOSETAIL:
        {
            LoseObjective* pObj = (LoseObjective*)(spInstance->mpObjective);
            
            float dist = static_cast<float>( atoi( argv[ 1 ] ));
            pObj->SetDistance( dist );

            break;
        }
    default:
        {
            // fook!
            rTuneAssertMsg( false, "The current objective cannot accept a distance\n" );
        }
    }
}

//=============================================================================
// MissionScriptLoader::SetCondTargetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetCondTargetVehicle( int argc, char** argv )
{
    rAssert( spInstance->mpCondition != NULL );
    
    Vehicle* vehicle = NULL;
    if ( strncmp( argv[ 1 ], "current", 7 ) == 0 )
    {
        vehicle = GetGameplayManager()->GetCurrentVehicle();

    }
    else
    {
        vehicle = spInstance->GetVehicleByName( argv[ 1 ] );
    }

    rTuneAssertMsg( vehicle != NULL, "Could not find the requested target vehicle\n" );

    switch( spInstance->mCondType )
    {
    case MissionCondition::COND_FOLLOW_DISTANCE:
    case MissionCondition::COND_RACE:
    case MissionCondition::COND_VEHICLE_DAMAGE:
        {
            VehicleCondition* pCond = (VehicleCondition*)(spInstance->mpCondition);
            pCond->SetVehicle( vehicle );
            break;
        }
    default:
        {
            // fook!
            rTuneAssertMsg( false, "The current Condition cannot accept a target vehicle\n" );
        }
    }
}

//=============================================================================
// MissionScriptLoader::AddStage
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddStage( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage == NULL, "No stage active\n" );

    HeapMgr()->PushHeap( spInstance->mMissionHeap );
    MEMTRACK_PUSH_GROUP( "Mission - Stages" );

    if ( argc <= 3 )  //This should only be "final" and/or some number or nothing.
    {
        //This is a standard
        spInstance->mpStage = new MissionStage();

        for( int i = 1; i < argc; i++ )
        {
            if( strcmp(argv[ i ], "final") == 0 )
            {
                spInstance->mpStage->SetFinalStage(true);
                break;
            }
        }

        int index = spInstance->mpMission->GetNumStages();

        spInstance->mpMission->SetNumStages( index + 1 );
        spInstance->mpMission->SetStage( index, spInstance->mpStage );

        spInstance->mpObjective = NULL;
        spInstance->mpCondition = NULL;
    }
    else if ( argc == 4 || argc == 7 )
    {
        //This is a new type of add stage that allows for locked stages.
        unsigned int num = 0;
        unsigned int i;
        for ( i = 1; i < (MAX_LOCK_REQUIREMENTS * 3) + 1; i = i + 3 )
        {
            if ( strcmp(argv[i], "locked") == 0 )
            {
                //This is a locked stage.
                rAssert( strcmp(argv[i + 1], "skin") == 0 || strcmp(argv[i + 1], "car") == 0 );

                MissionStage::LockRequirement::Type type = MissionStage::LockRequirement::NONE;
                if ( strcmp(argv[i + 1], "skin") == 0 )
                {
                    type = MissionStage::LockRequirement::SKIN;
                }
                else if ( strcmp(argv[i + 1], "car") == 0 )
                {
                    type = MissionStage::LockRequirement::CAR;
                }
                else
                {
                    rTuneAssertMsg(false, "Invalid use of locked stage type\n");
                }

                if ( type != MissionStage::LockRequirement::NONE )
                {
                    if ( spInstance->mpStage == NULL )
                    {
                        spInstance->mpStage = new MissionStage();
                        int index = spInstance->mpMission->GetNumStages();

                        spInstance->mpMission->SetNumStages( index + 1 );
                        spInstance->mpMission->SetStage( index, spInstance->mpStage );

                        spInstance->mpObjective = NULL;
                        spInstance->mpCondition = NULL;
                    }

                    spInstance->mpStage->SetLockRequirement( num, type, argv[i + 2] );
                    ++num;
                }
            }
        }
    }
    else
    {
        rTuneAssertMsg( false, "Incorrect format of AddStage script command.  Get Cary\n" );
    }

    MEMTRACK_POP_GROUP( "Mission - Stages" );
    HeapMgr()->PopHeap(spInstance->mMissionHeap);
}

//=============================================================================
// MissionScriptLoader::SetStageMessageIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetStageMessageIndex( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

    int index = atoi( argv[ 1 ] );
    spInstance->mpStage->SetStartMessageIndex( index );
}

//=============================================================================
// MissionScriptLoader::SetStageTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetStageTime( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );
    int factor = GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_EXTRA_TIME) ? 5 : 1;
    spInstance->mpStage->SetStageTime( MissionStage::STAGETIME_SET, atoi( argv[ 1 ] ) * factor);
}

//=============================================================================
// MissionScriptLoader::AddStageTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddStageTime( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );
    int factor = GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_EXTRA_TIME) ? 5 : 1;
    spInstance->mpStage->SetStageTime( MissionStage::STAGETIME_ADD, atoi( argv[ 1 ] ) * factor);
}

//=============================================================================
// MissionScriptLoader::ShowStageComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::ShowStageComplete( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );
    spInstance->mpStage->ShowStageComplete( true );
    
}

//=============================================================================
// MissionScriptLoader::SetHUDIcon
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetHUDIcon( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

    spInstance->mpStage->SetHUDIcon( argv[ 1 ] );
}

//=============================================================================
// MissionScriptLoader::SetIrisWipe
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetIrisWipe( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

    spInstance->mpStage->SetIrisAtEnd();
}

//=============================================================================
// MissionScriptLoader::SetFadeOut
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetFadeOut( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "No stage active\n" );

    spInstance->mpStage->SetFadeOutAtEnd();
}

//=============================================================================
// MissionScriptLoader::CloseStage
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::CloseStage( int argc, char** argv )
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "Stage already closed!\n" );
    spInstance->mpStage = NULL;
}

void MissionScriptLoader::SetVehicleAIParams( int argc, char** argv )
{
#if (RAD_TUNE || RAD_DEBUG)
    char errMsg[256];
#endif

    rTuneAssertMsg( argc == 4, "Bad number of args for this command!\n"
        "SYNTAX: SetVehicleAIParams( vehiclename, minshortcutskill, maxshortcutskill )" );

    // parse the args
    char vehiclename[32];
    strcpy( vehiclename, argv[1] );

    MissionStage::AIParams params;
    params.minShortcutSkill = atoi( argv[2] );
    params.maxShortcutSkill = atoi( argv[3] );

    rAssert( spInstance );
    Vehicle* vehicle = spInstance->GetVehicleByName( vehiclename );

#if (RAD_TUNE||RAD_DEBUG)
    sprintf( errMsg, "SetVehicleAIParams: Cannot find vehicle called \"%s\"", vehiclename );
    rTuneAssertMsg( vehicle, errMsg );
#endif

#if (RAD_TUNE||RAD_DEBUG)
    sprintf( errMsg, "SetVehicleAIParams: This command is invalid because no stage is active." );
    rTuneAssertMsg( spInstance->mpStage, errMsg );
#endif

    spInstance->mpStage->SetAIParams( vehicle, params );

}
//=============================================================================
// MissionScriptLoader::SetBonusMissionStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetBonusMissionStart( int argc, char** argv )
{
    rAssert( spInstance->mpStage );

    spInstance->mpStage->MakeBonusObjectiveStart();
}

//=============================================================================
// MissionScriptLoader::GetLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* name )
//
// Return:      EventLocator
//
//=============================================================================
Locator* MissionScriptLoader::GetLocator( char* name )
{
    Locator* locator = p3d::find<Locator>( name );

    rTuneAssertMsg( locator != NULL, "Could not find the requested locator\n" );

    return( locator );
}

//=============================================================================
// MissionScriptLoader::GetVehicleOfName
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* name )
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* MissionScriptLoader::GetVehicleByName( const char* name )
{    
    // change this to look through GameplayManagers list:
    
    Vehicle* vehicle = GetGameplayManager()->GetMissionVehicleByName(name);
    
    if(vehicle == NULL)
    {
        // also look through the 'user' cars...
        vehicle = GetGameplayManager()->GetUserVehicleByName(name);
        
    }
    
    rTuneAssertMsg(vehicle, "missionscriptloader: can't find vehicle by name\n");

    return vehicle;

    /*
    if( spInstance->mpMission != NULL )
    {
        vehicle = spInstance->mpMission->GetVehicleByName( name );
    }

    if( vehicle == NULL )
    {
        for( int i = 0; i < GetVehicleCentral()->GetMaxActiveVehicles(); i++ )
        //for( int i = 0; i < GetVehicleCentral()->GetNumVehicles(); i++ )
        {
            Vehicle* v = GetVehicleCentral()->GetVehicle( i );
            if( v == NULL )
            {
                continue;
            }
            if( strcmp( v->GetName(), name ) == 0 )
            {
                vehicle = v;
                break;
            }
        }
    }

    return vehicle;
    */
}

//=============================================================================
// MissionScriptLoader::GetDirectionalArrowType
//=============================================================================
// Description: Returns enumeration based upon string fetched from a mission script
//
// Parameters:  const char* input string name. Ptr to output enumeration type
//
// Return:      bool indicating whether or not the arrow is of a valid type
//
//=============================================================================
bool MissionScriptLoader::GetDirectionalArrowType( const char* string, DirectionalArrowEnum::TYPE* outArrowType )
{
    if ( string == NULL )
        return false;

    bool validType = true;
    if ( strcmp( string, "BOTH") == 0 || 
         strcmp( string, "both" ) == 0 || 
         strcmp( string, "b" ) == 0 )
    {
        *outArrowType = DirectionalArrowEnum::BOTH;   
    }
    else if ( strcmp( string, "NEITHER") == 0 || 
              strcmp( string, "neither" ) == 0 ||
              strcmp( string, "n" ) == 0 )
    {
        *outArrowType = DirectionalArrowEnum::NEITHER;         
    }
    else if ( strcmp( string, "INTERSECTION") == 0 || 
              strcmp( string, "intersection" ) == 0 ||
              strcmp( string, "i" ) == 0 )
    {
        *outArrowType = DirectionalArrowEnum::INTERSECTION;                 
    }
    else if ( strcmp( string, "nearest road" ) == 0 ||
              strcmp( string, "NEAREST ROAD" ) == 0 ||
              strcmp( string, "n" ) == 0 )
    {
        *outArrowType = DirectionalArrowEnum::NEAREST_ROAD;                 
    }
    else
    {
        validType = false;
    }

    return validType;
}


//=============================================================================
// MissionScriptLoader::LoadP3DFile
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::LoadP3DFile( int argc, char** argv )
{
#ifdef MEMORYTRACKER_ENABLED
    char name[256];
    sprintf( name, "Mission - Load: %s", argv[1] );
#else
    char* name = "";
#endif

    // HACK HACK HACK : ignore loads ona bunch of stuff that we know we have preloaded
    if((strcmp(argv[ 1 ], "art\\phonecamera.p3d") == 0) ||
       (strcmp(argv[ 1 ], "art\\cards.p3d") == 0) ||
       (strcmp(argv[ 1 ], "art\\wrench.p3d") == 0) ||
       (strcmp(argv[ 1 ], "art\\missions\\generic\\missgen.p3d") == 0) ||
       (strcmp(argv[ 1 ], "art\\cars\\common.p3d") == 0) ||
       (strcmp(argv[ 1 ], "art\\cars\\huskA.p3d") == 0))
    {
        return;
    }


    GameMemoryAllocator heap = GMA_LEVEL_MISSION;

    //
    // tName allocates string space for the life of this function, 
    // so direct it to the temp heap
    //
    HeapMgr()->PushHeap( GMA_TEMP );
    tName heapName = argv[2];
    HeapMgr()->PopHeap( GMA_TEMP );
  
    if ( argc > 2 )
    {
        // 2nd argument is the heap, switch upon it
        if ( heapName == "GMA_DEFAULT" )
            heap = GMA_DEFAULT;
        else if ( heapName == "GMA_TEMP" )
            heap = GMA_TEMP;
        #ifdef RAD_GAMECUBE
        else if ( heapName == "GMA_GC_VMM" )
            heap = GMA_GC_VMM;
        #endif
        else if ( heapName == "GMA_PERSISTENT" )
            heap = GMA_PERSISTENT;
        else if ( heapName == "GMA_LEVEL" )
        {
            rAssert( false );
            heap = GMA_LEVEL;
        }
        else if ( heapName == "GMA_LEVEL_MOVIE" )
            heap = GMA_LEVEL_MOVIE;
        else if ( heapName == "GMA_LEVEL_FE" )
            heap = GMA_LEVEL_FE;
        else if ( heapName == "GMA_LEVEL_ZONE" )
            heap = GMA_LEVEL_ZONE;
        else if ( heapName == "GMA_LEVEL_OTHER" )
            heap = GMA_LEVEL_OTHER;
        else if ( heapName == "GMA_LEVEL_HUD" )
            heap = GMA_LEVEL_HUD;
        else if ( heapName == "GMA_LEVEL_MISSION" )
            heap = GMA_LEVEL_MISSION;
        else if ( heapName == "GMA_LEVEL_AUDIO" )
            heap = GMA_LEVEL_AUDIO;
        else if ( heapName == "GMA_DEBUG" )
            heap = GMA_DEBUG;
        else if ( heapName == "GMA_SPECIAL" )
            heap = GMA_SPECIAL;
        #ifdef RAD_XBOX
        else if ( heapName == "GMA_XBOX_SOUND_MEMORY" )
            heap = GMA_XBOX_SOUND_MEMORY;
        #endif
        else
        {
            // unrecognized heapName!
            rTuneAssertMsg( false, "Mission Script LoadP3DFile not given a valid heap!" );
        }
    }

    if ( argc == 4 )
    {
        GetLoadingManager()->AddRequest( spInstance->mFileHandler, argv[ 1 ], heap, argv[ 3 ], argv[ 1 ], 0, name );
    }
    else
    {
        GetLoadingManager()->AddRequest( spInstance->mFileHandler, argv[ 1 ], heap,  0, name );
    }
}


//=============================================================================
// MissionScriptLoader::LoadDisposableCar
//=============================================================================
// Description: Use this to load any vehicle that may get unloaded during a level gameplay session,
// ie player car,ai car, forced car into its own iventory section.
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::LoadDisposableCar( int argc, char** argv )
{    
    
    if (strcmp (argv[3],"DEFAULT")== 0)
    {
        //loading a default vehicle
        if ( GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].mp_vehicle !=NULL )
        {
            //problem designer wants to load a default car when one has already be specified in the script previously
            rAssert(0);
        }
        else
        {
            //load pure3d car into the inventory under its filename
            GetLoadingManager()->AddRequest( spInstance->mFileHandler, argv[ 1 ], GMA_LEVEL_MISSION, argv[1], "Default" );    
            strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].filename,argv[1]);
            strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].name,argv[2]);
        }

    }
    else if (strcmp (argv[3],"OTHER") == 0)
    {
        if( strcmp(GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].name,argv[2]) ==0)
        {
            rTuneAssertMsg(0, "why are you doing this - see greg \n");
            //we are trying to load a car that already exists,so do nothing
        }
        else
        {      
                //clear player default car

                GetGameplayManager()->DumpCurrentCar();
                GetGameplayManager()->ClearVehicleSlot(GameplayManager::eOtherCar);            
            
                //TO DO dump Phone booth car if the player is using it.                
                if ( GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle !=NULL )
                {
                    //problem default vehicle already defined time to unload it and load another in its place.
        #ifndef FINAL 
                    printf("other vehicle already in inventory, time to dispose of car \n");
        #endif

                    rTuneAssertMsg(0, "this is concerning to me - see greg \n");


                    //clear the inventory of old car             
                    GetGameplayManager()->ClearVehicleSlot(GameplayManager::eOtherCar);   
                    //clear player default car
                    GetGameplayManager()->ClearVehicleSlot(GameplayManager::eDefaultCar);
                    //TO DO dump Phone booth car if the player is using it.

                    strcpy(GetGameplayManager()->GetVehicleSlotFilename(GameplayManager::eOtherCar),argv[1]);
                    strcpy(GetGameplayManager()->GetVehicleSlotFilename(GameplayManager::eOtherCar),argv[2]);
                    //load pure3d car into the inventory under its filename
                    GetLoadingManager()->AddRequest( spInstance->mFileHandler, argv[ 1 ], GMA_LEVEL_MISSION, argv[1], "Default" );
            
                }
                else
                {
                    strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].filename,argv[1]);
                    strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].name,argv[2]);
                    //load pure3d car into the inventory under its filename
                    GetLoadingManager()->AddRequest( spInstance->mFileHandler, argv[ 1 ], GMA_LEVEL_MISSION, argv[1], "Default" );
            
                }
            }
    }
    else if (strcmp(argv[3],"AI")== 0)
    {
        
        //loading an AI car
        GetGameplayManager()->ClearVehicleSlot(GameplayManager::eAICar);
        strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eAICar].filename,argv[1]);
        strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eAICar].name,argv[2]);
        //load pure3d car into the inventory under its filename
        //TODO: change GMA_LEVEL_MISSION to GMA_LEVEL_HUD if we deicide to unload cars for FMV

        GetLoadingManager()->AddRequest( spInstance->mFileHandler, argv[ 1 ], GMA_LEVEL_MISSION, argv[1], "Default" );        
     
    }
    else if (strcmp(argv[3],"CHASE")==0)
    {
        //copy the filename for this chase car into the car data struct for the chasemanager 
        //will come in handy when we unload it.
        strcpy(GetGameplayManager()->m_ChaseManager_Array[0].hostvehiclefilename,argv[1]);
        GetLoadingManager()->AddRequest(spInstance->mFileHandler,argv[1],GMA_LEVEL_MISSION,"level_chase",argv[1]);
    }

    else
    {
        //loading unknown flag
        rAssert (0);
    }
}   
    


/*
==============================================================================
MissionScriptLoader::AddCharacter
==============================================================================
Description:    Comment

Parameters:     (int argc, char** argv )

Return:         void 

=============================================================================
*/
void MissionScriptLoader::AddCharacter(int argc, char** argv )
{
    rAssert( argc == 3 );
    GetCharacterManager( )->AddPCCharacter( argv[ 1 ], argv[ 2 ] );
}


//=============================================================================
// MissionScriptLoader::AddNPCCharacterBonusMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddNPCCharacterBonusMission( int argc, char** argv )
{
    char uniqueName[16];
    sprintf(uniqueName, "b_%s", argv[1]);
    
    Character* c = GetCharacterManager()->AddCharacterDeferedLoad( CharacterManager::NPC, uniqueName, argv[1], argv[2], argv[3] );
    c->AddToWorldScene();
    c->SetRole(Character::ROLE_ACTIVE_BONUS);

    if ( !GetGameplayManager()->mIsDemo )
    {
        bool isRace = atoi(argv[7]) == 1;

        // Try and retrieve the mission record
        const MissionRecord* missionRecord = GetCharacterSheetManager()->QueryStreetRaceStatus( GetGameplayManager()->GetCurrentLevelIndex(), argv[4] );
        if ( missionRecord == NULL )
            GetCharacterSheetManager()->QueryBonusMissionStatus( GetGameplayManager()->GetCurrentLevelIndex(), argv[4] );

        // Mission record is valid
        // See if this bonus mission or street race has already been done
        bool wasMissionAlreadyCompleted;
        if ( missionRecord != NULL )
        {
            wasMissionAlreadyCompleted = missionRecord->mCompleted;
        }
        else
        {
            wasMissionAlreadyCompleted = false;
        }


        if ( argc == 8 )
        {
            GetGameplayManager()->SetBonusMissionInfo( uniqueName, argv[4], argv[5], argv[6], isRace, NULL, wasMissionAlreadyCompleted );
        }
        else if ( argc > 8 )
        {
            GetGameplayManager()->SetBonusMissionInfo( uniqueName, argv[4], argv[5], argv[6], isRace, argv[8], wasMissionAlreadyCompleted );
        }
    }
}

//=============================================================================
// MissionScriptLoader::SetBonusMissionDialoguePositions
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetBonusMissionDialoguePositions( int argc, char** argv )
{
    if ( GetGameplayManager()->mIsDemo )
    {
        return;
    }

    int missionNum = GetGameplayManager()->GetMissionNumByName( argv[1] );

    rTuneAssertMsg( missionNum != -1, "Trying to set character positions on non-existant mission\n" );
    rTuneAssertMsg( missionNum >= GameplayManager::MAX_MISSIONS, "Trying to set bonus mission positions on regular mission\n");

    if ( missionNum != -1 && missionNum >= GameplayManager::MAX_MISSIONS )
    {
        CarStartLocator* char1Pos = p3d::find<CarStartLocator>( argv[2] );

#ifndef RAD_RELEASE
        if ( !char1Pos )
        {
            char error[128];
            sprintf( error, "Could not find %s for dialogue position\n", argv[2] );
            rTuneAssertMsg( false, error );
        }
#endif

        CarStartLocator* char2Pos = p3d::find<CarStartLocator>( argv[3] );

#ifndef RAD_RELEASE
        if ( !char2Pos )
        {
            char error[128];
            sprintf( error, "Could not find %s for dialogue position\n", argv[3] );
            rTuneAssertMsg( false, error );
        }
#endif

        CarStartLocator* carPos = NULL;
        if ( argc >= 5 )
        {
            //Also need a car start
            carPos = p3d::find<CarStartLocator>( argv[4] );

#ifndef RAD_RELEASE
            if ( !carPos )
            {
                char error[128];
                sprintf( error, "Could not find %s for dialogue position\n", argv[4] );
                rTuneAssertMsg( false, error );
            }
#endif
        }

        BonusMissionInfo* bmi = GetGameplayManager()->GetBonusMissionInfo( missionNum - GameplayManager::MAX_MISSIONS );
        bmi->SetPositions( char1Pos, char2Pos, carPos );
    }
}


//=============================================================================
// MissionScriptLoader::AddAmbientCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddAmbientCharacter( int argc, char** argv )
{
    Locator* loc = p3d::find<Locator>( argv[2] );
    if(!loc)
    {
        char error[256];
        sprintf( error, "Can not find locator: %s, Designer Error!!!!\n", argv[2] );
        MissionScriptLoader::TreeOfWoeErrorMsg(error);
        rTuneAssertMsg(0, error );
    }

    Character* c = GetCharacterManager()->AddCharacterDeferedLoad( CharacterManager::NPC, argv[1], argv[1], "npd", argv[2] );
    c->SetAmbient(argv[2], (argc == 4) ? (float)atof(argv[3]) : 1.3f);
    c->AddToWorldScene();
}

void MissionScriptLoader::AddBonusMissionNPCWaypoint( int argc, char** argv )
{
    // Check syntax: commandname, NPCname, locatorname
    rTuneAssertMsg( argc == 3, "AddBonusMissionNPCWaypoint takes 2 args: NPCName, locatorName" ); 

    // grab the two arguments
    rTuneAssert( argv[1] );
    const char* locName = argv[2];
    rTuneAssert( locName );


    // Bonus mission characters have "b_" prefix... so prepend it before
    // calling CharacterManager::GetCharacterByName.
    char npcName[64];
    sprintf( npcName, "b_%s", argv[1] );

    bool succeeded = AddNPCWaypoint( npcName, locName );
    rTuneAssertMsg( succeeded, "Failed adding waypoint for a Bonus Mission NPC" );
}

void MissionScriptLoader::AddObjectiveNPCWaypoint( int argc, char** argv )
{
#if (RAD_TUNE || RAD_DEBUG)
    char errMsg[ 256 ];
#endif

    // Check syntax: commandname, NPCname, locatorname
    rTuneAssertMsg( argc == 3, "AddObjectiveNPCWaypoint takes 2 args: NPCName, locatorName" ); 

    // grab the two arguments
    const char* npcName = argv[1];
    rTuneAssert( npcName );
    const char* locName = argv[2];
    rTuneAssert( locName );

    // First look for the locator
    Locator* loc = p3d::find< Locator >( locName );
    if( loc == NULL )
    {
#if (RAD_TUNE || RAD_DEBUG)
        sprintf( errMsg, "Couldn't find locator %s for NPC %s", locName, npcName );
        rTuneAssertMsg( false, errMsg );
#endif 
        return;
    }
    rmt::Vector locPos;
    loc->GetPosition( &locPos );

    // Now, look for the character

    // see if we have an objective.. if so, we search through the objective first
    // to update its mNPCs member
    MissionScriptLoader* msl = GetInstance();
    rAssert( msl );

    if( msl->mpObjective )
    {
        bool foundInObjective = msl->mpObjective->AddNPCWaypoint( npcName, locName );

        // if we couldn't add it to the objective or if the NPC does not exist in the
        // objective, we search for it in Character manager...
        if( !foundInObjective )
        {
#if (RAD_TUNE || RAD_DEBUG)
            sprintf( errMsg, "Waypoint at locator %s could not be "
                "added for mission objective NPC %s", locName, npcName );
            rTuneAssertMsg( false, errMsg );
#endif
            return;
        }
    }
    else
    {
        // objective doesn't exist... But this is an objective NPC!!
#if (RAD_TUNE || RAD_DEBUG)
        sprintf( errMsg, "Mission Objective undefined before calling this command!" );
        rTuneAssertMsg( false, errMsg );
#endif 
        return;
    }
}

bool MissionScriptLoader::AddNPCWaypoint( const char* npcName, const char* locName )
{
#if (RAD_TUNE || RAD_DEBUG)
    char errMsg[256];
#endif

    // First look for the locator
    Locator* loc = p3d::find< Locator >( locName );
    if( loc == NULL )
    {
#if (RAD_TUNE || RAD_DEBUG)
        sprintf( errMsg, "Couldn't find locator %s for NPC %s", locName, npcName );
        rTuneAssertMsg( false, errMsg );
#endif 
        return false;
    }
    rmt::Vector locPos;
    loc->GetPosition( &locPos );

    // Now, look for the character in CharacterManager
    Character* character = GetCharacterManager()->GetCharacterByName( npcName );
    if( character == NULL )
    {
#if (RAD_TUNE || RAD_DEBUG)
        sprintf( errMsg, "NPC %s does not exist in character manager", npcName );
        rTuneAssertMsg( false, errMsg );
#endif
        return false;
    }

    // Better make sure it is also an NPC
    rAssert( character->IsNPC() );
    NPCharacter* npc = (NPCharacter*) character;
    
    // Make sure the controller exists (gets created in NPC constructor)
    NPCController* npcController = (NPCController*) npc->GetController();
    rAssert( npcController );

    bool succeeded = npcController->AddNPCWaypoint( locPos );
    if( !succeeded )
    {
#if (RAD_TUNE || RAD_DEBUG)
        sprintf( errMsg, "Could not add waypoint at %s for NPC %s", locName, npcName );
        rTuneAssertMsg( false, errMsg );
#endif
        return false;
    }
    return true;
}


void MissionScriptLoader::AddPurchaseCarNPCWaypoint( int argc, char** argv )
{

    // Check syntax: commandname, NPCname, locatorname
    rTuneAssertMsg( argc == 3, "AddPurchaseCarNPCWaypoint takes 2 args: NPCName, locatorName" ); 

    // grab the two arguments
    rTuneAssert( argv[1] );
    const char* locName = argv[2];
    rTuneAssert( locName );

    // Purchase car NPCs are named differently
    char npcName[64];
    sprintf( npcName, "reward_%s", argv[1] );

    bool succeeded = AddNPCWaypoint( npcName, locName );
    rTuneAssertMsg( succeeded, "Failed to add waypoint for Purchase Car Reward NPC." );
}

void MissionScriptLoader::AddAmbientNPCWaypoint( int argc, char** argv )
{
    // Check syntax: commandname, NPCname, locatorname
    rTuneAssertMsg( argc == 3, "AddAmbientNPCWaypoint takes 2 args: NPCName, locatorName" ); 

    // grab the two arguments
    const char* npcName = argv[1];
    rTuneAssert( npcName );
    const char* locName = argv[2];
    rTuneAssert( locName );

    bool succeeded = AddNPCWaypoint( npcName, locName );
    rTuneAssertMsg( succeeded, "Failed to add waypoint for Ambient NPC." );
}

//=============================================================================
// MissionScriptLoader::ActivateTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::ActivateTrigger( int argc, char** argv )
{
    Locator* loc;
    
    loc = p3d::find<Locator>( argv[1] );

    rAssert( loc );

    loc->SetFlag( Locator::ACTIVE, true );
}

//=============================================================================
// MissionScriptLoader::DeactivateTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::DeactivateTrigger( int argc, char** argv )
{
    Locator* loc;
    
    loc = p3d::find<Locator>( argv[1] );

    rAssert( loc );

    loc->SetFlag( Locator::ACTIVE, false );
}
/*
//=============================================================================
// MissionScriptLoader::InitAIVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::InitAIVehicle(  int argc, char** argv )
{
    char locator[16], vehicleName[16], aitype[16];
    strcpy( vehicleName, argv[ 1 ]);
    strcpy( aitype, argv[ 2 ]);
    strcpy( locator, argv[ 3 ]);

    Vehicle* vehicle = GetVehicleCentral()->InitVehicle( vehicleName, false );

    GetGameplayManager()->PlaceAtLocatorName( vehicle, locator );
    
    GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->AddGuts((tDrawable*)( vehicle ) );
    
    VehicleAI* pAI;
    if( strcmp( aitype, "follow") == 0 )
    {
        pAI = new ChaseAI( vehicle );
        ((ChaseAI*)pAI)->SetTarget( GetVehicleCentral()->GetVehicle( 0 ) );
    }
    else if( strcmp( aitype, "waypoint" ) == 0 )
    {
        pAI = new WaypointAI( vehicle );
    }
    else
    {
        char buffy[64];
        sprintf( buffy, "Unknown AI vehicle type: %s", aitype );
        rTuneAssertMsg( false, buffy );
    }
    GetVehicleCentral()->SetVehicleController( GetVehicleCentral()->GetVehicleId( vehicle ), pAI );
    
    pAI->Initialize();

    spInstance->mpCurrentVehicleAI = pAI;
}

//=============================================================================
// MissionScriptLoader::AddAIWaypoint
//=============================================================================
// Description: Comment
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddAIWaypoint(  int argc, char** argv )
{
    rAssert( spInstance->mpCurrentVehicleAI );

    Locator* loc = p3d::find<Locator>( argv[1] );
    rAssert( loc );

    WaypointAI* wpAI = dynamic_cast<WaypointAI*>( spInstance->mpCurrentVehicleAI );
    rTuneAssertMsg( wpAI != NULL, "This is not a waypoint following vehicle AI\n" );

    wpAI->AddWaypoint( loc );
}
*/
/*
==============================================================================
MissionScriptLoader::CreateAnimPhysObject
==============================================================================
Description:    Comment

Parameters:     ( int argc, char** argv )

Return:         void 

=============================================================================
*/
void MissionScriptLoader::CreateAnimPhysObject( int argc, char** argv )
{
// No longer neccessary; loading being done uniformly now.
// You have been assimilated.
    rTuneAssertMsg( false, "No longer neccessary; loading being done uniformly now.  You have been assimilated.\n" );
}

void MissionScriptLoader::CreateActionEventTrigger( int argc, char** argv )
{
//const char* triggerName, rmt::Vector& pos, float r );
    rmt::Vector pos;
    pos.x = static_cast<float>( atoi( argv[ 2 ] ) );
    pos.y = static_cast<float>( atoi( argv[ 3 ] ) );
    pos.z = static_cast<float>( atoi( argv[ 4 ] ) );

    float r = static_cast<float>( atoi( argv[ 5 ] ) );

    GetActionButtonManager()->CreateActionEventTrigger( argv[1], pos, r );
}
void MissionScriptLoader::LinkActionToObjectJoint( int argc, char** argv )
{
    //const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName );

    GetActionButtonManager()->LinkActionToObjectJoint( argv[1], argv[2], argv[3], argv[4], argv[5] );
}
void MissionScriptLoader::LinkActionToObject( int argc, char** argv )
{
    //const char* objectName, const char* triggerName, const char* typeName, const char* buttonName = (const char*)0 );
    
    GetActionButtonManager()->LinkActionToObject( argv[1], argv[2], argv[3], argv[4], argv[5], false );
}

void MissionScriptLoader::SetCoinDrawable( int argc, char** argv )
{
    rAssert( argc >= 1 );
    tDrawable* coinDrawable = p3d::find<tDrawable>( argv[ 1 ] );
    GetCoinManager()->SetCoinDrawable( coinDrawable );
}

void MissionScriptLoader::SetParticleTexture( int argc, char** argv )
{
    tTexture* t = p3d::find<tTexture>( argv[ 2 ] );
    int i = atoi( argv[ 1 ] );
    GetSparkleManager()->SetTexture( i, t );
}

//adds a safezone to a stage
void MissionScriptLoader::AddSafeZone(int argc, char** argv)
{
    Locator* locator = NULL;
    
    locator=p3d::find<Locator> (argv[1]) ;
    int radius =10;

    radius= atoi(argv[2]);
    
    rAssert( dynamic_cast< CarStartLocator* > (locator ) != NULL );
    CarStartLocator* carstartlocator = static_cast < CarStartLocator*>(locator);

    rAssert(carstartlocator);
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: AddSafeZone needs to be in a stage \n" );
    spInstance->mpStage->AddSafeZone(carstartlocator,radius);
}




//create a new ped group 
void MissionScriptLoader::CreatePedGroup(int argc,char** argv)
    {
        rAssert(argc == 2);
        int groupid = atoi(argv[1]);
        GetMissionScriptLoader()->mp_ModelGroup = new PedestrianManager::ModelGroup;
        GetMissionScriptLoader()->mPedGroupID = groupid;
        GetMissionScriptLoader()->mp_ModelGroup->numModels =0;
    }

void  MissionScriptLoader::AddPed(int argc,char** argv)
    {
        //if you asserted here then the wrong number of arguements have been passed
        rAssert(argc == 3);
        //check if group is full, if its full then dont add the ped
        if (spInstance->mp_ModelGroup->numModels == PedestrianManager::MAX_MODELS)
            {
                //you asserted here cause your adding too many peds to a group.
                rTuneAssert(0);
               
            }
        else
            {  
                char name [64];
                int max_number;
                strncpy(name,argv[1],64);
                name[63]='\0';
                max_number = atoi(argv[2]);
                spInstance->mp_ModelGroup->models[spInstance->mp_ModelGroup->numModels].Init(name,max_number);
                spInstance->mp_ModelGroup->numModels++;
            }
    }

//Closes a ped grp.
void MissionScriptLoader::ClosePedGroup(int argc, char** argv)
    {
        if (spInstance->mp_ModelGroup->numModels == 0 )
            {
                //if your asserted here, then the group has no peds, smack the designer.
                rTuneAssert(0);
            }

        PedestrianManager::GetInstance()->SetModelGroup(spInstance->mPedGroupID, *(spInstance->mp_ModelGroup));
        spInstance->mPedGroupID = -1;
        delete spInstance->mp_ModelGroup;
        spInstance->mp_ModelGroup =NULL;
    }


void MissionScriptLoader::UsePedGroup( int argc, char** argv )
{
    rTuneAssert(argc == 2);
    int groupId = atoi(argv[1]);

    rTuneAssertMsg( 0 <= groupId && groupId < PedestrianManager::MAX_MODEL_GROUPS, 
        "Invalid groupId" );

    spInstance->mpMission->SetInitPedGroup( groupId );
}

void MissionScriptLoader::BindReward (int argc,char** argv)
    {

        char name [16];
        char filename [64];
        int level =0;
        Reward::eQuestType qtype = Reward::eBlank;
        Reward::eRewardType rtype = Reward::eNULL;

        //Arguement check
        //rTuneAssertMsg((argc >= 6 && argc <= 8)  ,"Incorrect Number of Arguements passed \n");
       
        strncpy (name,argv[1],16);
        name[15] = '\0';
        strncpy (filename,argv[2],64);
        filename[63] = '\0';
        level =atoi (argv[5]);

        //decrement level since all the internal arrays for levels are 0-6, F#@King level enums!!
        level--;

        //check for the reward type
        if ( strcmp (argv[3],"skin") ==0)
        {
            rtype = Reward::ALT_SKIN_OTHER;
        }
        else if (strcmp (argv[3],"car") ==0)
        {
            rtype = Reward::ALT_PLAYERCAR;
        }
        else if (strcmp (argv[3],"fe_toy") ==0)
        {
            rtype = Reward::FE_TOY;
        }
        else
        {
            rTuneAssertMsg(0,"ERROR: Unknown Reward type! \n");
        }

        bool isMerchandise = false;

        //check for quest type
        if (strcmp (argv[4],"streetrace") ==0)
        {
            qtype = Reward::eStreetRace;
        }
        else if (strcmp (argv[4],"bonusmission") ==0)
        {
            qtype =Reward::eBonusMission;
        }
        else if (strcmp (argv[4],"cards") ==0)
        {
            qtype = Reward::eCards;
        }
        else if (strcmp (argv[4],"goldcards") ==0)
        {
            qtype = Reward::eGoldCards;
        }
        else if (strcmp (argv[4],"forsale") ==0)
        {
            isMerchandise = true;
        }
        else if (strcmp (argv[4],"defaultcar") == 0)
        {
            qtype = Reward::eDefaultCar;
        }
        else if (strcmp (argv[4],"defaultskin") == 0)
        {
            qtype = Reward::eDefaultSkin;
        }
        else
        {
            printf("ERROR: Unknown Quest type %s !\n",argv[4]);
        }
        
        if( isMerchandise )
        {
            rTuneAssertMsg( argc == 8, "Invalid number of arguments!" );

            int cost = atoi(argv[6]);

            Merchandise::eSellerType sellerType = Merchandise::INVALID_SELLER_TYPE;

            if( strcmp( argv[ 7 ], "interior" ) == 0 )
            {
                sellerType = Merchandise::SELLER_INTERIOR;
            }
            else if( strcmp( argv[ 7 ], "simpson" ) == 0 )
            {
                sellerType = Merchandise::SELLER_SIMPSON;
            }
            else if( strcmp( argv[ 7 ], "gil" ) == 0 )
            {
                sellerType = Merchandise::SELLER_GIL;
            }
            else
            {
                rTuneAssertMsg( false, "Invalid seller type!" );
            }

            GetRewardsManager()->AddMerchandise(name,filename,rtype,level,cost,sellerType);
        }
        else
        {
            //update the RewardsManager
            GetRewardsManager()->BindReward(name,filename,rtype,qtype,level);
        }


    }//end of Bind Reward


//use to set the cars attributes displayed in the phone booth interface
void MissionScriptLoader::SetCarAttributes(int argc,char** argv)
{
    char CarName [16] = "NULL";

    //check the correct number of parameters
    rTuneAssertMsg (argc == 6,"Incorrect number of parameter for the Set Car Attribute\n");

    //check the name length
    if(strlen(argv[1]) >16)
    {
        rTuneAssertMsg(0,"Name is too long \n");
    }
    strncpy(CarName,argv[1],16);
    CarName[15]='\0';

    float speed = static_cast<float>( atof(argv[2]) );
    float acceleration = static_cast<float>( atof(argv[3]) );
    float toughness= static_cast<float>( atof(argv[4]) );
    float stability = static_cast<float>( atof(argv[5]) );

    GetRewardsManager()->SetCarAttributes(CarName,speed,acceleration,toughness,stability);
}

//=============================================================================
// MissionScriptLoader::SetTotalGags
//=============================================================================
// Description: 
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetTotalGags(int argc,char ** argv)
{
    rAssert( argc >= 2 );
    int level = atoi( argv[ 1 ] ) - 1;
    int numGags = atoi( argv[ 2 ] );

    GetRewardsManager()->SetTotalGags( level, numGags );
}

//=============================================================================
// MissionScriptLoader::SetTotalWasps
//=============================================================================
// Description: 
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetTotalWasps(int argc,char ** argv)
{
    rAssert( argc >= 2 );
    int level = atoi( argv[ 1 ] ) - 1;
    int numWasps = atoi( argv[ 2 ] );

    GetRewardsManager()->SetTotalWasps( level, numWasps );
}

//create a new traffic group 
void MissionScriptLoader::CreateTrafficGroup(int argc,char** argv)
    {
        rAssert(argc == 2);
        int groupid = atoi(argv[1]);
        spInstance->mp_TrafficGroup=TrafficManager::GetInstance()->GetTrafficModelGroup(groupid);
        rTuneAssert(spInstance->mp_TrafficGroup);
        spInstance->mp_TrafficGroup->ClearGroup();
    }

void  MissionScriptLoader::AddTrafficModel(int argc,char** argv)
    {
        //check if group is full, if its full then dont add the ped
        {
                char name [64];
                int max_number;
                strncpy(name,argv[1],64);
                name[63]='\0';
                max_number = atoi(argv[2]);
                spInstance->mp_TrafficGroup->AddTrafficModel(name,max_number);               
        }

        if ((GetGameFlow()->GetCurrentContext () == CONTEXT_GAMEPLAY) || 
            (GetGameFlow()->GetCurrentContext () == CONTEXT_LOADING_GAMEPLAY))
        {
            bool isBig = false;
            if ( argc == 4 )
            {
                isBig = (atoi( argv[3] ) == 1);
            }
        
            if ( !isBig )
            {
                //Add this to the Parked Car Manager
                GetPCM().AddCarType( argv[1] );
            }
        }
    }

//Closes a traffic grp.
void MissionScriptLoader::CloseTrafficGroup(int argc, char** argv)
    {        
        spInstance->mp_TrafficGroup =NULL;
    }

    
//setting the respawn vaules inside the respawn manager
void MissionScriptLoader::SetRespawnRate(int argc,char ** argv)
{
    int seconds =0;
    seconds = atoi(argv[2]);
    char item[32];
    strncpy(item,argv[1],32);
    item[31] ='\0';

    seconds = seconds * 1000; //change seconds into milliseconds

    if (strcmp(item,"wrench") == 0)
    {
        GetGameplayManager()->GetRespawnManager()->SetWrenchRespawnTime(seconds);
    }
    else if (strcmp(item,"nitro")==0)
    {
        GetGameplayManager()->GetRespawnManager()->SetNitroRespawnTime(seconds);
    }
    else if (strcmp(item,"wasp")==0)
    {
        GetGameplayManager()->GetRespawnManager()->SetWaspRespawnTime(seconds);
    }
    else
    {
        rReleasePrintf("Unknown %s item type \n",item);
    }
}

//=============================================================================
// MissionScriptLoader::EnableTutorialMode
//=============================================================================
// Description: Triggers the tutorial mode to be enabled
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::EnableTutorialMode(int argc,char ** argv)
{
    int enableInt = atoi( argv[ 1 ] );
    bool enable = ( enableInt != 0 );
#ifdef RAD_WIN32

    if( !GetInputManager()->GetController(0)->IsTutorialDisabled() )
    {
#endif       
        TutorialManager::GetInstance()->EnableTutorialMode( enable );
#ifdef RAD_WIN32
    }
#endif 
}

//=============================================================================
// MissionScriptLoader::StartCountdown
//=============================================================================
// Description: starts the 321go countdown
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::StartCountdown(int argc,char ** argv)
{
    tUID secondUID;

    if( argc == 3 )
    {
        secondUID = tEntity::MakeUID( argv[2] );
    }
    else
    {
        secondUID = 0;
    }

    spInstance->mpStage->SetCountdownEnabled( radMakeKey32( argv[ 1 ] ), secondUID );
}

//=============================================================================
// MissionScriptLoader::AddToCountdownSequence
//=============================================================================
// Description: 
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::AddToCountdownSequence(int argc,char ** argv)
{
    if( argc > 2 )
    {
        spInstance->mpStage->AddCountdownSequenceUnit( argv[ 1 ],
                                                       atoi( argv[ 2 ] ) );
    }
    else
    {
        spInstance->mpStage->AddCountdownSequenceUnit( argv[ 1 ] );
    }
}

//=============================================================================
// MissionScriptLoader::SetCarStartCamera
//=============================================================================
// Description: this is the camera that will be used when a car is loaded from
//              a phone booth
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::SetCarStartCamera( int argc, char** argv )
{
}

//=============================================================================
// MissionScriptLoader::GoToPattyAndSelmaScreenWhenDone
//=============================================================================
// Description: should we trigger the patty and selma screen when we're done
//
// Parameters:  (  int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::GoToPattyAndSelmaScreenWhenDone( int argc, char** argv )
{
    spInstance->mpStage->GoToPattyAndSelmaScreenWhenDone();
}

//=============================================================================
// MissionScriptLoader::StreetRacePropsLoad
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::StreetRacePropsLoad( int argc, char** argv )
{
     spInstance->mpMission->LoadStreetRaceProps( argv[ 1 ] );
}


//=============================================================================
// MissionScriptLoader::StreetRacePropsUnload
//=============================================================================
// Description: Comment
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================
void MissionScriptLoader::StreetRacePropsUnload( int argc, char** argv )
{
     spInstance->mpMission->UnloadStreetRaceProps( argv[ 1 ] );
}


void MissionScriptLoader::UseElapsedTime(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: UseElapsedTime needs to be in a stage \n" );
    spInstance->mpStage->UseElapsedTime();
}

    
void MissionScriptLoader::AttachStatePropCollectible(int argc,char** argv)
{
    HeapMgr()->PushHeap( GMA_LEVEL_MISSION );

    // Create a new statepropcollectible and attach it to the current 
    // player vehicle
    const char* vehicleName = argv[1];
    const char* statepropName = argv[2];

    rAssert( spInstance != NULL );
    spInstance->mpMission->AttachStatePropCollectible( statepropName, vehicleName, 2 );

    HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
}

void MissionScriptLoader::ShowHUD(int argc,char** argv)
{
    bool isShowHUD = ( strcmp( argv[ 1 ], "false" ) != 0 );

    rAssert( spInstance != NULL && spInstance->mpMission != NULL );
    spInstance->mpMission->ShowHUD( isShowHUD );
}

void MissionScriptLoader::SetNumValidFailureHints( int argc, char** argv )
{
    rAssert( spInstance != NULL && spInstance->mpMission != NULL );
    spInstance->mpMission->SetNumValidFailureHints( atoi( argv[ 1 ] ) );
}

//used to set the ParTime for Gamble Missions ONLY!!!!
void MissionScriptLoader::SetParTime(int argc,char** argv)
{
    int i =0;
    //first check that this we have a race objective first
    if  (spInstance->mObjType == MissionObjective::OBJ_RACE) 
    {
        RaceObjective* pRaceObjective = dynamic_cast <RaceObjective*> (spInstance->mpObjective);
        
        //check if its a gamble race
        if (pRaceObjective->QueryIsGambleRace() == true)
        {
            pRaceObjective->SetParTime(atoi(argv[1]));
        }
        else
        {        
            rTuneAssertMsg(0,"ERROR: You CANNOT set a par time  for a  NON GAMBLE Race Objective!!!!\n");
        }
    }
    else
    {
         rTuneAssertMsg(0,"ERROR: You CANNOT set a par time for a  NON  RACE Objective!!!!\n");
    }
}


//Sets The entery fee for a stage.
void MissionScriptLoader::SetRaceEnteryFee(int argc, char** argv)
{
    int enteryfee = -1;
    enteryfee=atoi(argv[1]);
    if(enteryfee < 0 )
    {
        rTuneAssertMsg(0,"ERROR: Entery Fee MUST be at least GREATER than 1 coin, Double Check your Script!!\n");
    }
    else
    {
        spInstance->mpStage->SetRaceEnteryFee(enteryfee);
    }
}


//Set the Coins Fee Required to Pass a Coin Objective 
void MissionScriptLoader::SetCoinFee(int argc,char** argv)
{
    int coinfee=0;
    coinfee=atoi(argv[1]);
    if (coinfee < 0)
    {
        rTuneAssertMsg(0,"ERROR: Entery Fee MUST be at least GREATER than 1 coin, Double Check your Script!!\n");
    }
    else
    {
        if ( spInstance->mObjType == MissionObjective::OBJ_COIN)
        {
            CoinObjective* pCoinObjective = dynamic_cast <CoinObjective*> (spInstance->mpObjective);
            rAssert(pCoinObjective);
            pCoinObjective->SetCoinFee(coinfee);
            
        }
    }    
}


//Force the MF player in their car on mission stage start
void MissionScriptLoader::PutMFPlayerInCar(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: PutMFPlayerInCar needs to be in a stage \n" );
    spInstance->mpStage->PutMFPlayerInCar();
}

// Set which element of a stateprop is to be treated as the shadow (or light pool) 
void MissionScriptLoader::SetStatepropShadow(int argc,char** argv)
{
    const char* compDrawName = argv[1];
    const char* compDrawElement = argv[2];

    AnimDynaPhysLoader::SetShadowElement( compDrawName, compDrawElement );
}


void MissionScriptLoader::TreeOfWoeErrorMsg(const char* outputbuffer)
{
#ifndef RAD_XBOX
#ifndef FINAL
    IRadTextDisplay* textDisplay;

      ::radTextDisplayGet( &textDisplay, GMA_DEFAULT );

     if ( textDisplay )
    {
        textDisplay->SetBackgroundColor( 0 );
        textDisplay->SetTextColor( 0xffffffff );
        textDisplay->Clear();
        textDisplay->TextOutAt("Another Body for The Tree Of Woe", 5, 2 );
        textDisplay->TextOutAt(outputbuffer, 5, 5 );
        textDisplay->SwapBuffers();
        textDisplay->Release();
    }
#endif
#endif
}


void MissionScriptLoader::DisableHitAndRun(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: DisableHitAndRun needs to be in a stage \n" );
    spInstance->mpStage->DisableHitAndRun();
    GetHitnRunManager()->DisableHitnRun();
}

void MissionScriptLoader::EnableHitAndRun(int argc,char** argv)
{
    GetHitnRunManager()->EnableHitnRun();
}

void MissionScriptLoader::ResetHitAndRun(int argc,char** argv)
{
    GetHitnRunManager()->ResetState();
}

void MissionScriptLoader::SetNumChaseCars(int argc,char** argv)
{
    int numofcars =0;
    numofcars = atoi(argv[1]);
    GetHitnRunManager()->SetNumChaseCars(numofcars);
}

void MissionScriptLoader::SetHitAndRunDecay(int argc,char** argv)
{
    float value = 0.0f;
    value = static_cast<float>(atof(argv[1]));
    GetHitnRunManager()->SetDecayRate(value);
}

void MissionScriptLoader::SetHitAndRunDecayInterior(int argc,char** argv)
{
    float value = 0.0f;
    value = static_cast<float>(atof(argv[1]));
    GetHitnRunManager()->SetDecayRateInside(value);
}

void MissionScriptLoader::SetHitAndRunMeter(int argc,char** argv)
{
    float value = 0.0f;
    value = static_cast<float>(atof(argv[1]));
    GetHitnRunManager()->SetHitnRunValue(value);
}

void MissionScriptLoader::SwapInDefaultCar(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: SwapInDefaultCar needs to be in a stage \n" );
    spInstance->mpStage->SwapInDefaultCar();
}


void MissionScriptLoader::SetSwapDefaultCarRespawnLocatorName(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: SetSwapDefaultCarLocator needs to be in a stage \n" );
    spInstance->mpStage->SetSwapDefaultCarRespawnLocatorName(argv[1]);
}

void MissionScriptLoader::SetSwapForcedCarRespawnLocatorName(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: SetSwapForcedCarLocator needs to be in a stage \n" );
    spInstance->mpStage->SetSwapForcedCarRespawnLocatorName(argv[1]);
}

void MissionScriptLoader::SetSwapPlayerRespawnLocatorName(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: SetSwapPlayerLocator needs to be in a stage \n" );
    spInstance->mpStage->SetSwapPlayerRespawnLocatorName(argv[1]);
}


void MissionScriptLoader::NoTrafficForStage(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command:NoTrafficForStage needs to be in a stage \n" );
    spInstance->mpStage->DisableTraffic();
}


void MissionScriptLoader::ClearTrafficForStage(int argc,char** argv)
{
    rTuneAssertMsg( spInstance->mpStage != NULL, "This Command: ClearTrafficForStage needs to be in a stage \n" );
    spInstance->mpStage->ClearTrafficForStage();
}


void MissionScriptLoader::AddGlobalProp(int argc,char** argv)
{
    //GetGlobalPropManager()->AddProp(argv[1]));
}

void MissionScriptLoader::PlacePlayerAtLocatorName(int argc,char** argv)
{
    rTuneAssertMsg(spInstance->mpStage != NULL," You must USE: PlacePlayerAtLocatorName inside the Stage Command!! \n");
    spInstance->mpStage->SetPlayerRespawnLocatorName(argv[1]);
}

void MissionScriptLoader::msPlacePlayerCarAtLocatorName(int argc,char** argv)
{
    rTuneAssertMsg(spInstance->mpStage != NULL," You must USE: msPlacePlayerCarAtLocatorName inside the Stage Command!! \n");
    spInstance->mpStage->SetmsPlayerCarRespawnLocatorName(argv[1]);
}



void MissionScriptLoader::SetStageAIRaceCatchupParams( int argc, char** argv )
{
    rAssert( spInstance );

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    char cmd[64];
    sprintf( cmd, "SetStageAIRaceCatchupParams" );
    char errMsg[512];
#endif

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "You must use: %s inside the Stage Command!!", cmd );
    rTuneAssertMsg( spInstance->mpStage != NULL, errMsg);
#endif 

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "Bad number of args. Syntax: %s( STAGEVEHICLENAME, "
        "ALONGROADDISTFROMPLAYER_TO_APPLYMAXCATCHUP, "
        "FRACTIONPLAYERSPEED_MINCATCHUP, "
        "FRACTIONPLAYERSPEED_NOCATCHUP, "
        "FRACTIONPLAYERSPEED_MAXCATCHUP )", cmd );
    rTuneAssertMsg( argc == 6, errMsg );
#endif



    char vehiclename[32];
    strcpy( vehiclename, argv[1] );
    Vehicle* vehicle = spInstance->GetVehicleByName( vehiclename );

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "%s: Cannot find vehicle called \"%s\"", cmd, vehiclename );
    rTuneAssertMsg( vehicle, errMsg );
#endif

    VehicleAI::RaceCatchupParams params;
    params.DistMaxCatchup = (float) atof(argv[2]);
    params.FractionPlayerSpeedMinCatchup = (float) atof(argv[3]);
    params.FractionPlayerSpeedMidCatchup = (float) atof(argv[4]);
    params.FractionPlayerSpeedMaxCatchup = (float) atof(argv[5]);

    spInstance->mpStage->SetAIRaceCatchupParams( vehicle, params );

}
void MissionScriptLoader::SetStageAIEvadeCatchupParams( int argc, char** argv )
{
    rAssert( spInstance );

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    char cmd[64];
    sprintf( cmd, "SetStageAIEvadeCatchupParams" );
    char errMsg[512];
#endif

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "You must use: %s inside the Stage Command!!", cmd );
    rTuneAssertMsg( spInstance->mpStage != NULL, errMsg);
#endif 

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "Bad number of args. Syntax: %s( STAGEVEHICLENAME, "
        "CROWFLYDIST_PLAYERTOONEAR, CROWFLYDIST_PLAYERFARENOUGH )", cmd );
    rTuneAssertMsg( argc == 4, errMsg );
#endif


    char vehiclename[32];
    strcpy( vehiclename, argv[1] );
    Vehicle* vehicle = spInstance->GetVehicleByName( vehiclename );

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "%s: Cannot find vehicle called \"%s\"", cmd, vehiclename );
    rTuneAssertMsg( vehicle, errMsg );
#endif

    VehicleAI::EvadeCatchupParams params;
    params.DistPlayerTooNear = (float) atof(argv[2]);
    params.DistPlayerFarEnough = (float) atof(argv[3]);

    spInstance->mpStage->SetAIEvadeCatchupParams( vehicle, params );


}
void MissionScriptLoader::SetStageAITargetCatchupParams( int argc, char** argv )
{
    rAssert( spInstance );

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    char cmd[64];
    sprintf( cmd, "SetStageAITargetCatchupParams" );
    char errMsg[512];
#endif

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "You must use: %s inside the Stage Command!!", cmd );
    rTuneAssertMsg( spInstance->mpStage != NULL, errMsg);
#endif 

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "Bad number of args. Syntax: %s( STAGEVEHICLENAME, "
        "CROWFLYDIST_PLAYERNEARENOUGH, CROWFLYDIST_PLAYERTOOFAR )", cmd );
    rTuneAssertMsg( argc == 4, errMsg );
#endif


    char vehiclename[32];
    strcpy( vehiclename, argv[1] );
    Vehicle* vehicle = spInstance->GetVehicleByName( vehiclename );

#if defined(RAD_TUNE) || defined(RAD_DEBUG)
    sprintf( errMsg, "%s: Cannot find vehicle called \"%s\"", cmd, vehiclename );
    rTuneAssertMsg( vehicle, errMsg );
#endif

    VehicleAI::TargetCatchupParams params;
    params.DistPlayerNearEnough = (float) atof(argv[2]);
    params.DistPlayerTooFar = (float) atof(argv[3]);

    spInstance->mpStage->SetAITargetCatchupParams( vehicle, params );
}



void MissionScriptLoader::SetPauseDuration(int argc,char** argv)
{
    if (spInstance->mpObjective != NULL)
    {
        if (spInstance->mObjType == MissionObjective::OBJ_TIMER)
        {
            TimerObjective* pTimerObjective = dynamic_cast <TimerObjective*> (spInstance->mpObjective);
            rAssert(pTimerObjective);
            unsigned int milliseconds =0;
            milliseconds = atoi(argv[1]);
            //convert to milliseconds
            milliseconds *= 1000;
            pTimerObjective->SetTimer( milliseconds);
        }
        else
        {
            rTuneAssertMsg(0," You must USE SetPauseDuration : inside a Timer Objective \n");
        }
    }
    else
    {
        rTuneAssertMsg(0,"Timer Objective has not been declared, You must use AddObjective(timer); first \n");
    }
}


void MissionScriptLoader::SetCharacterToHide(int argc,char** argv)
{
    rTuneAssert(spInstance->mpStage != NULL);
    spInstance->mpStage->SetCharacterToHide(argv[1]);
}

void MissionScriptLoader::SetLevelOver(int argc,char** argv)
{
    rTuneAssert(spInstance->mpStage != NULL);
    spInstance->mpStage->SetLevelOver();
}

void MissionScriptLoader::SetGameOver(int argc, char** argv)
{
    rTuneAssert(spInstance->mpStage != 0);
    spInstance->mpStage->SetGameOver();
}

void MissionScriptLoader::StayInBlack(int argc,char** argv)
{
    rTuneAssertMsg(spInstance->mpStage != NULL,"You cant use this command to a Stage that doesnt exist, Use AddStage() before this command! \n");
    spInstance->mpStage->mbStayBlackForStage= true;
}

void MissionScriptLoader::AllowMissionAbort(int argc,char** argv)
{
    rTuneAssertMsg(spInstance->mpStage != NULL,"You cant use this command to a Stage that doesnt exist, Use AddStage() before this command! \n");

    bool allowMissionAbort = (strcmp( argv[ 1 ], "false" ) != 0);
    spInstance->mpStage->SetMissionAbortEnabled( allowMissionAbort );
}

