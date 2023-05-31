//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supersprintmanager.cpp
//
// Description: Implement SuperSprintManager
//
// History:     2/3/2003 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radtime.hpp>
#include <p3d/utility.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <string.h>

//========================================
// Project Includes
//========================================

#include <memory/srrmemory.h>
#include <loading/loadingmanager.h>
#include <gameflow/gameflow.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/animatedcam.h>
#include <mission/missionscriptloader.h>
#include <render/loaders/billboardwrappedloader.h>
#include <render/dsg/animcollisionentitydsg.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <meta/carstartlocator.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>
#include <input/inputmanager.h>
#include <events/eventmanager.h>
#include <meta/eventlocator.h>
#include <contexts/supersprint/supersprintcontext.h>
#include <gameflow/gameflow.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guimanager.h>
#include <sound/soundmanager.h>

#include <roads/roadmanager.h>
#include <roads/roadsegment.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <render/intersectmanager/intersectmanager.h>

#include <supersprint/supersprintmanager.h>

#include <mission/gameplaymanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

const float MIN_SCALE = 0.01f;
const float MAX_SCALE = 1.0f;
const float MIN_DIST = 10.0f;
const float MAX_DIST = 80.0f;

SuperSprintManager* SuperSprintManager::spInstance = NULL;

const int DNF_TIMEOUT_TIME = 10000;

#define GET_TIME ::radTimeGetMicroseconds()

static char CHEKPOINT_LIST_B01 [] =
{
    2, 3, 4, 5, 6, 7, 8, 1
};

static char CHEKPOINT_LIST_B01_R [] =
{
    8, 7, 6, 5, 4, 3, 2, 1
};

static char CHECKPOINT_LIST_B02 [] = 
{
    2, 3, 4, 5, 6, 3, 8, 9, 1
};

static char CHECKPOINT_LIST_B02_R [] = 
{
    9, 8, 3, 6, 5, 4, 3, 2, 1
};

static char CHECKPOINT_LIST_B03 [] = 
{
    2, 3, 4, 5, 6, 7, 1
};

static char CHECKPOINT_LIST_B03_R [] = 
{
    7, 6, 5, 4, 3, 2, 1
};

static char CHECKPOINT_LIST_B05 [] =
{
    2, 3, 4, 5, 6, 7, 8, 9, 10, 7, 3, 11, 1
};

static char CHECKPOINT_LIST_B05_R [] =
{
    11, 3, 7, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
};

static char CHECKPOINT_LIST [] = 
{
    2, 3, 1
};

static char CHECKPOINT_LIST_R [] = 
{
    3, 2, 1
};

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//From EventListener
//=============================================================================
// SuperSprintManager::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::HandleEvent( EventEnum id, void* pEventData )
{
#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
    char errMsg[128];
#endif

    if ( id == EVENT_LOCATOR + LocatorEvent::CHECK_POINT )
    {
        EventLocator* loc = static_cast<EventLocator*>(pEventData);

        char checkNum = static_cast<char>( loc->GetData() );

        if ( loc->GetPlayerEntered() && checkNum != 0 )  //Ignore the 0 ones.
        {
            //The idea here is to detect if the player is attempting to do the 
            //checkpoints out of order.  If the checkpoint passed is not the one we want
            //we test to see if the player has already hit a bad one and if not we decrement
            //where they are going (send them back) and mark them as cheaters.
            //Then, only when they pass the one they're supposed to do they get unmarked
            //as cheaters and get to move on to the next check point.  The first checkpoint is a 
            //bit tricky as the players start inside the last checkpoint and would thus be
            //considered cheaters.  We can ignore cheaters cheating on checkpoint 2.
            //Indexes start at 1

            int playerID = loc->GetPlayerID();

#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
            sprintf( errMsg, "Something wrong with player %d", playerID+1 );
            rTuneAssertMsg( 0 <= playerID && playerID < 4, errMsg );
            rTuneAssertMsg( 0 <= mPlayers[ playerID ].mNextCheckPoint && 
                mPlayers[ playerID ].mNextCheckPoint < GetNumCheckpoints(), errMsg );
#endif

            // find the one we're supposed to be at
            const char nextCheck = mCheckPoints[ mPlayers[ playerID ].mNextCheckPoint ];

            // find the one we were just at
            char lastCheckIdx = mPlayers[ playerID ].mNextCheckPoint;
            if ( lastCheckIdx == 0 )
            {
                lastCheckIdx = GetNumCheckpoints() - 1;
            }
            else
            {
                lastCheckIdx--;
            }
            rTuneAssert( 0 <= lastCheckIdx && lastCheckIdx < GetNumCheckpoints() );
            const char prevCheck = mCheckPoints[ lastCheckIdx ];

            if ( checkNum != nextCheck )
            {
                //This guy is possibly cheating
                if ( !mPlayers[ playerID ].mCheated && 
                     checkNum != prevCheck )
                {
                    //CHEATER!


                    // we want to knock him back to aim for the previous check
                    mPlayers[ playerID ].mNextCheckPoint = lastCheckIdx;
                    /*
                    if ( nextCheck == 1 )
                    {
                        mPlayers[ playerID ].mNextCheckPoint = GetNumCheckpoints() - 1;
                    }
                    else
                    {
                        mPlayers[ playerID ].mNextCheckPoint = nextCheck - 1;
                    }
                    */

#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
                    rTuneAssertMsg( 0 <= mPlayers[ playerID ].mNextCheckPoint && 
                        mPlayers[ playerID ].mNextCheckPoint < GetNumCheckpoints(), errMsg );
#endif

                    mPlayers[ playerID ].mCheated = true;
                }
            }
            else
            {
                //Unmark the cheater
                bool previouslyCheated = mPlayers[ playerID ].mCheated;
                mPlayers[ playerID ].mCheated = false;

                if( nextCheck == 1 && !previouslyCheated )  //This is the finish line.
                {
                    //This guy has completed a lap
                    mPlayers[ playerID ].mNumLaps++;

                    //Figure out his lap time.
                    unsigned int endTime = GET_TIME;
                    unsigned int time = endTime - mStartTime;

                    unsigned int lapTime = time - mPlayers[ playerID ].mRaceTime;

                    //Possibly set the best lap time
                    if ( lapTime < mPlayers[ playerID ].mBestLap )
                    {
                        mPlayers[ playerID ].mBestLap = lapTime;
                    }

                    //Set the lapTime
                    mPlayers[ playerID ].mLapTime = lapTime;

                    //Set the raceTime
                    mPlayers[ playerID ].mRaceTime += lapTime;

                    if ( static_cast<int>( mPlayers[ playerID ].mNumLaps ) == mNumLaps - 1 )
                    {
                        //Show the white flag for a few seconds.
                        mWFlag->ShouldRender( true );
                        mWFlagTimeout = SuperSprintData::FLAG_TIMEOUT;
                    }
                    else if ( static_cast<int>( mPlayers[ playerID ].mNumLaps ) == mNumLaps )
                    {
                        //Show the finished flag for a few seconds.
                        mFFlag->ShouldRender( true );
                        mFFlagTimeout = SuperSprintData::FLAG_TIMEOUT;

                        //This guy is finished.
                        if( !mVehicleSlots[ playerID ].mIsHuman )
                        {
                            mVehicleSlots[ playerID ].mVehicleAI->SetActive( false );
                        }
                        else
                        {
                            //Disable his controller
                            mNumHumansFinished++;
                            int controllerID = GetInputManager()->GetControllerIDforPlayer( playerID );
                            if( controllerID != -1 )
                            {
                                GetInputManager()->GetController( controllerID )->SetGameState( Input::ACTIVE_SS_GAME );
                            }
                        }

                        mPlayers[ playerID ].mPosition = mCurrentPosition;

                        //Only one guy left, or all humans done, go to DNF
                        if ( mCurrentState != DNF_TIMEOUT &&
                            ( mCurrentPosition == mNumActivePlayers - 1 ||
                              mNumHumansFinished == mNumHumanPlayers ) )
                        {
                            mCountDownTime = DNF_TIMEOUT_TIME;
                            mDrawable->SetRenderState( SuperSprintDrawable::COUNT_DOWN );
                            mDrawable->SetTextScale( 2.0f );
                            sprintf( mTime, "%d", mCountDownTime / 1000 );
                            mDrawable->SetCountDownMSG( mTime );
                             
                            SetState( DNF_TIMEOUT );
                        }
                        else if ( mCurrentPosition == mNumActivePlayers )
                        {
                            //We're done!
                            mCountDownTime = 0;
                        }

                        mCurrentPosition++;
                    }
                }
                
                mPlayers[ playerID ].mNextCheckPoint++;
                if ( mPlayers[ playerID ].mNextCheckPoint == GetNumCheckpoints() )
                {
                    mPlayers[ playerID ].mNextCheckPoint = 0;  //Reset
                }

#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
                rTuneAssertMsg( 0 <= mPlayers[ playerID ].mNextCheckPoint && 
                    mPlayers[ playerID ].mNextCheckPoint < GetNumCheckpoints(), errMsg );
#endif
            }
        }
    }
    else if ( id == EVENT_LOCATOR + LocatorEvent::TRAP )
    {
        EventLocator* loc = static_cast<EventLocator*>(pEventData);

        if ( loc->GetPlayerEntered() )
        {
            unsigned int locID = loc->GetData();
            if ( !mTrapTriggered && 
                 mPositions[ loc->GetPlayerID() ] == 1 &&
                 ((!mGoLeft && locID == 0) || (mGoLeft && locID == 1)) )
            {
                //This toggles on
                mTrapController->SetAnimationDirection( 1.0f );
                mTrapTriggered = true;
            }
            else if ( mTrapTriggered &&
                      mPositions[ loc->GetPlayerID() ] == 1 &&
                     ((!mGoLeft && locID == 1) || (mGoLeft && locID == 0)) )
            {
                //This toggles off
                mTrapController->SetAnimationDirection( -1.0f );
                mTrapTriggered = false;
            }        
        }
    }
    else if ( id == EVENT_ANIMATED_CAM_SHUTDOWN )
    {
        DisableAllControllers();
    }

    GameplayManager::HandleEvent( id, pEventData );
}


