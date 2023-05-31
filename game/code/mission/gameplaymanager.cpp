//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gameplaymgr.cpp
//
// Description: Implement GameplayManager
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

#include <string.h>

//========================================
// Project Includes
//========================================

#include <contexts/context.h>
#include <gameflow/gameflow.h>

#include <loading/filehandlerenum.h>

#include <memory/srrmemory.h>

#include <meta/carstartlocator.h>
#include <meta/locator.h>
#include <meta/triggerlocator.h>
#include <meta/triggervolume.h>
#include <meta/triggervolumetracker.h>
#include <meta/eventlocator.h>
#include <meta/locatorevents.h>
#include <meta/spheretriggervolume.h>

#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>
#include <mission/mission.h>
#include <mission/missionscriptloader.h>
#include <mission/animatedicon.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <presentation/presentation.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiwindow.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreeniriswipe.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>

#include <render/rendermanager/rendermanager.h>

#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/parkedcars/parkedcarmanager.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>

#include <events/eventmanager.h>
#include <events/eventlistener.h>
#include <events/eventenum.h>

#include <sound/soundmanager.h>

#include <render/culling/worldscene.h>

#include <main/commandlineoptions.h>
#include <worldsim/harass/chasemanager.h>

#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>

#include <camera/supercammanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
GameplayManager* GameplayManager::spInstance = NULL;

//THESE ARE THE CONTINUITY ISSUES RELATING TO CARS
struct ContinuityError
{
    RenderEnums::LevelEnum mLevel;
    RenderEnums::MissionEnum mMission;
    const char* mPlayerCarName;
    const char* mOtherCarName;
};

const ContinuityError CONTINUITY_ERRORS[] =
{
    { RenderEnums::L2, RenderEnums::M5, "cletu_v", "cletu_v" },
    { RenderEnums::L4, RenderEnums::M3, "cletu_v", "cletu_v" },
    { RenderEnums::L6, RenderEnums::M1, "otto_v", "otto_v" },
    { RenderEnums::L7, RenderEnums::M3, "frink_v", "frink_v" },
    { RenderEnums::L7, RenderEnums::M6, "snake_v", "snake_v" },
    { RenderEnums::L7, RenderEnums::M7, "gramp_v", "gramR_v" }
};

unsigned int NUM_CONTINUITY_ERRORS = 6;



//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//=============================================================================
// GameplayManager::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      GameplayManager
//
//=============================================================================
GameplayManager* GameplayManager::GetInstance()
{
//    rAssert( spInstance != NULL );
    
    return spInstance;
}

//=============================================================================
// GameplayManager::SetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ( GameplayManager* pInstance)
//
// Return:      GameplayManager
//
//=============================================================================
void GameplayManager::SetInstance( GameplayManager* pInstance )
{
//    rAssert( pInstance != NULL );
    
    spInstance = pInstance;
}

//==============================================================================
// GameplayManager::GameplayManager
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GameplayManager::GameplayManager() :
    mIsDemo( false ),
    //mVehicleIndex( -1 ),
    mCharacterIndex( -1 ),
    mShouldLoadDefaultVehicle(false),
    mSkipSunday( 0 ),
    mGameType( GT_NORMAL ),
    mIrisClosed( false ),
    mFadedToBlack( false ),
    mWaitingOnFMV( false ),
    mCurrentMission( -1 ),
    mNumPlayers( 0 ),
    mNumMissions( 0 ),
#ifdef RAD_GAMECUBE
    mCurrentMissionHeap( GMA_GC_VMM ),
#else
    mCurrentMissionHeap( GMA_LEVEL_MISSION ),
#endif
    mLevelComplete( false ),
    mGameComplete( false ),
    mCurrentVehicle( NULL ),
    //miNumLevelVehicles( 0 ),
    mNumBonusMissions( 0 ),
    mCurrentBonusMission( -1 ),
    mDesiredBonusMission( -1 ),
    mIsInBonusMission( false ),
    mFireBonusMissionDialogue( false ),
    mJumpToBonusMission( false ),
    mUpdateBonusMissions( true ),
    mCurrentMessage( NONE ),
    mpRespawnManager( NULL ),
    mIrisSpeed( 1.0f ),
    mPutPlayerInCar( false ),
    mbManualControlFade( false ),
    mCurrentVehicleIconID( -1 ),
    m_elapsedIdleTime( 0 )
{
    mEnablePhoneBooths = true;
    int i;
    //for( i = 0; i < MAX_LEVEL_VEHICLES; i++ )
    //{
    //    mLevelVehicles[ i ] = NULL;
    //}
    //strcpy(mDefaultVehicle,"NULL");
    
    
    // new
    // greg
    // jan 7, 2003
    
    for(i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        mMissionVehicleSlots[i].vehicle = NULL;
        mMissionVehicleSlots[i].name[0] = 0;
        //mMissionVehicleSlots[i].vehicleCentralIndex = -1;       
        
        mMissionVehicleSlots[i].pHuskVehicle = 0;
        mMissionVehicleSlots[i].usingHusk = false;
    
    }   
      
    for(i=0; i<MAX_MISSIONS + MAX_BONUS_MISSIONS; i++ ) 
    {
        mMissions[i] = NULL; 
    }

    if ( CommandLineOptions::Get( CLO_SKIP_SUNDAY ) )
    {
        mSkipSunday = 1;
    }

    //mLevelData.mission = RenderEnums::M1;

    for (i =0;i< MAX_VEHICLE_SLOTS;i++)
    {
        strcpy(mVehicleSlots[i].filename,"NULL");
        strcpy(mVehicleSlots[i].name,"NULL");
        mVehicleSlots[i].mp_vehicle=NULL;
        mVehicleSlots[i].heading =0;
        mVehicleSlots[i].position.x=0;
        mVehicleSlots[i].position.y=0;
        mVehicleSlots[i].position.z=0;
        
        mVehicleSlots[i].pHuskVehicle = 0;
        mVehicleSlots[i].usingHusk = false;
		
    }

	for (i=0;i<MAX_CHASEMANAGERS;i++)
	{
		strcpy(m_ChaseManager_Array[i].hostvehicle,"NULL");
		m_ChaseManager_Array[i].mp_chasemanager = NULL;
        strcpy(m_ChaseManager_Array[i].hostvehiclefilename,"NULL");
	}

    for(int index =0;index<MAX_VDU_CARS;index++)
    {
        mVDU.mpVehicle[index]=NULL;
    }
    mVDU.mCounter=0;
    mBlackScreenTimer = 2000; //Chuck set this to 1000 milliseconds
    mPlayerAndCarInfo.mbDirtyFlag =false;
    
    mPostLevelFMV[ 0 ] = 0;
}

//==============================================================================
// GameplayManager::~GameplayManager
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GameplayManager::~GameplayManager()
{
}

//=============================================================================
// GameplayManager::LevelLoaded
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::LevelLoaded()
{
    //GetGameFlow()->SetContext( CONTEXT_GAMEPLAY );

    //Now we should init level...
    mPostLevelFMV[ 0 ] = 0;
    mWaitingOnFMV = false;
//    GetGameFlow()->SetQuickStartLoading( false );

    InitLevelData();

    SetCurrentMission( mLevelData.mission * 2 + mSkipSunday );
}

//=============================================================================
// GameplayManager::SetLevelIndex
//=============================================================================
// Description: Comment
//
// Parameters:  (  RenderEnums::LevelEnum level  )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::SetLevelIndex(  RenderEnums::LevelEnum level  )
{
    //
    // Insert level hack here:
    //
#ifdef RAD_DEBUG
    //level = RenderEnums::L5;
#endif

    mLevelData.level = level;
    
    GetRenderManager()->SetLoadData( RenderEnums::LevelSlot,
                                     level,
                                     RenderEnums::M1 );
}

//=============================================================================
// GameplayManager::SetMissionIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ( RenderEnums::MissionEnum mission )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::SetMissionIndex( RenderEnums::MissionEnum mission )
{
    mLevelData.mission = mission;
}


bool GameplayManager::TestPosInFrustrumOfPlayer( const rmt::Vector& pos, int playerID, float radius )
{
    rAssert( 0 <= playerID && playerID < GetNumPlayers() );

    tPointCamera* pCam = (tPointCamera*)GetSuperCamManager()->GetSCC(playerID)->GetCamera();
    float oldFar = pCam->GetFarPlane();
    // Camera's quite far away in SuperSprint, so we'll omit the farplane hack
    if( ::GetGameplayManager()->GetGameType() != GameplayManager::GT_SUPERSPRINT )
    {
        pCam->SetFarPlane(250.0f);
    }
    bool r = pCam->SphereVisible(pos, radius);
    pCam->SetFarPlane(oldFar);
    return r;
}

//=============================================================================
// GameplayManager::ContinueGameplay
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::ContinueGameplay()
{
    if( GetLevelComplete() )
    {
        return;
    }

    Mission* currentMission = GetCurrentMission();

    if( currentMission == NULL )
    {
        SetCurrentMission( 0 );
    }
    else
    {
        switch( currentMission->GetState() )
        {
        case Mission::STATE_FAILED:
            {
                GetGuiSystem()->HandleMessage( GUI_MSG_RESUME_INGAME );

                // follow-through
            }
        case Mission::STATE_WAITING:
            {
                //Dirty dirty...
                currentMission->Reset();
                currentMission->GetCurrentStage()->Start();
                if ( currentMission->GetCurrentStage()->StartBonusObjective() )
                {
                    //Ugly.  Drink another for Darryl.
                    currentMission->StartBonusObjectives();
                }
                break;
            }

        case Mission::STATE_INPROGRESS:
            {
                currentMission->Reset();
                break;
            }
        case Mission::STATE_SUCCESS:
            {
                //
                // Stupid logic because sometimes the "final" stage
                // isn't the last stage in the mission
                //
                if( currentMission->IsComplete() )
                {
                    if ( mIsInBonusMission )
                    {
                        //Mark this bonus mission complete.
                        mBonusMissions[ mCurrentBonusMission - MAX_MISSIONS ].SetCompleted( true );
                    }
#ifdef RAD_E3
                    if ( currentMission->IsSundayDrive() )
                    {
                        NextMission();
                    }
                    else
                    {
                        PrevMission();  //Go back and forth only for E3.
                    }
#else
                    NextMission();
#endif                    
                }
                else
                {
                    currentMission->NextStage();

                    GetGuiSystem()->HandleMessage( GUI_MSG_RESUME_INGAME );
                }
                break;
            }
        default:
            {
                rAssert( false );
                break;
            }
        }
    }
}

