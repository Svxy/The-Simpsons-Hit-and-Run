//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        mission.cpp
//
// Description: Implement Mission
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radtime.hpp>

//========================================
// Project Includes
//========================================
#include <mission/gameplaymanager.h>
#include <mission/mission.h>
#include <mission/missionstage.h>
#include <mission/conditions/missioncondition.h>
#include <mission/objectives/missionobjective.h>
#include <mission/objectives/dialogueobjective.h>
#include <mission/bonusobjective.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/harass/chasemanager.h>

#include <worldsim/parkedcars/parkedcarmanager.h>

#include <events/eventmanager.h>

#include <meta/zoneeventlocator.h>
#include <meta/carstartlocator.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <memory/srrmemory.h>

#include <debug/profiler.h>

#include <contexts/context.h>
#include <gameflow/gameflow.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/boss.h>
#include <mission/ufo/tractorbeam.h>
#include <mission/statepropcollectible.h>
#include <mission/animatedicon.h>

#include <roads/roadmanager.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/intersection.h>

#include <interiors/interiormanager.h>
#include <stateprop/statepropdata.hpp>

#include <atc/atcmanager.h>

#include <ai/actionbuttonhandler.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

const float MS_PER_SEC = 1000.0f;
const float SEC_IN_MSEC = 0.001f;
const int FINAL_DELAY = 3000;  //How long to delay changing stages when this one is marked "final"
const int COMPLETE_DELAY = 0;  //How long to delay changing stages when this one shows COMPLETE
const int MAX_NUM_STATEPROP_COLLECTIBLES = 10;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Mission::Mission
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
Mission::Mission() :
    mIsStreetRace1Or2( false ),
    mNumMissionStages( 0 ),
    mCurrentStage( -1 ),
#ifdef RAD_GAMECUBE
    mHeap( GMA_GC_VMM ),
#else
    mHeap( GMA_LEVEL_MISSION ),
#endif
    mbComplete( false ),
    mbIsLastStage( false ),
    mMissionTimer( 0 ),
    mState( STATE_WAITING ),
    mLastStageState( MissionStage::STAGE_IDLE ),
    //mNumVehicles( 0 ),
    mVehicleRestart( NULL ),
    mPlayerRestart( NULL ),
    mDynaloadLoc( NULL ),
    mStreetRacePropsLoad(NULL),
    mStreetRacePropsUnload(NULL),    
    mResetToStage( 0 ),
    mSundayDrive( false ),
    mBonusMisison( false ),
    mNumBonusObjectives( 0 ),
    mIsForcedCar( false ),
    mbAutoRepairCar(false),
    mbSwappedCars(false),
    mbTriggerPattyAndSelmaScreen(false),
    mFinalDelay( -1 ),
    mCompleteDelay( -1 ),
    mChangingStages( false ),
    mNoTimeUpdate(false ),
    mJumpBackStage( false ),
    mJumpBackBy( 1 ),
    mNumStatePropCollectibles( 0 ),
    mStatePropCollectibles( NULL ),
    mDoorStars( NULL ),
    mInitPedGroupId( 0 ),
    mShowHUD( true ),
    mNumValidFailureHints( -1 )
{
    strcpy( mcName, "" );
    mbCarryOverOutOfCarCondition = false;

    unsigned int i;
    for ( i = 0; i < MAX_BONUS_OBJECTIVES; ++i )
    {
        mBonusObjectives[ i ] = NULL;
    }
    mElapsedTimems = 0;
    for (int j =0; j<MAX_STAGES;j++)
    {
        mMissionStages[j]=NULL;
    }
}

//==============================================================================
// Mission::~Mission
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
Mission::~Mission()
{
    /*
    for( i = 0; i < mNumVehicles; i++ )
    {
        mVehicles[ i ]->Release();
        mVehicles[ i ] = NULL;
    }
    */
    
    if ( mDynaloadLoc )
    {
        mDynaloadLoc->Release();
    }

    unsigned int k;
    for ( k = 0; k < mNumBonusObjectives; ++k )
    {
        delete mBonusObjectives[ k ];
        mBonusObjectives[ k ] = NULL;
    }
    mNumBonusObjectives = 0;

    if ( mStreetRacePropsLoad )
    {
        mStreetRacePropsLoad->ReleaseVerified();
    }

    
    if ( mStreetRacePropsUnload )
    {
        mStreetRacePropsUnload->ReleaseVerified();
    }

    // Release all statepropcollectibles and remove them from the world
    for ( int i = 0 ; i < mNumStatePropCollectibles ; i++ )
    {
        StatePropCollectible* collectible = mStatePropCollectibles[i];
        if ( collectible )
        {
            collectible->RemoveFromDSG();
            collectible->Release();
        }
    }
    delete [] mStatePropCollectibles;
    mStatePropCollectibles = NULL;
    mNumStatePropCollectibles = 0;

    if ( mDoorStars )
    {
        delete mDoorStars;
    }
}

//=============================================================================
// Mission::AddPlayerVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle )
//
// Return:      void 
//
//=============================================================================
/*
void Mission::AddVehicle( Vehicle* vehicle )
{
    rAssert( vehicle != NULL );

    int i;
    for ( i = 0; i < mNumVehicles; ++i )
    {
        if ( mVehicles[ i ] == vehicle )
        { 
            //We've already got this one!
            return;
        }
    }

    rAssert( mNumVehicles < MAX_VEHICLES );
    mVehicles[ mNumVehicles ] = vehicle;
    vehicle->AddRef();

    mNumVehicles++;
}
*/

//=============================================================================
// Mission::GetVehicleByName
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* name )
//
// Return:      Vehicle
//
//=============================================================================
/*
Vehicle* Mission::GetVehicleByName( char* name )
{
    Vehicle* vehicle = NULL;

    for( int i = 0; i < mNumVehicles; i++ )
    {
        if( strcmp( mVehicles[ i ]->GetName(), name ) == 0 )
        {
            vehicle = mVehicles[ i ];
            break;
        }
    }
    
    // TODOGREG - is this what we want to do here?
    // if mission doesn't have one of these, do we really want to return one from vehicle central??????????
    if ( !vehicle )
    {
        vehicle = GetVehicleCentral()->GetVehicleByName( name );
    }

    return vehicle;
}
*/

