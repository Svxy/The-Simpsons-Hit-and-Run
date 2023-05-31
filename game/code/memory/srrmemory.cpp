//==============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        memory.cpp
//
// Description: 
//
// History:     3/20/2002 + Created -- Darwin Chau
//
//==============================================================================

//========================================
// System Includes
//========================================      
#ifdef RAD_PS2      
#include <malloc.h>
#endif

// Foundation Tech
#include <radmath/util.hpp>
#include <radmemory.hpp>
#include <radmemorymonitor.hpp>
#include <radthread.hpp>
#include <raddebug.hpp>
#include <radtextdisplay.hpp>
#include <radsound_hal.hpp>
#include <p3d/utility.hpp>

#include <string.h>

//========================================
// Project Includes
//========================================
#include <main/game.h>
#include <memory/createheap.h>
#include <memory/srrmemory.h>
#include <memory/memoryutilities.h>

#include <cheats/cheatinputsystem.h>

#include <mission/gameplaymanager.h>

#ifdef RAD_PS2
#include <pddi/pddiext.hpp>
#include <main/ps2platform.h>
#define INIT_MEM()  Memory::InitializeMemoryUtilities();PS2Platform::InitializeMemory();
#endif // RAD_PS2

#ifdef RAD_XBOX
#include <main/xboxplatform.h>
#define INIT_MEM()  Memory::InitializeMemoryUtilities();XboxPlatform::InitializeMemory();radMemoryInitialize();
void MemoryHackCallback() { INIT_MEM() };
#endif // RAD_XBOX

#ifdef RAD_GAMECUBE
#include <main/gcplatform.h>
#include <dolphin.h>
#define INIT_MEM()  Memory::InitializeMemoryUtilities();GCPlatform::InitializeMemory();
extern IRadMemoryHeap *vmmHeap;
void MemoryHackCallback() { INIT_MEM() };
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
    void MemoryHackCallback() { INIT_MEM() };
#endif

#ifdef RAD_WIN32
#include <main/win32platform.h>
#define INIT_MEM()  Memory::InitializeMemoryUtilities();Win32Platform::InitializeMemory();
#define SHUTDOWN_MEM()  Win32Platform::ShutdownMemory();
#endif // RAD_WIN32

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
bool g_LockedPersistentHeap = false;
bool g_HeapManagerCreated   = false;
//
// Has the memory system been intialized via FTech?
//
bool gMemorySystemInitialized = false;

// 
// Temporarily disable allocation routing (to avoid infinite loops)
//
bool g_NoHeapRoute = false;

const char* HeapNames[] =
{
    "Default Heap",
        "Temp Heap",
        "GameCube Virtual Memory Heap",
        "Persistent Heap",
        "Level Heap",
        "Movie Heap",
        "Frontend Heap",
        "Zone and Rail Heap",
        "Level Other Heap",
        "HUD Heap",
        "Mission Heap",
        "Audio Heap",
        "Debug Heap",
        "Special Heap",
        "Music Heap",
        "Audio Persistent",
        "Small Alloc Heap",
#ifdef RAD_XBOX
        "Xbox Sound Heap",
#endif
#ifdef USE_CHAR_GAG_HEAP
        "Character and Gag Heap",
#endif
        "Anywhere in Level",
        "Anywhere in FE",
        "Either Other or Zone",
        "Allocator Search"
};


#ifndef RAD_RELEASE
//
// For overriding all allocation routing and sending everything to the special heap
//
bool HeapManager::s_bSpecialRoute = false;
#endif

inline void* AllocateThis( GameMemoryAllocator allocator, size_t size )
{
    void* pMemory = NULL;
#ifdef CORRAL_SMALL_ALLOCS
    if( size < 201 )
    {
        if( allocator != GMA_AUDIO_PERSISTENT && allocator != GMA_PERSISTENT && allocator != GMA_TEMP)
        {
            pMemory = radMemoryAlloc( GMA_SMALL_ALLOC, size );
        }
        else
        { 
            pMemory = radMemoryAlloc( allocator, size );
        }
    }
    else
#endif
    {
        if ( allocator >= GMA_ANYWHERE_IN_LEVEL && allocator != ALLOCATOR_SEARCH )
        {
            pMemory = FindFreeMemory( allocator, size );
        }
        else
        {
            //pMemory = radMemoryAlloc( ::radMemoryGetCurrentAllocator (), size );
            pMemory = radMemoryAlloc( allocator, size );
        }
    }

    return pMemory;
}

//==============================================================================
// new
//==============================================================================
//
// Description: regular new
//
// Parameters:  size - size of memory we're allocating
//
// Return:      
//
//==============================================================================
void* operator new( size_t size )
#ifdef RAD_PS2
#ifndef RAD_MW
throw( std::bad_alloc )
#endif
#endif
{
    if( gMemorySystemInitialized == false )
    {
        INIT_MEM();
    }

    void* pMemory;

    if (g_NoHeapRoute)
    {
        pMemory = radMemoryAlloc( 0, size );
    }
    else
    {
        GameMemoryAllocator curr = HeapMgr()->GetCurrentHeap();
        pMemory = AllocateThis( curr, size );

#ifdef MEMORYTRACKER_ENABLED
        ::radMemoryMonitorIdentifyAllocation (pMemory, HeapMgr()->GetCurrentGroupID ());
#endif
    }


    //MEMTRACK_ALLOC( pMemory, size, 0 );

    return( pMemory );
}


//==============================================================================
// delete
//==============================================================================
//
// Description: regular delete
//
// Parameters:  pMemory - pointer to the memory we're deleting
//
// Return:      
//
//==============================================================================
void operator delete( void* pMemory )
#ifdef RAD_PS2
#ifndef RAD_MW
throw()
#endif
#endif
{
    radMemoryFree( pMemory );
}


//==============================================================================
// 
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void* operator new[]( size_t size )
#ifdef RAD_PS2
#ifndef RAD_MW
throw( std::bad_alloc )
#endif
#endif
{
    if( gMemorySystemInitialized == false )
    {
        INIT_MEM();
    }

    void* pMemory;

    if (g_NoHeapRoute)
    {
        pMemory = radMemoryAlloc( 0, size );
    }
    else
    {
        GameMemoryAllocator curr = HeapMgr()->GetCurrentHeap();
        pMemory = AllocateThis( curr, size );

#ifdef MEMORYTRACKER_ENABLED
        ::radMemoryMonitorIdentifyAllocation (pMemory, HeapMgr()->GetCurrentGroupID ());
#endif
    }

    //MEMTRACK_ALLOC( pMemory, size, ALLOC_ARRAY );

    return( pMemory );
}


//==============================================================================
// 
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void operator delete[]( void* pMemory )
#ifdef RAD_PS2
#ifndef RAD_MW
throw()
#endif
#endif
{
    radMemoryFree( pMemory );
}


//==============================================================================
// 
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void* operator new( size_t size, GameMemoryAllocator allocator )
{
    if( gMemorySystemInitialized == false )
    {
        INIT_MEM();
    }

#ifndef RAD_RELEASE
    if (HeapManager::s_bSpecialRoute)
    {
        allocator = GMA_SPECIAL;
    }
#endif

    void* pMemory = AllocateThis( allocator, size );

    if (!g_NoHeapRoute)
    {
#ifdef MEMORYTRACKER_ENABLED
        ::radMemoryMonitorIdentifyAllocation (pMemory, HeapMgr()->GetCurrentGroupID ());
#endif
    }

    //MEMTRACK_ALLOC( pMemory, size, 0 );

    return( pMemory );
}


//==============================================================================
// 
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void operator delete( void* pMemory, GameMemoryAllocator allocator )
{
    radMemoryFree( pMemory );
}


//==============================================================================
// 
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void* operator new[]( size_t size, GameMemoryAllocator allocator )
{
    if( gMemorySystemInitialized == false )
    {
        INIT_MEM();
    }

#ifndef RAD_RELEASE
    if (HeapManager::s_bSpecialRoute)
    {
        allocator = GMA_SPECIAL;
    }
#endif

    void* pMemory = AllocateThis( allocator, size );

    if (!g_NoHeapRoute)
    {
#ifdef MEMORYTRACKER_ENABLED
        ::radMemoryMonitorIdentifyAllocation (pMemory, HeapMgr()->GetCurrentGroupID ());
#endif
    }

    //MEMTRACK_ALLOC( pMemory, size, ALLOC_ARRAY );

    return( pMemory );
}


//==============================================================================
// 
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void operator delete[]( void* pMemory, GameMemoryAllocator allocator )
{
    radMemoryFree( pMemory );
}

static GameMemoryAllocator AVAILABLE_FOR_RENT_IN_LEVEL[] =
{
    GMA_TEMP,
    GMA_LEVEL_ZONE,                            //    7         6     6
    GMA_LEVEL_OTHER,                           //    8         7     7
    GMA_LEVEL_MISSION,                         //    10        9     9
    GMA_LEVEL_HUD                              //    9         8     8
};
static GameMemoryAllocator AVAILABLE_FOR_RENT_IN_FE[] =
{
    GMA_LEVEL_MOVIE,                           //    5         4     4
    GMA_LEVEL_FE,                              //    6         5     5
    GMA_LEVEL_AUDIO                            //    11        10    10
};
static GameMemoryAllocator AVAILABLE_IN_OTHER_OR_ZONE[] =
{
    GMA_LEVEL_OTHER,                           //    8         7     7
    GMA_LEVEL_ZONE                             //    7         6     6
};


