//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pedestrianmanager.cpp
//
// Description: Implements Pedestrian Manager class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================
#define USEAVATARPOS

#include <raddebug.hpp> // for rAssert & other debug print outs
#include <stdlib.h>
#include <string.h>

#include <debug/profiler.h> // for the Profiler
#include <radmath/radmath.hpp> // for rmt::Vector 
#include <memory/srrmemory.h>

#include <meta/locatorevents.h>
#include <meta/pedgrouplocator.h>

#include <worldsim/ped/pedestrianmanager.h>

#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>

/*
#include <camera/supercammanager.h>
#include <camera/supercam.h>
*/

#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/characterrenderable.h>

#include <render/Culling/ReserveArray.h>
#include <render/IntersectManager/IntersectManager.h>

#include <roads/geometry.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>

#include <pedpaths/path.h>
#include <pedpaths/pathsegment.h>

#include <mission/gameplaymanager.h>

#include <worldsim/vehiclecentral.h>

// in meters, how much to translate spawn & remove radii forward 
// in the direction of the camera lookat

//#define PED_TEST
#ifdef PED_TEST
    const float PedestrianManager::FADE_RADIUS = 5.0f; // in meters
    const float PedestrianManager::CENTER_OFFSET = 5.0f;  // in meters
    const float PedestrianManager::ADD_RADIUS = 10.0f;     // in meters
    const float PedestrianManager::REMOVE_RADIUS = 15.0f;  // in meters
    const float PedestrianManager::INITIAL_ADD_RADIUS = 10.0f; // in meters
#else
    const float PedestrianManager::FADE_RADIUS = 60.0f; // in meters
    const float PedestrianManager::CENTER_OFFSET = 30.0f;  // in meters
    const float PedestrianManager::ADD_RADIUS = 45.0f;     // in meters
    const float PedestrianManager::REMOVE_RADIUS = 50.0f;  // in meters
    const float PedestrianManager::INITIAL_ADD_RADIUS = 90.0f; // in meters
#endif
const unsigned int PedestrianManager::MILLISECONDS_PER_GROUND_INTERSECT = 10;
const unsigned int PedestrianManager::MILLISECONDS_BETW_ADDS = 1;
const unsigned int PedestrianManager::MILLISECONDS_BETW_REMOVES = 1000;
const unsigned int PedestrianManager::MILLISECONDS_POPULATE_WORLD = 3000;

int PedestrianManager::mDefaultModelGroup = 0;

// *********************** STATICS ***********************

PedestrianManager* PedestrianManager::mInstance = NULL;

PedestrianManager* PedestrianManager::GetInstance() 
{
    if( mInstance == NULL )
    {
        mInstance = new(GMA_LEVEL_OTHER) PedestrianManager();
    }
    return mInstance;
}

void PedestrianManager::DestroyInstance()
{
    if( mInstance != NULL )
    {
        mInstance->mFreePeds.Clear();
        mInstance->mActivePeds.Clear();

        // Clean up Pedestrians. 
        int i = 0;
        for( i=0; i<PedestrianManager::MAX_PEDESTRIANS; i++ )
        {
            PedestrianStruct* pedStr = &(mInstance->mPeds[i]);
            rAssert( pedStr != NULL );

            Pedestrian* ped = pedStr->ped;
            rAssert( ped != NULL );

            if( ped->GetIsActive() )
            {
                ped->Deactivate();
            }
        }
        // Delete self
        delete mInstance;
    }
    mInstance = NULL;
}


// ******************** PUBLICS *************************

// call this in loading context
void PedestrianManager::InitDefaultModelGroups()
{
    // initialize all model groups with a default group
    // This is why we put PedManager::Init in LoadingContext, after 
    // CharacterManager::PreLoad and GamePlayManager::LoadBlahBlah,
    // cuz GamePlayManager will want to parse the level script which 
    // will register the modelgroups... 
    for( int i=0; i<PedestrianManager::MAX_MODEL_GROUPS; i++ )
    {
        mModelGroups[i].numModels = 4;
        mModelGroups[i].models[0].Init( "male1", 5 );
        mModelGroups[i].models[1].Init( "fem1", 5 );
        mModelGroups[i].models[2].Init( "boy1", 5 );
        mModelGroups[i].models[3].Init( "girl1", 5 );
        for( int j=4; j<PedestrianManager::MAX_MODELS; j++ )
        {
            mModelGroups[i].models[j].InitZero();
        }
    }
}

void PedestrianManager::SetDefaultModelGroup( int toGroupID )
{
    mDefaultModelGroup = toGroupID;
}

void PedestrianManager::Init()
{
    mAllowAdd = true;
    mMillisecondsTillRemove = PedestrianManager::MILLISECONDS_BETW_REMOVES;
    mMillisecondsTillAdd    = PedestrianManager::MILLISECONDS_BETW_ADDS;

    mMillisecondsPopulateWorld = PedestrianManager::MILLISECONDS_POPULATE_WORLD;

    mFreePeds.Clear();
    mActivePeds.Clear();

    for( int i=0; i<PedestrianManager::MAX_MODELS_IN_USE; i++ )
    {
        mModelsInUse[i].uid.SetText( NULL );
        mModelsInUse[i].currCount = 0;
    }
    mNumActiveModels = 0;

    // Recall that at this point, MissionManager has parsed the level script
    // which may or may not have populated our model groups. Recall also that
    // before that happened, we init all our model groups with a default group
    // Therefore, we can safely switch our current model group to group 0 
    // (it should exist one way or another).

    // switch to our group zero 
    UnregisterAllModels();

    mCurrGroupID = -1;
    SwitchModelGroup( mDefaultModelGroup ); // will set mCurrGroupID to 0 

    for( int i=0; i<PedestrianManager::MAX_PEDESTRIANS; i++ )
    {
        PedestrianStruct* pedStr = &(mPeds[i]);
        rAssert( pedStr != NULL );

        Pedestrian* ped = pedStr->ped;
        rAssert( ped != NULL );

        char name[5];
        sprintf( name, "ped%d", i );
        name[4] = '\0';

        // call AddCharacter with an initial, lightweight dummy model so 
        // we can init all the sim objects, the animation drivers, etc.
        Character* character = ::GetCharacterManager()->AddCharacter( 
            CharacterManager::NPC, name, "npd", "npd" );

        character->SetRole(Character::ROLE_PEDESTRIAN);

        rAssert( character != NULL );
        character->AddRef();
        character->SetController( pedStr->ped );
        character->mbAllowUnload = false;
        pedStr->ped->SetCharacter( character );

        int nameLen = strlen("npd");
        rAssert( nameLen <= PedestrianManager::MAX_STRING_LEN );
        strncpy( pedStr->modelName, "npd", PedestrianManager::MAX_STRING_LEN );
        pedStr->modelName[nameLen] = '\0';
        
        ped->InitZero();

        // add ped to list of free peds
        pedStr->listIndex = mFreePeds.AddLast( pedStr );
        rAssert( pedStr->listIndex != -1 );

    }
}




