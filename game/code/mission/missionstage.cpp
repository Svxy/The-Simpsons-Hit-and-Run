//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        missionstage.cpp
//
// Description: Implement MissionStage
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================

#include <contexts/context.h>
#include <gameflow/gameflow.h>

#include <ai/vehicle/chaseai.h>
#include <ai/vehicle/vehicleai.h>
#include <ai/vehicle/waypointai.h>

#include <meta/locator.h>
#include <meta/zoneeventlocator.h>
#include <meta/carstartlocator.h>

#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>
#include <mission/missionstage.h>
#include <mission/mission.h>
#include <mission/objectives/missionobjective.h>
#include <mission/conditions/missioncondition.h>
#include <mission/conditions/timeoutcondition.h>
#include <mission/conditions/getoutofcarcondition.h>
#include <mission/safezone/safezone.h>
#include <mission/objectives/raceobjective.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/harass/chasemanager.h>
#include <worldsim/hitnrunmanager.h>

#include <camera/supercamcentral.h>
#include <camera/supercammanager.h>
#include <camera/animatedcam.h>

#include <memory/srrmemory.h>

#include <debug/profiler.h>

#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/redbrick/vehiclecontroller/humanvehiclecontroller.h>
#include <worldsim/redbrick/vehiclecontroller/vehiclemappable.h>

#include <roads/roadmanager.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/intersection.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// MissionStage::MissionStage
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionStage::MissionStage() :
    mState( STAGE_IDLE ),
    mObjective( NULL ),
    mNumConditions( 0 ),
    mStageTimeType( STAGETIME_SET ),
    mStageTime( 0 ),
    mNumVehicles( 0 ),
    mNumWaypoints( 0 ),
    mNumCharacters( 0 ),
    mbFinalStage( false ),
    miNameIndex( -1 ),
    miStartMessageIndex( -1 ),
    mbShowMessage( true ),
    mMusicChange( false ),
    mKeepMusicOn( false ),
    mMusicEventKey( 0 ),
    mMusicStateKey( 0 ),
    mMusicStateEventKey( 0 ),
    mDialogEventKey( 0 ),
    mb_InsideSafeZone (false),
    mb_UseElapsedTime(false),
    mRaceEnteryFee(0),
    mbRacePaidOut( false ),
    mTrafficDensity( -1 ),
    mIsBonusObjectiveStart( false ),
    mStartBonusObjectives( false ),
    mMissionLocked( false ),
    mShowStageComplete( false ),
    mCountdownEnabled( false ),
    mIrisAtEnd( false ),
    mFadeOutAtEnd( false ),
    mIrisSpeed( 1.0f ),
    mCountdownSequenceUnits( NULL ),
    mNumCountdownSequenceUnits( 0 ),
    mCountdownDialogKey( 0 ),
    mSecondSpeakerUID( 0 ),
    mAllowMissionAbort( true )
{
#ifdef RAD_DEBUG
    static int count = 0;
    m_Id = count;
    ++count;

    if( m_Id == 12 )
    {
        int i = 0;
    }
#endif
    strcpy( mHUDIconImage, "" );

    int i;
    
	//initializing the damn chasevehicle struct.
	for (i=0; i<MAX_CHASE_STRUCTS;i++)
	{
		strcpy(m_ChaseData_Array[i].vehiclename,"NULL");
		m_ChaseData_Array[i].mChaseVehicleSpawnRate = 0;
	}
	
    //intialize the safezones.
    for (i=0;i<MAX_SAFEZONES;i++)
    {
        m_SafeZone_Array[i]=NULL;
    }

    for(i=0;i<MAX_CONDITIONS;i++)
    {
        mConditions[i] = NULL;
    }

    //intialize VehicleName Arrary to null
    for(i=0;i<MAX_CHARACTERS_IN_STAGE;i++)
    {
        strcpy(mCharacters[i].VehicleName,"NULL");
    }

    mbClearTrafficForStage = false;
    mbNoTrafficForStage = false;
    mbPutPlayerInCar = false;
    mb_DisableHitAndRun = false;
    mbStayBlackForStage = false;
    mbDisablePlayerControlForCountDown = false;
     
    //
    // By default line 0 of the conversation should be a wide angle PC shot
    //
    SetCameraForDialogLine( 0, "npc_far" );
    //Chuck Setting the Forced Car Locators to "NULL"

    strcpy(mSwapDefaultCarRespawnLocatorName,"NULL");
    strcpy(mSwapForcedCarRespawnLocatorName,"NULL");
    strcpy(mSwapPlayerRespawnLocatorName,"NULL");
    strcpy(mPlayerRespawnLocatorName,"NULL");
    strcpy(mPlayerCarRespawnLocatorName,"NULL");
    strcpy(mCharacterToHide,"NULL");
    mbLevelOver =false;
    mbGameOver = false;
    mResetCounter = 0;

}

//==============================================================================
// MissionStage::~MissionStage
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionStage::~MissionStage()
{
    if( mCountdownSequenceUnits != NULL )
    {
        delete [] mCountdownSequenceUnits;
        mCountdownSequenceUnits = NULL;
    }

    mNumCountdownSequenceUnits = 0;

    delete mObjective;
    mObjective = NULL;

    int i;
    for( i = 0; i < mNumConditions; i++ )
    {
        delete mConditions[ i ];
        mConditions[ i ] = NULL;
    }
    mNumConditions = 0;

    for( i = 0; i < mNumVehicles; i++ )
    {
        //This will be done too many times, but should be okay...
        //GetVehicleCentral()->RemoveVehicleFromActiveList( mVehicles[ i ].vehicle );
        
        // greg
        // jan 7, 2003
        // no longer do this here!
        //
        // GameplayManger owns these cars and is the only one that will add to and remove from ActiveList

        if(mVehicles[ i ].vehicle != NULL)
        {
            mVehicles[ i ].vehicle->Release();
        }
        mVehicles[ i ].vehicle = NULL;
        mVehicles[ i ].spawn = NULL;

    
		if ( mVehicles[ i ].vehicleAINum != -1 )
        {
            GetVehicleCentral()->SetVehicleController( mVehicles[ i ].vehicleAINum, NULL );
        }               
        
		if (mVehicles[ i ].vehicleAI != NULL)
		{
			mVehicles[ i ].vehicleAI->Release();
		}
        mVehicles[ i ].vehicleAI = NULL;
    }
    mNumVehicles = 0;

    for( i = 0; i < mNumCharacters; i++ )
    {
        if ( mCharacters[ i ].character )
        {
            mCharacters[ i ].character->Release();
            mCharacters[ i ].character = NULL;
        }

        if ( mCharacters[ i ].pZoneEventLocator )
        {
            mCharacters[ i ].pZoneEventLocator->Release();
            mCharacters[ i ].pZoneEventLocator = NULL;
        }

        if ( mCharacters[ i ].locator != NULL )
        {
            mCharacters[ i ].locator->Release();
            mCharacters[ i ].locator = NULL;
        }

        if ( mCharacters[ i ].carLocator != NULL )
        {
            mCharacters[ i ].carLocator->Release();
            mCharacters[ i ].carLocator = NULL;
        }

        if( mCharacters[ i ].vehicle != NULL )
        {
            //mCharacters[ i ].vehicle->Release();
            mCharacters[ i ].vehicle = NULL;
        }
    }
    DestroyAllSafeZones();
}

//==============================================================================
// MissionStage::DestroyStageVehicleAI
//==============================================================================
// Description: After  STAGE_COMPLETE is satified prepare to terminate all vehicle AI owned by this stage 
// with EXTREME PREJUDICE, by setting the AI into a inactive state
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
int MissionStage::DestroyStageVehicleAI()
{

    // TODO - greg
    // jan 8, 2003
    
    // stop calling this at the end of a stage when we switch to the new
    // system of not killing and reallocating cars and ai on a mission restart.
    


    int i;
  
    for( i = 0; i < mNumVehicles; i++ )
    {
        
        
		if (mVehicles[ i ].vehicleAI != NULL)
		{
		
            if ( mVehicles[ i ].vehicleAINum != -1 )
            {
                GetVehicleCentral()->SetVehicleController( mVehicles[ i ].vehicleAINum, NULL );
            }
		
            if (mVehicles[ i ].vehicleAI != NULL)
		    {
                mVehicles[ i ].vehicleAI->SetActive(false);
			    mVehicles[ i ].vehicleAI->Release();
		    }
        
            mVehicles[ i ].vehicleAI = NULL;
           
		}
	              
    }
	return 0;
}




