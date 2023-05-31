//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TrafficAI.cpp
//
// Description: Implement TrafficAI
//
// History:     09/09/2002 + Accel/Decel behavior - Dusit Eakkachaichanvet
//              06/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#include <stdlib.h>

//========================================
// Project Includes
//========================================
#include <ai/vehicle/TrafficAI.h>
#include <ai/vehicle/vehicleairender.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <roads/lane.h>
#include <roads/intersection.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/roadsegmentdata.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/redbrick/trafficlocomotion.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/character/character.h>
#include <render/culling/swaparray.h>


//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
const char* TRAFFICAI_NAMESPACE = "Traffic\\TrafficAI";

//const float TrafficAI::LANE_CHANGE_DIST = 15.0f;
const float LANE_CHANGE_SECONDS = 1.0f;
const float MIN_LANE_CHANGE_DIST = 15.0f;

const float TrafficAI::SECONDS_LOOKAHEAD = 1.5f;
const float TrafficAI::LOOKAHEAD_MIN = 15.0f;
//const float FRUSTRUM_DIST = 15.0f;
const float DECEL_LO = -10.0f; // We don't want to decel any more slowly than this
const float ACCEL = 3.5f;

const float CAR_SPAN = 1.5f;
const float CAR_LENGTH = 2.5f;
const float CHAR_SPAN = 0.5f;
const float CHAR_LENGTH = 0.5f;

const float MIN_SECONDS_BETW_LANE_CHANGES = 5.0f;

const float SECONDS_SWERVING = 0.8f;

const float SWERVE_HEADLIGHT_SCALE = 1.0f;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// TrafficAI::TrafficAI
//=============================================================================
// Description: Constructor.
//
// Parameters:	none
//
// Return:      N/A.
//
//=============================================================================
TrafficAI::TrafficAI( Vehicle* vehicle ) :
    AiVehicleController( vehicle )
{
    Init();
}

void TrafficAI::RegisterAI()
{
#ifdef DEBUGWATCH
    mRenderHandle = VehicleAIRender::GetVehicleAIRender()->RegisterAI( this, VehicleAIRender::TYPE_TRAFFIC_AI );
    rAssert( mRenderHandle != -1 );
#endif
}

void TrafficAI::UnregisterAI()
{
#ifdef DEBUGWATCH
    if( mRenderHandle >= 0 )
    {
        VehicleAIRender::GetVehicleAIRender()->UnregisterAI( mRenderHandle );
        mRenderHandle = -1;
    }
#endif
}

TrafficAI::~TrafficAI()
{
    UnregisterDebugInfo();
#ifdef DEBUGWATCH
    UnregisterAI();
#endif
}

//=============================================================================
// TrafficAI::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficAI::Init()
{
    // BEHAVIOR
    mDirection = LEFT;  //STRAIGHT; //RIGHT;

    // AFFILIATIONS
    mLane = NULL;
    mPrevLane = NULL;
    mLaneIndex = 0;
    mLaneLength = 0.0f;
    mSegment = NULL;
    mSegmentIndex = 0;
    mT = 0.0f;
    mAISpeed = 0.0f;
    mPrevIntersection = NULL;

    // STATES
    mNeedToSuddenlyStop = false;
    mIsActive = false;
    mIsInIntersection = false;
    if( mState == SWERVING )
    {
        StopSwerving();
    }
    mState = DRIVING;
    mPrevState = DRIVING;
    mSecondsDriving = 0.0f;

    // CONSTANTS
    mStopForSomethingDecel = 0.0f;

    RegisterDebugInfo();
    mRenderHandle = -1;

    mSecondsSinceLastTriggeredImpedence = 0.0f;
    mLastThingThatImpededMe = NULL;

    mSecondsSinceLaneChange = 0.0f;

    mOriginalMaxWheelTurnAngle = GetVehicle()->mDesignerParams.mDpMaxWheelTurnAngle;
    mSwervingLeft = false;

    mSwerveHighBeamOn = false;
    mOriginalHeadlightScale = GetVehicle()->mGeometryVehicle->GetHeadlightScale();
    mSecondsSwerveHighBeam = 0.0f;

    mEndOfRoadPos.Set( 0.0f, 0.0f, 0.0f );
}

void TrafficAI::Init( Vehicle* vehicle, 
                      Lane* lane,
                      unsigned int laneIndex,
                      RoadSegment* segment,
                      unsigned int segmentIndex,
                      float t, 
                      float mps )
{
    rAssert( vehicle );
    rAssert( lane );
    rAssert( segment );

    // BEHAVIOR
    mDirection = LEFT;

    // AFFILIATIONS
    mLane = lane;
    mPrevLane = lane;
    mLaneIndex = laneIndex;
    mSegment = segment;
    mSegmentIndex = segmentIndex;
    mT = t;
    SetAISpeed( mps );
    mPrevIntersection = NULL;
    
    // STATES 
    mNeedToSuddenlyStop = false;
    mState = DRIVING;
    mPrevState = DRIVING;
    mIsInIntersection = false;
    mIsActive = false;
    mSecondsDriving = 0.0f;

    // CONSTANTS
    mStopForSomethingDecel = 0.0f;

    RegisterDebugInfo();
    mRenderHandle = -1;

    mSecondsSinceLastTriggeredImpedence = 0.0f;
    mLastThingThatImpededMe = NULL;

    mSecondsSinceLaneChange = 0.0f;

    mOriginalMaxWheelTurnAngle = vehicle->mDesignerParams.mDpMaxWheelTurnAngle;
    mSwervingLeft = false;

    mSwerveHighBeamOn = false;
    mOriginalHeadlightScale = GetVehicle()->mGeometryVehicle->GetHeadlightScale();
    mSecondsSwerveHighBeam = 0.0f;

    mEndOfRoadPos.Set( 0.0f, 0.0f, 0.0f );
}

