//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        groundplanepool.h
//
// Description: manage pool of ground planes for dynamic physics objects
//
// History:     July 31, 2002 - created, gmayer
//
//=============================================================================

#ifndef GROUNDPLANEPOOL_H
#define GROUNDPLANEPOOL_H

//========================================
// Nested Includes
//========================================

#include <simcommon/simstate.hpp>

//========================================
// Forward References
//========================================


// note to self: maybe vehicles should use this?

class GroundPlanePool
{
public:

    GroundPlanePool(int num);   // how big the pool should be
    ~GroundPlanePool();
    
    //sim::ManualSimState* GetNewGroundPlane();
    
    //int GetNewGroundPlane();    // user refers to it with the returned index
    
    int GetNewGroundPlane(sim::SimState* simStateOwner);
    
    
    void UpdateGroundPlane(int index, rmt::Vector& position, rmt::Vector& normal);
    void FreeGroundPlane(int index);
    bool FreeAllGroundPlanes(); // returns false if there was a problem...

    sim::ManualSimState* GetSimState(int index);
    //sim::SimState* GetSimState(int index);
    
    void EnableCollision(int index);
    void DisableCollision(int index);


private:

    int mTotalNum;
    sim::ManualSimState** mPool;
    //sim::SimState** mPool;

    bool* mInUse;
    
    
    sim::SimState** mSimStateOwners;

    // to save memory, and potentially have more control over the values
    sim::PhysicsProperties* mGroundPlanePhysicsProperties;
            

};

         
#endif //GROUNDPLANEPOOL_H
