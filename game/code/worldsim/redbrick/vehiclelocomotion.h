/*===========================================================================
   vehiclelocomotion.h

   created April 24, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.


===========================================================================*/

#ifndef _VEHICLELOCOMOTION_H
#define _VEHICLELOCOMOTION_H

#include <radmath/radmath.hpp>

class Vehicle;

class VehicleLocomotion
{
public:

    VehicleLocomotion(Vehicle* vehicle) {};
    virtual ~VehicleLocomotion() {};

    virtual void PreSubstepUpdate() = 0;

    virtual void PreCollisionPrep(bool firstSubstep) = 0;
    virtual void UpdateVehicleGroundPlane() = 0;

    virtual void PreUpdate() = 0;
    virtual void Update(float dt) = 0;
    virtual void PostUpdate() = 0;

    virtual void CompareNormalAndHeight(int index, rmt::Vector& normalPointingAtCar, rmt::Vector& groundContactPoint) = 0;
    
};


#endif // _VEHICLELOCOMOTION_H
