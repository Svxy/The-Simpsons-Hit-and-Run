//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        chasemanager.cpp
//
// Description: ChaseManager Class Implementation
//
// History:     11/5/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#include <worldsim/harass/chasemanager.h>
#include <stdlib.h>

#include <worldsim/vehiclecentral.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
/*
#include <camera/supercammanager.h>
#include <camera/supercam.h>
*/
#include <ai/vehicle/chaseai.h>
#include <roads/roadsegment.h>
#include <roads/geometry.h>
#include <render/Culling/ReserveArray.h>
#include <render/IntersectManager/IntersectManager.h>
#include <events/EventManager.h>

#include <mission/gameplaymanager.h>

////////////////////////////////////
// Initialize Statics
//ChaseManager* spChaseManager = NULL;
static const float HARASS_BEELINE_DIST = 60.0f;

/////////////////////////////////////

// Constructors/Destructors
ChaseManager::ChaseManager() 
:
mNumRegisteredModels( 0 ),
mTotalSpawnFrequencies( 0 ),
mMaxVehicles( MAX_CHASE_VEHICLES )
{
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    //GetEventManager()->AddListener( this, EVENT_REPAIR_CAR ); // Chase cars won't be repairing.

    for( int i=0; i<MAX_MODELS; i++ )
    {
        mModels[i].spawnFreq = 0;
    }

    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        mVehicles[i].v = NULL;
        mVehicles[i].husk = NULL;
        mVehicles[i].vAI = NULL;
        mVehicles[i].isActive = false;
        //mVehicles[i].activeListIndex = -1;
        mVehicles[i].isOutOfSight = true;
        mVehicles[i].markedForDeletion = false;
        mVehicles[i].secondsOutOfSight = 0.0f;
    }

    // Gotta set stuff belonging to superclass to prevent a check in superclass from failing
    mSpawnRadius = CHASE_SPAWN_RADIUS;
    mRemoveRadius = CHASE_REMOVE_RADIUS;
}

ChaseManager::~ChaseManager()
{
    GetEventManager()->RemoveAll( this );
    DeactivateAllVehicles();
}

bool ChaseManager::IsChaseVehicle( Vehicle* v )
{
    ChaseVehicle* cv = FindChaseVehicle( v );
    return( cv != NULL );
}



ChaseManager::ChaseVehicle* ChaseManager::FindChaseVehicle( Vehicle* v )
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( v == mVehicles[i].v ) 
        {
            return &mVehicles[i];
        }
    }

    return NULL;
}


void ChaseManager::HandleEvent( EventEnum id, void* pEventData )
{
    if( id == EVENT_VEHICLE_DESTROYED )
    {
        Vehicle* v = (Vehicle*) pEventData;
        rAssert( v );

        ChaseVehicle* cv = FindChaseVehicle( v );
        if( cv == NULL )
        {
            return;
        }

        rAssert( cv->isActive );

        DeactivateVehicle( cv );
        /*** 
            WELL now they don't want husks for specifically for chase/harass cars
            and so we go round in circles.... Initially, they wanted ALL to have husks
            "for consistency"... Now, not so much.

        // obtain info from the vehicle
        rmt::Vector initPos, initDir;
        v->GetPosition( &initPos );
        initDir = v->GetFacing();
        rAssert( rmt::Epsilon( initDir.MagnitudeSqr(), 1.0f, 0.001f ) );

        // Remove original from VehicleCentral's ActiveList
        VehicleCentral* vc = ::GetVehicleCentral();
        rAssert( vc != NULL );

        vc->RemoveVehicleFromActiveList( cv->v );
        GetEventManager()->TriggerEvent( EVENT_CHASE_VEHICLE_DESTROYED, cv->v );

        // Update chase AI
        vc->SetVehicleController( cv->activeListIndex, NULL );
        cv->activeListIndex = -1;
        cv->vAI->SetActive( false );
        cv->vAI->Finalize();

        // 
        // Now we grab husk and put it in place of the original vehicle 
        // 
        Vehicle* husk = GetVehicleCentral()->mHuskPool.RequestHusk( VT_AI, v );
        if( husk == NULL )
        {
            return;
        }
        int res = GetVehicleCentral()->AddVehicleToActiveList( husk );
        if( res == -1 )
        {
            // not supposed to happen since the list can't be full!!!
            // we TOOK something out of the list before adding something in
            // If this assert happens, it is both fatal and strange
            rAssert( false );
            return;
        }
        cv->activeListIndex = res;

        husk->AddRef();
        husk->SetInitialPosition( &initPos );
        float angle = GetRotationAboutY( initDir.x, initDir.z );
        husk->SetResetFacingInRadians( angle );
        husk->Reset();
        husk->SetLocomotion( VL_PHYSICS );

        cv->husk = husk;
        */
    }
}



