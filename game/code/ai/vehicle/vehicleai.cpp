//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicleai.cpp
//
// Description: Implement VehicleAI
//
// History:     27/06/2002 + Created -- NAME
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

#include <ai/vehicle/vehicleai.h>
#include <ai/vehicle/vehicleairender.h>
#include <memory/classsizetracker.h>
#include <meta/locator.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <roads/intersection.h>
#include <roads/road.h>
#include <roads/roadmanager.h>
#include <roads/roadsegment.h>
#include <roads/geometry.h>

#include <simcollision/collisionmanager.hpp>
//#include <simcollision/collisionvolume.hpp>

#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/worldphysicsmanager.h>

#include <mission/missionmanager.h>
#include <mission/mission.h>
#include <mission/objectives/missionobjective.h>
#include <mission/objectives/destroyobjective.h>
#include <mission/objectives/collectdumpedobjective.h>

#include <debug/profiler.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

const bool EVADE_VEHICLES = true;
const bool EVADE_STATICS = false;

// how long going forward at minspeed do we allow before reversing
const unsigned int	FIRST_STUCK_TIME = 700;//500; 

// how long after reversing, going forward at speed <= minspeed, 
// do we allow before reversing again...
const unsigned int	NEXT_STUCK_TIME = 1500;//2500;

// number of milliseconds we spend applying reverse
const float REVERSE_BACKOFF      = 200.0f;//1000;
const float DEFAULT_REVERSE_TIME = 500.0f - REVERSE_BACKOFF;//1000;
const float MAX_REVERSE_TIME  = 1600.0f; // after this, just reset..
const float STILL_STUCK_SPEED = 14.0f;

//
// Distances the AI looks ahead for steering
const float         LOOK_AHEAD_CLOSE_SECONDS = 0.8f;//0.7f;
const float         LOOK_AHEAD_FAR_SECONDS = 1.0f;//1.0f;
const float         MIN_LOOK_AHEAD_CLOSE = 5.0f;//2.0f;
const float         MAX_LOOK_AHEAD_CLOSE = 1000.0f;//20.0f;
const float         MIN_LOOK_AHEAD_FAR   = 10.0f;//5.0f;
const float         MAX_LOOK_AHEAD_FAR   = 1000.0f;//40.0f;

//
// A low number favours the far look-ahead, a high number
// favours the close look-ahead
const float			DEFAULT_STEERING_RATIO = 1.0f;//0.5f;
const float         AVOID_STEERING_RATIO = 1.0f;//0.55f;

// A low number makes the gas more dependent on the steering,
// a high number makes it less.
const float			GAS_STEERING_RATIO = 0.50f;

// A low number favours the turn of the wheels, a high number favours
// the heading-to-destination
const float			STEER_HEADING_RATIO = 0.50f; 

// Default shortcut skills
const int           VehicleAI::DEFAULT_MIN_SHORTCUT_SKILL = 15;
const int           VehicleAI::DEFAULT_MAX_SHORTCUT_SKILL = 25;

// Shortcut & Speed, Catch-up values

//const float         VehicleAI::CATCHUP_MAX_SPEED_MOD = 20.0f;
const float         VehicleAI::CATCHUP_NORMAL_DRIVING_PERCENTAGE_OF_TOPSPEED = 0.7f;
const int           VehicleAI::CATCHUP_MAX_SHORTCUTSKILL_MOD = 20;

// number of seconds to start counting down before we repopulate
// giving us a chance to get back on path
const float         SECONDS_REPOPULATE_DELAY = 3.0f; 


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// VehicleAI::VehicleAI
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
VehicleAI::VehicleAI( 
    Vehicle* pVehicle, 
    VehicleAITypeEnum type,  
    bool enableSegmentOptimization,
    int minShortcutSkill,
    int maxShortcutSkill,
    bool useMultiplier ) :

AiVehicleController( pVehicle ),
mNumSegments( 0 ),
mSecondsStunned( 0.0f ),
mSecondsOutOfControl( 0.0f ),

mCurrPathElement( -1 ),

mLastRoadSegment( NULL ),
mLastRoadSegmentT( 0.0f ),
mLastRoadT( 0.0f ),

mRaceRoadSegment( NULL ),
mRaceRoadSegmentT( 0.0f ),
mRaceRoadT( 0.0f ),

mSecondsSinceLastDoCatchUp( 0.0f ),

/*** DEPRECATED: TO BE REMOVED ***
mDestIndex( -1 ),
mpRoad( NULL ),
mpSegment( NULL ),
*********************************/

mStartStuckTime( 0 ),
mNextStuckTime( FIRST_STUCK_TIME ),
mSteeringRatio( DEFAULT_STEERING_RATIO ),
mState( STATE_WAITING ),
mLimboPushedState( STATE_WAITING ),
mType( type ),
mHudIndex( -1 ),
mSecondsBeforeCorner( 0.0f ),
mRenderHandle( -1 ),
mSecondsLeftToGetBackOnPath( SECONDS_REPOPULATE_DELAY ),
mReverseTime(DEFAULT_REVERSE_TIME),
mEvadeVehicles( EVADE_VEHICLES ),
mEvadeStatics( EVADE_STATICS ),
mEvading( false ),
mEnableSegmentOptimization( enableSegmentOptimization ),
mUseMultiplier( useMultiplier )
{
    CLASSTRACKER_CREATE( VehicleAI );
    rAssert( minShortcutSkill >= 0 );
    rAssert( maxShortcutSkill >= 0 );
    rAssert( minShortcutSkill <= maxShortcutSkill );
    if( minShortcutSkill > maxShortcutSkill )
    {
        mMinShortcutSkill = maxShortcutSkill;
        mMaxShortcutSkill = minShortcutSkill;
    }
    else
    {
        mMinShortcutSkill = minShortcutSkill;
        mMaxShortcutSkill = maxShortcutSkill;
    }


    mDestination.Set( 0.0f, 0.0f, 0.0f );
    mNextDestination.Set( 0.0f, 0.0f, 0.0f );

    int maxPathElems = RoadManager::GetInstance()->GetMaxPathElements();
    mPathElements.Allocate( maxPathElems );

    mLastPathElement.elem = NULL;
    mRacePathElement.elem = NULL;

    ////////////////////////////////////////
    // Initialize catchup params with defaults, ONCE on construction 

    mCatchupParams.Race.DistMaxCatchup = 80.0f; 
    mCatchupParams.Race.FractionPlayerSpeedMinCatchup = 0.5f;
    mCatchupParams.Race.FractionPlayerSpeedMidCatchup = 1.1f;
    mCatchupParams.Race.FractionPlayerSpeedMaxCatchup = 1.7f;

    mCatchupParams.Evade.DistPlayerTooNear = 20.0f;
    mCatchupParams.Evade.DistPlayerFarEnough = 70.0f;

    mCatchupParams.Target.DistPlayerNearEnough = 20.0f;
    mCatchupParams.Target.DistPlayerTooFar = 70.0f;


    //////////////////////////////////////////
    // Reset temp catchup params
    ResetCatchUpParams();
}

//==============================================================================
// VehicleAI::~VehicleAI
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
VehicleAI::~VehicleAI()
{
    CLASSTRACKER_DESTROY( VehicleAI );
    mPathElements.Clear();
    Finalize();
}


void VehicleAI::SetRaceCatchupParams( const RaceCatchupParams& raceParams )
{
    rTuneAssertMsg( raceParams.DistMaxCatchup > 0.0f, 
        "Race Catchup: Dist Max Catchup must be > zero" );

    mCatchupParams.Race = raceParams;
}
void VehicleAI::SetEvadeCatchupParams( const EvadeCatchupParams& evadeParams )
{
    rTuneAssertMsg( evadeParams.DistPlayerFarEnough > 0.0f, 
        "Evade Cathcup: DistPlayerFarEnough must be > zero" );
    rTuneAssertMsg( evadeParams.DistPlayerTooNear > 0.0f,
        "Evade Catchup: DistPlayerTooNear must be > zero" );
    rTuneAssertMsg( evadeParams.DistPlayerFarEnough > evadeParams.DistPlayerTooNear,
        "Evade Catchup: DistPlayerFarEnough must be > DistPlayerTooNear" );

    mCatchupParams.Evade = evadeParams;
}
void VehicleAI::SetTargetCatchupParams( const TargetCatchupParams& targetParams )
{
    rTuneAssertMsg( targetParams.DistPlayerTooFar > 0.0f, 
        "Evade Cathcup: DistPlayerTooFar must be > zero" );
    rTuneAssertMsg( targetParams.DistPlayerNearEnough > 0.0f,
        "Evade Catchup: DistPlayerNearEnough must be > zero" );
    rTuneAssertMsg( targetParams.DistPlayerTooFar > targetParams.DistPlayerNearEnough,
        "Evade Catchup: DistPlayerTooFar must be > DistPlayerNearEnough" );

    mCatchupParams.Target = targetParams;
}



void VehicleAI::ResetControllerValues()
{
    // reset the controller values we use.
    mGas.SetValue( 0.0f );
    mBrake.SetValue( 0.0f );
    mHandBrake.SetValue( 0.0f );
    mSteering.SetValue( 0.0f );
    mReverse.SetValue( 0.0f );
    mHorn.SetValue( 0.0f );
}

void VehicleAI::Reset()
{
    mSecondsStunned = 0.0f;
    mSecondsOutOfControl = 0.0f;

    mNumSegments = 0;
    mPathElements.ClearUse();

    mCurrPathElement = -1;

    mLastPathElement.elem = NULL;
    mLastRoadSegment = NULL;
    mLastRoadSegmentT = 0.0f;
    mLastRoadT = 0.0f;

    mRacePathElement.elem = NULL;
    mRaceRoadSegment = NULL;
    mRaceRoadSegmentT = 0.0f;
    mRaceRoadT = 0.0f;


    // updates mLastPathElement, mLastRoadSegment, mLastRoadSegmentT, mLastRoadT
    UpdateSelf();
    // 
    // Place a check here to see which vehicle wasn't started properly on a 
    // locator. This also enforces that whoever initializes this AI has 
    // already placed down the vehicle at its proper location before calling
    // Initialize()
    //
    rTuneAssertMsg( (mLastPathElement.elem != NULL && mLastRoadSegment != NULL), 
        "Initialize() called on an AI car that was not on a road segment!\n" );

    mSecondsSinceLastDoCatchUp = 0.0f;

    mStartStuckTime = 0;
    mNextStuckTime = FIRST_STUCK_TIME;
    mSteeringRatio = DEFAULT_STEERING_RATIO;

    mSecondsBeforeCorner = 0.0f;
    mSecondsLeftToGetBackOnPath = SECONDS_REPOPULATE_DELAY;
    mEvading = false;

    /////////////////////////////////////////////////////
    mDestination.Set( 0.0f, 0.0f, 0.0f );
    mNextDestination.Set( 0.0f, 0.0f, 0.0f );
    ResetCatchUpParams();

    ResetControllerValues();
}

// Before calling Initialize, the vehicle's location should be 
// properly set on a road segment!
void VehicleAI::Initialize()
{
    Reset();

    mState = STATE_WAITING;
    mLimboPushedState = STATE_WAITING;

#ifdef DEBUGWATCH
    mRenderHandle = VehicleAIRender::GetVehicleAIRender()->RegisterAI( this );
    rAssert( mRenderHandle != -1 );
#else
    mRenderHandle = -1;
#endif
}


void VehicleAI::Finalize()
{
    SetActive( false );
#ifdef DEBUGWATCH
    if( mRenderHandle >= 0 )
    {
        VehicleAIRender::GetVehicleAIRender()->UnregisterAI( mRenderHandle );
    }
#endif
}

//=============================================================================
// VehicleAI::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeins )
//
// Return:      void 
//
//=============================================================================
void VehicleAI::Update( float timeins )
{
BEGIN_PROFILE( "VehicleAI Update" );    
    if( GetState() == STATE_LIMBO )
    {
        return;
    }
    UpdateSelf();
    UpdateSegments();
    FollowRoad();
    DoSteering();
    DoCatchUp( timeins );
    CheckState( timeins );
END_PROFILE( "VehicleAI Update" );    
}

