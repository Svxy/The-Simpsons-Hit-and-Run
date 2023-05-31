//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicleeventlistener.h
//
// Description: 
//
// History:     created, Nov 29, 2002, gmayer       
//
//=============================================================================



#ifndef VEHICLEEVENTLISTENER_H
#define VEHICLEEVENTLISTENER_H

#include <events/eventlistener.h>

class Vehicle;

class VehicleEventListener : public EventListener
{
public:
    
    VehicleEventListener();
    VehicleEventListener(Vehicle* owner);

    virtual ~VehicleEventListener();


    virtual void HandleEvent( EventEnum id, void* pEventData );

private:
        
    // Declared but not defined to prevent copying and assignment.
    VehicleEventListener( const VehicleEventListener& );
    VehicleEventListener& operator=( const VehicleEventListener& );

    Vehicle* mVehicleOwner;
};




#endif // VEHICLEEVENTLISTENER_H
