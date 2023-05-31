//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        createheap.cpp
//
// Description: this set of functions creates heaps
//
// History:     2003/04/13 + Created -- Ian Gipson
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <memory/createheap.h>
#include <radmemorymonitor.hpp>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
IRadMemoryHeap* g_HeapArray[ NUM_GAME_MEMORY_ALLOCATORS ];

struct HeapCreationData
{
    HeapType            type;
    GameMemoryAllocator parent;
    char                name[ 256 ];
};

HeapCreationData g_HeapCreationData[] = 
{
    { HEAP_TYPE_DOUG_LEA, GMA_DEFAULT, "Default"              },      
    { HEAP_TYPE_DOUG_LEA, GMA_DEFAULT, "Temp"                 },      
    { HEAP_TYPE_NONE,     GMA_DEFAULT, "Gamecube VMM"         },      
#ifdef RAD_WIN32
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Persistent"           },  // no static heap for pc
#else
    { HEAP_TYPE_STATIC,   GMA_DEFAULT, "Persistent"           },
#endif // RAD_WIN32
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Level"                },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Level Movie"          },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Level FE"             },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Level Zone"           },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Level Other"          },
    { HEAP_TYPE_DOUG_LEA, GMA_DEFAULT, "Level Hud"            },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Level Mission"        },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Level Audio"          },
    { HEAP_TYPE_NONE,     GMA_DEFAULT, "Debug"                },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Special"              },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Music"                },
    { HEAP_TYPE_DOUG_LEA, GMA_DEFAULT, "Audio Persistent"     },
    { HEAP_TYPE_DOUG_LEA, GMA_DEFAULT, "Small Alloc"          },
#ifdef RAD_XBOX
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "XBOX Sound"           },
#endif
#ifdef USE_CHAR_GAG_HEAP
    { HEAP_TYPE_DOUG_LEA, GMA_DEFAULT, "Characters and Gags"  },
#endif
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Anywhere in Level"    },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Anywhere in FE"       },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Either Other or Zone" },
    { HEAP_TYPE_TRACKING, GMA_DEFAULT, "Search"               },
    { HEAP_TYPE_NONE,     GMA_DEFAULT, "???"                  },
    { HEAP_TYPE_NONE,     GMA_DEFAULT, "???"                  },
};

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// AllocatorType
//=============================================================================
//
// Description: which type of heap is this?
//
// Parameters:  allocator - which heap are we concerned with
//
// Return:      N/A.
//
//=============================================================================
HeapType AllocatorType( GameMemoryAllocator allocator )
{
    return g_HeapCreationData[ allocator ].type;
}

//=============================================================================
// CreateHeap
//=============================================================================
//
// Description: Creates the specified heap
//
// Parameters:  allocator - which heap are we creating
//
// Return:      N/A.
//
//=============================================================================
void CreateHeap ( GameMemoryAllocator allocator, const unsigned int size )
{
    unsigned int index = static_cast< unsigned int >( allocator );
    rAssert( g_HeapArray[ index ] == NULL );

    HeapType type    = g_HeapCreationData[ index ].type;
    const char* name = g_HeapCreationData[ index ].name;
    GameMemoryAllocator parent = g_HeapCreationData[ index ].parent;

    rReleasePrintf ("Creating Heap: %s (%d)\n", name, size );

    #ifdef RAD_RELEASE
    if( type == HEAP_TYPE_TRACKING )
    {
        type = HEAP_TYPE_NONE;
    }
    #endif

    switch( type )
    {
        case HEAP_TYPE_STATIC :
        {
            HeapMgr()->PushHeap( GMA_DEBUG );
            g_HeapArray[ index ] = radMemoryCreateStaticHeap( size, RADMEMORY_ALLOC_DEFAULT, name );
            g_HeapArray[ index ]->AddRef();
            HeapMgr()->PopHeap( GMA_DEBUG );
            break;
        }
        case HEAP_TYPE_TRACKING :
        {
            HeapMgr()->PushHeap( GMA_DEBUG );
            g_HeapArray[ index ] = radMemoryCreateTrackingHeap( size, RADMEMORY_ALLOC_DEFAULT, name );
            HeapMgr()->PopHeap( GMA_DEBUG );
            break;
        }
        case HEAP_TYPE_DOUG_LEA :
        {
            HeapMgr()->PushHeap( GMA_DEBUG );
            g_HeapArray[ index ] = radMemoryCreateDougLeaHeap( size, RADMEMORY_ALLOC_DEFAULT, name );
            g_HeapArray[ index ]->AddRef();
            HeapMgr()->PopHeap( GMA_DEBUG );
            break;
        }
        case HEAP_TYPE_NONE :
        {
            //rAssert( false );
            return;
        }
        default:
        {
            rAssert( false );
            return;
        }
    }
    radMemoryRegisterAllocator( allocator, parent,g_HeapArray[ index ] );
}