//=============================================================================
// TrafficAI::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficAI::Update( float seconds )
{
    // reset VehicleController values till we need them
    mGas.SetValue( 0.0f );
    mBrake.SetValue( 0.0f );
    mReverse.SetValue( 0.0f );

    //NOTE: we COULD keep handbrake on (so we don't drift to a stop)
    // but we'll make traffic more an obstacle when you hit them.
    mHandBrake.SetValue( 0.0f ); 

    mSteering.SetValue( 0.0f );
    mHorn.SetValue( 0.0f );

    if( GetVehicle() == NULL )
    {
        return;
    }
    if( !mIsActive && mState != SWERVING )
    {
        return;
    }

    // ok, we're not dead... take handbrake off so we can drive normally 
    mHandBrake.SetValue( 0.0f ); 

    mSecondsSinceLastTriggeredImpedence += seconds;
    mSecondsSinceLaneChange += seconds;

    BEGIN_PROFILE( "Traffic AI Update" );

    switch ( mState )
    {
    case DEAD:
        {
            SetAISpeed( 0.0f );
        }
        break;

    //case SPLINING: // fall thru
    case DRIVING: // fall thru
        {
            mSecondsDriving += seconds;

            ObstacleType foundSOMETHING = OT_NOTHING;
            float distFromSOMETHINGSqr = 10000.0f;
            void* SOMETHING = NULL;
            bool SOMETHINGOnMyRight = false;
            CheckForObstacles( foundSOMETHING, distFromSOMETHINGSqr, SOMETHING, SOMETHINGOnMyRight );

            // ==========================================
            // Determining & setting new speed/velocity
            // ===========================================

            switch( foundSOMETHING )
            {

            case OT_NOTHING:
                {
                    // now there's really nothing in front of us... Yet inspite of 
                    // this, we still want to lanechange on a whim some percentage of
                    // the time...
                    if( mSecondsSinceLaneChange >= MIN_SECONDS_BETW_LANE_CHANGES )
                    {
                        mSecondsSinceLaneChange = 0.0f;
                        int coinflip = rand() % 2;
                        if( coinflip == 0 )
                        {
                            AttemptLaneChange( foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );
                        }
                    }
                    MaintainSpeed( seconds );
                }
                break;
            case OT_PLAYERVEHICLE:
                {
                    // ok, so whatever it was that impeded us is still impeding us...
                    PerhapsTriggerImpedence( foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );

                    /*
                    // don't slow down for the player, but we can lanechange out of the way?
                    if( !AttemptLaneChange( foundSOMETHING, distFromSOMETHINGSqr, SOMETHING ) )
                    {
                    */

                    bool gonnaSwerve = false;
                    
                    //////////////////////////////////////
                    // SWERVING LOGIC
                    //
                    // ok, so lanechange failed... 
                    // if player is coming straight at us, swerve and flash headlights!
                    // else maintain speed
                    //
                    // If a new thing is impeding us, decide if we should swerve.
                    if( mLastThingThatImpededMe != SOMETHING )
                    {
                        int coinflip = 0; //rand() % 4; 
                        if( coinflip == 0 ) // one in n chance to swerve
                        {
                            Vehicle* playerVehicle = GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle();
                            rAssert( playerVehicle );

                            Vehicle* myVehicle = GetVehicle();
                            rAssert( myVehicle );

                            const float MY_MIN_SPEED_THRESHOLD = 8.333f;
                            const float PLAYER_MIN_SPEED_THRESHOLD = 11.111f;
                            if( playerVehicle->mSpeed > PLAYER_MIN_SPEED_THRESHOLD && 
                                GetVehicle()->mTrafficLocomotion->mActualSpeed > MY_MIN_SPEED_THRESHOLD )
                            {
                                /*
                                rmt::Vector playerDir;
                                playerVehicle->GetHeading( &playerDir );
                                rAssert( rmt::Epsilon( playerDir.MagnitudeSqr(), 1.0f, 0.001f ) );

                                rmt::Vector myDir;
                                myVehicle->GetHeading( &myDir );
                                rAssert( rmt::Epsilon( myDir.MagnitudeSqr(), 1.0f, 0.001f ) );
                                
                                const float INCOMING_COS_ALPHA = -0.9396926f;
                                if( myDir.Dot( playerDir ) < INCOMING_COS_ALPHA )
                                {
                                    // ok, he's heading "right at us", so swerve!
                                    StartSwerving();
                                    gonnaSwerve = true;
                                }
                                */

                                // see if he's also headed right at us
                                rmt::Vector playerPos, playerDir, playerRight, playerUp;

                                playerVehicle->GetPosition( &playerPos );
                                playerVehicle->GetVelocity( &playerDir );

                                playerDir.Normalize(); // *** SQUARE ROOT! ***
                                rAssert( rmt::Epsilon( playerDir.MagnitudeSqr(), 1.0f, 0.001f ) );

                                playerUp.Set( 0.0f, 1.0f, 0.0f );

                                playerRight.CrossProduct( playerUp, playerDir );
                                playerRight.Normalize(); // *** SQUARE ROOT! ***
                                rAssert( rmt::Epsilon( playerRight.MagnitudeSqr(), 1.0f, 0.001f ) );

                                float span = CAR_SPAN + CAR_SPAN;
                                float lookAhead = GetLookAheadDistance();

                                rmt::Vector myPos;
                                myVehicle->GetPosition( &myPos );

                                bool onPlayersRight = false;
                                if( WillCollide( playerPos, playerDir, playerRight, 
                                    span, lookAhead, myPos, onPlayersRight ) )
                                {
                                    // ok, he's heading more or less "right at us"... 
                                    //
                                    // Test if we can safely swerve.... 
                                    // From the recent WillCollide test, we know if we're on the right 
                                    // or the left side of the player's heading...
                                    // From CheckForObstacles, we know which side of OUR heading, 
                                    // the player is on... 
                                    // So... 
                                    // If we're on his right and he's on our right, or if we're on his
                                    // left and he's on our left, we can safely swerve. Otherwise, 
                                    // it's not clear which way we should swerve, so don't do it.

                                    if( (onPlayersRight && SOMETHINGOnMyRight) ||
                                        (!onPlayersRight && !SOMETHINGOnMyRight) )
                                    {
                                        StartSwerving( !onPlayersRight );
                                        gonnaSwerve = true;
                                    }
                                }
                            }
                        }
                    }
                    if( !gonnaSwerve )
                    {
                        StopForSomething( seconds, foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );
                    }
                    else
                    {
                        MaintainSpeed( seconds );
                    }
                }
                break;
            case OT_PLAYERCHARACTER:
                {
                    PerhapsTriggerImpedence( foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );
                    StopForSomething( seconds, foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );
                } 
                break;
            case OT_NONPLAYERVEHICLE: // fall thru
            case OT_NONPLAYERCHARACTER: // fall thru
                {
                    // determine if we want to and can lane-change...
                    // if so, then do the appropriate actions...
                    if( AttemptLaneChange( foundSOMETHING, distFromSOMETHINGSqr, SOMETHING ) )
                    {
                        MaintainSpeed( seconds );
                    }
                    else
                    {
                        StopForSomething( seconds, foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );
                    }
                }
                break;
            case OT_ENDOFROAD:
                {
                    if( mState != WAITING_AT_INTERSECTION )
                    {
                        mPrevIntersection = (Intersection*)SOMETHING;

                        // transit to WAITING AT INTERSECTION
                        mSecondsDriving = 0.0f;
                        SetState( WAITING_AT_INTERSECTION );

                        // set the lane's waiting vehicle to be this one!
                        mLane->mWaitingVehicle = GetVehicle()->mTrafficVehicle;

                        Road* myRoad = (Road*)mLane->GetRoad();

                        bool found = false;
                        SwapArray<Road*>* array = &(((Intersection*)SOMETHING)->mWaitingRoads);
                        for( int i=0; i<array->mUseSize; i++ )
                        {
                            if( myRoad == (*array)[i] )
                            {
                                found = true;
                                break;
                            }
                        }
                        if( !found )
                        {
                            array->Add( (Road*)myRoad );
                        }
                    }
                    StopForSomething( seconds, foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );
                }
                break;
            default:
                {
                    rAssertMsg( false, "What's this type??" );
                }
                break;
            } // end of switch( foundSOMETHING )
        }
        break;

    case WAITING_AT_INTERSECTION:
        {
            // Get my pos
            rmt::Vector myPos;
            GetVehicle()->GetPosition( &myPos );

            // Fake "found end of road obstacle" 
            ObstacleType foundSOMETHING = OT_ENDOFROAD;
            float distFromSOMETHINGSqr = (mEndOfRoadPos - myPos).MagnitudeSqr();
            void* SOMETHING = NULL;

            StopForSomething( seconds, foundSOMETHING, distFromSOMETHINGSqr, SOMETHING );

            // we gotta slow down or remain at rest
            GetVehicle()->mGeometryVehicle->ShowBrakeLights();
        }
        break;

    case WAITING_FOR_FREE_LANE:
        {
            // NOTE: Don't need to decel or anything. We just stop suddenly.
            this->GetVehicle()->mGeometryVehicle->ShowBrakeLights();
            SetAISpeed( 0.0f );
        }
        break;
    case SPLINING: // fall thru
    case LANE_CHANGING:
        {   
            MaintainSpeed( seconds );
        }
        break;
    case SWERVING:
        {
            // fiddle with the headlights
            mSecondsSwerveHighBeam += seconds;

            const float SECONDS_HIGH_BEAM_FLICKER_INTERVAL = 0.2f;
            if( mSecondsSwerveHighBeam > SECONDS_HIGH_BEAM_FLICKER_INTERVAL )
            {
                if( mSwerveHighBeamOn )
                {
                    // on long enough, turn it off
                    GetVehicle()->mGeometryVehicle->SetHeadlightScale( mOriginalHeadlightScale );
                    mSwerveHighBeamOn = false;
                    mSecondsSwerveHighBeam = 0.0f;
                }
                else
                {
                    // off long enough, turn it on again
                    GetVehicle()->mGeometryVehicle->SetHeadlightScale( SWERVE_HEADLIGHT_SCALE );
                    mSwerveHighBeamOn = true;
                    mSecondsSwerveHighBeam = 0.0f;
                }
            }

            mSecondsSwerving += seconds; 
            if( mSecondsSwerving > SECONDS_SWERVING )
            {
                StopSwerving();
            }
            else
            {
                // apply mGas and mSteering in one direction
                // and maybe flash headlights too!
                Swerve();
            }
        }
        break;
    default:
        {
            rAssert( false );
        }
    }

    rAssert( !rmt::IsNan( mAISpeed ) );

    END_PROFILE( "Traffic AI Update" );

}