void PedestrianManager::Update( unsigned int milliseconds )
{
BEGIN_PROFILE( "Pedestrian Manager" );

    //rTunePrintf( "<<<<<<<<<<<<<<<<<<<<<<< NUM ACTIVE PEDS %d >>>>>>>>>>>>>>>>>>>>>>\n", mActivePeds.GetNumElems() );

    PedestrianStruct* pedStr = NULL;
    int i = 0;

    //
    // ================================
    // Get information about the player
    // ================================
    //
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
    /*
    SuperCam* superCam = ::GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
    rAssert( superCam != NULL );

    superCam->GetPosition( &pPos );
    superCam->GetVelocity( &pVel );
    pSpeed = pVel.Magnitude(); // *** SQUARE ROOT! ***
    */

    int debugCount, debugExpCount;

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
        center = pPos + camTarget * PedestrianManager::CENTER_OFFSET;
    }



    // ====================================
    // Update PedestrianManager properties
    // ======================================

    if( mMillisecondsPopulateWorld > milliseconds )
    {
        mMillisecondsPopulateWorld -= milliseconds;
    }
    else
    {
        mMillisecondsPopulateWorld = 0;
    }


    static bool remove = true;
    remove = !remove;

    //
    // ==================================================
    // Remove active pedestrians that are outside radius
    // ==================================================
    //
    if( remove &&  (mMillisecondsPopulateWorld == 0)) 
    {
        mMillisecondsTillRemove = PedestrianManager::MILLISECONDS_BETW_REMOVES;

        debugCount = 0;
        debugExpCount = mActivePeds.GetNumElems();

        int j = mActivePeds.GetHead();
        while( 0 <= j && j < DListArray::MAX_ELEMS )
        {   
            debugCount++;

            pedStr = (PedestrianStruct*) mActivePeds.GetDataAt( j );
            rAssert( pedStr != NULL );

            // Do radius test on character's bounding sphere
            // versus pedestrian radius...
            // 
            bool remove = false;

            rmt::Vector pedPos;
            pedStr->ped->GetCharacter()->GetPosition( pedPos );

            float distSqr = (center - pedPos).MagnitudeSqr();
            float zoneDistSqr = PedestrianManager::REMOVE_RADIUS * PedestrianManager::REMOVE_RADIUS;
            if( distSqr >= zoneDistSqr )
            {
                remove = true;
            }

            // Do frustrum test too... if outside our frustrum, remove it within n seconds

            const float MAX_OUT_OF_SIGHT_MILLISECONDS = 2000;

            if( !pedStr->ped->GetCharacter()->mbInAnyonesFrustrum &&
                pedStr->ped->mMillisecondsOutOfSight > MAX_OUT_OF_SIGHT_MILLISECONDS )
            {
                remove = true;
                pedStr->ped->mMillisecondsOutOfSight = 0;
            }

            j = mActivePeds.GetNextOf( j );

            if( remove )
            {
            #ifdef RAD_DEBUG
                CheckModelCounts();
            #endif 
                DeactivatePed( pedStr );
            #ifdef RAD_DEBUG
                CheckModelCounts();
            #endif 
                break;
            }
        }
//        rAssert( debugCount == debugExpCount );
    }
    else
    {
        //
        // ====================
        // Add Pedestrians
        // ====================
        //
        if( mMillisecondsPopulateWorld > 0 )
        {
            AddPeds( center, PedestrianManager::INITIAL_ADD_RADIUS );
        }
        else
        {
            AddPeds( center, PedestrianManager::ADD_RADIUS );
        }
    }


    // 
    // =================================
    // Update active pedestrians: 
    //   - fade them as necessary
    // =================================
    //
    i = mActivePeds.GetHead();
    while( 0 <= i && i < DListArray::MAX_ELEMS )
    {
        pedStr = (PedestrianStruct*) mActivePeds.GetDataAt(i);
        rAssert( pedStr != NULL );

        if( pedStr->ped->GetIsActive() )
        {
            ////////////////////////////////////////////////////
            // Update out of sight stuff
            if( !pedStr->ped->GetCharacter()->mbInAnyonesFrustrum )
            {
                pedStr->ped->mMillisecondsOutOfSight += milliseconds;
            }
            else
            {
                pedStr->ped->mMillisecondsOutOfSight = 0;
            }


            ////////////////////////////////////////////////////
            // Set fade if getting near fringe... 
            // 
            rmt::Vector myPos;
            pedStr->ped->GetCharacter()->GetPosition( myPos );

            float dist2Player = (pPos - myPos).Length(); // *** SQUARE ROOT! ***

            float fadeMinLimit = PedestrianManager::FADE_RADIUS;
            float fadeMaxLimit = PedestrianManager::ADD_RADIUS + PedestrianManager::CENTER_OFFSET;
            int fadeAlpha = 255;
            if( fadeMinLimit <= dist2Player && dist2Player <= fadeMaxLimit )
            {
                // if we're in the fading zone, gotta change fade alpha
                float fadeRatio = (dist2Player - fadeMinLimit)/(fadeMaxLimit - fadeMinLimit);
                fadeAlpha = (int) (255.0f * (1.0f - fadeRatio));
            }
            else if( dist2Player > fadeMaxLimit )
            {
                fadeAlpha = 0;
            }
            pedStr->ped->GetCharacter()->SetFadeAlpha( fadeAlpha );
        }
        i = mActivePeds.GetNextOf( i );
    }



