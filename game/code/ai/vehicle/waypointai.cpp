//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement WaypointAI
//
// History:     10/07/2002 + Created -- NAME
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

#include <ai/vehicle/waypointai.h>

#include <events/eventmanager.h>

#include <meta/eventlocator.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <roads/roadsegment.h>
#include <roads/intersection.h>
#include <roads/road.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/avatarmanager.h>

#include <debug/profiler.h>

#include <gameflow/gameflow.h>

#include <mission/gameplaymanager.h>
#include <mission/mission.h>
#include <mission/missionstage.h>
#include <mission/objectives/missionobjective.h>
#include <mission/objectives/raceobjective.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

const float SECONDS_BEFORE_RESET_ON_SPOT = 1.0f;
const float WaypointAI::DEFAULT_TRIGGER_RADIUS = 10.0f; // how close we have to be to waypoint to say we've reached it

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// WaypointAI::WaypointAI
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
WaypointAI::WaypointAI( 
    Vehicle* pVehicle, 
    bool enableSegmentOptimization,  
    float triggerRadius,
    bool autoResetOnDestroyed ) :

    VehicleAI( 
        pVehicle, 
        AI_WAYPOINT, 
        enableSegmentOptimization, 
        VehicleAI::DEFAULT_MIN_SHORTCUT_SKILL,
        VehicleAI::DEFAULT_MAX_SHORTCUT_SKILL,
        true),
    miNumWayPoints( 0 ),
    miCurrentWayPoint( -1 ),
    miNextWayPoint( 0 ),

    mTriggerRadius( triggerRadius ),

    mDistToCurrentCollectible( 0.0f ),
    miCurrentCollectible( -1 ),
    miNumLapsCompleted( 0 ),
    mCurrWayPointHasMoved( true ),
    mNeedsResetOnSpot( false ),
    mAutoResetOnDestroyed( autoResetOnDestroyed ),
    mSecondsTillResetOnSpot( 0.0f ),

    mWaypointAIType( RACE ),
    
    mSecondsSinceTurboUse( 0.0f ),

    mSecondsWaitingForPlayer( 0.0f )
{
}

//==============================================================================
// WaypointAI::~WaypointAI
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
WaypointAI::~WaypointAI()
{
}

//=============================================================================
// WaypointAI::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeins )
//
// Return:      void 
//
//=============================================================================
void WaypointAI::Update( float timeins )
{
BEGIN_PROFILE( "WaypointAI Update" );    

    if( GetState() == STATE_LIMBO )
    {
        END_PROFILE( "WaypointAI Update" );
        return;
    }

    if( miNumWayPoints <= 0 )
    {
        END_PROFILE( "WaypointAI Update" );
        return;
    }

    rAssert( 0 < miNumWayPoints && miNumWayPoints < WaypointAI::MAX_WAYPOINTS );

   
    if( miCurrentWayPoint < 0 )
    {
        SetCurrentWayPoint( 0 );
    }

    rAssert( 0 <= miCurrentWayPoint && miCurrentWayPoint < miNumWayPoints );
    rAssert( 0 < miNextWayPoint && miNextWayPoint <= miNumWayPoints );


    UpdateNeedToWaitForPlayer( timeins );
    UpdateSelf();
    UpdateNeedsResetOnSpot( timeins );
    FollowWaypoints();
    UpdateSegments();
    FollowRoad();
    EvadeTraffic( NULL );
    DoSteering();
    DoCatchUp( timeins );
    CheckState( timeins );

END_PROFILE( "WaypointAI Update" );    
}

void WaypointAI::Initialize()
{
    miNumWayPoints = 0;
    miCurrentWayPoint = -1;
    miNextWayPoint = 0;

    mDistToCurrentCollectible = 0.0f;
    miCurrentCollectible = -1;
    miNumLapsCompleted = 0;

    mSecondsSinceTurboUse = 0.0f;

    mSecondsWaitingForPlayer = 0.0f;

    // VehicleAI::Initialize will call our reset, 
    // our reset will call VehicleAI::Reset, so it's all good
    VehicleAI::Initialize(); 
}