void TrafficAI::StartSwerving( bool swerveRight )
{
    // transit to physics locomotion ===> vehicle->SetLocomotion( VL_PHYSICS ) 
    //   (this should already remove us from TrafficManager)
    // start timer: mSecondsSwerving = 0.0f
    Vehicle* v = GetVehicle();
    rAssert( v );

    rAssert( v->GetLocomotionType() == VL_TRAFFIC );

    v->SetLocomotion( VL_PHYSICS );
    mSecondsSwerving = 0.0f;

    // turn on highbeam
    mSwerveHighBeamOn = true;
    v->mGeometryVehicle->SetHeadlightScale( SWERVE_HEADLIGHT_SCALE );
    mSecondsSwerveHighBeam = 0.0f;

    // hack turning so it's more sensitive
    const float SWERVE_MAX_WHEEL_TURN_ANGLE = 120.0f;
    v->mDesignerParams.mDpMaxWheelTurnAngle = SWERVE_MAX_WHEEL_TURN_ANGLE;

    // figure out which direction we should be swerving

    /*
    // TODO:
    // Do we go by playerPos being on OUR right, or our pos being
    // on player's right? We should probably do the latter instead 
    // because our dir isn't all that accurate... and the player is 
    // probably going faster than we are!
    // If we DO decide to go with testing our pos against player's
    // velocity (to see if we're on his left or on his right), then
    // we can just pass in the value from the caller to this function.
    //

    rmt::Vector myPos, myRight;
    v->GetPosition( &myPos );
    myRight = v->mVehicleTransverse;
    rAssert( rmt::Epsilon( myRight.MagnitudeSqr(), 1.0f, 0.001f ) );

    Avatar* player = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( player );
    rmt::Vector playerPos;
    player->GetPosition( playerPos );

    rmt::Vector toPlayer = playerPos - myPos;

    if( toPlayer.Dot( myRight ) > 0.0f )
    {
        // player is on my right, so go left!
        mSwervingLeft = true;
    }
    else
    {
        // player is on my left, so go right!
        mSwervingLeft = false;
    }
    */
    mSwervingLeft = !swerveRight;

    SetState( SWERVING );
}

void TrafficAI::StopSwerving()
{
    Vehicle* v = GetVehicle();
    rAssert( v );
    rAssert( v->GetLocomotionType() == VL_PHYSICS );

    mHandBrake.SetValue( 1.0f );

    v->mDesignerParams.mDpMaxWheelTurnAngle = mOriginalMaxWheelTurnAngle;

    // turn off any highbeaming
    mSwerveHighBeamOn = false;
    v->mGeometryVehicle->SetHeadlightScale( mOriginalHeadlightScale );
    mSecondsSwerveHighBeam = 0.0f;

    // nothing to do here really... hmm.. 
    SetState( DEAD );
}