END_PROFILE( "Pedestrian Manager" );

}

void PedestrianManager::RemovePed( Character* character )
{
    if( character == NULL )
    {
        return;
    }
    int j = mActivePeds.GetHead();
    while( 0 <= j && j < DListArray::MAX_ELEMS )
    {   
        PedestrianStruct* pedStr = (PedestrianStruct*) mActivePeds.GetDataAt( j );
        rAssert( pedStr != NULL );

        if( pedStr->ped->GetCharacter() == character )
        {
        #ifdef RAD_DEBUG
            CheckModelCounts();
        #endif 
            DeactivatePed( pedStr );
        #ifdef RAD_DEBUG
            CheckModelCounts();
        #endif 
            return;
        }
        j = mActivePeds.GetNextOf( j );
    }
}

void PedestrianManager::RemoveAllPeds()
{
    int j = mActivePeds.GetHead();
    while( 0 <= j && j < DListArray::MAX_ELEMS )
    {   
        PedestrianStruct* pedStr = (PedestrianStruct*) mActivePeds.GetDataAt( j );
        rAssert( pedStr != NULL );

        j = mActivePeds.GetNextOf( j );
        
    #ifdef RAD_DEBUG
        CheckModelCounts();
    #endif 
        DeactivatePed( pedStr );
    #ifdef RAD_DEBUG
        CheckModelCounts();
    #endif 
    }
}

void PedestrianManager::AllowAddingPeds( bool allowed )
{
    mAllowAdd = allowed;
}


// ************************ PRIVATES *************************

PedestrianManager::PedestrianManager() :
    mMillisecondsTillRemove( 0 ),
    mMillisecondsTillAdd( 0 ),
    mMillisecondsPopulateWorld( 0 ),
    mNumRegisteredModels( 0 ),
    mCurrGroupID( -1 ),
    mNumActiveModels( 0 ),
    mAllowAdd( true )
{
    // check some stuff once per compile..
    rAssert( PedestrianManager::ADD_RADIUS > 0 ); 
    rAssert( PedestrianManager::REMOVE_RADIUS > 0 );
    rAssert( PedestrianManager::MAX_STRING_LEN >= 6 );
    rAssert( 1 <= PedestrianManager::MAX_MODELS );
    rAssert( 0 <= PedestrianManager::MAX_PEDESTRIANS && 
             PedestrianManager::MAX_PEDESTRIANS <= DListArray::MAX_ELEMS );

    // listen to appropriate events
    GetEventManager()->AddListener( this, (EventEnum)( EVENT_LOCATOR + LocatorEvent::LOAD_PED_MODEL_GROUP ) );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_PLAYER_CAR_HIT_NPC );
    GetEventManager()->AddListener( this, EVENT_OBJECT_KICKED );
    GetEventManager()->AddListener( this, EVENT_PLAYER_VEHICLE_HORN );

    /*** DEBUG ***
    GetEventManager()->AddListener( this, (EventEnum)(EVENT_GETOUTOFVEHICLE_END) );
    *** DEBUG ***/

    for( int i=0; i<PedestrianManager::MAX_PEDESTRIANS; i++ )
    {
        mPeds[i].ped = new (GMA_LEVEL_OTHER) Pedestrian();
        mPeds[i].ped->AddRef();
        mPeds[i].listIndex = -1;
    }
}

PedestrianManager::~PedestrianManager()
{
    GetEventManager()->RemoveAll( this );

    int i;
    for( i=0; i<PedestrianManager::MAX_PEDESTRIANS; i++ )
    {
        if( mPeds[i].ped != NULL )
        {
            Character* character = mPeds[i].ped->GetCharacter();
            if( character != NULL )
            {
                // Just release the pointer; don't bother calling
                // RemoveCharacter(). The character gets cleaned up 
                // later by CharacterManager's destructor
                character->Release();
            }
            mPeds[i].ped->Release();
        }
    }
}

void PedestrianManager::DeactivatePed( PedestrianStruct* pedStr )
{
    rAssert( pedStr != NULL );
    rAssert( pedStr->ped != NULL );

    rAssert( 1 <= mNumActiveModels && mNumActiveModels <= PedestrianManager::MAX_MODELS_IN_USE );

    //rDebugPrintf( "*** Removing %s\n", pedStr->modelName );

    // remove ped from path
    Path* path = pedStr->ped->GetPath();
    rAssert( path != NULL );
    bool res = path->RemovePedestrian();
    rAssert( res );

    // reset ped parameters
    pedStr->ped->Deactivate();

#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 
    // If we can find the model in list of currently registered models, 
    // decrement count. The reason we need to search is that the model
    // can be unregistered and re-registered in a different location.
    // If we can't find it anywhere, then it must have been unregistered.
    int modelID = GetModelIDByName( pedStr->modelName );
    if( modelID != -1 )
    {
        mModels[modelID].currCount--;
    }
    else
    {
        rDebugPrintf( "     Not found in model group, so I'll swap in npd!\n" );

    #ifdef RAD_DEBUG
        SanityCheck();
    #endif
        // we're swapping out the model for npd... better update the modelsinuse list
        FindModelInUseAndRemove( pedStr->modelName );

        // since this model has been unregistered, it's not in the current
        // model group, so keeping it around will only waste memory...
        // we need to swap in "npd" in its place.
        Character* character = pedStr->ped->GetCharacter();
        rAssert( character != NULL );
        ::GetCharacterManager()->SwapData( character, "npd", "npd" );
        sprintf( pedStr->modelName, "npd" );

    #ifdef RAD_DEBUG
        SanityCheck();
    #endif
    }

    // manage the ped manager's active/free lists
    res = mActivePeds.Remove( pedStr->listIndex );
    rAssert( res );
    pedStr->listIndex = mFreePeds.AddLast( pedStr );
    rAssert( pedStr->listIndex > -1 );

#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 


}