//=============================================================================
// Mission::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Mission::Initialize( GameMemoryAllocator heap)
{
    mHeap = heap;

    unsigned int i;
    for ( i = 0; i < mNumBonusObjectives; ++i )
    {
        mBonusObjectives[ i ]->Initialize();
    }

    mbComplete = false;
    mbIsLastStage = false;
    mJumpBackStage = false;
    mJumpBackBy = 1;

    SetToStage( -1 );
    
    mState = STATE_WAITING;


    //if missions are forced cars turn off phone booths
    if(mIsForcedCar == true)
    {
        GetGameplayManager()->DisablePhoneBooths();
    }

    if(mbAutoRepairCar == true)
    {
        //GetGameplayManager()->GetCurrentVehicle()->ResetDamageState();
        if (GetGameplayManager()->GetCurrentVehicle() != NULL)
        {
            if (GetGameplayManager()->GetCurrentVehicle()->IsVehicleDestroyed() == true)
            {
                GetEventManager()->TriggerEvent(EVENT_REPAIR_CAR);
                
            }
        }
    }
    else
    {
        if (IsSundayDrive() == true ||IsWagerMission()== true  )
        {
            
        }
        else
        {
            mbAutoRepairCar = true;
        }
    }

    //Purchase rewards only in sunday drive.
    ActionButton::PurchaseReward::SetEnabled( mSundayDrive );

    GetEventManager()->AddListener(this, EVENT_ENTER_INTERIOR_TRANSITION_START);
    GetEventManager()->AddListener(this, EVENT_ENTER_INTERIOR_END);
    GetEventManager()->AddListener(this, EVENT_EXIT_INTERIOR_START);
    GetEventManager()->AddListener(this, EVENT_EXIT_INTERIOR_END);
    //chuck: Adding this to handle the patty and selma done talking event. since this class will now trigger screen.
    GetEventManager()->AddListener(this, EVENT_GUI_TRIGGER_PATTY_AND_SELMA_SCREEN);

    //Try to setup the doorstars if this is sunday drive
    rAssert( mDoorStars == NULL );
    if ( mSundayDrive )
    {
        int level = GetGameplayManager()->GetCurrentLevelIndex() + 1;
        if ( level == 5 )
        {
            level = 2;
        }
        else if ( level == 6 )
        {
            level = 3;
        }
        else if ( level == 7 )
        {
            level = 4;
        }
        
        char name[ 64 ];
        sprintf( name, "l%d_doorstars", level );
        mDoorStars = new AnimatedIcon();
        mDoorStars->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
    }

    if( strncmp( mcName, "sr1", 3 ) == 0 ||
        strncmp( mcName, "sr2", 3 ) == 0 ) 
    {
        mIsStreetRace1Or2 = true;
    }
    else
    {
        mIsStreetRace1Or2 = false;
    }

}

