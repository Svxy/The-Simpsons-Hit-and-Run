//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pickup.h
//
// Description: Pickup an object
//
// Author:     Michael Riegger
//
//=============================================================================

#ifndef PICKUPITEMOBJECTIVE_H
#define PICKUPITEMOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/missionobjective.h>


//========================================
// Forward References
//========================================

class StatePropCollectible;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class PickupItemObjective : public MissionObjective
{
public:
    PickupItemObjective();
    virtual ~PickupItemObjective();

    void SetTarget( const char* instancename );
    void SetLocation( const rmt::Matrix& transform );

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );
    virtual void HandleEvent( EventEnum id, void* pEventData );
    // Moves the target obj back to mItemStartPosition, called in OnInitialize
    void MoveObjectToStartPosition();

    tUID m_TargetUID;
    StatePropCollectible* mTargetObj;
    // If you lose the barrel, the mission should reset instantly
    // However, the barrel should play out its explosion, 
    // use this flag to indicate when the explosion is still playing, and
    // we are waiting on it to reset
    bool m_WaitingForExplosion;

private:

    //Prevent wasteful constructor creation.
    PickupItemObjective( const PickupItemObjective& objective );
    PickupItemObjective& operator=( const PickupItemObjective& objective );
};

#endif //DESTROYOBJECTIVE_H