//=============================================================================
// MissionManager::Update
//=============================================================================
// Description: 
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::Update( int elapsedTime )
{
    if ( mCurrentMessage != NONE )
    {
        switch ( mCurrentMessage )
        {
        case NEXT_MISSION:
            {
              
                DoNextMission();
                break;
            }
        case PREV_MISSION:
            {
                DoPrevMission();
                break;
            }
        default:
            {
                rAssert( false );
            }
        }

        mCurrentMessage = NONE;
    }

    bool isWager = false;

    if ( mFireBonusMissionDialogue )
    {
        isWager = GetMission( mDesiredBonusMission )->IsWagerMission();

        if ( !isWager )
        {
            //HEY, we wanna do a bonus mission..  Have to do it here since
            //we can't nest events that effect the Character.
            mBonusMissions[ mDesiredBonusMission - MAX_MISSIONS ].TriggerDialogue();

            //This is probably not the best thing to do.
            GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
        }

        mFireBonusMissionDialogue = false;
    }

    if ( mJumpToBonusMission || isWager )
    {
        //
        // if it's a bonus mission, then wipe out the bitmap on the mission briefing screen
        //

        CGuiScreenMissionLoad::ClearBitmap();

        if( isWager )
        {
            GetPresentationManager()->ReplaceMissionBriefingBitmap( "b_louie" );
        }
/*
        else
        {
            //
            // Clear the loaded mission briefing picture
            //
            CGuiScreenMissionBase::ClearBitmap();
        }
*/

        //Start the mission itself!
        SetCurrentMission( mDesiredBonusMission );
        mDesiredBonusMission = -1;
        mJumpToBonusMission = false;
    }

    //convert in seconds for ChaseManager's update
	float sectime = elapsedTime/1000.00f;
	//update any chase managers if we have any
	for (int i=0;i<MAX_CHASEMANAGERS;i++)
	{
		if (m_ChaseManager_Array[i].mp_chasemanager != NULL)
		{
			m_ChaseManager_Array[i].mp_chasemanager->Update(sectime);
		}
	}


    Mission* currentMission = GetCurrentMission();

    if( currentMission == NULL || GetLevelComplete() )
    {
        return;
    }

    if( currentMission->IsComplete() )
    {
        switch( currentMission->GetState() )
        {
        case Mission::STATE_SUCCESS:
            {
#ifdef RAD_E3
                if ( currentMission->IsSundayDrive() )
                {
                    NextMission();
                }
                else
                {
                    PrevMission();  //Go back and forth only for E3.
                }
#else
                NextMission();
#endif                    
                break;
            }
        case Mission::STATE_FAILED:
            {
                ContinueGameplay();
                break;
            }
        case Mission::STATE_INPROGRESS:
            {
                currentMission->Update( elapsedTime );
                break;
            }
        default:
            {
                rAssert( false );
            }
        } 
    }
    else
    {
        currentMission->Update( elapsedTime );
    }

    if ( mUpdateBonusMissions )
    {
        int bonusMission;
        for ( bonusMission = 0; bonusMission < mNumBonusMissions; ++bonusMission )
        {
            mBonusMissions[ bonusMission ].Update( elapsedTime );
        }
    }
    //chuck update the VDU struct and try to delete the cars
    UpdateVDU();

    //Chuck check if the screen is black if it is then update the blackscreentimer

    if (mFadedToBlack == true)
    {
        mBlackScreenTimer -= elapsedTime;

        //check if time less than 0 then transition out of black screen to 
        if (mBlackScreenTimer < 0)
        {
           PauseForFadeFromBlack();
        }
    }
     

 }

//=============================================================================
// GameplayManager::PlaceCharacterAtLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* character, CarStartLocator* locator )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::PlaceCharacterAtLocator( Character* character, 
                                               Locator* locator )
{
    rAssert( character != NULL );

    rAssert( locator != NULL );

    rmt::Vector pos;
    locator->GetLocation( &pos );
    float rotation = 0.0f;
    
    CarStartLocator* csl = dynamic_cast<CarStartLocator*>(locator);
    if(csl)
    {
        rotation = csl->GetRotation();
    }

    character->RelocateAndReset( pos, rotation );
}

//=============================================================================
// GameplayManager::PlaceVehicleAtLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle, const char* locatorName )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::PlaceVehicleAtLocator( Vehicle* vehicle, 
                                             CarStartLocator* locator )
{
    rAssert( vehicle != NULL );

    rAssert( locator != NULL );

    rmt::Vector pos;

    locator->GetLocation( &pos );
    
    // fucking butt-ugly hack, 4 days after scheduled final, to deal with setting an AI car ontop of user's car
    
    // 1 day later...
    // you fucking retard, only do this if you are not the users car...
    if(GetAvatarManager()->GetAvatarForPlayer(0))
    {
        Vehicle* playersCar = GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle();
        
        if(playersCar == NULL)
        {
            playersCar = this->GetCurrentVehicle();
        }       
        
        if(playersCar && playersCar != vehicle)
        {          
            if(TestProximityToUsersCarAndNudgeUpIfNecessaryDamnUglyHack(pos, playersCar))
            {
                pos.y += 2.0f;
            }
        }                
        
    }
               
    vehicle->SetInitialPositionGroundOffsetAutoAdjust( &pos );
    float rotation = locator->GetRotation();
    vehicle->SetResetFacingInRadians( rotation );
    vehicle->Reset( false );
   
    if( vehicle->IsVehicleDestroyed() )
    {
        Vehicle* husk = GetVehicleCentral()->mHuskPool.FindHuskGivenOriginalVehicle( vehicle );
        //chuck testing if the vehicle generates a husk or not.
        if (husk != NULL)
        {
            husk->SetInitialPositionGroundOffsetAutoAdjust( &pos );
            husk->SetResetFacingInRadians( rotation );
            husk->Reset();
        }
    }
}


//=============================================================================
// GameplayManager::TestProximityToUsersCarAndNudgeUpIfNecessaryDamnUglyHack
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool GameplayManager::TestProximityToUsersCarAndNudgeUpIfNecessaryDamnUglyHack(rmt::Vector& pos, Vehicle* playersCar)
{
    if(playersCar)
    {
        rmt::Vector playersCarPosition;
        playersCar->GetPosition(&playersCarPosition);

        playersCarPosition.Sub(pos);
        float dist = playersCarPosition.Magnitude();
        if(dist < 3.0f)
        {
            return true;
        }
    }
        
    return false;
}


//=============================================================================
// GameplayManager::PlaceVehicleAtLocation
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle, rmt::Vector pos, float rotation  )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::PlaceVehicleAtLocation( Vehicle* vehicle, rmt::Vector pos, float rotation )
{
    rAssert( vehicle != NULL );
    vehicle->SetInitialPositionGroundOffsetAutoAdjust( &pos );
    vehicle->SetResetFacingInRadians( rotation );
    vehicle->Reset();
}

//=============================================================================
// GameplayManager::PlaceVehicleAtLocatorName
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle, const char* locatorName )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::PlaceVehicleAtLocatorName( Vehicle* vehicle, 
                                                 const char* locatorName )
{
    CarStartLocator* loc = p3d::find<CarStartLocator>( locatorName );
    #ifndef FINAL
        char outputbuffer [255];
        if (loc == NULL)
        {
            sprintf(outputbuffer,"Error:Locator %s cannot be found, Make sure its loaded!\n",locatorName);
            rTuneAssertMsg(0,outputbuffer);
        }
    #endif
    PlaceVehicleAtLocator( vehicle, loc );
}

//=============================================================================
// GameplayManager::InitVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* vehicleName )
//
// Return:      Vehicle
//
//=============================================================================
/*
Vehicle* GameplayManager::InitVehicle( char* vehicleName )
{
    // just to make sure
    rAssert(0);
    return GetVehicleCentral()->InitVehicle( vehicleName, true );
}
*/

//=============================================================================
// GameplayManager::AddLevelVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* vehicleName )
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* GameplayManager::AddLevelVehicle( char* vehicleName,eCarSlots slot, char* confile )
{
  
    Vehicle* v = NULL;

    if ( mIsDemo )
    {
        v = GetVehicleCentral()->InitVehicle( vehicleName, true, confile, VT_AI );

        mAIIndex = GetVehicleCentral()->GetVehicleId( v );
        rAssert( mAIIndex != -1 );
        WaypointAI* wai = new WaypointAI( v );
        GetVehicleCentral()->SetVehicleController( mAIIndex, wai );
    }
    else
    {
        //If this car is being placed in the Default slot it is owned by the Player, so we use init the car with mbPlayerCar true.
        if(slot == GameplayManager::eDefaultCar)
        {
            // default car gets driver, if it isn't supressed
            v = GetVehicleCentral()->InitVehicle( vehicleName, true, confile, VT_USER, VehicleCentral::ALLOW_DRIVER,true );
        }
        else
        {

            // ai and other slot always have driver, even if supressed
            v = GetVehicleCentral()->InitVehicle( vehicleName, true, confile, VT_USER, VehicleCentral::FORCE_DRIVER);
        }

    }

       
   
#ifndef FINAL

    if(strcmp(mVehicleSlots[slot].name,vehicleName) != 0)
    {
        rReleasePrintf("Attempting to add vehicle, name mismatch!! \n");
        rAssert (0);
    }
#endif
    
    mVehicleSlots[slot].mp_vehicle = v;
                    

    v->AddRef();
    

    return v;
}

void GameplayManager::RemoveLevelVehicleController()
{
    //
    //Judging from the symmetry of the code, this appears to be a hack that I'm willfully adding.
    //When/if you figure out wtf Greg seems to resolute in his refusal to addref or release,
    //feel free to remove the code.
    //
    // This is a Triage Hack for demo-mode stability --dm 12/01/02
    //
    if ( mIsDemo )
    {
        VehicleController* v = GetVehicleCentral()->RemoveVehicleController( mAIIndex );
        if(v)
        {
            v->ReleaseVerified();
        }
    }
 }

//=============================================================================
// GameplayManager::PauseForIrisClose
//=============================================================================
// Description: Comment
//
// Parameters:  ( float speedMod )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::PauseForIrisClose( float speedMod )
{
    mIrisSpeed = speedMod;

    GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );
    GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
    GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_CLOSE );

    CGuiScreenIrisWipe* iw = static_cast<CGuiScreenIrisWipe*>(GetGuiSystem()->GetInGameManager()->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE ));
    iw->SetRelativeSpeed( mIrisSpeed );
}

//=============================================================================
// GameplayManager::PauseForIrisOpen
//=============================================================================
// Description: Comment
//
// Parameters:  ( float speedMod )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::PauseForIrisOpen( float speedMod )
{
    GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_OPEN );
    GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
    GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_OPEN );

    float speed = mIrisSpeed;

    if ( speedMod != 0.0f )
    {
        speed = speedMod;
    }

    CGuiScreenIrisWipe* iw = static_cast<CGuiScreenIrisWipe*>(GetGuiSystem()->GetInGameManager()->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_IRIS_WIPE ));
    iw->SetRelativeSpeed( speed );

    //Reset
    mIrisSpeed = 1.0f;
}

void GameplayManager::PauseForFadeToBlack( float speedMod )
{
    mIrisSpeed = speedMod;

    GetEventManager()->AddListener( this, EVENT_GUI_FADE_OUT_DONE );
    GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
    GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_FADE_OUT );
}

void GameplayManager::PauseForFadeFromBlack( float speedMod )
{
    //check for manual control if some system has locked the screen to say black 
    //return and do nothing, and let the mBlackScreenTimer do its job in The ::Update().

    if (mbManualControlFade == true || mBlackScreenTimer > 0)
    {
        return;
    }
    GetEventManager()->AddListener( this, EVENT_GUI_FADE_IN_DONE );
    GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
    GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_FADE_IN );

    //Reset
    mIrisSpeed = 1.0f;
}