//From LoadingManager::ProcessRequestsCallback
//=============================================================================
// SuperSprintManager::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::OnProcessRequestsComplete( void* pUserData )
{
    PositionCharacters();

    SetupIcons();

    GetGameFlow()->SetContext( CONTEXT_SUPERSPRINT );
}

//Local
//=============================================================================
// SuperSprintManager::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      SuperSprintManager
//
//=============================================================================
SuperSprintManager* SuperSprintManager::GetInstance()
{
    if ( spInstance == NULL )
    {
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        spInstance = new SuperSprintManager();
        spInstance->AddRef();
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    }

    return spInstance;
}

//=============================================================================
// SuperSprintManager::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::DestroyInstance()
{
    if ( spInstance )
    {
        spInstance->Release();
    }

    spInstance = NULL;
}

//=============================================================================
// SuperSprintManager::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::Initialize()
{
    GameplayManager::Initialize();

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    //Add the drawable to the views
    mDrawable = new SuperSprintDrawable();
    rAssert( mDrawable );

    mDrawable->AddRef();

    RenderManager* rm = GetRenderManager();
    RenderLayer* rloutside = rm->mpLayer( RenderEnums::LevelSlot );
    rAssert( rloutside );

    rloutside->AddGuts( mDrawable );   

    mDrawable->SetCarData( mVehicleSlots );
    mDrawable->SetPlayerData( mPlayers );

    int i;
    for( i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        int controllerID = GetInputManager()->GetControllerIDforPlayer( i );

        mVehicleSlots[ i ].mState = (controllerID != -1) ?
                                    SuperSprintData::CarData::SELECTED :
                                    SuperSprintData::CarData::WAITING;

        mVehicleSlots[ i ].mIsHuman = (controllerID != -1);
        mPlayers[ i ].mRacing = (controllerID != -1);
    }

    EnumerateControllers();

    mNumCheckPoints = 0;

    GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::CHECK_POINT) );
    GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::TRAP) );
    GetEventManager()->AddListener( this, EVENT_ANIMATED_CAM_SHUTDOWN );

    mFFlag = new AnimatedIcon();
    mWFlag = new AnimatedIcon();

    int j;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        for ( j = 0; j < SuperSprintData::NUM_PLAYERS; ++j )
        {
            mPositionIcon[ i ][ j ] = new AnimatedIcon();
        }

        mPlayerID[ i ] = new AnimatedIcon();
        mNitroEffect[ i ] = new AnimatedIcon();
    }

    for ( i = 0; i < MAX_AI_WAYPOINTS; ++i )
    {
        mPathData[ i ].closestElem.elem = NULL;
        mPathData[ i ].roadT = 0.0f;
        mPathData[ i ].seg = NULL;
        mPathData[ i ].segT = 0.0f;
        mPathData[ i ].loc = NULL;
    }

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
}

//=============================================================================
// SuperSprintManager::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::Finalize()
{
    //Remove the drawable from the views
    RenderManager* rm = GetRenderManager();
    RenderLayer* rloutside = rm->mpLayer( RenderEnums::LevelSlot );
    rAssert( rloutside );

    rloutside->RemoveGuts( mDrawable );

    //Shut down the controllers.
    InputManager* im = GetInputManager();

    unsigned int i;
    for ( i = 0; i < Input::MaxControllers; ++i )
    {
        im->UnregisterMappable( i, this );
    }

    GetEventManager()->RemoveAll( this );

    mDrawable->Release();
    mDrawable = NULL;

    CleanUpCars();

    delete mFFlag;
    mFFlag = NULL;

    delete mWFlag;
    mWFlag = NULL;

    int j;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        for ( j = 0; j < SuperSprintData::NUM_PLAYERS; ++j )
        {
            delete mPositionIcon[ i ][ j ];
            mPositionIcon[ i ][ j ] = NULL;
        }

        delete mPlayerID[ i ];
        delete mNitroEffect[ i ];
    }

    if ( mTrapController )
    {
        mTrapController->Release();
        mTrapController = NULL;
    }

    for ( i = 0; i < mNumCheckPointLocators; ++i )
    {
        //mCheckPointLocators[ i ]->Release();
        mCheckPointLocators[ i ] = NULL;
    }

    mNumCheckPointLocators = 0;

    GameplayManager::Finalize();
}

