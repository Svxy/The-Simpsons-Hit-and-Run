//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   actormanager
//
// Description: Actormanager class, for holding stateprops that have AI
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ACTORMANAGER_H
#define ACTORMANAGER_H


//===========================================================================
// Nested Includes
//===========================================================================


#include <render/culling/swaparray.h>
#include <ai/actor/spawnpoint.h>
#include <events/eventlistener.h>
#include <memory/stlallocators.h>
#include <vector>

//===========================================================================
// Forward References
//===========================================================================

class Actor;
class ActorDSG;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//              Holds a list of actors and makes sure they get updated every frame
//
// Constraints:
//
//
//===========================================================================
class ActorManager : public EventListener
{
    public:
        ActorManager();
        ~ActorManager();
        
        // Adds an actor into the world
        void AddActor( Actor* );
        // Adds an actor into the bank. Basically a list of inactive actors
        // that can be brought up for use when the avatar enters a spawn point
        // or when an object fires a projectile
        void AddActorToBank( Actor* );

        Actor* GetActorByName( const char* name );
        Actor* GetActorByUID( tUID name );
        std::vector< Actor*, s2alloc< Actor* > > GetActorsByType( const char* typeName );


        void RemoveActor( int index, bool removeFromDSG = true );
        void RemoveActorByDSGPointer( ActorDSG* dsgObject, bool removeFromDSG = true );
        void RemoveAllActors();

        // Spawn point functions
        void AddSpawnPoint( SpawnPoint* );
        void FireProjectile( tUID typeName, const rmt::Vector& position, const rmt::Vector& direction, unsigned int id );
        SpawnPoint* GetSpawnPointByName( const char* name );
        void RemoveAllSpawnPoints();

        // Creates a new actor in the world
        Actor* CreateActor( tUID typeName, tUID instanceName, const rmt::Matrix& transform );


        // Update all actors
        void Update( unsigned int timeInMS );

        // Inherited from class EventListener
        virtual void HandleEvent( EventEnum id, void* pEventData );

        static ActorManager* CreateInstance();
        static ActorManager* GetInstance();
        static void DestroyInstance();

        static float ActorRemovalRangeSqr;

    protected:

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow ActorManager from being copied and assigned.
        ActorManager( const ActorManager& );
        ActorManager& operator=( const ActorManager& );

        void SetupConsoleFunctions();
        bool WithinAliveRange( int index );
        // Console functions
        static void AddFlyingActor( int argc, char** argv );
        static void AddFlyingActorByLocator( int argc, char** argv );
        static void AddBehaviour( int argc, char** argv );
        static void AddRespawnBehaviourPosition( int argc, char** argv );
        static void SetCollisionAttributes( int argc, char** argv );
        static void AddSpawnPointScript( int argc, char** argv );
        static void AddSpawnPointByLocatorScript( int argc, char** argv );
        static void SetProjectileStats( int argc, char** argv );
        static void PreallocateActors( int argc, char** argv ); 
        static void SetActorRotationSpeed( int argc, char** argv );
        static void AddShield( int argc, char** argv );
private:
        static ActorManager* sp_Instance;

        SwapArray< Actor* > m_ActorList;
        SwapArray< SpawnPoint* > m_SpawnPointList;

        // List of allocated but unused actors
        SwapArray< Actor* > m_ActorBank;
        // List of actors that will be removed at the end of an Update cycle
        // not a temporary variable because I want to get rid of dynamic allocations at runtime

        // List of actors that are flagged for removal. They will not be updated
        // and will not be available for reuse until next frame, when they will be moved into the
        // bank
        // The removequeue is equivalent to the breakablemanager::removequeue. When you want to
        // remove an object, but can't because it still inside the physics system
        SwapArray< Actor* > m_RemoveQueue;
};

inline ActorManager* GetActorManager()
{
    return ActorManager::GetInstance();
}


#endif