//=============================================================================
// GameplayManager::AddMissionVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  (char* vehiclename, char* confile)
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* GameplayManager::AddMissionVehicle(char* vehiclename, char* confile, char* driver)
{
    // check if the vehicle is already in our list
    
    Vehicle* vehicleSavedFromExecution = NULL;
    this->ReleaseFromVDU(vehiclename, &vehicleSavedFromExecution);
    if(vehicleSavedFromExecution)
    {
        //GetVehicleCentral()->RemoveVehicleFromActiveList(vehicleSavedFromExecution);
        //chuck this was added to fixed bug 11114 but now cause popping at the end of l4m2
        //trying a different method
        AddToVDU(vehicleSavedFromExecution);
        
    }
    // check this down below
    
    int i;
    for(i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        if(strcmp(vehiclename, mMissionVehicleSlots[i].name) == 0)
        {
            // this vehicle is already in our list
            rTuneAssertMsg(vehicleSavedFromExecution == 0, "you are trying to use two of the same car in a mission");
            
            tRefCounted::Release(vehicleSavedFromExecution);
            return mMissionVehicleSlots[i].vehicle;
        }
    }
    
    // extra test -
    // you should never be calling AddMissionVehicle on a car that is in the OTHER slot
    rTuneAssertMsg( (strcmp(GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eOtherCar),vehiclename) != 0), "you can't add the OTHER car to a mission");
    
        
    for(i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        if(mMissionVehicleSlots[i].vehicle == 0)
        {
            // here is an empty slot
            
            if(vehicleSavedFromExecution != 0)
            {
                mMissionVehicleSlots[i].vehicle = vehicleSavedFromExecution;
                mMissionVehicleSlots[i].vehicle->ResetFlagsOnly(true);
                mMissionVehicleSlots[i].vehicle->SetDriverName(driver);
                mMissionVehicleSlots[i].vehicle->mDriverInit = VehicleCentral::FORCE_DRIVER;
                GetVehicleCentral()->SetupDriver(mMissionVehicleSlots[i].vehicle);
            }
            else
            {
                //mMissionVehicleSlots[i].vehicle = GetVehicleCentral()->InitVehicle(vehiclename, false, confile, VT_AI);  // allow driver I guess?
                  
                mMissionVehicleSlots[i].vehicle = GetVehicleCentral()->InitVehicle( vehiclename,
                                                                                    false,          // add to active list
                                                                                    confile,
                                                                                    VT_AI,          // VehicleType
                                                                                    VehicleCentral::ALLOW_DRIVER,   
                                                                                    false,          // player car
                                                                                    //false);         // start out of car  
                                                                                    true);
        
                  
                  
                                                                                                                     
                if(driver && (driver[0] != 0))
                {
                    mMissionVehicleSlots[i].vehicle->mDriverInit = VehicleCentral::FORCE_DRIVER;
                    mMissionVehicleSlots[i].vehicle->SetDriverName(driver);
                    GetVehicleCentral()->SetupDriver(mMissionVehicleSlots[i].vehicle);
                }
            }
            
            strcpy(mMissionVehicleSlots[i].name, vehiclename);
            
            mMissionVehicleSlots[i].vehicle->AddRef();

            GetEventManager()->TriggerEvent(EVENT_MISSION_VEHICLE_CREATED,mMissionVehicleSlots[i].vehicle);

            //update the AI vehicle slot so It points to the correct vehicle.
            if(strcmp(GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eAICar),"NULL") != 0)
            {
                if(strcmp(mMissionVehicleSlots[i].vehicle->GetName(),GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eAICar)) ==0)
                {
                    GetGameplayManager()->mVehicleSlots[GameplayManager::eAICar].mp_vehicle = mMissionVehicleSlots[i].vehicle;
                }
            }


            
            //int dummy = GetVehicleCentral()->AddVehicleToActiveList(mMissionVehicleSlots[i].vehicle);
            //if(dummy == -1)
            //{
            //    rAssertMsg(0, "can't add vehicle to active list - see greg \n");
            //}
            //else
            //{
            //    //mMissionVehicleSlots[i].vehicleCentralIndex = dummy;
            //}            
      
            tRefCounted::Release(vehicleSavedFromExecution);
            return mMissionVehicleSlots[i].vehicle;
        }
    }   
        
    tRefCounted::Release(vehicleSavedFromExecution);
    return 0;   // bad
}


//=============================================================================
// GameplayManager::EmptyMissionVehicleSlots
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::EmptyMissionVehicleSlots()
{
    int i;
    for(i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        if(mMissionVehicleSlots[i].vehicle != NULL)
        {
            if(mMissionVehicleSlots[i].usingHusk)
            {               
                spInstance->AddToVDU(mMissionVehicleSlots[i].pHuskVehicle);              
                //GetVehicleCentral()->mHuskPool.FreeHusk(mMissionVehicleSlots[i].pHuskVehicle);                
                mMissionVehicleSlots[i].pHuskVehicle->Release();
                mMissionVehicleSlots[i].pHuskVehicle = NULL;
                mMissionVehicleSlots[i].usingHusk = false;
            }
            else
            {
                spInstance->AddToVDU(mMissionVehicleSlots[i].vehicle);
            }
        
            GetEventManager()->TriggerEvent(EVENT_MISSION_VEHICLE_RELEASED,mMissionVehicleSlots[i].vehicle); 
              
             //update the AI vehicle slot so It points to the correct vehicle.
            if(strcmp(GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eAICar),"NULL") != 0 && GetGameplayManager()->mVehicleSlots[GameplayManager::eAICar].mp_vehicle != NULL )
            {
                //update the Gameplaymanagers AICar slot mp_vehicle ptr.
                if(strcmp(mMissionVehicleSlots[i].vehicle->GetName(),GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eAICar)) ==0)
                {
                    GetGameplayManager()->mVehicleSlots[GameplayManager::eAICar].mp_vehicle = NULL;
                }
            }
            
            mMissionVehicleSlots[i].vehicle->Release();           
            mMissionVehicleSlots[i].vehicle = 0;
            mMissionVehicleSlots[i].name[0] = 0;         
    
        }    
    }
}


//=============================================================================
// GameplayManager::GetMissionVehicleByName
//=============================================================================
// Description: Comment
//
// Parameters:  (char* name)
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* GameplayManager::GetMissionVehicleByName( const char* name)
{
    int i;
    for(i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        if(strcmp(mMissionVehicleSlots[i].name, name) == 0)
        {
            return mMissionVehicleSlots[i].vehicle;
        }
    }
    return 0;
    
}

//=============================================================================
// GameplayManager::GetUserVehicleByName
//=============================================================================
// Description: Comment
//
// Parameters:  (char* name)
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* GameplayManager::GetUserVehicleByName(const char* name)
{
    if(mVehicleSlots[eDefaultCar].mp_vehicle &&
       strcmp(mVehicleSlots[eDefaultCar].name, name) == 0)
    {
        return mVehicleSlots[eDefaultCar].mp_vehicle;
    }
    
    if(mVehicleSlots[eOtherCar].mp_vehicle &&
       strcmp(mVehicleSlots[eOtherCar].name, name) == 0)
    {
        return mVehicleSlots[eOtherCar].mp_vehicle;
    }

    if(strcmp("current",name) == 0)
    {
         return GetCurrentVehicle();
    }
    
    return 0;
    
    // recall, mp_vehicle pointer in eAICar is never filled or used.
}


//=============================================================================
// GameplayManager::GetMissionVehicleIndex
//=============================================================================
// Description: Comment
//
// Parameters:  (Vehicle* vehicle)
//
// Return:      int 
//
//=============================================================================
int GameplayManager::GetMissionVehicleIndex(Vehicle* vehicle)
{
    int i;
    for(i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        if(mMissionVehicleSlots[i].vehicle == vehicle)
        {
            return GetVehicleCentral()->GetVehicleId(mMissionVehicleSlots[i].vehicle, false);
        }
    }
    return -1;
}




void GameplayManager::RepairVehicle( CarDataStruct* carData )
{   
    // check that the car is one of ours
    rAssert( carData );
    rAssert( carData == &(mVehicleSlots[ eDefaultCar ]) || 
        carData == &(mVehicleSlots[ eOtherCar ]) );
    rAssert( GetGameplayManager()->GetCurrentVehicle() == carData->mp_vehicle );

    //
    // if this car is fully destroyed, we need to 
    //   - put any NPC driver back into the driver seat
    //
    // if it was also using a husk, we need to 
    //   - swap out the husk
    //   - add back original vehicle (only do this if we have husk,
    //     cuz GameplayManager never removes original vehicle if there 
    //     wasn't husk for it).
    // 
    if( carData->mp_vehicle->mVehicleDestroyed )
    {
        if( carData->usingHusk )
        {
            // Want to swap out the husk and put the original vehicle
            // back in its place... so...
            // 
            // First we need to populate a matrix with husk's position and facing
            // and set the original vehicle's location to these position and facing
            // 
            rmt::Vector carPosition = carData->pHuskVehicle->rPosition();
            float ang = carData->pHuskVehicle->GetFacingInRadians();
            rmt::Matrix m;
            m.Identity();
            m.FillRotateXYZ( 0.0f, ang, 0.0f );
            m.FillTranslate( carPosition );
            carData->mp_vehicle->SetTransform( m );

            // Remove the husk
            GetVehicleCentral()->RemoveVehicleFromActiveList( carData->pHuskVehicle );
            GetVehicleCentral()->mHuskPool.FreeHusk( carData->pHuskVehicle );
            carData->pHuskVehicle->Release();
            carData->pHuskVehicle = NULL;
            carData->usingHusk = false;

            // Add back the original vehicle
            GetVehicleCentral()->AddVehicleToActiveList( carData->mp_vehicle );

            // if the avatar is inside a vehicle the vehicle
            // is probably a husk, update this vehicle to be the original 
            // vehicle and place the character in this new vehicle
            // 
            Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
            if( avatar->IsInCar() )
            {
                rAssert( avatar->GetVehicle() );
                rAssert( GetVehicleCentral()->mHuskPool.IsHuskType( avatar->GetVehicle()->mVehicleID ) );

                avatar->SetVehicle( carData->mp_vehicle );

                Character* character = avatar->GetCharacter();
                GetAvatarManager()->PutCharacterInCar( character, carData->mp_vehicle );
            }
        }

        // put the driver back inside the original vehicle (done
        // regardless of whether or not the vehicle obtained a husk
        // when it was destroyed.
        GetVehicleCentral()->SetupDriver( carData->mp_vehicle );

        // fire off event so Esan can know when we switch the vehicle on him.
        GetEventManager()->TriggerEvent(
            EVENT_VEHICLE_DESTROYED_SYNC_SOUND, (void*)carData->mp_vehicle );
    }
    // reset original vehicle's states
    bool resetDamage = true;
    carData->mp_vehicle->ResetFlagsOnly( resetDamage );
}