//=============================================================================
// MissionStage::AddVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle, CarStartLocator* spawnlocator )
//
// Return:      void 
//
//=============================================================================
void MissionStage::AddVehicle(  Vehicle* vehicle, 
                                int vehicleCentralIndex,   
                                CarStartLocator* spawnlocator,
                                char* ainame)
{
    MEMTRACK_PUSH_GROUP( "MissionStage" );
    //wrap allocates with the heap manager

    // check if we already have this vehicle in our list.
    // then, if we do, update it's ai and spawn

    rAssert( mNumVehicles < MAX_VEHICLES );        
    
    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    
    int i;
    //for(i = 0; i < mNumVehicles; i++)
    for(i = 0; i < MAX_VEHICLES; i++)
    {
        if(mVehicles[i].vehicle == vehicle)
        {
            // got this one.
            
            // why would this ever actually be the case?
            // I guess it would happen if they call
            // AddStageVehicle and then 
            // MoveStageVehicle in the same stage?
                                    
            rAssert(mVehicles[i].vehicleAINum == vehicleCentralIndex);
                                    
            mVehicles[i].spawn = spawnlocator;
            
            if(mVehicles[i].vehicleAI != NULL && ainame[0] != 0)
            {
                // we need to blow away the old ai to make room for the new
                mVehicles[i].vehicleAI->Release();
                mVehicles[i].vehicleAI = 0;                                            
            }
            
            if(ainame[0] != 0)
            {
                // a string was passed in
                    
                MEMTRACK_PUSH_GROUP( "Mission - AI " );

				//wrap allocates with the heap manager

#ifdef RAD_GAMECUBE
                HeapMgr()->PushHeap( GMA_GC_VMM );
#else
                HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
#endif

                VehicleAI* pAI = NULL;
                if( strcmp( ainame, "chase") == 0 )
                {
                    pAI = new ChaseAI( vehicle );
                }
                else if( strcmp( ainame, "race" ) == 0 )
                {
                    pAI = new WaypointAI( vehicle );
                    ((WaypointAI*)pAI)->SetAIType( WaypointAI::RACE );
                }
                else if( strcmp( ainame, "evade" ) == 0 )
                {
                    pAI = new WaypointAI( vehicle );
                    ((WaypointAI*)pAI)->SetAIType( WaypointAI::EVADE );
                }
                else if( strcmp( ainame, "target" ) == 0 )
                {
                    pAI = new WaypointAI( vehicle );
                    ((WaypointAI*)pAI)->SetAIType( WaypointAI::TARGET );
                }
                else if ( strcmp( ainame, "NULL" ) == 0 )
                {
			        pAI = NULL;
		        }
		        else 
		        {	
			        char buffy[64];
                    sprintf( buffy, "Unknown AI vehicle type: %s", ainame);
                    rTuneAssertMsg( false, buffy );
                }
                MEMTRACK_POP_GROUP("Mission - AI ");          
        
#ifdef RAD_GAMECUBE
                HeapMgr()->PopHeap( GMA_GC_VMM );
#else
                HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
#endif
        
                mVehicles[i].vehicleAI = pAI;
         
                if(pAI != 0)
                {
                    mVehicles[i].vehicleAI->AddRef();
                }                    
            }
            
            MEMTRACK_POP_GROUP("MissionStage");
            return;
        }        
    }


    // below is Darryl's old way of not looking through the list
    // makes me queazy
    
    // let's add this test just to be safe
    rAssert(mVehicles[mNumVehicles].vehicle == 0);


    mVehicles[ mNumVehicles ].vehicle = vehicle;
    vehicle->AddRef();
    
    mVehicles[mNumVehicles].vehicleAINum = vehicleCentralIndex;

    mVehicles[ mNumVehicles ].spawn = spawnlocator;

	//wrap allocates with the heap manager
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
#endif

    if(ainame[0] != 0)
    {
        // a string was passed in
            
        MEMTRACK_PUSH_GROUP( "Mission - AI " );
        VehicleAI* pAI = NULL;
        if( strcmp( ainame, "chase") == 0 )
        {
            pAI = new ChaseAI( vehicle );
        }
        else if( strcmp( ainame, "race" ) == 0 )
        {
            pAI = new WaypointAI( vehicle );
            ((WaypointAI*)pAI)->SetAIType( WaypointAI::RACE );
        }
        else if( strcmp( ainame, "evade" ) == 0 )
        {
            pAI = new WaypointAI( vehicle );
            ((WaypointAI*)pAI)->SetAIType( WaypointAI::EVADE );
        }
        else if( strcmp( ainame, "target" ) == 0 )
        {
            pAI = new WaypointAI( vehicle );
            ((WaypointAI*)pAI)->SetAIType( WaypointAI::TARGET );
        }
        else if ( strcmp( ainame, "NULL" ) == 0 )
        {
			pAI = NULL;
		}
		else 
		{	
			char buffy[256];
            sprintf( buffy, "Unknown AI vehicle type: %s. "
                "Need to specify race/evade/target/chase. Defaulting to \"race\".", 
                ainame);
            rTunePrintf( buffy );

            pAI = new WaypointAI( vehicle );
            ((WaypointAI*)pAI)->SetAIType( WaypointAI::RACE );
        }
        MEMTRACK_POP_GROUP("Mission - AI ");          


        mVehicles[mNumVehicles].vehicleAI = pAI;
        
        // recall, this still might be NULL
        if(pAI != 0)
        {
            mVehicles[mNumVehicles].vehicleAI->AddRef();
        }
    }
          
    mNumVehicles++;

#ifdef RAD_GAMECUBE
    HeapMgr()->PopHeap( GMA_GC_VMM );
#else
    HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
#endif

    MEMTRACK_POP_GROUP( "MissionStage" );
}


void MissionStage::SetAIParams( Vehicle* vehicle, const MissionStage::AIParams & params )
{
    rAssert( vehicle );

    for( int i = 0; i < MAX_VEHICLES; i++ )
    {
        if(mVehicles[i].vehicle == vehicle)
        {
            rTuneAssertMsg( mVehicles[i].vehicleAI, 
                "The AI for this vehicle should have been initialized before "
                "trying to set AI parameters." );

            mVehicles[i].vehicleAI->SetMaxShortcutSkill( params.maxShortcutSkill );
            mVehicles[i].vehicleAI->SetMinShortcutSkill( params.minShortcutSkill );
            return;
        }
    }

    rTuneAssertMsg( false, "The vehicle specified for setting AI params does not "
        "exist in the mission stage." );
}

void MissionStage::SetAIRaceCatchupParams( Vehicle* vehicle, const VehicleAI::RaceCatchupParams& params )
{
    rAssert( vehicle );

    for( int i = 0; i < MAX_VEHICLES; i++ )
    {
        if(mVehicles[i].vehicle == vehicle)
        {
            rTuneAssertMsg( mVehicles[i].vehicleAI, 
                "The AI for this vehicle should have been initialized before "
                "trying to set AI parameters." );

            mVehicles[i].vehicleAI->SetRaceCatchupParams( params );
            return;
        }
    }

    rTuneAssertMsg( false, "The vehicle specified for setting AI params does not "
        "exist in the mission stage." );

}

void MissionStage::SetAIEvadeCatchupParams( Vehicle* vehicle, const VehicleAI::EvadeCatchupParams& params )
{
    rAssert( vehicle );

    for( int i = 0; i < MAX_VEHICLES; i++ )
    {
        if(mVehicles[i].vehicle == vehicle)
        {
            rTuneAssertMsg( mVehicles[i].vehicleAI, 
                "The AI for this vehicle should have been initialized before "
                "trying to set AI parameters." );

            mVehicles[i].vehicleAI->SetEvadeCatchupParams( params );
            return;
        }
    }

    rTuneAssertMsg( false, "The vehicle specified for setting AI params does not "
        "exist in the mission stage." );

}

void MissionStage::SetAITargetCatchupParams( Vehicle* vehicle, const VehicleAI::TargetCatchupParams& params )
{
    rAssert( vehicle );

    for( int i = 0; i < MAX_VEHICLES; i++ )
    {
        if(mVehicles[i].vehicle == vehicle)
        {
            rTuneAssertMsg( mVehicles[i].vehicleAI, 
                "The AI for this vehicle should have been initialized before "
                "trying to set AI parameters." );

            mVehicles[i].vehicleAI->SetTargetCatchupParams( params );
            return;
        }
    }

    rTuneAssertMsg( false, "The vehicle specified for setting AI params does not "
        "exist in the mission stage." );

}



//=============================================================================
// MissionStage::AddWaypoint
//=============================================================================
// Description: Comment
//
// Parameters:  ( Locator* locator )
//
// Return:      void 
//
//=============================================================================
void MissionStage::AddWaypoint( Locator* locator )
{
    rAssert( 0 <= mNumWaypoints && mNumWaypoints < WaypointAI::MAX_WAYPOINTS );

    mWaypoints[ mNumWaypoints ] = locator;

    mNumWaypoints++;
}