void* FindFreeMemory( GameMemoryAllocator allocator, size_t size )
{
    GameMemoryAllocator* list = NULL;
    unsigned int numAvailable = 0;

    if ( allocator == GMA_ANYWHERE_IN_LEVEL )
    {
        list = AVAILABLE_FOR_RENT_IN_LEVEL;
        numAvailable = sizeof ( AVAILABLE_FOR_RENT_IN_LEVEL ) / sizeof( GameMemoryAllocator );
    }
    else if ( allocator == GMA_ANYWHERE_IN_FE )
    {
        list = AVAILABLE_FOR_RENT_IN_FE;
        numAvailable = sizeof ( AVAILABLE_FOR_RENT_IN_FE ) / sizeof( GameMemoryAllocator );
    }
    else if ( allocator == GMA_EITHER_OTHER_OR_ZONE )
    {
        list = AVAILABLE_IN_OTHER_OR_ZONE;
        numAvailable = sizeof ( AVAILABLE_IN_OTHER_OR_ZONE ) / sizeof( GameMemoryAllocator );       
    }
    else
    {
        rAssert( false );
    }

    if ( list != NULL )
    {
        ::radMemorySetUsableAllocators( (radMemoryAllocator*)list, numAvailable );
        void* memory = radMemoryAlloc( ALLOCATOR_SEARCH, size );
      
        return memory;
    }

    return NULL;
}

void SetupAllocatorSearch( GameMemoryAllocator allocator )
{
    rAssert( allocator >= GMA_ANYWHERE_IN_LEVEL );

    GameMemoryAllocator* list = NULL;
    unsigned int numAvailable = 0;

    if ( allocator == GMA_ANYWHERE_IN_LEVEL )
    {
        list = AVAILABLE_FOR_RENT_IN_LEVEL;
        numAvailable = sizeof ( AVAILABLE_FOR_RENT_IN_LEVEL ) / sizeof( GameMemoryAllocator );
    }
    else if ( allocator == GMA_ANYWHERE_IN_FE )
    {
        list = AVAILABLE_FOR_RENT_IN_FE;
        numAvailable = sizeof ( AVAILABLE_FOR_RENT_IN_FE ) / sizeof( GameMemoryAllocator );
    }
    else if ( allocator == GMA_EITHER_OTHER_OR_ZONE )
    {
        list = AVAILABLE_IN_OTHER_OR_ZONE;
        numAvailable = sizeof ( AVAILABLE_IN_OTHER_OR_ZONE ) / sizeof( GameMemoryAllocator );       
    }
    else
    {
        rAssert( false );
    }

    if ( list != NULL )
    {
        radMemorySetUsableAllocators( (radMemoryAllocator*)list, numAvailable );
    }
} 

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//
// Use FTT's radTextDisplay functionality to slap an out-of-memory
// message onto the screen before we die
//
void PrintOutOfMemoryMessage( void* userData, radMemoryAllocator heap, const unsigned int size )
{
#ifndef FINAL
    //
    // Print out memory information to the TTY first
    //
    Memory::PrintMemoryStatsToTty();

    //Disable this while we're here...
    ::radMemorySetOutOfMemoryCallback( NULL, NULL );

#ifdef RAD_GAMECUBE
    ::radMemoryMonitorSuspend();
#endif

    IRadTextDisplay* textDisplay;
    GameMemoryAllocator heapEnum = static_cast<GameMemoryAllocator>(heap);

#ifdef RAD_PS2
    //
    // Need to shut down the MFIFO for this to work properly
    //
    ((pddiExtPS2Control*)p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL))->MFIFOEnable( false );
    rReleasePrintf("MFIFO Disabled.\n");
#endif

    //
    // Ironically, the text display object needs to be allocated on a heap,
    // one of which is out of memory.  Hopefully we're not out on more
    // than one.
    //
    if( heapEnum == GMA_DEFAULT )
    {
        ::radTextDisplayGet( &textDisplay, GMA_TEMP );
    }
    else
    {
        ::radTextDisplayGet( &textDisplay, GMA_DEFAULT );
    }

    IRadMemoryAllocator* heapPtr = GetAllocator( static_cast< GameMemoryAllocator >( heap ) );
    unsigned int totalFree        = 0;
    unsigned int largestBlock     = 0;
    unsigned int numberOfObjects  = 0;
    unsigned int highWaterMark    = 0;
    unsigned int lastAllocation   = 0;
    if( heapPtr != NULL )
    {
        lastAllocation = size;
        heapPtr->GetStatus( &totalFree, &largestBlock, &numberOfObjects, &highWaterMark );
        largestBlock = Memory::GetLargestFreeBlock( heapPtr );
    }
    char freeMemoryString[256]   = "";
    char largestBlockString[256] = "";
    char lastAllocationStr[256]  = "";
    sprintf( freeMemoryString,   "Free:         %d", totalFree      );
    sprintf( largestBlockString, "LargestBlock: %d", largestBlock   );
    sprintf( lastAllocationStr,  "LastAlloc:    %d", lastAllocation );

    int yPos = 10;
    if ( textDisplay )
    {
        //GetHeapMgr()->DumpHeapStats(true)
        textDisplay->SetBackgroundColor( 0 );
        textDisplay->SetTextColor( 0xffffffff );
        textDisplay->Clear();
        textDisplay->TextOutAt( "OUT OF MEMORY",     20, yPos      );
        textDisplay->TextOutAt( HeapNames[heapEnum], 20, yPos += 1 );
        textDisplay->TextOutAt( freeMemoryString,    20, yPos += 1 );
        textDisplay->TextOutAt( largestBlockString,  20, yPos += 1 );
        textDisplay->TextOutAt( lastAllocationStr,   20, yPos += 1 );

        if ( CommandLineOptions::Get( CLO_DEMO_TEST ) ||
             GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_DEMO_TEST ) )
        {
            char buffy[32];
            sprintf( buffy, "Demo Count: %d", GetGame()->GetDemoCount() );
            textDisplay->TextOutAt( buffy, 20, yPos += 2 );

            unsigned int time = GetGame()->GetTime();
            unsigned int hours = time / 3600000;
            unsigned int deltaTime = time % 3600000;

            unsigned int minutes = deltaTime / 60000;
            deltaTime = deltaTime % 60000;

            unsigned int seconds = deltaTime / 1000;
            deltaTime = deltaTime % 1000;
            sprintf( buffy, "Time: %d:%d:%d.%d", hours, minutes, seconds, deltaTime );
            textDisplay->TextOutAt( buffy, 20, yPos += 2 );

            if ( GetGameplayManager() )
            {
                sprintf( buffy, "Level %d", GetGameplayManager()->GetCurrentLevelIndex() );
                textDisplay->TextOutAt( buffy, 20, yPos += 2 );
            }
        }

        textDisplay->SwapBuffers();
        textDisplay->Release();
    }

    rReleaseBreak();

#ifndef RAD_GAMECUBE
    ::radMemoryMonitorSuspend();
#endif

    //Re-enable now that we're through
    ::radMemorySetOutOfMemoryCallback( PrintOutOfMemoryMessage, NULL );
#endif
}

static int pushNumberStack[ 48 ];
static int currentStackPointer = 0;

HeapStack::HeapStack (GameMemoryAllocator defaultAllocator)
{
    int i;
    for( i = 0; i < 48; ++i )
    {
        pushNumberStack[ i ] = -1;
    }

    m_Size = STACKSIZE;
    m_CurPos = 0;
    m_Stack = new(GMA_PERSISTENT) GameMemoryAllocator[m_Size];

    // Seed this with the default heap
    //
    m_Stack[m_CurPos] = defaultAllocator;
}


HeapStack::~HeapStack ()
{
    delete[] m_Stack;
}

void HeapStack::Push (GameMemoryAllocator alloc)
{
    static int pushNumber = 0;
    rAssert( currentStackPointer < 48 );
    pushNumberStack[ currentStackPointer ] = pushNumber;
    ++currentStackPointer;
    ++pushNumber;
    m_CurPos++;
    rAssertMsg (m_CurPos < m_Size, "Heap Stack Overflow! Increase size of the Heap Stack. [jdy]");
    m_Stack[m_CurPos] = alloc;
}

void HeapStack::Pop ()
{
    --currentStackPointer;
    pushNumberStack[ currentStackPointer ] = -1;
    GameMemoryAllocator current = m_Stack[ m_CurPos ];
    m_CurPos--;
    rAssertMsg (m_CurPos >= 0, "Heap Stack Underflow! Calls to Push and Pop on heap stack are mismatched. [jdy]");
}

void HeapStack::Pop( GameMemoryAllocator alloc )
{
    --currentStackPointer;
    pushNumberStack[ currentStackPointer ] = -1;
    rAssertMsg( m_Stack[ m_CurPos ] == alloc, "HeapStack - Detected mismatch in push/pop calls - fix, or tell Ian Gipson immediately" );
    m_CurPos--;
    rAssertMsg (m_CurPos >= 0, "Heap Stack Underflow! Calls to Push and Pop on heap stack are mismatched. [jdy]");
}