float PedestrianManager::GetRandomMinDistSqr()
{
    static const float MINDISTSQR_FROM_PED_ON_SAME_PATH_A = 16.0f;
    static const float MINDISTSQR_FROM_PED_ON_SAME_PATH_B = 49.0f;
    static const float MINDISTSQR_FROM_PED_ON_SAME_PATH_C = 144.0f;

    int coinflip = rand() % 3;
    float mindistsqr = 0.0f;
    switch( coinflip )
    {
    case 0:
        {
            mindistsqr = MINDISTSQR_FROM_PED_ON_SAME_PATH_A;
        }
        break;
    case 1:
        {
            mindistsqr = MINDISTSQR_FROM_PED_ON_SAME_PATH_B;
        }
        break;
    case 2:
        {
            mindistsqr = MINDISTSQR_FROM_PED_ON_SAME_PATH_C;
        }
        break;
    default:
        {
            mindistsqr = MINDISTSQR_FROM_PED_ON_SAME_PATH_A;
        }
        break;
    }

    return mindistsqr;
}

bool PedestrianManager::ActivatePed( PathSegment* segment, rmt::Vector spawnPos )
{
    rAssert( segment != NULL );

    PedestrianStruct* pedStr = NULL;
    Pedestrian* ped = NULL;

    // get the path for this pedestrian & try to add ped to it & 
    // store index
    Path* path = segment->GetParentPath();
    rAssert( path != NULL );

    // Check to make sure it isn't already at max ped limit
    if( path->IsFull() )
    {
        return false;
    }

    /*
    // Make sure we're not too close to another ped...
    float minDistSqr = GetRandomMinDistSqr();
    rmt::Vector pedPos; 
    int i = mActivePeds.GetHead();
    while( 0 <= i && i < DListArray::MAX_ELEMS )
    {
        pedStr = (PedestrianStruct*) mActivePeds.GetDataAt(i);
        rAssert( pedStr != NULL );

        pedStr->ped->GetCharacter()->GetPosition( &pedPos );
        pedPos.y = spawnPos.y; // ignore diff in y

        if( (pedPos-spawnPos).MagnitudeSqr() < minDistSqr )
        {
            return false; 
        }
        i = mActivePeds.GetNextOf( i );
    }
    */

    // make sure we're not spawning too close to another character 
    // or vehicle
    float minDistSqr = GetRandomMinDistSqr();
    float distSqr = 100000.0f;
    rmt::Vector hisPos;

    CharacterManager* cm = GetCharacterManager();
    int maxChars = cm->GetMaxCharacters();
    for( int i=0; i<maxChars; i++ )
    {
        Character* c = cm->GetCharacter(i);
        if( c )
        {
            c->GetPosition( hisPos );
            hisPos.y = spawnPos.y; // ignore diff in y

            distSqr = (hisPos - spawnPos).MagnitudeSqr();
            if( distSqr < minDistSqr )
            {
                return false;
            }
        }
    }

    VehicleCentral* vc = GetVehicleCentral();
    for( int i=0; i<VehicleCentral::MAX_ACTIVE_VEHICLES; i++ )
    {
        Vehicle* v = vc->GetVehicle(i);
        if( v )
        {
            v->GetPosition( &hisPos );
            hisPos.y = spawnPos.y; // ignore diff in y

            rmt::Sphere s;
            v->GetBoundingSphere( &s );

            distSqr = (hisPos - spawnPos).MagnitudeSqr();

            minDistSqr = s.radius + 1.0f;
            minDistSqr *= minDistSqr;

            if( distSqr < minDistSqr )
            {
                return false;
            }
        }
    }



    // Choose a model for our new pedestrian
    int modelID = GetRandomModel();
    if( modelID <= -1 )
    {
        return false;
    }

    //*** Test ***
    //rDebugPrintf( "Model randomly chosen: %d, %s\n", modelID, mModels[modelID].name );
    //*** Test ***


    // if reached maxcount for this model, go to next model and use it instead
    bool foundUsable = false;
    for( int i=0; i<PedestrianManager::MAX_MODELS; i++ )
    {
        if( !mModels[modelID].free && 
            mModels[modelID].currCount < mModels[modelID].maxCount )
        {
            foundUsable = true;
            break;
        }
        // go to next model
        modelID = (modelID+1) % PedestrianManager::MAX_MODELS;
    }
    // if we never found a model with currCount < maxCount, then
    // we can't actually do any spawning... 
    if( !foundUsable )
    {
        rTunePrintf( "Can't spawn a new ped. Not enough Ped model "
            "instances allowed in leveli.mfk. See Dusit or Sheik.\n" );
        return false;
    }

    PedestrianStruct* freeNonNPDNotInCurrModelGroup = NULL;
    PedestrianStruct* pedStrFoundInFreeList = NULL;

    // See if this model is already available in our freepeds list...
    // If so, we don't need to call SwapData (save some processing time)
    bool foundInFreeList = false;
    int j = mFreePeds.GetHead();
    while( 0 <= j && j < DListArray::MAX_ELEMS )
    {
        pedStr = (PedestrianStruct*) mFreePeds.GetDataAt(j);
        rAssert( pedStr != NULL );
        if( !foundInFreeList && strcmp(pedStr->modelName, mModels[modelID].name)==0 )
        {
            foundInFreeList = true;
            pedStrFoundInFreeList = pedStr;
            //rDebugPrintf( "*** Yes! We found an existing model %s in the freelist!\n", pedStr->modelName );
            //break;
        }
        else
        {
            if( strcmp(pedStr->modelName, "npd") != 0 &&
                GetModelIDByName( pedStr->modelName ) == -1 )
            {
                freeNonNPDNotInCurrModelGroup = pedStr;
            }
        }
        j = mFreePeds.GetNextOf( j );
    }

    // if none of the freepeds has the model we want, we need
    // to swap out one of them and put in the model we want.
    if( !foundInFreeList )
    {
        // 
        // Since we never found the model we want in the list of free peds, 
        // we should try to find a "free" model (npd, or a model not
        // currently in our model group, so we don't clobber another real 
        // model that might be wanted later).. 
        // 
        // We could achieve this by searching for models not in current group in free list...
        // However, if we never found one, then we have to just pick one, preferrably an "npd"
        // Since we always add the most recently removed ped to the END of the freepeds 
        // list, we should take from the FRONT of the freepeds list... sort of a mini-scheduling
        // algorithm, ya know...
        //
        if( freeNonNPDNotInCurrModelGroup )
        {
            pedStr = freeNonNPDNotInCurrModelGroup;
        }
        else
        {
            pedStr = (PedestrianStruct*) mFreePeds.GetFirst();
        }
        rAssert( pedStr != NULL );
        bool res = SwapModels( modelID, pedStr );
        if( !res )
        {
            return false;
        }
    }
    else
    {
        pedStr = pedStrFoundInFreeList;
    }

    //////////////////////////////////////////////////////////
    // NOTE: 
    // At this point we're ready to do the actual activating (no more bailing out)

    //*** Test ***
    //rDebugPrintf( "*** Adding %s\n", mModels[modelID].name );
    //*** Test ***
#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 

    mModels[modelID].currCount++;

    rAssert( strcmp( pedStr->modelName, mModels[modelID].name ) == 0 );

    rAssert( pedStr->ped != NULL );

    // choose a random swatch
    int swatch = rand() % CharacterRenderable::NUM_CHARACTER_SWATCHES;
    pedStr->ped->GetCharacter()->SetSwatch( swatch );

    // add ped to path
    bool res = path->AddPedestrian( );
    rAssert( res );

    /*
    // get the segment's index (to the parent path's list of segments)
    // this will be needed by Pedestrian::Update() when locomoting.
    int pathSegmentIndex = segment->GetIndexToParentPath();
    rAssert( 0 <= pathSegmentIndex && pathSegmentIndex < path->GetNumPathSegments() );
    */

    // set the ped's parameters
    pedStr->ped->Activate( path, segment, spawnPos, pedStr->modelName );

    // manage the ped manager's active/free list
    res = mFreePeds.Remove( pedStr->listIndex );
    rAssert( res );

    pedStr->listIndex = mActivePeds.AddLast( pedStr );
    rAssert( pedStr->listIndex != -1 );

#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 
    return true;
}