void TrafficAI::Swerve()
{
    // apply mGas and mSteering in one direction
    // and maybe flash headlights too!

    Vehicle* v = GetVehicle();
    rAssert( v );
    rAssert( v->GetLocomotionType() == VL_PHYSICS );

    mGas.SetValue( 1.0f );
    float swerveValue = mSwervingLeft? -1.0f : 1.0f;
    mSteering.SetValue( swerveValue ); 



    //mHorn.SetValue( 1.0f );
    //mHandBrake.SetValue( 1.0f );
}


void TrafficAI::PerhapsTriggerImpedence( ObstacleType foundSOMETHING, float distSqr, void* SOMETHING )
{
    // not close enough to trigger
    rAssert( foundSOMETHING == OT_PLAYERCHARACTER || 
             foundSOMETHING == OT_PLAYERVEHICLE );

    const float CAR_DIST_TRIGGER_IMPEDENCE_SQR = 100.0f;
    const float CHAR_DIST_TRIGGER_IMPEDENCE_SQR = 64.0f;

    const float SECONDS_BETW_TRIGGER_IMPEDENCE = 5.0f;

    float testDistSqr = (foundSOMETHING == OT_PLAYERCHARACTER)? 
        CHAR_DIST_TRIGGER_IMPEDENCE_SQR :
        CAR_DIST_TRIGGER_IMPEDENCE_SQR;

    if( distSqr > testDistSqr )
    {
        return;
    }

    if( SOMETHING == mLastThingThatImpededMe )
    {
        // You again! Wait for awhile, then honk again!
        if( mSecondsSinceLastTriggeredImpedence > SECONDS_BETW_TRIGGER_IMPEDENCE )
        {
            mSecondsSinceLastTriggeredImpedence = 0.0f;
            ::GetEventManager()->TriggerEvent( EVENT_TRAFFIC_IMPEDED, GetVehicle() );
        }
    }
    else // honk for an entirely new thing 
    {
        mLastThingThatImpededMe = SOMETHING;
        mSecondsSinceLastTriggeredImpedence = 0.0f;
        ::GetEventManager()->TriggerEvent( EVENT_TRAFFIC_IMPEDED, GetVehicle() );
    }
}


//=============================================================================
// TrafficAI::GetDirection
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TrafficAI
//
//=============================================================================
TrafficAI::Direction TrafficAI::DecideTurn()
{
    int coinflip = 0;

    // if rightmost lane, only go right or straight...
    if( mLaneIndex == 0 )
    {
        coinflip = rand() % 2;
        mDirection = (coinflip == 0)? STRAIGHT : RIGHT;
    }
    // if leftmost lane, only go left or straight...
    else if( mLaneIndex == (mSegment->GetNumLanes()-1) )
    {
        coinflip = rand() % 2;
        mDirection = (coinflip == 0)? STRAIGHT: LEFT;
    }
    // middle lanes only allowed to go straight...
    else
    {
        mDirection = STRAIGHT;
    }
    return mDirection;
}

//=============================================================================
// TrafficAI::SetSegmentIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      void 
//
//=============================================================================
void TrafficAI::SetSegmentIndex( unsigned int index )
{
    mSegmentIndex = index;

    mSegment = mLane->GetRoad()->GetRoadSegment( mSegmentIndex );
    rAssert( mSegment );

    mLaneLength = mSegment->GetLaneLength( mLaneIndex );
}

//=============================================================================
// TrafficAI::RegisterDebugInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficAI::RegisterDebugInfo()
{
}

//=============================================================================
// TrafficAI::UnregisterDebugInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TrafficAI::UnregisterDebugInfo()
{
}



//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************


// returns the vertices of a triangle frustrum...
void TrafficAI::GetFrustrum( const rmt::Vector& pos, 
                             const rmt::Vector& dir, 
                             rmt::Vector& leftFrustrumVertex,
                             rmt::Vector& rightFrustrumVertex )
{
    // our simple frustrum is a triangular PLANE that 
    // pans out from player's center of mass for a specified
    // distance, whose base spans a width of 1 vehicle diameter

    // TODO:
    // We should really start the frustrum at vehicle's
    // front, not vehicle's center. If our vehicle is a long 
    // truck, we could get in trouble (end up sensing no cars
    // because the frustrum lies completely within the vehicle)
    // 
    // For now, a triangular plane will do.
    //
    rmt::Vector end;
    end = pos + dir * 15.0f;

    rmt::Vector leftVec, rightVec;
    leftVec.Set( -1 * dir.z, dir.y, dir.x );
    rightVec.Set( dir.z, dir.y, -1 * dir.x );
 
    // HACK:
    // Hardcode the pan value (normally 1 car radius, but it seems to be larger
    // than expected).
    float pan = CAR_SPAN;

    leftFrustrumVertex = end + leftVec * pan;
    rightFrustrumVertex = end + rightVec * pan;
}



float TrafficAI::GetGoSpeedMps()
{
    // TODO:
    // Vary speed according to angle of turn in intersection (sharper turns
    // == slower speed)
    static const float N_WAY_INTERSECTION_SPEED_MPS = 9.0f;
    static const float NO_STOP_INTERSECTION_SPEED_MPS = 12.0f;
    if( mIsInIntersection  )
    {
        const Intersection* intersection = mLane->GetRoad()->GetSourceIntersection();
        if( intersection->GetType() == Intersection::N_WAY )
        {
            return N_WAY_INTERSECTION_SPEED_MPS;
        }
        else if( intersection->GetType() == Intersection::NO_STOP )
        {
            return NO_STOP_INTERSECTION_SPEED_MPS;
        }
    }

    // HACK:
    // This is a temp hack to force speed limit, just trying it out
    // If we want speed limit to be 60kph (they are currently defaulted to 50.0f 
    // in the data), then we'll have to change it in the world data.
    //float speed = mLane->GetSpeedLimit();
    float speed = TrafficManager::GetInstance()->GetDesiredTrafficSpeed();

    return speed;
}


