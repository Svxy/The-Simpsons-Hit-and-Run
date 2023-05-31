//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TrafficManager.cpp
//
// Description: Implement TrafficManager
//
// History:     09/09/2002 + Spawning/Removing vehicles -- Dusit Eakkachaichanvet
//              04/07/2002 + Created -- Cary Brisebois
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
#include <mission/gameplaymanager.h>

#include <camera/supercammanager.h>
#include <worldsim/traffic/TrafficManager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/trafficlocomotion.h>
#include <memory/srrmemory.h>
#include <debug/profiler.h>
#include <worldsim/character/character.h>
#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/vehiclecentral.h>

#include <ai/vehicle/trafficai.h>

#include <roads/roadmanager.h>
#include <roads/roadmanager.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/roadsegmentdata.h>
#include <roads/lane.h>
#include <roads/intersection.h>
#include <roads/roadrendertest.h>

#include <render/Culling/ReserveArray.h>
#include <render/IntersectManager/IntersectManager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
TrafficManager* TrafficManager::mInstance = NULL;

//#define TRAFFIC_TEST
#ifdef TRAFFIC_TEST
    const float TrafficManager::FADE_RADIUS = 10.0f;
    const float TrafficManager::ADD_RADIUS = 30.0f;
    const float TrafficManager::CENTER_OFFSET = 20.0f;
    const float TrafficManager::REMOVE_RADIUS = 35.0f;
    const float TrafficManager::INITIAL_ADD_RADIUS = 20.0f;
#else
    const float TrafficManager::FADE_RADIUS = 85.0;
    const float TrafficManager::CENTER_OFFSET = 40.0f;
    const float TrafficManager::ADD_RADIUS = 65.0f;
    const float TrafficManager::REMOVE_RADIUS = 75.0f;
    const float TrafficManager::INITIAL_ADD_RADIUS = 100.0f;
#endif

const unsigned int TrafficManager::MILLISECONDS_BETWEEN_REMOVE = 200;
const unsigned int TrafficManager::MILLISECONDS_BETWEEN_ADD = 200; 
const unsigned int TrafficManager::MILLISECONDS_POPULATE_WORLD = 3000; 

const unsigned int MILLISECONDS_STUNNED_AFTER_DEACTIVATED = 3000;

const int MAX_TRAFFIC = 5;

// Define all the swatch colours here...
TrafficManager::SwatchColour TrafficManager::sSwatchColours[] =
{
    {148, 45, 12},
    {133, 124, 4},
    {110, 84,  145},
    {170, 43,  74},
    {48,  11,  102},

    {110, 30,  32},
    {140, 125, 207},
    {195, 98,  31},
    {122, 50,  69},
    {148, 191, 229},

    {0,   159, 123},
    {0,   75,  132},
    {43,  177, 166},
    {250, 166, 23},
    {172, 81, 127},

    {185, 162, 232},
    {229, 222, 33},
    {163, 203, 60},
    {213, 142, 210},
    {255, 239, 158},

    {122, 43,  103},
    {181, 133, 70},
    {68,  106, 171},
    {59,  149, 36},
    {205, 94,  0}
};

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************
void TrafficManager::InitDefaultModelGroups()
{
    for( int i=0; i<TrafficManager::MAX_TRAFFIC_MODEL_GROUPS; i++ )
    {
        // WORKING ONES
        //mTrafficModelGroups[i].AddTrafficModel( "minivanA", 5 );
        /*
        mTrafficModelGroups[i].AddTrafficModel( "compactA", 1 );
        mTrafficModelGroups[i].AddTrafficModel( "pickupA", 1 );
        mTrafficModelGroups[i].AddTrafficModel( "sportsA", 2 );
        mTrafficModelGroups[i].AddTrafficModel( "SUVA", 1 );
        */
        /*
        mTrafficModelGroups[i].AddTrafficModel( "coffin", 2 );
        mTrafficModelGroups[i].AddTrafficModel( "hallo",1 );
        mTrafficModelGroups[i].AddTrafficModel( "ship", 1 );
        mTrafficModelGroups[i].AddTrafficModel( "witchcar", 1 );
        */

        /*
        mTrafficModelGroups[i].AddTrafficModel( "sportsB", 1 );
        mTrafficModelGroups[i].AddTrafficModel( "wagonA", 1 );
        mTrafficModelGroups[i].AddTrafficModel( "minivanA", 1 );
        mTrafficModelGroups[i].AddTrafficModel( "taxiA", 2 );
        */
        /*
        mTrafficModelGroups[i].AddTrafficModel( "sedanA", 3 );
        mTrafficModelGroups[i].AddTrafficModel( "sedanB", 2 );
        */
        //mTrafficModelGroups[i].AddTrafficModel( "ambul", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "burnsarm", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "fishtruc", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "garbage", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "icecream", 5 );

        //mTrafficModelGroups[i].AddTrafficModel( "IStruck", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "nuctruck", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "pizza", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "schoolbu", 5 );
        //mTrafficModelGroups[i].AddTrafficModel( "votetruc", 5 );

        // NOT TESTED
        //mTrafficModelGroups[i].AddTrafficModel( "compactA", 5 );

        // DEFAULTS
        mTrafficModelGroups[i].AddTrafficModel( "compactA", 2 );
        mTrafficModelGroups[i].AddTrafficModel( "pickupA", 2 );
        mTrafficModelGroups[i].AddTrafficModel( "sportsA", 2 );
        mTrafficModelGroups[i].AddTrafficModel( "SUVA", 2 );
    }
    mCurrTrafficModelGroup = 0;
}


ITrafficSpawnController* TrafficManager::GetSpawnController()
{
    return (ITrafficSpawnController*) TrafficManager::GetInstance();
}


TrafficManager* TrafficManager::GetInstance() 
{
    MEMTRACK_PUSH_GROUP( "Traffic Manager" );
    if ( !mInstance )
    {
        mInstance = new(GMA_LEVEL_OTHER) TrafficManager();
    }
    MEMTRACK_POP_GROUP( "Traffic Manager" );
    return mInstance;
}

void TrafficManager::DestroyInstance()
{
    delete mInstance;
    mInstance = NULL;
}