//=============================================================================
// Mission::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void Mission::Finalize()
{
    SetToStage( -1 );

    //chuck adding draw synch I hope to fix the level 5 m1 crash when user aborts missions.
    p3d::pddi->DrawSync();

    //Stop harrass cars from spawning
    GameplayManager* gameplayManager = GetGameplayManager();
    if ( gameplayManager != NULL )
    {
        ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
        if ( chaseManager != NULL )
        {
            chaseManager->ClearAllObjects();
            chaseManager->SetMaxObjects(0);
        }
        gameplayManager->EmptyMissionVehicleSlots();
    }    

    

    int i;
    for ( i = 0; i < mNumMissionStages; i++ )
    {
        delete mMissionStages[ i ];
        mMissionStages[ i ] = NULL;
    }

    // Remove all collectibles from the vehicles
    GetVehicleCentral()->DetachAllCollectibles();


    // Release all statepropcollectibles and remove them from the world
    for ( int i = 0 ; i < mNumStatePropCollectibles ; i++ )
    {
        StatePropCollectible* collectible = mStatePropCollectibles[i];
        if ( collectible )
        {
            collectible->RemoveFromDSG();
            collectible->Release();
            p3d::inventory->Remove( collectible );
            collectible = NULL;
        }
    }
    mNumStatePropCollectibles = 0;



    /*
    for ( j = 0; j < mNumVehicles; ++j )
    {
        //bool succeeded = GetVehicleCentral()->RemoveVehicleFromActiveList( mVehicles[ j ] );
//        rAssert( succeeded );
        mVehicles[ j ]->Release();
        mVehicles[ j ] = NULL;
    }

    mNumVehicles = 0;
    */

    mNumMissionStages = 0;
    
    mbComplete = false;
    mbIsLastStage = false;

    unsigned int k;
    for ( k = 0; k < mNumBonusObjectives; ++k )
    {
        mBonusObjectives[ k ]->Finalize();
        delete mBonusObjectives[ k ];
    }

    mNumBonusObjectives = 0;

    //This is a safety catch
    GetEventManager()->RemoveAll( this );

    //
    // TODO: Dump all the mission vehicles (player & AI)
    //

    //Dump the car if this is a forced car mission
    if ( mIsForcedCar )
    {
        //re-enable phone booths
        GetGameplayManager()->EnablePhoneBooths();


        Character* player = GetAvatarManager()->GetAvatarForPlayer( 0 )->GetCharacter();

        Vehicle* forcedCar = GetGameplayManager()->GetVehicleInSlot(GameplayManager::eOtherCar);
		Vehicle* currCar = GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle();
		if(currCar && currCar->mVehicleID == VehicleEnum::HUSKA)
		{
			currCar = GetVehicleCentral()->mHuskPool.FindOriginalVehicleGivenHusk(currCar);
		}
        if ( forcedCar != NULL && currCar == forcedCar && (mbSwappedCars == false) )
        {
           
            //locking forced car since we are done with it. 
            forcedCar->TransitToAI();
            //The character is in the forced car
            GetAvatarManager()->PutCharacterOnGround( player, forcedCar );
           
            //Move the character somewhere reasonable
            rmt::Vector pos;
            player->GetPosition( &pos );
            float rotation = player->GetFacingDir();

           
            const Road* road;
            RoadSegment* roadSeg;
            int segIndex;
            float in;
            float lateral;

            if ( RoadManager::GetInstance()->FindRoad( pos, &road, &roadSeg, segIndex, in, lateral, true ) )
            {
                //We're on a road set our position to the side of the road.
                roadSeg->GetPosition( 0.5f, 0.5f, &pos );
            }
            else
            {
                //Am I in an intersection?
                Intersection* intersection = RoadManager::GetInstance()->FindIntersection( pos );
                if ( intersection != NULL )
                {
                    //Take the first road going out of the intersection and 
                    //put the player on the corner.
                    const Road* road = intersection->GetRoadOut( 0 );

                    if(!road)
                    {
                        road = intersection->GetRoadIn( 0 );
                    }

                    if(road)
                    {
                        roadSeg = road->GetRoadSegment( 0 );
                        roadSeg->GetPosition( 0.5f, 0.5f, &pos );
                    }
                }
                else
                {
                    // we're not on a road or on an intersection. get the "closeset" road
                    float distance;
                    RoadSegment* segment;
                    GetIntersectManager()->FindClosestRoad(
                        pos, 
                        50.0f,  //yay the magic numbers i just made this one up
                        segment, 
                        distance 
                    );
                    rAssert( segment != NULL );
                    segment->GetPosition( &pos );
                }
            }

            rmt::Vector CharacterPositionVsCar;

            rmt::Vector CharacterPositionVsCharacterStartPosition;

            player->GetPosition(CharacterPositionVsCar);

            player->GetPosition(CharacterPositionVsCharacterStartPosition);

            //subtract the character position from the forced car start locator
            CharacterPositionVsCar.Sub(GetGameplayManager()->mPlayerAndCarInfo.mForceLocation);

            //subtract the character's position from the original start position.

            CharacterPositionVsCharacterStartPosition.Sub(GetGameplayManager()->mPlayerAndCarInfo.mPlayerPosition);
           


            float vaule1,vaule2;

            //distance between character and the forced cars respawn locator
            vaule1 = rmt::Sqrt(CharacterPositionVsCar.MagnitudeSqr());

            //distance between character and the spot where they talked to NPC to start mission.
            vaule2 = rmt::Sqrt(CharacterPositionVsCharacterStartPosition.MagnitudeSqr());



            //if the character is less than 2 meter from the forced carstart location then place them at the  spot that 
            //they were in when starting the talk to objective that started this mission
            if (vaule1 < 2.0f  &&  vaule2 < 20.0f)
            {
                pos = GetGameplayManager()->mPlayerAndCarInfo.mPlayerPosition;
            }
            player->RelocateAndReset( pos, rotation, true );

            //GetGameplayManager()->PutPlayerInCar( true );  //Hackish way to put the player in the car in the next / prev stage / mission
            //GetGameplayManager()->mShouldLoadDefaultVehicle = true;

            GetEventManager()->TriggerEvent( EVENT_CHARACTER_POS_RESET );
        }
        GetGameplayManager()->ClearVehicleSlot(GameplayManager::eOtherCar);            
    }

    //unload streetrace props if there are any for this mission
    InitStreetRacePropUnload();
    mbSwappedCars = false;


    if ( mDoorStars )
    {
        delete mDoorStars;
        mDoorStars = NULL;
    }
}

//=============================================================================
// Mission::SetToStage
//=============================================================================
// Description: Comment
//
// Parameters:  (unsigned int index)
//
// Return:      void 
//
//=============================================================================
void Mission::SetToStage( int index, bool resetting )
{
    if( index == mCurrentStage )
    {
        return;
    }
    
    MissionStage* stage = GetCurrentStage();
    if( stage != NULL )
    {

        stage->Finalize();
    }

    stage = GetStage( index );
    //Test if this is a "locked" stage and see if you can do it.
    if ( stage && stage->GetMissionLocked() )
    {
        if ( UnlockStage( stage ) )
        {

            //Now we always play the dialogue.
//            //Go to the next stage
//            rAssert( index < MAX_STAGES );
//            ++index;
        }
    }

    mCurrentStage = index;

    stage = GetCurrentStage();
    if( stage != NULL )
    {
        unsigned int time;
        MissionStage::StageTimeType type;

        stage->GetStageTime( type, time );

        switch( type )
        {
        case MissionStage::STAGETIME_NOT_TIMED:
            {
                mMissionTimer = -1;
                break;
            }
        case MissionStage::STAGETIME_ADD:
            {
                mMissionTimer += static_cast<int>( time * MS_PER_SEC );
                break;
            }
        case MissionStage::STAGETIME_SET:
            {
                mMissionTimer = static_cast<int>( time * MS_PER_SEC );
                break;
            }
        default:
            {
                rAssert( false );
                break;
            }
        }

        // new 
        // greg
        // jan 10, 2003
        
        // loop through up to and including this stage and
        // re-initialize the vehicle stuff
        if(resetting)
        {
            ResetPlayer();
    
            int i;
            for(i = 0; i < index; i++)
            {            
                GetStage(i)->VehicleInfoInitialize();
                GetStage(i)->VehicleFinalize();
            }
        }
        stage->Initialize();

        // Is this the start of the mission?
        //
        if( index == 0 )
        {
            mState = STATE_INPROGRESS;
        }
    }
}

//=============================================================================
// Mission::NextStage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Mission::NextStage()
{
    SetToStage( mCurrentStage + 1 );
}

//=============================================================================
// Mission::PrevStage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Mission::PrevStage()
{
    rAssert( mCurrentStage != -1 );

    if ( mCurrentStage == 0 )
    { //We're on the first stage.
        GetCurrentStage()->Finalize();

        ResetStage();
    }
    else
    {
        SetToStage( mCurrentStage - 1 );
    }
}

//=============================================================================
// Mission::ResetStage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Mission::ResetStage()
{
    GetCurrentStage()->Reset();
}

