//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        loseobjective.h
//
// Description: Blahblahblah
//
// History:     24/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef LOSEOBJECTIVE_H
#define LOSEOBJECTIVE_H

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

class LoseObjective : public MissionObjective
{
public:
    LoseObjective();
    virtual ~LoseObjective();

    Vehicle* GetTargetVehicle() { return mTargetVehicle; }
    void SetTargetVehicle(Vehicle* pVehicle) { mTargetVehicle = pVehicle; }

    void SetDistance( float dist ) { mDistance = dist; }
    float GetDistance() { return mDistance; }

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );

private:
    float mDistance;
    Vehicle* mTargetVehicle;
    AnimatedIcon* mAnimatedIcon;

    //Prevent wasteful constructor creation.
    LoseObjective( const LoseObjective& loseobjective );
    LoseObjective& operator=( const LoseObjective& loseobjective );
};


#endif //LOSEOBJECTIVE_H
