//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// leakdetection.hpp
//
// Description: A stimple allocation counter for leak detection
//
// Modification History:
//
//  + Created Sept 10, 2002 Robert Sparks
//-----------------------------------------------------------------------------

#ifndef LEAK_DETECTION_DISABLE

//-----------------------------------------------------------------------------
// Included Files
//-----------------------------------------------------------------------------
#ifdef WORLD_BUILDER
#include "main/toolhack.h"
#pragma warning( disable:4786 )
#endif

#include "main/commandlineoptions.h"
#include <map>
#include "memory/leakdetection.h"
#include "memory/srrmemory.h"
#include "memory/stlallocators.h"
#include "p3d/utility.hpp"
#include <radtime.hpp>


//-----------------------------------------------------------------------------
// Local Declarations
//-----------------------------------------------------------------------------

struct MemoryInfo
{
    void*        m_Address;
    unsigned int m_Size;
}; 

typedef std::map< const void*, unsigned int, std::less<const void*>, s2alloc< std::pair<const void* const, unsigned int> > > VOIDINTMAP;
static VOIDINTMAP g_MemoryMap;
static bool            g_MemoryAllocationEnabled = false;
static unsigned int    g_TrapIndex =  0xFFFFFFFF;


//-----------------------------------------------------------------------------
// D o W e T r a c k L e a k s I n T h i s H e a p
//
// Synopsis:    Some heaps are on their own as far as leak detection goes. 
//              Sound and Temp for example
//
// Parameters:  heap - do we care about this heap?
//
// Returns:     bool - well do we?
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
bool DoWeTrackLeaksInThisHeap( radMemoryAllocator heap )
{
    if( heap == GMA_TEMP )              return false;
    if( heap == GMA_PERSISTENT )        return false;
    if( heap == GMA_MUSIC )             return false;
    if( heap == GMA_AUDIO_PERSISTENT )  return false;
#ifdef RAD_XBOX
    if( heap == GMA_XBOX_SOUND_MEMORY ) return false;
#endif
    if( heap == GMA_DEBUG )             return false;
    return true;
}

//-----------------------------------------------------------------------------
// L e a k D e t e c t i  o n S t a r t 
//
// Synopsis:    Begins the tracking of allocations, signifies the beginnig of a section
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------

void LeakDetectionStart( void )
{
    bool detectLeaks = CommandLineOptions::Get( CLO_DETECT_LEAKS );    
    if( detectLeaks )
    {
        g_MemoryAllocationEnabled = false;
        g_MemoryMap.clear();
        g_MemoryAllocationEnabled = true;
    }
}

//-----------------------------------------------------------------------------
// L e a k D e t e c t i o n S t o p 
//
// Synopsis:    Ends the tracking of allocation, signifies the end of a section.
//              Will print out a warning or will assert if a leak is detected
//
// Parameters:  nON
//
// Returns:     NOTHING
//
// Constraints: 
//
//-----------------------------------------------------------------------------

void LeakDetectionStop( void )
{

    if( !g_MemoryAllocationEnabled )
    {
        return;
    }
    
    int totalNumberOfLeaks = g_MemoryMap.size();

    bool printLeakResults = CommandLineOptions::Get( CLO_DETECT_LEAKS );
    if( printLeakResults )
    {
        //
        // Print out leaks
        //
        rReleasePrintf( "*****************************************************\n");
        rReleasePrintf( "************** LEAK DETECTION RESULTS ***************\n");
        rReleasePrintf( "** Total Leaked Memory Blocks = %d\n", totalNumberOfLeaks );
        VOIDINTMAP::iterator it;
        for(  it = g_MemoryMap.begin(); it != g_MemoryMap.end(); ++it )
        {
            const void* address = ( *it ).first;
            unsigned int size = ( *it ).second;

            if( address != NULL )
            {
                rReleasePrintf( "** Leak at address 0x%x, size 0x%x, Index = %d\n", address, size );
            }
        }

        //
        // Display number of leaks to the screen
        //
        if( totalNumberOfLeaks > 0 )
        {
            char buf[ 256 ];
            sprintf( buf, "MEMORY LEAKED: [%d] blocks\n", totalNumberOfLeaks );

            p3d::pddi->DrawString( buf, 256, (12 * 15), pddiColour( 255, 128, 0 ) );

            unsigned int nowTime = ::radTimeGetSeconds( );

            while( nowTime + 5 > ::radTimeGetSeconds( ) )
            {
                p3d::display->SwapBuffers();
            }
            radMemoryMonitorSuspend();
        }
        rReleasePrintf( "*****************************************************\n");
    }


    g_MemoryAllocationEnabled = false;
    g_MemoryMap.clear();
}

//-----------------------------------------------------------------------------
// L e a k D e t e c t i o n A d d  R e c o r d 
//
// Synopsis:    Reports that an allocation has happened
//
// Parameters:  the allocation address and sizeo
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------

void LeakDetectionAddRecord( const void* pMemory, const unsigned int size, const radMemoryAllocator heap )
{
    if( !g_MemoryAllocationEnabled )
    {
        return;
    }

    bool doWeTrackLeaksInThisHeap = DoWeTrackLeaksInThisHeap( heap );
    if( !doWeTrackLeaksInThisHeap )
    {
        return;
    }
    //
    // Is this address already there? It shouldn't be
    //
    HeapMgr()->PushHeap( GMA_DEBUG );
    VOIDINTMAP::iterator found = g_MemoryMap.find( pMemory );
    rAssert( found == g_MemoryMap.end() );
    g_MemoryAllocationEnabled = false;
    g_MemoryMap[ pMemory ] = size;
    g_MemoryAllocationEnabled = true;
    HeapMgr()->PopHeap( GMA_DEBUG );
}

//-----------------------------------------------------------------------------
// L e a k D e t e c t i o n R e m o v e  R e c o r d
//
// Synopsis:    Reports that an allocation has been freed
//
// Parameters:  the address of the freed allocation
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------

void LeakDetectionRemoveRecord( void* pMemory )
{   
    if( g_MemoryAllocationEnabled && ( pMemory != NULL ) )
    {
        VOIDINTMAP::iterator found = g_MemoryMap.find( pMemory );
        if( found == g_MemoryMap.end() )
        {
            //
            // IAN - put this in later - it means that we're freeing stuff that came from permanent memory
            //
            //rReleasePrintf( "LeakDetection: Untracked memory block at %x freed\n", pMemory);
        }
        else
        {
            g_MemoryMap.erase( found );
        }
    }
}                               

#endif // FINAL