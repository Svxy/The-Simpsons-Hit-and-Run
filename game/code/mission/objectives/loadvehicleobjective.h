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

// Recompilation protection flag.
#ifndef LoadVehicleObjective_H
#define LoadVehicleObjective_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <mission/objectives/missionobjective.h>
#include <loading/loadingmanager.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Generally, describe what behaviour this class possesses that
//      clients can rely on, and the actions that this service
//      guarantees to clients.
//
// Constraints:
//      Describe here what you require of the client which uses or
//      has this class - essentially, the converse of the description
//      above. A constraint is an expression of some semantic
//      condition that must be preserved, an invariant of a class or
//      relationship that must be preserved while the system is in a
//      steady state.
//
//===========================================================================
class LoadVehicleObjective : public MissionObjective
{
    public:
        LoadVehicleObjective();
        virtual ~LoadVehicleObjective();

        void SetFilename( const char* filename ) { strcpy( m_Filename, filename ); }
        void SetVehicleName( const char* vehicleName ) { strcpy( m_VehicleName, vehicleName ); }
        void SetLocatorName( const char* locatorName ) { strcpy( m_LocatorName, locatorName ); }

    protected:
        
        virtual void OnInitialize();
        virtual void OnFinalize();

        struct CallbackData
        {
            char vehicleName[256];
            char fileName[256];
            char locatorName[256];
            LoadVehicleObjective* objective;
        };

        class LoadDisposableCarAsyncCallback : public LoadingManager::ProcessRequestsCallback
        {
        public:
            LoadDisposableCarAsyncCallback( void ){}
            ~LoadDisposableCarAsyncCallback( void ){}
            void OnProcessRequestsComplete( void* pUserData );
        };

        char m_VehicleName[256];
        char m_Filename[256];
        char m_LocatorName[256];
        LoadDisposableCarAsyncCallback m_Callback;
        CallbackData m_CallbackData;


    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow LoadVehicleObjective from being copied and assigned.
        LoadVehicleObjective( const LoadVehicleObjective& );
        LoadVehicleObjective& operator=( const LoadVehicleObjective& );

        friend class LoadDisposableCarAsyncCallback;
};

#endif