//=============================================================================
// GameplayManager::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch (id)
    {
    //events we are interested in.
    //case EVENT_GETINTOTRAFFIC_END:
    case EVENT_GETINTOVEHICLE_END:
        {            
            Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );

            Vehicle* avatarVehicle = avatar->GetVehicle();
            rAssert( avatarVehicle );

            if( GetVehicleCentral()->mHuskPool.IsHuskType( avatarVehicle->mVehicleID ) )
            {
                // we're driving a husk.. so if this is a husk allocated
                // by the husk pool, then we've got to set the current 
                // vehicle to point to the original vehicle (not the husk itself)
                // But if we couldn't find the original vehicle, then this
                // husk either: 
                //   1) doesn't belong to the pool (somehow we created an original husk vehicle), or
                //   2) belongs to the pool, but is a free husk (so why do we have its pointer here??)

                Vehicle* originalVehicle = GetVehicleCentral()->
                    mHuskPool.FindOriginalVehicleGivenHusk( avatarVehicle );

                rAssert( originalVehicle );

                SetCurrentVehicle( originalVehicle );
            }
            else
            {
                SetCurrentVehicle( avatarVehicle );
            }

            break;
        }
    case EVENT_GETOUTOFVEHICLE_START:
        {
            break;
        }
    case EVENT_BONUS_MISSION_DIALOGUE: 
        {
            //OKAY, let's start a bonus mission dialogue!
            rAssert( GetCurrentMission()->IsSundayDrive() == true );

            //Which mission?
            EventLocator* evtLoc = static_cast<EventLocator*>(pEventData);

            int i;
            for ( i = 0; i < mNumBonusMissions; ++i )
            {
                if ( mBonusMissions[ i ].GetEventLocator() == evtLoc )
                {
                    //Store this for next time.
                    mDesiredBonusMission = i + MAX_MISSIONS;
                    break;
                }
            }

            rAssert( i < mNumBonusMissions );

            //We have to fire the dialogue event outside of an eventhandler...  Sigh.
            mFireBonusMissionDialogue = true;
            break;
        }
    case EVENT_CONVERSATION_DONE_AND_FINISHED:
        {
            if ( mDesiredBonusMission != -1 )
            {
                mJumpToBonusMission = true;

                GetBonusMissionInfo( mDesiredBonusMission - MAX_MISSIONS )->ResetCharacterPositions();
            }
            break;
        }
    case EVENT_REPAIR_CAR:
        {
            // Dusit [08/04/2003]:
            // the fact that we're not passing in the vehicle pointer as pEventData
            // and the fact that this event is only fired in the main game (not in demo
            // or supersprint) imply that we're only handling the event if the player 
            // picks up a wrench/repair icon.
            //
            // The rule is that we repair the user's current vehicle (the one he
            // was last in, as pointed to by                 
            // 
            //     Avatar::GetLastVehicle()
            // 
            // All managers that deal with husks (gameplaymanager, trafficmanager,
            // parkedcarmanager, and chasemanager) will need to test the current vehicle
            // against their list of vehicles to see if they need to swap out the husk
            // of that vehicle (if it's damaged to that extent)... otherwise, just reset
            // the vehicle's damage state.
            //

            // NOTE: 
            // We are assuming here that the gameplaymanager's current vehicle
            // is never set to the husk, but remains the original vehicle, even if 
            // it was destroyed and replaced with a husk
            //
            Vehicle* currVehicle = GetGameplayManager()->GetCurrentVehicle();
            if( currVehicle == NULL )
            {
                break;
            }
            
            // check the default car slot's original vehicle pointer
            Vehicle* testVehicle = mVehicleSlots[ eDefaultCar ].mp_vehicle;
            if( testVehicle == currVehicle ) // which also implies testVehicle != NULL
            {
                RepairVehicle( &(mVehicleSlots[ eDefaultCar ]) );
                                
            }
            else
            {
                // now check the other car slot
                testVehicle = mVehicleSlots[eOtherCar].mp_vehicle;
                if( testVehicle == currVehicle ) // which also implies testVehicle != NULL
                {
                    RepairVehicle( &(mVehicleSlots[ eOtherCar ]) );

                    // if it exists in OtherSlot, it could also exist in MissionVehicleSlots
                    // clear the husk out of these too
                    for( int i=0; i<MAX_MISSION_VEHICLE_SLOTS; i++ )
                    {
                        if( mMissionVehicleSlots[i].vehicle == testVehicle )
                        {
                            mMissionVehicleSlots[i].pHuskVehicle->Release();
                            mMissionVehicleSlots[i].pHuskVehicle = NULL;
                            mMissionVehicleSlots[i].usingHusk = false;
                        }
                    }
                }
            }


            break;
        }
        
    case EVENT_VEHICLE_DESTROYED:
        {
            bool foundInVehicleSlots = false;

            // see if the vehicle is one of ours and if so, swap in husk:
            for(int i = 0; i < MAX_VEHICLE_SLOTS; i++)
            {
                Vehicle* v = (Vehicle*)pEventData;
                if( mVehicleSlots[i].mp_vehicle == v &&
                    mVehicleSlots[i].usingHusk == false )
                {
                    foundInVehicleSlots = true;


                    // one of ours
                    // swap in a husk
                    VehicleType vt = mVehicleSlots[i].mp_vehicle->mVehicleType;
                    Vehicle* husk = GetVehicleCentral()->mHuskPool.RequestHusk
                        ( vt, mVehicleSlots[i].mp_vehicle );
                    if(husk)
                    {
                        husk->AddRef();
                        mVehicleSlots[i].pHuskVehicle = husk;
                        mVehicleSlots[i].usingHusk = true;

                        // try to find it in MissionVehicleSlot... It's possible for the
                        // same car to exist in 2 lists: the principle (heavy-weight) AI car,
                        // the forced car (for forced car missions), etc.
                        for( int j=0; j<MAX_MISSION_VEHICLE_SLOTS; j++ )
                        {
                            if( mMissionVehicleSlots[j].vehicle == v )
                            {
                                husk->AddRef();
                                mMissionVehicleSlots[j].pHuskVehicle = husk;
                                mMissionVehicleSlots[j].usingHusk = true;
                            }
                        }
                        
                        // set position and facing
                        rmt::Vector carPosition = mVehicleSlots[i].mp_vehicle->rPosition();
                        float ang = mVehicleSlots[i].mp_vehicle->GetFacingInRadians();
                                                
                        rmt::Matrix m;
                        m.Identity();
                        m.FillRotateXYZ( 0.0f, ang, 0.0f );
                        m.FillTranslate(carPosition);
                                      
                        GetVehicleCentral()->RemoveVehicleFromActiveList(mVehicleSlots[i].mp_vehicle);
                    
                        husk->SetTransform(m);
                        
                        GetVehicleCentral()->AddVehicleToActiveList(husk);

                        //
                        // Don't set mCurrentVehicle to husk because we want to preserve the original 
                        // vehicle for comparison during the REPAIR event
                        //
                        // Also, don't set the avatar's vehicle to husk because we want the player
                        // to play dodge animation out of the vehicle. 
                        // 
                        //this->SetCurrentVehicle(husk);
                        //Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
                        //avatar->SetVehicle(husk);
                        
                        // new
                        // fire off event so Esan can sync stuff up.
                        GetEventManager()->TriggerEvent(EVENT_VEHICLE_DESTROYED_SYNC_SOUND, (void*)husk);
                        
                        break;           
                    }    
                }
        
            }
            
            if( !foundInVehicleSlots )
            {
                for(int i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
                {
                    if( mMissionVehicleSlots[i].vehicle == (Vehicle*)pEventData &&
                        mMissionVehicleSlots[i].usingHusk == false )
                    {
                        // one of ours
                        // swap in a husk
                        VehicleType vt = mMissionVehicleSlots[i].vehicle->mVehicleType;
                        Vehicle* husk = GetVehicleCentral()->mHuskPool.RequestHusk
                            ( vt, mMissionVehicleSlots[i].vehicle );
                            
                        if(husk)
                        {
                            husk->AddRef();
                            mMissionVehicleSlots[i].pHuskVehicle = husk;
                            mMissionVehicleSlots[i].usingHusk = true;
                            
                            
                            // set position and facing
                            rmt::Vector carPosition = mMissionVehicleSlots[i].vehicle->rPosition();
                            float ang = mMissionVehicleSlots[i].vehicle->GetFacingInRadians();
                                                    
                            rmt::Matrix m;
                            m.Identity();
                            m.FillRotateXYZ( 0.0f, ang, 0.0f );
                            m.FillTranslate(carPosition);
                                                    
                            
                            GetVehicleCentral()->RemoveVehicleFromActiveList(mMissionVehicleSlots[i].vehicle);
                            //mMissionVehicleSlots[i].vehicleCentralIndex = -1;
                            
                            husk->SetTransform(m);
                            
                            int index = GetVehicleCentral()->AddVehicleToActiveList(husk);
                            //mMissionVehicleSlots[i].vehicleCentralIndex = index;
                            
                            // TODO - is this correct?
                            // are we thrown out of the car?
                            // is it safe to set mCurrentVehicle to 0?
                            //this->SetCurrentVehicle(husk);
                            //Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
                            //avatar->SetVehicle(husk); - what the hell were you doing you retard?
                            
                            // new
                            // fire off event so Esan can sync stuff up.
                            GetEventManager()->TriggerEvent(EVENT_VEHICLE_DESTROYED_SYNC_SOUND, (void*)husk);
                            
                            break;           
                        }       
                    }
                } // end of for loop
            }
            break;
        }
        
    case EVENT_GUI_IRIS_WIPE_CLOSED:
        {
            GetGameFlow()->GetContext( GetGameFlow()->GetCurrentContext() )->Resume();
            GetEventManager()->RemoveListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );
            rAssert( mFadedToBlack == false );
            mIrisClosed = true;
            GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_OPEN );

            //Chuck WTF if comment this line out the game seem to go into a suspend state.
            //GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_OPEN );
            break;
        }
    case EVENT_GUI_FADE_OUT_DONE:
        {
            GetGameFlow()->GetContext( GetGameFlow()->GetCurrentContext() )->Resume();
            GetEventManager()->RemoveListener( this, EVENT_GUI_FADE_OUT_DONE );
            rAssert( mIrisClosed == false );
            mFadedToBlack = true;
            mBlackScreenTimer = 2000; //Chuck set this timer to 1000 milliseconds, we will use this to countdown
            break;
        }
    case EVENT_GUI_IRIS_WIPE_OPEN:
    case EVENT_GUI_FADE_IN_DONE:
        {
            GetGameFlow()->GetContext( GetGameFlow()->GetCurrentContext() )->Resume();
            GetEventManager()->RemoveListener( this, EVENT_GUI_IRIS_WIPE_OPEN );
            GetEventManager()->RemoveListener( this, EVENT_GUI_FADE_IN_DONE );

            //Reset both.
            mIrisClosed = false;
            mFadedToBlack = false;
            break;
        }
    case EVENT_GUI_MISSION_LOAD_COMPLETE:
        {
            spInstance->MDKVDU();
            break;
        }
    case EVENT_WAYAI_HIT_LAST_WAYPOINT:
        {
            if ( mIsDemo )
            {
                HeapMgr()->DumpHeapStats( true );
            }
        }
    case EVENT_USER_CANCEL_MISSION_BRIEFING:
        {
            if ( GetCurrentMission()->IsForcedCar() == true)
            {
                //do some swapp.
                int swap =1;
                //put the player on the ground and place them were they were before when conversation started.
                //lock the forced car so the player can't enter.
                mVehicleSlots[eOtherCar].mp_vehicle->TransitToAI();
                GetCharacterManager()->RemoveCharacter(mVehicleSlots[eOtherCar].mp_vehicle->GetDriver());
                mVehicleSlots[eOtherCar].mp_vehicle->SetDriver(NULL);
            }
            
            if(GetCurrentMission()->IsBonusMission())
            {
                GetCharacterManager()->ResetBonusCharacters();
            }

            break;
        }
    case EVENT_USER_CANCEL_PAUSE_MENU:
        {
            //Allow user control of car, do this incase we exited from a stage with a countdown presentation.
            mCurrentVehicle->SetDisableGasAndBrake(false);
            mPlayerAndCarInfo.mbDirtyFlag = false;
            if(GetCurrentMission()->IsForcedCar() ==true)
            {  //we are quiting from forced car mission we need to lock the car so user cannot drive it.
                mVehicleSlots[eOtherCar].mp_vehicle->TransitToAI();
                Character* c = mVehicleSlots[eOtherCar].mp_vehicle->GetDriver();
                if(c)
                {
                    GetCharacterManager()->RemoveCharacter(c);
                }
                mVehicleSlots[eOtherCar].mp_vehicle->SetDriver(NULL);
            }

            if(GetCurrentMission()->IsBonusMission())
            {
                GetCharacterManager()->ResetBonusCharacters();
            }
            break;
        }

    default:
    
        {

            break; 
        }
    }
}