void HeapStack::SetTop (GameMemoryAllocator alloc)
{
    m_Stack[m_CurPos] = alloc;
}


GameMemoryAllocator HeapStack::Top () const
{
    return m_Stack[m_CurPos];
}


HeapActivityTracker::HeapActivityTracker ()
{
    for (int i = 0; i < NUM_GAME_MEMORY_ALLOCATORS; i++)
    {
        m_AllocsEnabled[i] = true;
        m_FreesEnabled[i] = true;
    }
}


void HeapActivityTracker::MemoryAllocated (radMemoryAllocator allocator, void* address, unsigned int size)
{
    if (CommandLineOptions::Get( CLO_NO_HEAPS ))
    {
        return;
    }

    // If you assert here, you are trying to allocate on a heap that does not allow it
    //
    rTuneAssert (m_AllocsEnabled[allocator]);

/*
    if (HeapManager::s_pIRadMemoryHeapTemp)
    {
        unsigned int size;
        size = HeapManager::s_pIRadMemoryHeapTemp->GetSize();
        unsigned int totalFree, largestBlock, numObjects, highWater;
        HeapManager::s_pIRadMemoryHeapTemp->GetStatus( &totalFree, &largestBlock, &numObjects, &highWater );
        if (totalFree < size)
        {
            unsigned int used = size - totalFree;
            if (used > 200 * 1024)
            {
                rTunePrintf ("Temp Used: %d\n", used);
                ::radMemoryMonitorSuspend ();
            }
        }
    } 
*/

}


void HeapActivityTracker::MemoryFreed (radMemoryAllocator allocator, void* address)
{
    if (CommandLineOptions::Get( CLO_NO_HEAPS ))
    {
        return;
    }

    // If you assert here, you are trying to free on a heap that does not allow it
    //
    rTuneAssert (m_FreesEnabled[allocator]);
}

HeapActivityTracker g_HeapActivityTracker;



IRadThreadLocalStorage* HeapManager::s_Instance = 0;

#ifdef DEBUGINFO_ENABLED
HeapManager* HeapManager::s_Instances[MAX_INST];
int HeapManager::s_NumInstances = 0;
#endif

HeapManager* HeapManager::GetInstance ()
{
    g_NoHeapRoute = true;

    // First check to see if the thread local storage object has been created
    //
    if (!s_Instance)
    {
        ::radThreadCreateLocalStorage (&s_Instance, GMA_PERSISTENT);
        s_Instance->SetValue (0);          // Is there a better way to initialize these values?  Perhaps radThread should do this.
    }

    // Now s_Instance is non-null.
    // Check to see if we have a HeapManager created for this thread yet.
    // If not, create it.
    //
    void* p = s_Instance->GetValue ();
    if (!p)
    {
        GameMemoryAllocator currentHeap = static_cast<GameMemoryAllocator>(::radMemoryGetCurrentAllocator ());
        p = static_cast<void*>(new(GMA_PERSISTENT) HeapManager (currentHeap));  // Create a heap manager.  Init the heap stack with the current heap.
        s_Instance->SetValue (p);
        ::radMemorySetAllocatorCallback (static_cast<IRadMemorySetAllocatorCallback*>(p));

#ifdef DEBUGINFO_ENABLED
        s_Instances[s_NumInstances] = static_cast<HeapManager*>(p);
        s_NumInstances++;
#endif
    }

    g_NoHeapRoute = false;
    g_HeapManagerCreated = true;
    return static_cast<HeapManager*>(p);
}