bool PedestrianManager::SwapModels( int modelID, PedestrianStruct* pedStr )
{
    rAssert( pedStr != NULL );
    rTuneAssert( 0 <= modelID && modelID < PedestrianManager::MAX_MODELS );

    Character* character = NULL;

#ifdef RAD_DEBUG
    SanityCheck();
#endif
    FindModelInUseAndRemove( pedStr->modelName );
    bool res = FindModelInUseAndAdd( mModels[modelID].name );
    if( res == false )
    {
        // we already removed pedStr->modelName from list, but haven't really
        // swapped him out... 
        // If he's in the list of currently registered models, we don't want
        // to really swap him out anyway (so call FindModelInUseAndAdd(pedStr->modelName))
        // But if he's not in the list of currently registered models, we DO
        // want to swap him out.. so call SwapData passing in "npd"
        if( strcmp( pedStr->modelName, "npd" ) != 0 )
        {
            int index = GetModelIDByName( pedStr->modelName );
            if( index == -1 )
            {
                character = pedStr->ped->GetCharacter();
                rAssert( character != NULL );
                ::GetCharacterManager()->SwapData( character, "npd", "npd" );
                sprintf( pedStr->modelName, "npd" );
            }
            else
            {
                res = FindModelInUseAndAdd( pedStr->modelName );
                rAssert( res );
            }
        }
        return false;
    }

    character = pedStr->ped->GetCharacter();
    rAssert( character != NULL );
    ::GetCharacterManager()->SwapData( character, mModels[modelID].name, "npd" );

    int nameLen = strlen( mModels[modelID].name );
    rAssert( nameLen <= PedestrianManager::MAX_STRING_LEN );
    strncpy( pedStr->modelName, mModels[modelID].name, PedestrianManager::MAX_STRING_LEN );
    pedStr->modelName[nameLen] = '\0';

#ifdef RAD_DEBUG
    SanityCheck();
#endif


    return true;
}

int PedestrianManager::GetRandomModel()
{
    if( mNumRegisteredModels < 1 )
    {
        rTunePrintf( "PedestrianManager: You haven't registered a pedestrian model!\n" );
        return -1;
    }

    // generate a number between 1 and mNumRegisteredModels
    int num = (rand() % mNumRegisteredModels) + 1;
    rAssert( 1 <= num && num <= mNumRegisteredModels );

    // loop through models array counting till we get to num & return this modelID
    int count = 0;
    for(int i=0; i<PedestrianManager::MAX_MODELS; i++)
    {
        if( !mModels[i].free )
        {
            count++;
        }
        if( count==num )
        {
            return i;
        }
    }
    rAssert( false ); // shouldn't get here
    return -1;
}