void WaypointAI::Reset()
{
    mCurrWayPointHasMoved = true;

    mNeedsResetOnSpot = false;
    mSecondsTillResetOnSpot = 0.0f;

    mSecondsWaitingForPlayer = 0.0f;

    VehicleAI::Reset();
}


void WaypointAI::UpdateNeedToWaitForPlayer( float timeins )
{
    /*
    I think we must finally deal with one design limitation... In a mission, the
    AI generally needs to drive quite far. We only have adjacent zones loaded, so
    since we start from the middle zone, we only have 1 zone to work with before
    the AI gets too far ahead or behind the player and no world is loaded. Track 
    data alone is insufficient: the AI WILL get stuck, hit fences, get caught in 
    all sorts of different ballyhoos, etc., etc., etc.. 

    Since this will have to be a last minute hack, I propose dealing with
    the different objective types differently. In each update the AI will have 
    different treatments for different objectives:

    OBJ_FOLLOW      ==> Do nothing. The crow-fly distance test is part of failure 
                        condition for the mission. Wouldn't hurt to do dist test.

    OBJ_LOSETAIL    ==> Do nothing. The crow-fly distance test is part of success 
                        condition for the mission. Wouldn't hurt to do dist test.

    OBJ_DESTROY     ==> If crow-fly distance from player too far (300 meters), 
                        temporarily disable. This is so they don't drive around 
                        destroying themselves.

    OBJ_DUMP        ==> Same as above. Justification: gameplay requires them to 
                        stay in your view.

    OBJ_RACE        ==> Trickier. If race pos is ahead of player's race pos AND if 
                        crow-fly dist is too far (300 meters), then temporarily 
                        disable (till player gets closer). We can't just do crow-fly
                        test because if it's possible at all for an AI that has been
                        "left in the dust" to catch up with the player, then we
                        should allow it to do so.
    */
    const float DIST_SQR_TOO_FAR_FROM_PLAYER = 90000.0f;

    bool needToDisable = false;

    Mission* m = GetGameplayManager()->GetCurrentMission();
    if( m && !m->mIsStreetRace1Or2 ) //ignore streetraces cuz they're do loops in small areas
    {
        MissionStage* ms = m->GetCurrentStage();
        if( ms )
        {
            MissionObjective* mo = ms->GetObjective();
            if( mo )
            {
                MissionObjective::ObjectiveTypeEnum type = mo->GetObjectiveType();
                if( type == MissionObjective::OBJ_RACE )
                {
                    float myDistToCheckPt = GetDistToCurrentCollectible();
                    int myCheckPt = GetCurrentCollectible();
                    int myLap = GetCurrentLap();
                    

                    float playerDistToCheckPt;
                    int playerCheckPt, playerLap;
                    Avatar* player = GetAvatarManager()->GetAvatarForPlayer( 0 );
                    player->GetRaceInfo( playerDistToCheckPt, playerCheckPt, playerLap );

                    bool aheadOfPlayer = false;

                    if( myLap > playerLap )
                    {
                        aheadOfPlayer = true;
                    }
                    else if( myLap == playerLap )
                    {
                        if( myCheckPt > playerCheckPt )
                        {
                            aheadOfPlayer = true;
                        }
                        else if( myCheckPt == playerCheckPt )
                        {
                            if( myDistToCheckPt < playerDistToCheckPt )
                            {
                                aheadOfPlayer = true;
                            }
                        }
                    }
                    if( aheadOfPlayer )
                    {
                        // ok, I'm ahead of the player (by assumption that
                        // if I have the same checkpoint as the player, 
                        // I will be within 300 meters of the player and
                        // thus won't need to worry about disabling self.)

                        rmt::Vector myPos, playerPos;
                        GetPosition( &myPos );
                        player->GetPosition( playerPos );

                        if( (myPos - playerPos).MagnitudeSqr() >= DIST_SQR_TOO_FAR_FROM_PLAYER )
                        {
                            needToDisable = true;
                        }
                    }
                }
                // Other objective types: go through the same treatment.
                else
                {
                    rmt::Vector myPos, playerPos;
                    GetPosition( &myPos );
                    GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( playerPos );

                    if( (myPos - playerPos).MagnitudeSqr() >= DIST_SQR_TOO_FAR_FROM_PLAYER )
                    {
                        needToDisable = true;
                    }
                }
            }
        }
    }
    if( needToDisable )
    {
        if( GetState() != STATE_WAITING_FOR_PLAYER )
        {
            //mSecondsWaitingForPlayer = 0.0f;
            SetState( STATE_WAITING_FOR_PLAYER );
        }
    }
    else
    {
        if( GetState() == STATE_WAITING_FOR_PLAYER )
        {
            // get back into accel state? Or do we need some sort of push state
            SetState( STATE_ACCEL );
        }
    }


    if( GetState() == STATE_WAITING_FOR_PLAYER )
    {
        const float MAX_WAIT_SECONDS = 20.0f;

        mSecondsWaitingForPlayer += timeins;
        if( mSecondsWaitingForPlayer > MAX_WAIT_SECONDS )
        {
            // fudge sending this event to fail the mission
            GetEventManager()->TriggerEvent( EVENT_WAYAI_AT_DESTINATION, GetVehicle() );
            mSecondsWaitingForPlayer = 0.0f; // reset so we don't continually send this event
        }
    }
}


