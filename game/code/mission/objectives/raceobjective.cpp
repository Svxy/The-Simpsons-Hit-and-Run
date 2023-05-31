//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement RaceObjective
//
// History:     23/07/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radmath/radmath.hpp>

//========================================
// Project Includes
//========================================

#include <mission/objectives/raceobjective.h>
#include <mission/gameplaymanager.h>
#include <mission/mission.h>
#include <mission/missionstage.h>
#include <mission/animatedicon.h>

#include <events/eventmanager.h>

#include <meta/eventlocator.h>
#include <meta/carstartlocator.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <worldsim/redbrick/vehicle.h>

#include <ai/vehicle/waypointai.h>

#include <roads/road.h>

#include <meta/locator.h>
#include <meta/triggervolume.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/vehiclecentral.h>

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
// RaceObjective::RaceObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
RaceObjective::RaceObjective() :
    mNextCollectible( 0 ),
    mNumAIVehicles( 0 ),
    mMyPosition( 1 ),
    mNumLaps( 1 ),
    mNumLapsCompleted( 0 ),
    mIsGambleRace (false),
    mParTime(-666),  // <<==-- EVIL!
    mIsTimeTrial( false ),
    mPlayerSeg( NULL ),
    mPlayerSegT( 0.0f ),
    mPlayerRoadT( 0.0f ),
    mFinishLine( NULL ),
    mFinishLineEffect( NULL ),
    mFinishActive( false )
{
    mPlayerElem.elem = NULL;
}

//==============================================================================
// RaceObjective::~RaceObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
RaceObjective::~RaceObjective()
{
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

void RaceObjective::OnInitialize()
{
    CollectibleObjective::OnInitialize();

    for( unsigned int i = 0; i < GetNumCollectibles(); i++ )
    {
        rAssert( mCollectibles[ i ].pLocator != NULL );
        
        rmt::Vector locPos;
        mCollectibles[ i ].pLocator->GetPosition( &locPos );

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
            rTuneAssertMsg( false, msg );

            RoadSegment* closestSeg = NULL;
            float dummy;
            GetIntersectManager()->FindClosestRoad( locPos, 100.0f, closestSeg, dummy );

            seg = (RoadSegment*) closestSeg;
            segT = RoadManager::DetermineSegmentT( locPos, seg );
            roadT = RoadManager::DetermineRoadT( seg, segT );
            closestElem.elem = seg->GetRoad();
            closestElem.type = RoadManager::ET_NORMALROAD;
        }

        mCollectibles[ i ].elem = closestElem;
        mCollectibles[ i ].seg = seg;
        mCollectibles[ i ].segT = segT;
        mCollectibles[ i ].roadT = roadT;

        rAssert( mCollectibles[ i ].elem.elem != NULL );

    }

}


//=============================================================================
// RaceObjective::OnInitCollectibles
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RaceObjective::OnInitCollectibles()
{
    rAssert( GetCollectibleLocator( 0 ) != NULL );
    Activate( 0, true, true, HudMapIcon::ICON_FLAG_WAYPOINT );

    MissionStage* ms = GetGameplayManager()->GetCurrentMission()->GetCurrentStage();
    
    unsigned int i;
    for( i = 1; i < GetNumCollectibles(); i++ )
    {
        rAssert( GetCollectibleLocator( i ) != NULL );

        if ( (mNumLapsCompleted == (mNumLaps - 1)) && ( i == GetNumCollectibles() - 1) )  //Last one
        {
            bool drawFinish = mFinishLine != NULL;

            //Don't draw this.
            Activate( i, false, false, HudMapIcon::ICON_FLAG_WAYPOINT, !drawFinish );

            //Draw this!
            if ( mFinishLine != NULL && !mFinishActive )
            {
                mFinishLine->ShouldRender( true );
                mFinishActive = true;
            }
        }
        else
        {
            Activate( i, false, false, HudMapIcon::ICON_COLLECTIBLE, true );
        }
    }
}

