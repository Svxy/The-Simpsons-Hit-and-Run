//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   LoadVehicleObjective
//
// Description: Mission objective thats always winnable, wait for a car to dynamically
//              load
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission\objectives\LoadVehicleObjective.h>
#include <memory\srrmemory.h>
#include <mission\gameplaymanager.h>
#include <worldsim\vehiclecentral.h>
#include <meta\carstartlocator.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================

LoadVehicleObjective::LoadVehicleObjective()
{

}

LoadVehicleObjective::~LoadVehicleObjective()
{

}

void LoadVehicleObjective::OnInitialize()
{
    // Start loading
    strcpy( m_CallbackData.fileName, m_Filename );
    strcpy( m_CallbackData.vehicleName, m_VehicleName );
    strcpy( m_CallbackData.locatorName, m_LocatorName );

    m_CallbackData.objective = this;

    // We are going to dump the current vehicle and replace it with a new one
    GetGameplayManager()->DumpCurrentCar();
    GetGameplayManager()->CopyVehicleSlot( GameplayManager::eAICar, GameplayManager::eDefaultCar );
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, m_Filename, GMA_LEVEL_MISSION, m_Filename, NULL, &m_Callback, &m_CallbackData );        
}

void LoadVehicleObjective::OnFinalize()
{

}

void LoadVehicleObjective::LoadDisposableCarAsyncCallback::OnProcessRequestsComplete( void* pUserData )
{
    rAssert( pUserData != NULL );
    LoadVehicleObjective::CallbackData* callbackData = reinterpret_cast < CallbackData* >( pUserData );

    GetGameplayManager()->DumpCurrentCar();
    Vehicle* vehicle = GetVehicleCentral()->InitVehicle( callbackData->vehicleName, true, NULL, VT_USER );
    if ( vehicle )
    {
        // Set the vehicle position and orientation.   
        CarStartLocator* pLocator = p3d::find<CarStartLocator>( callbackData->locatorName );
        if ( pLocator )
        {
            rmt::Vector position;
            pLocator->GetLocation( &position );
            float facing = pLocator->GetRotation();
           
            vehicle->SetResetFacingInRadians( facing );          
            //
            // This will activate the above settings.
            //
            vehicle->Reset( false );
            GetGameplayManager()->SetCurrentVehicle(vehicle);
                       
            GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].mp_vehicle = vehicle;
            GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].mp_vehicle->AddRef();
        }
    }
    // Tell the objective that we are all done
    callbackData->objective->SetFinished( true );
}

/*
//=============================================================================
// MissionScriptLoader::LoadDisposableCarAsync
//=============================================================================
// Description: Use this to load any vehicle that may get unloaded during a level gameplay session,
// ie player car,ai car, forced car into its own iventory section.
//
// Parameters:  ( int argc, char** argv )
//
// Return:      void 
//
//=============================================================================

/// VERY TEMPORARY CALLBACK for async vehicle loading


void MissionScriptLoader::LoadDisposableCarAsync( int argc, char** argv )
{
    const char* fileName = argv[1];
    const char* vehicleName = argv[2];
    const char* vehicleType = argv[3];

    if ( strcmp( vehicleType, "DEFAULT" ) == 0 )
    {
        rAssertMsg( 0, "Can't async load player vehicles other than the default!!");
        return;        
    }
    // We are trying to load a player vehicle ingame
    // first we must dump the player's vehicle
    // then set the player vehicle to be a temporary traffic vehicle.
    // this is just a placeholder while the new car is loaded async
    // Setup a callback that gets triggered on 
      
//    strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].name, name);
//    strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].filename, filename);

    GetGameplayManager()->DumpCurrentCar();

    //  GetGameplayManager()->ClearVehicleSlot(GameplayManager::eDefaultCar);
    //  GetVehicleCentral()->InitVehicle( vehicleName, 

    // Create a temp callback, this is just for testing purposes
}*/