void TrafficManager::HandleEvent( EventEnum id, void* pEventData )
{
    if( id == EVENT_VEHICLE_DESTROYED )
    {
        // ignore it if it's not one of our vehicles...
        Vehicle* v = (Vehicle*) pEventData;
        SwapInTrafficHusk( v );
    }

    else if( id == EVENT_REPAIR_CAR )
    {
        //
        // NOTE: When we want to support other cars than the user car 
        // picking up Wrench/repair icons in the main game, we should pass in
        // the vehicle pointer when we trigger this event.
        //
        Vehicle* currVehicle = GetGameplayManager()->GetCurrentVehicle();
        if( currVehicle == NULL )
        {
            return; // no current vehicle to repair.. oh well...
        }
        TrafficVehicle* tv = FindTrafficVehicle( currVehicle );
        if( tv == NULL )
        {
            return; // not one of ours, don't worry...
        }


        // repair this vehicle..
        if( currVehicle->mVehicleDestroyed )
        {
            bool usingHusk = false;
            Vehicle* husk = tv->GetHusk();
            if( husk )
            {
                usingHusk = true;
                // damage is extensive... gotta replace husk with original vehicle

                // obtain info from the husk
                rmt::Vector initPos, initDir;
                husk->GetPosition( &initPos );
                initDir = husk->GetFacing();
                rAssert( rmt::Epsilon( initDir.MagnitudeSqr(), 1.0f, 0.001f ) );

                // remove husk
                bool succeeded = GetVehicleCentral()->RemoveVehicleFromActiveList( husk );
                rAssert( succeeded );
                /*
                GetVehicleCentral()->SetVehicleController( tv->mActiveListIndex, NULL );
                tv->mActiveListIndex = -1;
                */

                ::GetVehicleCentral()->mHuskPool.FreeHusk( husk );
                // restore fade alpha if we set it, so other vehicles don't get confused
                husk->mGeometryVehicle->SetFadeAlpha( 255 ); 
                husk->Release();
                husk = NULL;
                tv->SetHusk( NULL );
                tv->SetHasHusk( false );

                currVehicle->SetInitialPosition( &initPos );
                float angle = GetRotationAboutY( initDir.x, initDir.z );
                currVehicle->SetResetFacingInRadians( angle );
                currVehicle->Reset();
                //currVehicle->SetLocomotion( VL_PHYSICS );

            }

            // put in original vehicle
            int res = GetVehicleCentral()->AddVehicleToActiveList( currVehicle );
            if( res == -1 )
            {
                // not supposed to happen since the list can't be full!!!
                // we TOOK something out of the list before adding something in
                // If this assert happens, it is both fatal and strange
                rAssert( false );
                return;
            }
            //tv->mActiveListIndex = res;
            //GetEventManager()->TriggerEvent( EVENT_TRAFFIC_SPAWN, currVehicle ); // tell sound

            if( usingHusk )
            {
                Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
                rAssert( avatar );

                // if the avatar is inside a vehicle the vehicle
                // is probably a husk, update this vehicle to be the original 
                // vehicle and place the character in this new vehicle
                // 
                if( avatar->IsInCar() )
                {
                    rAssert( avatar->GetVehicle() );
                    rAssert( GetVehicleCentral()->mHuskPool.IsHuskType( avatar->GetVehicle()->mVehicleID ) );

                    avatar->SetVehicle( currVehicle );

                    Character* character = avatar->GetCharacter();
                    GetAvatarManager()->PutCharacterInCar( character, currVehicle );
                }
            }

            // fire off event so Esan can know when we switch the vehicle on him.
            GetEventManager()->TriggerEvent( EVENT_VEHICLE_DESTROYED_SYNC_SOUND, currVehicle );

        }
        // repair any damage to original vehicle
        bool resetDamage = true;
        currVehicle->ResetFlagsOnly( resetDamage );
    }

    // If the player honks his horn, we trigger horn honking too for nearby traffic
    else if( id == EVENT_PLAYER_VEHICLE_HORN )
    {
        // if queue for the last time we honked horn hasn't been cleared yet, 
        // don't do more...
        if( mQueuedTrafficHorns.mUseSize > 0 )
        {
            return;
        }

        Vehicle* playerVehicle = (Vehicle*) pEventData;
        rAssert( playerVehicle );

        rmt::Vector playerPos;
        playerVehicle->GetPosition( &playerPos );

        for( int i=0; i<MAX_TRAFFIC; ++i )
        {
            TrafficVehicle* traffV = &mVehicles[i];
            if( !traffV->GetIsActive() )
            {
                continue;
            }

            if( mQueuedTrafficHorns.mUseSize >= MAX_QUEUED_TRAFFIC_HORNS )
            {
                break;
            }

            rmt::Vector traffPos;
            traffV->GetVehicle()->GetPosition( &traffPos );

            // within n meters; make sure n is less than traffic remove radius
            const float CLOSE_ENOUGH_TO_HONK_BACK_SQR = 625.0f; 

            float distSqr = (traffPos - playerPos).MagnitudeSqr();
            if( distSqr < CLOSE_ENOUGH_TO_HONK_BACK_SQR )
            {
                int coinflip = rand() % 10; // 1 in n chance of honking back
                if( coinflip == 0 )
                {
                    int delayInMilliseconds = rand() % 300 + 500;

                    TrafficHornQueue tmp;
                    tmp.delayInMilliseconds = static_cast<unsigned int>( delayInMilliseconds );
                    tmp.vehicle = traffV->GetVehicle();

                    mQueuedTrafficHorns.Add( tmp );
                }
            }
        }

    }
}



void TrafficManager::Init()
{
    //////////////////////////////////////////
    // Do normal cleaning up...
    Cleanup();

    //////////////////////////////////////////
    // Initialize some members
    mNumTraffic = 0;
    mMillisecondsBetweenRemove = TrafficManager::MILLISECONDS_BETWEEN_REMOVE;
    mMillisecondsBetweenAdd = 0;
    mMillisecondsPopulateWorld = TrafficManager::MILLISECONDS_POPULATE_WORLD;
    mDesiredTrafficSpeedKph = DetermineDesiredSpeedKph();

    Vehicle* newV = NULL; // vehicle
    Vehicle* newH = NULL; // vehicle husk

    //////////////////////////////////////////
    // Initialize Traffic cars
    for( int i=0 ; i<MAX_TRAFFIC ; i++ )
    {
        TrafficVehicle* tv = &mVehicles[i];
        rAssert( tv != NULL );

        newV = InitRandomVehicle();
        rAssert( newV != NULL );
        newV->AddRef();
        newV->SetLocomotion( VL_TRAFFIC );
        newV->mTrafficVehicle = tv;

        tv->SetVehicle( newV );
        tv->SetHusk( NULL );
    }

#ifdef DEBUGWATCH
    char nameSpace[64];
    sprintf( nameSpace, "Traffic Manager" );

    radDbgWatchAddFloat( &mDesiredTrafficSpeedKph,
                         "Global Traffic Target Speed",
                         nameSpace,
                         NULL,
                         NULL,
                         10.0f,
                         200.0f );
#endif

}


void TrafficManager::ClearOutOfSightTraffic()
{
    static const unsigned int MAX_MILLISECONDS_OUT_OF_SIGHT_BEFORE_REMOVAL = 5000;

    Vehicle* playerVehicle = GetGameplayManager()->GetCurrentVehicle();

    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        TrafficVehicle* traffV = &mVehicles[i];
        if( !traffV->GetIsActive() )
        {
            continue;
        }

        // if the player is using this vehicle, goddammit don't remove the thing
        if( traffV->GetVehicle() == playerVehicle )
        {
            continue;
        }

        if( traffV->mOutOfSight && 
            traffV->mMillisecondsOutOfSight > MAX_MILLISECONDS_OUT_OF_SIGHT_BEFORE_REMOVAL )
        {
            RemoveTraffic( i ); // remove this car
        }
    }
}


void TrafficManager::UpdateQueuedTrafficHorns( unsigned int milliseconds )
{
    // update our queue of traffic horns
    int i=0;
    while( i < mQueuedTrafficHorns.mUseSize )
    {
        if( mQueuedTrafficHorns[i].delayInMilliseconds <= milliseconds )
        {
            // trigger the event already, sheesh!
            GetEventManager()->TriggerEvent( EVENT_TRAFFIC_HORN,
                mQueuedTrafficHorns[i].vehicle );
            mQueuedTrafficHorns.Remove( i );
        }
        else
        {
            mQueuedTrafficHorns[i].delayInMilliseconds -= milliseconds;
            i++;
        }
    }
}



// ******************************
//
// BIG UPDATE METHOD
//
// ******************************