//=============================================================================
// SuperSprintManager::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::Update( unsigned int milliseconds )
{
    int oldTimeSecs;
    int newTimeSecs;

    //Test the flags
    if ( mFFlagTimeout > 0 )
    {
        if ( mFFlagTimeout <= milliseconds )
        {
            mFFlag->ShouldRender( false );
            mFFlagTimeout = 0;
        }
        else
        {
            mFFlagTimeout -= milliseconds;
            mFFlag->Update( milliseconds );
        }
    }

    if ( mWFlagTimeout > 0 )
    {
        if ( mWFlagTimeout <= milliseconds )
        {
            mWFlag->ShouldRender( false );
            mWFlagTimeout = 0;
        }
        else
        {
            mWFlagTimeout -= milliseconds;
            mWFlag->Update( milliseconds );
        }
    }

    UpdatePositionIcons( milliseconds );

    switch ( mCurrentState )
    {
    case COUNT_DOWN:
        {
            // 3 - 2 - 1 - GO!
            
            oldTimeSecs = mCountDownTime / 1000;

            if ( mCountDownTime -  static_cast<int>(milliseconds) <= 0 )
            {
                mCountDownTime = 0;
            }
            else
            {
                mCountDownTime -= milliseconds;
            }

            if ( mCountDownTime > 2000 )
            {
                mDrawable->SetCountDownMSG( "3" );
            }
            else if ( mCountDownTime > 1000 )
            {
                mDrawable->SetCountDownMSG( "2" );
                mDrawable->SetTextScale( 1.5f );
            }
            else if ( mCountDownTime > 0 )
            {
                mDrawable->SetCountDownMSG( "1" );
                mDrawable->SetTextScale( 2.0f );
            }
            else
            {
                mDrawable->SetCountDownMSG( "GO!" );
                mDrawable->SetTextScale( 2.5f );

                //Leave the go up for a little while.
                mCountDownTime = 500;
                mCurrentPosition = 1;
                mNumHumansFinished = 0;

                // Temp structures for turbo-on-start hack
                int aiIndices[ 3 ];
                int numAIs = 0;

                int i;
                for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
                {
                    int controllerID = GetInputManager()->GetControllerIDforPlayer( i );
                    if( controllerID != -1 )
                    {
                        GetInputManager()->GetController( controllerID )->SetGameState( Input::ACTIVE_ALL );
                        GetInputManager()->GetController( controllerID )->SetRumble( GetInputManager()->IsRumbleEnabled() );
                    }
                    else
                    {
                        if( mVehicleSlots[ i ].mVehicleAI != NULL )
                        {
                            mVehicleSlots[ i ].mVehicleAI->SetActive( true );

                            // Remember this AI index for turbo-on-start hack just below..
                            aiIndices[ numAIs ] = i;
                            numAIs++;
                        }
                    }
                }

                /////////////////////////
                // HACK: 
                // For each AI, it has a "m-in-n" chance to turbo at the start.
                // This helps to avoid cluttering at the start, as well as adds 
                // life to the game.
                //
                const int MAX_AIS_DOING_TURBO = numAIs - 1;
                int numAIsDoingTurbo = 0;

                for( int j=0; j<numAIs; j++ )
                {
                    if( numAIsDoingTurbo < MAX_AIS_DOING_TURBO )
                    {
                        int coinflip = rand() % 7;
                        if( coinflip == 0 || coinflip == 1 || coinflip == 2 )
                        {
                            rAssert( 0 <= aiIndices[j] && aiIndices[j] < SuperSprintData::NUM_PLAYERS );
                            mVehicleSlots[ aiIndices[j] ].mVehicleAI->UseTurbo();
                            numAIsDoingTurbo++;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                // If more than one AI, at least ONE AI will need to do turbo
                // so there's no cluttering near the start...
                if( numAIs > 1 && numAIsDoingTurbo == 0 )
                {
                    int randAI = rand() % numAIs;
                    rAssert( 0 <= aiIndices[randAI] && aiIndices[randAI] < SuperSprintData::NUM_PLAYERS );
                    mVehicleSlots[ aiIndices[randAI] ].mVehicleAI->UseTurbo();
                }
                //////////////////////////////



                GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );

                //record the starting time
                mStartTime = GET_TIME;
                SetState( RACING );
            }

            //
            // Reuse the menu select sound for a countdown sound
            //
            newTimeSecs = mCountDownTime / 1000;
            if( oldTimeSecs != newTimeSecs )
            {
                GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );
            }

            break;
        }
    case RACING:
        {
            //Watch to see who crosses the finish line first!
            if ( mCountDownTime - static_cast<int>(milliseconds) <= 0 )
            {
                mDrawable->SetRenderState( SuperSprintDrawable::NONE );
                mDrawable->SetTextScale( 1.0f );
                mCountDownTime = 0;
            }
            else
            {
                mCountDownTime -= milliseconds;
            }

            break;
        }
    case DNF_TIMEOUT:
        {
           oldTimeSecs = mCountDownTime / 1000;
           if ( mCountDownTime - static_cast<int>(milliseconds) <= 0 )
            {
                //mDrawable->SetRenderState( SuperSprintDrawable::PLAYER_DATA );
                //mDrawable->SetTextScale( 1.0f );

                GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );

                SetState( WINNER_CIRCLE );
                mCountDownTime = 0;

                mDrawable->SetRenderState( SuperSprintDrawable::NONE );

                DisableAllAI();
                DisableAllControllers();
            }
            else
            {
                mCountDownTime -= milliseconds;
            }

            sprintf( mTime, "%d", mCountDownTime / 1000 );

            //
            // Reuse the menu select sound for a countdown sound
            //
            newTimeSecs = mCountDownTime / 1000;
            if( oldTimeSecs != newTimeSecs )
            {
                GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );
            }

            break;
        }
    case WINNER_CIRCLE:
        {
            //Display the winner!
            GetInputManager()->SetGameState( Input::ACTIVE_SS_GAME );

            // go to Race Summary screen
            //
            GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->Suspend();

            GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_MINI_SUMMARY,
                                        0, 0, CLEAR_WINDOW_HISTORY );

            //Calculate your score
            int i;
            for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
            {
                if ( mPositions[ i ] == 1 )
                {
                    mPlayers[ i ].mWins++;

                    //
                    // We've found a winner.  Send the win/lose sound events
                    //
                    if( mVehicleSlots[i].mIsHuman )
                    {
                        GetEventManager()->TriggerEvent( EVENT_SUPERSPRINT_WIN );
                    }
                    else
                    {
                        GetEventManager()->TriggerEvent( EVENT_SUPERSPRINT_LOSE );
                    }
                }

                if ( mPlayers[ i ].mPosition == 0 )
                {
                    mPlayers[ i ].mPoints += 0;  //Joel made me set it to 0
                }
                else
                {
                    mPlayers[ i ].mPoints += 5 - mPositions[ i ];
                }
            }

            SetState( IDLE );

            break;
        }
    default:
        {
            break;
        }
    }
}

//=============================================================================
// SuperSprintManager::LoadScriptData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::LoadScriptData()
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );  // I choose other because this stuff will exist longer than a single mission

    char name[64];
    sprintf( name, "scripts\\ssi.mfk" );

    GetMissionScriptLoader()->SetFileHander( FILEHANDLER_LEVEL );
    GetMissionScriptLoader()->LoadScriptAsync( name );

    //Also load the carstarts and stuff.
    sprintf( name, "art\\B0%ddata.p3d", GetCurrentLevelIndex() );  //Hackish
    GetLoadingManager()->AddRequest( FILEHANDLER_LEVEL, name, GMA_LEVEL_OTHER );

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

    //MOve this here so the car con files get loaded in time.
    SetUpCars();

    GetLoadingManager()->AddCallback( this );
}

//=============================================================================
// SuperSprintManager::StartRace
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::StartRace()
{
    PositionCars();
    PositionAI();
    InitRaceData();
    PlaceCharactersInCars();  //TODO: texas scramble

    mCountDownTime = 5000;
    mDrawable->SetRenderState( SuperSprintDrawable::COUNT_DOWN );
    mDrawable->SetTextScale( 1.0f );

    InitCamera();
    SetupTraps();  //Only do this once.
    PlayIntroCam();

    SetState( COUNT_DOWN );
}

//=============================================================================
// SuperSprintManager::SetCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, const char* name )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::SetCharacter( int playerID, int index )
{
    mPlayers[ playerID ].mCharacterIndex = index;
}

//=============================================================================
// SuperSprintManager::SetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, const char* name )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::SetVehicle( int playerID, const char* name )
{
    rAssert( playerID >= 0 && playerID < SuperSprintData::NUM_PLAYERS );

    strncpy( mVehicleSlots[ playerID ].mCarName, name, sizeof( mVehicleSlots[ playerID ].mCarName ) );
}

//=============================================================================
// SuperSprintManager::FindLeader
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      char
//
//=============================================================================
char SuperSprintManager::FindLeader()
{
    char leader = -1;
    unsigned char numLaps = 0;
    char nextCheckpoint = -1;

    //For now just figure out who's in the lead.
    char i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if ( mPlayers[ i ].mNumLaps > numLaps )
        {
            //This guy is in the lead.
            leader = i;
            numLaps = mPlayers[ i ].mNumLaps;
            nextCheckpoint = mPlayers[ i ].mNextCheckPoint;
        }
        else if ( mPlayers[ i ].mNumLaps == numLaps )
        {
            if ( mPlayers[ i ].mNextCheckPoint > nextCheckpoint )
            {
                //This guys is in the lead.
                leader = i;
                numLaps = mPlayers[ i ].mNumLaps;
                nextCheckpoint = mPlayers[ i ].mNextCheckPoint;
            }
        }       
    }

    return leader;
}