//=============================================================================
// Mission::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void Mission::HandleEvent( EventEnum id, void* pEventData )
{
    if((id == EVENT_ENTER_INTERIOR_TRANSITION_START) || (id == EVENT_EXIT_INTERIOR_START))
    {
        mNoTimeUpdate = true;
    }
    else if ((id == EVENT_ENTER_INTERIOR_END) || (id == EVENT_EXIT_INTERIOR_END))
    {
        mNoTimeUpdate = false;
    }
    else if ( id == EVENT_GUI_MISSION_START )
    {
        //Dirty dirty
        GetCurrentStage()->Start();
        GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Resume();

        if ( GetCurrentStage()->StartBonusObjective() )
        {
            StartBonusObjectives();
        }
    }
    else if ( id == EVENT_GUI_TRIGGER_PATTY_AND_SELMA_SCREEN)
    {
        mbTriggerPattyAndSelmaScreen = true;
    }
    else
    {
        MissionStage* pStage = GetCurrentStage();
        if( pStage != NULL )
        {
            pStage->HandleEvent( id, pEventData );
        } 
    }
}

//=============================================================================
// Mission::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void Mission::Update( unsigned int elapsedTime )
{
    if ( mChangingStages )
    {
        DoStageChange();
    }

    MissionStage* stage = GetCurrentStage();
    
    // MS9: take this out! There should always be a stage
    if( stage == NULL )
        return;

    rAssert( stage != NULL );
    

BEGIN_PROFILE( "Mission Update" );
    MissionStage::MissionStageState state = stage->GetProgress();

    ContextEnum currentContext = GetGameFlow()->GetCurrentContext();
    if( currentContext == CONTEXT_PAUSE )
    {
        state = MissionStage::STAGE_INPROGRESS;
    }

    switch( state )
    {
    case MissionStage::STAGE_INPROGRESS:
        {
            if (GetCurrentStage() != NULL)
            {
                if( mMissionTimer >=1 && !mNoTimeUpdate && GetCurrentStage()->mbDisablePlayerControlForCountDown != true)
                {
                    if(GetCurrentStage()->QueryUseElapsedTime() ==true) //make timer count up if stage is using elasped time. 
                    {
                        mMissionTimer += elapsedTime;
                    }
                    else
                    {
                        mMissionTimer -= elapsedTime;
                    }

                    mElapsedTimems += elapsedTime;
                    
                    //Chuck:only trigger red flashing timer if we aren't using elasped time.
                    

                        if( ( mMissionTimer <= 10000 ) 
                            && ( mMissionTimer + elapsedTime > 10000 ) 
                            && ( stage->GetFinalStage()
                            && !(GetCurrentStage()->QueryUseElapsedTime())
                            ) )
                        {
                            GetEventManager()->TriggerEvent( EVENT_TIME_RUNNING_OUT );
                        }
                    

                }
                GetCurrentStage()->Update( elapsedTime );

                unsigned int i;
                for ( i = 0; i < mNumBonusObjectives; ++i )
                {
                    mBonusObjectives[ i ]->Update( elapsedTime );
                }
                break;
            }
        }
    case MissionStage::STAGE_COMPLETE:
        {
            if( state == mLastStageState && mFinalDelay == -1 && mCompleteDelay == -1 )
            {
                break;
            }
            
            mbComplete = mCurrentStage == mNumMissionStages-1;

            if ( mFinalDelay == -1 && stage->GetFinalStage() && mbComplete )
            {
                //Setup the delay and skip outta here.
                MissionStage* thisStage = this->GetCurrentStage();
                MissionObjective* objective = thisStage->GetObjective();
                if( objective->IsPattyAndSelmaDialog() )
                {
                    mFinalDelay = 0;
                }
                else
                {
                    mFinalDelay = FINAL_DELAY;
                    //
                    // Tell the GUI system that the mission succeeded
                    //
                    if(!IsSundayDrive())
                    {
                        GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_MISSION_COMPLETE );
                    }
                }

                //TODO: Send off the bonus mission info too.
                GetEventManager()->TriggerEvent( EVENT_MISSION_SUCCESS );

                if( stage->GetObjective()->GetObjectiveType() == MissionObjective::OBJ_LOSETAIL )
                {
                    GetEventManager()->TriggerEvent( EVENT_TAIL_LOST_DIALOG );
                }
                else
                {
                    GetEventManager()->TriggerEvent( EVENT_MISSION_SUCCESS_DIALOG );
                }

                break;
            }
            else if ( stage->GetFinalStage() && mbComplete )
            {
                if ( static_cast<int>(elapsedTime) >= mFinalDelay )
                {
                    //All done, carry on.
                    mFinalDelay = -1;
                }
                else
                {
                    //Decrement the time and get outta here.  WE pause to show the mission complete stuff.
                    mFinalDelay -= elapsedTime;
                    break;
                }
            }

            // trigger stage complete event
            //
            unsigned int showStageComplete = stage->IsShowStageComplete() ? 1 : 0;

            if ( showStageComplete && mCompleteDelay == -1 )
            {
                //We need to delay the mission stage change to prevent weirdness.
                mCompleteDelay = COMPLETE_DELAY;

                GetEventManager()->TriggerEvent( EVENT_STAGE_COMPLETE,
                    reinterpret_cast<void*>( showStageComplete ) );

                if( stage->GetObjective()->GetObjectiveType() == MissionObjective::OBJ_LOSETAIL )
                {
                    GetEventManager()->TriggerEvent( EVENT_TAIL_LOST_DIALOG );
                }

                break;
            }
            else if ( showStageComplete )
            {
                if ( mCompleteDelay > static_cast<int>(elapsedTime) )
                {
                    mCompleteDelay -= elapsedTime;
                    break;
                }
                else
                {
                    //We're good.
                    mCompleteDelay = -1;
                }
            }

            if ( stage->GetFinalStage() || mbComplete )
            {
                mState = STATE_SUCCESS;

                if ( stage->GetFinalStage() )
                {
                    //To Do::MESA Insert Call to CharacterSheetManager to update 
                    //character sheet with complete and bonus object stuff                                 
                                      

                    if (mBonusObjectives[0] != NULL)
                    {
                        int seconds = (mElapsedTimems /1000);
                        GetCharacterSheetManager()->SetMissionComplete(GetGameplayManager()->GetCurrentLevelIndex(),mcName,mBonusObjectives[0]->GetSuccessful(),seconds );
                    }
                    else
                    {
                        int seconds = (mElapsedTimems/1000);
                        GetCharacterSheetManager()->SetMissionComplete(GetGameplayManager()->GetCurrentLevelIndex(),mcName,false,seconds);
                    }

                    if ( mbTriggerPattyAndSelmaScreen == true)
                    {
                        GetGuiSystem()->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                            CGuiWindow::GUI_SCREEN_ID_MISSION_SUCCESS,
                                            CLEAR_WINDOW_HISTORY );
                        GetEventManager()->TriggerEvent( EVENT_GUI_ENTERING_MISSION_SUCCESS_SCREEN );                
                        GetGameFlow()->SetContext( CONTEXT_PAUSE );
                        InitStreetRacePropUnload();
                        mbTriggerPattyAndSelmaScreen = false;
                    }


                    
                }

                //This is silly....  Dirty too.  At least it's not the GUI doing it.
                //Really this just updates the state of all the missions.  GAH.
                GetGameplayManager()->ContinueGameplay();

                if ( mCurrentStage != -1 )
                {
                    //GetCurrentStage()->Reset();
                    GetCurrentStage()->Start();

                    if ( GetCurrentStage()->StartBonusObjective() )
                    {
                        StartBonusObjectives();
                    }
                }
            }
            else
            {
                SetupStageChange();
            }
            break;
        }
    case MissionStage::STAGE_FAILED:
        {
            unsigned int i;
            unsigned int numConditions;
            bool isChaseCondition = false;

            if( state == mLastStageState )
            {
                break;
            }
            
            mState = STATE_FAILED;
            //if we fail a forced car relock the forced car
            if ( IsForcedCar() == true)
            {                
                if (GetGameplayManager()-> mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle != NULL)
                {

                    GetGameplayManager()-> mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle->ActivateTriggers(false);
                }

            }



            // Tell the GUI system that mission failed
            //
            GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_MISSION_FAILED );

            // get the failure condition
            //
            MissionCondition* failedCondition = stage->GetFailureCondition();
            rTuneAssertMsg( failedCondition != NULL,
                        "WTF?? How could there be no failure condition?" );

            //
            // Determine whether any of the conditions involve chases
            //
            numConditions = stage->GetNumConditions();
            for( i = 0; i < numConditions; i++ )
            {
                if( stage->GetCondition( i )->IsChaseCondition() )
                {
                    isChaseCondition = true;
                    break;
                }
            }
            GetEventManager()->TriggerEvent( EVENT_MISSION_FAILURE, reinterpret_cast<void*>( failedCondition ) );

            mbComplete = true;

            //
            // We need to re-enable world rendering if we failed because of 
            // entering an interior
            //
            bool entering = GetInteriorManager()->IsEntering();
            if( entering )
            {
                GetRenderManager()->mpLayer( RenderEnums::LevelSlot )->Thaw();
            }

            if(IsBonusMission())
            {
                GetCharacterManager()->ResetBonusCharacters();
            }

            break;
        }
    case MissionStage::STAGE_BACKUP:
        {
            //This is a bad thing.
            rAssert( false );
            
/*            PrevStage();
            GetCurrentStage()->Start();
*/
            break;
        }
    default:
        {
            // whatever
            break;
        }
    }

    mLastStageState = state;

    if ( mDoorStars )
    {
        mDoorStars->Update( elapsedTime );
    }

    DoUpdate( elapsedTime );