void ChaseManager::Init()
{
    SpawnManager::Init();

    ////////////////////////////////////////////////////////////////
    // CLEAN UP PREVIOUS RUN
    //
    DeactivateAllVehicles();

    ////////////////////////////////////////////////////////////////
    // INITIALIZE FOR NEW RUN
    //

    int count = 0;

    // create a temp string array of exactly this many elems or "buckets"
    char** names = new char* [mTotalSpawnFrequencies];
    for( int i=0; i<MAX_MODELS; i++ )
    {
        for( int j=count; j<(count+mModels[i].spawnFreq); j++ )
        {
            names[j] = new char[160];
            strcpy( names[j], mModels[i].name );
        }
        count += mModels[i].spawnFreq;
    }
    rAssert( count == mTotalSpawnFrequencies );

    // pick a bucket random and spawn a car by that name
    VehicleCentral* vc = ::GetVehicleCentral();
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        int coinflip = rand() % mTotalSpawnFrequencies;


   
        Vehicle* v = vc->InitVehicle( names[coinflip], false, mConfileName, VT_AI,
                                      VehicleCentral::ALLOW_DRIVER,
                                      false,    // not a playercar
                                      false);    // start with in car bv representation immediately
        rAssert( v != NULL );

        mVehicles[i].v = v; 
        mVehicles[i].husk = NULL;
        mVehicles[i].v->AddRef();
        mVehicles[i].v->SetLocomotion( VL_PHYSICS );
        mVehicles[i].vAI = new (GMA_LEVEL_OTHER) ChaseAI( v, HARASS_BEELINE_DIST ); 
        mVehicles[i].vAI->AddRef(); // Corresponding call to Release() will already call delete if refcount<=1
        mVehicles[i].isActive = false;
        mVehicles[i].isOutOfSight = true;
        mVehicles[i].markedForDeletion = false;
        mVehicles[i].secondsOutOfSight = 0.0f;
    }

    // clean up our temporary name buckets...
    for( int i=0; i<mTotalSpawnFrequencies; i++ )
    {
        delete[] names[i];
    }
    delete[] names;
}



void ChaseManager::ClearAllObjects()
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            DeactivateVehicle( &mVehicles[i] );
        }
    }
}

int ChaseManager::GetNumActiveVehicles()
{
    int numActive = 0;
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            numActive++;
        }
    }
    return numActive;
}

void ChaseManager::ClearOutOfSightVehicles()
{
    static const float SECONDS_OUT_OF_SIGHT_BEFORE_REMOVAL = 5.0f;

    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive &&
            mVehicles[i].isOutOfSight &&
            ( mVehicles[i].husk != NULL || mVehicles[i].markedForDeletion == true ) &&
            mVehicles[i].secondsOutOfSight > SECONDS_OUT_OF_SIGHT_BEFORE_REMOVAL )
        {
            DeactivateVehicle( &mVehicles[i] );
        }
    }
}

void ChaseManager::ClearObjectsInsideRadius( rmt::Vector center, float radius )
{
    int nObjectsRemoved = 0;
    float minDistSqr = radius * radius;

    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            Vehicle* v = mVehicles[i].v;
            rAssert( v != NULL );

            rmt::Vector vPos;
            v->GetPosition( &vPos );

            rmt::Vector toSphere = center - vPos;

            if( toSphere.MagnitudeSqr() <= minDistSqr )
            {
                DeactivateVehicle( &mVehicles[i] );
            }
        }
    }
}


void ChaseManager::ClearObjectsOutsideRadius( rmt::Vector center, float radius )
{
    float minDistSqr = radius * radius;

    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            Vehicle* v = mVehicles[i].v;
            rAssert( v != NULL );

            rmt::Vector vPos;
            v->GetPosition( &vPos );

            rmt::Vector toSphere = center - vPos;

            if( toSphere.MagnitudeSqr() > minDistSqr )
            {
                DeactivateVehicle( &mVehicles[i] );
            }
        }
    }
}

