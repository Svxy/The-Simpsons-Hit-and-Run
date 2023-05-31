//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Spawnpoint
//
// Description: Describes a place where actors can get spawned
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef SPAWNPOINT_H
#define SPAWNPOINT_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <meta/triggerlocator.h>
#include <render/culling/swaparray.h>
#include <events/eventlistener.h>

//===========================================================================
// Forward References
//===========================================================================

class Behaviour;
class TriggerVolume;


//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================



//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Represents a spawn point in the world, essentially a trigger volume
//      that when entered, spawns a new actor with the given properties
//
// Constraints:
//      Each spawn point will only ever spawn one actor at a time
//
//===========================================================================
class SpawnPoint : public TriggerLocator, public EventListener
{
    public:
        SpawnPoint( const char* spawnPointName, const char* statePropName, const rmt::Sphere& sphere, unsigned int timeOut );
        ~SpawnPoint();

        tUID GetUID()const { return m_SpawnPointName; }       
        void AddBehaviour( Behaviour* );

        virtual LocatorType::Type GetDataType() const { return LocatorType::SPAWN_POINT; };
    
        // Can the spawn point respawn a new actor? Or has there
        // not been enough time since the last respawn
        bool CanRespawn()const;

        virtual void HandleEvent( EventEnum id, void* pEventData );

    protected:

        rmt::Sphere m_Sphere;
        unsigned int m_SpawnTimeout;
        TriggerVolume* m_TriggerVolume;
        tUID m_StatePropName; 
        tUID m_SpawnPointName;

        unsigned int m_TimeActorDestroyed;
        unsigned int m_TimeOutPeriod;
        bool m_WasDestroyed;
        short mPersistentObjectID;

        virtual void OnTrigger( unsigned int playerID );

        // List of behaviours that can be attached to a spawned object
    

    private:

        SwapArray< Behaviour* > m_Behaviours;

};



#endif