//returns a pointer to a vehicle if found in the internal index.
/*
Vehicle* GameplayManager::GetVehicle(char* name)
{
    for (int i=0; i<miNumLevelVehicles;i++)
    {
        //found the car
        if(strcmp(mLevelVehicles[i]->mName,name)==0)
        {
            return mLevelVehicles[i];
        }
    }

    //default if we didnt find car return a NULL
    return NULL;
}
*/

//returns lasted  used car
Vehicle* GameplayManager::GetCurrentVehicle( void)
{
    //rAssert( mCurrentVehicle != NULL);
    return mCurrentVehicle;
}

/*=============================================================================
Force whatever icon we have for the HUD to be unregistered.
=============================================================================*/
void GameplayManager::UnregisterVehicleHUDIcon(void)
{
    CGuiScreenHud* currentHud = GetCurrentHud();
    if(currentHud && mCurrentVehicleIconID != -1)
    {
        currentHud->GetHudMap( 0 )->UnregisterIcon( mCurrentVehicleIconID );
        mCurrentVehicleIconID = -1;
    }
}

//sets current car
void GameplayManager::SetCurrentVehicle(Vehicle* vehicle)
{

    if( vehicle )
    {
        // 
        // Dusit [08/04/2003]:
        // Make sure here that we don't set make husk (that is, the husk
        // that replaced our original vehicle because it got destroyed) 
        // as the current vehicle because there is code in various managers' 
        // HandleEvent() for EVENT_VEHICLE_DESTROYED and EVENT_REPAIR_CAR 
        // cases that assume mCurrentVehicle points to the original vehicle.
        // 
        rAssert( !GetVehicleCentral()->mHuskPool.IsHuskType( vehicle->mVehicleID ) );
    }

    //release old add ref
    if (mCurrentVehicle != NULL)
    {
        mCurrentVehicle->Release();
    }

    mCurrentVehicle = vehicle;
    
    if (mCurrentVehicle !=NULL )
    {
        mCurrentVehicle->AddRef();
    }


    // update player car icon in HUD map w/ new vehicle reference
    //
    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        // register new icon only if current vehicle is a user vehicle
        //
        if( mCurrentVehicle != NULL && mCurrentVehicle->mVehicleType == VT_USER )
        {
            // un-register old icon first
            //
            if( mCurrentVehicleIconID != -1 )
            {
                currentHud->GetHudMap( 0 )->UnregisterIcon( mCurrentVehicleIconID );
                mCurrentVehicleIconID = -1;
            }

            mCurrentVehicleIconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_PLAYER_CAR,
                                                                              rmt::Vector( 0.0f, 0.0f, 0.0f ),
                                                                              mCurrentVehicle );
        }
    }

    if ( vehicle )
    {
        //
        // This seems to be the best place to inform the sound manager
        // of the new car, just in case someone introduces a new way
        // to switch cars.  I'm open to other suggestions, though---Esan
        //
        GetSoundManager()->LoadCarSound( vehicle, true );
    }
}

//Call this to free memory taken up by player car if it was loaded with LoadDisposeable Car.
void GameplayManager::DumpCurrentCar ()
{
    if (mCurrentVehicle !=NULL )
    {
        if ( mVehicleSlots[eDefaultCar].mp_vehicle == mCurrentVehicle)
        {            
            ClearVehicleSlot(eDefaultCar);        
        }
        else if (mVehicleSlots[eOtherCar].mp_vehicle == mCurrentVehicle)
        {
            ClearVehicleSlot(eOtherCar);
        }
        //if the first two conditions are not met then player must have hijacked a AI car
        else if (mVehicleSlots[eDefaultCar].mp_vehicle != NULL)
        {
            //rAssertMsg( 0, "I don't think we should ever be here - see greg \n" );
            // no, actually it's ok - perhaps they drove a husk up to a phone booth

            //clear the current car
            GetVehicleCentral()->RemoveVehicleFromActiveList(mCurrentVehicle);

            //clear our default car too now.            
            ClearVehicleSlot(eDefaultCar);
        }
        else if (mVehicleSlots[eOtherCar].mp_vehicle != NULL)
        {            
            //rAssertMsg( 0, "I don't think we should ever be here - see greg \n" );
            ClearVehicleSlot(eOtherCar);
        }
        else
        {
            //we are getting rid some free car or traffic car
            GetVehicleCentral()->RemoveVehicleFromActiveList(mCurrentVehicle);
        }
        SetCurrentVehicle( NULL );       
    }
}

//=============================================================================
// GameplayManager::ClearVehicleSlotIfInSphere
//=============================================================================
void GameplayManager::ClearVehicleSlotIfInSphere( eCarSlots slot, const rmt::Sphere& s )
{
    Vehicle* vehicle = GetVehicleInSlot( slot );
    rmt::Vector position;
    if( vehicle != NULL &&
        GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle() != vehicle )
    {
        vehicle->GetPosition( &position );
        bool isVehicleInSphere = s.Contains( position );
        if( isVehicleInSphere )
        {
            ClearVehicleSlot( slot );
        }
    }
}

//=============================================================================
// GameplayManager::DumpCurrentCarIfInSphere
//=============================================================================
// Description: Dumps the current car only if it was in the sphere provided
//
// Parameters:  s - the sphere that the car's center must be inside
//
// Return:      void
//
//=============================================================================
void GameplayManager::DumpCurrentCarIfInSphere( const rmt::Sphere& s )
{
    ClearVehicleSlotIfInSphere( eDefaultCar, s );
    ClearVehicleSlotIfInSphere( eOtherCar,   s );
    //ClearVehicleSlotIfInSphere( eAICar,      s );
}

//=============================================================================
// GameplayManager::MakeSureHusksAreReverted
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::MakeSureHusksAreReverted(Vehicle* pvehicle)
{
    // just called from Mission::Reset
    int i;
    for(i = 0; i < MAX_VEHICLE_SLOTS; i++)
    {
        //we should only fix car if they match what is need for the mission stage 
        if( (mVehicleSlots[i].usingHusk == true) && (mVehicleSlots[i].mp_vehicle == pvehicle))
        {
            GetVehicleCentral()->RemoveVehicleFromActiveList(mVehicleSlots[i].pHuskVehicle);
            GetVehicleCentral()->mHuskPool.FreeHusk(mVehicleSlots[i].pHuskVehicle);
            mVehicleSlots[i].pHuskVehicle->Release();
            mVehicleSlots[i].pHuskVehicle = NULL;
            mVehicleSlots[i].usingHusk = false;
            
            if(mVehicleSlots[i].mp_vehicle)
            {
                // add back to world
                mVehicleSlots[i].mp_vehicle->ResetFlagsOnly(true);
                GetVehicleCentral()->AddVehicleToActiveList(mVehicleSlots[i].mp_vehicle);
                
            }
        }
    }    
    for(i = 0; i < MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        if ( (mMissionVehicleSlots[i].usingHusk == true) && (mMissionVehicleSlots[i].vehicle == pvehicle))
        {
            GetVehicleCentral()->RemoveVehicleFromActiveList(mMissionVehicleSlots[i].pHuskVehicle);
            GetVehicleCentral()->mHuskPool.FreeHusk(mMissionVehicleSlots[i].pHuskVehicle);
            mMissionVehicleSlots[i].pHuskVehicle->Release();
            mMissionVehicleSlots[i].pHuskVehicle = NULL;
            mMissionVehicleSlots[i].usingHusk = false;        
            
            // shoudl this also add back the vehicles to the world???
            if(mMissionVehicleSlots[i].vehicle)
            {
                mMissionVehicleSlots[i].vehicle->ResetFlagsOnly(true);
                GetVehicleCentral()->AddVehicleToActiveList(mMissionVehicleSlots[i].vehicle);
            }
            
        }
    }
}


//=============================================================================
//dumps car in slot from inventory and resets the slot 
// GameplayManager::ClearVehicleSlot 
//=============================================================================
// Description: Comment
//
// Parameters:  (eCarSlots slot)
//
// Return:      void 
//
//=============================================================================
void GameplayManager::ClearVehicleSlot (eCarSlots slot)
{
        

    if( mVehicleSlots[slot].mp_vehicle != NULL ||
        mVehicleSlots[slot].pHuskVehicle != NULL )
    {
        p3d::pddi->DrawSync(); 
        
        if(mVehicleSlots[slot].usingHusk)
        {
            rAssert( mVehicleSlots[slot].pHuskVehicle );

            GetVehicleCentral()->RemoveVehicleFromActiveList(mVehicleSlots[slot].pHuskVehicle);
            GetVehicleCentral()->mHuskPool.FreeHusk(mVehicleSlots[slot].pHuskVehicle);
            mVehicleSlots[slot].pHuskVehicle->Release();
            mVehicleSlots[slot].pHuskVehicle = NULL;
            mVehicleSlots[slot].usingHusk = false;
        }
        else
        {
            rAssert( mVehicleSlots[slot].mp_vehicle );

            GetEventManager()->TriggerEvent(EVENT_VEHICLE_DESTROYED_SYNC_SOUND,mVehicleSlots[slot].mp_vehicle); 
          
            //if the slot we are removing  this car is the in the other we put it into the VDU since we dont want popping
            if (slot == GameplayManager::eOtherCar)
            {
                spInstance->AddToVDU(mVehicleSlots[slot].mp_vehicle);
            }
            else
            {
                GetVehicleCentral()->RemoveVehicleFromActiveList(mVehicleSlots[slot].mp_vehicle);
            }
        }       

        if( mVehicleSlots[slot].mp_vehicle )
        {
            mVehicleSlots[slot].mp_vehicle->Release ();        
            mVehicleSlots[slot].mp_vehicle =NULL;
        }

        p3d::inventory->RemoveSectionElements(mVehicleSlots[slot].filename);
        p3d::inventory->DeleteSection(mVehicleSlots[slot].filename);        

        //we should never clear the default cars info
        
        // if we load "other" car, we need this info around
        
        // but, loading a new phone booth car will be essentially loading a new default car
        // things should still work ok, as long as the loading of the phone booth car overwrites
        // all the default slots
        
        if (slot != GameplayManager::eDefaultCar)
        {
            strcpy(mVehicleSlots[slot].name,"Null");
            strcpy(mVehicleSlots[slot].filename,"Null");
            mVehicleSlots[slot].heading=0;
            mVehicleSlots[slot].position.x =0;
            mVehicleSlots[slot].position.y =0;
            mVehicleSlots[slot].position.z =0;
        }
        else
        {
            UnregisterVehicleHUDIcon();
        }
    }
    
}

