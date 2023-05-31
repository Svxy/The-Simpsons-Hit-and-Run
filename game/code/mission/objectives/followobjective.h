//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        followobjective.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef FOLLOWOBJECTIVE_H
#define FOLLOWOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/missionobjective.h>

//========================================
// Forward References
//========================================

class Vehicle;
class Locator;
class AnimatedIcon;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FollowObjective : public MissionObjective
{
public:
    FollowObjective();
    virtual ~FollowObjective();

    Vehicle* GetTargetVehicle() { return( mFollowVehicle ); }
    void SetTargetVehicle(Vehicle* pVehicle) { mFollowVehicle = pVehicle; }

/*
    Locator* GetDestinationLocator() { return( mDestinationLocator ); }
    void SetDestinationLocator( Locator* pLocator ) { mDestinationLocator = pLocator; }
*/
    virtual void HandleEvent( EventEnum id, void* pEventData );

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );

private:
    Vehicle* mFollowVehicle;
    AnimatedIcon* mAnimatedIcon;
//    Locator* mDestinationLocator;
};

#endif //FOLLOWOBJECTIVE_H