bool PedestrianManager::RegisterModel( const char* name, int maxCount )
{
    rTuneAssert( name != NULL );
    rTuneAssert( maxCount > 0 );

    // search existing SPARSE list for name
    int freeIndex = -1;
    for( int i=0; i<PedestrianManager::MAX_MODELS; i++ )
    {
        if( mModels[i].free )
        {
            freeIndex = i;
            continue;
        }

        // found an existing model registered under same name, so overwrite w/ new values
        if( strcmp(mModels[i].name, name)==0 )
        {
            mModels[i].maxCount = maxCount;
            // currCount should be the same as there are active instances
        #ifdef RAD_DEBUG
            CheckModelCounts();
        #endif 
            return true;
        }
    }

    // If we haven't found an existing model, then we're adding one.
    // See if we can...
    if( mNumRegisteredModels >= PedestrianManager::MAX_MODELS )
    {
        return false;
    }

    if( 0 <= freeIndex && freeIndex < PedestrianManager::MAX_MODELS )
    {
        // loop through activelist looking for this model and counting the 
        // number of instances... init it with this value
        int count = 0;
        int n = mActivePeds.GetHead();
        while( 0 <= n && n < DListArray::MAX_ELEMS )
        {
            PedestrianStruct* pedStr = (PedestrianStruct*) mActivePeds.GetDataAt(n);
            rAssert( pedStr );

            if( strcmp( pedStr->modelName, name )==0 )
            {
                count++;
            }

            n = mActivePeds.GetNextOf( n );
        }

        mModels[freeIndex].Init( false, name, maxCount, count );
        mNumRegisteredModels++;
        return true;
    }

    return false;
}

bool PedestrianManager::UnregisterModel( const char* name )
{
    rAssert( name != NULL );

    if( mNumRegisteredModels <= 0 )
    {
        return false;
    }

    // search existing SPARSE list for name
    for( int i=0; i<PedestrianManager::MAX_MODELS; i++ )
    {
        if( mModels[i].free )
        {
            continue;
        }
        int nameLen = strlen( name );
        int modelNameLen = strlen( mModels[i].name );
        if( (nameLen == modelNameLen) && 
            (strncmp(mModels[i].name, name, nameLen)==0) )
        {
            mNumRegisteredModels--;
            mModels[i].InitZero();
            return true;
        }
    }
    return false;
}

bool PedestrianManager::UnregisterModel( int modelID )
{
    rAssert( 0 <= modelID && modelID < PedestrianManager::MAX_MODELS );

    if( mNumRegisteredModels <= 0 )
    {
        return false;
    }
    if( mModels[modelID].free )
    {
        return false;
    }
    mNumRegisteredModels--;
    mModels[modelID].InitZero();
    return true;
}

void PedestrianManager::UnregisterAllModels()
{
    mNumRegisteredModels = 0;
    for( int i=0; i<PedestrianManager::MAX_MODELS; i++ )
    {
        mModels[i].InitZero();
    }
}

void PedestrianManager::SwitchModelGroup( int toGroupID )
{
    rAssert( 0 <= toGroupID && toGroupID < PedestrianManager::MAX_MODEL_GROUPS );
#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 

    // if already in this group, forget it
    if( mCurrGroupID == toGroupID )
    {
        return;
    }

    // NOTE: 
    // Don't want to UnregisterAllModels here because if say male1 exists in new group as 
    // well as in old group, we'll be wiping its currcount. So just go around resetting 
    // maxcount to zero and let RegisterModel overwrite the ones that are pertinent. 
    // In the end, unregister everything that doesn't have maxcount > 0
    for( int i=0; i<PedestrianManager::MAX_MODELS; i++ )
    {
        mModels[i].maxCount = 0;
    }
    for( int i=0; i<mModelGroups[toGroupID].numModels; i++ )
    {
        RegisterModel(
            mModelGroups[toGroupID].models[i].name, 
            mModelGroups[toGroupID].models[i].maxCount );
    }
    for( int i=0; i<PedestrianManager::MAX_MODELS; i++ )
    {
        if( mModels[i].maxCount <= 0 )
        {
            UnregisterModel( i );
        }
    }
#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 
    mCurrGroupID = toGroupID;

#ifdef RAD_DEBUG
    char msg[256];
    sprintf( msg, "*** SWITCHING TO NEW MODEL GROUP: " );
    for( int i=0; i<mModelGroups[toGroupID].numModels; i++ )
    {
        sprintf( msg, "%s%s ", msg, mModelGroups[toGroupID].models[i].name );
    }
    sprintf( msg, "%s\n", msg );
    //rDebugPrintf( msg );
#endif


}

int PedestrianManager::GetModelIDByName( const char* name )
{
    for( int i=0; i<PedestrianManager::MAX_MODELS; i++ )
    {
        if( strcmp( mModels[i].name, name )==0 )
        {
            return i;
        }
    }
    return -1;
}

void PedestrianManager::SetModelGroup( int groupID, const ModelGroup& group )
{
    rAssert( 0 <= groupID && groupID < PedestrianManager::MAX_MODEL_GROUPS );
    rAssert( 0 <= group.numModels && group.numModels <= PedestrianManager::MAX_MODELS );

    mModelGroups[groupID].numModels = group.numModels;

    for( int i=0; i<group.numModels; i++ )
    {
        mModelGroups[groupID].models[i].Init( 
            group.models[i].name, group.models[i].maxCount );
    }

    for( int i=group.numModels ; i<PedestrianManager::MAX_MODELS; i++ )
    {
        mModelGroups[groupID].models[i].InitZero();
    }
}

//=============================================================================
// PedestrianManager::IsPed
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* character )
//
// Return:      bool 
//
//=============================================================================
bool PedestrianManager::IsPed( Character* character )
{
    int i;
    for (i = 0; i < MAX_PEDESTRIANS; ++i )
    {
        if ( mPeds[ i ].ped->GetCharacter() == character )
        {
            return true;
        }
    }

    return false;
}