//=============================================================================
// MissionStage::AddCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* character, CarStartLocator* spawnlocator, CarStartLocator* carlocator, Vehicle* vehicle )
//
// Return:      void 
//
//=============================================================================
void MissionStage::AddCharacter( char* name, 
                                 CarStartLocator* spawnlocator, 
                                 CarStartLocator* carlocator, 
                                 const char* dynaloadString,
                                 Vehicle* vehicle )
{
MEMTRACK_PUSH_GROUP( "Mission - Characters" );
    rAssert( mNumCharacters < MAX_CHARACTERS_IN_STAGE );

    strcpy( mCharacters[ mNumCharacters ].name, name );
    mCharacters[ mNumCharacters ].character = NULL;
    if( strcmp( dynaloadString, "" ) != 0 )
    {
        mCharacters[ mNumCharacters ].pZoneEventLocator = new(GetGameplayManager()->GetCurrentMissionHeap()) ZoneEventLocator;
        mCharacters[ mNumCharacters ].pZoneEventLocator->AddRef();
        mCharacters[ mNumCharacters ].pZoneEventLocator->SetZoneSize( strlen(dynaloadString) + 1 );
        mCharacters[ mNumCharacters ].pZoneEventLocator->SetZone( dynaloadString );
        mCharacters[ mNumCharacters ].pZoneEventLocator->SetPlayerEntered();
    }
    else
    {
        mCharacters[ mNumCharacters ].pZoneEventLocator = NULL;
    }

    rAssert( mCharacters[ mNumCharacters ].locator == NULL );

    if ( spawnlocator )
    {
        mCharacters[ mNumCharacters ].locator = spawnlocator;
        mCharacters[ mNumCharacters ].locator->AddRef();
    }

    rAssert( mCharacters[ mNumCharacters ].carLocator == NULL );
    if ( carlocator )
    {
        mCharacters[ mNumCharacters ].carLocator = carlocator;
        mCharacters[ mNumCharacters ].carLocator->AddRef();
    }

    mCharacters[ mNumCharacters ].vehicle = vehicle;
    if( vehicle != NULL )
    {
        //vehicle->AddRef();
    }

    mNumCharacters++;
MEMTRACK_POP_GROUP( "Mission - Characters" );
}


//=============================================================================
// MissionStage::AddCharacter
//=============================================================================
// Description: Overloaded Function Take in a char point for car name use only if the missionscript calls for the "current" car
//
// Parameters:  ( Character* character, CarStartLocator* spawnlocator, CarStartLocator* carlocator, char* VehicleName )
//
// Return:      void 
//
//=============================================================================
void MissionStage::AddCharacter( char* name, 
                                 CarStartLocator* spawnlocator, 
                                 CarStartLocator* carlocator, 
                                 const char* dynaloadString,
                                 char* VehicleName )
{
MEMTRACK_PUSH_GROUP( "Mission - Characters" );
    rAssert( mNumCharacters < MAX_CHARACTERS_IN_STAGE );

    strcpy( mCharacters[ mNumCharacters ].name, name );
    mCharacters[ mNumCharacters ].character = NULL;
    if( strcmp( dynaloadString, "" ) != 0 )
    {
        mCharacters[ mNumCharacters ].pZoneEventLocator = new(GetGameplayManager()->GetCurrentMissionHeap()) ZoneEventLocator;
        mCharacters[ mNumCharacters ].pZoneEventLocator->AddRef();
        mCharacters[ mNumCharacters ].pZoneEventLocator->SetZoneSize( strlen(dynaloadString) + 1 );
        mCharacters[ mNumCharacters ].pZoneEventLocator->SetZone( dynaloadString );
        mCharacters[ mNumCharacters ].pZoneEventLocator->SetPlayerEntered();
    }
    else
    {
        mCharacters[ mNumCharacters ].pZoneEventLocator = NULL;
    }

    rAssert( mCharacters[ mNumCharacters ].locator == NULL );

    if ( spawnlocator )
    {
        mCharacters[ mNumCharacters ].locator = spawnlocator;
        mCharacters[ mNumCharacters ].locator->AddRef();
    }

    rAssert( mCharacters[ mNumCharacters ].carLocator == NULL );
    if ( carlocator )
    {
        mCharacters[ mNumCharacters ].carLocator = carlocator;
        mCharacters[ mNumCharacters ].carLocator->AddRef();
    }

    mCharacters[ mNumCharacters ].vehicle = NULL;
    strcpy(mCharacters[ mNumCharacters ].VehicleName,VehicleName);

    mNumCharacters++;
MEMTRACK_POP_GROUP( "Mission - Characters" );
}

//=============================================================================
// MissionStage::GetFailureCondition
//=============================================================================
// Description: Returns the first failure condition that is found.
//
// Parameters:  ()
//
// Return:      reference to a mission condition
//
//=============================================================================
MissionCondition*
MissionStage::GetFailureCondition() const
{
    MissionCondition* failureCondition = NULL;

    // search for first failure condition
    //
    for( int i = 0; i < mNumConditions; i++ )
    {
        if( mConditions[ i ]->IsViolated() )
        {
            // ok, found it!
            //
            failureCondition = mConditions[ i ];

            break;
        }
    }

    return failureCondition;
}

//=============================================================================
// MissionStage::Initialize
//=============================================================================
// Description: BEEEFCAAAKE!  BEEEEEFCAAAAKE!
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::Initialize()
{
    Reset();
    GetEventManager()->AddListener(this,EVENT_GUI_MISSION_START);
}

//=============================================================================
// MissionStage::Finalize
//=============================================================================
// Description: 
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::Finalize()
{
    GetEventManager()->RemoveAll(this);
    VehicleFinalize();
 
    mObjective->Finalize();

    if( mCountdownEnabled )
    {
        CGuiScreenHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            HudEventHandler* hudEventHandler = currentHud->GetEventHandler( CGuiScreenHud::HUD_EVENT_HANDLER_COUNTDOWN );
            if( hudEventHandler != NULL )
            {
                hudEventHandler->Stop();
            }
        }
//        GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_COUNT_DOWN );
    }

    for( int j = 0; j < mNumConditions; j++ )
    {
        rAssert( mConditions[ j ] != NULL );

        if( mConditions[ j ] ->GetType()  == MissionCondition::COND_PLAYER_OUT_OF_VEHICLE)
        {
            GetOutOfCarCondition* pOutOfCarCondition = NULL;

            pOutOfCarCondition = dynamic_cast < GetOutOfCarCondition*> (mConditions[ j ]);
                
            if (pOutOfCarCondition->IsConditionActive() == true)
            {
                Mission* currMission = GetGameplayManager()->GetCurrentMission();
                if ( currMission )
                {
                    currMission->SetCarryOverOutOfCarCondition(true);
                }
            }
        }
        mConditions[ j ]->Finalize();

        if( mConditions[ j ]->GetType() == MissionCondition::COND_TIME_OUT )
        {
            GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_TIMER );
        }

        if( mConditions[ j ]->GetType() == MissionCondition::COND_FOLLOW_DISTANCE )
        {
            GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_PROXIMITY_METER );
        }
    }

    //Reset the showmessage to true
    mbShowMessage = true;
    if (mb_DisableHitAndRun == true)
    {
        GetHitnRunManager()->EnableHitnRun();
    }

    //hide character.
     if(strcmp(mCharacterToHide,"NULL") != 0)
    {
        Character* pCharacter =NULL;        
        pCharacter= GetCharacterManager()->GetCharacterByName(mCharacterToHide);
        rTuneAssert(pCharacter !=NULL);
        pCharacter->RemoveFromWorldScene();
    }

    if(mbStayBlackForStage == true)
    {
        GetGameplayManager()->ManualControlFade(false);
        GetGameplayManager()->PauseForFadeFromBlack(1.0);
    }

    if ( mbDisablePlayerControlForCountDown )
    {
        mbDisablePlayerControlForCountDown = false;

        if ( GetGameplayManager()->GetCurrentVehicle() )
        {
            VehicleController* controller = GetVehicleCentral()->GetVehicleController(
                GetVehicleCentral()->GetVehicleId(GetGameplayManager()->GetCurrentVehicle()));
            if( controller )
            {
                rAssert(dynamic_cast<HumanVehicleController*>(controller));
                static_cast<HumanVehicleController*>(controller)->SetDisableReset(false);
            }
        }
    }  
}


//=============================================================================
// MissionStage::VehicleFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::VehicleFinalize()
{
   for( int i = 0; i < mNumVehicles; i++ )
    {
        // GetVehicleCentral()->RemoveVehicleFromActiveList( mVehicles[ i ].vehicle );
        // only GameplayManager will do this now.
        
        // TODOGREG:
        //
        // for now, added code to RemoveVehicleFromActiveList to set the vehiclecontroller to 0 if the slot had something.
        // not sure that's really the palce for it?

        VehicleAI* ai = mVehicles[ i ].vehicleAI;
        if( ai != NULL )
        {
            ai->SetActive( false );           

            //if the car is not being used in the proceeding stages remove if the mission vehicles, and it will get autoplaced in 
            //Gameplayamanger's VDU only if stage is complete and it did not have a AI for this stage.
            Mission* currentMission = GetGameplayManager()->GetCurrentMission();
            if(         currentMission != NULL                 
                &&
                        currentMission->CanMDKCar( mVehicles[i].vehicle, this ) 
                &&
                        mState == STAGE_COMPLETE )
            {
                GetGameplayManager()->RemoveVehicleFromMissionVehicleSlots(mVehicles[i].vehicle);
                mVehicles[i].vehicle->Release();
                mVehicles[i].vehicle = NULL;                     
            }
            else
            {
                //add to to be removed from the world.
                if (mState != STAGE_IDLE)
                {
                    GetGameplayManager()->AddToVDU(mVehicles[i].vehicle);
                }
            }

        } //end if null AI check

            
    }//end for loop
    if( mObjective->IsFinished() )
    {
        DestroyStageVehicleAI();
    }

}