//=============================================================================
// DestroyHeap
//=============================================================================
//
// Description: Destroys the specified heap
//
// Parameters:  allocator - which heap are we destroying
//
// Return:      N/A.
//
//=============================================================================
void DestroyHeapA( GameMemoryAllocator allocator )
{
    unsigned int index = static_cast< unsigned int >( allocator );
    if( g_HeapArray[ index ] == NULL )
    {
        return;
    }

    // Check for heap not empty.
    // If it's not empty this is a Bad Thing.  We will be leaving dangling pointers to all the contained memory.
    // This either means we have a leak and they will never get deleted or they will get deleted eventually but
    // the heap will no longer exist.
    //
    // If you assert here, DO NOT IGNORE IT.  If you need help diagnosing the cause of the assertion, see Joel.
    //
    unsigned int numAllocs;
    unsigned int totalFree;
    g_HeapArray[ index ]->GetStatus ( &totalFree, 0, &numAllocs, 0);

    //
    // Suspend memory monitor etc if we've leaked
    //
    if( numAllocs > 0 )
    {
        #ifndef FINAL
            unsigned int size = g_HeapArray[ index ]->GetSize();
            char s[ 256 ];
            const char* name = g_HeapCreationData[ index ].name;
            unsigned int leakSize = size - totalFree;
            sprintf( s, "MEMORY LEAK: '%s' %d blocks %d bytes ", name, numAllocs, leakSize );
            rTunePrintf( s );
            g_HeapArray[index]->ValidateHeap();
        #endif

        // Destroy the heap
        //
        g_HeapArray[ index ]->Release ();
        g_HeapArray[ index ] = 0;
        ::radMemoryUnregisterAllocator( allocator );

        if( CommandLineOptions::Get( CLO_MEMORY_MONITOR ) )
        {
            ::radMemoryMonitorSuspend ();
        }
        #ifndef RAD_RELEASE
        else
        {
//                rAssertMsg (0, s);
        }
        #endif // RAD_RELEASE
    }
    else
    {
        // Destroy the heap
        //
        g_HeapArray[ index ]->Release ();
        g_HeapArray[ index ] = 0;
        ::radMemoryUnregisterAllocator( allocator );
    }
}

//=============================================================================
// GetHeap
//=============================================================================
//
// Description: allows access to the heaps
//
// Parameters:  allocator - which heap are we getting
//
// Return:      N/A.
//
//=============================================================================
IRadMemoryAllocator* GetAllocator( GameMemoryAllocator allocator )
{
    IRadMemoryAllocator* allocatorPtr = radMemoryGetAllocator( static_cast< int >( allocator ) );
    return allocatorPtr;
}

//=============================================================================
// GetHeapReference
//=============================================================================
//
// Description: allows access to the heaps in the array
//
// Parameters:  allocator - which heap are we getting
//
// Return:      N/A.
//
//=============================================================================
IRadMemoryHeap** GetHeapReference( GameMemoryAllocator allocator )
{
    return &( g_HeapArray[ allocator ] );
}

//=============================================================================
// GetTotalMemoryFreeInAllHeaps
//=============================================================================
//
// Description: tells us how much free space there is in all the heaps
//
// Parameters:  none
//
// Return:      total free space
//
//=============================================================================
size_t GetTotalMemoryFreeInAllHeaps()
{
    unsigned int totalFreeMemory = 0;
    unsigned int i;
    unsigned int size = NUM_GAME_MEMORY_ALLOCATORS;
    for( i = 0; i < size; ++i )
    {
        IRadMemoryHeap* heap = g_HeapArray[ i ];
        unsigned int free;
        unsigned int largestBlock;
        unsigned int numberOfObjects;
        unsigned int highWaterMark;
        if( heap != NULL )
        {
            heap->GetStatus( &free, &largestBlock, &numberOfObjects, &highWaterMark );
            totalFreeMemory += free;
        }
    }
    return totalFreeMemory;
}

//=============================================================================
// AllocatorType
//=============================================================================
//
// Description: figure out what heap this is
//
// Parameters:  none
//
// Return:      the index of the heap
//
//=============================================================================
GameMemoryAllocator WhichAllocator( const IRadMemoryAllocator* heap )
{
    int i;
    for( i = 0; i < NUM_GAME_MEMORY_ALLOCATORS; ++i )
    {
        if( g_HeapArray[ i ] == heap )
        {
            return static_cast< GameMemoryAllocator >( i );
        }
    }
    return NUM_GAME_MEMORY_ALLOCATORS;
}
