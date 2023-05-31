//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        huskpool.h
//
// Description: 
//
// History:     Mar 27, 2003 - created, gmayer
//
//=============================================================================

#ifndef HUSKPOOL_H
#define HUSKPOOL_H



#include <worldsim/redbrick/vehicle.h>

// vehicle central can own this
class HuskPool
{
public:

    HuskPool();   
    ~HuskPool();
    
    void Init(int num); // how big the pool should be - the max number of simultaneous husks we ever want to have
    void Empty();       // empty lists - for the OnStop..
    
    Vehicle* RequestHusk( VehicleType vt, Vehicle* originalVehicle );
    void FreeHusk( Vehicle* husk );

    Vehicle* FindOriginalVehicleGivenHusk( Vehicle* husk );
    Vehicle* FindHuskGivenOriginalVehicle( Vehicle* v );

    bool IsHuskType( VehicleEnum::VehicleID id );

private:
    bool WillMakeConvincingHusk( Vehicle* origV );

private:
    int mTotalNum;

    struct HuskData
    {
        Vehicle* originalVehicle;
        Vehicle* huskVehicle;
        bool inUse;
    };

    HuskData* mHuskArray;

};

         
#endif // HUSKPOOL_H