END_PROFILE( "Mission Update" );
}

//=============================================================================
// Mission::IsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Mission::IsComplete()
{
    return( mbComplete );
}

//=============================================================================
// Mission::SetRestartDyanload
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* loadString )
//
// Return:      void 
//
//=============================================================================
void Mission::SetRestartDynaload( const char* loadString, const char* interior)
{
    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
    MEMTRACK_PUSH_GROUP( "Mission - Dynaload" );

    if ( mDynaloadLoc )
    {
        mDynaloadLoc->Release();
    }

    if( strcmp( loadString, "" ) != 0 )
    {
        mDynaloadLoc = new ZoneEventLocator;
        mDynaloadLoc->AddRef();
        mDynaloadLoc->SetZoneSize( strlen(loadString) + 1 );
        mDynaloadLoc->SetZone( loadString );
        mDynaloadLoc->SetPlayerEntered();

        if(interior)
        {
            mDynaloadLoc->SetName(interior);
        }
    }
    else
    {
        mDynaloadLoc = NULL;
    }

    MEMTRACK_POP_GROUP( "Mission - Dynaload" );
    HeapMgr()->PopHeap (GMA_LEVEL_MISSION);
}

//=============================================================================
// Mission::AddBonusObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( BonusObjective* bo )
//
// Return:      void 
//
//=============================================================================
void Mission::AddBonusObjective( BonusObjective* bo )
{
    rAssert( mNumBonusObjectives < MAX_BONUS_OBJECTIVES );

    mBonusObjectives[ mNumBonusObjectives ] = bo;
    ++mNumBonusObjectives;
}

//=============================================================================
// Mission::StartBonusObjectives
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Mission::StartBonusObjectives()
{
    unsigned int i;
    for ( i = 0; i < mNumBonusObjectives; ++i )
    {
        mBonusObjectives[ i ]->Start();
    }
}


//=============================================================================
// Mission::SetForcedCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool isForced )
//
// Return:      void 
//
//=============================================================================
void Mission::SetForcedCar( bool isForced )
{
    mIsForcedCar = isForced;
}