//=============================================================================
// MissionStage::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::Reset()
{
    int i;

    mResetCounter++;

    //
    // call up the parent class
    HasPresentationInfo::Reset();

    //get a traffic ptr
    ITrafficSpawnController* pTraffic = TrafficManager::GetSpawnController();  


    //Chuck:clear traffic only if this stage is a race
    //if ( this->GetObjective()->GetObjectiveType() ==  MissionObjective::OBJ_RACE )
    if (mbClearTrafficForStage == true)
    {
           
        pTraffic->DisableTraffic();
        rmt::Vector vector;
        GetCharacterManager()->GetCharacter(0)->GetPosition(&vector);    
        rmt::Sphere sphere (vector, 200.0f);
        pTraffic->ClearTrafficInSphere( sphere);
    }

    if (mbNoTrafficForStage == true)
    {
        pTraffic->DisableTraffic();
        rmt::Vector vector;
        GetCharacterManager()->GetCharacter(0)->GetPosition(&vector);    
        rmt::Sphere sphere (vector, 200.0f);
        pTraffic->ClearTrafficInSphere( sphere);
        pTraffic->SetMaxTraffic(0);
    }
    else
    {
        pTraffic->EnableTraffic();
    }



    //Chuck we need to  revert any vehicle that are presently husks to normal if we are going to use them
    //for this stage since there will only be one instance of any vehicle in the world and we like to recycle
    //vehicles
    GameplayManager* gameplayManager = GetGameplayManager();
    if ( gameplayManager != NULL )
    {
        for(int j = 0;j<MAX_VEHICLES;j++)
        {
            if(mVehicles[j].vehicle != NULL)
            {
                gameplayManager->MakeSureHusksAreReverted(mVehicles[j].vehicle);  // make sure the same car (eg. CellA) used in multiple stages isn't left as a husk...
            }
        }
    }

    //
    // Place the vehicles at their spawn positions
    //
    

    VehicleInfoInitialize();

    mbRacePaidOut = false;

    if(mbPutPlayerInCar)
    {
#ifndef RAD_RELEASE

        if ( GetGameplayManager()->GetCurrentMission()->IsWagerMission() || GetGameplayManager()->GetCurrentMission()->IsRaceMission())
        {
        }
        else
        {   //if you assert here, tell chuck.
            //rTuneAssertMsg(0,"If you assert here tell Chuck, Tree of Woe \n");
        }

#endif
        Character* pCharacter= GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();
        rTuneAssert(pCharacter != NULL);

        Vehicle* v = GetGameplayManager()->GetCurrentVehicle();

        // 
        // If vehicle is gone and there's husk for it, just put the character down with no car
        // to his name.
        // repair the user's vehicle
        bool destroyed = v->IsVehicleDestroyed();
        if( destroyed )
        {
            GetEventManager()->TriggerEvent( EVENT_REPAIR_CAR );
        }
        GetAvatarManager()->PutCharacterInCar(pCharacter,v);
    }

    if(mb_DisableHitAndRun == true)
    {
        GetHitnRunManager()->DisableHitnRun();
    }
    
	for ( i=0;i<MAX_CHASE_STRUCTS;i++)
	{
		//Reset the Chase Vehicle Rates for this stage
		if (strcmp(m_ChaseData_Array[i].vehiclename,"NULL") !=0)
		{
			ChaseManager* p_chasemanager = NULL;
			p_chasemanager=GetGameplayManager()->GetChaseManager ( m_ChaseData_Array[i].vehiclename);
			rAssert(p_chasemanager);
			p_chasemanager->SetMaxObjects(m_ChaseData_Array[i].mChaseVehicleSpawnRate);
            if (m_ChaseData_Array[i].mChaseVehicleSpawnRate > 0)
            {
                p_chasemanager->SetActive(true);
            }
            else
            {
                p_chasemanager->SetActive(false);
            }


		}
	}

    // Place the characters and vehicles in their spawn positions
    //
    for( i = 0; i < mNumCharacters; i++ )
    {
        if( mCharacters[ i ].character == NULL )
        {
            mCharacters[ i ].character = GetCharacterManager()->
                GetCharacterByName( mCharacters[ i ].name );
            rAssert( mCharacters[ i ].character != NULL );
            mCharacters[ i ].character->AddRef();
        }

        //chuck: adding support for current vehicles should the user switch current cars during a stage.
        if (strcmp(mCharacters[ i ].VehicleName,"current") ==0)
        {
            mCharacters[ i ].vehicle= GetGameplayManager()->GetCurrentVehicle();
        }


        if( mCharacters[ i ].vehicle != NULL )
        {
            ITrafficSpawnController* tsc = TrafficManager::GetSpawnController();
            rmt::Vector spawnLoc;

            if ( mCharacters[ i ].carLocator )
            {
                mCharacters[ i ].carLocator->GetLocation( &spawnLoc );
                GetGameplayManager()->PlaceVehicleAtLocator( 
                    mCharacters[ i ].vehicle, 
                    mCharacters[ i ].carLocator );
            }
            else
            {
                rAssert( mCharacters[ i ].locator != NULL );
                mCharacters[ i ].locator->GetLocation( &spawnLoc );
                GetGameplayManager()->PlaceVehicleAtLocator( 
                    mCharacters[ i ].vehicle, 
                    mCharacters[ i ].locator );
            }

            rmt::Sphere spawnSphere( spawnLoc, 5.0f );
            tsc->ClearTrafficInSphere( spawnSphere );

            if ( mCharacters[ i ].locator == NULL )
            {
                GetAvatarManager()->PutCharacterInCar( 
                    mCharacters[ i ].character, 
                    mCharacters[ i ].vehicle );
            }
            else
            {
                if ( mCharacters[ i ].character->IsInCarOrGettingInOut())
                {
                    GetAvatarManager()->PutCharacterOnGround( mCharacters[ i ].character, GetGameplayManager()->GetCurrentVehicle() );
                }

                GetGameplayManager()->PlaceCharacterAtLocator( 
                    mCharacters[ i ].character, 
                    mCharacters[ i ].locator );
            }
        }
        else
        {
            GetGameplayManager()->PlaceCharacterAtLocator( 
                mCharacters[ i ].character, 
                mCharacters[ i ].locator );

            if ( mCharacters[ i ].character->IsInCarOrGettingInOut() )
            {
                GetAvatarManager()->PutCharacterOnGround( mCharacters[ i ].character, GetGameplayManager()->GetCurrentVehicle() );
            }
        }
    }

    if ( mNumCharacters == 0 && GetGameplayManager()->ShouldPutPlayerInCar() )
    {
        Character* player = GetAvatarManager()->GetAvatarForPlayer( 0 )->GetCharacter();
        float rotation = player->GetFacingDir();
        rmt::Vector pos;
        player->GetPosition( &pos );

        //We need to put the character in the default car anyway.
        Vehicle* veh = GetGameplayManager()->GetVehicleInSlot( GameplayManager::eDefaultCar );
        rAssert( veh );

        GetGameplayManager()->PlaceVehicleAtLocation( veh, pos, -rotation );  //The car and characters are reversed.
        GetAvatarManager()->PutCharacterInCar( player, veh );      
        GetGameplayManager()->PutPlayerInCar( false );

        //chuck: presentation stuff since we faded to black at the end of the forced car mission and set
        //fader to manual 
        GetGameplayManager()->ManualControlFade(false);
        GetGameplayManager()->PauseForFadeFromBlack();
      
        GetGameplayManager()->GetCurrentVehicle()->ResetOnSpot(false);
        
        //clear traffic
         rmt::Vector vector;
        GetCharacterManager()->GetCharacter(0)->GetPosition(&vector);    
        rmt::Sphere sphere (vector, 10.0f);
        pTraffic->ClearTrafficInSphere( sphere);
        
    }

    //The Character is likely moved from into to out of car or vice versa.
    GetEventManager()->TriggerEvent( EVENT_CHARACTER_POS_RESET );

    mObjective->Initialize();   

    int j;
    for( j = 0; j < mNumConditions; j++ )
    {
        rAssert( mConditions[ j ] != NULL );
        mConditions[ j ]->Initialize();

        if ( mConditions[ j ]->GetType() == MissionCondition::COND_PLAYER_OUT_OF_VEHICLE)
        {
            if (GetGameplayManager()->GetCurrentMission()->GetCarryOverOutOfCarCondition() == true)
            {
                GetOutOfCarCondition* pCarCondition = dynamic_cast < GetOutOfCarCondition*> (mConditions[ j ]);
                rAssert(pCarCondition != NULL);
                pCarCondition->SetConditionActive();
                GetGameplayManager()->GetCurrentMission()->SetCarryOverOutOfCarCondition(false);
            }
        }


        if( mConditions[ j ]->GetType() == MissionCondition::COND_TIME_OUT )
        {
            GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_TIMER );
        }

        if( mConditions[ j ]->GetType() == MissionCondition::COND_FOLLOW_DISTANCE )
        {
            GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_PROXIMITY_METER, 1 /* 1 = green bar */ );
        }
    }

    if ( mCamInfo.active )
    {
        //TODO: if there are multiple players, should determine which gets the camera set...
        int camFlags = SuperCamCentral::FORCE;
        camFlags |= mCamInfo.cut ? SuperCamCentral::CUT : 0x00000000;
        camFlags |= mCamInfo.quickTransition ? SuperCamCentral::QUICK : 0x00000000;
        SuperCamManager::GetInstance()->GetSCC( 0 )->SelectSuperCam( mCamInfo.type, camFlags );
    }
    
    mState = STAGE_IDLE;
	
	//Clear all Harass cars if they are present 

    //Chase cars should only be removed on mission success/fail
	//GetGameplayManager()->ClearAllChaseCars();



    if ( mTrafficDensity >= 0 )
    {
        ITrafficSpawnController* tsc = TrafficManager::GetSpawnController();
        tsc->SetMaxTraffic( mTrafficDensity );
    }

    mStartBonusObjectives = false;

    //Enable traffic so long as the notrafficflag is false.
    if (mbNoTrafficForStage == false)
    {
        pTraffic->EnableTraffic();
    }

    //
    // Start the countdown timer if required
    //
    if( mCountdownEnabled )
    {
        //
        // put all the vehicles into "limbo"
        //
        PutAllAisInLimbo( true );

        // supress letter box
        //
        AnimatedCam::SupressNextLetterbox();

       //making sure that we only trigger this event once this once since Evil DLongs code makes the first mission stage reset twice :'(
        if(GetMissionManager()->GetLoadingState() == MissionManager::STATE_INVALID)
        {
            GetEventManager()->TriggerEvent( EVENT_GUI_COUNTDOWN_START );
            
            if(GetGameplayManager()->GetCurrentMission()->IsWagerMission() == true)
            {
                GetGameplayManager()->GetCurrentMission()->SetMissionTime(1);
            }
        }
        mbDisablePlayerControlForCountDown = true;      
       
//        GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_COUNT_DOWN );
    }

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        if( mb_UseElapsedTime && this->GetObjective()->GetObjectiveType() == MissionObjective::OBJ_RACE )
        {
            RaceObjective* raceObjective = static_cast<RaceObjective*>( this->GetObjective() );
            rAssert( raceObjective != NULL );

            // set blinking interval to [ partime - 10 sec, partime ]
            //
            int parTimeInMilliseconds = raceObjective->GetParTime() * 1000;
            currentHud->SetTimerBlinkingInterval( parTimeInMilliseconds - 10000, parTimeInMilliseconds );
        }
        else
        {
            // restore blinking interval to [ 10 sec, 0 sec ]
            //
            currentHud->SetTimerBlinkingInterval( 10000, 0 );
        }
    }

    //check for character respawn on this stage

    if ( strcmp(mPlayerRespawnLocatorName,"NULL") != 0)
    {
        Vehicle* pVehicle= NULL;
        Character* pCharacter = NULL;

        pVehicle = GetGameplayManager()->GetCurrentVehicle();
        pCharacter= GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();
    
   
        //check if the player is in a car
        if (pCharacter->IsInCarOrGettingInOut() == true )
        {
            //they are in a car, put their ass on the street
            GetAvatarManager()->PutCharacterOnGround(pCharacter,pVehicle);
            Locator* playerLoc = p3d::find<Locator>( mPlayerRespawnLocatorName);
            rTuneAssertMsg(playerLoc != NULL,"Cant Find Locator to Player for PlacePlayerAtLocatorName!\n");
            GetGameplayManager()->PlaceCharacterAtLocator( pCharacter, playerLoc );
        }
    }

    //check for player car reset
    if (strcmp(mPlayerCarRespawnLocatorName,"NULL") !=0 )
    {
        Vehicle*  pVehicle = NULL;

        pVehicle = GetGameplayManager()->GetCurrentVehicle();
        rTuneAssertMsg(pVehicle != NULL,"Error: with msPlacePlayerCarAtLocatorName(); Player does NOT currently have a vehicle! \n");

        GetGameplayManager()->PlaceVehicleAtLocatorName(pVehicle,mPlayerCarRespawnLocatorName);
    }


    //if this a dummy stage and we want to pause the screen black

    if(mbStayBlackForStage == true)
    {
        
        GetGameplayManager()->ManualControlFade(true);
        //check if the iris is closed
        if (GetGameplayManager()->IsIrisClosed() == true)
        {
            //do nothing since its black no need to black screen again
        }
        else
        {
            GetGameplayManager()->PauseForFadeToBlack(1.0);
        }
    }

   

    //check for level over script command
    //
    if( mbLevelOver || mbGameOver )
    {
        GetGameplayManager()->SetLevelComplete();
        GetGameFlow()->SetContext( CONTEXT_PAUSE );

        if(mbGameOver)
        {
            GetGameplayManager()->SetGameComplete();
        }
    }
}