void WaypointAI::PossiblyUseTurbo()
{
    rAssert( GetGameFlow()->GetCurrentContext() == CONTEXT_SUPERSPRINT );

    // in the supersprint context, we want to use a turbo:
    //   - if it's been long enough since the last time we used one; and 
    //   XXX NAH XXX - if we're behind (in race position); and
    //   XXX NAH XXX - if we haven't used one this lap; and
    //   - if we have any turbo left
    //   - if vector to mDestination and current vehicle facing "line up"; and 
    //   XXX NAH XXX - if current vehicle velocity & facing "line up"; and 
    //   XXX NAH XXX - 50% chance when there's no one in our way or 
    //     50% chance when there's some one in our way; and 
    //   

    const float MIN_SECONDS_BETW_TURBOS = 10.0f;
    if( mSecondsSinceTurboUse < MIN_SECONDS_BETW_TURBOS )
    {
        return;
    }
    if( GetVehicle()->mNumTurbos <= 0 )
    {
        return;
    }

    // grab my facing value
    rmt::Vector myDir;
    GetHeading( &myDir );
    rAssert( rmt::Epsilon( myDir.MagnitudeSqr(), 1.0f, 0.001f ) );

    // grab the lookahead
    rmt::Vector myPos;
    GetPosition( &myPos );
    rmt::Vector destPt;
    GetDestination( destPt );

    rmt::Vector toDest = destPt - myPos;
    toDest.NormalizeSafe();
    rAssert( rmt::Epsilon( toDest.MagnitudeSqr(), 1.0f, 0.001f ) );

    // if lookahead not close enough to facing, we don't 
    // have a straightaway, abort
    const float LOOKAHEAD_FACING_COSALPHA = 0.9848077f; // cos10
    if( myDir.Dot( toDest ) < LOOKAHEAD_FACING_COSALPHA )
    {
        return;
    }

    /* Don't need to do this 
        The turbo will basically shoot it forward... 

    // test the velocity (if speed is sufficient)
    const float MIN_SPEED_FOR_TURBO = 1.0f;
    if( GetVehicle()->mSpeed > MIN_SPEED_FOR_TURBO )
    {
        rmt::Vector myVel;
        GetVehicle()->GetVelocity( myVel );
        myVel /= mSpeed;
        rAssert( rmt::Epsilon( toDest.MagnitudeSqr(), 1.0f, 0.001f ) );

        const float VELOCITY_FACING_COSALPHA = ????
        if( myDir.Dot( myVel ) < VELOCITY_FACING_COSALPHA )
        {
            return;
        }
    }
    */
    UseTurbo();

}

