//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   
//
// Description: 
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission\conditions\vehiclecarryingstateprop.h>
#include <worldsim\redbrick\vehicle.h>
#include <events/eventmanager.h>
#include <mission/statepropcollectible.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================

VehicleCarryingStateProp::VehicleCarryingStateProp()
{
    SetIsViolated( true );
}

VehicleCarryingStateProp::~VehicleCarryingStateProp()
{

}

// the Mission Stage should call this every frame
void VehicleCarryingStateProp::Update( unsigned int elapsedTime )
{

}

void VehicleCarryingStateProp::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_VEHICLE_COLLECTED_PROP:
        {
            // A vehicle has picked up a collectible,
            // check that the vehicle is the users car, and that
            // the object that was collected is the same
            // type that is associated with success
            Vehicle* vehicle = reinterpret_cast< Vehicle* >(pEventData);
            if ( vehicle->IsUserDrivingCar() )
            {
                StatePropCollectible* collectible = vehicle->GetAttachedCollectible();
                if ( collectible != NULL )
                {                   
                    if ( collectible->GetUID() == m_PropName )
                    {
                        SetIsViolated( false );
                    }
                    else
                    {
                        SetIsViolated( true );
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    MissionCondition::HandleEvent( id, pEventData );
}

void VehicleCarryingStateProp::OnInitialize()
{
    GetEventManager()->AddListener( this, EVENT_VEHICLE_COLLECTED_PROP );
}

void VehicleCarryingStateProp::OnFinalize()
{
    GetEventManager()->RemoveListener( this, EVENT_VEHICLE_COLLECTED_PROP );
}