void PedestrianManager::HandleEvent( EventEnum id, void* pEventData )
{
    /*** DEBUG ***
    switch( id )
    {
    case EVENT_GETOUTOFVEHICLE_END:
        {
            static int count = 0;
            SwitchModelGroup( count % 2 );
            count++;
        }
        break;
    }
    *** DEBUG ***/

    // Handle the events appropriately: 
    // - determine which model group we want to switch to and call SwitchToGroup on it
    switch ( id )
    {
    case EVENT_LOCATOR + LocatorEvent::LOAD_PED_MODEL_GROUP:
        {
            PedGroupLocator* pLocator = (PedGroupLocator*)pEventData;
            rAssert( pLocator != NULL );

            unsigned int groupID = pLocator->GetGroupNum();
            if( pLocator->GetPlayerEntered() )
            {
                SwitchModelGroup( (int)groupID );
            }
            break;
        }
    case EVENT_VEHICLE_DESTROYED:
        {
            // spawn a pedestrian in th driver's seat when a traffic vehicle is destroyed
            Vehicle* vehicle = (Vehicle*)pEventData;
            if( (GetGameplayManager()->GetCurrentLevelIndex() != RenderEnums::L7) && 
                (strcmp(vehicle->GetDriverName(), "phantom") == 0) &&
                mActivePeds.GetNumElems() > 0 )
            {
                int randPedNum = rand() % mActivePeds.GetNumElems();

                Character* character = NULL;

                int count = 0;
                int ind = mActivePeds.GetHead();
                while( 0 <= ind && ind <= MAX_PEDESTRIANS )
                {
                    if( count == randPedNum )
                    {
                        PedestrianStruct* pedStr = (PedestrianStruct*) mActivePeds.GetDataAt( ind );
                        character = pedStr->ped->GetCharacter();
                        break;
                    }
                    count++;
                    ind = mActivePeds.GetNextOf( ind );
                }

                //Character* character = mPeds[rand() % MAX_PEDESTRIANS].ped->GetCharacter();
        
                if(character)
                {
                    vehicle->SetDriver(character);

                    character->SetFadeAlpha( 255 ); // make sure we're not faded.
                    character->SetTargetVehicle( vehicle );
                    character->SetInCar( true );
                    character->UpdateTransformToInCar();        
                    character->GetStateManager()->SetState<CharacterAi::InCar>();
                }
            }
        }
        break;
    case EVENT_PLAYER_VEHICLE_HORN: // fall thru
    case EVENT_PLAYER_CAR_HIT_NPC: // fall thru
    case EVENT_OBJECT_KICKED:
        {
            static const float TRIGGER_PANIC_RADIUS_SQR = 144.0f;

            // Get player data
            Avatar* player = GetAvatarManager()->GetAvatarForPlayer(0);
            rmt::Vector playerPos;
            player->GetPosition( playerPos );

            // for any ped nearby, make him run away... 
            int i = mActivePeds.GetHead();
            while( 0 <= i && i <= DListArray::MAX_ELEMS )
            {
                PedestrianStruct* pedStr = (PedestrianStruct*) mActivePeds.GetDataAt( i );
                rAssert( pedStr );

                if( pedStr->ped->GetIsActive() )
                {
                    rmt::Vector pedPos;
                    pedStr->ped->GetCharacter()->GetPosition( pedPos );
                    float distSqr = (playerPos - pedPos).MagnitudeSqr();

                    if( distSqr < TRIGGER_PANIC_RADIUS_SQR )
                    {
                        pedStr->ped->IncitePanic();
                    }
                }
                i = mActivePeds.GetNextOf( i );
            }
        }
        break;

    default:
        {
            break;
        }
    }
}


void PedestrianManager::AddPeds( rmt::Vector center, float addRadius )
{
    if( CommandLineOptions::Get( CLO_NO_PEDS ) )
    {
        return;
    }

    if( !mAllowAdd )
    {
        return;
    }

    // if no more free pedestrians...
    if( mFreePeds.GetNumElems() <= 0 )
    {
        return;
    }

    // Invoke search on DSG for path segments that intersect with 
    // our radius
    ReserveArray<PathSegment*> orList;
    ::GetIntersectManager()->FindPathSegmentElems( center, addRadius, orList );

    //
    // For each segment we "intersect" with (in the loose sense), we
    //   a) break if we have no more pedestrians to spawn
    //   b) break if segment (or path?) already has a pedestrian
    //   c) put pedestrian on the segment at the intersection point.
    //   d) set pedestrian's facing/velocity/position, whatever's necessary
    //
    if(orList.mUseSize == 0)
        return;

    int which = rand() % orList.mUseSize;

    PathSegment* segment = NULL;

    segment = orList.mpData[which];
    rAssert( segment != NULL );
    
    // determine intersection point of the path segment & ped radius 
    // at normalized value "t"

    rmt::Sphere s;
    s.Set( center, addRadius );

    rmt::Vector start, end;
    segment->GetStartPos( start );
    segment->GetEndPos( end );

    rmt::Vector intPts[2];
    int numIntersections = IntersectLineSphere( start, end, s, intPts );

    // if no intersection, a mild surprise. 
    if( numIntersections <= 0 )
    {
        return;
    }
    
    // Just use the first intersection point
    rmt::Vector spawnPos = intPts[0];
    float t = GetLineSegmentT( start, end, spawnPos );

    // activate the pedestrian
#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 

    bool succeeded = ActivatePed( segment, spawnPos );
#ifdef RAD_DEBUG
    CheckModelCounts();
#endif 
}

void PedestrianManager::FindModelInUseAndRemove( const char* name )
{
    if( tName::MakeUID(name) == tName::MakeUID("npd") ) 
    {
        return;
    }
    // should be able to find it in the list of models being used...
    tUID uid = tEntity::MakeUID( name );
    bool foundUid = false;

    rAssert( 0 <= mNumActiveModels && mNumActiveModels <= PedestrianManager::MAX_MODELS_IN_USE );
    for( int i=0; i<mNumActiveModels; i++ )
    {
        if( uid == mModelsInUse[i].uid.GetUID() )
        {
            // count for this ped should be > 0 if it was found in this list
            rAssert( mModelsInUse[i].currCount > 0 );
            mModelsInUse[i].currCount--;

            // if currCount is zero, remove it from list of models currently in use.
            if( mModelsInUse[i].currCount == 0 )
            {
                // swap last entry into this position
                //rDebugPrintf( "*** Removing from ModelsInUse %s\n", name );
                mModelsInUse[i].currCount = mModelsInUse[mNumActiveModels-1].currCount;
                mModelsInUse[i].uid = mModelsInUse[mNumActiveModels-1].uid;
                mNumActiveModels--;
            }
            foundUid = true;
            break; // NOTE: MUST break here since we may have modified mNumActiveModels
        }
    }
    if( !foundUid )
    {
        // should have found it! Something wrong with our code...
        rAssert( false );
    }
}