void TrafficManager::Update( unsigned int milliseconds )
{
    rAssert( 0 <= mNumTraffic && mNumTraffic <= MAX_TRAFFIC );
    
BEGIN_PROFILE( "Traffic Man" );  

    if( mMillisecondsPopulateWorld > milliseconds )
    {
        mMillisecondsPopulateWorld -= milliseconds;
    }
    else
    {
        mMillisecondsPopulateWorld = 0;
    }

    // play the queued up traffic horn sounds, if the time is right
    UpdateQueuedTrafficHorns( milliseconds );

    //
    // ====================================
    // Getting information about the player
    // ====================================
    //

    Vehicle* v = NULL; 

    rmt::Vector pPos, pVel;//, pDir;
    float pSpeed;

    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( avatar != NULL );
    // Avatar::GetPosition() will return position of vehicle if avatar inside 
    // vehicle. Same deal with Avatar::GetVelocity() & GetSpeedMps()
    // Remember that we should use VELOCITY rather than facing because 
    // the player can face one way and travel in reverse.
    //
    avatar->GetPosition(pPos);
    avatar->GetVelocity(pVel);
    pSpeed = avatar->GetSpeedMps();

    // Get the camera for Player 1. 
    // It's what we need to apply the center offset to our spawn & remove radii
    SuperCam* pCam = GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
    rmt::Vector camTarget;
    pCam->GetHeadingNormalized( &camTarget );
    rAssert( rmt::Epsilon(camTarget.MagnitudeSqr(), 1.0f, 0.0005f) );

    rmt::Vector center;
    if( mMillisecondsPopulateWorld > 0 )
    {
        center = pPos;
    }
    else
    {
        center = pPos + camTarget * TrafficManager::CENTER_OFFSET;
    }

    /*
    SuperCam* superCam = ::GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
    rAssert( superCam != NULL );
    superCam->GetPosition( &pPos );
    superCam->GetVelocity( &pVel );
    pSpeed = pVel.Magnitude(); // *** SQUARE ROOT! ***
    */
    /*
    if( pSpeed > 0.001f )
    {
        pDir = pVel;
        pDir.Scale(1.0f / pSpeed);
    }
    else
    {
        avatar->GetHeading( pDir );
    }
    rAssert( rmt::Epsilon( pDir.MagnitudeSqr(), 1.0f, 0.001f ) );
    */

    // for sphere intersection/containment testing
    float minDistSqr = 0.0f;
    rmt::Vector distVec;


BEGIN_PROFILE( "Traffic Man: Remove" );  

    if( mMillisecondsBetweenRemove < milliseconds )
    {
        mMillisecondsBetweenRemove = MILLISECONDS_BETWEEN_REMOVE;

        // don't remove if populating world
        if( mMillisecondsPopulateWorld == 0 )
        {
            //
            // ==================================
            // Remove vehicles
            // ===================================
            // "Remove" from consideration vehicles that:
            //  - are no longer in the player's Traffic Radius, and/or
            //  - haven't been in player's view for some time
            //
            rmt::Sphere traffSphere;
            traffSphere.Set( center, TrafficManager::REMOVE_RADIUS );
            ClearTrafficOutsideSphere( traffSphere );

            ClearOutOfSightTraffic();
        }
    }
    else 
    {
        mMillisecondsBetweenRemove -= milliseconds;
    }

END_PROFILE( "Traffic Man: Remove" );  


BEGIN_PROFILE( "Traffic Man: Add" );  

    //
    // ==============================
    // Adding cars to lanes as needed
    // ==============================
    // Imagine two overlapping circles. Circle1 is the traffic radius of your 
    // current position (center, radius), and Circle2 (center2, radius)
    // is the traffic radius of your future position (based on velocity and 
    // assumption that it'll be just as long to reach the next loop as it took 
    // to reach the current loop). 
    // 
    if( mTrafficEnabled )
    {

        if( mMillisecondsBetweenAdd < milliseconds )
        {
            mMillisecondsBetweenAdd = MILLISECONDS_BETWEEN_ADD;

            float addRadius = TrafficManager::ADD_RADIUS;
            if( mMillisecondsPopulateWorld > 0 )
            {
                addRadius = TrafficManager::INITIAL_ADD_RADIUS;
            }

            rmt::Sphere pSphere( center, addRadius );

            // FindRoadSegmentElems returns to us a list of RoadSegments whose bounding spheres
            // come in contact with the player's traffic radius. These contact points give us 
            // the segments on the FRINGE of the traffic radius, where can we add cars.
            // 
            // For each point of intersection, we only place down a car IF:
            //  a) the candidate car's predicted position in the NEXT frame lies within Circle 2 
            //     (player's traffic zone in the NEXT frame)
            // AND
            //  b) the lane containing this segment has fewer cars on it than the desired 
            //     density value.
            // 
            // The reason for FindRoadSegmentElems (which consequently forced us to build DSG 
            // Tree out of RoadSegments, involving a lot of work) is that a lane isn't a 
            // straight line. It's comprised of meandering segments. A lane can enter and exit
            // the player's Traffic Radius as many times as it desires. 
            // 
            ReserveArray<RoadSegment*> orList;
            ::GetIntersectManager()->FindRoadSegmentElems( center, addRadius, orList );

            bool noMoreFreeTrafficVehicles = false; 

            for( int i=0; i<orList.mUseSize; i++ )
            {
                RoadSegment* segment;
                unsigned int numLanes;

                segment = orList.mpData[i];
                rAssert( segment != NULL );

                numLanes = segment->GetNumLanes();
                rAssert( numLanes >= 1 );
    
                // loop through all the lanes for this segment
                for( unsigned int j=0; j<numLanes; j++ )
                {

                    Road* road;
                    Lane* lane;
                    unsigned int nDesiredDensity;

                    road = segment->GetRoad();
                    rAssert( road != NULL );

                    lane = road->GetLane( j );
                    nDesiredDensity = lane->GetDensity(); 

                    // HACK:
                    // We only have 5 traffic cars man c'mon... 
                    if(nDesiredDensity > 2 )
                    {
                        nDesiredDensity = 2;
                    }

                    // we add vehicle only if number on the lane < expected density 
                    // AND if projected next position of the car is in the projected
                    // next position of the player's Traffic Radius

                    if( lane->mTrafficVehicles.mUseSize < (int)(nDesiredDensity) )
                    {
                        // Here we're determining where in the world to place the car. 
                        // We try to place it at the lane location where it intersects 
                        // with our traffic zone (pSphere).
              
                        rmt::Vector startPos, startDir, endPos, endDir; 
                        segment->GetLaneLocation( 0.0f, j, startPos, startDir );
                        segment->GetLaneLocation( 1.0f, j, endPos, endDir );
                
                        rmt::Vector intPts[2];
                        int numIntersections = IntersectLineSphere( startPos, endPos, pSphere, intPts );
                        if(numIntersections <= 0)
                        {
                            continue;
                        }

                        rmt::Vector cPos, cDir; 
                        cDir.Sub( endPos, startPos );
                        cDir.Normalize(); // *** SQUARE ROOT! ***

                        // for each intersection point found, plant a vehicle
                        for( int k=0; k<numIntersections; k++ )
                        {
                            // HACK:
                            // Designers want maxtraffic to be always 5 if you're on foot
                            // unless mMaxTraffic is actually 0, in which case, we leave at 
                            // zero.
                            int maxTrafficToUse = mMaxTraffic;
                            if( mMaxTraffic > 0 && !avatar->IsInCar()) 
                            {
                                maxTrafficToUse = MAX_TRAFFIC_MODEL_GROUPS;
                            }

                            if( mNumTraffic < maxTrafficToUse )
                            {
                                // set cPos;
                                cPos = intPts[k];

                                // see if we got any more vehicles...
                                TrafficVehicle* tv = this->GetFreeTrafficVehicle();
                                if( tv == NULL )
                                {
                                    noMoreFreeTrafficVehicles = true;
                                    break;
                                }
                                v = tv->GetVehicle();
                                rAssert( v != NULL );


                                // 
                                // We should detect if we're placing this car on top of another car
                                // We need to search the entire ActiveVehicles list, not just the 
                                // traffic cars in our lane, to take into account vehicles that 
                                // are lying around on the road in VL_PHYSICS
                                //
                                int nActiveVehicles = 0;
                                Vehicle** activeVehicles = NULL;

                                VehicleCentral* vc;
                                vc = ::GetVehicleCentral();
                                vc->GetActiveVehicleList( activeVehicles, nActiveVehicles );


                                rmt::Sphere cSphere;
                                v->GetBoundingSphere( &cSphere );


                                bool tryNextPoint = false;
                                int vCount = 0;
                                for( int i=0; i<vc->GetMaxActiveVehicles(); i++ )
                                {
                                    if( vCount >= nActiveVehicles )
                                    {
                                        break;
                                    }

                                    Vehicle* aCar = activeVehicles[i];
                                    if( aCar == NULL )
                                    {
                                        continue;
                                    }
                                    vCount++;

                                    rmt::Vector aPos;
                                    rmt::Sphere aSphere;
                                    aCar->GetPosition( &aPos );
                                    aCar->GetBoundingSphere( &aSphere );

                                    float distSqr = (aPos - cPos).MagnitudeSqr();

                                    // if same as our lane, make sure they're at least some
                                    // lookahead distance away... 
                                    float minDist = 5.0f; // initial buffer...
                                    if( aCar->mTrafficLocomotion->GetAILane() == lane )
                                    {
                                        /*
                                        float lookAhead = aCar->mTrafficLocomotion->GetAISpeed() * 
                                            TrafficAI::SECONDS_LOOKAHEAD;
                                        */
                                        float lookAhead = aCar->mTrafficLocomotion->mActualSpeed * 
                                            TrafficAI::SECONDS_LOOKAHEAD;
                                        if( lookAhead < TrafficAI::LOOKAHEAD_MIN )
                                        {
                                            lookAhead = TrafficAI::LOOKAHEAD_MIN;
                                        }
                                        minDist += lookAhead;
                                    }
                                    else 
                                    {
                                        minDist += aSphere.radius + cSphere.radius;
                                    }

                                    float minDistSqr = minDist * minDist;
                                    if( distSqr < minDistSqr )
                                    {
                                        // if we're too near another car, don't spawn here...
                                        tryNextPoint = true;
                                        break;
                                    }
                                }
                                if( tryNextPoint )
                                {
                                    continue;
                                }


                                //
                                // Check if in NEXT few seconds, vehicle will still be in 
                                // player's traffic zone.
                                //
                                const float SECONDS_LOOK_AHEAD = 1.5f;

                                // next frame player pos & traffic zone
                                rmt::Vector center2 = center + pVel * SECONDS_LOOK_AHEAD;

                                float speedLimit = lane->GetSpeedLimit();
                                rmt::Vector cVel = cDir * speedLimit;
                                rmt::Vector cPos2 = cPos + cVel * SECONDS_LOOK_AHEAD;

                                minDistSqr = TrafficManager::REMOVE_RADIUS * TrafficManager::REMOVE_RADIUS;
                                distVec.Sub( cPos2, center2 );


                                if( distVec.MagnitudeSqr() < minDistSqr )
                                {

                                    bool succeeded = this->AddTraffic(lane, tv);
                                    if( !succeeded )
                                    {
                                        // can't add more traffic to this lane, do next lane
                                        continue;
                                    }


                                    // Determine which point on the segment we are at...
                                    // Since we know that cPos is on the line between startPos & endPos
                                    // of the segment, we only need to test one coordinate (that didn't
                                    // remain the same, of course)... This should give us a good enough
                                    // approximation (within 0.00001)
                                    //
                                    float segmentT = GetLineSegmentT( startPos, endPos, cPos );
                                    rAssert( 0.0f <= segmentT && segmentT <= 1.0f ); 

                                    // INITIALIZE

                                    // Get a random color for this vehicle
                                    pddiColour randomColour;
                                    GenerateRandomColour( randomColour );
                                    v->mGeometryVehicle->SetTrafficBodyColour( randomColour );

                                    // initialize vehicle's position & facing
                                    v->SetInitialPosition( &cPos );
                                    float angle = GetRotationAboutY( cDir.x, cDir.z );
                                    v->SetResetFacingInRadians( angle );
                                    v->Reset();
                                    v->mHijackedByUser = false;

                                    // set up TrafficLocomotion/TrafficAI info
                                    v->mTrafficLocomotion->Init();
                                    v->mTrafficLocomotion->InitPos( cPos );
                                    v->mTrafficLocomotion->InitFacing( cDir );
                                    v->mTrafficLocomotion->InitVehicleAI( v );
                                    v->mTrafficLocomotion->InitLane( lane, j, lane->GetSpeedLimit() );
                                    v->mTrafficLocomotion->InitSegment( segment, segment->GetSegmentIndex(), segmentT );
                                    v->mTrafficLocomotion->SetActive( true );

                                    // determine initial speed for traffic.
                                    // if far from road's end, go at desired speed
                                    float speed = 0.0f;
                                    rmt::Vector segEnd, segFacing;
                                    RoadSegment* lastSeg = road->GetRoadSegment( road->GetNumRoadSegments()-1 );
                                    lastSeg->GetLaneLocation( 1.0f, 0, segEnd, segFacing );
                                    const float MIN_DIST_FROM_ROAD_END_SQR = 100.0f;
                                    if( (cPos-segEnd).MagnitudeSqr() > MIN_DIST_FROM_ROAD_END_SQR )
                                    {
                                        speed = GetDesiredTrafficSpeed();
                                    }
                                    v->mTrafficLocomotion->SetAISpeed( speed );
                                    
                                    v->mVehicleType = VT_TRAFFIC;
                                    v->SetLocomotion(VL_TRAFFIC);

                                } // end if vehicle's next pos lies in radius of player's next traffic zone
                            }
                        } 
                        if( noMoreFreeTrafficVehicles )
                        {
                            break;
                        }
                    } // end if(lane density < desired density)

                } // end for-loop through all the lanes in a particular segment

                if( noMoreFreeTrafficVehicles )
                {
                    break;
                }

            } // end for-loop through all segments returned by DSGFind
        }
        else 
        {
            mMillisecondsBetweenAdd -= milliseconds;
        }
    }

END_PROFILE( "Traffic Man: Add" );

BEGIN_PROFILE( "Traffic Man: Update AI & Intersections" );  

    //
    // ============================================================================
    // Go through the vehicles, updating 
    // ============================================================================
    //

    // at first, no intersection has been updated in this update call.
    for( int i=0; i<MAX_INTERSECTIONS; i++ )
    {
        mpIntersections[i] = NULL;
    }
    int nIntersectionsUpdated = 0;
    
    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        TrafficVehicle* traffV = &mVehicles[i];
        if( !traffV->GetIsActive() )
        {
            continue;
        }

        if( traffV->HasHusk() )
        {
            v = traffV->GetHusk();
        }
        else
        {
            v = traffV->GetVehicle();
        }
        rAssert( v );

        // Test for out of sight of player 0... If so, increment timer, if not reset timer
        rmt::Vector vPos;
        v->GetPosition( &vPos );
        traffV->mOutOfSight = !GetGameplayManager()->TestPosInFrustrumOfPlayer( vPos, 0 );

        traffV->mMillisecondsOutOfSight += milliseconds;
        if( !traffV->mOutOfSight )
        {
            traffV->mMillisecondsOutOfSight = 0;
        }

        // Determine fade alpha.. All active (in the world) traffic vehicles
        // fade. Whether or not they're in VL_TRAFFIC or VL_PHYSICS is irrelevant
        SetVehicleFadeAlpha( v, pPos );

        // figure out if we should allow entering the traffic vehicle
        const float GETIN_SPEED_THRESHOLD_MPS = 4.5f; // this is as fast as character can run
        if( !avatar->IsInCar() && !v->mHijackedByUser )
        {
            rAssert( v->mVehicleType != VT_AI );
            if( v->mSpeed > GETIN_SPEED_THRESHOLD_MPS )
            {
                v->ActivateTriggers( false );
            }
            else
            {
                v->ActivateTriggers( true );
            }
        }

        if( v->GetLocomotionType() == VL_PHYSICS && !v->mHijackedByUser )
        {
            traffV->mMillisecondsDeactivated += milliseconds;

            // Check through vehicles VL_PHYSICS to see if we can bring any of them 
            // back to life:
            // - check health
            // - check distance from last segment
            // - check if we've been deactivated long enough
            AttemptResurrection( traffV );
        }

        if( v->GetLocomotionType() == VL_TRAFFIC )
        {
            // Update Vehicle AI
            //v->mTrafficLocomotion->UpdateAI(milliseconds);

            // Update intersection
            UpdateIntersection( milliseconds, v, nIntersectionsUpdated );
        }
    }