//=============================================================================
// RaceObjective::OnInitCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RaceObjective::OnInitCollectibleObjective()
{
    rAssert( mNextCollectible < GetNumCollectibles() );
    mNextCollectible = 0;
    MissionStage* ms = GetGameplayManager()->GetCurrentMission()->GetCurrentStage();
    mNumAIVehicles = ms->GetNumVehicles();
    mMyPosition = mNumAIVehicles + 1;

    mPlayerElem.elem = NULL;
    mPlayerSeg = NULL;
    mPlayerSegT = 0.0f;
    mPlayerRoadT = 0.0f;

    unsigned int i;
    for ( i = 0; i < mNumAIVehicles; ++i )
    {
        mAIRaceCars[ i ].raceCar = ms->GetVehicle( i );
        (mAIRaceCars[ i ].raceCar)->AddRef();
        VehicleAI* vAI = GetVehicleCentral()->GetVehicleAI( mAIRaceCars[ i ].raceCar );
        rAssert( vAI );

        WaypointAI* wpAI = dynamic_cast<WaypointAI*>(vAI);
        rAssert( wpAI );

        wpAI->SetCurrentCollectible( 0 );
        wpAI->SetCurrentLap( 0 );

        mAIRaceCars[ i ].raceCarAI = wpAI;

        mAIRaceCars[ i ].mAnimatedIcon = new AnimatedIcon();
        rmt::Vector carPos;
        mAIRaceCars[ i ].raceCar->GetPosition( &carPos );
        mAIRaceCars[ i ].mAnimatedIcon->Init( ARROW_RACE, carPos );
        mAIRaceCars[ i ].mAnimatedIcon->ScaleByCameraDistance( MIN_ARROW_SCALE, MAX_ARROW_SCALE, MIN_ARROW_SCALE_DIST, MAX_ARROW_SCALE_DIST );
    }

    CGuiScreenHud* cgsmh = GetCurrentHud();

    mIsTimeTrial = mNumAIVehicles == 0;

    if( !mIsTimeTrial )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_RACE_POSITION );

        if ( cgsmh )
        {
            cgsmh->SetRacePosition( mMyPosition, mNumAIVehicles + 1 );
        }
    }

    if( mIsGambleRace )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_PAR_TIME );
    }

    mNumLapsCompleted = 0;

    if ( mNumLaps > 1 )
    {
        //Let's throw up the lap hud.
        GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_LAP_COUNTER );
        if ( cgsmh )
        {
            cgsmh->SetLap( mNumLapsCompleted + 1, mNumLaps );
        }
    }

    //Set up the finih line
    CarStartLocator* finish = p3d::find<CarStartLocator>("race_finish");
    if ( finish )
    {
        HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
        //Find and setup the finishline.
        rAssert( mFinishLine == NULL );
        mFinishLine = new AnimatedIcon();

        rmt::Vector finishPos;
        finish->GetLocation( &finishPos );
        
        float heading = finish->GetRotation();

        rmt::Matrix mat;
        mat.Identity();
        mat.FillRotateXYZ( 0.0f, heading, 0.0f );
        mat.FillTranslate( finishPos );

        mFinishLine->Init( "finish_line", mat, false, false );

        //And the effect
        mFinishLineEffect = new AnimatedIcon();
        mFinishLineEffect->Init( "mission_col", mat, false, true );  //One shot

        HeapMgr()->PopHeap( GMA_LEVEL_MISSION );
    }

    SetFocus( 0 );

    mFinishActive = false;
}

//=============================================================================
// RaceObjective::OnFinalizeCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RaceObjective::OnFinalizeCollectibleObjective()
{
    if ( mNumAIVehicles )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_RACE_POSITION );
    }

    if( mIsGambleRace )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_PAR_TIME );
    }

    unsigned int i;
    for ( i = 0; i < mNumAIVehicles; ++i )
    {
        (mAIRaceCars[ i ].raceCar)->Release();
        mAIRaceCars[ i ].raceCar = NULL;
        mAIRaceCars[ i ].raceCarAI = NULL;

        delete mAIRaceCars[ i ].mAnimatedIcon;
        mAIRaceCars[ i ].mAnimatedIcon = NULL;
    }

    mNumAIVehicles = 0;
    mNextCollectible = 0;
    mIsTimeTrial = false;

    if ( mNumLaps > 0 )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_LAP_COUNTER );
    }

    if ( mFinishLine )
    {
        delete mFinishLine;
        mFinishLine = NULL;
    }

    if ( mFinishLineEffect )
    {
        delete mFinishLineEffect;
        mFinishLineEffect = NULL;
    }
}