bool HeapManager::IsCreated()
{
    if( !g_HeapManagerCreated )
    {
        return false;
    }
    else
    {
        void* p = s_Instance->GetValue();
        if( p == NULL )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

void HeapManager::DestroyInstance()
{
    if( s_Instance != NULL )
    {
        void* p = s_Instance->GetValue();
        if( p != NULL )
        {
            ::radMemorySetAllocatorCallback( NULL );

            HeapManager* hm = static_cast<HeapManager*>( p );
            delete( GMA_PERSISTENT, hm );

            s_Instance->SetValue( NULL );
        }

        s_Instance->Release();
    }
}


/*
void HeapManager::DumpHeapStacks ()
{
    for (int i = 0; i < s_NumInstances; i++)
    {
        HeapManager* pHeapMgr = s_Instances[i];
        char s[64];
        if (pHeapMgr == HeapMgr())
        {
            strcpy (s, "Main Thread Heap Stack");
        }
        else
        {
            sprintf (s, "Heap Stack %p", pHeapMgr);
        }
        DEBUGINFO_PUSH_SECTION (s);

        pHeapMgr->m_HeapStack.Dump ();

        DEBUGINFO_POP_SECTION ();
    }
}


void HeapStack::Dump ()
{
    for (int i = m_CurPos-1; i >= 0; i--)
    {
        char s[64];
        sprintf (s, "%2d: %s", i, HeapNames[m_Stack[i]]);
        DEBUGINFO_ADDSCREENTEXT (s);
    }
}
*/

#ifdef RAD_GAMECUBE
IRadMemoryHeap* s_pIRadMemoryHeapVMM             = 0;
#endif

HeapManager::HeapManager (GameMemoryAllocator defaultAllocator) :
    m_HeapStack (defaultAllocator)
#ifndef FINAL
    ,mLowestFPS(2000), mHighestTris(0), mFPSLWTris(0), mHWTriFPS(0)
#endif
{

}


HeapManager::~HeapManager ()
{
    // Shut down all the heaps.
    for( int i = 0; i < NUM_GAME_MEMORY_ALLOCATORS; i++ )
    {
        DestroyHeapA( static_cast<GameMemoryAllocator>( i ) );
    }
}


void HeapManager::DestroyHeap (IRadMemoryHeap*& heap, bool justTest)
{
    if (heap)
    {
        // Check for heap not empty.
        // If it's not empty this is a Bad Thing.  We will be leaving dangling pointers to all the contained memory.
        // This either means we have a leak and they will never get deleted or they will get deleted eventually but
        // the heap will no longer exist.
        //
        // If you assert here, DO NOT IGNORE IT.  If you need help diagnosing the cause of the assertion, see Joel.
        //
        unsigned int numAllocs;
        heap->GetStatus (0, 0, &numAllocs, 0);
        if (numAllocs > 0)
        {
#ifndef FINAL
#ifndef RAD_RELEASE
            char s[64];
            sprintf (s, "%s is being destroyed but is not empty!", HeapNames[GetHeapID (heap)]);
            if ( CommandLineOptions::Get( CLO_MEMORY_MONITOR ) )
            {
                rTunePrintf( s );
                ::radMemoryMonitorSuspend ();
            }
            else
            {
//                rAssertMsg (0, s);
            }
#endif
#endif // RAD_RELEASE
        }

        if ( !justTest )
        {
            // Destroy the heap
            //
            ::radMemoryUnregisterAllocator (GetHeapID (heap));
            heap->Release ();
            heap = 0;
        }
    }
}


GameMemoryAllocator HeapManager::GetHeapID (const IRadMemoryHeap* heap)
{
/*
    if (s_pIRadMemoryHeapDefault && (heap == s_pIRadMemoryHeapDefault))
    {
        return GMA_DEFAULT;
    }
    if (s_pIRadMemoryHeapLevelMovie && (heap == s_pIRadMemoryHeapLevelMovie))
    {
        return GMA_LEVEL_MOVIE;
    }
    if (s_pIRadMemoryHeapDebug && (heap == s_pIRadMemoryHeapDebug))
    {
        return GMA_DEBUG;
    }
#ifdef RAD_GAMECUBE
    if (s_pIRadMemoryHeapVMM && (heap == s_pIRadMemoryHeapVMM))
    {
#ifdef NO_ARAM
        return GMA_LEVEL_OTHER;
#else
        return GMA_GC_VMM;
#endif
    }
#endif

    rAssert (false);
    */
    return GMA_DEFAULT;
}


float HeapManager::GetFudgeFactor ()
{
    float FUDGE;

#ifndef RAD_RELEASE
    if( CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        FUDGE = 1.0f;
    }
    else
    {
        #if defined( RAD_GAMECUBE ) && defined( RAD_TUNE )
            FUDGE = 1.0f;
        #else
            #ifdef RAD_MW
                FUDGE = 1.5f;
            #else
                FUDGE = 2.0f;
            #endif
        #endif
    }
#else
    if( CommandLineOptions::Get( CLO_LARGEHEAPS ) )
    {
        FUDGE = 1.5f;
    }
    else
    {
        FUDGE = 1.0f;
    }
#endif

    return FUDGE;
}


void HeapManager::PushHeap (GameMemoryAllocator alloc)
{
#ifndef RAD_RELEASE
    static bool init = false;
    if (!init)
    {
        init = true;
        //::radMemorySetActivityCallback (&g_HeapActivityTracker);
    }
#endif

    m_HeapStack.Push (alloc);
}

void HeapManager::PopHeap( GameMemoryAllocator alloc )
{
    m_HeapStack.Pop( alloc );
}

GameMemoryAllocator HeapManager::GetCurrentHeap () const
{
#ifndef RAD_RELEASE
    if (s_bSpecialRoute)
    {
        return GMA_SPECIAL;
    }
#endif

    return m_HeapStack.Top ();
}


#ifdef MEMORYTRACKER_ENABLED

void HeapManager::PushGroup( const char* name )
{
    rAssert( strcmp( name, "ScroobyBootup" ) != 0 );
    HeapMgr()->PushHeap (GMA_DEBUG);

    MemoryIDString str;
    strncpy (str.id, name, MAXSTRING);

    m_GroupIDStack.push (str);

    HeapMgr()->PopHeap ( GMA_DEBUG );
}


void HeapManager::PopGroup ( const char* name )
{
    HeapMgr()->PushHeap( GMA_DEBUG );

    rAssert( !m_GroupIDStack.empty () );
    MemoryIDString str = m_GroupIDStack.top();
    int equal = strncmp( name, str.id, MAXSTRING );
    rAssert( equal == 0 );
    m_GroupIDStack.pop();

    HeapMgr()->PopHeap( GMA_DEBUG );
}


const char* HeapManager::GetCurrentGroupID () const
{
    if (m_GroupIDStack.empty ())
    {
        return 0;
    }
    else
    {
        return m_GroupIDStack.top ().id;
    }
}


void HeapManager::PushFlag (const char* name)
{
    HeapMgr()->PushHeap (GMA_DEBUG);

    MemoryIDString str;
    strncpy (str.id, name, MAXSTRING);

    m_FlagStack.push (str);

    ::radMemoryMonitorIssueFlag (name);

    HeapMgr()->PopHeap (GMA_DEBUG);
}


void HeapManager::PopFlag (const char* name)
{
    HeapMgr()->PushHeap (GMA_DEBUG);

    assert (!m_FlagStack.empty ());
    MemoryIDString str = m_GroupIDStack.top();
    int equal = strncmp( name, str.id, MAXSTRING );
    //rAssert( equal == 0 );
    m_FlagStack.pop ();

    if (m_FlagStack.empty ())
    {
        ::radMemoryMonitorIssueFlag ("");
    }
    else
    {
        ::radMemoryMonitorIssueFlag (m_FlagStack.top ().id);
    }

    HeapMgr()->PopHeap (GMA_DEBUG);
}


#endif


radMemoryAllocator HeapManager::GetCurrentAllocator ()
{
    return GetCurrentHeap ();
}


radMemoryAllocator HeapManager::SetCurrentAllocator ( radMemoryAllocator allocator )
{
    radMemoryAllocator old = GetCurrentHeap ();
    m_HeapStack.SetTop ((GameMemoryAllocator)allocator);
    return old;
}


void HeapManager::DumpHeapStats ( bool text )
{
//#ifndef FINAL
//#ifndef RAD_GAMECUBE
    struct HeapInfo
    {
        GameMemoryAllocator gma;
        const char* name;
        IRadMemoryHeap** heap;
        unsigned int highwater;
    };

#ifdef RAD_GAMECUBE
    if ( vmmHeap && !s_pIRadMemoryHeapVMM )
    {
        //The virtual memory heap.
        rReleaseString ("Creating Heap: VMM\n");
        s_pIRadMemoryHeapVMM = vmmHeap;
        s_pIRadMemoryHeapVMM->AddRef();
    }
#endif

/*

    HeapInfo info[] = 
    { 
        { GMA_DEFAULT, "Default", &s_pIRadMemoryHeapDefault, 0 },
        { GMA_TEMP, "Temp", &s_pIRadMemoryHeapTemp, 0 },
        { GMA_PERSISTENT, "Persist", &s_pIRadMemoryHeapPersistent, 0  },
        { GMA_MUSIC, "Music", &s_pIRadMemoryHeapMusic, 0 },
        { GMA_AUDIO_PERSISTENT, "Audio Persistent", &s_pIRadMemoryHeapAudioPersistent, 0 },
        { GMA_LEVEL, "Level", &s_pIRadMemoryHeapLevel, 0 },
            { GMA_LEVEL_MOVIE, "- Movie", &s_pIRadMemoryHeapLevelMovie, 0 },
            { GMA_LEVEL_FE, "- FE", &s_pIRadMemoryHeapLevelFE, 0 },

            { GMA_LEVEL_ZONE, "- Zone", &s_pIRadMemoryHeapLevelZone, 0 },
//            { GMA_LEVEL_OTHER, "- Other", &s_pIRadMemoryHeapLevelOther, 0 },
            { GMA_LEVEL_HUD, "- HUD", &s_pIRadMemoryHeapLevelHUD, 0 },
//            { GMA_LEVEL_MISSION, "- Mission", &s_pIRadMemoryHeapLevelMission, 0 },

            { GMA_LEVEL_AUDIO, "- Audio", &s_pIRadMemoryHeapLevelAudio, 0 },
    
        { GMA_GC_VMM, "VMM", &s_pIRadMemoryHeapVMM, 0 }
    };
	*/

	HeapInfo info[] = 
    { 
        { GMA_DEFAULT,    "Default", GetHeapReference( GMA_DEFAULT ),    0 },
        { GMA_TEMP,       "Temp",    GetHeapReference( GMA_TEMP ),       0 },
        { GMA_PERSISTENT, "Persist", GetHeapReference( GMA_PERSISTENT ), 0 },
        { GMA_MUSIC,      "Music",   GetHeapReference( GMA_MUSIC ),      0 },
        { GMA_AUDIO_PERSISTENT, "Audio Persistent", GetHeapReference( GMA_AUDIO_PERSISTENT ), 0 },
        { GMA_LEVEL_ZONE, "Zones",   GetHeapReference( GMA_LEVEL_ZONE ), 0 },
        { GMA_SMALL_ALLOC, "Small Alloc", GetHeapReference( GMA_SMALL_ALLOC ),    0 },
#ifdef USE_CHAR_GAG_HEAP
        { GMA_CHARS_AND_GAGS, "Chars and Gags", GetHeapReference( GMA_CHARS_AND_GAGS ), 0 },
#endif
//        { GMA_LEVEL_MISSION, "Mission", &s_pIRadMemoryHeapLevelMission, 0 },
        { GMA_LEVEL_AUDIO, "Audio",  GetHeapReference( GMA_LEVEL_AUDIO ),0 },
        { GMA_LEVEL_FE,    "F/E",    GetHeapReference( GMA_LEVEL_FE ),   0 },
        { GMA_LEVEL_HUD,   "HUD",    GetHeapReference( GMA_LEVEL_HUD ),  0 }
//        { GMA_LEVEL_OTHER, "Other", &s_pIRadMemoryHeapLevelOther, 0 }
#ifdef RAD_GAMECUBE
        ,{ GMA_GC_VMM, "VMM", &s_pIRadMemoryHeapVMM, 0 }
#endif
    };

    if ( text )
    {
        size_t used         = Memory::GetTotalMemoryUsed();
        size_t available    = Memory::GetTotalMemoryFree();
        size_t unavailable  = Memory::GetTotalMemoryUnavailable();
        size_t lowWater     = Memory::GetTotalMemoryFreeLowWaterMark();
        size_t freeInAllHeaps = GetTotalMemoryFreeInAllHeaps();
        //size_t allocatable  = Memory::GetFreeMemoryProfile();
        size_t allocatable  = 0;
        size_t largestBlock = Memory::GetLargestFreeBlock();
        char buffer[ 256 ];
        sprintf( buffer, "Used: %d\n", used );
        rReleaseString( buffer );
        sprintf( buffer, "Free: %d Free in Malloc: %d\n", freeInAllHeaps, available );
        rReleaseString( buffer );
        sprintf( buffer, "Unavailable: %d\n", unavailable );
        rReleaseString( buffer );
        sprintf( buffer, "LargestFragment: %d\n", largestBlock );
        rReleaseString( buffer );
        sprintf( buffer, "Allocatable: %d\n", allocatable );
        rReleaseString( buffer );
        sprintf( buffer, "LowWater: %d\n", lowWater );
        rReleaseString( buffer );


        int i;
        int line = 0;
        for( i = 0; i < static_cast<int>(sizeof(info) / sizeof(info[0])); ++i )
        {

            IRadMemoryHeap* heap = *(info[i].heap);
            if(i==4)
            {
               // rReleasePrintf("\nheap %s size: %d", info[i].name, heap->GetSize() ); 
            }
            if (heap)
            {
                unsigned int size;
                size = heap->GetSize();

                unsigned int totalFree, largestBlock, numObjects, highWater;
                heap->GetStatus( &totalFree, &largestBlock, &numObjects, &highWater );
                unsigned int used = size - totalFree;

                float percentUsed = static_cast<float>( used ) / static_cast<float>( size ) * 100.0f;
                float highWaterPercentUsed = static_cast<float>( highWater ) / static_cast<float>( size ) * 100.0f;
                float usedMB = (float)used / MB;
                float sizeMB = (float)size / MB;
                float hwMB = (float)highWater / MB;

                char buffy[256];
                sprintf( buffy, "%8s %d/%d [%2.1f%%] HW: %2.2f [%2.1f%%] %d\n", info[i].name, used, size, percentUsed, hwMB, highWaterPercentUsed, numObjects );
                rReleaseString( buffy );
                ++line;
            }
        }
    }

    const pddiColour BLACK( 0, 0, 0 );
    const pddiColour ORANGE(255,127,0);
    const pddiColour WHITE(255,255,255);
    const pddiColour WHITE_50(255,255,255,128);
    const int LEFT = 30;
    const int TOP = 200;

    if( CommandLineOptions::Get( CLO_HEAP_STATS ) )
    {
        //
        // What does Malloc have left
        //

        static pddiShader* shader = p3d::device->NewShader( "simple" );
        shader->SetInt(PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT); 
        shader->SetInt(PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA);

        pddiProjectionMode proj = p3d::pddi->GetProjectionMode();
        p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);
        p3d::pddi->PushIdentityMatrix(PDDI_MATRIX_MODELVIEW);
        pddiPrimStream* stream;
        stream = p3d::pddi->BeginPrims(shader, PDDI_PRIM_TRISTRIP, PDDI_V_C, 4 );
        float z = 1.001f;
        float xMin = 0.0f;
        float xMax = 500.0f;
        float yMin =   0.0f + TOP;
        float yMax = 250.0f + TOP;
        stream->Colour(WHITE_50);
        stream->Coord(  0.0f,  yMin, z );
        stream->Colour(WHITE_50);
        stream->Coord( xMax,  yMin, z );
        stream->Colour(WHITE_50);
        stream->Coord( 0.0f, yMax, z );
        stream->Colour(WHITE_50);
        stream->Coord(  xMax, yMax, z );
        p3d::pddi->EndPrims(stream);
        p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);
        p3d::pddi->SetProjectionMode(proj);

        size_t used         = Memory::GetTotalMemoryUsed();
        size_t available    = Memory::GetTotalMemoryFree();
        size_t unavailable  = Memory::GetTotalMemoryUnavailable();
        size_t lowWater     = Memory::GetTotalMemoryFreeLowWaterMark();
        size_t freeInAllHeaps = GetTotalMemoryFreeInAllHeaps();
        //size_t allocatable  = Memory::GetFreeMemoryProfile();
        size_t allocatable  = 0;
        size_t largestBlock = Memory::GetLargestFreeBlock();
        char buffer[ 256 ];
        int printLine = TOP - 10;
        sprintf( buffer, "Used: %d", used );
        p3d::pddi->DrawString( buffer, LEFT, printLine += 15 , BLACK );
        sprintf( buffer, "Free: %d Free in Malloc: %d", freeInAllHeaps, available );
        p3d::pddi->DrawString( buffer, LEFT, printLine += 15 , BLACK );
        sprintf( buffer, "Unavailable: %d", unavailable );
        p3d::pddi->DrawString( buffer, LEFT, printLine += 15 , BLACK );
        sprintf( buffer, "LargestFragment: %d", largestBlock );
        p3d::pddi->DrawString( buffer, LEFT, printLine += 15 , BLACK );
        sprintf( buffer, "Allocatable: %d", allocatable );
        p3d::pddi->DrawString( buffer, LEFT, printLine += 15 , BLACK );
        sprintf( buffer, "LowWater: %d", lowWater );
        p3d::pddi->DrawString( buffer, LEFT, printLine += 15 , BLACK );
        

        int i;
        int line = 0;
        for( i = 0; i < static_cast<int>(sizeof(info) / sizeof(info[0])); ++i )
        {

            IRadMemoryHeap* heap = *(info[i].heap);
            if (heap)
            {
                unsigned int size;
                size = heap->GetSize();

                unsigned int totalFree, largestBlock, numObjects, highWater;
                heap->GetStatus( &totalFree, &largestBlock, &numObjects, &highWater );
                unsigned int used = size - totalFree;

                float percentUsed = static_cast<float>( used ) / static_cast<float>( size ) * 100.0f;
                float highWaterPercentUsed = static_cast<float>( highWater ) / static_cast<float>( size ) * 100.0f;
                float usedMB = (float)used / MB;
                float sizeMB = (float)size / MB;
                float hwMB = (float)highWater / MB;

                char buffy[256];
                sprintf( buffy, "%8s %d/%d [%2.1f%%] HW: %2.2f [%2.1f%%] %d", info[i].name, used, size, percentUsed, hwMB, highWaterPercentUsed, numObjects );
                p3d::pddi->DrawString( buffy,
                                    LEFT , 
                                    100 + TOP + (line*20),
                                    BLACK );
                ++line;
            }
        }
    }

//#endif // !RAD_GAMECUBE

//#endif // !FINAL
}

