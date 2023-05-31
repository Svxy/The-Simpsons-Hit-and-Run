//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        chasemanager.h
//
// Description: ChaseManager Class declaration.
//
// History:     11/5/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#ifndef CHASEMANAGER_H
#define CHASEMANAGER_H

#include <worldsim/spawn/spawnmanager.h>
#include <events/eventlistener.h>

class Vehicle;
class VehicleAI;

static const float CHASE_SPAWN_RADIUS = 100.0f;
static const float CHASE_REMOVE_RADIUS = 110.0f;
static const float SECONDS_BETW_CHASE_ADDS = 0.7f;
static const float SECONDS_BETW_CHASE_REMOVES = 0.5f;
static const float SECONDS_BETW_CHASE_UPDATES = 0.0f;

class ChaseManager
: public SpawnManager, EventListener    
{
public:

    static const int MAX_CHASE_VEHICLES = 5;
    static const int MAX_MODELS = 1;
	static const int MAX_STRING_LEN = 64;

    // Dusit [Nov 4,2002]:
    // Don't make ChaseManager static. Make it possible for there
    // to be 1 ChaseManager to spawn the 1 cSedan that needs to always 
    // be present, and another ChaseManager for the other set of chase cars.
    ///////////////////////////////////////////////////////////////
    // Statics
    //static ChaseManager* GetInstance();
    //static ChaseManager* DestroyInstance();

    ChaseManager();
    virtual ~ChaseManager();

    ///////////////////////////////////////////////////////////////
    // Implementing SpawnManager Stuff
    void Init();
    void ClearAllObjects();
    void ClearObjectsInsideRadius( rmt::Vector center, float radius );
    void ClearObjectsOutsideRadius( rmt::Vector center, float radius );
    int GetAbsoluteMaxObjects() const;
    int GetMaxObjects() const;
    void SetMaxObjects( int maxObjects );
    int GetMaxModels() const;
    int GetNumRegisteredModels() const;
    bool RegisterModel( const char* name, int spawnFreq );
    bool IsModelRegistered( const char* name );
    bool UnregisterModel( const char* name );
	void SetConfileName( const char* name );
    ///////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////
    // EventListener
    void HandleEvent( EventEnum id, void* pEventData );
    ///////////////////////////////////////////////////////////////

    bool IsChaseVehicle( Vehicle* v );
    void DisableAllActiveVehicleAIs();
    void EnableAllActiveVehicleAIs();
    void DeactivateAllVehicles();
    int GetNumActiveVehicles();
    void MarkAllVehiclesForDeletion();

    void SuspendAllVehicles();
    void ResumeAllVehicles();

    float GetClosestCarPosition(rmt::Vector* toPos, rmt::Vector* carPos);

protected:
    ///////////////////////////////////////////////////////////////
    // Implementing SpawnManager Stuff
    void AddObjects( float seconds );
    void RemoveObjects( float seconds );
    void UpdateObjects( float seconds );
    float GetSecondsBetwAdds() const;
    float GetSecondsBetwRemoves() const;
    float GetSecondsBetwUpdates() const;


private:

    //static ChaseManager* spChaseManager;

    struct Model
    {
        char name[MAX_STRING_LEN+1];
        int spawnFreq;
    };
    Model mModels[MAX_MODELS];
    int mNumRegisteredModels;
    int mTotalSpawnFrequencies;

    struct ChaseVehicle
    {
        Vehicle* v;
        Vehicle* husk;
        VehicleAI* vAI;
        //int activeListIndex;
        bool isActive;
        bool isOutOfSight;
        bool markedForDeletion;
        float secondsOutOfSight;
    };

    int mMaxVehicles; // betw 0 and AbsoluteMax 

    ChaseVehicle mVehicles[MAX_CHASE_VEHICLES];

	char mConfileName[MAX_STRING_LEN+1];

    void ClearOutOfSightVehicles();

    void DeactivateVehicle( ChaseVehicle* cv );
    bool ActivateVehicle( ChaseVehicle* cv );

    ChaseVehicle* FindChaseVehicle( Vehicle* v );

    ChaseVehicle* GetInactiveVehicle();
    bool SpawningOnTopOfAnotherVehicle( const rmt::Sphere& s );
};

////////////////////////////// INLINES /////////////////////////////////

// etc, etc, etc...
inline int ChaseManager::GetAbsoluteMaxObjects() const
{
    rAssert( MAX_CHASE_VEHICLES >= 0 );
    return MAX_CHASE_VEHICLES;
}
inline int ChaseManager::GetMaxObjects() const
{
    return mMaxVehicles;
}
inline void ChaseManager::SetMaxObjects( int maxObjects )
{
    int absoluteMax = GetAbsoluteMaxObjects();
    rAssert( absoluteMax >= 0 );

    if( maxObjects < 0 )
    {
        maxObjects = 0;
    }
    else if( maxObjects > absoluteMax )
    {
        maxObjects = absoluteMax;
    }

    mMaxVehicles = maxObjects;
}

inline int ChaseManager::GetMaxModels() const
{
    return MAX_MODELS;
}
inline int ChaseManager::GetNumRegisteredModels() const
{
    return mNumRegisteredModels;
}
inline float ChaseManager::GetSecondsBetwAdds() const
{
    rAssert( SECONDS_BETW_CHASE_ADDS >= 0.0f );
    return SECONDS_BETW_CHASE_ADDS;
}
inline float ChaseManager::GetSecondsBetwRemoves() const
{
    rAssert( SECONDS_BETW_CHASE_REMOVES >= 0.0f );
    return SECONDS_BETW_CHASE_REMOVES;
}
inline float ChaseManager::GetSecondsBetwUpdates() const
{
    rAssert( SECONDS_BETW_CHASE_UPDATES >= 0.0f );
    return SECONDS_BETW_CHASE_UPDATES;
}

#endif