bool ChaseManager::RegisterModel( const char* name, int spawnFreq )
{
    rAssert( name != NULL );
    rAssert( spawnFreq > 0 );

    // search existing SPARSE list for name
    int freeIndex = -1;
    for( int i=0; i<MAX_MODELS; i++ )
    {
        // use spawnFreq = 0 to mark empty spots
        if( mModels[i].spawnFreq == 0 )
        {
            freeIndex = i;
            continue;
        }

        // found an existing model registered under same name, so overwrite w/ new values
        if( strcmp(mModels[i].name, name)==0 )
        {
            mTotalSpawnFrequencies += spawnFreq - mModels[i].spawnFreq;
            mModels[i].spawnFreq = spawnFreq;
            return true;
        }
    }
    
    if( 0 <= freeIndex && freeIndex < MAX_MODELS )
    {
        int len = strlen( name );
        rAssert( len <= MAX_STRING_LEN );
        strncpy( mModels[freeIndex].name, name, MAX_STRING_LEN );
        mModels[freeIndex].name[MAX_STRING_LEN] = '\0';

        mModels[freeIndex].spawnFreq = spawnFreq;
        mTotalSpawnFrequencies += spawnFreq;
        mNumRegisteredModels++;
        return true;
    }
    return false;
}

bool ChaseManager::IsModelRegistered( const char* name )
{
    rAssert( name != NULL );

    // search existing SPARSE list for name
    for( int i=0; i<MAX_MODELS; i++ )
    {
        // found an existing model registered under this name
        if( strcmp(mModels[i].name, name)==0 )
        {
            return true;
        }
    }
    return false;
}

bool ChaseManager::UnregisterModel( const char* name )
{
    rAssert( name != NULL );

    // search existing SPARSE list for name
    for( int i=0; i<MAX_MODELS; i++ )
    {
        if( mModels[i].spawnFreq == 0 )
        {
            continue;
        }
        int nameLen = strlen( name );
        int modelNameLen = strlen( mModels[i].name );
        if( (nameLen == modelNameLen) && 
            (strncmp(mModels[i].name, name, nameLen)==0) )
        {
            mTotalSpawnFrequencies -= mModels[i].spawnFreq;
            mNumRegisteredModels--;
            mModels[i].spawnFreq = 0;
            return true;
        }
    }
    return false;
}

void ChaseManager::SetConfileName( const char* name )
{
    int len = strlen( name );
    rAssert( len <= MAX_STRING_LEN );
	strncpy( mConfileName, name, MAX_STRING_LEN );
	mConfileName[MAX_STRING_LEN] = '\0';
}


void ChaseManager::DisableAllActiveVehicleAIs()
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            //mVehicles[i].vAI->SetActive( false );
            mVehicles[i].vAI->EnterLimbo();
        }
    }
}

void ChaseManager::EnableAllActiveVehicleAIs()
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            //mVehicles[i].vAI->SetActive( true );
            mVehicles[i].vAI->ExitLimbo();
        }
    }
}