void VehicleAI::SetActive( bool bIsActive )
{
    if( bIsActive )
    {
        mState = STATE_ACCEL;
        //mpRoad = NULL;

        if( mHudIndex == -1 )
        {
/*
            rmt::Vector initialLoc;
            GetVehicle()->GetPosition( &initialLoc );

            CGuiScreenMultiHud* currentHud = GetCurrentHud();
            if( currentHud )
            {
                HudMapIcon::eIconType iconType = mType == AI_CHASE ? HudMapIcon::ICON_AI_HARASS_CAR : HudMapIcon::ICON_AI_CAR;
                mHudIndex = currentHud->GetHudMap( 0 )->RegisterIcon( iconType, initialLoc, this );
            }
*/
            mHudIndex = this->RegisterHudMapIcon();
        }
    }
    else
    {
        if( mHudIndex >= 0 )
        {
            CGuiScreenMultiHud* currentHud = GetCurrentHud();
            if( currentHud )
            {
                currentHud->GetHudMap( 0 )->UnregisterIcon( mHudIndex );
            }
            mHudIndex = -1;
        }

        SetState( STATE_WAITING );
    }
}

void VehicleAI::GetPosition( rmt::Vector* currentLoc ) 
{ 
    GetVehicle()->GetPosition( currentLoc ); 
}

void VehicleAI::GetHeading( rmt::Vector* heading )
{
    GetVehicle()->GetHeading( heading );
}

void VehicleAI::SetMaxShortcutSkill( int skill )
{
    mMaxShortcutSkill = skill;
}
int VehicleAI::GetMaxShortcutSkill()
{
    return mMaxShortcutSkill;
}
void VehicleAI::SetMinShortcutSkill( int skill )
{
    mMinShortcutSkill = skill;
}
int VehicleAI::GetMinShortcutSkill()
{
    return mMinShortcutSkill;
}
int VehicleAI::GetShortcutSkillMod()
{
    return mShortcutSkillMod;
}
void VehicleAI::EnterLimbo()
{
    if( mState != STATE_LIMBO )
    {
        //makes the AI goto a limbo state
        mLimboPushedState = mState;
        SetState(STATE_LIMBO);

        // clear all inputs
        ResetControllerValues();

        // fudge handbrake, so we don't drift..
        mHandBrake.SetValue( 1.0f );
    }
}
void VehicleAI::ExitLimbo()
{
    if( mState == STATE_LIMBO )
    {
        //get out of limbo and enter waiting state
        SetState(mLimboPushedState);
    }
}


int VehicleAI::DetermineShortcutSkill()
{
    if( mMaxShortcutSkill < mMinShortcutSkill )
    {
        int temp = mMaxShortcutSkill;
        mMaxShortcutSkill = mMinShortcutSkill;
        mMinShortcutSkill = temp;
    }

    // roll a number between min skill and max skill
    int diff = mMaxShortcutSkill - mMinShortcutSkill;
    int roll = (rand() % diff) + mMinShortcutSkill;

    return (roll + mShortcutSkillMod);
}

void VehicleAI::ResetCatchUpParams()
{
    rAssert( GetVehicle() );
    mDesiredSpeedKmh = GetVehicle()->mDesignerParams.mDpTopSpeedKmh * 
        CATCHUP_NORMAL_DRIVING_PERCENTAGE_OF_TOPSPEED;
    mShortcutSkillMod = 0;
}

void VehicleAI::FillPathElements()
{
    rAssert( mPathElements.IsSetUp() );

    ////////////////////////////////////////////////////////////////////////
    // clear the list to be refilled 
    mPathElements.ClearUse();

    if( mLastPathElement.elem == NULL )
    {
        // TODO:
        // This VehicleAI was not created on a road segment or in an intersection.
        // This case happens when a new AI is put in to replace some old AI of some
        // mission vehicle, at the time when the old AI was not on a road segment
        // or in an intersection.
        // Since pathfinding requires it's on some sort of path element, this car 
        // is going nowhere fast.
        // More eloquent solution pending. Perhaps use IntersectionManager::FindClosestRoad?
        return;
    }

    rmt::Vector sourcePos;
    GetPosition( &sourcePos );

    /////////////////////////////////////////////////////////////////////
    // find our target element
    rmt::Vector targetPos;
    RoadManager::PathElement targetElem;
    RoadSegment* targetSeg = NULL;
    float targetSegT = 0.0f;
    float targetRoadT = 0.0f;

    GetClosestPathElementToTarget( 
        targetPos, targetElem, targetSeg, targetSegT, targetRoadT );

    if( targetElem.elem == NULL ) // target element can be NULL
    {
        return;
    }

    //////////////////////////////////////////////////////////////////////
    // invoke the pathfinder
    RoadManager::GetInstance()->FindPathElementsBetween( 
        mUseMultiplier,
        mLastPathElement, mLastRoadT, sourcePos, 
        targetElem, targetRoadT, targetPos, 
        mPathElements );
}
    
