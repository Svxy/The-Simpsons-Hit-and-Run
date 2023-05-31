//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        interiorobjective.h
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef INTERIOROBJECTIVE_H
#define INTERIOROBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/objectives/missionobjective.h>
#include <presentation/gui/utility/hudmap.h>

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

class InteriorObjective : public MissionObjective, public IHudMapIconLocator
{
public:
    InteriorObjective();
    virtual ~InteriorObjective();

    virtual void HandleEvent( EventEnum id, void* pEventData );
    
    //void SetTargetVehicle(Vehicle* pVehicle) { mInteriorVehicle = pVehicle; };

    //Interface for IHudMapIconLocator
    void GetPosition( rmt::Vector* currentLoc );
    void GetHeading( rmt::Vector* heading );

    void SetDestination(const char*);
    void SetIcon(const char*);

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );

    PathStruct mArrowPath;

private:
    int mHUDID;
    AnimatedIcon* mAnimatedIcon;
    char mDestination[64];
    char mIcon[64];
    rmt::Vector mPosition;

    //Prevent wasteful constructor creation.
    InteriorObjective( const InteriorObjective& Interiorobjective );
    InteriorObjective& operator=( const InteriorObjective& Interiorobjective );
};


#endif //GETINOBJECTIVE_H