//=============================================================================
// Mission::SetMissionTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( int timeMilliseconds )
//
// Return:      void 
//
//=============================================================================
void Mission::SetMissionTime( int timeMilliseconds )
{
    mMissionTimer = timeMilliseconds;
}
//=============================================================================
// Mission::CreateStatePropCollectible
//=============================================================================
// Description: Creates a new statepropcollectible and places it at the given locator
//
// Parameters:  name of the CStateProp data chunk in the inventory, name of the locator
//
// Return:      void 
//
//=============================================================================
void Mission::CreateStatePropCollectible( const char* statepropname, const char* locatorname, int collisionattributes )
{
    // No space in the array, bail
    if ( mNumStatePropCollectibles >= MAX_NUM_STATEPROP_COLLECTIBLES )
        return;

    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
    Locator* locator = p3d::find< Locator >( locatorname );
    if ( locator )
    {
        CStatePropData* propdata = p3d::find< CStatePropData >(statepropname);
        if ( propdata )
        {
            // Get the locator's position
            rmt::Vector locatorposition;
            locator->GetPosition( &locatorposition );

            // Fill out a translation matrix
            rmt::Matrix transform;
            transform.Identity();
            transform.FillTranslate( locatorposition );

            StatePropCollectible* collectible = new StatePropCollectible();
            CollisionAttributes* collAttr = GetATCManager()->CreateCollisionAttributes( PROP_MOVEABLE, collisionattributes, 2.242f );
            collAttr->AddRef();
            collectible->LoadSetup( propdata, 0, transform, collAttr, false, NULL );
            collAttr->Release();

            collectible->SetName( statepropname );
            p3d::inventory->Store( collectible );

            // Add it to the internal list of prop collectibles
            // Allocate a new list if necessary
            if ( mNumStatePropCollectibles == 0 )
            {
                mStatePropCollectibles = new StatePropCollectible*[ MAX_NUM_STATEPROP_COLLECTIBLES ];
            }
            mStatePropCollectibles[ mNumStatePropCollectibles ] = collectible;
            collectible->AddRef();

            mNumStatePropCollectibles++;

        }
        else
        {
            rReleaseAssertMsg("can't find stateprop %s\n",statepropname);
        }
    }
    else
    {
        rReleaseAssertMsg("can't find locator %s\n",locatorname);
    }
    HeapMgr()->PopHeap (GMA_LEVEL_MISSION);
}



//=============================================================================
// Mission::AttachStatePropCollectible
//=============================================================================
// Description: Creates and attaches a new statepropcollectible on a vehicle
//
// Parameters:  name of the CStateProp data chunk in the inventory, name of the vehicle
//
// Return:      void 
//
//=============================================================================
void Mission::AttachStatePropCollectible( const char* statepropname, const char* vehicleName, int collisionattributes )
{
    // No space in the array, bail
    if ( mNumStatePropCollectibles >= MAX_NUM_STATEPROP_COLLECTIBLES )
        return;

    CStatePropData* statePropData = p3d::find< CStatePropData > (statepropname);
    if ( statePropData )
    {
        Vehicle* vehicle = GetVehicleCentral()->GetVehicleByName( vehicleName );
        StatePropCollectible* stateprop = new StatePropCollectible();
        rmt::Matrix transform;
        transform.Identity();
        // Create some collision attributes. Unfort I may just have to leave these hardcoded.
        // Settings seem to make very little difference anyway.
        CollisionAttributes* collAttr = GetATCManager()->CreateCollisionAttributes( PROP_MOVEABLE, collisionattributes, 2.242f );
        collAttr->AddRef();
        stateprop->LoadSetup( statePropData, 0, transform, collAttr, false );
        collAttr->Release();
        // Add it to scenegraph
        stateprop->AddToDSG();
        vehicle->AttachCollectible( stateprop );    

        // Add it to the internal list of prop collectibles
        // Allocate a new list if necessary
        if ( mNumStatePropCollectibles == 0 )
        {
            mStatePropCollectibles = new StatePropCollectible*[ MAX_NUM_STATEPROP_COLLECTIBLES ];
        }
            
        stateprop->SetName( statepropname );
        p3d::inventory->Store( stateprop );

        mStatePropCollectibles[ mNumStatePropCollectibles ] = stateprop;
        stateprop->AddRef();

        mNumStatePropCollectibles++;
    }


}


//=============================================================================
// Mission::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool JumpStage )
//
// Return:      void 
//
//=============================================================================
void Mission::Reset( bool jumpStage )
{
    mbComplete = false;
    mState = STATE_INPROGRESS;
    mbIsLastStage = false;
    mElapsedTimems = 0;
    mbCarryOverOutOfCarCondition = false;

    //Reset ChaseManager Spawn Rate incase of Retries
    GameplayManager* gameplayManager = GetGameplayManager();
    if ( gameplayManager != NULL )
    {
        ChaseManager* chaseManager = gameplayManager->GetChaseManager(0);
        if ( chaseManager != NULL )
        {
            chaseManager->SetMaxObjects(0);
        }
        gameplayManager->MakeSureHusksAreReverted();    // do this rather than let GameplayManager catch event below, because that's too late I think
    }

    SetToStage( -1 );

    if ( jumpStage )
    {   
        SetToStage( mResetToStage, jumpStage );   
        
        // repair car on mission reset or retry
    
        GetEventManager()->TriggerEvent(EVENT_REPAIR_CAR);
       
        
    }
    else
    {
        SetToStage( 0 );
     
    }
    
    //Reset the bonus objectives.
    unsigned int i;
    for ( i = 0; i < mNumBonusObjectives; ++i )
    {
        mBonusObjectives[ i ]->Reset();
    }

    GetEventManager()->TriggerEvent( EVENT_MISSION_RESET, (void*)( jumpStage ) );
       
    
    GetCharacterSheetManager()->IncrementMissionAttempt(GetGameplayManager()->GetCurrentLevelIndex(),this->mcName);
    InitStreetRacePropLoad();
    
    //make sure our current car doesn't have locked doors
    //since forced cars have locked doors.

    if(IsForcedCar())
    {
        if(GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle != NULL)
        {
            GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].mp_vehicle->ActivateTriggers(true);
        }
    }

}