void WaypointAI::UseTurbo()
{
    // Ok, all conditions are a go.. use Turbo...
    GetVehicle()->TurboOnHorn();
    mSecondsSinceTurboUse = 0.0f;
}

void WaypointAI::DoCatchUp( float timeins )
{
    mSecondsSinceTurboUse += timeins;

    const float SECONDS_BETW_CATCHUPS = 2.0f;
    mSecondsSinceLastDoCatchUp += timeins;
    if( mSecondsSinceLastDoCatchUp < SECONDS_BETW_CATCHUPS )
    {
        return;
    }

    mSecondsSinceLastDoCatchUp = 0.0f;

    ///////////////////////////////////////////////////////
    // info about myself & player
    Avatar* player = GetAvatarManager()->GetAvatarForPlayer(0);
    rAssert( player );

    Vehicle* myVehicle = GetVehicle();
    rAssert( myVehicle );

    // reset old values
    ResetCatchUpParams();

    Vehicle* playerVehicle = player->GetVehicle();
    if( playerVehicle == NULL )
    {
        return;
    }

    float mySpeedKmh = myVehicle->mSpeedKmh;
    float playerSpeedKmh = playerVehicle->mSpeedKmh;

    // figure rough distance to player
    rmt::Vector myPos, playerPos;
    GetPosition( &myPos );
    playerVehicle->GetPosition( &playerPos );

    float roughDistToPlayerSqr = (playerPos - myPos).MagnitudeSqr();
    //float roughDistToPlayer = rmt::Sqrt( roughDistToPlayerSqr );

    // Set the following values in the upcoming logic
    float catchUpFactor = 0.0f; // ranges from -1.0f to 1.0f
    float minDesiredSpeedKmh = 0.0f;
    float midDesiredSpeedKmh = 0.0f;
    float maxDesiredSpeedKmh = 0.0f;

    // perform type-specific catch-up logic
    switch( mWaypointAIType )
    {
    case RACE:
        {
            if( GetGameFlow()->GetCurrentContext() == CONTEXT_SUPERSPRINT )
            {
                PossiblyUseTurbo();
            }

            // Tunable values specific for Race situation

            // If displacement distance lies within this value,
            // we give the AI maximum capabilities to keep up with player
            const float DIST_KEEP_UP_WITH_PLAYER = 25.0f; 

            // If it ever comes down to comparing distances to a
            // collectible, this value is used to determine degree of catch-up
            const float DIST_MAX_CATCHUP = mCatchupParams.Race.DistMaxCatchup; 

            const float MIN_FACTOR = mCatchupParams.Race.FractionPlayerSpeedMinCatchup; //0.5f;
            const float MID_FACTOR = mCatchupParams.Race.FractionPlayerSpeedMidCatchup; //1.1f;
            const float MAX_FACTOR = mCatchupParams.Race.FractionPlayerSpeedMaxCatchup; //1.7f;

            const float ABSOLUTE_MIN_DESIRED_SPEED_KPH = 
                playerVehicle->mDesignerParams.mDpTopSpeedKmh * MIN_FACTOR;
            const float ABSOLUTE_MAX_DESIRED_SPEED_KPH =
                playerVehicle->mDesignerParams.mDpTopSpeedKmh * MAX_FACTOR;

            // figure out the min (when catchup == -1.0f)
            float tmpMinDesiredSpeed = playerVehicle->mSpeedKmh * MIN_FACTOR;
            minDesiredSpeedKmh = rmt::Clamp( tmpMinDesiredSpeed,
                ABSOLUTE_MIN_DESIRED_SPEED_KPH,
                ABSOLUTE_MAX_DESIRED_SPEED_KPH );

            // figure out the middle point (when catchup == 0.0f)
            float tmpMidDesiredSpeed = playerVehicle->mSpeedKmh * MID_FACTOR;
            midDesiredSpeedKmh = rmt::Clamp( tmpMidDesiredSpeed,
                ABSOLUTE_MIN_DESIRED_SPEED_KPH,
                ABSOLUTE_MAX_DESIRED_SPEED_KPH );

            // figure out the max range (when catchup == 1.0f)
            float tmpMaxDesiredSpeed = playerVehicle->mSpeedKmh * MAX_FACTOR;
            maxDesiredSpeedKmh = rmt::Clamp( tmpMaxDesiredSpeed,
                ABSOLUTE_MIN_DESIRED_SPEED_KPH,
                ABSOLUTE_MAX_DESIRED_SPEED_KPH );


            // if within range of player, regardless of race position, 
            // try to keep up with the player... but not too successfully...
            if( roughDistToPlayerSqr <= rmt::Sqr( DIST_KEEP_UP_WITH_PLAYER ) )
            {
                // NOTE:
                // Figure out if it's better to do nothing here, or do what's below
                //catchUpFactor = 1.0f;
            }
            // ok, out of range... check race position
            else
            {
                // get the avatar's race info
                float playerDistToCheckpoint = -1.0f;
                int playerCurrCheckpoint = -1;
                int playerLapsCompleted = -1;
                player->GetRaceInfo( playerDistToCheckpoint,
                    playerCurrCheckpoint, playerLapsCompleted );

                // check laps first
                if( miNumLapsCompleted > playerLapsCompleted )
                {
                    catchUpFactor = -1.0f;
                }
                else if( miNumLapsCompleted < playerLapsCompleted )
                {
                    catchUpFactor = 1.0f;
                }
                else
                {
                    // Ok, now use race checkpoints
                    if( miCurrentCollectible > playerCurrCheckpoint )
                    {
                        catchUpFactor = -1.0f;
                    }
                    else if( miCurrentCollectible < playerCurrCheckpoint )
                    {
                        catchUpFactor = 1.0f;
                    }
                    else
                    {
                        // now use dist to curr checkpoint

                        // what's the difference between my dist and player dist?
                        float distDiff = rmt::Fabs( playerDistToCheckpoint - mDistToCurrentCollectible );

                        //catchUpFactor = ( distDiff - DIST_KEEP_UP_WITH_PLAYER ) / DIST_DELTA;
                        catchUpFactor = distDiff / DIST_MAX_CATCHUP;
                        catchUpFactor = rmt::Clamp( catchUpFactor, 0.0f, 1.0f );
                        if( mDistToCurrentCollectible < playerDistToCheckpoint )
                        {
                            catchUpFactor *= -1.0f;
                        }
                    }
                }
            }
        }
        break;
    case EVADE:
        {
            // Don't need the min range b/c catchup in this case ranges from 0 to 1
            // We never want to slow down for the player. At worst, we match his 
            // speed. When he comes close, we aim for 1.5x his speed

            float myTopSpeedKmh = GetVehicle()->mDesignerParams.mDpTopSpeedKmh;
            rAssert( myTopSpeedKmh >= mDesiredSpeedKmh );
            minDesiredSpeedKmh = mDesiredSpeedKmh;
            midDesiredSpeedKmh = mDesiredSpeedKmh;
            maxDesiredSpeedKmh = myTopSpeedKmh;

            // Tunable catch-up values used only in EVADE situation
            const float DIST_PLAYER_TOO_CLOSE_FOR_COMFORT = mCatchupParams.Evade.DistPlayerTooNear;
            const float DIST_PLAYER_NICE_AND_FAR = mCatchupParams.Evade.DistPlayerFarEnough;

            // Law & Order must be maintained!
            rAssert( DIST_PLAYER_NICE_AND_FAR > DIST_PLAYER_TOO_CLOSE_FOR_COMFORT );
            const float DIST_DELTA = DIST_PLAYER_NICE_AND_FAR - 
                DIST_PLAYER_TOO_CLOSE_FOR_COMFORT;

            float roughDistToPlayer = rmt::Sqrt( roughDistToPlayerSqr );

            // The closer the player is, the better the AI should perform
            catchUpFactor = (DIST_PLAYER_NICE_AND_FAR - roughDistToPlayer) / DIST_DELTA;
            catchUpFactor = rmt::Clamp( catchUpFactor, 0.0f, 1.0f );
        }
        break;
    case TARGET:
        {
            // wanna make it easy for the player to hit us... 
            // At our best, we never want to exceed the player's speed..
            // If getting too far from player, speed should approach near zero

            const float ABSOLUTE_MIN_DESIRED_SPEED_KPH = 
                playerVehicle->mDesignerParams.mDpTopSpeedKmh * 0.2f;
            const float ABSOLUTE_MAX_DESIRED_SPEED_KPH =
                playerVehicle->mDesignerParams.mDpTopSpeedKmh;

            minDesiredSpeedKmh = ABSOLUTE_MIN_DESIRED_SPEED_KPH;
            midDesiredSpeedKmh = ABSOLUTE_MAX_DESIRED_SPEED_KPH;
            maxDesiredSpeedKmh = ABSOLUTE_MAX_DESIRED_SPEED_KPH;

            // TUNABLE catch-up params used only in TARGET situation
            const float DIST_PLAYER_CLOSE_ENOUGH = mCatchupParams.Target.DistPlayerNearEnough; // dist at which catchup is 0.0f
            const float DIST_PLAYER_TOO_FAR = mCatchupParams.Target.DistPlayerTooFar; // dist at which catchup is -1.0f

            // Police our own values
            rAssert( DIST_PLAYER_TOO_FAR > DIST_PLAYER_CLOSE_ENOUGH );
            const float DIST_DELTA = DIST_PLAYER_TOO_FAR - DIST_PLAYER_CLOSE_ENOUGH;

            float roughDistToPlayer = rmt::Sqrt( roughDistToPlayerSqr );

            // the further the player is, the worse the AI should perform
            catchUpFactor = (DIST_PLAYER_CLOSE_ENOUGH - roughDistToPlayer) / DIST_DELTA;
            catchUpFactor = rmt::Clamp( catchUpFactor, -1.0f, 0.0f );
        }
        break;
    default:
        {
            rAssert( false );
        }
        break;
    }

    rAssert( minDesiredSpeedKmh <= midDesiredSpeedKmh && 
             midDesiredSpeedKmh <= maxDesiredSpeedKmh );

    mDesiredSpeedKmh = midDesiredSpeedKmh + catchUpFactor * ((catchUpFactor > 0.0f)? 
        (maxDesiredSpeedKmh - midDesiredSpeedKmh) : 
        (midDesiredSpeedKmh - minDesiredSpeedKmh) );

    mShortcutSkillMod = (int)(catchUpFactor * VehicleAI::CATCHUP_MAX_SHORTCUTSKILL_MOD);
}