void GameplayManager::CopyVehicleSlot( eCarSlots sourceSlot, eCarSlots destSlot )
{            
    ClearVehicleSlot(sourceSlot);     
    // Now copy the data from source to dest

    // copy name and filenames over
    const char* vehicleName = mVehicleSlots[ sourceSlot ].name;
    strcpy( mVehicleSlots[ destSlot ].filename, mVehicleSlots[ sourceSlot ].filename);
    strcpy( mVehicleSlots[ destSlot ].name, vehicleName );

    Vehicle* vehicle = GetVehicleCentral()->InitVehicle( vehicleName, true, 0, VT_USER );
    rAssert( vehicle != NULL );
    mVehicleSlots[ destSlot ].mp_vehicle = vehicle;
    // lets not use a husk, make a new one
    mVehicleSlots[ destSlot ].pHuskVehicle = NULL;
    mVehicleSlots[ destSlot ].usingHusk = false;
    // Use the same position ( could this cause a problem? )
    mVehicleSlots[ destSlot ].position = mVehicleSlots[ sourceSlot ].position;
    mVehicleSlots[ destSlot ].heading = mVehicleSlots[ sourceSlot ].heading;
}

//returns filename/path of car in that slot
char* GameplayManager::GetVehicleSlotFilename(eCarSlots slot)
{
    return mVehicleSlots[slot].filename;
}


//returns the name of the car in that slot
char* GameplayManager::GetVehicleSlotVehicleName(eCarSlots slot)
{
    return mVehicleSlots[slot].name;
}

//=============================================================================
// GameplayManager::GetVehicleInSlot
//=============================================================================
// Description: Comment
//
// Parameters:  (eCarSlots slot)
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* GameplayManager::GetVehicleInSlot(eCarSlots slot)
{
    return mVehicleSlots[slot].mp_vehicle;
}


//=============================================================================
// GameplayManager::SetBonusMissionInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* NPCname, const char* missionName, const char* iconName, bool isOneShot )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::SetBonusMissionInfo( const char* NPCname, 
                                           const char* missionName, 
                                           const char* iconName, 
                                           const char* dialogueName, 
                                           bool isOneShot,
                                           const char* alternateIconName,
                                           bool wasCompletedAlready )
{
MEMTRACK_PUSH_GROUP( "GamePlayManager::BonusMissionInfo" );
    //Which mission is this for?

int missionNum = GetMissionNumByName( missionName );
    rAssert( missionNum >= 0 && missionNum >= MAX_MISSIONS && missionNum < MAX_BONUS_MISSIONS + MAX_MISSIONS );

    if ( missionNum >= MAX_MISSIONS )
    {
        if (strcmp(missionName,"bm1") ==0)
        {
            if (GetCharacterSheetManager()->QueryBonusMissionCompleted(GetMissionManager()->GetCurrentLevelIndex()) == true)
            {
                Character* character = GetCharacterManager()->GetCharacterByName( NPCname );
                rAssert( character );
                character->SetRole(Character::ROLE_COMPLETED_BONUS);
                GetCharacterManager()->SetGarbage(character, true);
                return;
            }
        }

        int missionIndex = missionNum - MAX_MISSIONS;
        mBonusMissions[ missionIndex ].SetMissionNum( missionNum );

        Character* character = GetCharacterManager()->GetCharacterByName( NPCname );
        rAssert( character );

        mBonusMissions[ missionIndex ].SetNPC( character );

        #ifdef RAD_GAMECUBE
            HeapMgr()->PushHeap( GMA_GC_VMM );
        #else
            HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
        #endif
        //Set up the talk trigger around this guy.
        EventLocator* evtLoc = new EventLocator();

        rAssert( evtLoc );

        mBonusMissions[ missionIndex ].SetEventLocator( evtLoc );

        evtLoc->SetName( NPCname );
        evtLoc->SetEventType( LocatorEvent::GENERIC_BUTTON_HANDLER_EVENT );
    
        int id = -1;
        ActionButton::GenericEventButtonHandler* pABHandler = static_cast<ActionButton::GenericEventButtonHandler*>( ActionButton::GenericEventButtonHandler::NewAction( evtLoc, EVENT_BONUS_MISSION_DIALOGUE, HeapMgr()->GetCurrentHeap() ) ); 
        rAssert( dynamic_cast<ActionButton::GenericEventButtonHandler*> ( pABHandler ) != NULL ); 
        rAssert( pABHandler );

        pABHandler->SetEventData( evtLoc );

        bool bResult = GetActionButtonManager()->AddAction( pABHandler, id );
    
        rAssert( bResult );
    
        evtLoc->SetData( id );

        // Radius should equal about 1m.
        //
        const float r = 1.3f;
        rmt::Vector charPos;
        character->GetPosition( charPos );
        SphereTriggerVolume* pTriggerVolume = new SphereTriggerVolume( charPos, r );

        evtLoc->SetNumTriggers( 1, HeapMgr()->GetCurrentAllocator() );
    
        evtLoc->AddTriggerVolume( pTriggerVolume );
        pTriggerVolume->SetLocator( evtLoc );
        pTriggerVolume->SetName( "BONUS Trigger" );   

        //Set up the animated icon
        mBonusMissions[ missionIndex ].SetUpIcon( iconName, charPos );

        mBonusMissions[ missionIndex ].SetOneShot( isOneShot );

        mBonusMissions[ missionIndex ].SetDialogueName( dialogueName );

        #ifdef RAD_GAMECUBE
            HeapMgr()->PopHeap( GMA_GC_VMM );
        #else
            HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
        #endif

        if ( alternateIconName != NULL )
        {
            mBonusMissions[ missionIndex ].SetUpAlternateIcon( alternateIconName, charPos );
        }

        mBonusMissions[ missionIndex ].SetCompleted( wasCompletedAlready );

    }
MEMTRACK_POP_GROUP( "GamePlayManager::BonusMissionInfo" );
}




// Kill this!
//
/*
void GameplayManager::GetOverrideVehicleName( char* name )
{
    if( mVehicleIndex < 0 )
    {
        // ignore, using default vehicle specified in script
        return;
    }

    const char* VEHICLE_NAMES[] =
    {
        "apu_v",    // Vehicle 1
        "bart_v",   // Vehicle 2
        "cletu_v",  // Vehicle 3
        "comic_v",  // and so on ...
        "famil_v",
        "gramp_v",
        "homer_v",
        "marge_v",
        "skinn_v",
        "smith_v",
        "snake_v",
        "wiggu_v",
        "zombi_v",
        "cVan",
        "compactA",

        // new additions
        "frink_v",
        "lisa_v",
        "cCola"
    };

    if( mVehicleIndex >= 0 &&
        mVehicleIndex < static_cast<int>(sizeof( VEHICLE_NAMES ) / sizeof( VEHICLE_NAMES[ 0 ] )) )
    {
        strcpy( name, VEHICLE_NAMES[ mVehicleIndex ] );
    }
    else
    {
        rAssertMsg( 0, "WARNING: *** Vehicle not available!\n" );
    }
}
*/

// MS10: Hack to allow character selection
void GameplayManager::GetOverrideCharacterName( char* name )
{
    switch( mCharacterIndex )
    {
    case -1:
        {
            strcpy( name, "homer" );
            break;
        }
    case 0:
        {
            strcpy( name, "homer" );
            break;
        }
    case 1:
        {
            strcpy( name, "marge" );
            break;
        }
    case 2:
        {
            strcpy( name, "bart" );
            break;
        }
    case 3:
        {
            strcpy( name, "lisa" );
            break;
        }
    case 4:
        {
            strcpy( name, "apu" );
            break;
        }
    }
}



//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// GameplayManager::Initialize
//=============================================================================
// Description: 
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::Initialize()
{
    rAssertMsg( mNumMissions == 0, "The Gameplay Manager wasn't finalized"
        " when the game was stopped!" );

    mNumMissions = 0;
    mNumBonusMissions = 0;
    mLevelComplete = false;
    mGameComplete = false;
    mCurrentMission = -1;
    mCurrentBonusMission =  -1;
    mDesiredBonusMission = -1;
    mIsInBonusMission = false;
    mFireBonusMissionDialogue = false;
    //miNumLevelVehicles = 0;
    GetEventManager()->AddListener(this,EVENT_GETINTOVEHICLE_END);
    GetEventManager()->AddListener(this,EVENT_BONUS_MISSION_DIALOGUE);
    GetEventManager()->AddListener(this,EVENT_CONVERSATION_DONE_AND_FINISHED);
    GetEventManager()->AddListener(this,EVENT_REPAIR_CAR);
    GetEventManager()->AddListener(this,EVENT_VEHICLE_DESTROYED);
    //GetEventManager()->AddListener(this,EVENT_GETINTOTRAFFIC_END);
    GetEventManager()->AddListener(this,EVENT_GUI_MISSION_LOAD_COMPLETE);
    GetEventManager()->AddListener(this,EVENT_USER_CANCEL_MISSION_BRIEFING);
    GetEventManager()->AddListener(this,EVENT_USER_CANCEL_PAUSE_MENU);

    if ( mIsDemo )
    {
        GetEventManager()->AddListener( this, EVENT_WAYAI_HIT_LAST_WAYPOINT );
    }

    mDefaultLevelVehicleName[0] = '\0';
    mDefaultLevelVehicleLocator[0] = '\0';
    mDefaultLevelVehicleConfile[0] = '\0';    
    mShouldLoadDefaultVehicle = false;
    //Create the RespawnManager
    mpRespawnManager = new (GMA_LEVEL_OTHER) RespawnManager();
}

