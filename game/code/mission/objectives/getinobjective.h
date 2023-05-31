//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        getinobjective.h
//
// Description: Blahblahblah
//
// History:     09/09/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef GETINOBJECTIVE_H
#define GETINOBJECTIVE_H

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
class tName;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class GetInObjective : public MissionObjective, public IHudMapIconLocator
{
public:
    GetInObjective();
    virtual ~GetInObjective();
    
    //void SetTargetVehicle(Vehicle* pVehicle) { mGetInVehicle = pVehicle; };

    void SetStrict( const char* name );

    //Interface for IHudMapIconLocator
    void GetPosition( rmt::Vector* currentLoc );
    void GetHeading( rmt::Vector* heading );

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );
    
private:
    int mHUDID;
    AnimatedIcon* mAnimatedIcon;
    bool mStrict;
    tUID mVehicleUID;

    //Prevent wasteful constructor creation.
    GetInObjective( const GetInObjective& getinobjective );
    GetInObjective& operator=( const GetInObjective& getinobjective );
};


#endif //GETINOBJECTIVE_H