void WaypointAI::UpdateNeedsResetOnSpot( float timeins )
{
    Vehicle* v = GetVehicle();
    if( v != NULL )
    {
        rmt::Vector up;
        rmt::Vector testUp( 0.0f, -1.0f, 0.0f );
        v->GetVUP( &up );
        float ep = -0.258819f;//cos100
        float alpha = up.Dot(testUp);

        if( !mNeedsResetOnSpot ) // if not reset is scheduled, see if we need to
        {
            if( v->mVehicleDestroyed && this->mAutoResetOnDestroyed )
            {
                mNeedsResetOnSpot = true;
                mSecondsTillResetOnSpot = SECONDS_BEFORE_RESET_ON_SPOT;
            }
            else
            {
                // Test to make sure that we're not flipped over.
                // If so, schedule a reset...
                if( alpha >= ep )
                {
                    // this means our up vector is quite close to the 
                    // upside-down vector. Reset!
                    mNeedsResetOnSpot = true;
                    mSecondsTillResetOnSpot = SECONDS_BEFORE_RESET_ON_SPOT;
                }
            }
        }
        else // here we're counting down till reset
        {
            if( !v->mVehicleDestroyed || !mAutoResetOnDestroyed )
            {
                // if we're not resetting cuz we were destroyed 
                // and the vector corrected itself, cancel the reset
                if( alpha < ep ) 
                {
                    mNeedsResetOnSpot = false;
                }
            }

            // here we still need to reset, continue with reset countdown
            if( mNeedsResetOnSpot )
            {
                if( mSecondsTillResetOnSpot <= 0.0f )
                {
                    // countdown finished.. Do reset
                    // Figure out if we need to reset damage states
                    bool resetDamageStates = 
                        (v->mVehicleDestroyed && mAutoResetOnDestroyed)? true : false;

                    v->ResetOnSpot( resetDamageStates );
                    mNeedsResetOnSpot = false;
                }
                else
                {
                    // countdown not complete... keep goin
                    mSecondsTillResetOnSpot -= timeins;
                }
            }
        }
    }
}