//=============================================================================
// SuperSprintManager::CalculatePositions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::CalculatePositions()
{
#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
    char errMsg[128];
#endif

    int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        if ( mPlayers[ i ].mNumLaps == mNumLaps )
        {
            //No more updates for this guy.
            continue;
        }

        if ( !mVehicleSlots[ i ].mIsHuman )
        {
            WaypointAI* ai = mVehicleSlots[ i ].mVehicleAI;

            rmt::Vector aiPos;
            ai->GetPosition( &aiPos );

            /*
            // reset as necessary.
            rmt::Sphere aiSphere;
            ai->GetVehicle()->GetBoundingSphere( &aiSphere );

            SuperCam* superCam = GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
            rAssert( superCam );

            if( superCam->GetType() == SuperCam::SUPER_SPRINT_CAM && 
                !superCam->GetCamera()->SphereVisible( aiPos, aiSphere.radius ) )
            {
                ai->GetVehicle()->ResetOnSpot( false );
            }
            */


            //////////////////////////////////////////////////////
            // Query the road manager to get the distance
            RoadManager::PathElement aiElem; 
            aiElem.elem = NULL;
            RoadSegment* aiSeg = NULL;
            float aiSegT = 0.0f;
            float aiRoadT = 0.0f;

            ai->GetRacePathInfo( aiElem, aiSeg, aiSegT, aiRoadT );


            // make sure the AI is on a path element
#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
            sprintf( errMsg, "Something wrong with player %d!\n", i+1 );
            rTuneAssertMsg( aiElem.elem != NULL, errMsg );
            rTuneAssertMsg( 0 <= mPlayers[ i ].mNextCheckPoint && 
                mPlayers[ i ].mNextCheckPoint < GetNumCheckpoints(), errMsg );
#endif
            char nextCheckpoint = mCheckPoints[ mPlayers[ i ].mNextCheckPoint ];

            int path = GetPathDataWith( GetCheckpointWith( nextCheckpoint ) );
            rAssert( path != -1 );

            RoadManager::PathElement collectibleElem = mPathData[ path ].closestElem;
            float collectibleRoadT = mPathData[ path ].roadT;
            
            rmt::Vector collectiblePos;
            mPathData[ path ].loc->GetLocation( &collectiblePos );

            // make sure the collectible is on a path element
            rAssert( collectibleElem.elem != NULL );

            float aiDistToColl = NEAR_INFINITY;
            if( aiElem.elem != NULL && collectibleElem.elem != NULL )
            {
                SwapArray<RoadManager::PathElement> dummy;

                HeapMgr()->PushHeap(GMA_TEMP);
                dummy.Allocate( RoadManager::GetInstance()->GetMaxPathElements() );
                HeapMgr()->PopHeap(GMA_TEMP);

                aiDistToColl = RoadManager::GetInstance()->FindPathElementsBetween(
                    false,
                    aiElem, aiRoadT, aiPos,
                    collectibleElem, collectibleRoadT, collectiblePos,
                    dummy );
            }

            ai->SetDistToCurrentCollectible( aiDistToColl );
            mPlayers[ i ].mDistToCheckpoint = aiDistToColl;
        }
        else
        {
            ////////////////////////////////////////////////////////
            // Find out player's dist to mNextCollectible
            //
            Avatar* player = GetAvatarManager()->GetAvatarForPlayer( i );
            rAssert( player );

            rmt::Vector playerPos;
            player->GetPosition( playerPos );

            /*
            // if player has gone out of bounds, reset to nearest segment
            rmt::Sphere playerSphere;
            player->GetVehicle()->GetBoundingSphere( &playerSphere );

            SuperCam* superCam = GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
            rAssert( superCam );

            if( superCam->GetType() == SuperCam::SUPER_SPRINT_CAM && 
                !superCam->GetCamera()->SphereVisible( playerPos, playerSphere.radius ) )
            {
                player->GetVehicle()->ResetOnSpot( false );
                player->mHasBeenUpdatedThisFrame = false;
            }
            */


            RoadManager::PathElement playerElem;
            playerElem.elem = NULL;
            RoadSegment* playerSeg = NULL;
            float playerSegT = 0.0f;
            float playerRoadT = 0.0f;

            player->GetLastPathInfo( playerElem, playerSeg, playerSegT, playerRoadT );


#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
            rTuneAssertMsg( playerElem.elem != NULL, errMsg );
            rTuneAssertMsg( 0 <= mPlayers[ i ].mNextCheckPoint && 
                mPlayers[ i ].mNextCheckPoint < GetNumCheckpoints(), errMsg );
#endif

            char nextCheckpoint = mCheckPoints[ mPlayers[ i ].mNextCheckPoint ];

            int path = GetPathDataWith( GetCheckpointWith( nextCheckpoint ) );
            rAssert( path != -1 );

            RoadManager::PathElement collectibleElem = mPathData[ path ].closestElem;
            float collectibleRoadT = mPathData[ path ].roadT;

            rmt::Vector collectiblePos;
            mPathData[ path ].loc->GetLocation( &collectiblePos );

            // make sure the collectible is on a path element
            rAssert( collectibleElem.elem != NULL );

            float playerDistToCurrCollectible = NEAR_INFINITY;
            if( playerElem.elem != NULL && collectibleElem.elem != NULL )
            {
                SwapArray<RoadManager::PathElement> dummy;

                HeapMgr()->PushHeap(GMA_TEMP);
                dummy.Allocate( RoadManager::GetInstance()->GetMaxPathElements() );
                HeapMgr()->PopHeap(GMA_TEMP);

                playerDistToCurrCollectible = RoadManager::GetInstance()->FindPathElementsBetween(
                    false,
                    playerElem, playerRoadT, playerPos,
                    collectibleElem, collectibleRoadT, collectiblePos,
                    dummy );
            }
            
            mPlayers[ i ].mDistToCheckpoint = playerDistToCurrCollectible;
        }
    }

    ////////////////////////////////////////////////////////////////////
    // Update my position 
    //

    unsigned int j;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if ( mPlayers[ i ].mNumLaps == mNumLaps )
        {
            //No more updates for this guy.
            continue;
        }

        unsigned int numInFront = 0;
        unsigned int vehiclesWithMe[ SuperSprintData::NUM_PLAYERS ];
        unsigned int numWithMe = 0;
        unsigned int numFinished = 0;

        int whichOtherPlayer = (i + 1) % SuperSprintData::NUM_PLAYERS;

        for( j = 0; j < SuperSprintData::NUM_PLAYERS - 1; ++j )
        {
            int numLapsCompleted = mPlayers[ whichOtherPlayer ].mNumLaps;

            if ( numLapsCompleted == mNumLaps )
            {
                //They're finshed racing
                numFinished++;
            }
            else if ( numLapsCompleted > mPlayers[ i ].mNumLaps )
            {
                //This guys finished more laps than I.
                numInFront++;         
            }
            else if ( numLapsCompleted == mPlayers[ i ].mNumLaps )
            {
                //We're racing the same lap number.
                char currCollectible = mPlayers[ whichOtherPlayer ].mNextCheckPoint;
                char playerCollectible = mPlayers[ i ].mNextCheckPoint;

                if( currCollectible > playerCollectible )
                {
                    numInFront++;
                }
                else if( currCollectible == playerCollectible )
                {
                    vehiclesWithMe[ numWithMe ] = whichOtherPlayer;
                    numWithMe++;
                }
            }

            whichOtherPlayer = ( whichOtherPlayer + 1 ) % SuperSprintData::NUM_PLAYERS;
        }

        mPositions[ i ] = 1 + numInFront + numFinished;

        //Now, who is actually in front of me going to the same waypoint?
        const Locator* waypointLoc = GetCheckpointWith( mCheckPoints[ mPlayers[ i ].mNextCheckPoint ] );
        if ( !waypointLoc )
        {
            rAssert( false );  //Why?  Because we've not started to race yet.
            return;
        }

        ////////////////////////////////////////////////////////////
        // The last set of vehicles to consider are the ones that
        // are headed to the same collectible (race checkpoint) and
        // are in the same lap as we are... 
        //
        for( j = 0; j < numWithMe; ++j )
        {
            int index = vehiclesWithMe[ j ];

            //This guy and I are racing for the same collectible
            //Test the dist to the collectible.

            float aiDistToCurrCollectible = mPlayers[ index ].mDistToCheckpoint;
            float playerDistToCurrCollectible = mPlayers[ i ].mDistToCheckpoint;

            if( aiDistToCurrCollectible < playerDistToCurrCollectible )
            {
                // blast! he's ahead of me... my pos is thus bumped even lower...
                mPositions[ i ] = mPositions[ i ] + 1;
            }
        }
    }
}

//=============================================================================
// SuperSprintManager::LoadLevelData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::LoadLevelData()
{
    char name[64];
    sprintf( name, "scripts\\ss.mfk" );

    BillboardWrappedLoader::OverrideLoader( true );

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );  // I choose other because this stuff will exist longer than a single mission

    GetMissionScriptLoader()->SetFileHander( FILEHANDLER_LEVEL );
    GetMissionScriptLoader()->LoadScriptAsync( name );

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

    BillboardWrappedLoader::OverrideLoader( false );
}

//=============================================================================
// SuperSprintManager::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::Reset()
{
    //Reset the same race with all the existing cars and stuff.
    PositionCars();
    PositionAI();
    ResetRaceData();

    mCountDownTime = 5000;
    mDrawable->SetRenderState( SuperSprintDrawable::COUNT_DOWN );
    mDrawable->SetTextScale( 1.0f );

    InitCamera();
    PlayIntroCam();

    SetState( COUNT_DOWN );    
}

//=============================================================================
// SuperSprintManager::OnButton
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerId, int id, const Button* pButton )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::OnButton( int controllerId, int id, const Button* pButton )
{
}

//=============================================================================
// SuperSprintManager::OnButtonUp
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerId, int buttonId, const Button* pButton )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::OnButtonUp( int controllerId, int buttonId, const Button* pButton )
{
    if ( buttonId == ShowPositions )
    {
        int playerID = GetInputManager()->GetControllerPlayerIDforController( controllerId );

        if ( playerID >= 0 && playerID < SuperSprintData::NUM_PLAYERS )
        {
            mTogglePosition[ playerID ] = false;
        }
    }
}