END_PROFILE( "Traffic Man: Update AI & Intersections" );  



END_PROFILE( "Traffic Man" );  
}

void TrafficManager::ClearTrafficOutsideSphere( const rmt::Sphere& s )
{
    rmt::Vector distVec;
    rmt::Vector vPos;
    rmt::Sphere vSphere;
    float minDistSqr = 0.0f;

    Vehicle* playerVehicle = GetGameplayManager()->GetCurrentVehicle();//GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle();

    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        TrafficVehicle* traffV = &mVehicles[i];
        rAssert( traffV != NULL );

        if( !traffV->GetIsActive() )
        {
            continue;
        }

        Vehicle* v = NULL;
        if( traffV->HasHusk() )
        {
            v = traffV->GetHusk();
        }
        else
        {
            v = traffV->GetVehicle();
        }
        rAssert( v != NULL );

        // if the player is driving this traffic vehicle, don't take it away from him!
        if( traffV->GetVehicle() == playerVehicle ) // get original vehicle to compare, not husk
        {
            continue;
        }

        v->GetPosition( &vPos );

        v->GetBoundingSphere( &vSphere );
        vSphere.centre = vPos;

        minDistSqr = s.radius * s.radius;
        distVec.Sub( vSphere.centre, s.centre );
        if( distVec.MagnitudeSqr() < minDistSqr )
        {
            // BAH! vehicle still in traffic zone... leave it alone
            continue;
        }

        // At this point, we want to kill the vehicle
        // because it is now outside our traffic zone
        RemoveTraffic( i ); 
    }
}