void WaypointAI::ClearWaypoints()
{
    miNumWayPoints = 0;
    miNextWayPoint = 0;
    miCurrentWayPoint = -1;
}

void WaypointAI::AddWaypoint( Locator* loc )
{
    rAssert( miNumWayPoints < MAX_WAYPOINTS );
    rAssert( loc != NULL );

    rmt::Vector locPos;
    loc->GetLocation( &locPos );

    float dummy;
    RoadSegment* closestSeg = NULL;
    GetIntersectManager()->FindClosestAnyRoad( locPos, 100.0f, closestSeg, dummy );
    
#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
    char errMsg[512];
    sprintf( errMsg, 
        "Waypoint at %.1f, %.1f, %.1f is not within %.1f meters of a road segment! "
        "Please contact Darren Evenson or Dusit for a fix!\n", 
        locPos.x, locPos.y, locPos.z, mTriggerRadius );
    rTuneAssertMsg( dummy < mTriggerRadius*mTriggerRadius, errMsg );
#endif

    RoadSegment* tmpSeg = (RoadSegment*) closestSeg;
    float tmpSegT = RoadManager::DetermineSegmentT( locPos, tmpSeg );
    float tmpRoadT = RoadManager::DetermineRoadT( tmpSeg, tmpSegT );

    mpWayPoints[ miNumWayPoints ].loc = loc;
    mpWayPoints[ miNumWayPoints ].seg = tmpSeg;
    mpWayPoints[ miNumWayPoints ].segT = tmpSegT;
    mpWayPoints[ miNumWayPoints ].roadT = tmpRoadT;
    mpWayPoints[ miNumWayPoints ].elem.elem = tmpSeg->GetRoad();
    mpWayPoints[ miNumWayPoints ].elem.type = RoadManager::ET_NORMALROAD;

    miNumWayPoints += 1;
}

