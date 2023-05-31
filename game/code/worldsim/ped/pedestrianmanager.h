//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pedestrianmanager.h
//
// Description: Defines Pedestrian Manager class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================


#ifndef PEDESTRIANMANAGER_H
#define PEDESTRIANMANAGER_H


#include <worldsim/ped/pedestrian.h>
#include <roads/geometry.h>
#include <events/eventlistener.h>
#include <p3d/entity.hpp>
#include <string.h>

class PathSegment;

// Hey look! I'm a singleton!
class PedestrianManager :
public EventListener
{

// MEMBERS
public:
    // STATICS
    static const int MAX_PEDESTRIANS = 7; // should be <= DListArray::MAX_ELEMS;
    static const int MAX_MODELS_IN_USE = 4; // number of distinct models we can current support at a time
    static const int MAX_MODELS = 10; // num distinct models for 2 groups (while one takes over the other)
    static const int MAX_STRING_LEN = 64;
    static const int MAX_MODEL_GROUPS = 10; // number of different groups we allow to be defined

    static PedestrianManager* mInstance;
    static const float FADE_RADIUS;
    static const float CENTER_OFFSET;
    static const float ADD_RADIUS;     
    static const float INITIAL_ADD_RADIUS;     
    static const float REMOVE_RADIUS;  
    static const unsigned int MILLISECONDS_PER_GROUND_INTERSECT;
    static const unsigned int MILLISECONDS_BETW_ADDS;
    static const unsigned int MILLISECONDS_BETW_REMOVES;
    static const unsigned int MILLISECONDS_POPULATE_WORLD;

    // NON-STATICS
    struct ModelData
    {
        ModelData() : maxCount( 0 ) {};
        char name[MAX_STRING_LEN+1];
        int maxCount;
        void Init( const char* modelName, int max )
        {
            rAssert( modelName != NULL );
            rAssert( max > 0 );

            int nameLen = strlen( modelName );
            rAssert( nameLen <= PedestrianManager::MAX_STRING_LEN );
            strncpy( name, modelName, PedestrianManager::MAX_STRING_LEN );
            name[ nameLen ] = '\0';

            maxCount = max;
        }
        void InitZero()
        {
            name[0] = '\0';
            maxCount = 0;
        }

    };
    struct ModelGroup
    {
        ModelGroup() : numModels( 0 ) {};
        int numModels;
        ModelData models[MAX_MODELS];
    };

    // Need lists to keep track of inactive peds & active peds
    // 
    // Whenever we set mPeds[i] to active, remove it from free list
    // and add it to active list:
    // 
    //   mFreePeds.Remove(mPeds[i].listIndex);
    //   mPeds[i].listIndex = mActivePeds.AddLast( &mPeds[i] );
    //
    // Whenever we set mPeds[i] to inactive, add it to free list &
    // remove it from active list:
    //
    //   mActivePeds.Remove(mPeds[i].listIndex);
    //   mPeds[i].listIndex = mFreePeds.AddLast(&mPeds[i]);
    //
    DListArray mFreePeds;   
    DListArray mActivePeds;

    // Pedestrian alone is not sufficient for our purposes... 
    struct PedestrianStruct
    {
        PedestrianStruct() : ped( NULL ), listIndex( -1 ) {};
        Pedestrian* ped;
        int listIndex; // index to mActivePeds or mFreePeds depending on whether ped is active
        char modelName [MAX_STRING_LEN+1]; // name of model
    };


// METHODS
public:

    // STATICS
    static PedestrianManager* GetInstance();
    static void DestroyInstance();
    static int GetMaxPedestrians( void )
    {
        return MAX_PEDESTRIANS;
    }

    // NON-STATICS

    void Init(); // Initializes pedmanager itself; called from GamePlayContext::OnStart()
    void Update( unsigned int milliseconds ); // called once per frame by GamePlayContext::Update()

    void RemovePed( Character* character );
    void RemoveAllPeds();
    void AllowAddingPeds( bool allowed );

    void DumpAllPedModels(); // will remove all peds and dump their models (so revert all to "npd")

    ////////// EVENT LISTENER STUFF /////////////
    virtual void HandleEvent( EventEnum id, void* pEventData );
    /////////////////////////////////////////////

    // Makes all model groups default to:
    //   numModels = 4
    //   model 0 = male1, 2
    //   model 1 = fem1, 1
    //   model 2 = boy1, 1
    //   model 3 = girl1, 1
    // called from LoadingContext::OnStart()
    void InitDefaultModelGroups(); 

    void SetModelGroup( int groupID, const ModelGroup& group );

    bool IsPed( Character* character );

    void SwitchModelGroup( int toGroupID );

    static void SetDefaultModelGroup( int toGroupID );

// MEMBERS
private:
    static int mDefaultModelGroup;

    unsigned int mMillisecondsTillRemove;
    unsigned int mMillisecondsTillAdd;

    // all peds we'll ever use/need
    PedestrianStruct mPeds[ MAX_PEDESTRIANS ]; 

    unsigned int mMillisecondsPopulateWorld;

    struct Model
    {
        Model() : free( true ), maxCount( 0 ), currCount( 0 ) {};

        bool free;                   // whether or not this model slot has been registered
        char name[MAX_STRING_LEN+1]; // name of the model (duh)
        int maxCount;                // max ped instances of this model 
        int currCount;               // current num instances of this model 
        void InitZero()
        {
            free = true;
            name[0] = '\0';
            maxCount = 0;
            currCount = 0;
        }
        void Init( bool isFree, const char* modelName, int max, int curr )
        {
            rAssert( modelName != NULL );
            rAssert( max > 0 );
            rAssert( curr >= 0 );

            free = isFree;

            int nameLen = strlen( modelName );
            rAssert( nameLen <= PedestrianManager::MAX_STRING_LEN );
            strncpy( name, modelName, PedestrianManager::MAX_STRING_LEN );
            name[ nameLen ] = '\0';

            maxCount = max;
            currCount = curr;
        }
    };
    Model mModels[MAX_MODELS]; // array of currently registered models (of current group)
    int mNumRegisteredModels;  // number of currently registerd models

    ModelGroup mModelGroups[MAX_MODEL_GROUPS]; // groups of models for swapping
    int mCurrGroupID;

    struct UsedModel
    {
        UsedModel() : uid( 0 ), currCount( 0 ) {};
        tName uid;
        int currCount;
    };
    UsedModel mModelsInUse[ MAX_MODELS_IN_USE ];
    int mNumActiveModels;

    bool mAllowAdd;

// METHODS
private:
    PedestrianManager();
    virtual ~PedestrianManager();

    void AddPeds( rmt::Vector center, float addRadius );
    void DeactivatePed( PedestrianStruct* ped );
    bool ActivatePed( PathSegment* segment, rmt::Vector spawnPos );

    float GetRandomMinDistSqr();

    int GetRandomModel();
    bool SwapModels( int modelID, PedestrianStruct* pedStr );

    bool RegisterModel( const char* name, int maxCount );
    bool UnregisterModel( const char* name );
    bool UnregisterModel( int modelID );
    void UnregisterAllModels();
    int GetModelIDByName( const char* name );

    void FindModelInUseAndRemove( const char* name );
    bool FindModelInUseAndAdd( const char* name );

    void SanityCheck();
    void CheckModelCounts();

};

#endif //PEDESTRIANMANAGER_H