//=============================================================================
// GameplayManager::Finalize
//=============================================================================
// Description: 
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::Finalize()
{
    p3d::pddi->DrawSync();
    //clear any cars that we are holding on to.
    RemoveLevelVehicleController();
    SetCurrentMission( -1 );

    int i;
    for( i = 0; i < MAX_MISSIONS + MAX_BONUS_MISSIONS; i++)
    {
        if ( mMissions[ i ] != NULL )
        {
            delete mMissions[ i ];
        }

        mMissions[ i ] = NULL;
    }

    for ( i = 0; i < mNumBonusMissions; ++i )
    {
        mBonusMissions[i].CleanUp();
    }

    mNumMissions = 0;
    mNumBonusMissions = 0;
    mUpdateBonusMissions = true;
    mJumpToBonusMission = false;


    EmptyMissionVehicleSlots();
    spInstance->MDKVDU();


    //miNumLevelVehicles = 0;
    SetCurrentVehicle( NULL );

    ClearVehicleSlot(eDefaultCar);
    ClearVehicleSlot(eOtherCar);
    ClearVehicleSlot(eAICar);

	KillAllChaseManagers();

    //Kill RespawnManager
    delete mpRespawnManager;
    mpRespawnManager = NULL;



    p3d::inventory->RemoveSectionElements("Mission");
    p3d::inventory->RemoveSectionElements("Level");
    p3d::inventory->DeleteSection("Mission");
    p3d::inventory->DeleteSection("Level");
    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// GameplayManager::SetCurrentMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      inline 
//
//=============================================================================
void GameplayManager::SetCurrentMission( int index )
{
    //Switching to a new mission, so clean up the old one.
    Mission* oldMission = GetCurrentMission();

    //Set to new mission.
    if ( mIsDemo && index >= mNumMissions )
    {
        //Reselect the mission in the range of mNumMissions
        mCurrentMission = rand() % mNumMissions;
        mIsInBonusMission = false;
    }
    else if ( index >= mNumMissions )
    {
        //This better be a bonus mission.
        rAssert( index < MAX_MISSIONS + MAX_BONUS_MISSIONS);
        mCurrentBonusMission = index;
        mIsInBonusMission = true;
    }
    else
    {
        mCurrentMission = index;
        mIsInBonusMission = false;
    }

    Mission* newMission = GetCurrentMission();

    if( oldMission != NULL )
    {
        // stop mission-related HUD event handlers
        //
        CGuiScreenHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            currentHud->GetEventHandler( CGuiScreenHud::HUD_EVENT_HANDLER_MISSION_OBJECTIVE )->Stop();
            currentHud->GetEventHandler( CGuiScreenHud::HUD_EVENT_HANDLER_MISSION_OBJECTIVE )->Stop();
        }

        oldMission->Finalize();
        CleanMissionData();
    }

    if( newMission != NULL )
    {
                   
        if ( mIsInBonusMission )
        {
            mCurrentMissionHeap = GetMissionHeap( mCurrentBonusMission );
        }
        else
        {
            mCurrentMissionHeap = GetMissionHeap( mCurrentMission );
        }

        LoadMission();

       

        
        if ( mShouldLoadDefaultVehicle )
        {
            if ( newMission->IsForcedCar() )
            {
                //We're loading a different car, so ignore.  TRUCK, I HATE THIS HACK
                mShouldLoadDefaultVehicle = false;
                mPutPlayerInCar = false;
            }
            else
            {
                //Create the filename
                char filename[128];
                sprintf( filename, "art\\cars\\%s.p3d", mDefaultLevelVehicleName );
                char* argv[] = { "", filename, mDefaultLevelVehicleName, "DEFAULT" };
                MissionScriptLoader::LoadDisposableCar( 4, argv );
            }
        }
        //update CharacterSheet with current mission
        GetCharacterSheetManager()->SetCurrentMission(spInstance->GetCurrentLevelIndex(),
                                                      static_cast<RenderEnums::MissionEnum>( spInstance->GetCurrentMissionIndex() ) );

    }
}

//=============================================================================
// GameplayManager::NextMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::NextMission()
{
    mCurrentMessage = NEXT_MISSION;
}

//=============================================================================
// GameplayManager::PrevMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::PrevMission()
{
    mCurrentMessage = PREV_MISSION;
}

//=============================================================================
// GameplayManager::DoNextMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::DoNextMission()
{
    if((mLevelComplete == true) || (mGameComplete == true))
    {
        return;
    }
        
    int newMission = mCurrentMission;

    if ( mIsInBonusMission )
    {
        //Return to the current mission.  (Should be sunday drive mode)
        //Do nothing.
    }
    else if ( (mCurrentMission + 1) >= mNumMissions )
    {
        if( strlen( mPostLevelFMV ) > 0 )
        {
           /*
            GetPresentationManager()->PlayFMV( mPostLevelFMV, this );
            mPostLevelFMV[ 0 ] = 0;
            mWaitingOnFMV = true;
            GetGameFlow()->SetQuickStartLoading( true );
            GetRenderManager()->FreezeAllLayers();
            return;
           */
        }
        //Loop
        //TODO: ???
        if( !mWaitingOnFMV )
        {
            mLevelComplete = true;
            if( GetCurrentLevelIndex() == RenderEnums::numLevels - 1 )
            {
                mGameComplete = true;
            }
            GetGameFlow()->SetContext( CONTEXT_PAUSE );        
        }

        //Hmmmm.
        return;
    }
    else
    {
        //Next
        if((mLevelComplete != true) && (mGameComplete != true))
        {
            newMission = mCurrentMission + 1;
        }
    }

    if ( GetCurrentMission()->IsForcedCar() && (GetCurrentMission()->GetSwappedCarsFlag() != true ) )
    {
        mShouldLoadDefaultVehicle = true;
    }
    SetCurrentMission( newMission );
}

//=============================================================================
// GameplayManager::DoPrevMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::DoPrevMission()
{
    int newMission = mCurrentMission;

    if ( mIsInBonusMission )
    {
        //Return to the current mission.  (Should be sunday drive mode)
        //mIsInBonusMission = false;
    }
    else if ( (mCurrentMission - 1) < 0 || (mCurrentMission - 1) >= mNumMissions )
    {
        //Loop
        newMission = 0;
    }
    else
    {
        //Next
        newMission = mCurrentMission - 1;
    }

    SetCurrentMission( newMission );
}

//=============================================================================
// GameplayManager::GetCurrentMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Mission
//
//=============================================================================
Mission* GameplayManager::GetCurrentMission()
{
    if ( mIsInBonusMission )
    {
        if ( mCurrentBonusMission >= MAX_MISSIONS && mCurrentBonusMission < MAX_MISSIONS + MAX_BONUS_MISSIONS )
        {
            return ( GetMission( mCurrentBonusMission ) );
        }
    }
    else
    {
        if(( mCurrentMission >= 0 ) && ( mCurrentMission < (int)mNumMissions ))
        {
            return( GetMission( mCurrentMission ) );
        }
    }

    return( NULL );
}

//=============================================================================
// GameplayManager::GetCurrentBonusMissionInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      BonusMissionInfo
//
//=============================================================================
const BonusMissionInfo* GameplayManager::GetCurrentBonusMissionInfo() const
{
    if( mIsInBonusMission )
    {
        int bonusMissionInfoIndex = mCurrentBonusMission - MAX_MISSIONS;
        rAssert( bonusMissionInfoIndex >= 0 && bonusMissionInfoIndex < MAX_BONUS_MISSIONS );

        return &(mBonusMissions[ bonusMissionInfoIndex ]);
    }

    return NULL;
}

//=============================================================================
// GameplayManager::GetBonusMissionInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ( int missionNumber )
//
// Return:      BonusMissionInfo
//
//=============================================================================
BonusMissionInfo* GameplayManager::GetBonusMissionInfo( int missionNumber )
{
    if ( missionNumber < MAX_BONUS_MISSIONS )
    {
        return &mBonusMissions[ missionNumber ];
    }

    return NULL;
}


//=============================================================================
// GameplayManager::GetMissionHeap
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      GameMemoryAllocator 
//
//=============================================================================
GameMemoryAllocator GameplayManager::GetMissionHeap( int index )
{
#ifdef RAD_GAMECUBE
    return GMA_GC_VMM;
#else
    return( GMA_LEVEL_MISSION );
#endif
}

//=============================================================================
// GameplayManager::GetCurrentMissionNum
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
int GameplayManager::GetCurrentMissionNum() const
{
    if ( mIsInBonusMission )
    {
        return mCurrentBonusMission;
    }
    else
    {
        return mCurrentMission;
    }
}

//=============================================================================
// GameplayManager::RestartCurrentMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::RestartCurrentMission()
{
    
}

//=============================================================================
// GameplayManager::RestartToMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( RenderEnums::MissionEnum mission )
//
// Return:      void 
//
//=============================================================================
void GameplayManager::RestartToMission( RenderEnums::MissionEnum mission )
{

}


//=============================================================================
// GameplayManager::AbortCurrentMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::AbortCurrentMission()
{

}

//=============================================================================
// GameplayManager::IsBonusMissionDesired
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
bool GameplayManager::IsBonusMissionDesired() const
{
    return ( mDesiredBonusMission != -1 );
}

//=============================================================================
// GameplayManager::CancelBonusMission
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::CancelBonusMission()
{
    mBonusMissions[ mDesiredBonusMission - MAX_MISSIONS ].ResetMissionBitmap();
    mDesiredBonusMission = -1;
}

//=============================================================================
// GameplayManager::EnabledPhonesBooths
//=============================================================================
// Description: Sets mEnablePhoneBooths flag to true
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::EnablePhoneBooths()
{
	mEnablePhoneBooths = true;
}



//=============================================================================
// GameplayManager::DisablePhonesBooths
//=============================================================================
// Description: Sets mEnablePhoneBooths flag to false
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::DisablePhoneBooths()
{
	mEnablePhoneBooths = false;
}

//=============================================================================
// GameplayManager::QueryPhoneBoothsEnabled
//=============================================================================
// Description: returns  mEnablePhoneBooths flag to true
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
bool GameplayManager::QueryPhoneBoothsEnabled()
{
	return mEnablePhoneBooths;
}


//creates a chasemanager for the hostvehicle, returns 0 if good -1 if it cant
//create a chasemanager
int GameplayManager::CreateChaseManager(char* hostvehiclename,char* confile,int spawnrate)
{	
	for (int i=0;i<MAX_CHASEMANAGERS;i++)
	{
		if (strcmp(m_ChaseManager_Array[i].hostvehicle,"NULL") == 0 && m_ChaseManager_Array[i].mp_chasemanager==NULL)
		{
			//found an empty slot make the chasemanager and return 0
			strcpy(m_ChaseManager_Array[i].hostvehicle,hostvehiclename);
			m_ChaseManager_Array[i].mp_chasemanager=new (GMA_LEVEL_OTHER) ChaseManager();
			
			m_ChaseManager_Array[i].mp_chasemanager->RegisterModel(hostvehiclename,spawnrate);
			m_ChaseManager_Array[i].mp_chasemanager->SetConfileName( confile);
			m_ChaseManager_Array[i].mp_chasemanager->Init();
			m_ChaseManager_Array[i].mp_chasemanager->SetMaxObjects(0);
            m_ChaseManager_Array[i].mp_chasemanager->SetActive(false);

			
			return 0;
		}
	}

	//default return if no empty slots
	return -1;
}


//returns a ptr to the correct chasemanager for a host vehicle or NULL if failure
ChaseManager* GameplayManager::GetChaseManager(char* hostvehiclename)
{
	for(int i =0; i<MAX_CHASEMANAGERS;i++)
	{
		if (strcmp(m_ChaseManager_Array[i].hostvehicle,hostvehiclename) == 0)
		{
			return m_ChaseManager_Array[i].mp_chasemanager;
		}
	}
	return NULL;
}


// overloaded function , GetChaseManager by index.
ChaseManager* GameplayManager::GetChaseManager(int index)
{
    if ((index >=0) && (index <= MAX_CHASEMANAGERS))
    {
        return m_ChaseManager_Array[index].mp_chasemanager ;
    }

    else 
    {
        return NULL;
    }
}




//=============================================================================
// GameplayManager::GetMissionNumByName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      int 
//
//=============================================================================
int GameplayManager::GetMissionNumByName( const char* name )
{
    int i;
    for ( i = 0; i < MAX_MISSIONS + mNumBonusMissions; ++i )
    {
        if ( mMissions[i] && strcmp( mMissions[i]->GetName(), name ) == 0 )
        {
            return i;
        }
    }

    rAssertMsg( false, "Trying to access non-existant mission...  Poo.\n" );
    return -1;
}

//=============================================================================
// GameplayManager::GetBonusMissionNumByName
//=============================================================================
// Description: bonus missions are enumerated differently
//
// Parameters:  ( const char* name )
//
// Return:      int 
//
//=============================================================================
int GameplayManager::GetBonusMissionNumByName( const char* name )
{
    int missionNum = GetMissionNumByName( name );
    return missionNum - MAX_MISSIONS;
}

