//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        collectdumpedobjective.h
//
// Description: Blahblahblah
//
// History:     1/7/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef COLLECTDUMPEDOBJECTIVE_H
#define COLLECTDUMPEDOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/collectibleobjective.h>

#include <events/eventlistener.h>

#include <ai/vehicle/waypointai.h>

//========================================
// Forward References
//========================================
class Locator;
class Vehicle;
class AnimatedIcon;
class CollectionCondition;


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class CollectDumpedObjective : public CollectibleObjective
{
public:
        CollectDumpedObjective();
        virtual ~CollectDumpedObjective();

        void BindCollectibleToWaypoint( int collectibleNum, unsigned int waypointNum );
        void SetDumpVehicle( Vehicle* vehicle );
        Vehicle* GetDumpVehicle() {return mDumpVehicle;}

        bool IsBumperCars() {return mBumperCars;}

        virtual void HandleEvent( EventEnum id, void* pEventData );

protected:
    virtual void OnInitCollectibles();
    virtual void OnInitCollectibleObjective();
    virtual void OnFinalizeCollectibleObjective();
    virtual bool OnCollection( unsigned int collectibleNum, bool &shouldReset );
    virtual void OnUpdateCollectibleObjective( unsigned int elapsedTimeMilliseconds );

    int FindFreeSlot(void);

    CollectionCondition* mCondition;

private:

    Vehicle* mDumpVehicle;
    WaypointAI* mDumpVehicleAI;

    enum { MAX_ICON_NAME = 32 };

    unsigned int mNumUncollected;

    struct DumpData
    {
        DumpData() : collectibleNum( -1 ), active( false ), collected( false ), lifetime(0) {};
        int collectibleNum;
        bool active;
        bool collected;
        int lifetime;
    };

    DumpData mDumpData[ MAX_COLLECTIBLES ];

    bool mBumperCars;
    unsigned int mNumSpawned;

    int mDumpTimeout;
    unsigned int mWhatToDump;
    bool mAmIDumping;
    Vehicle* mAssaultCar;
    unsigned int mMeDumpTimout;
    unsigned int mDumpLifetime;
    bool mTerminalDump;

    AnimatedIcon* mAnimatedIcon;

    //Prevent wasteful constructor creation.
    CollectDumpedObjective( const CollectDumpedObjective& collectdumpedobjective );
    CollectDumpedObjective& operator=( const CollectDumpedObjective& collectdumpedobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //COLLECTDUMPEDOBJECTIVE_H