//=============================================================================
// Mission::ResetPlayer
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Mission::ResetPlayer()
{
    //This is where we get the reset data and apply it.  Sounds good eh?
    Character* player = GetCharacterManager()->GetCharacter( 0 ); //This is the player...
    Vehicle* car = GetGameplayManager()->GetCurrentVehicle(); //Current car

    if( mVehicleRestart != NULL )
    {
        GetGameplayManager()->PlaceVehicleAtLocator( car, mVehicleRestart );

    }

    //if ( mPlayerRestart == NULL || IsForcedCar() || IsRaceMission() )
    if ( mPlayerRestart == NULL || IsRaceMission() )
    {
        rmt::Vector pos;
        mVehicleRestart->GetLocation( &pos );
        player->RelocateAndReset( pos, 0, true );
        if (GetGameplayManager()->GetCurrentVehicle()->mVehicleDestroyed != true)
        {
            GetAvatarManager()->PutCharacterInCar( player, car );
        }
    }
    else 
    {
        //TODO:  Make a put player on ground type thingie.  I shouldn't have to know about this.
        GetGameplayManager()->PlaceCharacterAtLocator( player, mPlayerRestart );
        GetAvatarManager()->PutCharacterOnGround( player, car );
    }    

    GetEventManager()->TriggerEvent( EVENT_CHARACTER_POS_RESET );

    //Let's cut the camera.
    GetSuperCamManager()->GetSCC( 0 )->NoTransition();
    GetSuperCamManager()->GetSCC( 0 )->DoCameraCut();
}

//=============================================================================
// Mission::InitDynaLoad
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Mission::InitDynaLoad()
{
    rTuneAssertMsg( mDynaloadLoc, "DYNALOAD information is required in all missions!" );

    GetEventManager()->TriggerEvent( EVENT_FIRST_DYNAMIC_ZONE_START, mDynaloadLoc );   

    // set the pedgroup sheeyatsu...
    if( 0 <= mInitPedGroupId && mInitPedGroupId < PedestrianManager::MAX_MODEL_GROUPS )
    {
        PedestrianManager::GetInstance()->SwitchModelGroup( mInitPedGroupId );
        PedestrianManager::SetDefaultModelGroup( mInitPedGroupId );
    }
}

void Mission::SetInitPedGroup( int initGroupId )
{
    rTuneAssert( 0 <= mInitPedGroupId && mInitPedGroupId < PedestrianManager::MAX_MODEL_GROUPS );
    mInitPedGroupId = initGroupId;
}

//=============================================================================
// Mission::DialogueCharactersTeleported
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Mission::DialogueCharactersTeleported()
{
    if ( !GetCurrentStage() ||
         !(GetCurrentStage()->GetObjective()) )
    {
        //DAMN THOSE COSMIC RAYS!
        return false;  //No cut.
    }

    if ( GetCurrentStage()->GetObjective()->GetObjectiveType() == MissionObjective::OBJ_DIALOGUE )
    {
        return static_cast<DialogueObjective*>(GetCurrentStage()->GetObjective())->CharactersReset();
    }

    return false;
}

//=============================================================================
// Mission::GetCurrentStage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
MissionStage* Mission::GetCurrentStage()
{
    if(( mCurrentStage >= 0 ) && (mCurrentStage < (int)mNumMissionStages ))
    {
        return( mMissionStages[ mCurrentStage ] ); 
    }
    else
    {
        return( NULL );
    }
    
}

//=============================================================================
// Mission::GetMissionTimeLeftInSeconds
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
int Mission::GetMissionTimeLeftInSeconds()
{
    return( static_cast<int>( mMissionTimer * SEC_IN_MSEC ));
}