bool WaypointAI::MustRepopulateSegments()
{
    return mCurrWayPointHasMoved;

}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

void WaypointAI::SetCurrentWayPoint( int index )
{
    miCurrentWayPoint = index;

    if(( miCurrentWayPoint >= 0 ) && ( miCurrentWayPoint < miNumWayPoints ))
    {
        mCurrWayPointHasMoved = true;
        int next = miCurrentWayPoint + 1;
        miNextWayPoint = next;
    }
}

void WaypointAI::FollowWaypoints()
{
    mCurrWayPointHasMoved = false;
    if( miNumWayPoints <= 0 )
    {
        return;
    }
    
    if( miCurrentWayPoint < 0 )
    {
        SetCurrentWayPoint( 0 );
    }

    if ( miCurrentWayPoint < miNumWayPoints )
    {
        if ( TestWaypoint( miCurrentWayPoint ) )
        {
            //Im at the currentWaypoint.  Time to choose another or stop if this
            //is the end of the line.
            GetEventManager()->TriggerEvent( EVENT_WAYAI_HIT_WAYPOINT, NULL );

            if( miCurrentWayPoint >= miNumWayPoints-1 )
            {
                // loop again! whee!

                // broadcast to folks that I just reached my last waypoint.. 
                // it will be a repeat of the last normal HIT_WAYPOINT event
                GetEventManager()->TriggerEvent( EVENT_WAYAI_HIT_LAST_WAYPOINT, GetVehicle() );

                // TODO:
                // We might want to expose control over looping here.
                // For example, introduce an mLooping variable that is set
                // upon construction, exposed to the designers via mission script
                // command. If mLooping is true, we go to beginning... if not
                // then SetActive( false ).
                // For now, default to YES! We are looping! The governing race
                // logic should do the calling of SetActive( false ) when we've
                // reached a certain waypoint if they want us to stop.
                miNextWayPoint = 0;
            }
            SetCurrentWayPoint( miNextWayPoint );
        }
    }
}


