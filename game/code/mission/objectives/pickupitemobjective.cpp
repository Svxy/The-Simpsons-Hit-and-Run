//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   PickupItemObjective
//
// Description: PickupItemObjective
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission/objectives/PickupItemObjective.h>
#include <ai/actor/actor.h>
#include <ai/actor/actormanager.h>
#include <stateprop/statepropdata.hpp>
#include <mission/gameplaymanager.h>
#include <atc/atcmanager.h>
#include <mission/statepropcollectible.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/rendermanager/rendermanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/avatarmanager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const int MAX_NUM_COLLECTIBLE_STATEPROPS = 20;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================

PickupItemObjective::PickupItemObjective():
mTargetObj( NULL ),
m_WaitingForExplosion( false )
{

}

PickupItemObjective::~PickupItemObjective()
{
    if ( mTargetObj )
    {
        mTargetObj->RemoveFromDSG();
        mTargetObj->Release();
        mTargetObj = NULL;
    }
}

void
PickupItemObjective::SetTarget( const char* target )
{
    m_TargetUID = tName::MakeUID( target ); 
    // Find the object in the inventory
    mTargetObj = p3d::find< StatePropCollectible >( m_TargetUID );
    rTuneAssertMsg( mTargetObj != NULL, "PickupItemObjective - can't find item in inventory!" );
    mTargetObj->AddRef();
}


void
PickupItemObjective::OnInitialize()
{
    GetEventManager()->AddListener( this, EVENT_VEHICLE_COLLECTED_PROP );

    rAssert( mTargetObj != NULL );

    // Add it to the DSG, since the objective has just started
    if ( mTargetObj )
    {
        mTargetObj->EnableHudIcon( true );
        mTargetObj->EnableCollisionTesting( true );
    
        if ( mTargetObj->GetState() == 3 )
        {
            m_WaitingForExplosion = true;
        }
        else
        {
            MoveObjectToStartPosition();
        }
    }
}

void 
PickupItemObjective::OnFinalize()
{
    GetEventManager()->RemoveListener( this, EVENT_VEHICLE_COLLECTED_PROP );
}

void 
PickupItemObjective::OnUpdate( unsigned int elapsedTime )
{
    if ( m_WaitingForExplosion )
    {
        // The thing no longer exists - ergo, it blew up.
        if ( mTargetObj->IsInDSG() == false )
        {
            // Finally! We are done waiting for this to get reset
            // Rest it now.
            MoveObjectToStartPosition();
            m_WaitingForExplosion = false;
        }
    }   
}

void 
PickupItemObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_VEHICLE_COLLECTED_PROP:
        {
            Vehicle* vehicle = reinterpret_cast< Vehicle* >( pEventData );
            // We only care if the user is driving a vehicle, not if its AI
            if ( vehicle->IsUserDrivingCar() )
            {
                StatePropDSG* collectible = vehicle->GetAttachedCollectible();
                if ( mTargetObj != NULL && collectible == mTargetObj )
                {
                    SetFinished( true );
                }
            }
        }
        break;
    default:
        rTuneAssertMsg( 0, "PickupItemObjective - illegal event" );
        break;
    }
}


void 
PickupItemObjective::MoveObjectToStartPosition()
{
    // Is it attached to a vehicle?? We must detach it.
    int numActiveVehicles;
    Vehicle** activeVehicleList = NULL;
    GetVehicleCentral()->GetActiveVehicleList( activeVehicleList, numActiveVehicles );
    for ( int i = 0 ; i < numActiveVehicles ; i++ )
    {
        if ( activeVehicleList[i] != NULL )
        {
            Vehicle* v = activeVehicleList[i];
            if ( v->GetAttachedCollectible() == mTargetObj )
            {
                // This car is carrying our stateprop
                // Remove it from the vehicle
                v->DetachCollectible( rmt::Vector(0,0,0 ), false );
            }
        }
    }


    rAssert( mTargetObj != NULL );
    // Better make sure we dont try to move an object that isnt in the DSG
    // This function is safe to call even if it already is in the dsg
    mTargetObj->AddToDSG(); 
    // Move the object to the start position
    mTargetObj->SetTransform( mTargetObj->GetStartingPosition() );    
    // Reset state, from lord knows what to start state
    mTargetObj->SetState( 0 );
    mTargetObj->EnableCollisionVolume( true );
    ///mTargetObj->GetSimState()->SetControl( sim::simAICtrl );
}