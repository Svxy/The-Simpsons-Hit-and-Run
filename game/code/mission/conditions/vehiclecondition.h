//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclecondition.h
//
// Description: Blahblahblah
//
// History:     08/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef VEHICLECONDITION_H
#define VEHICLECONDITION_H

//========================================
// Nested Includes
//========================================

#include <mission/conditions/missioncondition.h>

//========================================
// Forward References
//========================================

class Vehicle;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class VehicleCondition : public MissionCondition
{
public:
    VehicleCondition();
    virtual ~VehicleCondition();

    Vehicle* GetVehicle();
    void SetVehicle( Vehicle* vehicle );

protected:
    virtual void OnInitialize() {};
    virtual void OnFinalize() {};

private:

    //Prevent wasteful constructor creation.
    VehicleCondition( const VehicleCondition& vehiclecondition );
    VehicleCondition& operator=( const VehicleCondition& vehiclecondition );

    Vehicle* mpVehicle;
};

//=============================================================================
// VehicleCondition::SetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle )
//
// Return:      inline 
//
//=============================================================================
inline void VehicleCondition::SetVehicle( Vehicle* vehicle )
{
    mpVehicle = vehicle;
}

//=============================================================================
// VehicleCondition::GetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline Vehicle* VehicleCondition::GetVehicle()
{
    return mpVehicle;
}

#endif //VEHICLECONDITION_H
