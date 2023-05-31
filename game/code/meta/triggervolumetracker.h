//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     13/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef TRIGGERVOLUMETRACKER_H
#define TRIGGERVOLUMETRACKER_H

//========================================
// Nested Includes
//========================================
#include <raddebugwatch.hpp>
#include <p3d/inventory.hpp>

#include <meta/triggervolume.h>

#ifndef WORLD_BUILDER
#include <constants/maxplayers.h>
#include <events/eventlistener.h>
#include <meta/locatortypes.h>
#else
#include "../constants/maxplayers.h"
#include "../events/eventlistener.h"
#include "locatortypes.h"
#endif

#include <simcollision/collisionvolume.hpp> 

//========================================
// Forward References
//========================================
class Vehicle;

namespace sim
{
    enum CollisionVolumeTypeEnum;
}

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TriggerVolumeTracker : public EventListener
{
    public:
        // Static Methods for accessing this singleton.
        static void CreateInstance();
        static TriggerVolumeTracker* GetInstance();
        static void DestroyInstance();

        void Cleanup();

        void Update( int elapsedTime );

        void AddTrigger( TriggerVolume* triggerVolume );
        void RemoveTrigger( TriggerVolume* triggerVolume );
        void UnregisterFromInventory( tInventory* inv );

        virtual void HandleEvent( EventEnum id, void* pEventData );

        void Render();

        unsigned int GetNumTriggers() { return( mTriggerCount ); }
        TriggerVolume* GetTrigger( unsigned int index ) { return( mTriggerVolumes[ index ] ); }
        TriggerVolume* GetTriggerByName( const char* name );

        void RegisterAI( Vehicle* ai, int locatorTypes = ( 1 << LocatorType::EVENT ) );
        void UnregisterAI( Vehicle* ai );
        Vehicle* GetAI( int id );
        void RegisterLocatorTypeForAI( LocatorType::Type type, Vehicle* ai );
        void UnregisterLocatorTypeForAI( LocatorType::Type type, Vehicle* ai );

        void IgnoreTriggers( bool on );

        void ActivateNearestInteriorLoadZone(int iPlayerID, rmt::Vector& irPosnRef, float iMaxDistance);

        // MS9: yadda yadda
        void SetTriggerSphere( tDrawable* triggersphere );

        void ResetDynaloadZones();

#ifndef RAD_RELEASE
#ifndef WORLD_BUILDER
        pddiShader* GetMaterial(void);
#endif
#endif
        tDrawable* mpTriggerSphere;

    protected:
    private:
        TriggerVolumeTracker();
        virtual ~TriggerVolumeTracker();

        //Prevent wasteful constructor creation.
        TriggerVolumeTracker( const TriggerVolumeTracker& triggerVolumeTracker );
        TriggerVolumeTracker& operator=( const TriggerVolumeTracker& triggerVolumeTracker );

        // Pointer to the one and only instance of this singleton.
        static TriggerVolumeTracker* spInstance;

        void AddActive( int playerID, TriggerVolume* triggerVolume );
        void RemoveActive( int playerID, int index );
        void CheckForActiveVolumes( int playerID, rmt::Vector& center, float radius );
        void AddAIActive( int playerID, TriggerVolume* triggerVolume );
        void RemoveAIActive( int playerID, int index );

        void TestVolume( TriggerVolume* vol, rmt::Vector& center, float radius, int playerID, unsigned int time );

        enum 
        { 
            MAX_VOLUMES     = 500,
            MAX_ACTIVE      = 20,
            MAX_AI          = 10,
            MAX_AI_VOLUMES  = 100
        };
        
        unsigned int mTriggerCount;
        TriggerVolume* mTriggerVolumes[ MAX_VOLUMES ];

        unsigned int mActiveCount[ MAX_PLAYERS ];
        TriggerVolume* mActiveVolumes[ MAX_PLAYERS ][ MAX_ACTIVE ];

        unsigned int mAICount[ MAX_AI ];
        TriggerVolume* mActiveAIVolumes[ MAX_AI ][ MAX_AI_VOLUMES ];
        unsigned int mNumRegisteredAI;

        struct RegisteredAI
        {
            RegisteredAI() : mTriggerTypes( 0 ), mVehicle( NULL ) {};
            int mTriggerTypes;
            Vehicle* mVehicle;
        };

        RegisteredAI mRegisteredVehicles[ MAX_AI ];
    
        bool mIgnoreTriggers;

        static rmt::Vector sP1, sP2, sP3, sP4;
        static sim::CollisionVolumeTypeEnum sColType;
    
#ifndef RAD_RELEASE
        bool mDisplayEnable[ LocatorType::NUM_TYPES ];
        bool mDisplayAmbients;
        
        bool mDisplayActive;
        bool mDisplayInactive;
        bool mDisplayAll;

        pddiShader* material;
#endif
};

#ifndef WORLD_BUILDER
// A little syntactic sugar for getting at this singleton.
inline TriggerVolumeTracker* GetTriggerVolumeTracker() { return( TriggerVolumeTracker::GetInstance() ); }
#endif

//=============================================================================
// TriggerVolumeTracker::SetTriggerSphere
//=============================================================================
// Description: Comment
//
// Parameters:  ( tDrawable* triggersphere )
//
// Return:      void 
//
//=============================================================================
inline void TriggerVolumeTracker::SetTriggerSphere( tDrawable* triggersphere )
{
  //rAssert( triggersphere != NULL );
    mpTriggerSphere = triggersphere;
}

//=============================================================================
// TriggerVolumeTracker::IgnoreTriggers
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool on )
//
// Return:      void 
//
//=============================================================================
inline void TriggerVolumeTracker::IgnoreTriggers( bool on )
{
    mIgnoreTriggers = on;
}

#endif //TRIGGERVOLUMETRACKER_H