//Kills all chasemanagers, call this when level is over
void GameplayManager::KillAllChaseManagers()
{
	//kill kill kill
	for (int i =0;i<MAX_CHASEMANAGERS;i++)
	{
		strcpy(m_ChaseManager_Array[i].hostvehicle,"NULL");
		delete m_ChaseManager_Array[i].mp_chasemanager;
		m_ChaseManager_Array[i].mp_chasemanager=NULL;
	}
}

//=============================================================================
// GameplayManager::EnableBonusMissions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::EnableBonusMissions()
{
    mUpdateBonusMissions = true;

    int i;
    for ( i = 0; i < mNumBonusMissions; ++i )
    {
        mBonusMissions[ i ].Enable();
    }
}

//=============================================================================
// GameplayManager::DisableBonusMissions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayManager::DisableBonusMissions()
{
    mUpdateBonusMissions = false;

    int i;
    for ( i = 0; i < mNumBonusMissions; ++i )
    {
        mBonusMissions[ i ].Disable();
    }
}


//clears all Chase Cars from the Screen
void GameplayManager::ClearAllChaseCars ()
{
	for (int i =0;i< MAX_CHASE_STRUCTS;i++)
	{
		if (m_ChaseManager_Array[i].mp_chasemanager != NULL)
		{
			m_ChaseManager_Array[i].mp_chasemanager->ClearAllObjects();
		}
	}
}

//disable all harrass cars
void GameplayManager::DisableAllChaseAI()
{
    for (int i =0;i< MAX_CHASE_STRUCTS;i++)
	{
		if (m_ChaseManager_Array[i].mp_chasemanager != NULL)
		{
			m_ChaseManager_Array[i].mp_chasemanager->DisableAllActiveVehicleAIs();
		}
	}
}

//enable all harass cars
void GameplayManager::EnableAllChaseAI()
{
    for (int i =0;i< MAX_CHASE_STRUCTS;i++)
	{
		if (m_ChaseManager_Array[i].mp_chasemanager != NULL)
		{
			m_ChaseManager_Array[i].mp_chasemanager->EnableAllActiveVehicleAIs();
		}
	}
}

GameplayManager::GameTypeEnum GameplayManager::GetGameType()
{
    return mGameType;
}

tColour GameplayManager::GetControllerColour( int id )
{
    tColour cColour;
    switch( id )
    {
    case 0:
        cColour.Set( 255, 2, 2 ); // red
        break;
    case 1:
        cColour.Set( 36, 232, 255 ); //blue
        break;
    case 2:
        cColour.Set( 246, 233, 5 ); // yellow
        break;
    case 3:
        cColour.Set( 33, 209, 14 ); // green
        break;
    case 4:
        cColour.Set( 0, 255, 0 ); // all green
        break;
    default:
        cColour.Set( 0, 0, 0 ); // black
        break;
    }
    return cColour;
}


RespawnManager* GameplayManager::GetRespawnManager()
{
    return mpRespawnManager;
}




void GameplayManager::SetPostLevelFMV( const char* FileName )
{
    if( ( FileName == 0 ) || ( strlen( FileName ) == 0 ) )
    {
        mPostLevelFMV[ 0 ] = 0;
    }
    else
    {
        strcpy( mPostLevelFMV, FileName );
    }
}

int GameplayManager::AddToVDU(Vehicle* pvehicle)
{
    if (mVDU.mCounter < (MAX_VDU_CARS -1))
    {
        //we have room add it
        
        //check if its already in our list we dont want to re-add it
        for(int i =0;i<MAX_VDU_CARS;i++)
        {
            if(mVDU.mpVehicle[i] == pvehicle)
            {
                //already in the list return
                return 0;
            }        
        }


        //if the car isnt in our list then find room for it.
        for(int i =0;i<MAX_VDU_CARS;i++)
        {
            if(mVDU.mpVehicle[i] == NULL)
            {
                mVDU.mpVehicle[i]=pvehicle;
                mVDU.mpVehicle[i]->AddRef();
                mVDU.mCounter++;
                return 0;
            }
        
        }

        //searched the array no room trouble tell Chuck.
        rTuneAssert(0);
        return 0;
    }
    else
    {
        rTuneAssert(0);
        return 1;
            
    }
}


int  GameplayManager::UpdateVDU()
{
    if (mVDU.mCounter==0)
    {
        return 0;
    }
    else
    {
        rmt::Vector CharacterPositionStart;
        GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter()->GetPosition(CharacterPositionStart);

        for(int i=0;i<MAX_VDU_CARS;i++)
        {
            if (mVDU.mpVehicle[i] != NULL)
            {
                rmt::Vector position;
                rmt::Vector CarPositionDifference;

                mVDU.mpVehicle[i]->GetPosition(&position);
                mVDU.mpVehicle[i]->GetPosition(&CarPositionDifference);
                
                CarPositionDifference.Sub(CharacterPositionStart);

                //dont care about the y vaule since we are going to just do a top down radial check
                CarPositionDifference.y =0;                
                //chuck: remove the car if it is out of the players view and greater than 5.0 meters from player
                if ((spInstance->TestPosInFrustrumOfPlayer(position,0) == false) && (rmt::Sqrt(CarPositionDifference.MagnitudeSqr()) > 5.0f))
                {
                    GetVehicleCentral()->RemoveVehicleFromActiveList(mVDU.mpVehicle[i]);

                    if( mVDU.mpVehicle[i]->mVehicleID == VehicleEnum::HUSKA )
                    {
                        GetVehicleCentral()->mHuskPool.FreeHusk(mVDU.mpVehicle[i]);
                    }
                    mVDU.mpVehicle[i]->Release();
                    mVDU.mpVehicle[i]=NULL;
                    mVDU.mCounter--;
                }
            }
        }
    }
    return 0;
}
    
int GameplayManager::MDKVDU()
{
    for(int i=0;i<MAX_VDU_CARS;i++)
    {
        if (mVDU.mpVehicle[i] != NULL)
        {
            GetVehicleCentral()->RemoveVehicleFromActiveList(mVDU.mpVehicle[i]);
            if( mVDU.mpVehicle[i]->mVehicleID == VehicleEnum::HUSKA )
            {
                GetVehicleCentral()->mHuskPool.FreeHusk(mVDU.mpVehicle[i]);
            }
            mVDU.mpVehicle[i]->Release();
            mVDU.mpVehicle[i]=NULL;
            mVDU.mCounter--;
        }
    }
    return 0;
}

void GameplayManager::ReleaseFromVDU(char* carname, Vehicle** outVehicle)
{
     bool found = false;     
    
     for (int i=0;i<MAX_VDU_CARS;i++)
    {
        if(mVDU.mpVehicle[i] != NULL)
        {
            if (strcmp(mVDU.mpVehicle[i]->GetName(),carname)== 0)
            {
                //check for cars that have the same name
                if (found == true)
                {
                    //problems there were are 2 or more cars in this list with the same name can't decide which one to return;
                    rAssert(0);
                }
                else
                {   

                    if(outVehicle)
                    {
                        tRefCounted::Assign(*outVehicle, mVDU.mpVehicle[i]);
                    }
                    mVDU.mCounter--;
                    mVDU.mpVehicle[i]->Release(); //undo our add ref.
                    found = true;
                    mVDU.mpVehicle[i]=NULL;
                }
            }//end if matching name loop

        }//end if NULL check
    }//end of for loop
}




//=============================================================================
// GameplayManager::TestForContinuityErrorWithCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* v, bool aiSlot )
//
// Return:      bool 
//
//=============================================================================
bool GameplayManager::TestForContinuityErrorWithCar( Vehicle* v, bool aiSlot )
{
    bool continuityError = false;

    Vehicle* vehicle = NULL;
    if ( aiSlot )
    {
        vehicle = GetVehicleInSlot( eAICar );
    }
    else
    {
        vehicle = GetCurrentVehicle();
    }

    unsigned int i;
    for ( i = 0; vehicle && i < NUM_CONTINUITY_ERRORS; ++i )
    {
        if ( GetCurrentLevelIndex() != CONTINUITY_ERRORS[ i ].mLevel ||
             GetCurrentMissionIndex() != CONTINUITY_ERRORS[ i ].mMission )
        {
            continue;
        }

        if ( strcmp( CONTINUITY_ERRORS[ i ].mPlayerCarName, v->mName ) == 0 )
        {
            if ( strcmp( vehicle->mName, CONTINUITY_ERRORS[ i ].mOtherCarName ) == 0 )
            {
                continuityError = true;
                break;
            }
        }
    }

    return continuityError;
}

int GameplayManager::RemoveVehicleFromMissionVehicleSlots(Vehicle* pVehicle)
{
    for (int i=0;i< MAX_MISSION_VEHICLE_SLOTS; i++)
    {
        //match the vehicle to be removed
        if (mMissionVehicleSlots[i].vehicle == pVehicle)
        {
            if(mMissionVehicleSlots[i].usingHusk)
            {               
                spInstance->AddToVDU(mMissionVehicleSlots[i].pHuskVehicle);              
                mMissionVehicleSlots[i].pHuskVehicle->Release();
                mMissionVehicleSlots[i].pHuskVehicle = NULL;
                mMissionVehicleSlots[i].usingHusk = false;
            }
            else
            {
                spInstance->AddToVDU(mMissionVehicleSlots[i].vehicle);
            }
        
            GetEventManager()->TriggerEvent(EVENT_MISSION_VEHICLE_RELEASED,mMissionVehicleSlots[i].vehicle);
            
            //update the AI vehicle slot so It points to the correct vehicle.
            if(strcmp(GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eAICar),"NULL") != 0 && GetGameplayManager()->mVehicleSlots[GameplayManager::eAICar].mp_vehicle != NULL )
            {
                //update the Gameplaymanagers AICar slot mp_vehicle ptr.
                if(strcmp(mMissionVehicleSlots[i].vehicle->GetName(),GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eAICar)) ==0)
                {
                    GetGameplayManager()->mVehicleSlots[GameplayManager::eAICar].mp_vehicle = NULL;
                }
            }
            
            mMissionVehicleSlots[i].vehicle->Release();           
            mMissionVehicleSlots[i].vehicle = 0;
            mMissionVehicleSlots[i].name[0] = 0;
            return 0;
        }//end of if loop

    }//end of for loop
    return -1;
}

void GameplayManager::AbortFade()
{
    mFadedToBlack = false;
    mBlackScreenTimer = 0;

    //
    // Go to the HUD screen and turn off the bloody fade
    //
    CGuiScreenHud* hud = GetCurrentHud();
    hud->AbortFade();

}


//******************************************************************************
//
// E3-Specific Member Functions
//
//******************************************************************************

#ifdef RAD_DEMO

void
GameplayManager::ResetIdleTime()
{
    m_elapsedIdleTime = 0;
}

void
GameplayManager::UpdateIdleTime( unsigned int elapsedTime )
{
    m_elapsedIdleTime += elapsedTime;

    const unsigned int MAX_IDLE_TIME = 3 * 60000; // in msec
    if( m_elapsedIdleTime > MAX_IDLE_TIME )
    {
        mLevelComplete = true; // this is how we quit out of gameplay automatically

        // make sure we don't try to switch context when another
        // switch is already pending
        //
        if( GetGameFlow()->GetCurrentContext() == GetGameFlow()->GetNextContext() )
        {
            GetGameFlow()->SetContext( CONTEXT_PAUSE );
        }
    }
}

#endif // RAD_DEMO