void TrafficManager::ClearTrafficInSphere( const rmt::Sphere& s )
{
    rmt::Vector distVec;
    rmt::Vector vPos;
    rmt::Sphere vSphere;
    float minDistSqr = 0.0f;

    Vehicle* playerVehicle = GetGameplayManager()->GetCurrentVehicle();//GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle();


    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        TrafficVehicle* traffV = &mVehicles[i];
        rAssert( traffV != NULL );

        if( !traffV->GetIsActive() )
        {
            continue;
        }

        Vehicle* v = NULL;
        if( traffV->HasHusk() )
        {
            v = traffV->GetHusk();
        }
        else
        {
            v = traffV->GetVehicle();
        }
        rAssert( v != NULL );

        // if the player is driving this traffic vehicle, don't take it away from him!
        if( traffV->GetVehicle() == playerVehicle ) 
        {
            continue;
        }

        v->GetPosition( &vPos );

        v->GetBoundingSphere( &vSphere );
        vSphere.centre = vPos;

        minDistSqr = s.radius * s.radius;
        distVec.Sub( vSphere.centre, s.centre );

        // If vehicle still in sphere... kill it
        if( distVec.MagnitudeSqr() < minDistSqr )
        {
            // At this point, we want to kill the vehicle
            // because it is now outside our traffic zone
            RemoveTraffic( i ); 
        }
    }
}


void TrafficManager::RemoveTraffic( Vehicle* vehicle )
{
    rAssert( vehicle != NULL );

    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        TrafficVehicle* tv = &mVehicles[i];
        rAssert( tv != NULL );

        Vehicle* v = NULL;
        if( GetVehicleCentral()->mHuskPool.IsHuskType( vehicle->mVehicleID ) )
        {
            v = tv->GetHusk();
        }
        else
        {
            v = tv->GetVehicle();
        }

        // found it.
        if( vehicle == v )
        {
            RemoveTrafficVehicle( tv );
        }
    }
}


void TrafficManager::EnableTraffic()
{
    if( !CommandLineOptions::Get(CLO_NO_TRAFFIC) && !DISABLETRAFFIC )
    {
        mTrafficEnabled = true;
    }
}

void TrafficManager::DisableTraffic()
{
    mTrafficEnabled = false;
}

void TrafficManager::AddCharacterToStopFor( Character* character )
{
    rAssert( character != NULL );

    bool assigned = false;

    for( int i=0; i<MAX_CHARS_TO_STOP_FOR; i++ )
    {
        if( mCharactersToStopFor[i] == character )
        {
            mCharactersToStopFor[i]->Release();
            mCharactersToStopFor[i] = NULL;
        }
        if( !assigned && mCharactersToStopFor[i] == NULL )
        {
            tRefCounted::Assign( mCharactersToStopFor[i], character );
            assigned = true;
            mNumCharsToStopFor++;
        }
    }
}

void TrafficManager::RemoveCharacterToStopFor( Character* character )
{
    rAssert( character != NULL );

    for( int i=0; i<MAX_CHARS_TO_STOP_FOR; i++ )
    {
        if( mCharactersToStopFor[i] == character )
        {
            mCharactersToStopFor[i]->Release();
            mCharactersToStopFor[i] = NULL;
            mNumCharsToStopFor--;
        }
    }
}

void TrafficManager::GenerateRandomColour( pddiColour& colour )
{
    int alpha = 255;
    int red, green, blue;

    int index = rand() % TrafficManager::NUM_SWATCH_COLOURS;
    red = sSwatchColours[index].red;
    green = sSwatchColours[index].green;
    blue = sSwatchColours[index].blue;
    /*
    red = rand() % 256;
    green = rand() % 256;
    blue = rand() % 256;
    */
    colour.Set( red, green, blue, alpha );
}

void TrafficManager::Deactivate( Vehicle* vehicle )
{
    rAssert( vehicle != NULL );

    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        TrafficVehicle* tv = &mVehicles[i];
        rAssert( tv != NULL );

        Vehicle* v = tv->GetVehicle();

        // found it.
        if( vehicle == v )
        {
            rAssert( v != NULL );
            v->mTrafficLocomotion->SetActive( false );
            // 
            // Remove the vehicle from the lane
            Lane* lane = tv->GetLane();
            if( lane )
            {
                for( int j=0; j<lane->mTrafficVehicles.mUseSize; j++ )
                {
                    if( tv == lane->mTrafficVehicles[j] )
                    {
                        lane->mTrafficVehicles.Remove(j);
                        break;
                    }
                }
            }

            // NOTE:
            // hold off NULLing out the lane variable.
            // We could use it in case we want to resurrect it
            //tv->SetLane( NULL );

            // NOTE:
            // DO NOT SET TrafficVehicle::mIsActive TO FALSE HERE, 
            // We want traffic manager to consider it as being
            // active still (in the sense that it's still in the world and is
            // subject to removal). We just want to remove it from the lane and 
            // deactivate its AI... 
            //tv->SetIsActive( false );

            // start the deactivation timer
            tv->mMillisecondsDeactivated = 0;
            tv->mCanBeResurrected = true;
        }
    }

}

void TrafficManager::SwapInTrafficHusk( Vehicle* vehicle )
{
    rAssert( vehicle );

    TrafficVehicle* tv = FindTrafficVehicle( vehicle );
    if( tv == NULL )
    {
        return;
    }

    // obtain info from the vehicle
    rmt::Vector initPos, initDir;
    vehicle->GetPosition( &initPos );
    initDir = vehicle->GetFacing();
    rAssert( rmt::Epsilon( initDir.MagnitudeSqr(), 1.0f, 0.001f ) );

    //
    // Deactivate traffic vehicle, take it out of the world, swap in a free husk...
    // that sort of thing
    //
    Deactivate( vehicle );
    bool succeeded = ::GetVehicleCentral()->RemoveVehicleFromActiveList( vehicle );
    rAssert( succeeded );
    //GetVehicleCentral()->SetVehicleController( tv->mActiveListIndex, NULL );
    GetEventManager()->TriggerEvent( EVENT_TRAFFIC_REMOVE, vehicle );

    // 
    // Now we grab husk and put it in place of the original vehicle 
    // 
    Vehicle* husk = InitRandomHusk( vehicle );
    if( husk == NULL )
    {
        // by returning here, we have removed the traffic car from the 
        // manager, the lane, and world scene... But not entirely! (RemoveTrafficVehicle
        // will be called on it when it goes out of range later...) Good job!
        return;
    }
    int res = ::GetVehicleCentral()->AddVehicleToActiveList( husk );
    if( res == -1 )
    {
        // not supposed to happen since the list can't be full!!!
        // we TOOK something out of the list before adding something in
        // If this assert happens, it is both fatal and strange
        rAssert( false );
        return;
    }
    //tv->mActiveListIndex = res;
    GetVehicleCentral()->SetVehicleController( res, husk->mTrafficLocomotion->GetAI() );


    husk->AddRef();
    husk->SetInitialPosition( &initPos );
    float angle = GetRotationAboutY( initDir.x, initDir.z );
    husk->SetResetFacingInRadians( angle );
    husk->Reset();
    husk->SetLocomotion( VL_PHYSICS );

    /*
    // tell the avatar that it's now in a husk
    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    avatar->SetVehicle(husk);
    */

    tv->SetHusk( husk );

    // tell the TrafficVehicle that it is destroyed
    tv->SetHasHusk( true );

}

void TrafficManager::SetMaxTraffic( int n )
{
    if( n < 0 )
    {
        n = 0;
    }
    else if( n > MAX_TRAFFIC )
    {
        n = MAX_TRAFFIC;
    }

    mMaxTraffic = n;
}

int TrafficManager::GetMaxTraffic()
{
    return mMaxTraffic;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

TrafficManager::TrafficManager()
{
    // start listening for events
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_REPAIR_CAR );
    GetEventManager()->AddListener( this, EVENT_PLAYER_VEHICLE_HORN );

    mVehicles = new TrafficVehicle[ MAX_TRAFFIC ];

    mCurrTrafficModelGroup = -1;
    mMaxTraffic = MAX_TRAFFIC;

    // initialize NULL list.
    mNumCharsToStopFor = 0;
    for( int i=0; i<MAX_CHARS_TO_STOP_FOR; i++ )
    {
        mCharactersToStopFor[i] = NULL;
    }

    mTrafficEnabled = !DISABLETRAFFIC;
    if( CommandLineOptions::Get(CLO_NO_TRAFFIC) )
    {
        mTrafficEnabled = false;
    }

    mQueuedTrafficHorns.Allocate( MAX_QUEUED_TRAFFIC_HORNS );

}
TrafficManager::~TrafficManager()
{
    GetEventManager()->RemoveAll( this );
    Cleanup();

    mQueuedTrafficHorns.Clear();
    delete[] mVehicles;
}