void HeapManager::ResetArtStats()
{
#ifndef FINAL
  mLowestFPS = 2000;
  mHighestTris = 0;
  mFPSLWTris = 0;
  mHWTriFPS = 0;
#endif
}

void HeapManager::DumpArtStats ()
{
#ifndef FINAL

    struct HeapInfo
    {
        GameMemoryAllocator gma;
        const char* name;
        IRadMemoryHeap** heap;
        unsigned int highwater;
    };

    static HeapInfo info[] = 
    { 
        { GMA_LEVEL_ZONE, "Zones", GetHeapReference( GMA_LEVEL_ZONE ), 0 },
//        { GMA_LEVEL_MISSION, "Mission", &s_pIRadMemoryHeapLevelMission, 0 },
//        { GMA_LEVEL_OTHER, "Other", &s_pIRadMemoryHeapLevelOther, 0 }
    };

    if( CommandLineOptions::Get( CLO_ART_STATS ) )
    {
        const int LEFT = 35;
        const int TOP = 285;
        const pddiColour BLACK(0,0,0);
        const pddiColour WHITE(128,128,128);

        int i;
        int count = static_cast<int>(sizeof(info) / sizeof(info[0]));
        for( i = 0; i < count; ++i )
        {
            IRadMemoryHeap* heap = *(info[i].heap);
            if (heap)
            {
                unsigned int size;
                size = heap->GetSize();

                unsigned int totalFree, largestBlock, numObjects, highWater;
                heap->GetStatus( &totalFree, &largestBlock, &numObjects, &highWater );

                unsigned int used = size - totalFree;
                float percentUsed = static_cast<float>( used ) / static_cast<float>( size ) * 100.0f;
                float highWaterPercentUsed = static_cast<float>( highWater ) / static_cast<float>( size ) * 100.0f;
                float usedMB = (float)used / MB;
                float sizeMB = (float)size / MB;
                float hwMB = (float)info[i].highwater / MB;

                char buffy[256];
                sprintf( buffy, "%8s %2.2f/%2.2f [%2.1f%%] HW: %2.2f [%2.1f%%]", info[i].name, usedMB, sizeMB, percentUsed, hwMB, highWaterPercentUsed );

                p3d::pddi->DrawString( buffy,
                                       LEFT , 
                                       TOP + (i*20),
                                       WHITE );
            }
        }
			
        char buff1[256];
		char buff2[256];
		char buff3[256];
		char buff4[256];
		float memtextures = 0;
		float memmeshes = 0;
		int fps=0,tris;
            
        int frameTime = p3d::pddi->GetIntStat(PDDI_STAT_FRAME_TIME);
        if( frameTime == 0 )
        {
            frameTime = 1;
        }
		fps = 1000 / ( frameTime );
#ifdef RAD_RELEASE
	    tris = 300000;
#else
	    tris = p3d::pddi->GetIntStat(PDDI_STAT_BUFFERED_PRIM) + p3d::pddi->GetIntStat(PDDI_STAT_BUFFERED_INDEXED_PRIM);
#endif

        if(tris>20000 && fps>12)
        {
            if(fps<mLowestFPS)
            {
                mLowestFPS = fps;
                mFPSLWTris = tris;
            }
        }
        if(tris>mHighestTris)
        {
            mHighestTris = tris;
            mHWTriFPS    = fps;
        }

		memtextures = p3d::pddi->GetFloatStat(PDDI_STAT_TEXTURE_ALLOC_8BIT);
		memtextures = memtextures /1024;
		memmeshes  = p3d::pddi->GetFloatStat(PDDI_STAT_BUFFERED_ALLOC);
		memmeshes  = memmeshes/1024 ;

		sprintf(buff1,"FPS: %d \n", fps);
		sprintf(buff2,"# Textures 8bit: %d ,MemUsage %2.2f MB \n",p3d::pddi->GetIntStat(PDDI_STAT_TEXTURE_COUNT_8BIT),memtextures );
		sprintf(buff3,"# Meshes: %d , MemUsage %2.2f MB \n",p3d::pddi->GetIntStat(PDDI_STAT_BUFFERED_COUNT),memmeshes);
        sprintf(buff4,"Tri's: %d Tri(HW): %d, FPS at TriHW: %d\n FPS(LW):%d, Tri's at FPS LW: %d\n",tris,mHighestTris,mHWTriFPS,
                                                                                                    mLowestFPS,mFPSLWTris);
		
		p3d::pddi->DrawString(buff1,LEFT,TOP+(count*20)+ 0,WHITE);
        p3d::pddi->DrawString(buff2,LEFT,TOP+(count*20)+20,WHITE);
		p3d::pddi->DrawString(buff3,LEFT,TOP+(count*20)+40,WHITE);
		p3d::pddi->DrawString(buff4,LEFT,TOP+(count*20)+60,WHITE);
		p3d::pddi->EnableStatsOverlay(0); //disable the regular p3d debug stats.
    }
#endif
}