void ChaseManager::AddObjects( float seconds )
{
    rAssert( seconds >= 0.0f );

    // Get VehicleCentral. We'll need it lots.
    VehicleCentral* vc = ::GetVehicleCentral();
    rAssert( vc != NULL );

    // Get Player info.
    rmt::Vector playerPos, playerVel;

    /*
    SuperCam* superCam = ::GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
    rAssert( superCam != NULL );
    superCam->GetPosition( &playerPos );
    superCam->GetVelocity( &playerVel );
    */
    Avatar* avatar = ::GetAvatarManager()->GetAvatarForPlayer(0);
    rAssert( avatar != NULL );
    avatar->GetPosition( playerPos );
    avatar->GetVelocity(playerVel);


    float spawnRadius = GetSpawnRadius();


    // Do FindRoadElems to get the road segments that intersect our sphere.
    //   For each road segment's lane, tally a list of all intersection points (max of 2)
    //     For each intersection point,
    //       if mNumObjects < mMaxObjects,
    //         ActivateVehicle()
    //         Initialize vehicle

    ReserveArray<RoadSegment*> orList;
    ::GetIntersectManager()->FindRoadSegmentElems( playerPos, spawnRadius, orList );

    // This is the vehicle from our list of inactive chase vehicles that we'll spawn
    // Abort early if there's no more vehicle available to spawn...
    ChaseVehicle* cv = GetInactiveVehicle(); 
    if( cv == NULL )
    {
        return;
    }

    for( int i=0; i<orList.mUseSize; i++ )
    {
        RoadSegment* segment = orList.mpData[i];
        rAssert( segment != NULL );

        // loop through all the lanes for this segment
        for( unsigned int j=0; j<segment->GetNumLanes(); j++ )
        {
            // Find places where lane intersects with spawn radius (max 2 locations)
            rmt::Vector startPos, startDir, endPos, endDir; 
            segment->GetLaneLocation( 0.0f, j, startPos, startDir );
            segment->GetLaneLocation( 1.0f, j, endPos, endDir );
    
            rmt::Vector intPts[2];
            rmt::Sphere playerSphere( playerPos, spawnRadius );
            int numIntersections = IntersectLineSphere( startPos, endPos, playerSphere, intPts );
            if(numIntersections <= 0)
            {
                continue; // doesn't intersect our spawn sphere; skip this lane
            }

            // for each intersection point found, plant a vehicle
            for( int k=0; k<numIntersections; k++ )
            {
                if( mNumObjects < mMaxVehicles )
                {
                    rmt::Vector vPos = intPts[k];

                    if( cv == NULL )
                    {
                        return; // no more inactive vehicles available for spawning
                    }
                    Vehicle* v = cv->v; 
                    rAssert( v != NULL );

                    // 
                    // Detect if we're placing this car on top of another car
                    // by looping through active vehicles list. If there is car beneath us, 
                    // don't spawn here and try next point.
                    //
                    rmt::Sphere vSphere;
                    v->GetBoundingSphere( &vSphere );
                    vSphere.centre = vPos;
                    if( SpawningOnTopOfAnotherVehicle(vSphere) )
                    {
                        continue; // gonna spawn on a car; try next spawn point
                    }

                    //
                    // Check if in the next n seconds, vehicle will still be in 
                    // player's spawn zone (so we didn't add it in vain).
                    //

                    float timeToLiveInSeconds = 1.0f; //GetSecondsBetwAdds();

                    //
                    // Predict player pos after timeToLiveInSeconds seconds
                    //
                    rmt::Vector playerPos2 = playerPos + playerVel * timeToLiveInSeconds;

                    //
                    // Predict v pos after timeToLiveInSeconds seconds
                    //
                    rmt::Vector vDir = endPos - startPos;
                    vDir.Normalize(); // *** SQUARE ROOT! ***

                    float vAccel = v->mDesignerParams.mDpGasScale * 
                                   v->mSlipGasModifier * 
                                   1.0f; //v->mGas; 
                    
                    float vDistWillTravel = 0.5f * vAccel * timeToLiveInSeconds * timeToLiveInSeconds;

                    rmt::Vector vPos2 = vPos + vDir * vDistWillTravel;

                    //
                    // Make sure vPos2 still inside playerPos2's radius before we spawn it
                    //
                    float minDistSqr = spawnRadius * spawnRadius;
                    float distSqr = (vPos2 - playerPos2).MagnitudeSqr();
                    if( distSqr < minDistSqr )
                    {
                        // want the vehicle to spawn facing the player
                        rmt::Vector initFacing = playerPos - vPos; 

                        // initialize vehicle 
                        v->SetInitialPosition( &vPos );
                        float angle = GetRotationAboutY( initFacing.x, initFacing.z );
                        v->SetResetFacingInRadians( angle );
                        v->Reset();

                        bool succeeded = ActivateVehicle( cv );
                        if( !succeeded )
                        {
                            // vehiclecentral's activelist is full.. 
                            // no point trying to add anymore
                            return;
                        }

                        // just used the inactive vehicle; grab a new inactive vehicle
                        cv = GetInactiveVehicle();
                        
                    } // end-of-if vehicle's next pos lies in radius of player's next spawn sphere
                } // end-of-if mNumObjects < mMaxObjects
            } // end-of-loop through all intersection points on spawn radius for this lane
        } // end-of-loop through all lanes belonging to one segment
    }// end-of-loop through all segments returned by DSGFind
}