Vehicle* TrafficManager::InitRandomVehicle()
{
    rAssert( mCurrTrafficModelGroup >= 0 );

    Vehicle* newV = NULL;

    // randomly choose a model index
    int numModels = mTrafficModelGroups[mCurrTrafficModelGroup].GetNumModels();
    int modelIndex = rand() % numModels;

    TrafficModel* tm = NULL;

    // if there are already too many instances of traffic cars under this model
    // we go to the next model... and so on until we're out of models.
    int count = 0;
    while( count < numModels )
    {
        tm = mTrafficModelGroups[mCurrTrafficModelGroup].GetTrafficModel( modelIndex );
        if( tm->mNumInstances >= tm->mMaxInstances )
        {
            modelIndex = (modelIndex+1) % numModels;
            count++;
            continue;
        }
        else
        {
            break;
        }
    }

    if( count >= numModels )
    {
        char message[256];
        sprintf( message, "DOH! Can't initialize a traffic car \"%s\" because\n"
            "we already have %d of max %d instances allowed.\n"
            "See leveli.mfk to increase the max allowed for this model\n",
            tm->mModelName, tm->mNumInstances, tm->mMaxInstances );

        rTuneAssertMsg( false, message );
        return NULL;
    }

    newV = ::GetVehicleCentral()->InitVehicle( tm->mModelName, false, 0, VT_TRAFFIC );
    rTuneAssert( newV != NULL );
    tm->mNumInstances++;

    return newV;
}

void TrafficManager::Cleanup()
{
    // Clean up list of characters to stop for...
    //
    for( int i=0; i<MAX_CHARS_TO_STOP_FOR; i++ )
    {
        if( mCharactersToStopFor[i] != NULL )
        {
            mCharactersToStopFor[i]->Release();
            mCharactersToStopFor[i] = NULL;
            mNumCharsToStopFor--;
        }
    }
    rAssert( mNumCharsToStopFor == 0 );

    // Clean up swap array
    //
    mQueuedTrafficHorns.ClearUse();

    // Clean up traffic vehicles
    //
    for( int i=0 ; i<MAX_TRAFFIC ; i++ )
    {
        TrafficVehicle* tv = &mVehicles[i];
        rAssert( tv != NULL );

        Vehicle* v = tv->GetVehicle();
        if( tv->GetIsActive() )
        {
            // tell trafficlocomotion to be inactive,
            // clear out traffic vehicle from a lane,
            // reset trafficvehicle members (except vehicle pointer member) to indeterminate values,
            // decrement numtraffic count by 1,
            // remove from activelist
            RemoveTraffic( i ); 
        }

        rAssert( tv->GetHusk() == NULL );

        if( v != NULL )
        {
            v->ReleaseVerified();
            tv->SetVehicle( NULL );
        }
    }

    // clean up intersections
    for( int j=0; j<MAX_INTERSECTIONS; j++ )
    {
        mpIntersections[j] = NULL;
    }

#ifdef DEBUGWATCH
    radDbgWatchDelete( &mDesiredTrafficSpeedKph );
#endif

}

bool TrafficManager::AddTraffic(Lane* lane, TrafficVehicle* tv)
{
    rAssert( tv != NULL );
    rAssert( tv->GetIsActive() == false );
    rAssert( tv->GetLane() == NULL );

    Vehicle* v = tv->GetVehicle();
    rAssert( v != NULL );
    rAssert( v->mTrafficLocomotion->GetActive() == false );

    // add vehicle to the lane
    // if there was some error in adding (say if list was full)
    // then just abort
    if( lane->mTrafficVehicles.mUseSize >= lane->mTrafficVehicles.mSize )
    {
        return false;
    }

    // add vehicle to ActiveList
    if( ::GetVehicleCentral()->ActiveVehicleListIsFull() )
    {
        return false;
    }

    v->mVehicleType = VT_TRAFFIC;

    int res = ::GetVehicleCentral()->AddVehicleToActiveList( v );
    if( res == -1 )
    {
        // not supposed to happen since we already eliminated the
        // safe failure condition (activelistisfull)
        rAssert( false );
        return false;
    }

    ///////  NO MORE BAILING OUT AFTER THIS POINT  /////////

    lane->mTrafficVehicles.Add( tv );

    // now add the AI to active vehicle controller list
    GetVehicleCentral()->SetVehicleController( res, v->mTrafficLocomotion->GetAI() );

    // set trafficVehicle fields
    tv->SetLane( lane );
    tv->SetIsActive( true );
    tv->SetHasHusk( false );
    //tv->mActiveListIndex = res;
    tv->mMillisecondsDeactivated = 0;
    tv->mMillisecondsOutOfSight = 0;
    tv->mOutOfSight = true;

    ::GetEventManager()->TriggerEvent( EVENT_TRAFFIC_SPAWN, v );

    mNumTraffic++;
    return true;
}

void TrafficManager::RemoveTraffic( int vIndex )
{
    int i = vIndex;
    TrafficVehicle* tv = &mVehicles[i];
    RemoveTrafficVehicle( tv );
}

void TrafficManager::RemoveTrafficVehicle( TrafficVehicle* tv )
{
    rAssert( tv != NULL );
    rAssert( tv->GetIsActive() == true );

    Vehicle* v = tv->GetVehicle();
    rAssert( v != NULL );
    v->mTrafficLocomotion->SetActive( false );

    // Remove the vehicle from the lane, if it hasn't been removed
    // already (if the car was Deactivated, it will have already been
    // removed from the lane.. the lane pointer will not be NULL cuz 
    // it's needed later for resurrection, but the traffic index will
    // be -1.)
    Lane* lane = tv->GetLane();
    if( lane != NULL )
    {
        for( int i=0; i<lane->mTrafficVehicles.mUseSize; i++ )
        {
            if( tv == lane->mTrafficVehicles[i] )
            {
                lane->mTrafficVehicles.Remove(i);
                break;
            }
        }
    }

    // remove Vehicle (or its husk if it's destroyed) from ActiveList
    if( tv->HasHusk() )
    {
        Vehicle* husk = tv->GetHusk();
        rAssert( husk );

        bool succeeded = ::GetVehicleCentral()->RemoveVehicleFromActiveList( husk );
        rAssert( succeeded );

        ::GetVehicleCentral()->mHuskPool.FreeHusk( husk );
        // restore fade alpha if we set it, so other vehicles don't get confused
        husk->mGeometryVehicle->SetFadeAlpha( 255 ); 
        husk->Release(); // don't verify destruction cuz huskpool has final ownership
        tv->SetHusk( NULL );
    }
    else
    {
        Vehicle* v = tv->GetVehicle();
        rAssert( v );
        rAssert( tv->GetHusk() == NULL );

        GetVehicleCentral()->RemoveVehicleFromActiveList( v );
        GetEventManager()->TriggerEvent( EVENT_TRAFFIC_REMOVE, v );
    }

    // remove from our traffic system
    //GetVehicleCentral()->SetVehicleController( tv->mActiveListIndex, NULL );
    tv->SetLane( NULL );
    tv->SetIsActive( false );
    tv->SetHasHusk( false );
    //tv->mActiveListIndex = -1;
    tv->mMillisecondsDeactivated = 0;
    tv->mMillisecondsOutOfSight = 0;
    tv->mOutOfSight = true;

    mNumTraffic--;

    // search through queued traffic horns list and remove self...
    Vehicle* traffVehicle = tv->GetVehicle();
    for( int i=0; i<mQueuedTrafficHorns.mUseSize; i++ )
    {
        if( mQueuedTrafficHorns[i].vehicle == traffVehicle )
        {
            mQueuedTrafficHorns.Remove( i );
            break;
        }
    }
}

TrafficVehicle* TrafficManager::GetFreeTrafficVehicle()
{
    // gotta start at some random index, so we don't recycle
    // the same car (and thus same car model) over and over again
    // when maxtraffic is set to a small number

    int randIndex = rand() % MAX_TRAFFIC;
    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        if( !mVehicles[randIndex].GetIsActive() )
        {
            return &mVehicles[randIndex];
        }
        randIndex = (randIndex + 1) % MAX_TRAFFIC;
    }
    return NULL;
}

float TrafficManager::DetermineDesiredSpeedKph()
{
    float speedKph = 0.0f;
    switch( GetGameplayManager()->GetCurrentLevelIndex() )
    {
    case RenderEnums::L1:
        speedKph = 60.0f;
        break;
    case RenderEnums::L2:
        speedKph = 60.0f;
        break;
    case RenderEnums::L3:
        speedKph = 60.0f;
        break;
    case RenderEnums::L4:
        speedKph = 60.0f;
        break;
    case RenderEnums::L5:
        speedKph = 60.0f;
        break;
    case RenderEnums::L6:
        speedKph = 60.0f;
        break;
    case RenderEnums::L7:
        speedKph = 60.0f;
        break;
    default:
        rAssert( false );
        break;
    }
    return speedKph;
}