//=============================================================================
// MissionStage::VehicleInfoInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::VehicleInfoInitialize()
{
    int i;
    for( i = 0; i < mNumVehicles; i++ )
    {
        // new
        // greg
        // jan 7, 2003

        //Chuck 3:16
        //add car to activelist in case it was used in a earlier stage and got removed from the world.
        if ( mVehicles[ i ].vehicle != NULL)
        {    
            //This is to deal with continuity errors.
            if ( !GetGameplayManager()->TestForContinuityErrorWithCar( mVehicles[ i ].vehicle, false ) )
            {
                mVehicles[ i ].vehicleAINum = GetVehicleCentral()->AddVehicleToActiveList( mVehicles[ i ].vehicle );
                GetGameplayManager()->ReleaseFromVDU(mVehicles[ i ].vehicle->mName, NULL);
            }
            else
            {
                //Remove again, just in case.
                GetVehicleCentral()->RemoveVehicleFromActiveList( mVehicles[ i ].vehicle );
                continue;
            }
        }

        // just in case
        // this could change when husk is swapped in and then out and then vehicle back in
        mVehicles[i].vehicleAINum = GetVehicleCentral()->GetVehicleId(mVehicles[i].vehicle, true);

        if( mVehicles[ i ].spawn != NULL )
        {
            ITrafficSpawnController* tsc = TrafficManager::GetSpawnController();
            rmt::Vector spawnLoc;
            mVehicles[ i ].spawn->GetLocation( &spawnLoc );
            rmt::Sphere spawnSphere( spawnLoc, 5.0f );
            tsc->ClearTrafficInSphere( spawnSphere );
            GetGameplayManager()->PlaceVehicleAtLocator( 
                mVehicles[ i ].vehicle, 
                mVehicles[ i ].spawn );
            
            //chuck: repair the car in the case it was used in a previous stage like the cell phone cars or cola truck.           
            mVehicles[i].vehicle->ResetFlagsOnly(true); // greg - that call didn't quite do enough.
        }

        VehicleAI* ai = mVehicles[ i ].vehicleAI;
        if( ai != NULL )
        {
            GetVehicleCentral()->SetVehicleController( mVehicles[ i ].vehicleAINum, ai );

            // try this
            // set all cars to use out of car controller, unless avatar gets in them or they have some ai
            // 
            if(mVehicles[i].vehicle)
            {
                 //chuck: regardless of ai type we will now allow the car to be damaged
                //unless its a dump and collect mission
                if (GetObjective()->GetObjectiveType() != MissionObjective::OBJ_DUMP)
                {
                    mVehicles[i].vehicle->SetVehicleCanSustainDamage(true);
                }
                else
                {
                    mVehicles[i].vehicle->SetVehicleCanSustainDamage(false);
                }

                mVehicles[i].vehicle->SetInCarSimState();
            }


            // enable driving on both sides of the road when there's no traffic
            if( mbNoTrafficForStage || TrafficManager::GetInstance()->GetMaxTraffic() == 0 )
            {
                ai->SetUseMultiplier( false );
            }
            ai->Initialize();
            ai->SetActive( true );

            //chuck: regardless of ai type we will now allow the car to be damaged


            
            switch( ai->GetType() )
            {
            case VehicleAI::AI_WAYPOINT:
                {
                    WaypointAI* wai;
                    wai = dynamic_cast<WaypointAI*>( ai );
                    rAssert( wai != NULL );
        
                    wai->ClearWaypoints();
                    for( int j = 0; j < mNumWaypoints; j++ )
                    {
                        wai->AddWaypoint( mWaypoints[ j ] );
                    }

                    break;
                }
            case VehicleAI::AI_CHASE:
                {
                    ChaseAI* cai;
                    cai = dynamic_cast<ChaseAI*>( ai );
                    rAssert( cai != NULL );

                    //Vehicle* playervehicle;
                    //cai->SetTarget( playervehicle );
                    
                    break;
                }
            default:
                {
                    rAssert( false );
                    break;
                }
            }//end  switch
        }//end if ai null check	
        else
        {
            if(mVehicles[i].vehicle)    // do this?
            {
                mVehicles[i].vehicle->SetOutOfCarSimState();

                //chuck: any vehicles created with an AI with now be invulnerable to damage
                //this will get set to sustain damage with the AI is intialized or added.
                mVehicles[i].vehicle->SetVehicleCanSustainDamage(false);


            }
        }
	}//end of mVehicles loop

    if ( GetGameplayManager()->mIsDemo )
    {
        //Setup the player car to follow waypoints too..
        Vehicle* playerCar = GetGameplayManager()->GetCurrentVehicle();
        rAssert( playerCar );
        VehicleAI* ai = GetVehicleCentral()->GetVehicleAI( playerCar );
        rAssert( ai );

        WaypointAI* wai = dynamic_cast<WaypointAI*>(ai);
        rAssert( wai );

        wai->Initialize();
        wai->SetActive( true );

        wai->ClearWaypoints();
        for( int j = 0; j < mNumWaypoints; j++ )
        {
            wai->AddWaypoint( mWaypoints[ j ] );
        }

        GetVehicleCentral()->SetVehicleController( playerCar->mVehicleCentralIndex, ai );
        playerCar->SetVehicleCanSustainDamage( false );
    }
}


