//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        srrmemory.h
//
// Description: Overides for new and delete.
//
// History:     3/20/2002 + Created -- Darwin Chau
//
//=============================================================================

#ifndef SRRMEMORY_H
#define SRRMEMORY_H

//========================================
// Nested Includes
//========================================
#include <radmemory.hpp>
#include <radthread.hpp>
#include <main/commandlineoptions.h>
#include <memory/stlallocators.h>
#include <stddef.h>
#include <vector>
#include <stack>

//========================================
// Forward References
//========================================

extern bool gMemorySystemInitialized;

//========================================
// Global Declarations
//========================================


// Enable memory monitor in tune and debug
//
#ifndef TOOLS
    #ifndef RAD_MW
        #ifndef RAD_RELEASE
            //#define MEMORYTRACKER_ENABLED
        #endif // RAD_RELEASE
    #endif // RAD_MW
#endif // TOOLS




//
// Override built-in new.
//
void* operator new( size_t size )
#ifdef RAD_PS2
#ifndef RAD_MW
throw( std::bad_alloc )
#endif
#endif
;

//
// Override built-in delete.
//
void  operator delete( void* pMemory )
#ifdef RAD_PS2
#ifndef RAD_MW
throw()
#endif
#endif
;

//
// Override built-in array new.
//
void* operator new[]( size_t size )
#ifdef RAD_PS2
#ifndef RAD_MW
throw( std::bad_alloc )
#endif
#endif
;

//
// Override built-in array delete.
//
void  operator delete[]( void* pMemory )
#ifdef RAD_PS2
#ifndef RAD_MW
throw()
#endif
#endif
;

#ifdef RAD_PS2
    #define CORRAL_SMALL_ALLOCS   
#endif

// #define USE_CHAR_GAG_HEAP

//
// Enumeration of the available memory heaps.
//
enum GameMemoryAllocator
{                                              // 
    GMA_DEFAULT = RADMEMORY_ALLOC_DEFAULT,     //    0         
    GMA_TEMP = RADMEMORY_ALLOC_TEMP,           //    1         
#ifdef RAD_GAMECUBE                                            
    GMA_GC_VMM = RADMEMORY_ALLOC_VMM,          //    2         
#endif                                                         
    GMA_PERSISTENT = 3,                        //    3         
    GMA_LEVEL,                                 //    4          
    GMA_LEVEL_MOVIE,                           //    5         
    GMA_LEVEL_FE,                              //    6         
    GMA_LEVEL_ZONE,                            //    7         
    GMA_LEVEL_OTHER,                           //    8         
    GMA_LEVEL_HUD,                             //    9         
    GMA_LEVEL_MISSION,                         //    10        
    GMA_LEVEL_AUDIO,                           //    11        
    GMA_DEBUG,                                 //    12        
    GMA_SPECIAL,                               //    13        
    GMA_MUSIC,                                 //    14        
    GMA_AUDIO_PERSISTENT,                      //    15        
    GMA_SMALL_ALLOC,                           //    16
#ifdef RAD_XBOX                                
    GMA_XBOX_SOUND_MEMORY,                     //    17           
#endif
#ifdef USE_CHAR_GAG_HEAP
    GMA_CHARS_AND_GAGS,
#else
    GMA_CHARS_AND_GAGS = GMA_LEVEL_OTHER,
#endif
    GMA_ANYWHERE_IN_LEVEL = 25,                //    25        
    GMA_ANYWHERE_IN_FE,                        //    26        
    GMA_EITHER_OTHER_OR_ZONE,                  //    27     

    GMA_ALLOCATOR_SEARCH = ALLOCATOR_SEARCH,   //   If you feel like using this one, see an example in FMVPlayer::LoadData
    NUM_GAME_MEMORY_ALLOCATORS
};

extern const char* HeapNames[];

void* operator new( size_t size, GameMemoryAllocator allocator );
void  operator delete( void* pMemory, GameMemoryAllocator allocator );

void* operator new[]( size_t size, GameMemoryAllocator allocator );
void  operator delete[]( void* pMemory, GameMemoryAllocator allocator );

void* FindFreeMemory( GameMemoryAllocator allocator, size_t size );  //Use with caution.  This is meant to be temporary.
void  LockPersistentHeap();
void  PrintOutOfMemoryMessage( void* userData, radMemoryAllocator heap, const unsigned int size );
void  SetupAllocatorSearch( GameMemoryAllocator allocator );


// These are for the global heap stack.  This stack maintains where unrouted allocations
// should go.  This way we are able to trap virtually every allocation.
//

class HeapStack
{
public:
    HeapStack (GameMemoryAllocator defaultAllocator);
    virtual ~HeapStack ();

    void Push (GameMemoryAllocator alloc);
    void Pop  (GameMemoryAllocator alloc);
    void Pop  ();
    void SetTop (GameMemoryAllocator alloc);
    GameMemoryAllocator Top () const;

#ifndef RAD_REALEASE
    void Dump ();
#endif

private:
    GameMemoryAllocator* m_Stack;
    int m_CurPos;
    int m_Size;

