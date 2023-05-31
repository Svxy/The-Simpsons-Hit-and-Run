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

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#ifndef RAD_RELEASE

#include <memory/propstats.h>
#include <sstream>
#include <raddebugwatch.hpp>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

static GameMemoryAllocator s_ListOfValidHeaps[] =
{
#ifdef RAD_GAMECUBE                                            
    GMA_GC_VMM,              
#endif                                                         
    GMA_LEVEL_ZONE,
};

static int s_NumHeapsToCheck = sizeof( s_ListOfValidHeaps )/sizeof( s_ListOfValidHeaps[0] );

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

bool PropStats::s_TrackingEnabled = false;
int PropStats::s_MemInUseAtStart = 0;
PropStats::PropMemMap PropStats::s_PropMemData;


//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// PropStats::PropStats
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

PropStats::PropStats()
{

}

PropStats::~PropStats()
{

}
  
void PropStats::EnableTracking()
{
    if ( s_TrackingEnabled == false )
    {
        s_TrackingEnabled = true;
        s_PropMemData.reserve( 50 );
        radDbgWatchAddFunction( "Dump PropStats", Print, NULL, "Memory" );
    }
}

void PropStats::StartTracking( const char* proptype )
{
    if ( s_TrackingEnabled )
    {
        // Check to see if we already have this prop in memory
        PropMemMapIt it = s_PropMemData.find( proptype );    
        if ( it == s_PropMemData.end() )
        {
            // Not present, push a new pair
            MemUsage usage;
            s_PropMemData.insert( proptype, usage );
        }
        s_MemInUseAtStart = GetMemAvailable();
    }
}

void PropStats::IncreaseInstanceCount( const char* proptype, int count )
{
    if ( s_TrackingEnabled )
    {
        PropMemMapIt it = s_PropMemData.find( proptype );    
        rAssert( it != s_PropMemData.end() );
        it->second.numInstances += count;
    }

}

void PropStats::StopTracking( const char* proptype, int count )
{
    if ( s_TrackingEnabled )
    {    
        int memInUseNow = GetMemAvailable();
        PropMemMapIt it = s_PropMemData.find( proptype );    
        rAssert( it != s_PropMemData.end() );
        if ( it->second.numInstances == 0 && count == 1 )
        {
        	it->second.memUsedInitialInstance = ( memInUseNow - s_MemInUseAtStart );
        } 
        it->second.numInstances += count;
        it->second.memUsed += ( memInUseNow - s_MemInUseAtStart );
    }
}

void PropStats::Print( void* userData )
{
    rTunePrintf( "--------------------------------------------------------\n" );

    float totalMemoryAllProps = 0;
    int totalNumAllProps = 0;

    PropMemMapIt it;
    for ( it = s_PropMemData.begin() ; it != s_PropMemData.end() ; it++ )
    {
        std::ostringstream stream;
        int count = it->second.numInstances;
        float initialCost = it->second.memUsedInitialInstance / 1024.0f;
        float totalKBUsed = static_cast< float >( it->second.memUsed ) / 1024.0f;
        float avgKBUsed = (totalKBUsed - initialCost ) / count;

        totalMemoryAllProps += totalKBUsed;
        totalNumAllProps += count;

        stream << it->first << ", " << count << " instances, Total: " << totalKBUsed << 
            " KB";

        if ( avgKBUsed > 0 )
        {
            stream << " Instance cost: " << avgKBUsed << " KB.";
        }

        if ( it->second.memUsedInitialInstance > 0 )
        {
        	stream << " Initial cost: " << initialCost << " KB.";
        }    
            
        stream << std::endl;
                        
            
        rTunePrintf( stream.str().c_str() );
    }
    rTunePrintf( "--------------------------------------------------------\n" );
        
    std::ostringstream stream;
    stream << "Total number of props : " << totalNumAllProps << std::endl;;
    stream << "Total number of kbytes used : " << totalMemoryAllProps << std::endl;;
    rTunePrintf( stream.str().c_str() );
    rTunePrintf( "--------------------------------------------------------\n" );
}

int PropStats::GetMemAvailable()
{
    int memcount = 0; 
    for ( int i = 0 ; i < s_NumHeapsToCheck ; i++ )
    {
       memcount += HeapMgr()->GetLoadedUsage( s_ListOfValidHeaps[i] ); 
    }
    return memcount;
}

#endif