//=============================================================================
// MissionStage::Start
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::Start()
{
    radKey32 musicKey;
    radKey32 musicStateKey;
    radKey32 musicStateEventKey;

    DisplayMissionStageIndexMessage();

    mState = STAGE_INPROGRESS;

    if ( mIsBonusObjectiveStart )
    {
        mStartBonusObjectives = true;
    }

    //
    // This appears to be the best point for analyzing the stage
    // to see if we should inform the music player of something
    // dramatic in the mission play -- Esan
    //
    if( GetMusicChangeFlag() )
    {
        unsigned int numConditions;
        unsigned int i;
        bool isClose = false;

        //
        // Last stage, cue the drama
        //
        numConditions = GetNumConditions();
        for( i = 0; i < numConditions; i++ )
        {
            if( GetCondition( i )->IsClose() )
            {
                isClose = true;
                break;
            }
        }

        if( isClose )
        {
            GetEventManager()->TriggerEvent( EVENT_MISSION_DRAMA );
        }
    }

    musicKey = GetStageStartMusicEvent();
    if( musicKey != 0 )
    {
        GetEventManager()->TriggerEvent( EVENT_CHANGE_MUSIC, &musicKey );
    }

    GetStageMusicState( musicStateKey, musicStateEventKey );
    if( musicStateKey != 0 )
    {
        MusicStateData data( musicStateKey, musicStateEventKey );
        GetEventManager()->TriggerEvent( EVENT_CHANGE_MUSIC_STATE, &data );
    }
}

//=============================================================================
// MissionStage::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void MissionStage::Update( unsigned int elapsedTime )
{
BEGIN_PROFILE("MissionStage Update");
    mObjective->Update( elapsedTime );

        //Chuck: Reward player if this was a gamble race
        //I hate putting in code there, seem ill planned imho too much info hiding       

        //Check if objective was a race 
        if  ( mObjective->GetObjectiveType() == MissionObjective::OBJ_RACE) 
        { 
            //it was a race dynamic cast to a race objective ptr should be ok we dont do this alot
            RaceObjective* pRaceObjective = dynamic_cast <RaceObjective *> (mObjective);
            rAssert(pRaceObjective);

            if (pRaceObjective->QueryIsGambleRace() == true && !mbRacePaidOut )
            {
                //check the player time vs the par time Reward the player with coins and reward screen
                int elapsedtime = GetGameplayManager()->GetCurrentMission()->GetMissionTimeLeftInSeconds();
                int partime =pRaceObjective->GetParTime();

                if( ( elapsedtime <= partime ) && mObjective->IsFinished() )
                {
                    mbRacePaidOut = true;
                    //do rewards stuff here                   
                    GetCoinManager()->AdjustBankValue( (int)( rmt::LtoF(mRaceEnteryFee) + (rmt::FtoL(GetGameplayManager()->GetCurrentVehicle()->GetGamblingOdds() * rmt::LtoF(mRaceEnteryFee))) ) );
                    GetEventManager()->TriggerEvent( EVENT_COLLECTED_COINS);

                    RenderEnums::LevelEnum currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
                    int currentBestTime = GetCharacterSheetManager()->GetGambleRaceBestTime( currentLevel );
                    if( currentBestTime < 0 || elapsedtime < currentBestTime )
                    {
                        GetCharacterSheetManager()->SetGambleRaceBestTime( currentLevel, elapsedtime );

                        const int NEW_BEST_TIME = 1;
                        GetEventManager()->TriggerEvent( EVENT_GAMBLERACE_SUCCESS, (void*)NEW_BEST_TIME );
                    }
                    else
                    {
                        GetEventManager()->TriggerEvent( EVENT_GAMBLERACE_SUCCESS );
                    }
                    //Fire off some thing saying user Beat Par Time you get coins overlay thingy
                }
                else
                {
                    //GetCurrentHud()->HandleMessage(GUI_MSG_HIDE_HUD_OVERLAY,HUD_MISSION_COMPLETE);
                    if ( elapsedtime > partime)
                    {
                        GetEventManager()->TriggerEvent( EVENT_GAMBLERACE_FAILURE);                                       
                        
                        if (elapsedtime > (partime+15))
                        {
                            TimeOutCondition* pTimeOutCondition = new TimeOutCondition();                        
                            pTimeOutCondition->SetViolated(true);
                            unsigned int counter = 0;
                            counter += this->GetNumConditions() +1;
                            this->SetCondition(this->GetNumConditions(),pTimeOutCondition);
                            this->SetNumConditions( counter);

                            mState= STAGE_FAILED;
                            GetEventManager()->TriggerEvent( EVENT_GAMBLERACE_FAILURE);

                            mbRacePaidOut = true;
                        }
                    }
                }
            }
        }    

    if( mObjective->IsFinished() )
    {
        mState = STAGE_COMPLETE;
        OnStageCompleteSuccessful();		
        DestroyAllSafeZones ();
        triggerStageDialog();            

    }
    else
    {
        for( int i = 0; i < mNumConditions; i++ )
        {
            mConditions[ i ]->Update( elapsedTime );
            
            if( mConditions[ i ]->IsViolated() )
            {
                mState = STAGE_FAILED;
            }
            else if ( mConditions[ i ]->LeaveInterior() )
            {
                mState = STAGE_BACKUP;
            }
        }
    }

    Vehicle* p_vehicle =NULL;
    //get players p1 position
    p_vehicle=GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle();

    //player must be the car for safezone check
    if (p_vehicle != NULL)
    {

        //check if the player car is inside the safezones for this stage.
        for( int i=0; i<MAX_SAFEZONES;i++)
        {
            Vehicle* p_vehicle =NULL;
            //get players p1 position
            p_vehicle=GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle();
            rmt::Vector player_position;
            p_vehicle->GetPosition(&player_position);
            
            if (m_SafeZone_Array[i] != NULL)
            {
                //if true and false
                if ( m_SafeZone_Array[i]->InsideZone(player_position) && (mb_InsideSafeZone == false))
                {
                    //player has entered safezone
                    GetGameplayManager()->DisableAllChaseAI();
                    mb_InsideSafeZone = true;
                }
                

                //if false and true
                if ( (m_SafeZone_Array[i]->InsideZone(player_position) == false )  && (mb_InsideSafeZone) )
                {
                    //just left the safezone
                    GetGameplayManager()->EnableAllChaseAI();
                    mb_InsideSafeZone = false;
                }
               
            } //end if check if safezone isnt null
        }//end for loop to check safezones
    }//end if player is inside their car

    //Chuck: Disable player control of car while count down is in progress.
    if (mbDisablePlayerControlForCountDown == true)
    {
        Vehicle* v = GetGameplayManager()->GetCurrentVehicle();
        if (v != NULL)
        {
            v->SetDisableGasAndBrake(true);
            v->SetEBrake( 1.0f, 0.0f );
            VehicleController* controller = GetVehicleCentral()->GetVehicleController(
                GetVehicleCentral()->GetVehicleId(GetGameplayManager()->GetCurrentVehicle()));
            if( controller )
            {
                rAssert(dynamic_cast<HumanVehicleController*>(controller));
                static_cast<HumanVehicleController*>(controller)->SetDisableReset(true);
            }
        }

    }
  
END_PROFILE("MissionStage Update");
}

//=============================================================================
// MissionStage::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void MissionStage::HandleEvent( EventEnum id, void* pEventData )
{
    switch (id)
    {
    case EVENT_GUI_MISSION_START:
        {
            mbDisablePlayerControlForCountDown = false;
            if ( GetGameplayManager()->GetCurrentVehicle() != NULL)
            {
                GetGameplayManager()->GetCurrentVehicle()->SetDisableGasAndBrake(false);
                VehicleController* controller = GetVehicleCentral()->GetVehicleController(
                    GetVehicleCentral()->GetVehicleId(GetGameplayManager()->GetCurrentVehicle()));
                if( controller )
                {
                    rAssert(dynamic_cast<HumanVehicleController*>(controller));
                    static_cast<HumanVehicleController*>(controller)->SetDisableReset(false);
                }
            }

            if (GetGameplayManager()->GetCurrentMission()->IsWagerMission() == true)
            {
                //get a traffic ptr
                ITrafficSpawnController* pTraffic = TrafficManager::GetSpawnController(); 
                pTraffic->EnableTraffic();
            }
            
            break;
        }
    default:
        {
            break;
        }
    }
}