bool TrafficAI::AttemptLaneChange( ObstacleType foundSOMETHING, float distFromSOMETHINGSqr, void* SOMETHING )
{
    // some sanity checking
    rAssert( mLane != NULL );
    rAssert( GetVehicle()->mTrafficVehicle != NULL );
    rAssert( GetVehicle()->mTrafficVehicle->GetIsActive() );
    rAssert( mLane == GetVehicle()->mTrafficVehicle->GetLane() );

    if( mState != TrafficAI::DRIVING )
    {
        return false;
    }

    if( mIsInIntersection )
    {
        //rDebugPrintf( "*** ABORTED LANECHANGE: Currently in an intersection ***\n" );
        return false;
    }

    if( mSegment->GetRoad()->GetNumLanes() <= 1 )
    {
        return false;
    }

    // check if a car or a character is in front...
    if( foundSOMETHING != OT_NONPLAYERVEHICLE &&
        foundSOMETHING != OT_PLAYERVEHICLE &&
        foundSOMETHING != OT_PLAYERCHARACTER  &&
        foundSOMETHING != OT_NONPLAYERCHARACTER && 
        foundSOMETHING != OT_NOTHING )
    {
        //rDebugPrintf( "*** ABORTED LANECHANGE: Not a car or character in front of us ***\n" );
        return false;
    }

    rmt::Vector myVel;
    GetVehicle()->GetVelocity( &myVel );
    if( rmt::Epsilon( myVel.MagnitudeSqr(), 0.0f, 0.0001f ) )
    {
        // if I have no speed, I shouldn't be lane-changing
        return false;
    }

    /*
    // TODO:
    // This isn't working quite right. Set up a test case and test it more properly...
    // check if he's moving faster than us (in the same direction as us)
    // don't need to lane change if he is...

    rmt::Vector velOfSOMETHING;
    if( foundSOMETHING == OT_NONPLAYERVEHICLE || 
        foundSOMETHING == OT_PLAYERVEHICLE ) // car
    {
        ((Vehicle*)SOMETHING)->GetVelocity(&velOfSOMETHING);
    }
    else if( foundSOMETHING == OT_PLAYERCHARACTER || 
        foundSOMETHING == OT_NONPLAYERCHARACTER ) // character
    {
        ((Character*)SOMETHING)->GetVelocity(velOfSOMETHING);
    }

    if( !rmt::Epsilon( velOfSOMETHING.MagnitudeSqr(), 0.0f, 0.0001f ) )
    {
        rmt::Vector hisVel = GetProjectionVector( velOfSOMETHING, myVel );
        rmt::Vector diff = hisVel - myVel;

        // if dp is +ve, the difference vector points in my direction,
        // therefore he is moving in the same direction as I am, but faster,
        // so don't lane change...
        float dp = diff.Dot( myVel );
        if( dp >= 0.0f )
        {
            rDebugPrintf( "*** ABORTED LANECHANGE: Object in front is moving away at faster velocity ***\n" );
            return false;
        }
    }
    */

    float laneChangeDist = MIN_LANE_CHANGE_DIST;
    if( foundSOMETHING != OT_NOTHING )
    {
        // 
        // Don't determine lanechangedist based on the number of seconds. 
        // it's not accurate anyway.
        // We want to grab the distance from the object, subtract the
        // objects' spans (roughly 5 meters altogether), account for some
        // slippage due to position and heading histories, and use this
        // as the lanechangedist
        // 
        float distFromSOMETHING = rmt::Sqrt(distFromSOMETHINGSqr);
        const float APPROX_SPANS = 6.0f;
        laneChangeDist = distFromSOMETHING - APPROX_SPANS;
    }

    // need at least this much room for lane change to look realistic
    if( laneChangeDist < MIN_LANE_CHANGE_DIST )
    {
        //rDebugPrintf( "*** ABORTED LANECHANGE: Lanechange distance (%.2f m) at this speed is too small ***\n", laneChangeDist );
        return false;
    }

    /*
    // Check how close we are to the object...
    // if it's closer than lane change dist, don't lane change 
    // since we won't clear the object
    float minDistFromObj = laneChangeDist + 0.5f;
    if( distFromSOMETHINGSqr < (minDistFromObj*minDistFromObj) )
    {
        float dist = rmt::Sqrt(distFromSOMETHINGSqr);
        rDebugPrintf( "*** ABORTED LANECHANGE: Object (%f m) closer than the %f m required to lanechange ***\n",
            dist, minDistFromObj);
        return false;
    }
    */

    float BUFFER = laneChangeDist;
    float safeDistSqr = (laneChangeDist + BUFFER) * (laneChangeDist + BUFFER);

    const Road* road = mSegment->GetRoad();
    rAssert( road != NULL );

    rmt::Vector myPos;
    rAssert( GetVehicle() != NULL );
    GetVehicle()->GetPosition(&myPos);

    /* THIS CHECK IS MADE INSTEAD IN BUILDLANECHANGECURVE

    // Check how close we are to the edge of the last segment
    // If it's too close to the intersection, don't lane change 
    // since we won't have time...

    rmt::Vector endPos, endFacing;

    RoadSegment* seg = road->GetRoadSegment( 
        mSegment->GetRoad()->GetNumRoadSegments() - 1 );
    rAssert( seg != NULL );       
    seg->GetLaneLocation( 1.0f, mLaneIndex, endPos, endFacing );

    float distSqr = (endPos - myPos).MagnitudeSqr();
    if( distSqr < safeDistSqr )
    {
        return false;
    }
    */

    // Tally a list of possible lanes ...
    unsigned int lanes[2];
    int count = 0;

    unsigned int laneInd;

    int leftOrRight = rand() % 2;
    if( leftOrRight == 0 ) // check left lane first
    {
        laneInd = mLaneIndex + 1;
        if( laneInd < road->GetNumLanes() )
        {
            lanes[count] = laneInd;
            count++;
        }
        if( mLaneIndex > 0 )
        {
            laneInd = mLaneIndex - 1;
            if( laneInd >= 0 )
            {
                lanes[count] = laneInd;
                count++;
            }
        }
    }
    else // check right lane first
    {
        if( mLaneIndex > 0 )
        {
            laneInd = mLaneIndex - 1;
            if( laneInd >= 0 )
            {
                lanes[count] = laneInd;
                count++;
            }
        }
        laneInd = mLaneIndex + 1;
        if( laneInd < road->GetNumLanes() )
        {
            lanes[count] = laneInd;
            count++;
        }
    }

    // no lane available, forget it...
    if( count == 0 )
    {
        //rDebugPrintf( "*** ABORTED LANECHANGE: No lane available ***\n" );
        return false;
    }

    // Now we have a list of Lanes...
    // Check if prospective lane's current traffic is > n meters away from me 
    // Check if prospective lane has enough density to hold my car

    Lane* targetLane = mLane;
    unsigned int targetLaneIndex = mLaneIndex;

    for( int i=0; i<count; i++ )
    {
        Lane* lane = road->GetLane( lanes[i] );
        int numTraffic = lane->mTrafficVehicles.mUseSize;
        if( numTraffic < (int)(lane->GetDensity()) )
        {
            bool clearToGo = true;

            for( int j=0; j<numTraffic; j++ )
            {
                TrafficVehicle* tv = lane->mTrafficVehicles[j];

                rmt::Vector vPos;
                tv->GetVehicle()->GetPosition( &vPos );

                float distSqr = (vPos - myPos).MagnitudeSqr();
                if( distSqr < safeDistSqr )
                {
                    clearToGo = false;
                    break;
                }
            }

            if( clearToGo )
            {
                targetLane = lane;
                targetLaneIndex = lanes[i];
            }
        }
    }

    // At this point, we have target lane determined
    rAssert( targetLane != NULL );
    rAssert( targetLaneIndex >= 0 );

    if( mLane != targetLane )
    { 
        bool succeeded = GetVehicle()->mTrafficLocomotion->BuildLaneChangeCurve( 
            mSegment, mT, mLaneIndex, targetLaneIndex, laneChangeDist );
        if( !succeeded )
        {
            //rDebugPrintf( "*** ABORTED LANECHANGE: Couldn't build lane change curve (too few segments ahead) ***\n" );
            return false;
        }

        // remove from old lane, add to new lane, update everything...
        GetVehicle()->mTrafficLocomotion->UpdateLanes
            ( GetVehicle()->mTrafficVehicle, mLane, targetLane );

        // update state...
        GetVehicle()->mTrafficLocomotion->mLaneChangeProgress = 0.0f;
        GetVehicle()->mTrafficLocomotion->mLaneChangeDist = laneChangeDist;
        GetVehicle()->mTrafficLocomotion->mLaneChangingFrom = mLaneIndex;
        SetState( LANE_CHANGING );

        mPrevLane = mLane;
        mLane = targetLane;
        mLaneIndex = targetLaneIndex;
        mLaneLength = mSegment->GetLaneLength( mLaneIndex );

        //rDebugPrintf( " >>>>>>>>>>>>>>>>> LANECHANGE! <<<<<<<<<<<<<<<<<<\n" );
        mSecondsSinceLaneChange = 0.0f;
        return true;
    }
    //rDebugPrintf( "*** ABORTED LANECHANGE: All adjacent lanes either is full (max density) or has a car too close by ***\n" );
    return false;
}