void VehicleAI::UpdateSelf()
{
    rmt::Vector myPos;
    GetVehicle()->GetPosition( &myPos );

    // find our source element
    RoadManager::PathElement elem;
    RoadSegment* seg = NULL;
    float segT = 0.0f;
    float roadT = 0.0f;

    /*
    RoadManager::PathfindingOptions options = RoadManager::PO_SEARCH_SHORTCUTS;
    RoadManager::GetInstance()->FindClosestPathElement( 
        myPos, 100.0f, options, elem, seg, segT, roadT );
    */

    // Special initial case: 
    // If my last path info is completely unset (started off the road)
    // then we fetch the closest path element using Devin's thing 
    if( mLastPathElement.elem == NULL && mLastRoadSegment == NULL )
    {
        RoadSegment* closestSeg = NULL;
        float dummy;
        GetIntersectManager()->FindClosestAnyRoad( myPos, 100.0f, closestSeg, dummy );

        seg = (RoadSegment*) closestSeg;
        segT = RoadManager::DetermineSegmentT( myPos, seg );
        roadT = RoadManager::DetermineRoadT( seg, segT );
        elem.elem = seg->GetRoad();
        elem.type = RoadManager::ET_NORMALROAD;
    }
    else
    {
        FindClosestPathElement( myPos, elem, seg, segT, roadT, true );
    }

    if( elem.elem == NULL )
    {
        return;
    }

    rAssert( elem.elem != NULL );

    // update race stuff..
    mRacePathElement = elem;
    if( seg )
    {
        mRaceRoadSegment = seg;
        mRaceRoadSegmentT = segT;
        mRaceRoadT = roadT;
    }


    /////////////////////////////////////////////////////////////
    // THE RULES (assuming last element and curr element are different)
    //  -----------------------
    // | LAST =======> CURRENT |
    //  -----------------------
    // 
    // OK means we don't ignore this new pathelement
    // Not OK means we ignore the new pathelement and continue using last element
    //
    // shortcut =====> intersection  
    //      This is OK as long as intersection is the destination 
    //      intersection of that shortcut road.
    //
    // shortcut =====> normalroad    
    //      This is not OK 
    //
    // shortcut =====> shortcut      
    //      This is not OK 
    // 
    // normalroad ===> intersection  
    //      This is OK as long as intersection is either source or dest 
    //      intersection of this non-shortcut road.
    // 
    // normalroad ===> normalroad    
    //      This is OK because cars transit from road to road all 
    //      the time at corners, skipping over intervening intersection
    // 
    // normalroad ===> shortcut
    //      This is not OK
    // 
    // intersection => intersection
    //      This is not OK
    //
    // intersection => normalroad
    //      This is OK as long as the road is connected to the intersection
    //
    // intersection => shortcut
    //      This is OK as long as the shortcut is an OUT road from the 
    //      intersection---i.e. the shortcut road's source intersection 
    //      is this intersection.
    //
    // NULL  ========> anything else
    //      This is OK
    //

    //
    // true means not to ignore the path element we just found
    // default is true because we could have lastpathelement == elem
    // but we could be on different segments...
    //
    bool OK = true; 
    //bool recomputePath = false;

    if( mLastPathElement != elem )
    {
        if( mLastPathElement.elem == NULL )
        {
            OK = true;
        }
        else
        {
            Road* lastRoad = NULL;
            Intersection* lastInt = NULL;
            Road* currRoad = NULL;
            Intersection* currInt = NULL;

            // temporary type: 0 = intersection, 1 = road, 2 = shortcut

            // Figure out what type is LAST
            int lastElemType = 0; 
            if( mLastPathElement.type == RoadManager::ET_NORMALROAD )
            {
                lastRoad = (Road*) mLastPathElement.elem;
                lastElemType++;

                if( lastRoad->GetShortCut() )
                {
                    lastElemType++;
                }
            }
            else
            {
                lastInt = (Intersection*) mLastPathElement.elem;
            }

            // Figure out what type is CURR
            int currElemType = 0;
            if( elem.type == RoadManager::ET_NORMALROAD )
            {
                currRoad = (Road*) elem.elem;
                currElemType++;

                if( currRoad->GetShortCut() )
                {
                    currElemType++;
                }
            }
            else
            {
                currInt = (Intersection*) elem.elem;
            }

            //
            // Start resolving ...
            ///////////////
            // LAST: Intersection
            if( lastElemType == 0 ) 
            {
                rAssert( lastInt );
                if( currElemType == 2 )// CURR: shortcut
                {
                    rAssert( currRoad );
                    if( lastInt == (Intersection*) currRoad->GetSourceIntersection() ||
                        lastInt == (Intersection*) currRoad->GetDestinationIntersection() )
                    {
                        OK = true;
                    }
                    else
                    {
                        OK = false;
                    }
                }
                else
                {
                    OK = true;
                }
               /*
                if( currElemType == 0 ) // CURR: intersection
                {
                    rAssert( currInt );
                    OK = false;
                }
                else if( currElemType == 1 ) // CURR: normal road
                {
                    rAssert( currRoad );
                    if( lastInt == (Intersection*) currRoad->GetSourceIntersection() ||
                        lastInt == (Intersection*) currRoad->GetDestinationIntersection() )
                    {
                        OK = true;
                    }
                    else
                    {
                        OK = false;
                    }
                }
                else // CURR: shortcut
                {
                    rAssert( currRoad );
                    if( lastInt == (Intersection*) currRoad->GetSourceIntersection() )
                    {
                        OK = true;
                    }
                    else
                    {
                        OK = false;
                    }
                }
                */
            }
            /////////////////
            // LAST: Normal road 
            else if( lastElemType == 1 ) 
            {
                rAssert( lastRoad );

                if( currElemType == 0 ) // CURR: intersection
                {
                    rAssert( currInt );
                    if( currInt == (Intersection*)lastRoad->GetSourceIntersection() ||
                        currInt == (Intersection*)lastRoad->GetDestinationIntersection() )
                    {
                        OK = true;
                    }
                    else
                    {
                        OK = false;
                    }
                }
                else if( currElemType == 1 ) // CURR: normal road
                {
                    rAssert( currRoad );
                    OK = true;
                    /*
                    if( lastRoad->GetSourceIntersection() == currRoad->GetDestinationIntersection() &&
                        lastRoad->GetDestinationIntersection() == currRoad->GetSourceIntersection() )
                    {
                        OK = false;
                    }
                    else
                    {
                        OK = true;
                    }
                    */
                }
                else // CURR: shortcut
                {
                    rAssert( currRoad );
                    OK = false;
                }
            }
            ////////////////
            // LAST: Shortcut
            else
            {
                rAssert( lastRoad );

                if( currElemType == 0 ) // CURR: intersection
                {
                    rAssert( currInt );

					// NOTE:
					// This is the fix to Level 3 Mission 7 where the limo drives over the where the shortcuts 
					// and normal road segments merge near the squidport.... This was where its last good element
					// was found... It then transitted onto the intersection, but rejected it because back then
					// we only tested for the destination intersection... So it was stuck there for a long time
					// but didn't realize it until it hit the next waypoint (wayy... over by the dam) and decided
					// it needed to turn back. 
                    if( currInt == (Intersection*)lastRoad->GetDestinationIntersection() ||
						currInt == (Intersection*)lastRoad->GetSourceIntersection() )
                    {
                        OK = true;

                        // TODO:
                        // Tell AI to recompute?
                        // recomputePath = true;
                    }
                    else
                    {
                        OK = false;
                    }
                }
                else if( currElemType == 1 ) // CURR: normal road
                {
                    rAssert( currRoad );
                    OK = false;
                }
                else // CURR: shortcut
                {
                    rAssert( currRoad );
                    OK = false;
                }
            }
        }
    }

    if( OK )
    {
        mLastPathElement = elem;
        if( seg )
        {
            if( mLastRoadSegment != seg )
            {
                mLastRoadSegment = seg;
            }
            mLastRoadSegmentT = segT;
            mLastRoadT = roadT;
        }
        //if( recomputePath )
        //{
        //    mNumSegments = 0;
        //    FillPathElements();
        //}
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

void VehicleAI::FollowRoad()
{
    // Traverse mSegments for some lookaheaddistance meters, if
    // FillSegments was successful...
    if( mNumSegments > 0 )
    {
        rmt::Vector myPos;
        GetVehicle()->GetPosition( &myPos );

        // determine our progress along the first Segment...
        rmt::Vector toMyPos, segForward;
        segForward = mSegments[0].mEnd - mSegments[0].mStart;
        toMyPos = myPos - mSegments[0].mStart;

        float scale = segForward.Dot(toMyPos) / segForward.Dot(segForward);
        rmt::Vector proj = segForward * scale;

        float t = 0.0f;
        if( scale >= 0.0f )
        {
            // projection is going in same direction as segment,
            // so we can safely produce a "t" value here...
            t = proj.Length() / mSegments[0].mLength; // *** SQUARE ROOT! ***
        }
        //rAssert( 0.0f <= t && t <= 1.0f );

        //
        // determine how far we want to look ahead for first destination
        // 
        float lookAheadCloseDist;
        
        if(GetVehicle()->IsInReverse())
        {
            lookAheadCloseDist = MIN_LOOK_AHEAD_CLOSE;
        }
        else
        {
            lookAheadCloseDist = rmt::Clamp( GetVehicle()->mSpeed * LOOK_AHEAD_CLOSE_SECONDS,
            MIN_LOOK_AHEAD_CLOSE, MAX_LOOK_AHEAD_CLOSE );

        }

        rmt::Vector lookAheadClose;
        GetPosAheadAlongRoad( t, lookAheadCloseDist, 0, &lookAheadClose );
        SetDestination( lookAheadClose );


        //
        // determine how far we want to look ahead for second destination
        //
        float lookAheadFarDist = rmt::Clamp( 
            GetVehicle()->mSpeed * LOOK_AHEAD_FAR_SECONDS,
            MIN_LOOK_AHEAD_FAR, MAX_LOOK_AHEAD_FAR );

        rmt::Vector lookAheadFar;
        GetPosAheadAlongRoad( t, lookAheadFarDist, 0, &lookAheadFar );
        SetNextDestination( lookAheadFar );

        rAssert( !rmt::IsNan(t) );
    }
    return;
}

void VehicleAI::FindClosestSegment( const rmt::Vector& pos, 
                                    int& closestIndex, 
                                    float& closestDistSqr, 
                                    rmt::Vector& closestPt )
{
    rAssert( 0 <= mNumSegments && mNumSegments <= MAX_SEGMENTS );

    closestIndex = -1;
    closestDistSqr = 100.0f; // this is the max distance we'll allow to be considered "near enough"
                             // to a segment. If we never find something at least this close,
                             // caller to this method should attempt to repopulate the segments list 

    for( int i=0; i<mNumSegments; i++ )
    {
        rmt::Vector closestSegPos;
        FindClosestPointOnLine( mSegments[i].mStart, mSegments[i].mEnd, pos, closestSegPos );

        rmt::Vector toPos = pos - closestSegPos;
        float distToPosSqr = toPos.MagnitudeSqr();
        if( distToPosSqr <= closestDistSqr )
        {
            closestPt = closestSegPos;
            closestDistSqr = distToPosSqr;
            closestIndex = i;
        }
    }
    /*
    rDebugPrintf( " *** closest segment == %d, distance = %f, pos = (%f,%f,%f), segStart = (%f,%f,%f)\n", 
        closestIndex, rmt::Sqrt(closestDistSqr), pos.x, pos.y, pos.z, 
        mSegments[closestIndex].mStart.x, mSegments[closestIndex].mStart.y, mSegments[closestIndex].mStart.z);
    */
}

// this resets the segments
void VehicleAI::ResetSegments()
{
    mNumSegments = 0;
}


//
// Replace element at index "first" with element at index "first+numShifts"
//     and element at index "first+1" with element at index "first+numShifts+1"
//     and so forth till "first+numShifts+i" == mNumSegments-1
void VehicleAI::ShiftSegments( int numShifts, int first )
{
    rAssert( 0 <= first && first < mNumSegments );
    rAssert( 0 <= (first+numShifts) && (first+numShifts) < mNumSegments );

    if( numShifts == 0 )
    {
        return;
    }

    mNumSegments -= numShifts;
    for( int i=first; i<mNumSegments; i++ )
    {
        rAssert( 0 <= i && i < mNumSegments );
        mSegments[i] = mSegments[i+numShifts];
    }
}

void VehicleAI::FillSegments()
{
    rmt::Vector myPos;
    GetPosition( &myPos );

    ////////////////////////////// METHOD 1: ITERATING OVER PATHELEMENTS /////////////////////////
    //
    // If we need to repopulate for any reasons:
    //  - we reached the target (waypoint) or target (chase) has moved to another pathelement
    //  - we ran out of segments
    //  - we ran out of path elements
    //  - we are > 10 meters from all of the segments in our segments arrray
    //
    // Then recompute path and refill segments from scratch
    //
    if( mNumSegments == 0 || mCurrPathElement == mPathElements.mUseSize )
    {
        mNumSegments = 0;

        /*
        // re-search for our nearest road only if we're not a chase AI
        // (cuz chase AI repopulates too often, this will take too long)
        if( mType == AI_WAYPOINT )
        {
            mLastPathElement.elem = NULL;
            mLastRoadSegment = NULL;
            UpdateSelf();
        }
        */

        // Redo pathfinding
        FillPathElements(); 
        if( mPathElements.mUseSize <= 0 )
        {
            mCurrPathElement = -1;
            return;
        }
        mCurrPathElement = 0;
    }

    //rDebugPrintf( "*** Starting to fill segments: mNumSegments = %d ***\n", mNumSegments );

    ///////////////////////////////////////////////////
    // Grab some target information
    //
    rmt::Vector targetPos;
    RoadManager::PathElement targetElem;
    RoadSegment* targetSeg = NULL;
    float targetSegT = 0.0f;
    float targetRoadT = 0.0f;

    // can return NULL targetElem if the chase target, for example, 
    // gets out of his vehicle...
    GetClosestPathElementToTarget( targetPos, targetElem, targetSeg, targetSegT, targetRoadT );
    if( targetElem.elem == NULL ) 
    {
        return;
    }
    
    unsigned int targetSegIndex = 0;
    if( targetElem.type == RoadManager::ET_NORMALROAD )
    {
        rAssert( targetSeg );
        rAssert( targetSeg->GetRoad() == (Road*) targetElem.elem );
        targetSegIndex = targetSeg->GetSegmentIndex();
    }


    //////////////////////////////////////////////////////////////////
    // Loop over all the path elements, filling up mSegments array
    // 
    while( mCurrPathElement < mPathElements.mUseSize )
    {
        int i = mCurrPathElement;
        if( mNumSegments >= MAX_SEGMENTS )
        {
            break;
        }

        // the ever-useful... last segment (if one exists)
        Segment* last = (mNumSegments == 0)? NULL : &(mSegments[mNumSegments-1]);
        if( last )
        {
            if( TestReachedTarget( last->mStart, last->mEnd ) )
            {
                return; // don't populate beyond the current waypoint
            }
        }
        RoadManager::PathElement* currElem = &(mPathElements[i]);
        RoadManager::PathElement* nextElem = NULL;
        if( (i+1) < mPathElements.mUseSize )
        {
            nextElem = &(mPathElements[i+1]);
        }

        if( currElem->type == RoadManager::ET_INTERSECTION )
        {
            //rDebugPrintf( "*** FILLSEGMENTS: CurrElem is intersection, mCurrPathElement = %d\n", mCurrPathElement );


            Intersection* currInt = (Intersection*) currElem->elem;
            rmt::Vector currIntPos;
            currInt->GetLocation( currIntPos );

            // SPECIAL CASE:
            // if no next element and we are in same intersection as target...
            // then beeline at the target
            if( nextElem == NULL )
            {
                rAssert( targetElem == (*currElem) );

                // TODO:
                // Use currIntPos or myPos here... Hmm.... 
                rmt::Vector startPos = (last)? last->mEnd : myPos;

                rAssert( !targetPos.Equals( startPos, 0.001f ) );

                mSegments[mNumSegments].mStart = startPos;
                mSegments[mNumSegments].mEnd = targetPos;
                mSegments[mNumSegments].mType = 1; // irrelevant... 1 or 2
                mSegments[mNumSegments].mLength = (
                    mSegments[mNumSegments].mStart - 
                    mSegments[mNumSegments].mEnd).Magnitude(); // *** SQUARE ROOT! ***
                mSegments[mNumSegments].mpSegment = mLastRoadSegment;
                mSegments[mNumSegments].SelfVerify();

                mNumSegments++;

                // done.. Don't increment mCurrPathElement
                // we reached target. Yay.
                return;
            }
            // if there is a next element, it has to be a road. 
            // We create tweening segments to that road
            else
            {
                rAssert( targetElem != (*currElem) );
                rAssert( nextElem->type == RoadManager::ET_NORMALROAD );

                ///////////////// SHORTCUT LOGIC ////////////////////////
                // Decide whether or not to take shortcut:
                //  will not take if shortcut road is not an OUT road 
                //  will not take if shortcut's dest int is not in the list from mCurrPathElement+1 forward
                //  will not take if shortcut skill is higher than my skill (adjusted by catch-up logic)
                // If we take the shortcut, we have to do some dissecting such that
                // the PathElements skipped over by the shortcut (which we are thankfully 
                // not yet visiting) are removed from mPathElements array (do removeKeepOrder)
                //
                // NOTE: 
                // We are assuming here that the shortcut is actually SHORTER 
                // than these elements we're replacing... It'd better be! 
                // If we can't assume this then we'll have to add up the 
                // lengths of the elements we're replacing and compare with 
                // length of this shortcut road... *sigh*
                //
                int mySkill = DetermineShortcutSkill();

                int shortcutDestIntElemIndex = -1;
                Road* shortcutRoad = NULL;

                //
                // process ALL shortcuts (which are by definition OUT roads...
                // Note that direction matters... sometimes you can't take 
                // a shortcut in the opposite direction... e.g. a jump)
                //
                for( int k=0; k<currInt->mOutgoingShortcuts.mUseSize; k++ )
                {
                    Road* candidate = currInt->mOutgoingShortcuts[k];
                    rAssert( candidate );
                    rAssert( candidate->GetShortCut() );

                    // NOTE: We are limited here by the assumption that the shortcut 
                    //       is only one road long!

                    // check if the destination intersection for this shortcut
                    // exists in my list of path elements!
                    Intersection* destInt = (Intersection*) candidate->GetDestinationIntersection();

                    for( int m=mCurrPathElement+2; m<mPathElements.mUseSize; m+=2 )
                    {
                        RoadManager::PathElement* intElem = &(mPathElements[m]);
                        rAssert( intElem->type == RoadManager::ET_INTERSECTION );

                        if( (Intersection*)intElem->elem == destInt )
                        {
                            // found the destination intersection! Now see if we are
                            // skilled enough to take this shortcut
                            if( mySkill >= (int)(candidate->GetDifficulty()) )
                            {
                                // ok, we're skilled enough... but does this shortcut
                                // take us further than all the other shortcuts in
                                // this intersection?
                                if( m > shortcutDestIntElemIndex )
                                {
                                    shortcutDestIntElemIndex = m;
                                    shortcutRoad = candidate;
                                }
                            }
                            break; // break here cuz we found the intersection
                        }
                    }
                }

                // if we didn't find anything, that's ok. But if we did, it'd
                // better be within expected bounds!
                rAssert( shortcutDestIntElemIndex == -1 || 
                    (mCurrPathElement+2 <= shortcutDestIntElemIndex &&
                      shortcutDestIntElemIndex < mPathElements.mUseSize) );

                // ok, found a shortcut that we want to take...
                if( shortcutRoad )
                {
                    rAssert( mCurrPathElement+2 <= shortcutDestIntElemIndex &&
                             shortcutDestIntElemIndex < mPathElements.mUseSize );

                    // Do some cut&paste:
                    // replace elems between mCurrElements and shortcutDestIntElemIndex
                    // (exclusive) with the shortcut road element...

                    mPathElements[mCurrPathElement+1].type = RoadManager::ET_NORMALROAD;
                    mPathElements[mCurrPathElement+1].elem = shortcutRoad;

                    // If necessary, shift elements to get rid of stuff in between:
                    // ...,curr+1,curr+2,curr+3,...,m,m+1,... ==> ...,curr+1,m,m+1,...
                    int numShifts = shortcutDestIntElemIndex - (mCurrPathElement+2);
                    if( numShifts > 0 )
                    {
                        int newUseSize = mPathElements.mUseSize - numShifts;
                        for( int n=mCurrPathElement+2; n<newUseSize; n++ )
                        {
                            mPathElements[n] = mPathElements[n+numShifts];
                        }
                        mPathElements.mUseSize = newUseSize;
                    }

                    // make sure connectivity is correct...
                    rAssert( (Intersection*)shortcutRoad->GetSourceIntersection() == currInt );
                    rAssert( (Intersection*)shortcutRoad->GetDestinationIntersection() == 
                        (Intersection*)(mPathElements[mCurrPathElement+2].elem) );
                }
                /////////////////////////////////////////



                //////////////////////////////////////////////////////
                // Determine an appropriate OUT road segment, OUT road, and join-point 
                // (for tweening). Recall that we can traverse a road in either 
                // directions.. so make sure we get the correct segments and join-points

                RoadSegment* outSeg = NULL;
                rmt::Vector outPosStart, outPosEnd; 

                bool forward = true;
                Road* outRoad = (Road*) nextElem->elem;
                if( currInt == (Intersection*) outRoad->GetSourceIntersection() )
                {
                    outSeg = outRoad->GetRoadSegment( 0 );
                }
                else
                {
                    forward = false;
                    outSeg = outRoad->GetRoadSegment( outRoad->GetNumRoadSegments()-1 );
                }
                rAssert( outSeg );

                rmt::Vector vec0,vec1,vec2,vec3;
                rmt::Vector outSegStart, outSegEnd;

                outSeg->GetCorner( 0, vec0 );
                outSeg->GetCorner( 1, vec1 );
                outSeg->GetCorner( 2, vec2 );
                outSeg->GetCorner( 3, vec3 );
                outSegStart = (vec0 + vec3) * 0.5f;
                outSegEnd = (vec1 + vec2) * 0.5f;

                if( forward )
                {
                    outPosStart = outSegStart;
                    outPosEnd = outSegEnd;
                }
                else
                {
                    outPosStart = outSegEnd;
                    outPosEnd = outSegStart;
                }

                ////////////////////////////////////////////////
                // Build tween segments to that OUT road

                float distToOutPos = 0.0f;
                bool needFirstTween = true;
                bool needLastTween = true;
                if( last )
                {
                    distToOutPos = (last->mEnd - outPosStart).Magnitude();
                    if( distToOutPos <= 0.001f )
                    {
                        // so close they're actually the same point!
                        // no tweening necessary since last->mEnd is already at outPosStart...
                        needFirstTween = false;
                        needLastTween = false;
                    }
                    if( distToOutPos <= 1.0f )
                    {
                        // we're so close... don't need the first tween, but need last tween
                        needFirstTween = false;
                    }
                }
                else // no last segment available --> no first tween necessary, last tween ok
                {
                    needFirstTween = false;
                }

                // TODO:
                // Use currIntPos or myPos here? Hmmm...
                rmt::Vector intermediatePos = (last)? last->mEnd : currIntPos;

                if( needFirstTween )
                {
                    // we never do first tween without second tween
                    rAssert( needLastTween );

                    // make sure we can support adding both tweens at once
                    // plus the actual out segment (later)... 
                    if( mNumSegments >= (MAX_SEGMENTS-2) )
                    {
                        // by returning, we keep mCurrPathElement on the same
                        // element. Hopefully we'll have more room next time.
                        return;
                    }
                    rAssert( distToOutPos > 1.0f );
                    float tweenDist = distToOutPos * 0.4f;

                    rmt::Vector lastSegDir = (last->mEnd - last->mStart) / last->mLength;
                    intermediatePos = last->mEnd + lastSegDir * tweenDist;

                    mSegments[mNumSegments].mStart = last->mEnd;
                    mSegments[mNumSegments].mEnd = intermediatePos;
                    mSegments[mNumSegments].mType = 1; 
                    mSegments[mNumSegments].mLength = (
                        mSegments[mNumSegments].mStart - 
                        mSegments[mNumSegments].mEnd).Magnitude(); // *** SQUARE ROOT! ***
                    mSegments[mNumSegments].mpSegment = outSeg;
                    mSegments[mNumSegments].SelfVerify();

                    mNumSegments++;

                    rAssert( 1 <= mNumSegments && mNumSegments < MAX_SEGMENTS );
                    last = &(mSegments[mNumSegments]);
                }
                
                if( needLastTween )
                {
                    // make sure we can support adding at least one of the tweens
                    // plus the actual out segment...
                    if( mNumSegments >= (MAX_SEGMENTS-1) )
                    {
                        // by returning, we keep mCurrPathElement on the same
                        // element. Hopefully we'll have more room next time.
                        return;
                    }

                    // Second tween: join intermediate pos with out pos. 
                    // The intermediate pos at this point can be one of:
                    //  - intersection center ... if there was no last segment to continue from
                    //  - last segment's end pos ... if we skipped doing the first tween
                    //  - intermediate pos ... if we just did a first tween 
                    //
                    mSegments[mNumSegments].mStart = intermediatePos;
                    mSegments[mNumSegments].mEnd = outPosStart;
                    mSegments[mNumSegments].mType = 2;
                    mSegments[mNumSegments].mLength = (
                        mSegments[mNumSegments].mStart - 
                        mSegments[mNumSegments].mEnd).Magnitude(); // *** SQUARE ROOT! ***
                    mSegments[mNumSegments].mpSegment = outSeg;
                    mSegments[mNumSegments].SelfVerify();

                    mNumSegments++;
                }

                // Add the actual segment
                mSegments[mNumSegments].mStart = outPosStart;
                mSegments[mNumSegments].mEnd = outPosEnd;
                mSegments[mNumSegments].mType = 0;
                mSegments[mNumSegments].mLength = outSeg->GetSegmentLength();
                mSegments[mNumSegments].mpSegment = outSeg;
                mSegments[mNumSegments].SelfVerify();

                mNumSegments++;

                // OK done with currElem... go on to next one...
                mCurrPathElement++;
            }
        }
        else if( currElem->type == RoadManager::ET_NORMALROAD )
        {
            //rDebugPrintf( "*** FILLSEGMENTS: CurrElem is road, mCurrPathElement = %d\n", mCurrPathElement );
            Road* currRoad = (Road*) currElem->elem;

            unsigned int numRoadSegs = currRoad->GetNumRoadSegments();
            rAssert( numRoadSegs > 0 );

            /*** ATTEMPT 1***/
            while( mNumSegments < MAX_SEGMENTS )
            {
                int j = mNumSegments; // numsegs should increment as j increments
                last = (j <= 0)? NULL : &(mSegments[j-1]);

                // don't populate beyond the current waypoint
                if( last )
                {
                    if( TestReachedTarget( last->mStart, last->mEnd ) )
                    {
                        return;
                    }
                }
                RoadSegment* seg = NULL;
                float segT = 0.0f;
                unsigned int segIndex = 0;
                rmt::Vector segStart, segEnd;
                
                // if last doesn't exist then we're populating this 
                // for the first time... presumably our stored last road segment
                // reflects the current road element.. use this as our starting point
                if( last == NULL )
                {
                    rAssert( mLastRoadSegment );
                    // if last road segment isn't even on the current road, just
                    // use the current road rather than the last road segment.
                    if( mLastRoadSegment->GetRoad() != currRoad ) 
                    {
                        rmt::Vector closePos; 
                        float closeDist = NEAR_INFINITY;
                        int ind = -1;

                        RoadManager::FindClosestPointOnRoad( currRoad, myPos, closePos, closeDist, ind );
                        rAssert( ind != -1 );

                        segIndex = (unsigned int)ind;
                        seg = currRoad->GetRoadSegment( segIndex );
                        segT = RoadManager::DetermineSegmentT( myPos, (RoadSegment*)seg );

                    }
                    else
                    {
                        seg = mLastRoadSegment;
                        segT = mLastRoadSegmentT;
                        segIndex = seg->GetSegmentIndex();
                    }

                    rmt::Vector vec0,vec1,vec2,vec3;
                    seg->GetCorner( 0, vec0 );
                    seg->GetCorner( 1, vec1 );
                    seg->GetCorner( 2, vec2 );
                    seg->GetCorner( 3, vec3 );
                    segStart = (vec0+vec3) * 0.5f;
                    segEnd = (vec1+vec2) * 0.5f;

                    // if no next element exists, we're near target
                    bool useSegStart = true;
                    if( nextElem == NULL ) // there is no next element
                    {
                        rAssert( targetElem == (*currElem) );
                        rAssert( targetSeg->GetRoad() == currRoad );

                        // if mPathElements has only one element, then
                        // we are on the same road as our target...
                        // 
                        // if target is further along road.. 
                        if( targetSegIndex > segIndex )
                        {
                            useSegStart = true;
                        }
                        // target lies in the other direction, go against traffic!
                        else if( targetSegIndex < segIndex )
                        {
                            useSegStart = false;
                        }
                        // target is on the same road segment
                        else
                        {
                            if( targetSegT >= segT ) // target ahead of us
                            {
                                useSegStart = true;
                            }
                            else
                            {
                                useSegStart = false;
                            }
                        }
                    }
                    else // next elem is NOT null
                    {
                        rAssert( targetElem != (*currElem) );

                        // we got more than 1 element, we now know the direction 
                        // the first segment should be heading simply by checking 
                        // the next mPathElement (an intersection) and finding out
                        // whether that is the current PathElement (road)'s source
                        // or dest intersection

                        rAssert( nextElem->type == RoadManager::ET_INTERSECTION );
                        Intersection* nextInt = (Intersection*) nextElem->elem;

                        // next intersection is this road's destination intersection,
                        // so the road goes forward
                        if( (Intersection*) currRoad->GetDestinationIntersection() == nextInt )
                        {
                            useSegStart = true;
                        }
                        // otherwise the road goes the other way
                        else
                        {
                            rAssert( (Intersection*) currRoad->GetSourceIntersection() == nextInt );
                            useSegStart = false;
                        }
                    }

                    // time to actually do some segment population
                    if( useSegStart )
                    {
                        mSegments[j].mStart = segStart;
                        mSegments[j].mEnd = segEnd;
                    }
                    else
                    {
                        mSegments[j].mStart = segEnd;
                        mSegments[j].mEnd = segStart;
                    }
                    mSegments[j].mType = 0;
                    mSegments[j].mLength = seg->GetSegmentLength();
                    mSegments[j].mpSegment = seg;
                    mSegments[j].SelfVerify();

                    mNumSegments++;
                }
                else // if last is not NULL, we have to worry about join point
                {
                    RoadSegment* lastSeg = last->mpSegment;
                    rAssert( lastSeg );

                    // TODO:
                    // if it's not even on our road, should we just join it with
                    // the first or the last road segment on this road?
                    // or does this situation never arise? Assert for now.
                    rAssert( lastSeg->GetRoad() == currRoad );

                    unsigned int lastSegIndex = lastSeg->GetSegmentIndex();

                    bool useSegStart = false;

                    if( nextElem == NULL )
                    {
                        // we are on the same road as our target...
                        rAssert( targetElem == (*currElem) );
                        rAssert( targetSeg->GetRoad() == currRoad );

                        // if target is further along road, we want to begin this
                        // segment at segStart and end it at segEnd
                        if( targetSegIndex > lastSegIndex )
                        {
                            useSegStart = true;

                            // if target can have index > our index, next segment exists!
                            rAssert( lastSegIndex < (numRoadSegs-1) ); 

                            segIndex = lastSegIndex+1;
                            seg = currRoad->GetRoadSegment( segIndex );
                        }
                        // target lies in the other direction, use seg's end pos... 
                        else if( targetSegIndex < lastSegIndex )
                        {
                            useSegStart = false;

                            // if target can have index < our index, prev segment exists!
                            rAssert( lastSegIndex > 0 );

                            segIndex = lastSegIndex-1;
                            seg = currRoad->GetRoadSegment( segIndex );
                        }
                        else // target is on the same road segment
                        {
                            segIndex = lastSegIndex;
                            seg = lastSeg;

                            // We should really stop filling segments after this one...
                            // We hope that the segment we're about to make takes
                            // us close enough to the target to yield a TRUE value
                            // when TestReachedTarget is called
                            if( targetSegT >= segT ) // target ahead of us
                            {
                                useSegStart = true;
                            }
                            else
                            {
                                useSegStart = false;
                            }
                        }
                    }
                    else // if nextElem is not NULL
                    {
                        rAssert( targetElem != (*currElem) );
                        rAssert( nextElem->type == RoadManager::ET_INTERSECTION );

                        Intersection* nextInt = (Intersection*) nextElem->elem;

                        // if we want to traverse the road in the forward direction...
                        if( nextInt == (Intersection*) currRoad->GetDestinationIntersection() )
                        {
                            useSegStart = true;

                            if( lastSegIndex < (numRoadSegs-1) )
                            {
                                segIndex = lastSegIndex+1;
                                seg = currRoad->GetRoadSegment( segIndex );
                            }
                            else
                            {
                                //seg = lastSeg;
                                // break out of the current WHILE loop over mSegments
                                // we've reached one end of the road and are thus
                                // done with the current element (ready for next one)
                                mCurrPathElement++;
                                break;
                            }
                        }
                        else // traverse in the "backward" direction
                        {
                            rAssert( nextInt == (Intersection*) currRoad->GetSourceIntersection() );
                            useSegStart = false;
                            if( lastSegIndex > 0 )
                            {
                                segIndex = lastSegIndex-1;
                                seg = currRoad->GetRoadSegment( segIndex );
                            }
                            else
                            {
                                //seg = lastSeg;
                                // break out of the current WHILE loop over mSegments
                                // we've reached one end of the road and are thus
                                // done with the current element (ready for next one)
                                mCurrPathElement++;
                                break;
                            }
                        }
                    }

                    rAssert( seg );
                    rmt::Vector vec0,vec1,vec2,vec3;
                    seg->GetCorner( 0, vec0 );
                    seg->GetCorner( 1, vec1 );
                    seg->GetCorner( 2, vec2 );
                    seg->GetCorner( 3, vec3 );
                    segStart = (vec0+vec3) * 0.5f;
                    segEnd = (vec1+vec2) * 0.5f;

                    // figure out which endpoint of the segment this is...
                    // 0 = at segStart, 1 = at segEnd, 2 = somewhere else
                    // (the last scenario can occur if we do Optimization 1
                    // in UpdateSegments)
                    int joinPointLocation = -1; 

                    if( last->mEnd.Equals( segEnd, 0.001f ) )
                    {
                        joinPointLocation = 1;
                    }
                    else if( last->mEnd.Equals( segStart, 0.001f ) )
                    {
                        joinPointLocation = 0;
                    }
                    else 
                    {
                        // if this is the case... then we shall (later)
                        // need to build a tween segment from joinpoint to
                        // one of the segment's endpoints (segStart or segEnd)
                        // whichever one we want to use as the starting point
                        // for the next segment
                        joinPointLocation = 2;
                    }

                    // now actually do the segment building
                    if( useSegStart ) // want to build segment from segStart to segEnd
                    {
                        if( joinPointLocation == 0 ) // joinpoint at segStart
                        {
                            mSegments[j].mStart = segStart;
                            mSegments[j].mEnd = segEnd;
                            mSegments[j].mType = 0;
                        }
                        else if( joinPointLocation == 1 ) // already at segEnd
                        {
                            // skip adding this particular segment
                            // try adding the next one... (we have to add 
                            // SOMETHING else we'd be stuck here...)
                            if( segIndex < (numRoadSegs-1) )
                            {
                                segIndex++;
                                seg = currRoad->GetRoadSegment( segIndex );
                                seg->GetCorner( 0, vec0 );
                                seg->GetCorner( 1, vec1 );
                                seg->GetCorner( 2, vec2 );
                                seg->GetCorner( 3, vec3 );
                                segStart = (vec0+vec3) * 0.5f;
                                segStart = (vec1+vec2) * 0.5f;
                                mSegments[j].mStart = segStart;
                                mSegments[j].mEnd = segEnd;
                                mSegments[j].mType = 0;
                            }
                            else
                            {
                                // break out of this inner FOR loop because we're done
                                // with the current path element (reached end of the road)
                                mCurrPathElement++;
                                break; 
                            }
                        }
                        else // joinpoint is somewhere else entirely... 
                        {
                            // add a tween segment from joinpoint right to segEnd
                            mSegments[j].mStart = last->mEnd;
                            mSegments[j].mEnd = segEnd;
                            mSegments[j].mType = 2;
                        }
                    }
                    else // want to build a segment from segEnd to segStart
                    {
                        if( joinPointLocation == 0 ) // already at segStart
                        {
                            // skip adding this particular segment
                            // try adding the next one... (we have to add 
                            // SOMETHING else we'd be stuck here...)
                            if( segIndex > 0 )
                            {
                                segIndex--;
                                seg = currRoad->GetRoadSegment( segIndex );
                                seg->GetCorner( 0, vec0 );
                                seg->GetCorner( 1, vec1 );
                                seg->GetCorner( 2, vec2 );
                                seg->GetCorner( 3, vec3 );
                                segStart = (vec0+vec3) * 0.5f;
                                segStart = (vec1+vec2) * 0.5f;
                                mSegments[j].mStart = segEnd;
                                mSegments[j].mEnd = segStart;
                                mSegments[j].mType = 0;
                            }
                            else
                            {
                                // break out of this inner FOR loop because we're done
                                // with the current path element (reached end of the road)
                                mCurrPathElement++;
                                break; 
                            }
                        }
                        else if( joinPointLocation == 1 ) // at segEnd
                        {
                            mSegments[j].mStart = segEnd;
                            mSegments[j].mEnd = segStart;
                            mSegments[j].mType = 0;
                        }
                        else // joinpoint is somewhere else entirely... 
                        {
                            // add a tween segment from joinpoint right to segStart
                            mSegments[j].mStart = last->mEnd;
                            mSegments[j].mEnd = segStart;
                            mSegments[j].mType = 2;
                        }
                    }
                    mSegments[j].mpSegment = seg;
                    mSegments[j].mLength = (mSegments[j].mType == 0)?
                        seg->GetSegmentLength() :
                        (mSegments[j].mStart - mSegments[j].mEnd).Magnitude(); // *** SQUARE ROOT! ***
                    mSegments[j].SelfVerify();

                    mNumSegments++;

                }// end of else (last is not NULL)
            }// end of WHILE loop over mSegments
        } // end of if currElem is type ET_NORMALROAD
        else
        {
            rAssertMsg( false, "Uh-oh... I never anticipated this type... something new?\n" );
        }
    } // end of WHILE loop over mPathElements

}

bool VehicleAI::MustRepopulateSegments()
{
    return false;
}

void VehicleAI::GetPosAheadAlongRoad( float t, float lookAheadDist, int i, rmt::Vector* lookAheadPos )
{
    rAssert( 0 <= i && i < mNumSegments );

    //rDebugPrintf( "Looking ahead %f meters\n", lookAheadDist );

    float origT = t;
    int origI = i;

    rmt::Vector segDir = mSegments[i].mEnd - mSegments[i].mStart;
    rmt::Vector start = mSegments[i].mStart + segDir * t;

    // augment t with our new distance and figure out which 
    // segment this falls on and what the exact position is...
    t += lookAheadDist / mSegments[i].mLength;
    while( t > 1.0f && i < (mNumSegments-1) )
    {
        i++;
        rAssert( 1 <= i && i < mNumSegments );

        t -= 1.0f;
        t *= mSegments[i-1].mLength / mSegments[i].mLength;
        rAssert( !rmt::IsNan(t) );
    }

    // last segment, we don't want to go behind this.
    t = rmt::Clamp( t, 0.0f, 1.0f );

    // now we have the segment we want... find the position along this segment
    segDir = mSegments[i].mEnd - mSegments[i].mStart;
    (*lookAheadPos) = mSegments[i].mStart + segDir * t;

    rAssert( !rmt::IsNan((*lookAheadPos).x) && !rmt::IsNan( (*lookAheadPos).z) );

}

void VehicleAI::UpdateSegments()
{
    // THE APPROACH
    // ============
    // - get my position "p"
    // - find out which Segment "s" in mSegments I'm closest to
    // - TODO: if my distance to "s" is greater than some tolerance, then we're too far off course... 
    //           looks like we need to do some sort of pathfinding back. We should be able to insert 
    //           new segments into mSegments
    // - shift mSegments till "s" is at mSegments[0], adding new segments to the end as necessary...
    //   (when adding a new segment, look at the last segment "l"...
    //      if "l" is a normal roadsegment that's not the last segment of that road 
    //          add the next roadsegment 
    //      else if "l" is the last segment of that road, 
    //          add the first tweening segment from the IN segment's trailing edge to the intersection center
    //          Decide which OUT road to take and store its first segment in pSegment field
    //      else if "l" is the first tweening intersection segment,
    //          add the second tweening segment from intersection center to leading edge of the OUT segment 
    //            that was stored in "l"'s pSegment field, store the OUT segment in pSegment field
    //      else if "l" is the second tweening intersection segment,
    //          add the OUT road segment stored in "l"'s pSegment field.)
    //          
    // - find out my projected "t" along this segment "s" based on my position "p"...
    // - calc my lookahead distance based on my current speed and lookahead time (minimum = 5.0f meters)
    // - t += lookaheaddist / segmentlen
    // - while( t > 1.0f && while we're not out of segments in mSegments ) { 
    //      t -= 1.0f
    //      go to next segment in mSegments 
    //   }
    // - Finally, we have t < 1.0f... get the position "q" at t along this segment...
    // - we use "q" as a guidance for steering...

    rmt::Vector myPos;
    GetVehicle()->GetPosition( &myPos );

    //
    // If waypoint's nearest segment has changed (won't for waypointAI, will for chaseAI)
    // and the new segment doesn't already exist in our list, repopulate mSegments 
    // by setting numsegments to 0
    //
    bool mustRepopulate = MustRepopulateSegments();
    if( mustRepopulate )
    {
        ResetSegments();
        mSecondsLeftToGetBackOnPath = SECONDS_REPOPULATE_DELAY;
    }

    // Attempt to fill up mSegments again
    FillSegments();
    // We try to shift the segments and optimize them here
    //
    // NOTE: Since we never populate segments beyond the current target
    //       waypoint, we don't need to worry about shifting too far (past
    //       the current waypoint)
    
    //
    // Find out which Segment in mSegments I'm closest to & shift this to 
    // the front of the segment queue
    //
    int segmentIndex;
    float distToSegmentSqr;
    rmt::Vector closestPt;
    FindClosestSegment( myPos, segmentIndex, distToSegmentSqr, closestPt );
    if( 0 < segmentIndex && segmentIndex < mNumSegments )
    {
        // If we found a segment, make this the first in our 
        // mSegments array (shift off the other segments)
        ShiftSegments( segmentIndex );
        mSecondsLeftToGetBackOnPath = SECONDS_REPOPULATE_DELAY;
    }
    else if( segmentIndex == -1 || segmentIndex >= mNumSegments )
    {
        // If we never found a close segment, repopulate our array?
        // Only if we have been this way for some time...
        if( mSecondsLeftToGetBackOnPath < 0.0f )
        {
            ResetSegments(); // repopulate now...
            mSecondsLeftToGetBackOnPath = SECONDS_REPOPULATE_DELAY;
        }
    }
    else // segmentIndex == 0
    {
        mSecondsLeftToGetBackOnPath = SECONDS_REPOPULATE_DELAY;
    }

    //
    // Optimize the remaining segments...
    //

    if( mEnableSegmentOptimization )
    {
        // Optimization 1
        // ==============
        // Eliminate unnecessary segments:
        // Segments that lie at least 3 segments away from our closest segment
        // (segment zero... because we did a Shift earlier) should be distance
        // tested ... if close enough, we tween so we eliminate the segments 
        // in between us. Has to be at least 3 because we don't want to tween 
        // through the two tweening segments in an intersection!
        // 
        if( mNumSegments > 3 && (0 <= segmentIndex && segmentIndex < mNumSegments) )
        {
            rAssert( mSegments[0].mpSegment );
            float segmentWidth = mSegments[0].mpSegment->GetRoadWidth();
            float closeEnoughRadius = segmentWidth + 1.0f;

            float SEGMENT_BEELINE_RADIUS_SQR = closeEnoughRadius * closeEnoughRadius;
            for( int i=mNumSegments-1; i>2; i-- )
            {
                rAssert( 1 < i && i < mNumSegments );

                // skip if this is a tweening segment (we shouldn't eliminate these since 
                // they guide us through the intersection)
                if( mSegments[i].mType == 1 || mSegments[i].mType == 2 )
                {
                    continue;
                }

                rmt::Vector closestPtToMySeg;
                FindClosestPointOnLine( mSegments[i].mStart, mSegments[i].mEnd, 
                    closestPt, closestPtToMySeg );
                float distSqr = (closestPt - closestPtToMySeg).MagnitudeSqr();

                // If this segment is close enough to our current segment,
                // build a tween segment directly to it!
                if( distSqr < SEGMENT_BEELINE_RADIUS_SQR )
                {
                    // what if these two points are identical?
                    if( closestPt.Equals( closestPtToMySeg, 0.001f ) )
                    {
                        // Just skip the thing...
                        continue;
                    }
                    else
                    {
                        // Shift so that 0,1,2,..,i,i+1 becomes 0,1,i,i+1,...
                        // if( closestPt at t=0 and closestPtToMySeg at t=1 )
                        //   1 segment
                        // else if( closestPt at t=0 and closestPtToMySeg not at t=1 )
                        //   2 segments
                        // else if( closestPt not at t=0 and closestPtToMySeg at t=1 )
                        //   2 segments
                        // else 
                        //   3 segments
                        bool closestPtAt0 = false;
                        bool closestPtToMySegAt1 = false;
                        if( closestPt.Equals( mSegments[0].mStart, 0.001f ) )
                        {
                            closestPtAt0 = true;
                        }
                        if( closestPtToMySeg.Equals( mSegments[i].mEnd, 0.001f ) )
                        {
                            closestPtToMySegAt1 = true;
                        }

                        if( closestPtAt0 && closestPtToMySegAt1 )
                        {
                            // modify i to start where seg0 starts (change) and 
                            // end where segi already ends (no change)
                            mSegments[i].mStart = mSegments[0].mStart;
                            mSegments[i].mLength = (mSegments[i].mEnd - mSegments[i].mStart).Magnitude(); 
                            mSegments[i].SelfVerify();

                            // shift so 0,1,2,..,i,i+1 ===> i,i+1,i+2
                            ShiftSegments( i, 0 );
                        }
                        else if( closestPtAt0 && !closestPtToMySegAt1 )
                        {
                            // we'll have 2 segments
                            // modify seg0 to end at closestPtToMySeg, segi to start at closestPtToMySeg
                            mSegments[0].mEnd = closestPtToMySeg;
                            mSegments[0].mLength = (mSegments[0].mEnd - mSegments[0].mStart).Magnitude(); 
                            mSegments[0].SelfVerify();

                            mSegments[i].mStart = closestPtToMySeg;
                            mSegments[i].mLength = (mSegments[i].mEnd - mSegments[i].mStart).Magnitude(); 
                            mSegments[i].SelfVerify();

                            // shift so 0,1,2,..,i,i+1 ===> 0,i,i+1
                            ShiftSegments( i-1, 1 );
                        }
                        else if( !closestPtAt0 && closestPtToMySegAt1 )
                        {
                            // we'll have 2 segments
                            // modify seg0 to end at closestPt, segi to start at closestPt
                            mSegments[0].mEnd = closestPt;
                            mSegments[0].mLength = (mSegments[0].mEnd - mSegments[0].mStart).Magnitude(); 
                            mSegments[0].SelfVerify();

                            mSegments[i].mStart = closestPt;
                            mSegments[i].mLength = (mSegments[i].mEnd - mSegments[i].mStart).Magnitude(); 
                            mSegments[i].SelfVerify();

                            // shift so 0,1,2,..,i,i+1 ===> 0,i,i+1
                            ShiftSegments( i-1, 1 );
                        }
                        else
                        {
                            // we'll have 3 segments
                            // seg0 ends at closestPt, tween between closestPt and closestPtToMySeg, 
                            // segi starts at closestPtToMySeg
                            mSegments[0].mEnd = closestPt;
                            mSegments[0].mLength = (mSegments[0].mEnd - mSegments[0].mStart).Magnitude(); 
                            mSegments[0].SelfVerify();
                            
                            // modify seg 1 to be our tween
                            mSegments[1].mType = 1;
                            mSegments[1].mpSegment = mSegments[i].mpSegment;
                            mSegments[1].mStart = closestPt;
                            mSegments[1].mEnd = closestPtToMySeg;
                            mSegments[1].mLength = (mSegments[1].mEnd - mSegments[1].mStart).Magnitude(); 
                            mSegments[1].SelfVerify();

                            mSegments[i].mStart = closestPtToMySeg;
                            mSegments[i].mLength = (mSegments[i].mEnd - mSegments[i].mStart).Magnitude(); 
                            mSegments[i].SelfVerify();

                            // shift so 0,1,2,..,i,i+1 ===> 0,1,i,i+1
                            ShiftSegments( i-2, 2 );
                        }
                    }
                    break;
                }
            }
        }
        /*
        // Optimization 2
        // ==============
        // Do the 12B12 ==> 12 optimization (when B is a segment no 
        // longer than.. say.. 15 meters)
        static const float MAX_LENGTH_OF_MIDDLE_SEGMENT = 20.0f;
        if( mNumSegments >= 5 )
        {
            // Loop through segments detecting a 1-2-0-1-2 segment type pattern
            // keeping track by seenUpTo counter 1 2 3 4 5 according to above
            int seenUpTo = 0; // have seen nothing...
            for( int i=0; i<mNumSegments; i++ )
            {
                int type = mSegments[i].mType;
                switch( seenUpTo )
                {
                case 0: // seen nothing
                    if( type == 1 )
                        seenUpTo = 1;
                    break;
                case 1: // seen 1
                    if( type == 2 )
                        seenUpTo = 2;
                    else if( type == 1 )
                        seenUpTo = 1;
                    else
                        seenUpTo = 0;
                    break;
                case 2: // seen 1-2
                    if( type == 0 )
                    {
                        // test if length of this segment is short enough 
                        if( mSegments[i].mLength <= MAX_LENGTH_OF_MIDDLE_SEGMENT )
                            seenUpTo = 3;
                        else
                            seenUpTo = 0;
                    }
                    else if( type == 1 )
                        seenUpTo = 1;
                    else
                        seenUpTo = 0;
                    break;
                case 3: // seen 1-2-0
                    if( type == 1 )
                        seenUpTo = 4;
                    else 
                        seenUpTo = 0;
                    break;
                case 4: // seen 1-2-0-1
                    if( type == 2 )
                        seenUpTo = 5;
                    else if( type == 1 )
                        seenUpTo = 1;
                    else 
                        seenUpTo = 0;
                    break;
                }

                // if we got to the end, it means we saw the complete string
                // and the middle segment is short enough to bypass
                if( seenUpTo == 5 )
                {
                    // now we convert segments 
                    // i-4,i-3,i-2,i-1,i  ===> i-4,i-3
                    // The two new segments shall go from the start of i-4 to 
                    // the midpoint of i-2, and then from there to the end of
                    // segment i.

                    // build the new segments
                    int tween1 = i-4;
                    int tween2 = i-3;
                    int mid = i-2;
                    rAssert( 0 <= tween1 && tween1 < mNumSegments );
                    rAssert( 0 <= tween2 && tween2 < mNumSegments );
                    rAssert( 0 <= mid && mid < mNumSegments );

                    RoadSegment* seg = mSegments[i].mpSegment;
                    rmt::Vector midPt = (mSegments[mid].mStart + mSegments[mid].mEnd) / 2.0f;

                    mSegments[tween1].mEnd = midPt;
                    mSegments[tween1].mLength = (mSegments[tween1].mStart - mSegments[tween1].mEnd).Magnitude(); // *** SQUARE ROOT! ***
                    mSegments[tween1].mpSegment = seg;
                    mSegments[tween1].mType = 1;
                    mSegments[tween1].SelfVerify();

                    mSegments[tween2].mStart = midPt;
                    mSegments[tween2].mEnd = mSegments[i].mEnd;
                    mSegments[tween2].mLength = (mSegments[tween2].mStart - mSegments[tween2].mEnd).Magnitude(); // *** SQUARE ROOT! ***
                    mSegments[tween2].mpSegment = seg;
                    mSegments[tween2].mType = 2;
                    mSegments[tween2].SelfVerify();
                    
                    // As a result, we reduce numsegments by 3 and should end up with at least 2 segments
                    mNumSegments -= 3;
                    rAssert( 2 <= mNumSegments && mNumSegments < VehicleAI::MAX_SEGMENTS );

                }
            }// end of for-loop
        }// end of if numsegments >= 5 (and end of Optimization 2)
        */
    }
}


//=============================================================================
// VehicleAI::EvadeTraffic
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleAI::EvadeTraffic( Vehicle* exceptThisOne )
{
    const float STEER_AROUND = 3.0f; //  distance to the side of something to go around
    const float MAX_DIST = 50.0f; // dont worry about anything farther than this
    const float IGNORE_COS = -0.707f; // don't worry about stuff in this angle behind you

    mEvading = false;

    if(!EVADE_VEHICLES)
    {
        return;
    }

    if( mState == STATE_REVERSE )
    {
        return;
    }
    
    rmt::Vector mypos;
    GetVehicle()->GetPosition( &mypos );

    float closestdistsqr = MAX_DIST * MAX_DIST;
    Vehicle* closestVehicle = NULL;
    rmt::Vector closestpos;

    rmt::Vector heading;
    GetVehicle()->GetHeading( &heading );

    for( int i = 0; i < GetVehicleCentral()->GetNumVehicles(); i++ )
    {
        Vehicle* vehicle = GetVehicleCentral()->GetVehicle( i );
        if( vehicle && (vehicle != GetVehicle()) && (vehicle != exceptThisOne) )
        {
            rmt::Vector pos;
            vehicle->GetPosition( &pos );

            rmt::Vector vec2car;
            vec2car.Sub( pos, mypos );

            float dp = vec2car.DotProduct( heading );

            if( dp > IGNORE_COS )
            {
                float distsqr = vec2car.MagnitudeSqr();

                if( distsqr < closestdistsqr )
                {
                    closestdistsqr = distsqr;
                    closestVehicle = vehicle;
                    closestpos = pos;
                }
            }

        }
    }

    if( closestVehicle != NULL )
    {
        mEvading = true;

        // Project pos into local happy car space
        rmt::Vector vUp;
        GetVehicle()->GetVUP( &vUp );

        rmt::Vector side;
        side.CrossProduct( heading, vUp );

        rmt::Vector relpos = closestpos;
        relpos.Sub( mypos );

        rmt::Vector vec2relpos;

        vec2relpos.x = side.DotProduct( relpos );
        vec2relpos.y = 0.0f;
        vec2relpos.z = heading.DotProduct( relpos );


        // if it's in front, steer around
        if( vec2relpos.z > 0.0f )
        {
            rmt::Vector normvec = relpos;
            normvec.Normalize();
            float dx = heading.DotProduct( normvec ) * STEER_AROUND * rmt::Sign(vec2relpos.x);

            side.Scale( dx, dx, dx );

            rmt::Vector newdest = mDestination;
            newdest.Sub( side );

            SetDestination( newdest );
        }
    }
}

/*
void VehicleAI::EvadeTraffic( Vehicle* exceptThisOne )
{
    if( mState == STATE_REVERSE || mState == STATE_WAITING || mState == STATE_STUNNED )
    {
        return;
    }

    rmt::Vector mypos;
    GetVehicle()->GetPosition( &mypos );

    rmt::Vector myheading;
    GetVehicle()->GetHeading( &myheading );
    rAssert( rmt::Epsilon( myheading.MagnitudeSqr(), 1.0f, 0.00001f ) );

    bool bCheck = false;


    // Initialize potential field

    mPotentialField.Initialize( 40.0f, 20.0f, mypos, myheading );
    mPotentialField.Clear( -1.0f );

    float closestcar = 10000.0f;

    float radius, value, falloff;

    float lookAhead = (LOOK_AHEAD_CLOSE_SECONDS * GetVehicle()->mSpeed);

    // Populate field with statics
    if( mEvadeStatics )
    {

        int collindex = GetVehicle()->mCollisionAreaIndex;

        sim::TList<sim::CollisionObject*>* list = GetWorldPhysicsManager()->
            mCollisionManager->GetCollisionObjectList( collindex );

        for( int i = 0; i < list->GetSize(); i++ )
        {
            sim::CollisionObject* obj = list->GetAt( i );
            if( !obj->IsStatic() )
            {
                continue;
            }

            sim::CollisionVolume* volume = obj->GetCollisionVolume();
            rAssert( volume != NULL );

            rmt::Vector pos;
            pos = volume->mPosition;

            rmt::Vector vec2obj;
            vec2obj.Sub( pos, mypos );

            float distsqr = vec2obj.MagnitudeSqr();

            if( distsqr > 10.0f && distsqr < (lookAhead * lookAhead))
            {
                vec2obj.Normalize();

                float dp = vec2obj.DotProduct( myheading );

                // if the object lies within 120 frustrum (60 on either side of heading),
                // then we want to add it to the potential field
                if( dp > 0.5f ) //cos60=0.5
                {
                    radius = 0.0;
                    value = -1.0f;
                    falloff = 1.0f; // the bigger the value, the less influence this potential exerts over distance

                    mPotentialField.AddPotential( pos, value, radius, falloff );
                    bCheck = true;
                }
            }
        }
    }
    
    // Populate field with vehicles
    if( mEvadeVehicles )
    {

        // DUSIT: Must loop through, max, not num vehicles, because active list is sparse
        //for( int i = 0; i < GetVehicleCentral()->GetNumVehicles(); i++ )
        for( int i = 0; i < GetVehicleCentral()->GetMaxActiveVehicles(); i++ )
        {
            Vehicle* vehicle = GetVehicleCentral()->GetVehicle( i );
            if( vehicle != NULL && vehicle != GetVehicle() && vehicle != exceptThisOne )
            {
                rmt::Vector pos;
                vehicle->GetPosition( &pos );

                rmt::Vector vec2car;
                vec2car.Sub( pos, mypos );

                if( vec2car.MagnitudeSqr() < (lookAhead * lookAhead))
                {
                    vec2car.Normalize();

                    float dp = vec2car.DotProduct( myheading );

                    if( dp > 0.5f )
                    {
                        for( int wheel = 0; wheel < 4; wheel++ )
                        {
                            if( mPotentialField.AddPotential( vehicle->mSuspensionWorldSpacePoints[ wheel ], -1.0f, 0.0f ))
                            {
                                bCheck = true;
                            }
                        }
                        if( mPotentialField.AddPotential( pos, -1.0f, 0.0f ))
                        {
                            bCheck = true;
                        }

                        if( vec2car.MagnitudeSqr() < closestcar )
                        {
                            closestcar = vec2car.MagnitudeSqr();
                        }
                    }
                }
            }
        }
    }

    mPotentialField.AddPotential( mDestination, 2.0f, 2.0f ); 

    mSteeringRatio = DEFAULT_STEERING_RATIO;

    if( bCheck )
    {
        float aheaddist;

        // DUSIT:
        // Instead of using a fixed distance, calculate a distance based on
        // the vehicle's present speed & seconds of look-ahead
        //float aheaddist = LOOK_AHEAD_CLOSE;
        //aheaddist = LOOK_AHEAD_CLOSE;
        aheaddist = (GetVehicle()->GetSpeedKmh() * KPH_2_MPS) * LOOK_AHEAD_CLOSE_SECONDS;
        if( aheaddist > lookAhead )
        {
            aheaddist = lookAhead;
        }
        else if( aheaddist < 1.0f )
        {
            aheaddist = 1.0f;
        }


        rmt::Vector pos;
        if( mPotentialField.FindBestPosition( pos, aheaddist ))
        {
            pos.y = 0.0f;
            SetDestination( pos );

            //mState = STATE_CORNER_PREPARE;
            mSteeringRatio = AVOID_STEERING_RATIO;
        }
    }
}
*/

void VehicleAI::DoCatchUp( float timeins )
{
}


void VehicleAI::DoSteering()
{
    float steering = 0.0f;
    float nextsteering = 0.0f;
    float dist = 0.0f;
    float nextdist = 0.0f;

    //
    // Calculates the steering necessary to move towards
    // both the current dest and the next.  Creates
    // smoother driving.
    //
    DriveTowards( &mDestination, dist, steering );
    DriveTowards( &mNextDestination, nextdist, nextsteering );

    float combinedsteering = mSteeringRatio * steering 
        + ( 1.0f - mSteeringRatio ) * nextsteering;

    if((mState == STATE_REVERSE) || (mState == STATE_STOP))
    {
        combinedsteering = rmt::Clamp(combinedsteering * 1.25f, -1.0f, 1.0f);
        mSteering.SetValue( -combinedsteering );
    }
    else
    {
        mSteering.SetValue( combinedsteering );
    }
}



void VehicleAI::CheckState( float timeins )
{
    // Determine if we should transit to being OUT OF CONTROL
    static const float MAX_SECONDS_STUNNED = 0.5f;
    static const float MAX_SECONDS_OUT_OF_CONTROL = 1.5f;

    // steering will be this many times more sensitive when out of control...
    static const float OUT_OF_CONTROL_STEERING_MODIFIER = 1.5f; 
    static const float NORMALIZED_SWERVE_THRESHOLD = 0.020f;

    mSteeringRatio = DEFAULT_STEERING_RATIO;

    if( mState != STATE_WAITING &&  
        GetVehicle()->mWasHitByVehicle && 
        //GetVehicle()->mWasHitByVehicleType == VT_USER && 
        GetVehicle()->mNormalizedMagnitudeOfVehicleHit > NORMALIZED_SWERVE_THRESHOLD )
    {
        GetVehicle()->mOutOfControl = true;
        GetVehicle()->mVehicleState = VS_SLIP;
        mOutOfControlNormal = GetVehicle()->mSwerveNormal;
        mSecondsOutOfControl = 0.0f;
        mState = STATE_OUT_OF_CONTROL;
    }

    // if this reaches zero or lower, we repopulate the paths because
    // we have been too far from prescribed segment too long...
    // UpdateSegment is supposed to check this variable
    mSecondsLeftToGetBackOnPath -= timeins;

    static const float SECONDS_BEFORE_CORNER = 0.0f;//0.25f;
    static const float CORNER_COSALPHA = 0.7071067f;//0.7f; //cos45
    static const float HANDBRAKE_COSALPHA = 0.0f; // cos90

    VehicleAIState newState = mState;

    mBrake.SetValue( 0.0f );
    mGas.SetValue( 0.0f );
    mHandBrake.SetValue( 0.0f );
    mReverse.SetValue( 0.0f );

    //
    // Calc the vector from my position to the destination
    //
    rmt::Vector mypos;
	rmt::Vector myheading;
    GetVehicle()->GetPosition( &mypos );
    GetVehicle()->GetHeading( &myheading );
    myheading.y = 0.0f;
    myheading.NormalizeSafe(); // *** SQUARE ROOT! ***

    rmt::Vector vec2dest;
    vec2dest.Sub( mDestination, mypos );
	vec2dest.y = 0.0f;
    vec2dest.Normalize(); // *** SQUARE ROOT! ***

	float dp2heading = vec2dest.DotProduct( myheading );

    //
    // Calc the vector from the dest to the next dest
    //
    rmt::Vector dest2next;
    dest2next.Sub( mNextDestination, mDestination );
	dest2next.y = 0.0f;
    dest2next.Normalize(); // *** SQUARE ROOT! ***

    //
    // Find the angle between them
    //
    float dp2next = dest2next.DotProduct( myheading );

    /***
    //
    // Accelerate less as we're cornering
    //
    float steer = STEER_HEADING_RATIO * dp2heading 
        + ( 1.0f - STEER_HEADING_RATIO ) * ( 1.0f - rmt::Fabs( mSteering.GetValue() ));
    ***/
    float steer = rmt::Fabs(mSteering.GetValue());

    Vehicle* vehicle = GetVehicle();
    float skidding = vehicle->GetSkidLevel();

    /*
    float givergas;
    if( skidding > 0.75f )
    {
        givergas = 1.0f;//0.0f;
    }
    else
    {
        // givergas = 1.0f - (GAS_STEERING_RATIO * steer);
        givergas = 1.0f;
    }
    */

    const float DESIRED_SPEED_BUFFER = 5.0f;

    float givergas = 1.0f; // TODO: Is this a good value?
    float speedKmh = GetVehicle()->GetSpeedKmh();
    if( speedKmh > (mDesiredSpeedKmh + DESIRED_SPEED_BUFFER) )
    {
        givergas = 0.0f;
    }
    else if( speedKmh < (mDesiredSpeedKmh - DESIRED_SPEED_BUFFER) )
    {
        givergas = 1.0f;
    }


    static const float MIN_SPEED = 1.0f;
    switch( mState )
    {
    case STATE_WAITING:
    case STATE_WAITING_FOR_PLAYER:
        {
            mHandBrake.SetValue( 1.0f );
            break;
        }
    case STATE_ACCEL:
        {
            //rDebugPrintf( "============ ACCELERATING ==============\n" );

            float speed = GetVehicle()->mSpeed;

            if( (speed > STILL_STUCK_SPEED) && (mReverseTime > DEFAULT_REVERSE_TIME))
            {
               mReverseTime = DEFAULT_REVERSE_TIME; 
            }

            if( speed <= MIN_SPEED )
            {
                //
                // We're going way too slow, so we might be stuck... 
                // Start a timer...
                //
                unsigned int currentTime = radTimeGetMilliseconds();
                if( mStartStuckTime == 0 )
                {
                    mStartStuckTime = currentTime;
                }
                else if( (currentTime - mStartStuckTime) > mNextStuckTime )
                {
                    // If chase AI gets stuck, it's screwed anyway, not 
                    // doing back off might help it get free a little 
                    // faster if it isn't very stuck
                    if(mType == AI_CHASE)
                    {
                        mReverseTime = DEFAULT_REVERSE_TIME + REVERSE_BACKOFF;
                    }
                    else
                    {
                        mReverseTime += REVERSE_BACKOFF;
                    }

                    // we've been iteratively reversing too long... 
                    // time to just reset...
                    if( mReverseTime > MAX_REVERSE_TIME )
                    {
                        mStartStuckTime = 0;
                        mNextStuckTime = NEXT_STUCK_TIME;
                        mReverseTime = DEFAULT_REVERSE_TIME;

                        ////////
                        // see if we should reset on the spot... 
                        // don't do this for:
                        // - chase vehicles (they don't do iterative reverse anyway
                        // - destroy objectives 
                        // - dump objectives where you need to bump it 
                        //

                        bool needsReset = true;

                        if( GetType() == AI_CHASE )
                        {
                            needsReset = false;
                        }
                        else
                        {
                            Mission* m = GetGameplayManager()->GetCurrentMission();
                            if( m )
                            {
                                MissionStage* ms = m->GetCurrentStage();
                                if( ms )
                                {
                                    MissionObjective* mobj = ms->GetObjective();
                                    if( mobj )
                                    {
                                        MissionObjective::ObjectiveTypeEnum type = mobj->GetObjectiveType();
                                        if( type == MissionObjective::OBJ_DESTROY )
                                        {
                                            DestroyObjective* dobj = (DestroyObjective*) mobj;
                                            if( dobj->GetTargetVehicle() == GetVehicle() )
                                            {
                                                needsReset = false;
                                            }
                                        }
                                        else if( type == MissionObjective::OBJ_DUMP )
                                        {
                                            CollectDumpedObjective* cdobj = (CollectDumpedObjective*) mobj;
                                            if( cdobj->IsBumperCars() && cdobj->GetDumpVehicle() )
                                            {
                                                needsReset = false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if( needsReset )
                        {
                            GetVehicle()->ResetOnSpot( false );
                        }
                    }
                    else
                    {
                        //
                        // We've been stuck long enough to do something about it
                        //
                        //rDebugPrintf( "ACCEL: Stuck longer than mNextStuckTime = %d. Going to REVERSE\n", mNextStuckTime ); 
                        newState = STATE_REVERSE;
                        mStartStuckTime = currentTime;
                    }
                    break;
                }
            }
            else if( mNextStuckTime == NEXT_STUCK_TIME )
            {
                mNextStuckTime = FIRST_STUCK_TIME;
            }
            else
            {
                // if angle between heading and DestinationToNextdestination 
                // is great enough, we are going to have to take a corner...
                if(( dp2next < CORNER_COSALPHA ) && ( speed > MIN_SPEED * 2.0f ))
                {
                    newState = STATE_CORNER_PREPARE;

                    mSecondsBeforeCorner = 0.0f; // prepare counter
                }
            }

            mGas.SetValue( givergas );

            break;
        }
    case STATE_CORNER_PREPARE:
        {
            float speed = GetVehicle()->mSpeed;

            // steer to far point for smoother conering, unless evading
            mSteeringRatio = 0.0f;

            if ( speed > MIN_SPEED )
            {
                static const float BRAKE_SPEED = 50.0f;//24.0f;// speed above which to apply brake (strongest)
                static const float OFF_GAS_SPEED = 40.0f;//21.5f;// speed above which to let off gas (do nothing)
                static const float POWERSLIDE_HEADING = 0.85f; // cos of angle to try to powerslide at
                static const float POWERSLIDE_MINSPEED = 6.0f; // minimum speed needed to powerslide

                //rDebugPrintf( "\n         =========== CORNERING (%f mps) =============\n", speed );
                
                // we are getting in danger of not making the turn, powerslide
                if((dp2heading < POWERSLIDE_HEADING) && (speed > POWERSLIDE_MINSPEED))
                {
                    mHandBrake.SetValue( 1.0f );
                    mGas.SetValue( 0.0f );
                    break;
                }
                if( dp2next > CORNER_COSALPHA )
                {
                    mGas.SetValue( 1.0f );
                    mSteeringRatio = DEFAULT_STEERING_RATIO;
                    newState = STATE_ACCEL;
                    break;
                }

                if( speed > BRAKE_SPEED )
                {
                    //rDebugPrintf( "         Using normal brake!\n" );
                    mBrake.SetValue(1.0f);
                }
                else if( speed > OFF_GAS_SPEED )
                {
                    //rDebugPrintf( "         Letting off gas\n" );
                    mGas.SetValue(0.0f);
                }
                else 
                {
                    //rDebugPrintf( "         Speed too low, accelerating!!\n" );
                    mGas.SetValue(givergas);
                    // TODO: 
                    // Transit here to Accelerate? Maybe.... or maybe we need
                    // a new criterion for determining when we're out of a cornering
                    // state... hmmm..
                    //newState = STATE_ACCEL;
                }

            }
            else
            {
                // We're probably stuck at this point, so do something
                newState = STATE_ACCEL;
                mSteeringRatio = DEFAULT_STEERING_RATIO;
                mGas.SetValue( 1.0f );
                break;
            }
            break;
        }
    case STATE_REVERSE:
        {
            mReverse.SetValue( 1.0f );

            unsigned int currentTime = radTimeGetMilliseconds();
            if( (currentTime - mStartStuckTime) > mReverseTime )
            {
                //rDebugPrintf( "REVERSE: Been reversing for longer than DEFAULT_REVERSE_TIME, going to STOP\n" );
                newState = STATE_STOP;
                mNextStuckTime = NEXT_STUCK_TIME;
                mStartStuckTime = 0;
            }
            break;
        }
    case STATE_STOP:
        {
            // NOTE:
            // Don't 
            //mBrake.SetValue( 1.0f );
            mHandBrake.SetValue( 1.0f );

            //float dp = GetVehicle()->mVehicleFacing.DotProduct( GetVehicle()->mVelocityCM );

            float speed = GetVehicle()->mSpeed;
            if(( speed < 1.0f ))//< 0.15f ))/* && ( dp > 0.0f ))*/
            {
                //rDebugPrintf( "STOPPED: speed = %f, which is < 0.15f, so I'll ACCEL\n", speed );
                newState = STATE_ACCEL;
            }
            
            break;
        }
    case STATE_STUNNED:
        {
            mSecondsStunned += timeins;
            mHandBrake.SetValue( 1.0f );
            if( mSecondsStunned >= MAX_SECONDS_STUNNED )
            {
                //rDebugPrintf( "STUNNED: Been stunned for %f seconds, so I'll REVERSE\n", mSecondsStunned );
                mStartStuckTime = radTimeGetMilliseconds();
                newState = STATE_REVERSE;
            }
            break;
        }
    case STATE_OUT_OF_CONTROL:
        {
            rAssert( GetVehicle()->mOutOfControl );
            rAssert( GetVehicle()->mVehicleState == VS_SLIP );

            mGas.SetValue( givergas );

            mSecondsOutOfControl += timeins;
            if( mSecondsOutOfControl >= MAX_SECONDS_OUT_OF_CONTROL )
            {
                // if been out of control long enough, transit out...
                GetVehicle()->mOutOfControl = false;
                newState = STATE_ACCEL;
            }
            else
            {
                // do stuff while we're out of control...
                //GetVehicle()->mVehicleState = VS_SLIP;
                
                float steering = mSteering.GetValue();
                steering *= OUT_OF_CONTROL_STEERING_MODIFIER;
                mSteering.SetValue( steering );
            }
            break;
        }
    default:
        {
            rAssert( false );
            break;
        }
    }

    if( newState != mState )
    {
        mState = newState;
    }
}

//=============================================================================
// VehicleAI::DriveTowards
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleAI::DriveTowards( rmt::Vector* dest, float &distToTarget, 
                              float &steering )
{
    // Return a steering value clamped between -1 and 1, analogous to 
    // the steering stick on the game controller. Value of 1 means we'll
    // be turning the wheel to MaxWheelAngle (tunable parameter in .con file)
    // to the Right side; -1 is to the left side. 

    // Remember: CxB=A, BxA=C, AxC=B
    //              
    //          C
    //          |    B      
    //          |  /
    //          |/
    //           \
    //             \
    //               A
    steering = 0.0f;

    Vehicle* myVehicle = GetVehicle();
    rAssert( myVehicle != NULL );

    rmt::Vector vec2dest, myPos;
    myVehicle->GetPosition( &myPos );

    myPos.y = dest->y; // get rid of y values.. we'll work solely in xz plane

    if( (*dest).Equals( myPos, 0.001f ) )
    {
        return;
    }

    vec2dest.Sub( *dest, myPos );

    distToTarget = vec2dest.Magnitude();

    // If we're not doing evasive manoeuver, we go about business as usual
    vec2dest.Scale( 1.0f / distToTarget );

    rmt::Vector myHeading;
    myVehicle->GetHeading( &myHeading );
    myHeading.y = 0.0f;
    myHeading.NormalizeSafe();
    rAssert( rmt::Epsilon( myHeading.MagnitudeSqr(), 1.0f, 0.00001f ) );

    rmt::Vector myUp, myRightSide;
    /*
    // find out which way we have to turn by getting the vector
    // representing our right side & dotting with our vec2dest.
    myVehicle->GetVUP( &myUp );
    myRightSide = myVehicle->mVehicleTransverse;
    */
    myUp.Set( 0.0f, 1.0f, 0.0f );
    myRightSide.CrossProduct( myUp, myHeading );

    float rightDot = myRightSide.DotProduct( vec2dest );
    float turnDir = 0.0f;
    if( rightDot < 0.0f )
    {
        // dest on my left
        turnDir = -1.0f;
    }
    else
    {
        // dest on my right or exactly behind me
        turnDir = 1.0f;
    }

    // Need to determine the steering required to get to destination.
    // 


    // first, find the angle between my heading and my destination vector.
    // TODO: 
    // The destination vector should be projected onto the plane of my vehicle
    // as described by my heading and my rightside vector before we figure out
    // the angle, but we'll ignore this for now because it involves too many
    // float ops to do projection matrix calcs.
    // For now, it's ok to clamp down all y values to 0 and assume xz-plane flatness

    /*
    myHeading.y = 0.0f;
    myHeading.NormalizeSafe();

    vec2dest.y = 0.0f;
    vec2dest.NormalizeSafe();
    */

    float dotprod = myHeading.DotProduct( vec2dest ); 

    // need to place these checks in to make sure we're not going out of bounds
    // when we feed the value into ACos
    if(dotprod > 0.99999f)
    {
        dotprod = 0.99999f;
    }
    else if( dotprod < -0.9999f )
    {
        dotprod = -0.99999f;
    }


    float alphaInRadians = rmt::ACos( dotprod ); //*** ACK! ArcCosine.. unavoidable ***

    float maxWheelTurnInRadians = rmt::DegToRadian(myVehicle->mDesignerParams.mDpMaxWheelTurnAngle);
    rAssert( maxWheelTurnInRadians > 0.0f );

    steering = alphaInRadians / maxWheelTurnInRadians;
    if( steering > 1.0f )
    {
        steering = 1.0f;
    }
    steering *= turnDir;

    rAssert( !rmt::IsNan( steering ) );
}

void VehicleAI::SetDestination( rmt::Vector& pos )
{
    rAssert( !rmt::IsNan(pos.x) && !rmt::IsNan(pos.y) && !rmt::IsNan(pos.z) );
    mDestination = pos;
}
void VehicleAI::SetNextDestination( rmt::Vector& pos )
{
    rAssert( !rmt::IsNan(pos.x) && !rmt::IsNan(pos.y) && !rmt::IsNan(pos.z) );
    mNextDestination = pos;
}


/*
bool VehicleAI::FindNextRoad( const Road** pRoad, rmt::Vector& nextDestination )
{
    rAssert( pRoad != NULL );
    rAssert( (*pRoad) != NULL );

    const Intersection* currentInt = (*pRoad)->GetDestinationIntersection();
    rAssert( currentInt != NULL );
    
    rmt::Vector mypos;
    GetVehicle()->GetPosition( &mypos );
    const Road* nextRoad = NULL;
    const Road* closestRoad = (*pRoad);
    
    float closestDist = 1000000.0f; 
    rmt::Vector closestPos;
    rmt::Vector vec2pos;
    float dist;
    int segmentIndex;

    for( int i = 0; i < MAX_ROADS; i++ )
    {
        nextRoad = currentInt->GetRoadOut( i );
        if( nextRoad != NULL )
        {
            if( RoadManager::FindClosestPointOnRoad( nextRoad, nextDestination, 
                closestPos, dist, segmentIndex ))
            {
                if( dist < closestDist )
                {
                    closestDist = dist;
                    closestRoad = nextRoad;
                }
            }
        }
    }

    (*pRoad) = closestRoad;

    return true;
}
*/



bool VehicleAI::FindClosestPathElement( 
    rmt::Vector& pos, 
    RoadManager::PathElement& elem, 
    RoadSegment*& seg, 
    float& segT,
    float& roadT,
    bool considerShortcuts )
{
    elem.elem = NULL;
    seg = NULL;
    segT = 0.0f;
    roadT = 0.0f;


    RoadManager* rm = RoadManager::GetInstance();
    rAssert( rm != NULL );

    const Road* road = NULL;
    RoadSegment* roadseg = NULL;
    int segIndex = -1;
    float in = 0.0f;
    float lateral = 0.0f;

    // The last argument we pass in forces the find query to 
    // IGNORE shortcut roads
    rm->FindRoad( pos, &road, &roadseg, segIndex, in, lateral, considerShortcuts );
    if( road == NULL )
    {
        // so if we are not on a road, see if we're in an intersection
        Intersection* startInt = rm->FindIntersection( pos );
        if( startInt == NULL )
        {
            return false;
        }
        else
        {
            elem.type = RoadManager::ET_INTERSECTION;
            elem.elem = startInt;
        }
    }
    else
    {
        rAssert( roadseg != NULL );
        rAssert( roadseg->GetRoad() == road );
        rAssert( 0 <= segIndex && segIndex < (int)(road->GetNumRoadSegments()) );
        rAssert( roadseg->GetSegmentIndex() == (unsigned int) segIndex );
        //rAssert( !road->GetShortCut() );

        seg = (RoadSegment*) roadseg;

        segT = RoadManager::DetermineSegmentT( pos, seg );
        roadT = RoadManager::DetermineRoadT( seg, segT );

        elem.type = RoadManager::ET_NORMALROAD;
        elem.elem = (Road*)road;

    }
    return true;

}

void VehicleAI::GetLastPathInfo( 
    RoadManager::PathElement& elem,
    RoadSegment*& seg,
    float& segT,
    float& roadT )
{
    elem = mLastPathElement;
    seg = (RoadSegment*) mLastRoadSegment;
    segT = mLastRoadSegmentT;
    roadT = mLastRoadT;
}

void VehicleAI::GetRacePathInfo( 
    RoadManager::PathElement& elem,
    RoadSegment*& seg, 
    float& segT,
    float& roadT )
{
    elem = mRacePathElement;
    seg = (RoadSegment*) mRaceRoadSegment;
    segT = mRaceRoadSegmentT;
    roadT = mRaceRoadT;
}