TrafficVehicle* TrafficManager::FindTrafficVehicle( Vehicle* vehicle )
{
    for( int i=0; i<MAX_TRAFFIC; ++i )
    {
        TrafficVehicle* tv = &mVehicles[i];
        rAssert( tv != NULL );

        Vehicle* v = tv->GetVehicle();
        if( vehicle == v )
        {
            return tv;
        }
    }
    return NULL;
}

bool TrafficManager::IsVehicleTrafficVehicle( Vehicle* vehicle )
{
    return FindTrafficVehicle( vehicle ) != NULL;
}


Vehicle* TrafficManager::InitRandomHusk( Vehicle* v )
{
    Vehicle* husk = ::GetVehicleCentral()->mHuskPool.RequestHusk( VT_TRAFFIC, v );
    return husk;
}

void TrafficManager::UpdateIntersection( unsigned int milliseconds, Vehicle* v, int& nIntersectionsUpdated )
{
    rAssert( v->GetLocomotionType() == VL_TRAFFIC );
    rAssert( v != NULL );

    Intersection* intersection = NULL;
    if( !v->mTrafficLocomotion->IsInIntersection() )
    {
        intersection = (Intersection*) v->mTrafficLocomotion->
            GetAILane()->GetRoad()->GetDestinationIntersection();
    }
    else 
    {
        intersection = (Intersection*) v->mTrafficLocomotion->
            GetAILane()->GetRoad()->GetSourceIntersection();
    }

    // update the intersection if it hasn't already been updated this frame
    bool foundIntersection = false;
    for( int i=0; i<nIntersectionsUpdated; i++ )
    {
        if( mpIntersections[i] == intersection )
        {
            foundIntersection = true;
            break;
        }
    }
    if( !foundIntersection )
    {
        if( nIntersectionsUpdated < MAX_INTERSECTIONS )
        {
            mpIntersections[nIntersectionsUpdated] = intersection;
            nIntersectionsUpdated++;
            intersection->Update(milliseconds);
        }
    }
}

void TrafficManager::SetVehicleFadeAlpha( Vehicle* v, const rmt::Vector& pPos )
{
    rAssert( v );

    rmt::Vector vPos;
    v->GetPosition( &vPos );
    float distFromPlayer = (pPos - vPos).Length(); // *** SQUARE ROOT! ***

    float fadeMinLimit = TrafficManager::FADE_RADIUS;
    float fadeMaxLimit = TrafficManager::ADD_RADIUS + TrafficManager::CENTER_OFFSET;
    int fadeAlpha = 255;
    if( fadeMinLimit <= distFromPlayer && distFromPlayer <= fadeMaxLimit )
    {
        // if we're in the fading zone, gotta change fade alpha
        float fadeRatio = (distFromPlayer - fadeMinLimit)/(fadeMaxLimit - fadeMinLimit);
        fadeAlpha = (int) (255.0f * (1.0f - fadeRatio));
    }
    else if( distFromPlayer > fadeMaxLimit )
    {
        fadeAlpha = 0;
    }

    v->mGeometryVehicle->SetFadeAlpha( fadeAlpha );
}