//=============================================================================
// SuperSprintManager::OnButtonDown
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerId, int buttonId, const Button* pButton )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::OnButtonDown( int controllerId, int buttonId, const Button* pButton )
{
    switch ( mCurrentState )
    {
    case RACING:
        {
            int playerID = GetInputManager()->GetControllerPlayerIDforController( controllerId );
            if ( playerID == -1 )
            {
                playerID = controllerId;

                rAssert( playerID < SuperSprintData::NUM_PLAYERS );
                if( playerID >= SuperSprintData::NUM_PLAYERS )
                {
                    return;
                }
            }

            int controllerIDPlayer = GetInputManager()->GetControllerIDforPlayer( playerID );
            if ( buttonId == CamSelect && mVehicleSlots[ playerID ].mIsHuman && !GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->IsSuspended())
            {
                unsigned int playerCount = 0;
                int i;
                for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
                {
                    if ( mVehicleSlots[ i ].mIsHuman )
                    {
                        ++playerCount;
                    }
                }

                if ( playerCount == 1 )
                {
                    GetSuperCamManager()->GetSCC( 0 )->SetTarget( mVehicleSlots[ playerID ].mVehicle );

                    if ( GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam()->GetType() == SuperCam::WRECKLESS_CAM )
                    {
                        //Skip the animated cams.
                        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( (unsigned int)0 );
                    }
                    else
                    {
                        GetSuperCamManager()->GetSCC( 0 )->ToggleSuperCam( true );
                    }
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }

    if ( buttonId == ShowPositions )
    {
        int playerID = GetInputManager()->GetControllerPlayerIDforController( controllerId );

        if ( playerID >= 0 && playerID < SuperSprintData::NUM_PLAYERS )
        {
            mTogglePosition[ playerID ] = true;
        }
    }
}

//=============================================================================
// SuperSprintManager::LoadControllerMappings
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int controllerId )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::LoadControllerMappings( unsigned int controllerId )
{
    if ( GetInputManager()->GetController( controllerId )->IsConnected() )
    {
#ifdef RAD_XBOX
        ClearMap(0);
        Map( "Start", Start, 0, controllerId );
        Map( "A", Select, 0, controllerId );
        Map( "B", Back, 0, controllerId );
        Map( "DPadRight", Right, 0, controllerId );
        Map( "DPadLeft", Left, 0, controllerId );
        Map( "LeftStickX", StickX, 0, controllerId );
        Map( "LeftTrigger", L1, 0, controllerId );
        Map( "Black", CamSelect, 0, controllerId );
        Map( "Y", ShowPositions, 0, controllerId );
#endif

#ifdef RAD_PS2
        ClearMap(0);
        Map( "Start", Start, 0, controllerId );
        Map( "X", Select, 0, controllerId );
        Map( "Triangle", Back, 0, controllerId );
        Map( "DPadRight", Right, 0, controllerId );
        Map( "DPadLeft", Left, 0, controllerId );
        Map( "LeftStickX", StickX, 0, controllerId );
        Map( "L1", L1, 0, controllerId );
        Map( "Select", CamSelect, 0, controllerId );
        Map( "Triangle", ShowPositions, 0, controllerId );
#endif

#ifdef RAD_GAMECUBE
        ClearMap(0);
        Map( "Menu", Start, 0, controllerId );
        Map( "A", Select, 0, controllerId );
        Map( "B", Back, 0, controllerId );
        Map( "DPadRight", Right, 0, controllerId );
        Map( "DPadLeft", Left, 0, controllerId );
        Map( "LeftStickX", StickX, 0, controllerId );
        Map( "AnalogTriggerL", L1, 0, controllerId );
        Map( "DPadDown", CamSelect, 0, controllerId );
        Map( "Y", ShowPositions, 0, controllerId );
#endif

#ifdef RAD_WIN32
        ClearMap(0);
        Map( "Pause", Start, 0, controllerId );
        Map( "Attack", Select, 0, controllerId );
        Map( "Jump", Back, 0, controllerId );
        Map( "MoveRight", Right, 0, controllerId );
        Map( "MoveLeft", Left, 0, controllerId );
        Map( "MoveX", StickX, 0, controllerId );
        //Map( "LeftTrigger", L1, 0, controllerId );
        Map( "CameraToggle", CamSelect, 0, controllerId );
#endif
    }
}

//=============================================================================
// SuperSprintManager::OnControllerConnect
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::OnControllerConnect( int id )
{
    int playerID = GetInputManager()->GetControllerPlayerIDforController( id );

    if ( playerID != -1 && mVehicleSlots[ playerID ].mState == SuperSprintData::CarData::SELECTED && 
         static_cast<int>( mPlayers[ playerID ].mNumLaps ) < mNumLaps && 
         mVehicleSlots[ playerID ].mIsHuman &&
         (mCurrentState == RACING || mCurrentState == DNF_TIMEOUT) )
    {
        GetInputManager()->GetController( id )->SetGameState( Input::ACTIVE_ALL );
        GetInputManager()->GetController( id )->SetRumble( GetInputManager()->IsRumbleEnabled() );
    }
    else
    {
        GetInputManager()->GetController( id )->SetGameState( Input::ACTIVE_SS_GAME );
    }
}

//=============================================================================
// SuperSprintManager::OnControllerDisconnect
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::OnControllerDisconnect( int id )
{
}

//=============================================================================
// SuperSprintManager::GetNumCheckpoints
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      char 
//
//=============================================================================
char SuperSprintManager::GetNumCheckpoints()
{
    if ( mNumCheckPoints == 0 )
    {
        if ( (RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B02 ||
             (RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B07 )
        {
            if ( mGoLeft )
            {
                mCheckPoints = CHECKPOINT_LIST_B02_R;
            }
            else
            {
                mCheckPoints = CHECKPOINT_LIST_B02;
            }

            mNumCheckPoints = sizeof( CHECKPOINT_LIST_B02 ) / sizeof( char );
        }
        else if ( (RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B05 )
        {
            if ( mGoLeft )
            {
                mCheckPoints = CHECKPOINT_LIST_B05_R;
            }
            else
            {
                mCheckPoints = CHECKPOINT_LIST_B05;
            }

            mNumCheckPoints = sizeof( CHECKPOINT_LIST_B05 ) / sizeof( char );
        }
        else if ( (RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B01 )
        {
            if ( mGoLeft )
            {
                mCheckPoints = CHEKPOINT_LIST_B01_R;
            }
            else
            {
                mCheckPoints = CHEKPOINT_LIST_B01;
            }

            mNumCheckPoints = sizeof( CHEKPOINT_LIST_B01 ) / sizeof( char );
        }
        else if ( ((RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B03 ) ||
                  ((RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B04 )||
                  ((RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B06 ) )
        {
            if ( mGoLeft )
            {
                mCheckPoints = CHECKPOINT_LIST_B03_R;
            }
            else
            {
                mCheckPoints = CHECKPOINT_LIST_B03;
            }

            mNumCheckPoints = sizeof( CHECKPOINT_LIST_B03 ) / sizeof( char );
        }
        else
        {
            if ( mGoLeft )
            {
                mCheckPoints = CHECKPOINT_LIST_R;
            }
            else
            {
                mCheckPoints = CHECKPOINT_LIST;
            }

            mNumCheckPoints = sizeof( CHECKPOINT_LIST ) / sizeof( char );
        }
    }

    return mNumCheckPoints;
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperSprintManager::SetUpCars
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::SetUpCars()
{
    unsigned int conFileIndex = 1;

    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if ( mVehicleSlots[ i ].mState == SuperSprintData::CarData::SELECTED )
        {
            mNumActivePlayers++;
            char* carName = mVehicleSlots[i].mCarName; //Why does this have to be non-const?

            if( mVehicleSlots[ i ].mIsHuman )
            {
                mNumHumanPlayers++;
                mVehicleSlots[ i ].mVehicle = GetVehicleCentral()->InitVehicle( carName, true, NULL, VT_USER, 
                                                                                VehicleCentral::FORCE_NO_DRIVER,
                                                                                false,   // playercar - I think this is chucks thing for the character sheet
                                                                                false);  // one of the few rare cases where we start in the car
            }
            else
            {
                char scriptName[64];
                sprintf( scriptName, "bonus/bg%d_%d.con", GetCurrentLevelIndex(), conFileIndex );
                mVehicleSlots[ i ].mVehicle = GetVehicleCentral()->InitVehicle( carName, true, scriptName, VT_AI, VehicleCentral::FORCE_NO_DRIVER, false, false );  // see comments above
                ++conFileIndex;
            }
            
            rAssert( mVehicleSlots[ i ].mVehicle );

            mVehicleSlots[ i ].mVehicle->AddRef();

            int added = GetVehicleCentral()->AddVehicleToActiveList( mVehicleSlots[ i ].mVehicle );
            rAssert( added != -1 );

            mVehicleSlots[ i ].mActiveListIndex = added;
        }

        if( mVehicleSlots[ i ].mVehicle != NULL )
        {
            GetSuperCamManager()->GetSCC( i )->SetTarget( mVehicleSlots[ i ].mVehicle );
            GetSoundManager()->LoadCarSound( mVehicleSlots[ i ].mVehicle, false );
        }
    }
}

//=============================================================================
// SuperSprintManager::CleanUpCars
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::CleanUpCars()
{
    int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if ( mVehicleSlots[ i ].mVehicle != NULL )
        {
            // drop character out of the car...
            Character* playerCharacter = GetAvatarManager()->GetAvatarForPlayer( i )->GetCharacter();
            //PlaceCharacterAtLocator( playerCharacter, this->m );
            GetAvatarManager()->PutCharacterOnGround( playerCharacter, mVehicleSlots[ i ].mVehicle );

            GetVehicleCentral()->RemoveVehicleFromActiveList( mVehicleSlots[ i ].mVehicle );

            mVehicleSlots[ i ].mVehicle->Release();
            mVehicleSlots[ i ].mVehicle = NULL;
        }
        if( mVehicleSlots[ i ].mVehicleAI != NULL )
        {
            rAssert( mVehicleSlots[ i ].mActiveListIndex != -1 );
            GetVehicleCentral()->SetVehicleController( mVehicleSlots[ i ].mActiveListIndex, NULL );
			mVehicleSlots[ i ].mVehicleAI->SetActive( false );
			mVehicleSlots[ i ].mVehicleAI->Finalize();
			mVehicleSlots[ i ].mVehicleAI->Release();
			mVehicleSlots[ i ].mVehicleAI = NULL;
		}
        mVehicleSlots[ i ].mActiveListIndex = -1;

    }

    //Dump the car geo.
    p3d::pddi->DrawSync();
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        p3d::inventory->RemoveSectionElements( mVehicleSlots[ i ].mCarName );
        p3d::inventory->DeleteSection( mVehicleSlots[ i ].mCarName );
    }

    mNumActivePlayers = 0;
    mNumHumanPlayers = 0;
    mNumHumansFinished = 0;
}

//=============================================================================
// SuperSprintManager::PositionCars
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::PositionCars()
{
    unsigned int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        // figure out the start position ... 
        char carlocator[32];
        sprintf( carlocator, "car%d", i + 1 );

        CarStartLocator* loc = p3d::find<CarStartLocator>(carlocator);
        rAssert( loc );

        if ( loc )
        {
            rmt::Vector pos;
            float facing;
            loc->GetLocation( &pos );
            facing = loc->GetRotation();

            if ( mGoLeft )
            { 
                //We go Left.
                facing += rmt::PI;
            }

            rAssert( mVehicleSlots[ i ].mVehicle );
            mVehicleSlots[ i ].mVehicle->SetInitialPosition( &pos );
            mVehicleSlots[ i ].mVehicle->SetResetFacingInRadians( facing );
            mVehicleSlots[ i ].mVehicle->Reset();
        }
    }

    DisableAllControllers();
}

//=============================================================================
// SuperSprintManager::PositionAI
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::PositionAI()
{
    ///////////////////////////////////////////////////
    // Go through all the waypoints, accumulating them...
    //
    Locator* waypoints[ MAX_AI_WAYPOINTS ];
    int nWaypoints = 0;

    // can either drive left around the track or right around the track
    char waypointDir[4];
    if( mGoLeft ) 
    {
        sprintf( waypointDir, "WPL" );
    }
    else
    {
        sprintf( waypointDir, "WPR" );
    }

    // iterate from 1 to MAX_AI_WAYPOINTS (inclusive)
    char waypointName[8];
    unsigned int i;
    for( i = 1; i <= MAX_AI_WAYPOINTS; ++i )
    {   
        if( i < 10 )
        {
            sprintf( waypointName, "%s0%d", waypointDir, i );
        }
        else
        {
            rAssert( i < 100 ); // keep it to 2 digits
            sprintf( waypointName, "%s%d", waypointDir, i );
        }

        waypointName[5] = '\0';
        Locator* wayloc = p3d::find<Locator>( waypointName );

        if( wayloc == NULL )
        {
            // well... no more waypoints, just quit
            break;
        }

        waypoints[ nWaypoints ] = wayloc;
        nWaypoints++;                
    }

    if( nWaypoints <= 0 )
    {
        rDebugPrintf( "SUPERSPRINT WARNING: Uh... We didn't see any waypoints"
            "for AI. AI-controlled vehicles will not budge.\n" );
    }

    mNumCheckPointLocators = 0;

    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        // Put in the AI here... 
        const float smallTriggerRadius = 7.0f;
        if( !mVehicleSlots[ i ].mIsHuman )
        {
            if ( mVehicleSlots[ i ].mVehicleAI == NULL )
            {
                mVehicleSlots[ i ].mVehicleAI = new WaypointAI( mVehicleSlots[ i ].mVehicle, false, smallTriggerRadius, true );  // indicate false to segment optimization
                mVehicleSlots[ i ].mVehicleAI->SetUseMultiplier( false ); // no traffic in supersprint... no need for multiplier
                mVehicleSlots[ i ].mVehicleAI->AddRef(); // Corresponding call to Release() will already call delete if refcount<=1
            }
            else
            {
                //Cleanup to restart
                mVehicleSlots[ i ].mVehicleAI->Finalize();

            }

            mVehicleSlots[ i ].mVehicleAI->Initialize();

            // Just before this, we obtained a list of waypoints...
            // Add them to the AIs here
            for( int j=0; j<nWaypoints; j++ )
            {
                static_cast<WaypointAI*>(mVehicleSlots[ i ].mVehicleAI)->AddWaypoint( waypoints[j] );
            }
        }
    }

    DisableAllAI();

    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( "Level" );

    HeapMgr()->PushHeap( GMA_TEMP );
    tInventory::Iterator<EventLocator> it( p3d::inventory );

    EventLocator* loc = it.First();

    while( loc != NULL )
    {
        if ( loc->GetEventType() == LocatorEvent::CHECK_POINT && loc->GetData() > 0 )
        {
            mCheckPointLocators[ mNumCheckPointLocators ] = loc;
            //mCheckPointLocators[ mNumCheckPointLocators ]->AddRef();
            ++mNumCheckPointLocators;
        }



        loc = it.Next();
    }

    HeapMgr()->PopHeap( GMA_TEMP );
    p3d::inventory->PopSection();

    for( i = 0; i < mNumCheckPointLocators; i++ )
    {
        rAssert( mCheckPointLocators[ i ] != NULL );

        rmt::Vector locPos;
        mCheckPointLocators[ i ]->GetPosition( &locPos );

        //////////////////////////////////////////////////
        // Get what the locator was placed on & other info
        //
        // NOTE: We assume that a collectible isn't going to move..
        //       and that it's always either on a road segment or 
        //       an intersection... This is safe to do because
        //       the information below is only going to be used
        //       for race objectives (a subclass of collectibleobjective)
        //       which requires that its collectibles don't move around.
        //       It's not that bad to fix it if this changes... we
        //       will simply need to re-invoke FindClosestPathElement
        //       every frame, as the collectible moves around and 
        //       remember the last valid (non-off-road) values. 
        //

        RoadSegment* seg = NULL;
        float segT = 0.0f;
        float roadT = 0.0f;
        RoadManager::PathElement closestElem;
        closestElem.elem = NULL;

        bool succeeded = VehicleAI::FindClosestPathElement( locPos, closestElem, seg, segT, roadT, true );
        if( !succeeded )
        {
            char msg[512];
            sprintf( msg, "Locator at (%0.1f,%0.1f,%0.1f) must either be placed on a roadsegment "
                "or in an intersection! Woe be the designer who placed down this locator! "
                "For now, the closest road segment will be chosen instead.\n", 
                locPos.x, locPos.y, -1 * locPos.z );
            rAssertMsg( false, msg );

            RoadSegment* closestSeg = NULL;
            float dummy;
            GetIntersectManager()->FindClosestRoad( locPos, 100.0f, closestSeg, dummy );

            seg = (RoadSegment*) closestSeg;
            segT = RoadManager::DetermineSegmentT( locPos, seg );
            roadT = RoadManager::DetermineRoadT( seg, segT );
            closestElem.elem = seg->GetRoad();
            closestElem.type = RoadManager::ET_NORMALROAD;
        }

        mPathData[ i ].closestElem = closestElem;
        mPathData[ i ].seg = seg;
        mPathData[ i ].segT = segT;
        mPathData[ i ].roadT = roadT;
        mPathData[ i ].loc = mCheckPointLocators[ i ];

        rAssert( mPathData[ i ].closestElem.elem != NULL );
    }
}

//=============================================================================
// SuperSprintManager::DisableAllAI
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::DisableAllAI()
{
    unsigned int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if ( !mVehicleSlots[ i ].mIsHuman )
        {
            mVehicleSlots[ i ].mVehicleAI->SetActive( false );
        }
    }
}

//=============================================================================
// SuperSprintManager::DisableAllControllers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::DisableAllControllers()
{
    GetInputManager()->SetGameState( Input::ACTIVE_SS_GAME );

    unsigned int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        int controllerID = GetInputManager()->GetControllerIDforPlayer( i );
        if( controllerID != -1 )
        {
            GetInputManager()->SetRumbleForDevice( controllerID, false );
        }
    }
}

//=============================================================================
// SuperSprintManager::InitRaceData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::InitRaceData()
{
    unsigned int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        //Reset the player race data.
        mPlayers[ i ].mBestLap = 0xffffffff;
        mPlayers[ i ].mBestLapEntry = 0;
        mPlayers[ i ].mBestTimeEntry = 0;
        mPlayers[ i ].mLapTime = 0;
        mPlayers[ i ].mNextCheckPoint = 0;
        mPlayers[ i ].mNumLaps = 0;
        mPlayers[ i ].mPosition = 0;
        mPlayers[ i ].mWins = 0;
        mPlayers[ i ].mRaceTime = 0;
        mPlayers[ i ].mPoints = 0;
        mPlayers[ i ].mDistToCheckpoint = NEAR_INFINITY;

        mVehicleSlots[ i ].mVehicle->mNumTurbos = SuperSprintData::DEFAULT_TURBO_NUM;

        mPositions[ i ] = i;
    }
}

//=============================================================================
// SuperSprintManager::ResetRaceData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::ResetRaceData()
{
    unsigned int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        //Reset the player race data.
        mPlayers[ i ].mBestLap = 0xffffffff;
        mPlayers[ i ].mBestLapEntry = 0;
        mPlayers[ i ].mBestTimeEntry = 0;
        mPlayers[ i ].mLapTime = 0;
        mPlayers[ i ].mNextCheckPoint = 0;
        mPlayers[ i ].mNumLaps = 0;
        mPlayers[ i ].mPosition = 0;
        mPlayers[ i ].mRaceTime = 0;
        mPlayers[ i ].mDistToCheckpoint = NEAR_INFINITY;

        mVehicleSlots[ i ].mVehicle->mNumTurbos = SuperSprintData::DEFAULT_TURBO_NUM;

        mPositions[ i ] = i;
    }
}


//=============================================================================
// SuperSprintManager::PlaceCharactersInCars
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::PlaceCharactersInCars()
{
    int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if ( mVehicleSlots[ i ].mState == SuperSprintData::CarData::SELECTED )
        {
            GetAvatarManager()->PutCharacterInCar( GetAvatarManager()->GetAvatarForPlayer( i )->GetCharacter(), mVehicleSlots[i].mVehicle );
            if( !mVehicleSlots[ i ].mIsHuman )
            {
                GetVehicleCentral()->SetVehicleController( 
                    mVehicleSlots[ i ].mActiveListIndex, 
                    mVehicleSlots[ i ].mVehicleAI );
            }
        }
    }
}