void ChaseManager::RemoveObjects( float seconds )
{
    // Get Player info

    rmt::Vector playerPos;
    /*
    SuperCam* superCam = ::GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
    rAssert( superCam != NULL );
    superCam->GetPosition( &playerPos );
    */
    Avatar* avatar = ::GetAvatarManager()->GetAvatarForPlayer(0);
    rAssert( avatar != NULL );
    avatar->GetPosition( playerPos );

    float radius = GetRemoveRadius();

    // Remove!
    ClearObjectsOutsideRadius( playerPos, radius );

    ClearOutOfSightVehicles();
}

void ChaseManager::UpdateObjects( float seconds )
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive ) 
        {
            if( mVehicles[i].v->mVehicleDestroyed && 
                mVehicles[i].vAI->GetState() != VehicleAI::STATE_LIMBO )
			{
				mVehicles[i].vAI->EnterLimbo();
				// NOTE: 
				// Never set mVehicles[i].isActive to false here because
				// it means something entirely different (it means that it's not
				// within your radius). Just set the AI's active flag to false
				//mVehicles[i].isActive = false;
			}

            if( mVehicles[i].husk || mVehicles[i].markedForDeletion )
            {
                Vehicle* vehicle = mVehicles[i].v;
                if(mVehicles[i].husk)
                {
                    vehicle = mVehicles[i].husk;
                }

                // Test for out of sight of player 0... If so, increment timer, if not reset timer
                rmt::Vector pos;

                vehicle->GetPosition( &pos );
                mVehicles[i].isOutOfSight = !GetGameplayManager()->TestPosInFrustrumOfPlayer( pos, 0 );

                mVehicles[i].secondsOutOfSight += seconds;
                if( !mVehicles[i].isOutOfSight )
                {
                    mVehicles[i].secondsOutOfSight = 0.0f;
                }
            }
        }
    }

    /*
    // VehicleCentral automatically updates all vehicles & their AI 
    // controllers if they're under Physics Locomotion (which they are)
    // so we need do nothing here.
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            mVehicles[i].v->Update( seconds );
        }
    }
    */
}

void ChaseManager::SuspendAllVehicles()
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].v != NULL && mVehicles[i].isActive)
        {
            mVehicles[i].v->CarDisplay( false );
            GetVehicleCentral()->RemoveVehicleFromActiveList( mVehicles[i].v );
        }
    }
}

void ChaseManager::ResumeAllVehicles()
{
    VehicleCentral* vc = ::GetVehicleCentral();
    rAssert( vc != NULL );

    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].v != NULL && mVehicles[i].isActive)
        {
            mVehicles[i].v->CarDisplay( true );
            // add to vehiclecentral's activelist
            if( vc->ActiveVehicleListIsFull() )
            {
                rAssert(false);
                return;
            }
            int res = vc->AddVehicleToActiveList( mVehicles[i].v );
            if( res == -1 )
            {
                // not supposed to happen since we already eliminated the
                // safe failure condition (activelistisfull).. So something else
                // went wrong...
                rAssert( false );
            }

            // update chase AI
            vc->SetVehicleController( res, mVehicles[i].vAI );
            mVehicles[i].vAI->Initialize();
            mVehicles[i].vAI->SetActive( true );
        }
    }
}

void ChaseManager::DeactivateVehicle( ChaseVehicle* cv )
{
    rAssert( cv->v != NULL );
    rAssert( cv->vAI != NULL );
    rAssert( cv->isActive );

    // Remove from VehicleCentral's ActiveList
    VehicleCentral* vc = ::GetVehicleCentral();
    rAssert( vc != NULL );

    if( cv->husk == NULL )
    {
        vc->RemoveVehicleFromActiveList( cv->v );
        GetEventManager()->TriggerEvent( EVENT_CHASE_VEHICLE_DESTROYED, cv->v );
    }
    else
    {
        bool succeeded = vc->RemoveVehicleFromActiveList( cv->husk );
        rAssert( succeeded );

        ::GetVehicleCentral()->mHuskPool.FreeHusk( cv->husk );
        cv->husk->Release(); // don't verify destruction cuz huskpool has final ownership
        cv->husk = NULL;
    }

    //if( cv->activeListIndex != -1 )
    //{
    //    vc->SetVehicleController( cv->activeListIndex, NULL );
    //}
    cv->vAI->SetActive( false );
    cv->vAI->Finalize();
    //cv->activeListIndex = -1;
    cv->isActive = false;
    cv->isOutOfSight = true;
    cv->markedForDeletion = false;
    cv->secondsOutOfSight = 0.0f;
    mNumObjects--;
}