    enum { STACKSIZE = 48 };

    // Disable
    HeapStack (const HeapStack&);
};


class HeapActivityTracker : public IRadMemoryActivityCallback
{
public:
    HeapActivityTracker ();

    void EnableHeapAllocs (GameMemoryAllocator alloc, bool enable) { m_AllocsEnabled[alloc] = enable; }
    void EnableHeapFrees (GameMemoryAllocator alloc, bool enable) { m_FreesEnabled[alloc] = enable; }
    bool AllocsAllowed (GameMemoryAllocator alloc) const { return m_AllocsEnabled[alloc]; }
    bool FreesAllowed (GameMemoryAllocator alloc) const { return m_FreesEnabled[alloc]; }

    void MemoryAllocated (radMemoryAllocator allocator, void* address, unsigned int size);
    void MemoryFreed (radMemoryAllocator allocator, void* address);

private:
    bool m_AllocsEnabled[NUM_GAME_MEMORY_ALLOCATORS];
    bool m_FreesEnabled[NUM_GAME_MEMORY_ALLOCATORS];

};

extern HeapActivityTracker g_HeapActivityTracker;


class HeapManager : public IRadMemorySetAllocatorCallback
{
public:
    friend class HeapActivityTracker;

    static HeapManager* GetInstance ();
    static bool IsCreated();
    static void DestroyInstance();

    void PrepareHeapsStartup ();
    void PrepareHeapsFeCleanup ();
    void PrepareHeapsFeSetup   ();
    void PrepareHeapsInGame ();
    void PrepareHeapsSuperSprint ();

    int GetLoadedUsageFE ();
    int GetLoadedUsageInGame ();
    int GetLoadedUsageSuperSprint ();
    int GetLoadedUsage( GameMemoryAllocator allocator );

    void PushHeap (GameMemoryAllocator alloc);
    void PopHeap  (GameMemoryAllocator alloc);
    GameMemoryAllocator GetCurrentHeap () const;

#ifdef MEMORYTRACKER_ENABLED
    void PushGroup( const char* name );
    void PopGroup ( const char* name );
    const char* GetCurrentGroupID () const;

    void PushFlag (const char* name);
    void PopFlag (const char* name);
#endif

    // From IRadMemorySetAllocatorCallback
    //
    radMemoryAllocator GetCurrentAllocator ();
    radMemoryAllocator SetCurrentAllocator ( radMemoryAllocator allocator );

    void DumpHeapStats ( bool text = false );
    void DumpArtStats ();

#ifndef RAD_RELEASE
    static bool s_bSpecialRoute;
#endif

    void ResetArtStats();

private:
    HeapManager (GameMemoryAllocator defaultAllocator);
    HeapManager (const HeapManager&);
    virtual ~HeapManager ();

    void DestroyHeap (IRadMemoryHeap*& heap, bool justTest = false );
    GameMemoryAllocator GetHeapID (const IRadMemoryHeap* heap);
    static float GetFudgeFactor ();
    int GetHeapUsage ( IRadMemoryAllocator* allocator );
    int GetSoundMemoryHeapUsage ();

    static IRadThreadLocalStorage* s_Instance;    // We maintain a separate heap stack for each thread

#ifdef MEMORYTRACKER_ENABLED
public:  //HACK for the GC, sorry.
    enum { MAXSTRING = 128 };
private:
    typedef struct MemoryIDString { char id[MAXSTRING]; };
    typedef std::vector< MemoryIDString, s2alloc<MemoryIDString> > MemoryIDVector;
    typedef std::stack< MemoryIDString, MemoryIDVector > MemoryIDStack;

    MemoryIDStack m_GroupIDStack;
    MemoryIDStack m_FlagStack;
#endif

    HeapStack m_HeapStack;
    enum { MB = 1048576, KB = 1024 };

#ifndef FINAL
    int mLowestFPS, mHighestTris, mFPSLWTris, mHWTriFPS;
#endif

};

HeapManager* HeapMgr ();

#ifndef MEMORYTRACKER_ENABLED

    // Push and Pop Group Name
    #define MEMTRACK_PUSH_GROUP( string )
    #define MEMTRACK_POP_GROUP(  string )

    // Push and Pop Flag Name    
    #define MEMTRACK_PUSH_FLAG( string )
    #define MEMTRACK_POP_FLAG(  string )

#else

    // Push and Pop Group Name
    #define MEMTRACK_PUSH_GROUP( string ) HeapMgr()->PushGroup( string )
    #define MEMTRACK_POP_GROUP(  string ) HeapMgr()->PopGroup(  string )

    // Push and Pop Flag Name    
    #define MEMTRACK_PUSH_FLAG( string )  HeapMgr()->PushFlag( string )
    #define MEMTRACK_POP_FLAG(  string )  HeapMgr()->PopFlag(  string )

#endif // !MEMORYTRACKER_ENABLED


#endif //SRRMEMORY_H