//=============================================================================
// Mission::GetMissionTimeLeftInMilliSeconds
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
int Mission::GetMissionTimeLeftInMilliSeconds()
{
    return( mMissionTimer );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// Mission::DoUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void Mission::DoUpdate( int elapsedTime )
{
}

//=============================================================================
// Mission::SetBonusMission
//=============================================================================
// Description: sets a flag if this is a bonus mission
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void Mission::SetBonusMission()
{
    mBonusMisison = true;
}

//=============================================================================
// Mission::IsBonusMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( int elapsedTime )
//
// Return:      void 
//
//=============================================================================
bool Mission::IsBonusMission()
{
    if( IsRaceMission() )
    {
        return false;
    }
    return mBonusMisison;
}

//=============================================================================
// Mission::IsBonusMission
//=============================================================================
// Description: determines if this mission is a special road race
//
// Parameters:  ( int elapsedTime )
//
// Return:      void 
//
//=============================================================================
bool Mission::IsRaceMission()
{
    if( ( mcName[ 0 ] == 's' ) && ( mcName[ 1 ] == 'r' ) )
    {
        return true;
    }
    return false;
}

//=============================================================================
// Mission::IsWagerMission
//=============================================================================
// Description: determines if this mission is a wager mission or not
//
// Parameters:  NONE
//
// Return:      bool - is it or isn't it? 
//
//=============================================================================
bool Mission::IsWagerMission()
{
    if( ( mcName[ 0 ] == 'g' ) && ( mcName[ 1 ] == 'r' ) )
    {
        return true;
    }
    return false;
}

//=============================================================================
// Mission::UnlockStage
//=============================================================================
// Description: Comment
//
// Parameters:  ( MissionStage* stage )
//
// Return:      bool 
//
//=============================================================================
bool Mission::UnlockStage( MissionStage* stage )
{
    unsigned int i;
    for ( i = 0; i < MAX_LOCK_REQUIREMENTS; ++i )
    {
        const MissionStage::LockRequirement& requirement = stage->GetLockRequirement( i );
        if ( requirement.mType == MissionStage::LockRequirement::NONE )
        {
            continue;
        }

        //Test this against what the character has.
        if ( requirement.mType == MissionStage::LockRequirement::CAR )
        {
            if ( strcmp( GetGameplayManager()->GetCurrentVehicle()->mName, requirement.mName ) != 0 )
            {
                //Failed the car test.
                return false;
            }
        }
        else if ( requirement.mType == MissionStage::LockRequirement::SKIN /*Should test for skins.*/ )
        {
            Character* character = GetAvatarManager()->GetAvatarForPlayer(0)->GetCharacter();
            if ( strcmp( GetCharacterManager()->GetModelName( character ), requirement.mName ) != 0 )
            {
                //failed the skin test
                return false;
            }
        }
    }

    return true;
}


//=============================================================================
// Mission::LoadStreetRaceProps
//=============================================================================
// Description: Load the p3d file that contains the instanced streetrace props
//
// Parameters:  ( const char* loadString )
//
// Return:      void 
//
//=============================================================================
void Mission::LoadStreetRaceProps( const char* loadString )
{
    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
    MEMTRACK_PUSH_GROUP( "Mission - StreetRaceProps" );

    //clear any junk
    if ( mStreetRacePropsLoad != NULL)
    {
        mStreetRacePropsLoad->ReleaseVerified();
    }

    if( strcmp( loadString, "" ) != 0 )
    {
        mStreetRacePropsLoad = new ZoneEventLocator;
        mStreetRacePropsLoad->AddRef();
        mStreetRacePropsLoad->SetZoneSize( strlen(loadString) + 1 );
        mStreetRacePropsLoad->SetZone( loadString );
        mStreetRacePropsLoad->SetPlayerEntered();
    }
    else
    {
        mStreetRacePropsLoad = NULL;
    }

    MEMTRACK_POP_GROUP( "Mission - StreetRaceProps" );
    HeapMgr()->PopHeap (GMA_LEVEL_MISSION);
}


//=============================================================================
// Mission::LoadStreetRaceProps
//=============================================================================
// Description: Unload the p3d file that contains the instanced streetrace props
//
// Parameters:  ( const char* loadString )
//
// Return:      void 
//
//=============================================================================
void Mission::UnloadStreetRaceProps( const char* loadString )
{
    HeapMgr()->PushHeap (GMA_LEVEL_MISSION);
    MEMTRACK_PUSH_GROUP( "Mission - StreetRacePropsUnload" );

    //clear any junk
    if ( mStreetRacePropsUnload != NULL)
    {
        mStreetRacePropsUnload->ReleaseVerified();
    }

    if( strcmp( loadString, "" ) != 0 )
    {
        mStreetRacePropsUnload = new ZoneEventLocator;
        mStreetRacePropsUnload->AddRef();
        mStreetRacePropsUnload->SetZoneSize( strlen(loadString) + 1 );
        mStreetRacePropsUnload->SetZone( loadString );
        mStreetRacePropsUnload->SetPlayerEntered();
    }
    else
    {
        mStreetRacePropsUnload = NULL;
    }

    MEMTRACK_POP_GROUP( "Mission - StreetRacePropsUnload" );
    HeapMgr()->PopHeap (GMA_LEVEL_MISSION);
}

void Mission::SetupStageChange()
{
    mChangingStages = true;

     GetCurrentStage()->DoTransition();
}

void Mission::DoStageChange()
{
//    if( !GetInteriorManager()->IsEntering() && !GetInteriorManager()->IsExiting() )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_RESUME_INGAME );
    }

    MissionStage* currStage = GetCurrentStage();
    if ( currStage->GetMissionLocked() && !UnlockStage( currStage ) )
    {
        //Display the new message and stop the next stage from displaying its message
        rAssert( currStage->GetStartMessageIndex() >= 0 );

        GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_DISPLAY_PROMPT, currStage->GetStartMessageIndex() );
        GetEventManager()->TriggerEvent( EVENT_STAGE_TRANSITION_FAILED );

        GetStage( mCurrentStage + 1 )->ShowStartMessageIndex( false );

        NextStage();
    }
    else if ( mJumpBackStage )
    {
        rTuneAssertMsg( (mCurrentStage - mJumpBackBy) >= 0, "Jumping back too far!" );
        SetToStage( mCurrentStage - mJumpBackBy );
        mJumpBackStage = false;
    }
    else
    {
        NextStage();
    }

    GetCurrentStage()->Start();
    if ( GetCurrentStage()->StartBonusObjective() )
    {
        StartBonusObjectives();
    }

    mChangingStages = false;
}


void Mission::SetSwappedCarsFlag(bool flag)
{
    mbSwappedCars = flag;
}


bool Mission::GetSwappedCarsFlag()
{
    return mbSwappedCars;
}


void Mission::InitStreetRacePropLoad()
{
     //Chuck: Trigger the loading of the Street Race Specify Props
    if (mStreetRacePropsLoad != NULL)
    {
        GetEventManager()->TriggerEvent( (EventEnum)(EVENT_LOCATOR+LocatorEvent::DYNAMIC_ZONE), mStreetRacePropsLoad );
                
        // about to do a street race so disable peds and parked cars
        if( IsRaceMission() )
        {
            PedestrianManager::GetInstance()->DumpAllPedModels();
        }
        else
        {
            PedestrianManager::GetInstance()->RemoveAllPeds();
        }
        PedestrianManager::GetInstance()->AllowAddingPeds(false);        
        GetPCM().DisableParkedCars();     
                
    }

}

void Mission::InitStreetRacePropUnload()
{
    if (mStreetRacePropsUnload != NULL)
    {
        //Chuck: Trigger the unloading of the Street Race Specify Props
        GetEventManager()->TriggerEvent( (EventEnum)(EVENT_LOCATOR+LocatorEvent::DYNAMIC_ZONE), mStreetRacePropsUnload );
        
        // street race done so reenable peds and parked cars
        PedestrianManager::GetInstance()->AllowAddingPeds(true);        
        GetPCM().EnableParkedCars();   
        
    }
}

//=============================================================================
// Mission::ShowHUD
//=============================================================================
// Description: Comment
//
// Parameters:  
//
// Return:      inline 
//
//=============================================================================
void Mission::ShowHUD( bool isShowHUD )
{
    mShowHUD = isShowHUD;

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        currentHud->SetVisible( isShowHUD );
    }
}

bool Mission::CanMDKCar(Vehicle* pVehicle,MissionStage* pStage)
{
    //search for the input stage.

    int counter =0;
    int i = 0;
    int index =0;

    for (i = 0;i<MAX_STAGES;i++)
    {
        if (mMissionStages[i] != NULL)
        {
            if (mMissionStages[i] == pStage)
            {
                index  = i+1;
                break;
            }
        }
    }

    for (index; index <MAX_STAGES;index++)
    {
        if (mMissionStages[index] != NULL )
        {
            for (int j =0; j< MissionStage::MAX_VEHICLES;j++)
            {
                if ( mMissionStages[index]->GetVehicle(j) == pVehicle)
                {            
                    counter++;
                }
            }//end of vehicle search loop
        }
    }//end of out missionstage loop
    
    if ( counter > 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

 