bool PedestrianManager::FindModelInUseAndAdd( const char* name )
{
    if( strcmp( name, "npd" ) == 0 )
    {
        return true;
    }

    // PS2 memory can only support a few (4) models in use at all times... 
    // see if we have room for this model
    bool founduid = false;
    tUID uid = tEntity::MakeUID( name );

    rAssert( 0 <= mNumActiveModels && mNumActiveModels <= PedestrianManager::MAX_MODELS_IN_USE );
    for( int i=0; i<mNumActiveModels; i++ )
    {
        if( uid == mModelsInUse[i].uid.GetUID() )
        {
            mModelsInUse[i].currCount++;
            founduid = true;
            break;
        }
    }
    if( !founduid )
    {
        // couldn't find it in list of models currently in use, so try to add it
        // to the list. If we can't, we don't spawn this model
        if( mNumActiveModels == PedestrianManager::MAX_MODELS_IN_USE )
        {
            return false;
        }
        //rDebugPrintf( "*** Adding to ModelsInUse %s\n", name );
        mModelsInUse[mNumActiveModels].uid.SetText( name );
        mModelsInUse[mNumActiveModels].currCount = 1;
        mNumActiveModels++;
    }
    return true;
}

void PedestrianManager::SanityCheck()
{
#ifdef RAD_DEBUG
    // make sure every non-"npd" model in mPeds exists in ModelsInUse
    char msg[256];
    for( int i=0; i<PedestrianManager::MAX_PEDESTRIANS; i++ )
    {
        if( strcmp( mPeds[i].modelName, "npd" ) == 0 )
        {
            continue;
        }

        bool found = false;
        for( int j=0; j<mNumActiveModels; j++ )
        {
            tUID test = tEntity::MakeUID( mPeds[i].modelName );
            if( test == mModelsInUse[j].uid.GetUID() )
            {
                sprintf( msg, "%s already came up once in mModelsInUse list\n", 
                    mPeds[i].modelName );
                rAssertMsg( !found, msg);

                sprintf( msg, "UIDs equal but names are not! %s vs %s\n", 
                    mPeds[i].modelName, mModelsInUse[j].uid.GetText() );
                rAssertMsg( strcmp( mPeds[i].modelName, mModelsInUse[j].uid.GetText() ) == 0, msg );
                found = true;
            }
        }
        sprintf( msg, "%s in mPeds could not be found in mModelsInUse\n", 
            mPeds[i].modelName );
        rAssertMsg( found, msg );
    }

    // make sure every model in ModelsInUse is non-"npd" and exists in mPeds, with correct count
    for( int i=0; i<mNumActiveModels; i++ )
    {
        rAssertMsg( strcmp( mModelsInUse[i].uid.GetText(), "npd" ) != 0, 
            "Found npd in mModelsInUse\n" );

        bool found = false;
        int count = 0;
        for( int j=0; j<PedestrianManager::MAX_PEDESTRIANS; j++ )
        {
            tUID test = tEntity::MakeUID( mPeds[j].modelName );
            if( test == mModelsInUse[i].uid.GetUID() )
            {
                count++;
                sprintf( msg, "UIDs equal but names are not! %s vs %s\n", 
                    mPeds[j].modelName, mModelsInUse[i].uid.GetText() );
                rAssertMsg( strcmp( mPeds[j].modelName, mModelsInUse[i].uid.GetText() ) == 0, msg );
                found = true;
            }
        }
        sprintf( msg, "%s in mModelsInUse could not be found in mPeds\n", 
            mModelsInUse[i].uid.GetText() );
        rAssertMsg( found, msg );

        sprintf( msg, "Not equals numbers of %s found in mPeds (%d) and mModelsInUse (%d)\n", 
            mModelsInUse[i].uid.GetText(), count, mModelsInUse[i].currCount );
        rAssertMsg( count == mModelsInUse[i].currCount, msg );
    }
#endif
}


void PedestrianManager::CheckModelCounts()
{
    // *** DEBUG ***
    // do some checking here to make sure mModels' currcount is 
    // correct
    for( int m=0; m<PedestrianManager::MAX_MODELS; m++ )
    {
        if( mModels[m].free )
        {
            continue;
        }
        int expectedCount = mModels[m].currCount;

        int count = 0;
        int n = mActivePeds.GetHead();
        while( 0 <= n && n < DListArray::MAX_ELEMS )
        {
            PedestrianStruct* pedStr = (PedestrianStruct*) mActivePeds.GetDataAt(n);
            rAssert( pedStr );

            if( strcmp( pedStr->modelName, mModels[m].name )==0 )
            {
                count++;
            }

            n = mActivePeds.GetNextOf( n );
        }
        rAssert( count == expectedCount );
    }
    // *** DEBUG ***
}

void PedestrianManager::DumpAllPedModels()
{
    // deactivate all current peds
    RemoveAllPeds();

    // Swap "npd" in for the other models
    for( int i=0; i<PedestrianManager::MAX_PEDESTRIANS; i++ )
    {
        PedestrianStruct* pedStr = &(mPeds[i]);
        rAssert( pedStr );

        if( tName::MakeUID( pedStr->modelName ) == tName::MakeUID( "npd" ) )
        {
            continue;
        }

    #ifdef RAD_DEBUG
        SanityCheck();
    #endif
        // we're swapping out the model for npd... better update the modelsinuse list
        FindModelInUseAndRemove( pedStr->modelName );

        // since this model has been unregistered, it's not in the current
        // model group, so keeping it around will only waste memory...
        // we need to swap in "npd" in its place.
        Character* character = pedStr->ped->GetCharacter();
        rAssert( character != NULL );
        ::GetCharacterManager()->SwapData( character, "npd", "npd" );
        sprintf( pedStr->modelName, "npd" );

    #ifdef RAD_DEBUG
        SanityCheck();
    #endif
    }

}