//=============================================================================
// SuperSprintManager::StartLoadingCars
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::LoadCars()
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        // Force unjoined positions to be AI-controlled
        if( mVehicleSlots[ i ].mState == SuperSprintData::CarData::WAITING )
        {
            //Let's randomly pick a car.
            unsigned int carNum = rand() % SuperSprintData::NUM_NAMES;
            
            while ( strcmp( SuperSprintData::VEHICLE_NAMES[ carNum ].name, mVehicleSlots[ (i + 1) % SuperSprintData::NUM_PLAYERS ].mCarName ) == 0 ||
                    strcmp( SuperSprintData::VEHICLE_NAMES[ carNum ].name, mVehicleSlots[ (i + 2) % SuperSprintData::NUM_PLAYERS ].mCarName ) == 0 ||
                    strcmp( SuperSprintData::VEHICLE_NAMES[ carNum ].name, mVehicleSlots[ (i + 3) % SuperSprintData::NUM_PLAYERS ].mCarName ) == 0 )
            {
                carNum = rand() % SuperSprintData::NUM_NAMES;
            }

            SetVehicle( i, SuperSprintData::VEHICLE_NAMES[ carNum ].name );
            
            // transit to SELECTED, so that our car will get loaded, below
            mVehicleSlots[ i ].mState = SuperSprintData::CarData::SELECTED;
        }

        //Force undecided players to the current selection.
        if ( mVehicleSlots[ i ].mState == SuperSprintData::CarData::SELECTING )
        {
            mVehicleSlots[ i ].mState = SuperSprintData::CarData::SELECTED;
        }

        if ( mVehicleSlots[ i ].mState == SuperSprintData::CarData::SELECTED )
        {
            char fileName[64];
            sprintf( fileName, "art\\cars\\%s.p3d", mVehicleSlots[ i ].mCarName );
            p3d::inventory->AddSection( mVehicleSlots[ i ].mCarName );

            GetLoadingManager()->AddRequest( FILEHANDLER_LEVEL, 
                                             fileName,
                                             GMA_LEVEL_OTHER,
                                             mVehicleSlots[ i ].mCarName,
                                             mVehicleSlots[ i ].mCarName ); 
        }
    }

    //When we get this callback, the loading of all cars is done.