//Memory Layout.
//--HS_DEFAULT
//--HS_TEMP
//--HS_MUSIC
//--HS_AUDIO_PERSISTENT
//--HS_PERSISTENT
//--HS_LEVEL
// ||
// | --HS_LEVEL_MOVIE     -|
// | --HS_LEVEL_AUDIO_FE   | - Front end sub heaps.
// | --HS_LEVEL_FE        -|
// |
//  --HS_LEVEL_ZONE         -|
//  --HS_LEVEL_OTHER         |
//  --HS_LEVEL_HUD           | - In-game sub heaps.
//  --HS_LEVEL_MISSION       |
//  --HS_LEVEL_AUDIO_INGAME -|

//==================================
//These only exist in debug and tune

//--HS_DEBUG
//--HS_DEBUG_FIREWIRE
//--HS_SPECIAL

// These constants are the heap sizes for each platform
//
#if defined (RAD_PS2)
    #ifdef RAD_RELEASE
    const float HS_DEFAULT = 0.12f;  // For only very core FTech stuff      (DO NOT change this)
    #else                            //                                       If you run out of room in GMA_DEFAULT,
                                    //                                        you have an unrouted allocation
    const float HS_DEFAULT = 0.13f;  // For that plus debug comm stuff, etc (DO NOT change this)
    #endif

    #ifdef CORRAL_SMALL_ALLOCS
        const float HS_TEMP = 0.350000f;
    #else
        const float HS_TEMP = 0.550000f;
    #endif

    #ifdef PAL
        // an additional 4-5K is needed for PAL builds, due to other languages
        // in the text bible using up slightly more memory
        //
        const float HS_PERSISTENT = 1.905f;
    #else
        const float HS_PERSISTENT = 1.900f;
    #endif

    const float HS_MUSIC = 0.2f;
    const float HS_AUDIO_PERSISTENT = 0.62f;//0.693000f;

    //FE Only
    const float HS_LEVEL_MOVIE = 1.94f;
    const float HS_LEVEL_AUDIO_FE = 0.05f;
    const float HS_LEVEL_FE = 14.0f;
    //In-game Only
    //const float HS_LEVEL_ZONE = 11.25f;
    const float HS_LEVEL_ZONE = 11.50f;
    //const float HS_LEVEL_OTHER = 6.48f;
    #ifdef CORRAL_SMALL_ALLOCS
        const float HS_LEVEL_HUD = 1.6700f;
    #else
        const float HS_LEVEL_HUD =  2.51f;;
    #endif

    const float HS_LEVEL_AUDIO_INGAME = 0.05f;

    // level heap is the sum of other heaps
    const float HS_LEVEL = 0.01 + rmt::Max( ( HS_LEVEL_ZONE + HS_LEVEL_HUD + HS_LEVEL_AUDIO_INGAME ), ( HS_LEVEL_FE + HS_LEVEL_AUDIO_FE + HS_LEVEL_MOVIE ) );

    //Mnigame Only
    const float HS_MINIGAME_ZONE        = 3.0f;
    const float HS_MINIGAME_OTHER       = 7.7f;
    const float HS_MINIGAME_HUD         = 2.3f;
    const float HS_MINIGAME_MISSION     = 2.0f;
    const float HS_MINIGAME_AUDIO       = 0.5f;

    #ifndef RAD_RELEASE
    const float HS_DEBUG = 5.0f;
    const float HS_DEBUG_FIREWIRE = 0.4f;
    const float HS_SPECIAL = 10.0f;
    #endif

#elif defined (RAD_GAMECUBE)
    //NOTE:  The VMM is using 1 MEG - See VMM_MAIN_RAM in gcplatform.h
    //NOTE2: GameCube libraries now allocate from GMA_AUDIO_PERSISTENT rather than OSAlloc,
    //NOTE3: There is also a static heap defined in radCore/radmemory memorymanager.cpp of 
    //       STATIC_HEAP_SIZE ( 1024 * 1024 * 2 ) + ( 600 * 1024 ) = 2.6 Megs This is replacing the
    //       regular persistent heap.
    //NOTE4: There is also an amount of emergency memory in radmemory (memorymanager) in the initializePlatform
    //       for printfs and such.  It is 32 * 1024 bytes = 32K
    const float HS_DEFAULT              = 0.08f; //0.13f // For only very core FTech stuff
    const float HS_TEMP                 = 0.3f; //0.37f;

    const float HS_PERSISTENT           = 2.56f; //Need extra 8K for PAL
    const float HS_MUSIC                = 0.2f;
    const float HS_AUDIO_PERSISTENT     = 0.65f;
    const float HS_LEVEL                = 12.95f; // a little extra sub-heap creation overhead
    //FE Only
    const float HS_LEVEL_MOVIE          = 3.1f;
    const float HS_LEVEL_AUDIO_FE       = 0.1f;
    const float HS_LEVEL_FE             = 9.75f;
    //In-game Only 
    const float HS_LEVEL_ZONE           = 6.48f;
    const float HS_LEVEL_OTHER          = 3.20f;
    const float HS_LEVEL_HUD            = 2.50f;
    //const float HS_LEVEL_MISSION        = 0.0f;
    const float HS_LEVEL_AUDIO_INGAME   = 0.04f;
    //Mnigame Only
    const float HS_MINIGAME_ZONE        = 3.0f;
    const float HS_MINIGAME_OTHER       = 7.5f;
    const float HS_MINIGAME_HUD         = 1.7f;
    const float HS_MINIGAME_MISSION     = 2.0f;
    const float HS_MINIGAME_AUDIO       = 0.5f;

    #ifndef RAD_RELEASE
    const float HS_DEBUG                = 2.0f;
    const float HS_DEBUG_FIREWIRE       = 99999.99f;
    const float HS_SPECIAL              = 0.0f;
    #endif

#elif defined (RAD_XBOX)
    #ifdef RAD_RELEASE
    const float HS_DEFAULT = 0.1f;  // For only very core FTech stuff
    #else
    const float HS_DEFAULT = 0.4f;   // For that plus debug comm stuff, etc
    #endif
    const float HS_TEMP = 1.0f;
    const float HS_PERSISTENT = 1.65f;
    const float HS_MUSIC = 0.2f;
    const float HS_AUDIO_PERSISTENT = 0.7f;
    const float HS_LEVEL = 20.01f;    // 0.01 for sub-heap creation overhead
    //FE Only
    const float HS_LEVEL_MOVIE = 3.94f; // TC: added extra 2.0 MB until memory leak
                                        //     in radMovie Bink player is fixed
    const float HS_LEVEL_AUDIO_FE = 0.05f;
    const float HS_LEVEL_FE = 5.0f;
    //In-game Only
    const float HS_LEVEL_ZONE = 8.0f;
    //const float HS_LEVEL_OTHER = 5.0f;
    const float HS_LEVEL_HUD = 2.5f;
    //const float HS_LEVEL_MISSION = 2.8f;
    const float HS_LEVEL_AUDIO_INGAME = 0.05f;
    //Mnigame Only
    const float HS_MINIGAME_ZONE        = 3.0f;
    const float HS_MINIGAME_OTHER       = 8.0f;
    const float HS_MINIGAME_HUD         = 1.55f;
    const float HS_MINIGAME_MISSION     = 2.0f;
    const float HS_MINIGAME_AUDIO       = 0.5f;

    #ifndef RAD_RELEASE
    const float HS_DEBUG = 5.0f;
    const float HS_DEBUG_FIREWIRE = 0.4f;
    const float HS_SPECIAL = 10.0f;
    #endif

#elif defined (RAD_WIN32) // these have not been optimized yet.
    #ifdef RAD_RELEASE
    const float HS_DEFAULT = 0.1f;  // For only very core FTech stuff
    #else
    const float HS_DEFAULT = 0.4f;   // For that plus debug comm stuff, etc
    #endif
    const float HS_TEMP = 1.0f;
    const float HS_PERSISTENT = 1.65f;
    const float HS_MUSIC = 0.2f;
    const float HS_AUDIO_PERSISTENT = 2.7f;
    const float HS_LEVEL = 20.01f;    // 0.01 for sub-heap creation overhead
    //FE Only
    const float HS_LEVEL_MOVIE = 3.94f; // TC: added extra 2.0 MB until memory leak
    //     in radMovie Bink player is fixed
    const float HS_LEVEL_AUDIO_FE = 0.05f;
    const float HS_LEVEL_FE = 5.0f;
    //In-game Only
    const float HS_LEVEL_ZONE = 8.0f;
    //const float HS_LEVEL_OTHER = 5.0f;
    const float HS_LEVEL_HUD = 2.5f;
    //const float HS_LEVEL_MISSION = 2.8f;
    const float HS_LEVEL_AUDIO_INGAME = 0.05f;
    //Mnigame Only
    const float HS_MINIGAME_ZONE        = 3.0f;
    const float HS_MINIGAME_OTHER       = 8.0f;
    const float HS_MINIGAME_HUD         = 1.55f;
    const float HS_MINIGAME_MISSION     = 2.0f;
    const float HS_MINIGAME_AUDIO       = 0.5f;

    #ifndef RAD_RELEASE
    const float HS_DEBUG = 5.0f;
    const float HS_DEBUG_FIREWIRE = 0.4f;
    const float HS_SPECIAL = 10.0f;
    #endif