//=============================================================================
// RaceObjective::OnCollection
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int collectibleNum, bool &shouldReset )
//
// Return:      bool 
//
//=============================================================================
bool RaceObjective::OnCollection( unsigned int collectibleNum, bool &shouldReset )
{
    if ( collectibleNum == mNextCollectible )
    {
        mNextCollectible++;

        if ( mNextCollectible < GetNumCollectibles() )
        {
            if ( mNextCollectible == mNumCollectibles - 1 && mFinishLine != NULL && !mFinishActive && mNumLapsCompleted == mNumLaps - 1 )
            {
                HudMapIcon::eIconType iconType = HudMapIcon::ICON_FLAG_WAYPOINT;

                if( mFinishLine != NULL )
                {
                    mFinishLine->ShouldRender( true );
                    mFinishActive = true;

                    iconType = HudMapIcon::ICON_FLAG_CHECKERED; // use checkered flag icon instead
                }

                Activate( mNextCollectible, true, true, iconType, false );
            }
            else
            {
                Activate( mNextCollectible, true, true, HudMapIcon::ICON_FLAG_WAYPOINT, true );
            }
        }
        else
        {
            //Make sure we have laps left.
            if ( mNumLapsCompleted >= mNumLaps - 1 )
            {
                //We're done...                
                mNumLapsCompleted++;

                if ( mFinishLineEffect )
                {
                    mFinishLineEffect->Reset();
                    mFinishLineEffect->ShouldRender( true );
                }
                return true;
            }
            else
            {
                // need to collect the last collectible
                //
                Collect( mNextCollectible - 1, false );

                //Start over on a new lap!
                ResetCollectibles();

                mNextCollectible = 0;
                mNumLapsCompleted++;

                Activate( mNextCollectible, true, true, HudMapIcon::ICON_FLAG_WAYPOINT );

                //Do this to prevent the locator from being disabled.
                shouldReset = true;

                if ( GetCurrentHud() )
                {
                    GetCurrentHud()->SetLap( mNumLapsCompleted + 1, mNumLaps );
                }
            }
        }
        SetFocus( mNextCollectible );

        return true;        
    }

    return false;
}

//=============================================================================
// RaceObjective::OnUpdateCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTimeMilliseconds )
//
// Return:      void 
//
//=============================================================================
void RaceObjective::OnUpdateCollectibleObjective( unsigned int elapsedTimeMilliseconds )
{
    unsigned int oldPosition = mMyPosition;

    if ( mNumAIVehicles > 0 )
    {
        CalculatePosition();
    }

    if( mMyPosition < oldPosition )
    {
        GetEventManager()->TriggerEvent( EVENT_RACE_PASSED_AI );
    }
    else if( mMyPosition > oldPosition )
    {
        GetEventManager()->TriggerEvent( EVENT_RACE_GOT_PASSED_BY_AI );
    }

    if ( mNumAIVehicles > 0 && GetCurrentHud() )
    {
        GetCurrentHud()->SetRacePosition( mMyPosition, mNumAIVehicles + 1 );
    }

    if ( mFinishActive && mFinishLine )
    {
        mFinishLine->Update( elapsedTimeMilliseconds );
    }

    if ( mFinishActive && mFinishLineEffect )
    {
        mFinishLineEffect->Update( elapsedTimeMilliseconds );
    }

    unsigned int i;
    for ( i = 0; i < mNumAIVehicles; ++i )
    {
        rmt::Vector carPos;
        Vehicle* vehicle = mAIRaceCars[ i ].raceCar;
        vehicle->GetPosition( &carPos );

        rmt::Box3D bbox;
        vehicle->GetBoundingBox( &bbox );
        carPos.y = bbox.high.y;

        mAIRaceCars[ i ].mAnimatedIcon->Move( carPos );
        mAIRaceCars[ i ].mAnimatedIcon->Update( elapsedTimeMilliseconds );
    }
}



//Return boolean mIsGambleRace 

bool RaceObjective::QueryIsGambleRace()
{
    return mIsGambleRace;
}


//Sets mIsGambleRace with input vaule boolean
void RaceObjective::SetGambleRace(bool boolean)
{
    mIsGambleRace = boolean;
}

//Set the Partime or time to beat for gamble races
void RaceObjective::SetParTime(int seconds)
{
    mParTime = seconds;

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        currentHud->SetParTime( mParTime );
    }
}

//Returns mParTime , call is from mission stage 
int RaceObjective::GetParTime()
{
    return mParTime;
}







