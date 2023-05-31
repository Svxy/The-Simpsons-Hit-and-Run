//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        destroyobjective.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef DESTROYOBJECTIVE_H
#define DESTROYOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/missionobjective.h>

//========================================
// Forward References
//========================================

class Vehicle;
class AnimatedIcon;


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class DestroyObjective : public MissionObjective
{
public:
    DestroyObjective();
    virtual ~DestroyObjective();

    Vehicle* GetTargetVehicle() { return( mDestroyVehicle ); }
    void SetTargetVehicle(Vehicle* pVehicle);

    virtual void HandleEvent( EventEnum id, void* pEventData );

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );

private:
    Vehicle* mDestroyVehicle;
    AnimatedIcon* mAnimatedIcon;

    //Prevent wasteful constructor creation.
    DestroyObjective( const DestroyObjective& objective );
    DestroyObjective& operator=( const DestroyObjective& objective );
};

#endif //DESTROYOBJECTIVE_H
