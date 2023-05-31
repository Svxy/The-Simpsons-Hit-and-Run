//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicleairender.h
//
// Description: Blahblahblah
//
// History:     04/02/2003 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#ifndef VEHICLEAIRENDER_H
#define VEHICLEAIRENDER_H

#include <roads/geometry.h>

#include <p3d/drawable.hpp>

class VehicleAIRender
{
public:
    static VehicleAIRender* GetVehicleAIRender();
    static void Destroy();

    enum 
    {
        TYPE_VEHICLE_AI,
        TYPE_TRAFFIC_AI,
        NUM_TYPES
    };

    void Display();
    int RegisterAI( void* ai, int type=TYPE_VEHICLE_AI );
    void UnregisterAI( int handle );

private:
    static VehicleAIRender* spInstance;
    static const int MAX_REGISTRATIONS = 20;

    struct RegisteredAI
    {
        void* ai;
        int index;
        int type;
        bool show;  //not a bitfield because of watcher
    };
    RegisteredAI mAIs[MAX_REGISTRATIONS];
    DListArray mFreeSlots;
    DListArray mRegistered;

    tShader* mAIShader;

    static void DropPathPoint(void*);

    // Singletons don't expose these...
    VehicleAIRender();
    virtual ~VehicleAIRender();

    //Prevent wasteful constructor creation.
    VehicleAIRender( const VehicleAIRender& render );
    VehicleAIRender& operator=( const VehicleAIRender& render );
};

#endif //VEHICLEAIRENDER_H