//=============================================================================
// MissionStage::SetStageTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( StageTimeType type, unsigned int seconds )
//
// Return:      void 
//
//=============================================================================
void MissionStage::SetStageTime( StageTimeType type, unsigned int seconds )
{
    mStageTimeType = type;
    mStageTime = seconds + 1;  //This is to make the time down look better.
}

//=============================================================================
// MissionStage::GetStageTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( StageTimeType &type, unsigned int &seconds )
//
// Return:      void 
//
//=============================================================================
void MissionStage::GetStageTime( StageTimeType &type, unsigned int &seconds )
{
    type = mStageTimeType;
    seconds = mStageTime;
}

//=============================================================================
// MissionStage::LoadMissionStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool MissionStage::LoadMissionStart()
{
    if( mNumCharacters > 0 && mCharacters[ 0 ].pZoneEventLocator != NULL )
    {
        GetEventManager()->TriggerEvent( EVENT_FIRST_DYNAMIC_ZONE_START, 
                                         static_cast<void*>( mCharacters[ 0 ].pZoneEventLocator ) );

        return true;
    }

    return false;
}

//=============================================================================
// MissionStage::SetCameraInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ( SuperCam::Type type, bool cut, bool quickTransition )
//
// Return:      void 
//
//=============================================================================
void MissionStage::SetCameraInfo( SuperCam::Type type, bool cut, bool quickTransition )
{
    mCamInfo.active = true;
    mCamInfo.cut = cut;
    mCamInfo.quickTransition = quickTransition;
    mCamInfo.type = type;
}

//=============================================================================
// MissionStage::GetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int which )
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* MissionStage::GetVehicle(  int which )
{
   
    return mVehicles[ which ].vehicle;
}


//=============================================================================
// MissionStage::SetChaseSpawnRate
//=============================================================================
// Description: Sets the Max number of harras AI cars that spawn for this stage
//
// Parameters:  ( unsigned int which )
//
// Return:      Vehicle
//
//=============================================================================
void MissionStage::SetChaseSpawnRate( char* vehicle_name, unsigned int max_chase_cars )
{
	for (int i =0; i<MAX_CHASE_STRUCTS;i++)
	{
		if ( strcmp (m_ChaseData_Array[i].vehiclename,"NULL") ==0)
		{
			strcpy(m_ChaseData_Array[i].vehiclename,vehicle_name);
			m_ChaseData_Array[i].mChaseVehicleSpawnRate = max_chase_cars;
		}
	}
}

  
//=============================================================================
// MissionStage::AddSafeZone
//=============================================================================
// Description: Adds a safezone instance to the mission stages m_SafeZone_Array
//
// Parameters:  ( CarStartLocator* locator, unsigned int radius)
//
// Return:      int , 0 if ok -1 if out of space.
//
//=============================================================================
int MissionStage::AddSafeZone ( CarStartLocator* locator, unsigned int radius)
{
    for (int i =0;i<MAX_SAFEZONES;i++)
    {
        if (m_SafeZone_Array[i] == NULL)
        {
            #ifdef RAD_GAMECUBE
                HeapMgr()->PushHeap( GMA_GC_VMM );
            #else
                HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
            #endif
            m_SafeZone_Array[i] = new SafeZone (locator,radius);
            #ifdef RAD_GAMECUBE
                HeapMgr()->PopHeap( GMA_GC_VMM );
            #else
                HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
            #endif
            return 0; 
        }
    }

    return -1;

} 




//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// MissionStage::ActivateVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle, bool bIsActive )
//
// Return:      void 
//
//=============================================================================
void MissionStage::ActivateVehicle( Vehicle* vehicle, bool bIsActive )
{
    rAssert( vehicle );
    VehicleAI* ai = GetVehicleCentral()->GetVehicleAI( vehicle );
    rAssert( ai );

    ai->SetActive( bIsActive );
}


//=============================================================================
// MissionStage::DestroyALLSafeZones
//=============================================================================
// Description: deletes all safezones in the  mission stages 
// Parameters:  none
//
// Return:      void
//
//=============================================================================
void MissionStage::DestroyAllSafeZones ( )
{
    for (int i =0;i<MAX_SAFEZONES;i++)
    {
        if (m_SafeZone_Array[i] != NULL)
        {
            delete m_SafeZone_Array[i];
            m_SafeZone_Array[i]=NULL;
             
        }
    }  

} 

//=============================================================================
// MissionStage::triggerStageDialog
//=============================================================================
// Description: Play dialog associated with stage completion if it exists
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MissionStage::triggerStageDialog()
{
    DialogEventData data;
    Character* avatarCharacter;
    const char* modelName;

    if( mDialogEventKey != 0 )
    {
        avatarCharacter = GetAvatarManager()->GetAvatarForPlayer( 0 )->GetCharacter();
        modelName = GetCharacterManager()->GetModelName( avatarCharacter );
        data.charUID1 = tEntity::MakeUID( modelName );
        data.charUID2 = mConversationCharacterKey;
        data.dialogName = mDialogEventKey;

        GetEventManager()->TriggerEvent( EVENT_IN_GAMEPLAY_CONVERSATION, static_cast<void*>(&data) );
    }
}

//=============================================================================
// MissionStage::SetLockRequirement
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int which, LockRequirement::Type type, const char* name )
//
// Return:      void 
//
//=============================================================================
void MissionStage::SetLockRequirement( unsigned int which, LockRequirement::Type type, const char* name )
{
    rAssert( which < MAX_LOCK_REQUIREMENTS );

    unsigned int nameLen = strlen(name);

    rAssert( nameLen < LockRequirement::MAX_NAME_LEN );

    mRequirement[ which ].mType = type;
    strncpy( mRequirement[ which ].mName, name, (nameLen < LockRequirement::MAX_NAME_LEN) ? nameLen : LockRequirement::MAX_NAME_LEN );
    mRequirement[ which ].mName[nameLen] = '\0';  //+1 is added on creation of this mName

    mMissionLocked = true;
}

//=============================================================================
// MissionStage::GetLockRequirement
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int which )
//
// Return:      const 
//
//=============================================================================
const MissionStage::LockRequirement& MissionStage::GetLockRequirement( unsigned int which )
{
    rAssert( which < MAX_LOCK_REQUIREMENTS );

    return mRequirement[ which ];
}

//=============================================================================
// MissionStage::DisplayMissionStageIndexMessage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::DisplayMissionStageIndexMessage()
{
    if( mbShowMessage && !GetMissionLocked() && miStartMessageIndex >= 0 )
    {
        GetEventManager()->TriggerEvent( EVENT_SHOW_MISSION_OBJECTIVE,
                                         reinterpret_cast<void*>( miStartMessageIndex ) );
    }
    else
    {
        // this just updates the mission objective icon on the HUD, if changed
        //
        GetEventManager()->TriggerEvent( EVENT_SHOW_MISSION_OBJECTIVE,
                                         reinterpret_cast<void*>( -1 ) );
    }
}

//=============================================================================
// MissionStage::SetCountdownEnabled
//=============================================================================
// Description: Sets the flag that will later enable the countdown timer
//              when this stage starts
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
void MissionStage::SetCountdownEnabled( radKey32 dialogID, tUID secondSpeakerUID )
{
    mCountdownEnabled = true;

    rAssert( mCountdownSequenceUnits == NULL );
    mCountdownSequenceUnits = new( GMA_LEVEL_MISSION ) CountdownSequenceUnit[ MAX_NUM_COUNTDOWN_SEQUENCE_UNITS ];
    rAssert( mCountdownSequenceUnits != NULL );

    for( int i = 0; i < MAX_NUM_COUNTDOWN_SEQUENCE_UNITS; i++ )
    {
        mCountdownSequenceUnits[ i ].durationTime = 0;
    }

    mNumCountdownSequenceUnits = 0;

    mCountdownDialogKey = dialogID;
    mSecondSpeakerUID = secondSpeakerUID;
}

void MissionStage::AddCountdownSequenceUnit( const char* textID, int durationTime )
{
    rTuneAssertMsg( mNumCountdownSequenceUnits < MAX_NUM_COUNTDOWN_SEQUENCE_UNITS,
                    "Number of countdown sequence units exceeds maximum!" );

    strcpy( mCountdownSequenceUnits[ mNumCountdownSequenceUnits ].textID, textID );
    mCountdownSequenceUnits[ mNumCountdownSequenceUnits ].durationTime = durationTime;

    mNumCountdownSequenceUnits++;
}

MissionStage::CountdownSequenceUnit* MissionStage::GetCountdownSequenceUnit( int index ) const
{
    rAssert( index >= 0 );

    if( index < mNumCountdownSequenceUnits )
    {
        rAssert( mCountdownSequenceUnits != NULL );

        return &(mCountdownSequenceUnits[ index ]);
    }
    else
    {
        return NULL;
    }
}