//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// RaceObjective::CalculatePosition
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RaceObjective::CalculatePosition()
{
    rAssert( mNumLaps > 0 );

    unsigned int numCollectibles = GetNumCollectibles();
    if( numCollectibles == 0 )
    {
        return;
    }

    //////////////////////////////////////////////////////////////
    // Figure out which collectibles have been reached by AI
    //
    for( unsigned int i=0; i<mNumAIVehicles; i++ )
    {
        WaypointAI* ai = mAIRaceCars[i].raceCarAI;
        rAssert( ai );

        rmt::Vector aiLoc;
        ai->GetPosition( &aiLoc );

        unsigned int currCollectible = (unsigned int)(ai->GetCurrentCollectible()); 
        if( 0 <= currCollectible && currCollectible < numCollectibles )
        {
            //
            // CollectibleObjective stores Locator*, but if we're 
            // in RaceObjective, the assumption is that it's an 
            // EventLocator (so it has a trigger volume)
            //
            rAssert( dynamic_cast<EventLocator*>( (Locator*)GetCollectibleLocator( currCollectible ) ) );

            EventLocator* eloc = (EventLocator*) GetCollectibleLocator( currCollectible );
            unsigned int triggerCount = eloc->GetNumTriggers();

            bool reachedCollectible = false;

            for( unsigned int j=0; j<triggerCount; j++ )
            {
                TriggerVolume* t = eloc->GetTriggerVolume( j );
                rAssert( t );

                if( t->Contains( aiLoc ) )
                {
                    reachedCollectible = true;
                    break;
                }
            }

            if( reachedCollectible )
            {
                if( currCollectible < (numCollectibles - 1) )
                {
                    currCollectible++;

                    // if we're not at the last collectible, set our sights
                    // on the next collectible...
                    ai->SetCurrentCollectible( currCollectible );

                    // Broadcast how many waypoints we have left, so we can 
                    // tell if the race is close
                    int numCollectiblesLeft = numCollectibles - currCollectible - 1;
                    GetEventManager()->TriggerEvent( 
                        EVENT_WAYAI_HIT_CHECKPOINT, static_cast<void*>(&numCollectiblesLeft) );
                }
                else
                {
                    // if we're at the last collectible, then...

                    // we've completed a lap, so increment
                    ai->SetCurrentLap( ai->GetCurrentLap() + 1 );

                    // no more laps... we're done!
                    if( ai->GetCurrentLap() >= mNumLaps )
                    {
                        GetEventManager()->TriggerEvent( 
                            EVENT_WAYAI_AT_DESTINATION, (void*) ai->GetVehicle() );
                        ai->SetActive( false );
                    }
                    else
                    {
                        ai->SetCurrentCollectible( 0 ); // start a new lap... 
                    }
                }
            } // end of if( reachedCollectible )

            // calculate distance to this collectible
            rAssert( 0 <= currCollectible && currCollectible < numCollectibles );

            //////////////////////////////////////////////////////
            // Query the road manager to get the distance
            RoadManager::PathElement aiElem; 
            aiElem.elem = NULL;
            RoadSegment* aiSeg = NULL;
            float aiSegT = 0.0f;
            float aiRoadT = 0.0f;

            ai->GetRacePathInfo( aiElem, aiSeg, aiSegT, aiRoadT );

            // make sure the AI is on a path element
            rAssert( aiElem.elem != NULL );

            RoadManager::PathElement collectibleElem;
            float collectibleRoadT;
            GetCollectiblePathInfo( currCollectible, collectibleElem, collectibleRoadT );
            
            rmt::Vector collectiblePos;
            eloc->GetLocation( &collectiblePos );


            // make sure the collectible is on a path element
            rAssert( collectibleElem.elem != NULL );

            float aiDistToColl = NEAR_INFINITY;
            if( aiElem.elem != NULL && collectibleElem.elem != NULL )
            {
                SwapArray<RoadManager::PathElement> dummy;
                dummy.Allocate( RoadManager::GetInstance()->GetMaxPathElements() );
                aiDistToColl = RoadManager::GetInstance()->FindPathElementsBetween(
                    false,
                    aiElem, aiRoadT, aiLoc,
                    collectibleElem, collectibleRoadT, collectiblePos,
                    dummy );
            }
            ai->SetDistToCurrentCollectible( aiDistToColl );

        } // end of if( currCollectible is valid )
    } // end of for-loop over AI vehicles

    ////////////////////////////////////////////////////////////////////
    // Update my position 
    //
    if ( mNextCollectible == GetNumCollectibles() )
    {
        return;
    }
    
    unsigned int numInFront = 0;
    unsigned int vehiclesWithMe[ MAX_RACECARS ];
    unsigned int numWithMe = 0;
    unsigned int numFinished = 0;
    
    for( unsigned int i = 0; i < mNumAIVehicles; ++i )
    {
        int numLapsCompleted = (mAIRaceCars[ i ].raceCarAI)->GetCurrentLap();
        if ( numLapsCompleted == mNumLaps )
        {
            //They're finshed racing
            numFinished++;
        }
        else if ( numLapsCompleted > mNumLapsCompleted )
        {
            //This guys finished more laps than I.
            numInFront++;         
        }
        else if ( numLapsCompleted == mNumLapsCompleted )
        {
            //We're racing the same lap number.
            int currCollectible = (mAIRaceCars[ i ].raceCarAI)->GetCurrentCollectible();

            int playerCollectible = static_cast<int>( mNextCollectible );
            if( currCollectible > playerCollectible )
            {
                numInFront++;
            }
            else if( currCollectible == playerCollectible )
            {
                vehiclesWithMe[ numWithMe ] = i;
                numWithMe++;
            }
        }
    }

    mMyPosition = 1 + numInFront + numFinished;

    //Now, who is actually in front of me going to the same waypoint?
    const Locator* collectibleLoc = GetCollectibleLocator( mNextCollectible );
    if( collectibleLoc == NULL )
    {
        rAssert( false );  //Why?
        return;
    }

    ////////////////////////////////////////////////////////
    // Find out player's dist to mNextCollectible
    //
    Avatar* player = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( player );

    RoadManager::PathElement playerElem;
    playerElem.elem = NULL;
    RoadSegment* playerSeg = NULL;
    float playerSegT = 0.0f;
    float playerRoadT = 0.0f;

    player->GetLastPathInfo( playerElem, playerSeg, playerSegT, playerRoadT );
    // update only if value is valid
    if( playerElem.elem != NULL )
    {
        // update old values only if the new values are good..
        if( mPlayerElem != playerElem )
        {
            mPlayerElem = playerElem;
        }
        if( playerSeg )
        {
            if( mPlayerSeg != playerSeg )
            {
                mPlayerSeg = playerSeg;
            }
            mPlayerSegT = playerSegT;
            mPlayerRoadT = playerRoadT;
        }
    }

    rmt::Vector playerPos;
    player->GetPosition( playerPos );

    //rAssert( mPlayerElem.elem != NULL );

    RoadManager::PathElement collectibleElem;
    float collectibleRoadT;
    GetCollectiblePathInfo( mNextCollectible, collectibleElem, collectibleRoadT );

    rmt::Vector collectiblePos;
    collectibleLoc->GetLocation( &collectiblePos );

    // make sure the collectible is on a path element
    rAssert( collectibleElem.elem != NULL );

    float playerDistToCurrCollectible = NEAR_INFINITY;
    if( mPlayerElem.elem != NULL && collectibleElem.elem != NULL )
    {
        SwapArray<RoadManager::PathElement> dummy;
        dummy.Allocate( RoadManager::GetInstance()->GetMaxPathElements() );
        playerDistToCurrCollectible = RoadManager::GetInstance()->FindPathElementsBetween(
            false,
            mPlayerElem, mPlayerRoadT, playerPos, 
            collectibleElem, collectibleRoadT, collectiblePos,
            dummy );
    }

    // store this info away in the avatar...
    player->SetRaceInfo( playerDistToCurrCollectible, mNextCollectible, mNumLapsCompleted );
        

    ////////////////////////////////////////////////////////////
    // The last set of vehicles to consider are the ones that
    // are headed to the same collectible (race checkpoint) and
    // are in the same lap as we are... 
    //
    for( unsigned int i = 0; i < numWithMe; ++i )
    {
        int index = vehiclesWithMe[ i ];

        WaypointAI* ai = mAIRaceCars[ index ].raceCarAI;

        // We wouldn't have added this AI to the vehiclesWithMe list if it
        // wasn't headed to the same collectible as we are and is in the same lap
        rAssert( ai->GetCurrentCollectible() == static_cast<int>( mNextCollectible ) );
        rAssert( ai->GetCurrentLap() == mNumLapsCompleted );

        //This guy and I are racing for the same collectible
        //Test the dist to the collectible.

        float aiDistToCurrCollectible = ai->GetDistToCurrentCollectible();
        if( aiDistToCurrCollectible < playerDistToCurrCollectible )
        {
            // blast! he's ahead of me... my pos is thus bumped even lower...
            mMyPosition++;
        }
    }
    return;
}