//    GetLoadingManager()->AddCallback( this );
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

    mDrawable->DoCountDownToo( false );
}

//=============================================================================
// SuperSprintManager::LoadCharacters
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::LoadCharacters()
{
    unsigned int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if ( !mVehicleSlots[ i ].mIsHuman )
        {
            //We need to select a character for this AI.
            unsigned int whichName = rand() % SuperSprintData::NUM_CHARACTER_NAMES;

            unsigned int j;
            for ( j = 0; j < SuperSprintData::NUM_CHARACTER_NAMES; ++j )
            {
                bool found = false;
                unsigned int k;
                for ( k = 0; k < SuperSprintData::NUM_PLAYERS && !found; ++k )
                {
                    if ( SuperSprintData::CHARACTER_NAMES[ mPlayers[ k ].mCharacterIndex ] != '\0' && 
                       ( mPlayers[ k ].mCharacterIndex == static_cast<int>( whichName ) ) )
                    {
                        //This name is taken.
                        found = true;
                        break;
                    }
                }

                if ( found )
                {
                    whichName = ( whichName + 1 ) % SuperSprintData::NUM_CHARACTER_NAMES;
                }
                else
                {
                    //We have a winner!
                    break;
                }
            }

            rAssert( j < SuperSprintData::NUM_CHARACTER_NAMES );

            SetCharacter( i, whichName );
        }

        GetCharacterManager()->AddPCCharacter( SuperSprintData::CHARACTER_NAMES[ mPlayers[ i ].mCharacterIndex ], "npd" );
    }
}

//=============================================================================
// SuperSprintManager::EnumerateControllers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::EnumerateControllers()
{
    //Set up the controllers.
    InputManager* im = GetInputManager();

    unsigned int i;

    for ( i = 0; i < Input::MaxControllers; ++i )
    {
        im->UnregisterMappable( i, this );
    }

    for ( i = 0; i < Input::MaxControllers; ++i )
    {
        int handle = im->RegisterMappable( i, this );
        rAssert( handle != -1 );
    }
}