void TrafficAI::SetAISpeed( float mps )
{
    // Keep new speed within bounds (ensure we don't ever reverse this way)
    if( mps < 0.0f )
    {
        mps = 0.0f;
    }
    mAISpeed = mps;
}

float TrafficAI::GetAISpeed() const
{
    return mAISpeed;
}

void TrafficAI::MaintainSpeed( float seconds )
{
    float newSpeed = 0.0f;
    float desiredSpeed = GetGoSpeedMps();
    if( mAISpeed > desiredSpeed )
    {
        newSpeed = mAISpeed + DECEL_LO * seconds;
        // NOTE: DOn't show brakelights for tiny adjustments in speed...
        if( (mAISpeed - desiredSpeed) > 1.0f )
        {
            this->GetVehicle()->mGeometryVehicle->ShowBrakeLights();
        }
    }
    else
    {
        newSpeed = mAISpeed + ACCEL * seconds;
        this->GetVehicle()->mGeometryVehicle->HideBrakeLights();
    }

    SetAISpeed( newSpeed );
    mNeedToSuddenlyStop = false;

}


void TrafficAI::StopForSomething( float seconds, ObstacleType ID, float distSqr, void* obj )
{
    // show brakelights when we're slowing down for something
    this->GetVehicle()->mGeometryVehicle->ShowBrakeLights();

    float speed = GetVehicle()->mTrafficLocomotion->mActualSpeed;

    float EPSILON = 0.001f; // this is to prevent divide-by-zero in normal slowing
    float BUFFER = CAR_LENGTH; // BUFFER is the minimal separation dist required to NOT interpenetrate
    switch( ID ) // now account for the length of SOMETHING
    {
    case OT_NOTHING:
        return;
    case OT_PLAYERVEHICLE:  // fall thru
    case OT_NONPLAYERVEHICLE:
        {
            // another vehicle
            BUFFER += CAR_LENGTH;
        }
        break;
    case OT_PLAYERCHARACTER: // fall thru 
    case OT_NONPLAYERCHARACTER:
        {
            // a character
            BUFFER += CHAR_LENGTH;
        }
        break;
    case OT_ENDOFROAD: 
        // no need to modify "buffer"
        break;
    default:
        rAssert( false );
    }

    ///////////////////////////
    // Determine if we should be super-slowing... 
    // We do normal slowing when we're far away. If this isn't enough
    // we super-slow when we're getting too close. If this is STILL not 
    // enough and if a non-submitting entity (for now, just traffic)
    // is ahead of us, we suddenly stop so we don't collide with it.

    // This is as close as we want to get to any car 
    float distDoSuddenStop = 1.0f; // this or less means we need to stop suddenly 

    /////////// Figure out distance needed to do super braking /////////////

    float numFramesInHistory = TrafficLocomotion::ON_ROAD_HISTORY_SIZE;
    float secondsHistoryDelay = numFramesInHistory * TrafficLocomotion::SECONDS_BETW_HISTORY_UPDATES;
    float superSlowDriftDist = secondsHistoryDelay * speed;

    // this is the distance the car will drift if we set speed to zero, thus
    // populating all of POS_HISTORY one frame at a time (more or less) with the same pos.
    float distDoSuperSlow = superSlowDriftDist + distDoSuddenStop;

    /////////// Figure out distance we have to work with ////////////
    float stopDist = rmt::Sqrt( distSqr ) - BUFFER; // *** SQUARE ROOT! ***

    /////////// Figure out if we need to stop suddenly /////////////
    /*
    // NOTE: Disable need to suddenly stop. Traffic cars are submitting now
    mNeedToSuddenlyStop = false;
    if( stopDist <= distDoSuddenStop )
    {
        if( ID == OT_NONPLAYERVEHICLE )
        {
            Vehicle* npv = (Vehicle*) obj;
            if( npv->GetLocomotionType() == VL_TRAFFIC &&
                npv->mVehicleType == VT_TRAFFIC )
            {
                mNeedToSuddenlyStop = true;
                mStopForSomethingDecel = -10000.0f;
                SetAISpeed( 0.0f );
            }
        }
    }
    */

    /////////// Figure out if we're doing super slow or normal slow /////////////
    if( stopDist <= distDoSuperSlow )
    {
        mStopForSomethingDecel = -10000.0f;
        SetAISpeed( 0.0f );
    }
    else
    {
        // here stopDist is greater than distDoSuperSlow,
        // which hopefully means it is > zero
        rAssert( stopDist > EPSILON );

        mStopForSomethingDecel = -1*speed*speed / (2*stopDist); 

        /*
        // HACK:
        // The normal slow just ain't doing enough...
        // Due to the fact that we use POS_HISTORY, the AISpeed we're setting will
        // add only one new POS_HISTORY item amidst 20 items; then the average of
        // these items is taken to determine new position. It's a very small 
        // contribution and skews our braking capabilities
        // 
        const float FUDGEFUDGEFUDGE = 1.5f;
        mStopForSomethingDecel *= FUDGEFUDGEFUDGE;
        */

        float newAISpeed = GetAISpeed() + mStopForSomethingDecel * seconds;
        SetAISpeed( newAISpeed );
    }

}