#endif


#define ALL_DL_HEAPS

void HeapManager::PrepareHeapsStartup ()
{
    //
    // Setup our memory heaps.
    //
    const float FUDGE = GetFudgeFactor ();

    // The temporary heap.
    // This heap is for temporary allocations that happen during loading, to prevent fragmentation.
    //
    CreateHeap( GMA_TEMP, static_cast<unsigned int>(HS_TEMP * MB * FUDGE) );

    // The persistent heap.
    // This heap holds everything that is allocated at the beginning of the game and never gets freed.
    //
    float persistent_size = HS_PERSISTENT;
#ifdef RAD_PS2
#ifndef RAD_RELEASE
    // On the PS2, in non-release, radSound allocates about 0.1 MB more for radRemoteScript
    //
    persistent_size += 0.1f;
#endif

    // On the PS2, the host drive mount costs about 254K more than the CD drive mount
    //
    if (!CommandLineOptions::Get( CLO_CD_FILES_ONLY ))
    {
        persistent_size += 0.25f;
    }
#endif

    CreateHeap( GMA_PERSISTENT, static_cast<unsigned int>(persistent_size * MB * FUDGE) );

//#ifndef RAD_GAMECUBE
    //
    // The audio persistent heap.
    // This heap holds allocations that audio makes that are done once
    // and persist throughout the game.  They aren't necessarily done
    // at start up, hence its own heap
    //
#if defined( RAD_GAMECUBE ) || defined( RAD_PS2 )
    CreateHeap( GMA_AUDIO_PERSISTENT, static_cast< int >( HS_AUDIO_PERSISTENT * FUDGE * MB ) );
    IRadMemoryAllocator* audioHeap = GetAllocator( GMA_AUDIO_PERSISTENT );
    radMemoryRegisterAllocator( GMA_MUSIC,            RADMEMORY_ALLOC_DEFAULT, audioHeap );
#endif
#ifdef CORRAL_SMALL_ALLOCS   
    CreateHeap( GMA_SMALL_ALLOC, (int)(((float)4*(1024*1024))*FUDGE+(200*1024)));//-170608) );
    extern bool gbSmallAllocCreated;
    gbSmallAllocCreated = true;
#endif
//#else
//    radMemoryRegisterAllocator( GMA_AUDIO_PERSISTENT, RADMEMORY_ALLOC_VMM, vmmHeap );
//    radMemoryRegisterAllocator( GMA_MUSIC, RADMEMORY_ALLOC_VMM, vmmHeap );
//#endif

#ifdef RAD_GAMECUBE 
    // The default heap.
    // This heap holds everything we are otherwise unable to route to a heap.
    // This includes objects initialized before the heaps are created.
    //
    unsigned int size = Memory::GetTotalMemoryFree() - 3 * 1024;
    CreateHeap( GMA_DEFAULT, size );
    IRadMemoryAllocator* defaultHeap = GetAllocator( GMA_DEFAULT );
    //radMemoryRegisterAllocator( GMA_TEMP,             RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_MOVIE,      RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_FE,         RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_ZONE,       RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_OTHER,      RADMEMORY_ALLOC_DEFAULT, defaultHeap );
//    radMemoryRegisterAllocator( GMA_LEVEL_HUD,        RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_MISSION,    RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_AUDIO,      RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_DEBUG,            RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_SPECIAL,          RADMEMORY_ALLOC_DEFAULT, defaultHeap );
//    radMemoryRegisterAllocator( GMA_MUSIC,            RADMEMORY_ALLOC_DEFAULT, defaultHeap );
//    radMemoryRegisterAllocator( GMA_AUDIO_PERSISTENT, RADMEMORY_ALLOC_DEFAULT, defaultHeap );
#endif

#if defined(RAD_PS2) && defined(ALL_DL_HEAPS) 
    unsigned int fudgeFactor;

    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR ) )
    {
        fudgeFactor = 64 * 1024;
    }
    else
    {
        #ifdef RAD_TUNE
            fudgeFactor = 64 * 1024;
        #else
            fudgeFactor = 16 * 1024;
        #endif
    }

    //unsigned int size = Memory::GetTotalMemoryFree() - fudgeFactor;// - 256 * 1024; //mfifo
    unsigned int size = Memory::GetLargestFreeBlock() - fudgeFactor;// - 256 * 1024; //mfifo
    #ifdef RAD_MW
        size -= (512 * 1024);
    #endif
        //size -= (512*1024);
    CreateHeap( GMA_DEFAULT, size );
    IRadMemoryAllocator* defaultHeap = GetAllocator( GMA_DEFAULT );
    //radMemoryRegisterAllocator( GMA_TEMP,             RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_MOVIE,      RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_FE,         RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_ZONE,       RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_OTHER,      RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    //radMemoryRegisterAllocator( GMA_LEVEL_HUD,        RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_MISSION,    RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_LEVEL_AUDIO,      RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_DEBUG,            RADMEMORY_ALLOC_DEFAULT, defaultHeap );
    radMemoryRegisterAllocator( GMA_SPECIAL,          RADMEMORY_ALLOC_DEFAULT, defaultHeap );
//    radMemoryRegisterAllocator( GMA_MUSIC,            RADMEMORY_ALLOC_DEFAULT, defaultHeap );
//    radMemoryRegisterAllocator( GMA_AUDIO_PERSISTENT, RADMEMORY_ALLOC_DEFAULT, defaultHeap );

#endif

    if (CommandLineOptions::Get( CLO_NO_HEAPS ))
    {
        return;
    }

    //
    // The music heap.
    // This heap holds allocations related to radMusic.  It's a bit of a tweener,
    // since it can allocate/deallocate unpredictably, and can occasionally
    // persist temporarily over the FE/game crossover.  It doesn't seem to fit
    // anywhere else, hence its own heap.
    //
//    CreateHeap( GMA_MUSIC, static_cast< unsigned int >( HS_MUSIC * FUDGE ) );

    //
    // The audio persistent heap.
    // This heap holds allocations that audio makes that are done once
    // and persist throughout the game.  They aren't necessarily done
    // at start up, hence its own heap
    //
    rReleasePrintf( "Creating Heap: AUDIO PERSISTENT (%f)\n", HS_AUDIO_PERSISTENT * FUDGE );
    //s_pIRadMemoryHeapAudioPersistent = radMemoryCreateDougLeaHeap( static_cast<unsigned int>( HS_AUDIO_PERSISTENT * MB * FUDGE ), RADMEMORY_ALLOC_DEFAULT, "Audio Persistent" );
    //s_pIRadMemoryHeapAudioPersistent->AddRef();
    //radMemoryRegisterAllocator( GMA_AUDIO_PERSISTENT, RADMEMORY_ALLOC_DEFAULT, s_pIRadMemoryHeapAudioPersistent );

    // The level heap.
    // This is the main heap for holding game data.  This includes the front end and all in-game assets.
    // This is the largest heap.
    //
    //rReleasePrintf ("Creating Heap: LEVEL (%f)\n", HS_LEVEL * FUDGE);
    //s_pIRadMemoryHeapLevel = radMemoryCreateTrackingHeap( static_cast<unsigned int>(HS_LEVEL * MB * FUDGE), RADMEMORY_ALLOC_DEFAULT, "Level" );
    //s_pIRadMemoryHeapLevel->AddRef();
    //radMemoryRegisterAllocator( GMA_LEVEL, RADMEMORY_ALLOC_DEFAULT, s_pIRadMemoryHeapLevel );

#ifndef RAD_RELEASE
    // The debug heap.
    // This heap is for holding all debugging related materials, such as the host communication channel, the watcher, the profiler, etc.
    // It is not created in release mode.
    //
    float debugSize;
    if( CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        debugSize = HS_DEBUG_FIREWIRE;
    }
    else
    {
        debugSize = HS_DEBUG;
    }

    CreateHeap( GMA_DEBUG, static_cast<unsigned int>(debugSize * MB) );

    if ( !CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        // The special heap.
        // This heap is created in debug and tune only and is used for routing a group of allocations to a specific place.
        //
        CreateHeap( GMA_SPECIAL, static_cast<unsigned int>(HS_SPECIAL * MB) );
    }
