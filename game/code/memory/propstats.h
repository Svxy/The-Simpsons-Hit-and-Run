//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Propstats
//
// Description: Singleton that logs prop memory usage
//
// Authors:     Michael Riegger
//
//===========================================================================

#ifndef RAD_RELEASE

// Recompilation protection flag.
#ifndef PROPSTATS_H
#define PROPSTATS_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <memory/srrmemory.h>
#include <memory/map.h>
#include <string>

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
//
//===========================================================================
class PropStats
{
    public:
        PropStats();
        ~PropStats();
   
        static void EnableTracking();

        static void StartTracking( const char* proptype );
        static void IncreaseInstanceCount( const char* proptype, int count );
        static void StopTracking( const char* proptype, int count = 0 );

        static void Print( void* userData );

    protected:

        static int s_MemInUseAtStart;
        static bool s_TrackingEnabled;
        static int GetMemAvailable();

        struct MemUsage
        {
            MemUsage() : numInstances( 0 ), memUsed( 0 ), memUsedInitialInstance( 0 ){}
            int numInstances;
            int memUsed;
            int memUsedInitialInstance;
        };

        typedef Map< std::string, MemUsage > PropMemMap;
        typedef PropMemMap::iterator PropMemMapIt;

        static PropMemMap s_PropMemData;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow PropStats from being copied and assigned.
        PropStats( const PropStats& );
        PropStats& operator=( const PropStats& );

};

#endif
#endif