bool TrafficManager::AttemptResurrection( TrafficVehicle* tv )
{
    rAssert( tv );
    rAssert( tv->GetIsActive() );

    Vehicle* v = tv->GetVehicle();
    rAssert( v );
    rAssert( v->mTrafficLocomotion );
    rAssert( v->mTrafficLocomotion->GetActive() == false );

    // if has been hijacked by user, don't resurrect
    if( v->mHijackedByUser )
    {
        tv->mCanBeResurrected = false;
        return false;
    }

    // if we have determined that it can't be resurrect, no point trying
    if( !tv->mCanBeResurrected )
    {
        tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
        return false;
    }

    // don't allow resurrection right away... cool down... cool down... cool down...
    if( tv->mMillisecondsDeactivated < MILLISECONDS_STUNNED_AFTER_DEACTIVATED )
    {
        tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
        return false;
    }

    if( v->mVehicleDestroyed )
    {
        // reset it to 0 so we don't check again every frame
        tv->mCanBeResurrected = false;
        tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
        return false;
    }

    // Check through vehicles VL_PHYSICS to see if we can bring any of them 
    // back to life:
    // - check health
    // - check distance from last segment
    // - check if we've been deactivated long enough

    // if not at rest yet, don't resurrect...
    if( !v->IsAtRest() )
    {
        tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
        return false;
    }

    // If the lane won't support us anymore, wait for awhile
    Lane* oldLane = tv->GetLane();
    rAssert( oldLane != NULL );
    if( oldLane->mTrafficVehicles.mUseSize >= oldLane->GetDensity() )
    {
        // reset it to 0 so there's a lull before we check again..
        tv->mMillisecondsDeactivated = 0; 
        tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
        return false;
    }

    //
    // check if there's anything ahead ...
    //
    TrafficAI::ObstacleType foundSOMETHING = TrafficAI::OT_NOTHING;
    float distFromSOMETHINGSqr = 100000.0f;
    void* SOMETHING = NULL;
    bool SOMETHINGOnMyRight = false;

    v->mTrafficLocomotion->GetAI()->CheckForObstacles(
        foundSOMETHING, distFromSOMETHINGSqr, SOMETHING, SOMETHINGOnMyRight );

    if( foundSOMETHING != TrafficAI::OT_NOTHING )
    {
        // reset it to 0 so there's a lull before we check again..
        tv->mMillisecondsDeactivated = 0; 
        tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
        return false;
    }



    rmt::Vector vPos, vDir, vUp;
    v->GetPosition( &vPos );

    vDir = v->GetFacing();
    rAssert( rmt::Epsilon( vDir.MagnitudeSqr(), 1.0f, 0.001f ) );

    v->GetVUP( &vUp );
    rAssert( rmt::Epsilon( vUp.MagnitudeSqr(), 1.0f, 0.001f ) );

    // ACTION:
    // - if we were in an intersection, just build spline straight to the 
    //   target segment (no problem!). Remain in DRIVING state.
    // - else 
    //   - Make sure we're not too far from closest point on last segment
    //   - Make sure the line from us to closest point on last segment does not
    //     cross over the segment (meaning that we were outside the segment... 
    //     e.g. on the sidewalk... in which case we don't recover)
    //   - Build spline to a position 5 meters ahead of the closestPt on segment
    //     flowing onto a new segment as necessary... do not flow over into an 
    //     intersection. Store out segment's t value and transit to RECOVERING
    //     state. When done RECOVERING state, add t value to the t. 

    TrafficAI* ai = v->mTrafficLocomotion->GetAI();
    rAssert( ai );

    RoadSegment* seg = ai->GetSegment();
    rAssert( seg );

    int laneIndex = (int)(ai->GetLaneIndex());

    const float TOLERANCE_DIST_SQR = 64.0f;
    const float UP_COSALPHA = 0.9848077f;
    const float FACING_RESURRECT_COSAPLHA = 0.0f;

    rmt::Vector targetPos, targetDir;

    if( v->mTrafficLocomotion->IsInIntersection() ||
        ai->GetState() == TrafficAI::LANE_CHANGING )
    {
        // TODO: 
        // The assumption that the intersection's UP vector is simply 0,1,0 is
        // no longer valid since we don't enforce the intersection to be completely
        // horizontal anymore. So... we have to determine the up vector ourselves...
        // *shudder*....

        // If vehicle's up vector isn't anywhere close to the horizontal up vector
        // (0,1,0), don't resurrect (it might be tipped over or laying on its side)
        rmt::Vector testUp( 0.0f, 1.0f, 0.0f );
        if( vUp.Dot( testUp ) < UP_COSALPHA )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }

        // Just build a spline to the end of the intersection, if we're 
        // close enough to the original intersection spline 
        rmt::Vector* ways;
        int nPts, currPt;
        v->mTrafficLocomotion->GetSplineCurve( ways, nPts, currPt );

        rAssert( ways ); // we should have mWays populated since we're in the intersection

        rmt::Vector tmpStart, tmpEnd, tmpClosestPt;
        tmpStart = ways[0];
        tmpEnd = ways[nPts-1];
        FindClosestPointOnLine( tmpStart, tmpEnd, vPos, tmpClosestPt );
        
        const float CLOSE_ENOUGH_TO_ORIG_SPLINE_SQR = 16.0f;
        float distSqr = (vPos - tmpClosestPt).MagnitudeSqr();
        if( distSqr > CLOSE_ENOUGH_TO_ORIG_SPLINE_SQR )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }

        // now it's time to get the target of our new spline...
        seg->GetLaneLocation( 0.0f, laneIndex, targetPos, targetDir );

        // if our pos is too close to the end, don't do it;
        // it will look too strange...
        const float US_FAR_ENOUGH_FROM_TARGET_POS_SQR = 25.0f;
        distSqr = (vPos - targetPos).MagnitudeSqr();
        if( distSqr < US_FAR_ENOUGH_FROM_TARGET_POS_SQR )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }

        // if the closest point on original spline is too close to
        // the end, don't do it; it will look too strange...
        const float PROJ_FAR_ENOUGH_FROM_TARGET_POS_SQR = 25.0f;
        distSqr = (tmpClosestPt - targetPos).MagnitudeSqr();
        if( distSqr < PROJ_FAR_ENOUGH_FROM_TARGET_POS_SQR )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }

        if( !rmt::Epsilon( targetDir.MagnitudeSqr(), 1.0f, 0.001f ) )
        {
            targetDir.NormalizeSafe(); // *** SQUARE ROOT! ***
        }
        rAssert( rmt::Epsilon( targetDir.MagnitudeSqr(), 1.0f, 0.001f ) );

        // If vehicle's facing is at too great an angle from target dir, 
        // and our lateral distance is not that far, don't do it...
        // (it will look too strange)
        if( targetDir.Dot( vDir ) < FACING_RESURRECT_COSAPLHA )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }

        ////////////////////// OK, NO ABORTING NOW //////////////////////
        if( ai->GetState() == TrafficAI::LANE_CHANGING )
        {
            ai->SetState( TrafficAI::SPLINING );
        }
    }
    else
    {
        // Here we're neither lane-changing nor in an intersection

        // Find the closest seg along road (rather than use the last seg we were on)
        const Road* road = seg->GetRoad();
        rAssert( road );

        float closestDistSqr = TOLERANCE_DIST_SQR;
        RoadSegment* closestSeg = NULL;
        rmt::Vector closestPt, start, end, closestPtOnSeg;

        unsigned int numSegs = road->GetNumRoadSegments();
        for( unsigned int i=0; i<numSegs; i++ )
        {
            RoadSegment* aSeg = road->GetRoadSegment( i );
            rAssert( aSeg );

            rmt::Vector tmpStart, tmpEnd, tmpDir;
            aSeg->GetLaneLocation( 0.0f, laneIndex, tmpStart, tmpDir );
            aSeg->GetLaneLocation( 1.0f, laneIndex, tmpEnd, tmpDir );

            FindClosestPointOnLine( tmpStart, tmpEnd, vPos, closestPtOnSeg );

            float distSqr = (vPos - closestPtOnSeg).MagnitudeSqr();
            if( distSqr < closestDistSqr )
            {
                closestDistSqr = distSqr;
                closestSeg = aSeg;
                closestPt = closestPtOnSeg;
                start = tmpStart;
                end = tmpEnd;
            }
        }
        if( closestSeg == NULL )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }

        seg = closestSeg;

        // If vehicle's up vector isn't anywhere close to the segment's up vector,
        // don't resurrect (it might be tipped over or laying on its side)
        rmt::Vector testUp;
        seg->GetSegmentNormal( testUp );
        rAssert( rmt::Epsilon( testUp.MagnitudeSqr(), 1.0f, 0.001f ) );
        if( vUp.Dot( testUp ) < UP_COSALPHA )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }


        // determine starting t
        float startT = GetLineSegmentT( start, end, closestPt ); 
        rAssert( 0.0f <= startT && startT <= 1.0f );

        // TODO:
        // Maybe make sure line from us to closestPt doesn't cross over a segment bound
        // (test against line segments---v0,v1 and v2,v3---to make sure that 
        // closestPt and vPos are both on the left of v2,v3 and right of v0,v1)

        // find a new lane position, 5 meters ahead...
        unsigned int segmentIndex = seg->GetSegmentIndex();
        float pathLength = seg->GetLaneLength( laneIndex );
        float t = startT;
        float distAhead = 10.0f;
        t += distAhead / pathLength;
        while( t > 1.0f )
        {
            t -= 1.0f;

            if( segmentIndex < (numSegs-1) )
            {
                // move ahead a segment
                segmentIndex++;
                seg = road->GetRoadSegment( segmentIndex ); 
                float newLength = seg->GetLaneLength( laneIndex );
                t *= pathLength / newLength;
                pathLength = newLength;
            }
            else // if we're out of segments.. we are at an intersection... 
            {
                // TODO:
                // Aborting here keeps us from being able to resurrect properly if 
                // we're closestDistSqr meters or less from the intersection (on approach).
                // We need to be able to deal effectively with this case..
                tv->mCanBeResurrected = false;
                tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
                return false;
            }
        }

        rAssert( 0.0f <= t && t <= 1.0f );

        seg->GetLaneLocation( t, laneIndex, targetPos, targetDir );
        if( !rmt::Epsilon( targetDir.MagnitudeSqr(), 1.0f, 0.001f ) )
        {
            targetDir.NormalizeSafe(); // *** SQUARE ROOT! ***
        }
        rAssert( rmt::Epsilon( targetDir.MagnitudeSqr(), 1.0f, 0.001f ) );

        // If vehicle's facing is at too great an angle from target dir, 
        // and our lateral distance is not that far, don't do it...
        // (it will look too strange)
        if( targetDir.Dot( vDir ) < FACING_RESURRECT_COSAPLHA )
        {
            tv->mCanBeResurrected = false;
            tv->GetVehicle()->GetSimState()->SetControl( sim::simSimulationCtrl );
            return false;
        }

        //////////////////////////// OK NO ABORTING NOW ////////////////////////
        // Update TrafficAI to look at the new segment, segIndex, and outT
        ai->SetSegmentIndex( segmentIndex ); 
        ai->SetState( TrafficAI::SPLINING );

        v->mTrafficLocomotion->mOutLaneT = t;
    }

    float restHeightAboveGround = v->GetRestHeightAboveGround();

    /////////////////////////////////////////////
    // Adjust ground height (unfortunately we need to do this
    // because we use pos averaging (so our y value is off)
    rmt::Vector groundPosition, outnorm;
    bool bFoundPlane = false;

    groundPosition = vPos;
    outnorm.Set( 0.0f, 1.0f, 0.0f );

    GetIntersectManager()->FindIntersection(
        groundPosition,   // IN
        bFoundPlane,      // OUT
        outnorm,          // OUT
        groundPosition    // OUT
        );  

    if( bFoundPlane )
    {
        vPos.y = groundPosition.y + restHeightAboveGround;
    }

    ///////////////////////////////////
    // build spline 
    vPos.y -= restHeightAboveGround;
    //vDir.y = 0.0f;
    if( !rmt::Epsilon( vDir.MagnitudeSqr(), 1.0f, 0.001f ) )
    {
        vDir.NormalizeSafe(); // *** SQUARE ROOT! ***
    }
    rAssert( rmt::Epsilon( vDir.MagnitudeSqr(), 1.0f, 0.001f ) );

    v->mTrafficLocomotion->BuildArbitraryCurve( vPos, vDir, targetPos, targetDir );


    // bring vehicle back into traffic: 
    // - set loco back to VL_TRAFFIC
    // - put it back in the same lane, on the closest point on the closestSeg
    // - TrafficLomotion::SetIsActive(true)

    /* NOTE: 
       Don't need to do this sheeyatsu... the vehicle is already where 
       we want it to be and the simstate is up to date.
    v->SetInitialPosition( &vPos );
    float angle = GetRotationAboutY( vDir.x, vDir.z );
    v->SetResetFacingInRadians( angle );
    v->Reset();
    */

    // Just a test to see if it's ever anything else... if it is, this could be
    // a problem.. Please notify Dusit.
    rAssert( v->mVehicleType == VT_TRAFFIC );

    v->mVehicleType = VT_TRAFFIC;
    v->SetLocomotion( VL_TRAFFIC );
    v->mTrafficLocomotion->SetActive( true );
    v->mTrafficLocomotion->SetAISpeed( 0.0f );
    v->mTrafficLocomotion->InitPos( vPos );
    v->mTrafficLocomotion->InitFacing( vDir );

    // Readd the vehicle to the lane
    oldLane->mTrafficVehicles.Add( tv );

    return true;
}