#endif

    // The default heap.
    // This heap holds everything we are otherwise unable to route to a heap.
    // This includes objects initialized before the heaps are created.
    //
    //rReleasePrintf ("Creating Heap: DEFAULT (%f)\n", HS_DEFAULT);
    //s_pIRadMemoryHeapDefault = radMemoryCreateDougLeaHeap( static_cast<unsigned int>(HS_DEFAULT * MB), RADMEMORY_ALLOC_DEFAULT, "Default" );
    //s_pIRadMemoryHeapDefault->AddRef();
    //radMemoryRegisterAllocator( GMA_DEFAULT, RADMEMORY_ALLOC_DEFAULT, s_pIRadMemoryHeapDefault );

    /*
#ifdef RAD_GAMECUBE
    if ( vmmHeap )
    {
        //The virtual memory heap.
        rReleaseString ("Creating Heap: VMM\n");
        s_pIRadMemoryHeapVMM = vmmHeap;
        s_pIRadMemoryHeapVMM->AddRef();
    }

    // The default heap.
    // This heap holds everything we are otherwise unable to route to a heap.
    // This includes objects initialized before the heaps are created.
    //
    unsigned int size = Memory::GetTotalMemoryFree();
    rReleasePrintf ("Creating Heap: DEFAULT (%f)\n", HS_DEFAULT);
    s_pIRadMemoryHeapDefault = radMemoryCreateDougLeaHeap( static_cast<unsigned int>(size), RADMEMORY_ALLOC_DEFAULT, "Default" );
    s_pIRadMemoryHeapDefault->AddRef();
    radMemoryRegisterAllocator( GMA_DEFAULT, RADMEMORY_ALLOC_DEFAULT, s_pIRadMemoryHeapDefault );

#endif
*/
}
//=============================================================================
// HeapManager::PrepareHeapsFeCleanup
//=============================================================================
void HeapManager::PrepareHeapsFeCleanup()
{
    DestroyHeapA( GMA_LEVEL_HUD     );

    if (CommandLineOptions::Get( CLO_NO_HEAPS ))
    {
        return;
    }

    // Free in game heaps
    //
    DestroyHeapA( GMA_LEVEL_ZONE     );
    DestroyHeapA( GMA_LEVEL_OTHER    );
    DestroyHeapA( GMA_LEVEL_MISSION  );
    DestroyHeapA( GMA_LEVEL_AUDIO    );
#ifdef USE_CHAR_GAG_HEAP
    DestroyHeapA( GMA_CHARS_AND_GAGS );
#endif
}

//=============================================================================
// HeapManager::PrepareHeapsFeSetup
//=============================================================================
void HeapManager::PrepareHeapsFeSetup()
{
    if (CommandLineOptions::Get( CLO_NO_HEAPS ))
    {
        return;
    }
    const float FUDGE = GetFudgeFactor ();
    CreateHeap( GMA_LEVEL_FE,    static_cast<unsigned int>(HS_LEVEL_FE * FUDGE * MB) );
    CreateHeap( GMA_LEVEL_MOVIE, static_cast<unsigned int>(HS_LEVEL_MOVIE * MB) );
    CreateHeap( GMA_LEVEL_AUDIO, static_cast<unsigned int>(HS_LEVEL_AUDIO_FE * MB) );
}


void HeapManager::PrepareHeapsInGame ()
{
    const float FUDGE = GetFudgeFactor ();
    DestroyHeapA( GMA_LEVEL_MOVIE );

    DestroyHeapA( GMA_LEVEL_HUD );
    CreateHeap( GMA_LEVEL_HUD, static_cast<unsigned int>(HS_LEVEL_HUD * FUDGE * MB) );

#ifdef USE_CHAR_GAG_HEAP
    CreateHeap( GMA_CHARS_AND_GAGS, static_cast<unsigned int>(1.3f * MB) );
#endif

    if (CommandLineOptions::Get( CLO_NO_HEAPS ))
    {
        return;
    }

    DestroyHeapA( GMA_LEVEL_AUDIO );
    DestroyHeapA( GMA_LEVEL_FE    );
    DestroyHeapA( GMA_LEVEL_ZONE );

    // Create in game heaps
    //
    CreateHeap( GMA_LEVEL_ZONE, static_cast<unsigned int>(HS_LEVEL_ZONE * FUDGE * MB) );
    CreateHeap( GMA_LEVEL_AUDIO, static_cast<unsigned int>(HS_LEVEL_AUDIO_INGAME * FUDGE * MB) );
}

void HeapManager::PrepareHeapsSuperSprint ()
{
    const float FUDGE = GetFudgeFactor ();

    DestroyHeapA( GMA_LEVEL_HUD );
    CreateHeap( GMA_LEVEL_HUD, static_cast<unsigned int>(HS_MINIGAME_HUD * FUDGE * MB) );

    if (CommandLineOptions::Get( CLO_NO_HEAPS ))
    {
        return;
    }

    // Free front end heaps
    //
    DestroyHeapA( GMA_LEVEL_FE    );
    DestroyHeapA( GMA_LEVEL_MOVIE );
    DestroyHeapA( GMA_LEVEL_AUDIO );

    // Free in game heaps (this has to happen for the mission select case, i.e. jumping level to level)
    //
    DestroyHeapA( GMA_LEVEL_ZONE );
//    DestroyHeap (s_pIRadMemoryHeapLevelOther);
    DestroyHeapA( GMA_LEVEL_HUD );
//    DestroyHeap (s_pIRadMemoryHeapLevelMission);

    // Create in game heaps
    //
    CreateHeap( GMA_LEVEL_ZONE, static_cast<unsigned int>(HS_MINIGAME_ZONE * FUDGE * MB) );

//    rReleaseString ("Creating Heap: OTHER\n");
//    s_pIRadMemoryHeapLevelOther = radMemoryCreateDougLeaHeap ( static_cast<unsigned int>(HS_MINIGAME_OTHER * FUDGE * MB), GMA_LEVEL, "Other" );
//    s_pIRadMemoryHeapLevelOther->AddRef ();
//    radMemoryRegisterAllocator (GMA_LEVEL_OTHER, GMA_LEVEL, s_pIRadMemoryHeapLevelOther);

    CreateHeap( GMA_LEVEL_HUD, static_cast<unsigned int>(HS_MINIGAME_HUD * FUDGE * MB) );

//    rReleaseString ("Creating Heap: MISSION\n");
//    s_pIRadMemoryHeapLevelMission = radMemoryCreateDougLeaHeap ( static_cast<unsigned int>(HS_MINIGAME_MISSION * FUDGE * MB), GMA_LEVEL, "Mission" );
//    s_pIRadMemoryHeapLevelMission->AddRef ();
//    radMemoryRegisterAllocator (GMA_LEVEL_MISSION, GMA_LEVEL, s_pIRadMemoryHeapLevelMission);

    CreateHeap( GMA_LEVEL_AUDIO, static_cast<unsigned int>(HS_MINIGAME_AUDIO * FUDGE * MB) );

#ifdef USE_CHAR_GAG_HEAP
    CreateHeap( GMA_CHARS_AND_GAGS, static_cast<unsigned int>(1.0f * MB) );
#endif
}


int HeapManager::GetLoadedUsageFE ()
{
    return GetHeapUsage(  GetAllocator( GMA_LEVEL_FE    ) ) +
           GetHeapUsage ( GetAllocator( GMA_LEVEL_MOVIE ) ) +
           GetHeapUsage ( GetAllocator( GMA_LEVEL_AUDIO ) ) +
           GetSoundMemoryHeapUsage();
}


int HeapManager::GetLoadedUsageInGame ()
{
    return GetHeapUsage ( GetAllocator( GMA_LEVEL_ZONE ) )     +
//           GetHeapUsage (s_pIRadMemoryHeapLevelOther)   +
           GetHeapUsage ( GetAllocator( GMA_LEVEL_HUD ) )      +
//           GetHeapUsage (s_pIRadMemoryHeapLevelMission) +
           GetHeapUsage ( GetAllocator( GMA_LEVEL_AUDIO ) )    +
           GetSoundMemoryHeapUsage();
}


int HeapManager::GetLoadedUsageSuperSprint ()
{
    // Right now this should be the same as In Game loading
    //
    return GetLoadedUsageInGame ();
}

int HeapManager::GetLoadedUsage( enum GameMemoryAllocator allocator )
{
    return GetHeapUsage ( GetAllocator( allocator ) );
}

int HeapManager::GetHeapUsage (IRadMemoryAllocator* allocator)
{
    if( allocator != NULL )
    {
        unsigned int size, totalFree, largestBlock, numObjects, highWater;
        size = allocator->GetSize();
        allocator->GetStatus( &totalFree, &largestBlock, &numObjects, &highWater );
        return size - totalFree;
    }
    else
    {
        return 0;
    }
}

int HeapManager::GetSoundMemoryHeapUsage()
{
    int soundMemoryUsage = 0;

    #ifdef RAD_PS2
        unsigned int size;
        unsigned int totalFree;
        unsigned int largestBlock;
        unsigned int numObjects;
        IRadSoundHalMemoryRegion* soundMemory = ::radSoundHalSystemGet()->GetRootMemoryRegion();
        rAssert( soundMemory != NULL );

        size = soundMemory->GetSize();
        soundMemory->GetStats( &totalFree, &numObjects, &largestBlock, true );

        soundMemoryUsage = size - totalFree;
    #endif

    return( soundMemoryUsage );
}

HeapManager* HeapMgr ()
{
    return HeapManager::GetInstance ();
}

void LockPersistentHeap()
{
    g_LockedPersistentHeap = true;
}