bool ChaseManager::ActivateVehicle( ChaseVehicle* cv )
{
    rAssert( cv->v != NULL );
    rAssert( cv->vAI != NULL );
    //rAssert( cv->activeListIndex == -1 );
    rAssert( !cv->isActive );

    VehicleCentral* vc = ::GetVehicleCentral();
    rAssert( vc != NULL );

    // add to vehiclecentral's activelist
    if( vc->ActiveVehicleListIsFull() )
    {
        return false;
    }
    int res = vc->AddVehicleToActiveList( cv->v );
    if( res == -1 )
    {
        // not supposed to happen since we already eliminated the
        // safe failure condition (activelistisfull).. So something else
        // went wrong...
        rAssert( false );
    }

    // update chase AI
    vc->SetVehicleController( res, cv->vAI );
    cv->vAI->Initialize();
    cv->vAI->SetActive( true );

    // update our variables
    //cv->activeListIndex = res;
    cv->isActive = true;
    rAssert( cv->husk == NULL );
    cv->isOutOfSight = true;
    cv->markedForDeletion = false;
    cv->secondsOutOfSight = 0.0f;
    mNumObjects++;

    // tell sound
    GetEventManager()->TriggerEvent( EVENT_CHASE_VEHICLE_SPAWNED, cv->v );

    return true;
}

void ChaseManager::DeactivateAllVehicles()
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            DeactivateVehicle( &mVehicles[i] );
        }
        if( mVehicles[i].v != NULL )
        {
            mVehicles[i].v->Release();
            mVehicles[i].v = NULL;
        }
        if( mVehicles[i].vAI != NULL )
        {
            mVehicles[i].vAI->Release();
            mVehicles[i].vAI = NULL;
        }
    }
    rAssert( mNumObjects == 0 );
}

void ChaseManager::MarkAllVehiclesForDeletion()
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].v != NULL )
        {
            mVehicles[i].markedForDeletion = true;
        }
    }
}

ChaseManager::ChaseVehicle* ChaseManager::GetInactiveVehicle()
{
    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( !mVehicles[i].isActive )
        {
            return &mVehicles[i];
        }
    }
    return NULL;
}

bool ChaseManager::SpawningOnTopOfAnotherVehicle( const rmt::Sphere& s ) 
{
    VehicleCentral* vc = ::GetVehicleCentral();

    int nActiveVehicles = 0;
    Vehicle** activeVehicles = NULL;
    vc->GetActiveVehicleList( activeVehicles, nActiveVehicles );

    Vehicle* aCar;
    rmt::Vector aPos;
    rmt::Sphere aSphere;

    //
    // Because VehicleCentral's ActiveList is a SPARSE array, we have to loop
    // through the MAX array size. But since we know how many actual vehicles
    // there are in there, we can quit after we've reached that number. So keep
    // a counter.
    //
    int vCount = 0;
    for( int i=0; i<vc->GetMaxActiveVehicles(); i++ )
    {
        if( vCount >= nActiveVehicles )
        {
            break;
        }

        aCar = activeVehicles[i];
        if( aCar == NULL )
        {
            continue;
        }
        vCount++;
        aCar->GetPosition( &aPos );
        aCar->GetBoundingSphere( &aSphere );

        float distSqr = (aPos - s.centre).MagnitudeSqr();
        float minDist = aSphere.radius + s.radius;
        float minDistSqr = minDist * minDist;

        // if we're colliding with another car
        if( distSqr < minDistSqr )
        {
            return true;
        }
    }
    return false;
}

float ChaseManager::GetClosestCarPosition(rmt::Vector* toPos, rmt::Vector* carPos)
{
    float distSqr = 999999999.0f;

    for( int i=0; i<MAX_CHASE_VEHICLES; i++ )
    {
        if( mVehicles[i].isActive )
        {
            if( mVehicles[i].v != NULL )
            {
                rmt::Vector currPos;
                mVehicles[i].v->GetPosition(&currPos);
                float currDistSqr = (*toPos - currPos).MagnitudeSqr();
                if(currDistSqr < distSqr)
                {
                    distSqr = currDistSqr;
                    *carPos = currPos;
                }
            }
        }
    }
    return distSqr;
}

