//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   vehiclecarryingstateprop
//
// Description: The vehicle must be carrying a statepropcollectible object
//              as a condition for mission success
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef VEHICLECARRYINGSTATEPROP_H
#define VEHICLECARRYINGSTATEPROP_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <mission\conditions\missioncondition.h>

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
//
// Constraints:
//
//===========================================================================
class VehicleCarryingStateProp : public MissionCondition
{
    public:
        VehicleCarryingStateProp();
        virtual ~VehicleCarryingStateProp();

        virtual void Update( unsigned int elapsedTime );
        virtual void HandleEvent( EventEnum id, void* pEventData );
        
        // Sets the prop name that the vehicle must be carrying for condition success
        void SetDesiredProp( const char* name )
        {
            m_PropName = tName::MakeUID( name );
        }

    protected:
        virtual void OnInitialize();
        virtual void OnFinalize();

        tUID m_PropName;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow VehicleCarryingStateProp from being copied and assigned.
        VehicleCarryingStateProp( const VehicleCarryingStateProp& );
        VehicleCarryingStateProp& operator=( const VehicleCarryingStateProp& );

};



#endif