//=============================================================================
// SuperSprintManager::SetupIcons
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::SetupIcons()
{
    //Get and init the two flags.
    mFFlag->Init( "fflag", rmt::Vector( 3.0f, 6.0f, -40.0f ), false, false );
    mFFlag->ScaleByCameraDistance( 1.0f, 3.0f, 10.0f, 100.0f );
    mWFlag->Init( "wflag", rmt::Vector( 1.8f, 6.0f, -40.0f ), false, false );
    mWFlag->ScaleByCameraDistance( 1.0f, 3.0f, 10.0f, 100.0f );

    int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        char name[64];
        sprintf( name, "MiniArrow%dr", i + 1 );  //Start at 1
        mPositionIcon[ 0 ][ i ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
        mPositionIcon[ 0 ][ i ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
        sprintf( name, "MiniArrow%db", i + 1 );  //Start at 1
        mPositionIcon[ 1 ][ i ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
        mPositionIcon[ 1 ][ i ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
        sprintf( name, "MiniArrow%dy", i + 1 );  //Start at 1
        mPositionIcon[ 2 ][ i ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
        mPositionIcon[ 2 ][ i ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
        sprintf( name, "MiniArrow%dg", i + 1 );  //Start at 1
        mPositionIcon[ 3 ][ i ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
        mPositionIcon[ 3 ][ i ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );

        sprintf( name, "MiniArrowP%d", i + 1 );
        mPlayerID[ i ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
        mPlayerID[ i ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
    }

    char name[64];
    sprintf( name, "MiniArrow%drGlow", 1 );
    mNitroEffect[ 0 ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
    mNitroEffect[ 0 ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
    sprintf( name, "MiniArrow%dbGlow", 2 );
    mNitroEffect[ 1 ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
    mNitroEffect[ 1 ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
    sprintf( name, "MiniArrow%dyGlow", 3 );
    mNitroEffect[ 2 ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
    mNitroEffect[ 2 ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
    sprintf( name, "MiniArrow%dgGlow", 4 );
    mNitroEffect[ 3 ]->Init( name, rmt::Vector( 0.0f, 0.0f, 0.0f ), true, false );
    mNitroEffect[ 3 ]->ScaleByCameraDistance( MIN_SCALE, MAX_SCALE, MIN_DIST, MAX_DIST );
}

//=============================================================================
// SuperSprintManager::InitCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::InitCamera()
{
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        SuperCamCentral* scc = GetSuperCamManager()->GetSCC( i );
        rAssert( scc );

        scc->SelectSuperCam( SuperCam::SUPER_SPRINT_CAM, SuperCamCentral::CUT | SuperCamCentral::QUICK, 0 );
    }
}


int SuperSprintManager::GetOnlyHumanPlayerID()
{
    int onlyHumanID = -1;
    int humanCount = 0;
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        if( mVehicleSlots[ i ].mIsHuman )
        {
            onlyHumanID = i;
            humanCount++;
        }
    }
    if( humanCount == 1 ) // only return the ID of the one human player
    {
        return onlyHumanID;
    }
    return -1;
}

//=============================================================================
// SuperSprintManager::RestoreControllerState
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::RestoreControllerState()
{
    unsigned int i;
    for ( i = 0; i < Input::MaxControllers; ++i )
    {
        OnControllerConnect( i );
    }
}

//=============================================================================
// SuperSprintManager::SetupTraps
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::SetupTraps()
{
    //Get the spikes if we need them
    if ( (RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B07 )
    {
        //Find the trap multicontroller.
        p3d::inventory->PushSection();
        p3d::inventory->SelectSection( "Default" );

        mTrapController = p3d::find<AnimCollisionEntityDSG>( "spikes" );
        mTrapController->AddRef();

        //Set them running backwards so that it doesn't animate.
        mTrapController->SetAnimationDirection( -1.0f );


        p3d::inventory->PopSection();

        rAssert( mTrapController );
    }
    else if ( (RenderEnums::LevelEnum)(GetCurrentLevelIndex() + RenderEnums::B00) == RenderEnums::B04 )
    {
        //Find the trap multicontroller.
        p3d::inventory->PushSection();
        p3d::inventory->SelectSection( "Default" );

        mTrapController = p3d::find<AnimCollisionEntityDSG>( "pistons" );
        mTrapController->AddRef();

        //Set them running backwards so that it doesn't animate.
        mTrapController->SetAnimationDirection( -1.0f );


        p3d::inventory->PopSection();

        rAssert( mTrapController );
    }
}

//=============================================================================
// SuperSprintManager::PlayIntroCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::PlayIntroCam()
{
    AnimatedCam::SetMulticontroller( "minigamecam" );
    AnimatedCam::SetCamera( "minigamecamShape" );
    AnimatedCam::CheckPendingCameraSwitch();
    AnimatedCam::SetCameraTransitionFlags( SuperCamCentral::CUT | SuperCamCentral::FORCE );
}

//=============================================================================
// SuperSprintManager::UpdatePositionIcons
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::UpdatePositionIcons( unsigned int milliseconds )
{
    CalculatePositions();

    //Sort positions.
    int i, j;

    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        rmt::Vector carPos;
        mVehicleSlots[ i ].mVehicle->GetPosition( &carPos );

        rmt::Box3D bbox;
        mVehicleSlots[ i ].mVehicle->GetBoundingBox( &bbox );
        carPos.y = bbox.high.y;

        for ( j = 0; j < SuperSprintData::NUM_PLAYERS; ++j )
        {
            mPositionIcon[ i ][ j ]->ShouldRender( false );
        }

        mPlayerID[ i ]->ShouldRender( false );
        mNitroEffect[ i ]->ShouldRender( false );

        if ( mNumHumanPlayers == 1 )
        {
            bool pressed = false;
            unsigned int k;
            for ( k = 0; k < SuperSprintData::NUM_PLAYERS; ++k )
            {
                if ( mTogglePosition[ k ] && mVehicleSlots[ k ].mIsHuman )
                {
                    pressed = true;
                    break;
                }
            }

            //If the toggle button is pressed, show all the positions
            if ( pressed )
            {
                mPositionIcon[ i ][ mPositions[ i ] - 1 ]->Move( carPos );
                mPositionIcon[ i ][ mPositions[ i ] - 1 ]->Update( milliseconds );               
                mPositionIcon[ i ][ mPositions[ i ] - 1 ]->ShouldRender( true );
            }
            else
            {
                //If the camera is in top-view render otherwise don't
                if ( !mVehicleSlots[ i ].mIsHuman || 
                     GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam()->GetType() == SuperCam::SUPER_SPRINT_CAM ||
                     GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam()->GetType() == SuperCam::ANIMATED_CAM )
                {
                    if ( mVehicleSlots[ i ].mVehicle->mNumTurbos > 0 )
                    {
                        mNitroEffect[ i ]->Move( carPos );
                        mNitroEffect[ i ]->Update( milliseconds );
                        mNitroEffect[ i ]->ShouldRender( true );
                    }
                    else
                    {
                        mPlayerID[ i ]->Move( carPos );
                        mPlayerID[ i ]->Update( milliseconds );
                        mPlayerID[ i ]->ShouldRender( true );
                    }
                }
            }
        }
        else
        {
            //If the toggle button is pressed, show all the positions
            if ( mTogglePosition[ i ] && mVehicleSlots[ i ].mIsHuman )
            {
                mPositionIcon[ i ][ mPositions[ i ] - 1 ]->Move( carPos );
                mPositionIcon[ i ][ mPositions[ i ] - 1 ]->Update( milliseconds );               
                mPositionIcon[ i ][ mPositions[ i ] - 1 ]->ShouldRender( true );
            }
            else
            {
                if ( mVehicleSlots[ i ].mVehicle->mNumTurbos > 0 )
                {
                    mNitroEffect[ i ]->Move( carPos );
                    mNitroEffect[ i ]->Update( milliseconds );
                    mNitroEffect[ i ]->ShouldRender( true );
                }
                else
                {
                    mPlayerID[ i ]->Move( carPos );
                    mPlayerID[ i ]->Update( milliseconds );
                    mPlayerID[ i ]->ShouldRender( true );
                }
            }
        }
    }
}

//=============================================================================
// SuperSprintManager::GetCheckpointWith
//=============================================================================
// Description: Comment
//
// Parameters:  ( char data )
//
// Return:      Locator
//
//=============================================================================
Locator* SuperSprintManager::GetCheckpointWith( char data )
{
    Locator* returnLoc = NULL;
    unsigned int i;
    for ( i = 0; i < mNumCheckPointLocators; ++i )
    {
        if ( mCheckPointLocators[ i ]->GetData() == (unsigned int)data )
        {
            returnLoc = mCheckPointLocators[ i ];
            break;
        }
    }


    return returnLoc;
}

//=============================================================================
// SuperSprintManager::GetPathDataWith
//=============================================================================
// Description: Comment
//
// Parameters:  ( Locator* loc )
//
// Return:      int
//
//=============================================================================
int SuperSprintManager::GetPathDataWith( Locator* loc )
{
    int i;
    for ( i = 0; i < MAX_AI_WAYPOINTS; ++i )
    {
        if ( mPathData[ i ].loc == loc )
        {
            return i;
        }
    }

    return -1;    
}


//=============================================================================
// SuperSprintManager::PositionCharacters
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperSprintManager::PositionCharacters()
{
    int i;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        char carlocator[32];
        sprintf( carlocator, "car%d", i + 1 );

        CarStartLocator* loc = p3d::find<CarStartLocator>(carlocator);
        rAssert( loc );

        if ( loc )
        {
            Character* character = GetCharacterManager()->GetCharacterByName( tEntity::MakeUID( SuperSprintData::CHARACTER_NAMES[ mPlayers[ i ].mCharacterIndex ] ) );

            rmt::Vector pos;
            loc->GetLocation( &pos );
            character->RelocateAndReset( pos, 0.0f, true );
        }
    }
}


//=============================================================================
// SuperSprintManager::SuperSprintManager
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
SuperSprintManager::SuperSprintManager() :
    Mappable( Input::ACTIVE_SS_GAME ),
    mCurrentState( IDLE ),
    mDrawable( NULL ),
    mCountDownTime( 3000 ),
    mCurrentPosition( 1 ),
    mStartTime( 0 ),
    mNumActivePlayers( 0 ),
    mNumHumanPlayers( 0 ),
    mNumHumansFinished( 0 ),
    mNumLaps( 1 ),
    mGoLeft( false ),
    mNumCheckPoints( 0 ),
    mCheckPoints( CHECKPOINT_LIST ),
    mFFlag( NULL ),
    mWFlag( NULL ),
    mFFlagTimeout( 0 ),
    mWFlagTimeout( 0 ),
    mTrapController( NULL ),
    mTrapTriggered( false )
{
    mGameType = GameplayManager::GT_SUPERSPRINT;
    SetNumPlayers( SuperSprintData::NUM_PLAYERS );

    int i, j;
    for ( i = 0; i < SuperSprintData::NUM_PLAYERS; ++i )
    {
        for ( j = 0; j < SuperSprintData::NUM_PLAYERS; ++j )
        {
            mPositionIcon[ i ][ j ] = NULL;
        }

        mPlayerID[ i ] = NULL;
        mTogglePosition[ i ] = false;
        mNitroEffect[ i ] = NULL;
    }
}

//=============================================================================
// SuperSprintManager::~SuperSprintManager
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
SuperSprintManager::~SuperSprintManager()
{
}