void MissionStage::PutAllAisInLimbo( bool inLimbo )
{
    if( inLimbo )
    {
        int i;
        for( i = 0; i < MAX_VEHICLES; ++i )
        {
            VehicleAI* ai = mVehicles[ i ].vehicleAI;
            if( ai != NULL )
            {
                ai->EnterLimbo();
            }
        }
    }
    else
    {
        int i;
        for( i = 0; i < MAX_VEHICLES; ++i )
        {
            VehicleAI* ai = mVehicles[ i ].vehicleAI;
            if( ai != NULL )
            {
                ai->ExitLimbo();
            }
        }
    }
}

//=============================================================================
// MissionStage::DoTransition
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionStage::DoTransition()
{
    if ( mIrisAtEnd )
    {
        //Do Forced car Swapping while we are irising
        if(mbSwapInDefaultCar == true)
        {
            //do the swapping
        }
        GetGameplayManager()->PauseForIrisClose( mIrisSpeed );
    } else if ( mFadeOutAtEnd )
    {
        //Do Forced car Swapping while we are irising
        if(mbSwapInDefaultCar == true)
        {
            //do the swapping
             SwapInDefaultCarStart();
        }
        GetGameplayManager()->PauseForFadeToBlack( mIrisSpeed );
       
    }
}


//sets mb_UseElapsed time to true which make hud counter count up.
void MissionStage::UseElapsedTime()
{
    mb_UseElapsedTime = true;
}

bool MissionStage::QueryUseElapsedTime()
{
    return mb_UseElapsedTime;
}

//used for Gamble Races
void MissionStage::SetRaceEnteryFee(int coins)
{
    mRaceEnteryFee = coins;
}

void MissionStage::PutMFPlayerInCar( )
{
    mbPutPlayerInCar = true;
}

void MissionStage::DisableHitAndRun()
{
    mb_DisableHitAndRun = true;
}


void MissionStage::SwapInDefaultCar()
{
    mbSwapInDefaultCar = true;
}

void MissionStage::SwapInDefaultCarStart()
{
    Vehicle* pVehicle= NULL;
    Character* pCharacter = NULL;

    //get Gameplaymanager and take manual control of fadein
    GetGameplayManager()->ManualControlFade(true);

    pVehicle = GetGameplayManager()->GetCurrentVehicle();
    pCharacter= GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();
    
    //check if car is a forced car

    if (pVehicle == GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle )
    {        
        //check if the player is in a car
        if (pCharacter->IsInCarOrGettingInOut() == true )
        {
            //they are in a car, put their ass on the street
            GetAvatarManager()->PutCharacterOnGround(pCharacter,pVehicle);
            Locator* playerLoc = p3d::find<Locator>( mSwapPlayerRespawnLocatorName);
            rTuneAssertMsg(playerLoc != NULL,"Cant Find Locator to Spawn Player after Car SWAP !\n");
            GetGameplayManager()->PlaceCharacterAtLocator( pCharacter, playerLoc );
        }

        //Removed  the forced car from the world
        //GetGameplayManager()->DumpCurrentCar();
        GetGameplayManager()->MDKVDU();

        //load the players default car
        GetLoadingManager()->AddRequest(FILEHANDLER_PURE3D,
            GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].filename,
            GMA_LEVEL_MISSION,
            GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].filename,
            "Default",
            this
            );
    }

}


void MissionStage::OnProcessRequestsComplete( void* pUserData)
{

    //changed the forced car to an AI type car and so its locked from player

    Character* pCharacter = NULL;

    GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle->SetUserDrivingCar(false);
    GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle->TransitToAI();
    
    //removed the NPC character driver.
    pCharacter = GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle->GetDriver();
    if (pCharacter != NULL)
    {
        GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle->SetDriver(NULL);
        GetCharacterManager()->RemoveCharacter(pCharacter);
    }    
    GetGameplayManager()->PlaceVehicleAtLocatorName(
    GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle,
        mSwapForcedCarRespawnLocatorName);

    //init the vehicle
    Vehicle* vehicle;
    char conName[64];
    sprintf(conName,"%s.con", GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].name);
    vehicle = GetGameplayManager()->AddLevelVehicle( GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].name,GameplayManager::eDefaultCar, conName);
    GetGameplayManager()->SetCurrentVehicle(vehicle);

    //place at the default car right locator.

   GetGameplayManager()->PlaceVehicleAtLocatorName( vehicle, mSwapDefaultCarRespawnLocatorName );
   //GetGameplayManager()->PlaceCharacterAtLocator( player, mPlayerRestart );
   
   //update the mission so we dont reload default car since it being done in this method
   GetGameplayManager()->GetCurrentMission()->SetSwappedCarsFlag(true);

  

   //cleanup any cars lying around while we are in an iris state.
   GetGameplayManager()->MDKVDU();
   
   //open fade
   GetGameplayManager()->ManualControlFade(false);

};

void MissionStage::SetSwapDefaultCarRespawnLocatorName(char* locatorName)
{
    strncpy(mSwapDefaultCarRespawnLocatorName,locatorName,32);
    mSwapDefaultCarRespawnLocatorName[31]='\0';
}

void MissionStage::SetSwapPlayerRespawnLocatorName(char* locatorName)
{
    strncpy(mSwapPlayerRespawnLocatorName,locatorName,32);
    mSwapPlayerRespawnLocatorName[31]='\0';
}

void MissionStage::SetSwapForcedCarRespawnLocatorName(char* locatorName)
{
    strncpy(mSwapForcedCarRespawnLocatorName,locatorName,32);
    mSwapForcedCarRespawnLocatorName[31]='\0';
}


void MissionStage::EnableTraffic()
{

}

void MissionStage::DisableTraffic()
{
    mbNoTrafficForStage = true;
}

void MissionStage::ClearTrafficForStage()
{
    mbClearTrafficForStage = true;
}

void MissionStage::SetPlayerRespawnLocatorName(char* locatorName)
{
    strncpy(mPlayerRespawnLocatorName,locatorName,32);
    mPlayerRespawnLocatorName[31] = '\0';
}

void MissionStage::SetmsPlayerCarRespawnLocatorName(char* locatorName)
{
    strncpy(mPlayerCarRespawnLocatorName,locatorName,32);
    mPlayerCarRespawnLocatorName[31]='\0';
}

void MissionStage::OnStageCompleteSuccessful()
{
    HasPresentationInfo::OnStageCompleteSuccessful();
}

void MissionStage::SetCharacterToHide(char* charactername)
{
    strncpy(mCharacterToHide,charactername,16);
    mCharacterToHide[15]='\0';
}

void MissionStage::SetLevelOver()
{
    mbLevelOver = true;
}

//=============================================================================
// MissionStage::SetStageMusicState
//=============================================================================
// Description: Set the radMusic state change that should be triggered when
//              this stage starts
//
// Parameters:  stateKey - name of state
//              stateEventKey - name of event
//
// Return:      void 
//
//=============================================================================
void MissionStage::SetStageMusicState( radKey32 stateKey, radKey32 stateEventKey )
{
    mMusicStateKey = stateKey;
    mMusicStateEventKey = stateEventKey;
}

//=============================================================================
// MissionStage::GetStageMusicState
//=============================================================================
// Description: Get the info for the radMusic state change that should be 
//              triggered when this stage starts
//
// Parameters:  stateKey - name of state
//              stateEventKey - name of event
//
// Return:      void 
//
//=============================================================================
void MissionStage::GetStageMusicState( radKey32& stateKey, radKey32& stateEventKey )
{
    stateKey = mMusicStateKey;
    stateEventKey = mMusicStateEventKey;
}


//returns the vehicle that hopefully is the main AI vehicle
Vehicle* MissionStage::GetMainAIVehicleForThisStage()
{
    int indexArray[MAX_VEHICLES] = {-1,-1,-1,-1};
    bool isAISlotInThisList = false;
    Vehicle* pAISlotVehicle = NULL;

    pAISlotVehicle = GetGameplayManager()->GetVehicleInSlot(GameplayManager::eAICar);

    //find out which vehicles in this stage have drivers and Are Active with AI.
    //we fill in array for the second pass
    for (int i=0; i <mNumVehicles;i++)
    {
        if (mVehicles[i].vehicle != NULL && mVehicles[i].vehicleAI != NULL)
        {
            //check if it has a driver.
            if (mVehicles[i].vehicle->GetDriver()  != NULL)
            {
                indexArray[i]=i;
            }
        }
    }

    //check all Active AI against the Main AI slot car 
    for (int j=0;j<MAX_VEHICLES;j++)
    {
        if (indexArray[j] > -1)
        {
            if (pAISlotVehicle != NULL)
            {
                if ( mVehicles[indexArray[j]].vehicle == pAISlotVehicle)
                {
                    isAISlotInThisList = true;
                    return mVehicles[indexArray[j]].vehicle;
                }
            }
        }
        isAISlotInThisList = false;
    }

    // return the first car since we cant determine which car is main ai for this stage 
    for (int j=0;j<MAX_VEHICLES;j++)
    {
        if (indexArray[j] > -1)
        {       
            return mVehicles[indexArray[j]].vehicle;           
        }       
    }
    //return NULL as a fall back.
    return NULL;
}









        
    