bool WaypointAI::TestReachedTarget( const rmt::Vector& start, const rmt::Vector& end )
{
    if( miNumWayPoints > 0 )//miCurrentWayPoint >= 0 )
    {
        rAssert( miCurrentWayPoint >= 0 );

        int point = miCurrentWayPoint;
        if ( point >= miNumWayPoints )
        {
            point = 0;
        }
        rAssert( mpWayPoints[ point ].loc );

        rmt::Vector wayLoc;
        mpWayPoints[ point ].loc->GetLocation( &wayLoc );

        rmt::Vector closestPt;
        FindClosestPointOnLine( start, end, wayLoc, closestPt );

        float distSqr = (closestPt - wayLoc).MagnitudeSqr();
        return ( distSqr < (mTriggerRadius*mTriggerRadius) );
    }
    
    // if no current waypoint, dont' go anywhere, we're already here...
    return true;

}

void WaypointAI::GetClosestPathElementToTarget( 
    rmt::Vector& targetPos,
    RoadManager::PathElement& elem,
    RoadSegment*& seg,
    float& segT,
    float& roadT )
{
    elem.elem = NULL;
    seg = NULL;
    segT = 0.0f;
    roadT = 0.0f;

    if( miNumWayPoints > 0 )
    {
        rAssert( miCurrentWayPoint >= 0 );

        int point = miCurrentWayPoint;
        if ( point >= miNumWayPoints )
        {
            point = 0;
        }
        rAssert( mpWayPoints[ point ].loc );

        mpWayPoints[ point ].loc->GetLocation( &targetPos );
        elem = mpWayPoints[ point ].elem;
        seg = mpWayPoints[ point ].seg; 
        segT = mpWayPoints[ point ].segT;
        roadT = mpWayPoints[ point ].roadT;
    }
}

int WaypointAI::RegisterHudMapIcon()
{
    int iconID = -1;

    rmt::Vector initialLoc;
    GetVehicle()->GetPosition( &initialLoc );

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        switch( mWaypointAIType )
        {
            case RACE:
            {
                iconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_AI_RACE, initialLoc, this );

                break;
            }
            case EVADE:
            {
                iconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_AI_EVADE, initialLoc, this );

                break;
            }
            case TARGET:
            {
                iconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_AI_TARGET, initialLoc, this );

                break;
            }
            default:
            {
                rAssertMsg( false, "Invalid waypoint AI type!" );

                break;
            }
        }
    }

    return iconID;
}

bool WaypointAI::TestWaypoint( int waypoint )
{
    rmt::Vector mypos;
    GetVehicle()->GetPosition( &mypos );

    rmt::Vector vec2dest;
    mpWayPoints[ waypoint ].loc->GetLocation( &vec2dest );
    vec2dest.Sub( mypos );
    float dist = vec2dest.MagnitudeSqr();
    return ( dist < mTriggerRadius*mTriggerRadius );
}
