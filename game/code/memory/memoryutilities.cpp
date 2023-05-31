//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        memoryutilities.h
//
// Description: some funcitons for checking the amount of free memory, etc
//
// History:     2002/12/03 + Created -- Ian Gipson
//
//=============================================================================

//========================================
// System Includes
//========================================

#ifdef _WIN32
    #include <crtdbg.h>
    #ifndef _XBOX
        #include <windows.h>
    #endif
#endif


#ifdef _XBOX
    #include <typeinfo.h>
    #include <xtl.h>
#endif // XBOX

#ifdef RAD_PS2
    #include <malloc.h>
    #include <typeinfo>
#endif // RAD_PS2

#ifdef RAD_GAMECUBE
    #include <dolphin.h>
    extern OSHeapHandle gGCHeap;
#endif
//========================================
// Project Includes
//========================================
#include <radmemorymonitor.hpp>
#include <memory/createheap.h>
#include <memory/memoryutilities.h>

namespace Memory
{
//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifdef RAD_PS2
static size_t g_MaxFreeMemory = 0;  //used by the PS2 to determine how much memory is free
#endif


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

size_t AllocateLargestFreeBlock( IRadMemoryAllocator* allocator, void*& returnMemory )
{
    //
    // Find out the largest block of memory I can grab
    //
    size_t lo = 0;
    size_t hi = 1024*1024*256; //this is just less than 256mb
    int pivot;
    void* memory = NULL;
    do
    {
        pivot = ( hi + lo ) / 2;
        if( memory != NULL )
        {
            allocator->FreeMemory( memory );
            memory = NULL;
        }
        memory = allocator->GetMemory( pivot );
        if( memory != NULL )
        {
            lo = pivot;
        }
        else
        {
            memory = allocator->GetMemory( lo );
            hi = pivot;
        }
    } while( ( hi - lo ) > 1 );
    returnMemory = memory;
    return lo;
}

//==============================================================================
// InitializeMemoryUtilities
//==============================================================================
//
// Description: does any setup required for the memory utility system. This is
//              absolutely essential on the PS2
//
// Parameters:	None.
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//==============================================================================
void InitializeMemoryUtilities()
{
    static bool alreadyCalled = false;
    if( alreadyCalled )
    {
        return;
    }
    alreadyCalled = true;
#ifdef RAD_PS2
    //this is the largest amount of memory that is free
    g_MaxFreeMemory = GetFreeMemoryProfile();
#endif
}

//==============================================================================
// GetFreeMemoryEntropy
//==============================================================================
//
// Description: attempt at getting a measurement of fragmentation
//
// Parameters:	None.
//
// Return:      fragmentation metric
//
// Constraints: None
//
//==============================================================================
float GetFreeMemoryEntropy( IRadMemoryAllocator* allocator )
{
    //
    // if it's a tracking heap, just quit
    //
    GameMemoryAllocator which = WhichAllocator( allocator );
    HeapType type = AllocatorType( which );
    if( type != HEAP_TYPE_DOUG_LEA )
    {
        return 0.0f;
    }

//    #ifdef RAD_XBOX
//    return 0.0;
//    #endif

    unsigned int totalFreeMemory;
    allocator->GetStatus( &totalFreeMemory, NULL, NULL, NULL );

    //
    // Get the largest N blocks of memory
    //
    const int N = 10;
    size_t largestSize[ N ];
    void*  memory     [ N ];

    int i;
    for( i = 0; i < N; ++i )
    {
        void* pointer = NULL;
        unsigned int freeMemory;
        unsigned int largestBlock;
        allocator->GetStatus( &freeMemory, &largestBlock, NULL, NULL );

        largestSize[ i ] = AllocateLargestFreeBlock( allocator, pointer );
        memory[ i ] = pointer;
    }

    //
    // Compute the entropy
    //
    float totalEntropy = 0.0f;
    float remainingPercentage = 1.0f;
    for( i = 0; i < N; ++i )
    {
        float size = static_cast< float >( largestSize[ i ] );
        float percentage = size / static_cast< float >( totalFreeMemory );
        float entropy = -percentage * logf( percentage ) / logf( 2.0f );
        totalEntropy += entropy;
        remainingPercentage -= percentage;
    }

    //
    //
    //

    //
    // free the memory
    //
    for( i = 0; i < N; ++i )
    {   
        allocator->FreeMemory( memory[ i ] );
        memory[ i ] = NULL;
    }

    return totalEntropy;
}

//==============================================================================
// GetFreeMemoryProfile
//==============================================================================
//
// Description: Tries to fill up availiable memory and reports back on how much
//              it got
//
// Parameters:	None.
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//==============================================================================
size_t GetFreeMemoryProfile()
{
    static int numberOfTimesCalled = 0;
    ++numberOfTimesCalled;

#ifdef _WIN32
    return 0;
#endif
    const int size = 256;
    void* pointers[ size ];
    size_t sizes[ size ];

    int index = 0;
    int i;
    for( i = 0; i < size; i++ )
    {
        pointers[ i ] = NULL;
        sizes[ i ] = 0;
    }

    int retrys = 0;
    do
    {
        //
        // Find out the largest block of memory I can grab
        //
        int lo = 0;
        int hi = 1024*1024*256; //this is just less than 256mb
        int pivot;
        void* memory = NULL;
        do
        {
            pivot = ( hi + lo ) / 2;
            if( memory != NULL )
            {
                free( memory );
                memory = NULL;
            }
            memory = malloc( pivot );
            if( memory != NULL )
            {
                lo = pivot;
            }
            else
            {
                memory = malloc( lo );
                hi = pivot;
            }
        } while( ( hi - lo ) > 1 );

        if( ( memory == NULL ) && ( retrys < 2 ) )
        {
            ++retrys;
        }
        else
        {
            sizes[ index ] = lo;
            pointers[ index ] = memory;
            memory = NULL;
            ++index;
        }
    } while( ( pointers[ index - 1 ] != NULL ) && ( index < size ) );

    //
    // Need to sum the memory
    //
    size_t total = 0;
    for( i = 0; i < size; i++ )
    {
        total += sizes[ i ];
    }

    //
    // Need to delete the memory
    //
    for( i = 0; i < size; i++ )
    {
        void* pointer = pointers[ i ];
        if( pointer != NULL )
        {
            free( pointer );
            pointers[ i ] = NULL;
        }
        else
        {
            break;
        }
    }

#ifndef FINAL
    //rReleasePrintf( "Free Memory Profile - %d\n", total );
#endif
    return total;
}

//==============================================================================
// GetLargestFreeBlock
//==============================================================================
//
// Description: Attempts via a binary search to determine the largest free 
//              block of main memory (uses malloc)
//
// Parameters:	None.
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//==============================================================================
size_t GetLargestFreeBlock()
{
    #ifdef _WIN32
        return 0;
    #endif
    
    #ifdef RAD_GAMECUBE
        return 0;
    #endif

    //
    // Find out the largest block of memory I can grab
    //
    size_t lo = 0;
    size_t hi = 1024*1024*256; //this is just less than 256mb
    int pivot;
    void* memory = NULL;
    do
    {
        pivot = ( hi + lo ) / 2;
        if( memory != NULL )
        {
            free( memory );
            memory = NULL;
        }
        memory = malloc( pivot );
        if( memory != NULL )
        {
            lo = pivot;
        }
        else
        {
            memory = malloc( lo );
            hi = pivot;
        }
    } while( ( hi - lo ) > 1 );
    free( memory );
    return hi;
}

//==============================================================================
// GetLargestFreeBlock
//==============================================================================
//
// Description: Attempts via a binary search to determine the largest free 
//              block of main memory (uses malloc)
//
// Parameters:	allocator - the allocator to test
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//==============================================================================
size_t GetLargestFreeBlock( IRadMemoryAllocator* allocator )
{
    //
    // Can't call this on the persistent heap
    //
    IRadMemoryAllocator* persistent = GetAllocator( GMA_PERSISTENT );
    if( allocator == persistent )
    {
        unsigned int totalFreeMemory;
        unsigned int largestBlock;
        unsigned int numberOfObjects;
        unsigned int highWater;
        allocator->GetStatus( &totalFreeMemory, &largestBlock, &numberOfObjects, &highWater );
        return largestBlock;
    }

    //
    // Find out the largest block of memory I can grab
    //
    void* memory = NULL;
    size_t size = AllocateLargestFreeBlock( allocator, memory );
    allocator->FreeMemory( memory );
    return size;
}

//==============================================================================
// GetLargestNFreeBlocks
//==============================================================================
//
// Description: Attempts via a binary search to determine the largest free 
//              block of main memory (uses malloc)
//
// Parameters:	allocator - the allocator to test
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//==============================================================================
void GetLargestNFreeBlocks( IRadMemoryAllocator* allocator, const int n, size_t blocks[] )
{
}

//==============================================================================
// GetMaxFreeMemory
//==============================================================================
//
// Description: Returns the maximum amount of memory that has ever been free
//              in the lifetime of the game
//
// Parameters:	None.
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//==============================================================================
size_t GetMaxFreeMemory()
{
#ifdef RAD_PS2
    return g_MaxFreeMemory;
#else
    rAssertMsg( false, "GetMaxFreeMemory - this doesnt work on any platform but the PS2\n" );
    return 0;
#endif
}

//=============================================================================
// GetTotalMemoryFree
//=============================================================================
//
// Description: Returns the total amount of free memory
//
// Parameters:	None.
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//=============================================================================
size_t GetTotalMemoryFree()
{
    #if defined _WIN32
        MEMORYSTATUS status;
        GlobalMemoryStatus (&status);
        return status.dwAvailPhys;
    #elif defined RAD_PS2
        size_t used = GetTotalMemoryUsed();
        size_t maxFree = GetMaxFreeMemory();
        size_t free = maxFree - used;
        return free;
    #elif defined RAD_GAMECUBE
        return OSCheckHeap( gGCHeap );
    #else
        #pragma error( "CMemoryTracker::GetTotalMemoryFree - What platform is this then?");
    #endif
    return 0;
}

//=============================================================================
// GetTotalMemoryFreeLowWaterMark
//=============================================================================
//
// Description: Returns the low water mark of free memory. Must be called at
//              the frequency at which you want the memory sampled
//
// Parameters:	None.
//
// Return:      total amount of free memory at the worst point in the game
//
// Constraints: None
//
//=============================================================================
size_t GetTotalMemoryFreeLowWaterMark()
{
    static size_t lowWaterMark = GetTotalMemoryFree();
    size_t currentFree = GetTotalMemoryFree();
    if( currentFree < lowWaterMark )
    {
        lowWaterMark = currentFree;
    }
    return lowWaterMark;
}

//=============================================================================
// GetTotalMemoryUnavailable
//=============================================================================
//
// Description: Returns the total amount of free memory
//
// Parameters:	None.
//
// Return:      total amount of free memory.
//
// Constraints: None
//
//=============================================================================
size_t GetTotalMemoryUnavailable()
{
    #if defined _WIN32
        //IAN didn't bother writing this yet
        return 0;
    #elif defined RAD_PS2
        #ifdef RAD_RELEASE
            size_t totalPhysicalMemory = 1024 * 1024 * 32;
        #else
            size_t totalPhysicalMemory = 1024 * 1024 * 128;
        #endif
        size_t maxFree = GetMaxFreeMemory();
        size_t unavailable = totalPhysicalMemory - maxFree;
        return unavailable;
    #elif defined RAD_GAMECUBE
        return 0;
    #else
        #pragma error( "CMemoryTracker::GetTotalMemoryFree - What platform is this then?");
    #endif
    return 0;
}

//==============================================================================
// GetTotalMemoryUsed
//==============================================================================
//
// Description: query the OS with regard to the amount of memory used
//
// Parameters:	None.
//
// Return:      total amount of memory used
//
// Constraints: None
//
//==============================================================================
size_t GetTotalMemoryUsed()
{
#if defined _WIN32
    #ifdef RAD_DEBUG
        _CrtMemState state;
        _CrtMemCheckpoint( &state );
        int total = 0;
        int i;
        for( i = 0; i < _MAX_BLOCKS; i++ )
        {
            total += state.lSizes[ i ];
        }

        //double check against the MEMORYSTATUS numbers
        MEMORYSTATUS status;
        GlobalMemoryStatus (&status);
        size_t doubleCheck =  status.dwTotalPhys - status.dwAvailPhys;
        //double check these numbers - they should match on the XBOX!
        return total;
    #else
        return 0;
    #endif
#endif

#if defined RAD_PS2
    struct mallinfo info = mallinfo();
    return info.uordblks;
#endif

#if defined RAD_GAMECUBE
    return 0;
#endif
};

//=============================================================================
// PrintMemoryStatsToTty
//=============================================================================
//
// Description: Prints the total free memory to the TTY
//
// Parameters:	None.
//
// Return:      None
//
// Constraints: None
//
//=============================================================================
void PrintMemoryStatsToTty()
{
#ifdef RAD_PS2
    size_t totalFree    = GetTotalMemoryFree();
    size_t profileFree  = GetFreeMemoryProfile();
    size_t largestFree  = GetLargestFreeBlock();
    size_t lowWaterMark = GetTotalMemoryFreeLowWaterMark();
    rReleasePrintf( "MEMSTATS\ttotalfree:%d\tprofile:%d\tlargestBlock:%d\tlowWater:%d\n", totalFree, profileFree, largestFree, lowWaterMark );
#else
    size_t totalFree = GetTotalMemoryFree();
    rReleasePrintf( "MEMSTATS\t%d\t\n", totalFree );
#endif
}

}   //namespace MEMORY