float TrafficAI::GetLookAheadDistance()
{
    return GetVehicle()->mTrafficLocomotion->mActualSpeed * SECONDS_LOOKAHEAD;
}

void TrafficAI::CheckForObstacles( ObstacleType& objID, float& distFromObjSqr, void*& obj, bool& objOnMyRight )
{
    float lookAhead = GetLookAheadDistance();
    if( lookAhead < LOOKAHEAD_MIN )
    {
        lookAhead = LOOKAHEAD_MIN;
    }

    // Get information about this Traffic Vehicle
    rmt::Vector playerPos;
    Avatar* avatar = ::GetAvatarManager()->GetAvatarForPlayer( 0 );
    avatar->GetPosition( playerPos );
    Vehicle* playerVehicle = avatar->GetVehicle();

    rmt::Vector pPos, pDir, pRightSide;
    GetVehicle()->GetPosition( &pPos );

    // *** WARNING ***
    // GetHeading actually returns a normalized FACING value, meaning
    // that a vehicle going in reverse will still have the same "heading"
    // as the vehicle going forward.
    //
    // But as far as traffic's concerned, we never go in reverse, so 
    // this doesn't matter. Should we ever go into reverse, this will be
    // a problem.
    //
    // we should use the velocity instead, but it's not accurate because
    // we're not in physicslocomotion (it's probably stil 0, though we're moving)
    //
    if( !mIsInIntersection )
    {
        // TODO: 
        // Should one day change this so that it deals effectively with corners... 
        // right now the direction is just straight ahead of the car
        // blegh..
        GetVehicle()->GetHeading( &pDir ); 
    }
    // special lookahead dir for being in intersection, cuz we'll turn weird
    else 
    {
        rmt::Vector* ways;
        int npts, currWay;
        GetVehicle()->mTrafficLocomotion->GetSplineCurve( ways, npts, currWay );

        if( currWay < (npts-1) ) // currway not the last point
        {
            const int NUM_WAYS_AHEAD_TO_LOOK = 20;
            int lookAheadIndex = currWay + NUM_WAYS_AHEAD_TO_LOOK;
            if( lookAheadIndex >= npts )
            {
                lookAheadIndex = npts-1;
            }
            pDir = ways[lookAheadIndex] - pPos;
            pDir.NormalizeSafe();
        }
        else // if currway is last point, then just get heading normally
        {
            GetVehicle()->GetHeading( &pDir );
        }
    }
    rAssert( rmt::Epsilon( pDir.MagnitudeSqr(), 1.0f, 0.001f ) );

    mLookAheadPt = pPos + pDir * lookAhead;

    // up x forward = right!
    rmt::Vector pUp( 0.0f, 1.0f, 0.0f );
    pRightSide.CrossProduct( pUp, pDir );//GetVehicle()->mVehicleTransverse;
    pRightSide.NormalizeSafe();
    rAssert( rmt::Epsilon( pRightSide.MagnitudeSqr(), 1.0f, 0.001f ) );

    rmt::Sphere pSphere;
    GetVehicle()->GetBoundingSphere( &pSphere );

    float span = 0.0f;

    // Get the vehicle/player character "q" that is:
    //  a) in front of us, AND
    //  b) closest to this vehicle "p" 
    // *** FORGET Item B because we have uniform deceleration
    //     just start slowing down if ANYONE's in front of us ***
    // Must take into account all active vehicles:
    //   - other traffic vehicles, 
    //   - player's vehicle,
    //   - other AI vehicles
    // And all characters:
    //   - peds (actually, if they don't cross the street, we don't have to check)
    //   - player char
    //   - NPCs (actually, if they aren't placed in the street, don't have to check)

    rmt::Vector qPos;

    int nActiveVehicles = 0;
    Vehicle** activeVehicles = NULL;
    VehicleCentral* vc = ::GetVehicleCentral();
    vc->GetActiveVehicleList( activeVehicles, nActiveVehicles );

    // make sure there's at least 1 active vehicle because 
    // WE ARE SUPPOSED TO BE THAT ONE.
    rAssert( nActiveVehicles >= 1 );

    ObstacleType foundSOMETHING = OT_NOTHING;
    float distFromSOMETHINGSqr = 100000.0f;
    void* SOMETHING = NULL;
    bool SOMETHINGOnMyRight = false;

    float distSqr = 100000.0f;

    Vehicle* aCar = NULL;
    int aCount = 0;
    int i;
    for( i=0; i<vc->GetMaxActiveVehicles(); i++ )
    {
        if( aCount >= nActiveVehicles )
        {
            break;
        }
        aCar = activeVehicles[i];
        if( aCar == NULL )
        {
            continue;
        }

        // Because ActiveList is a sparse list,
        // we track how many we've seen till we've reached 
        // nActiveVehicles (to save some work at the end)
        aCount++;

        // if it's our own vehicle, forget it
        if( aCar == GetVehicle() )
        {
            continue;
        }

        // Skip the vehicle if it's traffic and in different lane
        if( aCar->GetLocomotionType() == VL_TRAFFIC )
        {
            Lane* hisLane = NULL;
            Lane* myLane = mLane;

            // Remember.. we care if he's ahead of us
            if( mIsInIntersection )
            {
                // I'm in an intersection, so if he's ahead of us:
                //   if he's in intersection with us, he'll have same mLane (OUT lane)
                //   if he's not in intersection, he'll already be in mLane
                hisLane = aCar->mTrafficLocomotion->GetAILane();
            }
            else 
            {   
                // I'm not in an intersection, so if he's ahead of us:
                //   if he's in an intersection, get his previous lane, as it could be our lane
                //   if he's NOT in an intersection, make sure he's in the same lane as we are. 
                if( aCar->mTrafficLocomotion->IsInIntersection() )
                {
                    hisLane = aCar->mTrafficLocomotion->GetAIPrevLane();
                }
                else
                {
                    hisLane = aCar->mTrafficLocomotion->GetAILane();
                }
            }
            if( hisLane != myLane )
            {
                continue;
            }
        }

        //
        // Get info about the other car
        //
        aCar->GetPosition( &qPos );

        span = CAR_SPAN + CAR_SPAN;
        bool qPosLiesOnMyRight;
        if( WillCollide( pPos, pDir, pRightSide, span, lookAhead, qPos, qPosLiesOnMyRight ) )
        {
            distSqr = (pPos - qPos).MagnitudeSqr();
            if( distSqr < distFromSOMETHINGSqr )
            {
                if( aCar == playerVehicle )
                {
                    foundSOMETHING = OT_PLAYERVEHICLE;
                }
                else
                {
                    foundSOMETHING = OT_NONPLAYERVEHICLE;
                }
                distFromSOMETHINGSqr = distSqr;
                SOMETHING = aCar;
                SOMETHINGOnMyRight = qPosLiesOnMyRight;
            }
        }
    }


    // Not so fast! Must check to see if we'll be running over the player character!
    span = CAR_SPAN + CHAR_SPAN;
    bool charPosLiesOnMyRight;
    if( !avatar->IsInCar() && WillCollide( pPos, pDir, pRightSide, span, lookAhead, playerPos, charPosLiesOnMyRight ) )
    {
        distSqr = (pPos - playerPos).MagnitudeSqr();
        if( distSqr < distFromSOMETHINGSqr )
        {
            foundSOMETHING = OT_PLAYERCHARACTER;
            distFromSOMETHINGSqr = distSqr;
            SOMETHING = avatar->GetCharacter();
            SOMETHINGOnMyRight = charPosLiesOnMyRight;
        }
    }

    // Not so fast! Check if we're going to collide with any other characters 
    // we're supposed to avoid!
    int charCount = 0;
    int numChars = TrafficManager::GetInstance()->GetNumCharsToStopFor();
    for( int i=0; (i<TrafficManager::MAX_CHARS_TO_STOP_FOR) && (charCount<numChars); i++ )
    {
        Character* charToStopFor = TrafficManager::GetInstance()->GetCharacterToStopFor(i);
        if( charToStopFor != NULL )
        {
            rmt::Vector charPos;
            charToStopFor->GetPosition( charPos );

            charCount++;
            if( WillCollide( pPos, pDir, pRightSide, span, lookAhead, charPos, charPosLiesOnMyRight ) )
            {
                distSqr = (pPos - charPos).MagnitudeSqr();
                if( distSqr < distFromSOMETHINGSqr )
                {
                    foundSOMETHING = OT_NONPLAYERCHARACTER;
                    distFromSOMETHINGSqr = distSqr;
                    SOMETHING = charToStopFor;
                    SOMETHINGOnMyRight = charPosLiesOnMyRight;
                }
            }
        }
    }

    // Not so fast! If we are not already in an intersection, must check to see 
    // if we need to stop at this intersection
    if( !mIsInIntersection && mState != LANE_CHANGING )
    {
        // If the destination intersection is of type N_WAY, we check
        // to see if it's inside our frustrum...
        // 
        const Intersection* intersection = mLane->GetRoad()->GetDestinationIntersection();
        if( intersection->GetType() == Intersection::N_WAY &&
            intersection != mPrevIntersection )
        {
            //
            // if we haven't been driving long enough after we just got free from
            // an intersection, don't transit to WAITING_AT_INTERSECTION
            // (or we'll never go anywhere).
            //
            float lull = (float) NWayStop::NWAY_TURN_MILLISECONDS / 1000.0f;
            if( (mPrevState != WAITING_AT_INTERSECTION) ||
                (mSecondsDriving > lull && mPrevState == WAITING_AT_INTERSECTION) )
            {
                //
                // Gotta sense if we are nearing an intersection by seeing if the 
                // lane location at t = 1.0f of the last segment on this
                // road is inside our frustrum. 

                const Road* myRoad = mLane->GetRoad();
                rAssert( myRoad != NULL );
                unsigned int nSegments = myRoad->GetNumRoadSegments();

                rmt::Vector endOfRoadPos, dummy;
                myRoad->GetRoadSegment( nSegments - 1 )->GetLaneLocation
                    ( 1.0f, mLaneIndex, endOfRoadPos, dummy );
        
                span = CAR_SPAN;
                bool endOfRoadPosLiesOnMyRight;
                if( WillCollide( pPos, pDir, pRightSide, span, lookAhead, endOfRoadPos, endOfRoadPosLiesOnMyRight ) )
                {
                    float distToIntersectionSqr = (endOfRoadPos - pPos).MagnitudeSqr(); 
                    if( distToIntersectionSqr < distFromSOMETHINGSqr )
                    {
                        foundSOMETHING = OT_ENDOFROAD;
                        distFromSOMETHINGSqr = distToIntersectionSqr;
                        SOMETHING = (Intersection*)intersection;
                        SOMETHINGOnMyRight = endOfRoadPosLiesOnMyRight;

                        mEndOfRoadPos = endOfRoadPos;

                    }
                }
            }
        }                        
    }

    objID = foundSOMETHING;
    distFromObjSqr = distFromSOMETHINGSqr;
    obj = SOMETHING;
    objOnMyRight = SOMETHINGOnMyRight;

}

