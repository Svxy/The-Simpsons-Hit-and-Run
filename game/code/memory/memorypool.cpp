//==============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        memorypool.cpp
//
// Description: Implementation of the memory pool based on the one described
//              in "Effective C++".
//
// History:     + 2001/12/04 Created -- Darwin Chau
//              + 2002/06/19 Adapted form Simpsons2 -- Darwin Chau
//
//==============================================================================

//========================================
// System Includes
//========================================
// Foundation
#include <raddebug.hpp> 

//========================================
// Project Includes
//========================================
#include <memory/memorypool.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// FBMemoryPool::FBMemoryPool
//==============================================================================
//
// Description: Constructor
//
// Parameters:  size
//                - size (in bytes) of the object being memory pooled
//
//              blockCapacity 
//                - controls the size of each memory block in the the pool by
//                  specifying the number of objects that will fit in each block
//                  (block size = size * blockCapacity)
//
//              allocator
//                - specifiy which heap to allocate memory from
//               
// Return:      N/A.      
//
//==============================================================================
FBMemoryPool::FBMemoryPool
(
    size_t size,
    int blockCapacity,
    GameMemoryAllocator allocator
)
:
    mSize( size ),  
    mBlockCapacity( blockCapacity ),
    mGMAllocator( allocator ),
    mpHeadOfFreeList( NULL ),
    mCurrentAllocs( 0 ),
    mCurrentBlock( 0 )
{
    int i;
    for( i = 0; i < MAX_BLOCKS; ++i )
    {
        mpBlockArray[mCurrentBlock] = NULL;
    }

#ifndef RAD_RELEASE
    mTotalAllocs = 0;
    mTotalFrees = 0;
    mPeakAllocs = 0;
#endif // !RAD_RELEASE
}


//==============================================================================
// FBMemoryPool::~FBMemoryPool
//==============================================================================
//
// Description: Destructor
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
FBMemoryPool::~FBMemoryPool()
{
    rAssertMsg( mCurrentAllocs == 0, "*** MEMORY LEAK ! ***\n" );
}


//==============================================================================
// FBMemoryPool::Allocate
//==============================================================================
//
// Description: Allocate memory from the pool.  The pool allocates memory in
//              large blocks which it then parcels out for subsequent allocation
//              requests.  All allocations from the pool must be the same size.
//
// Constraints: The size of the memory pool is aritificially capped by the
//              max number of blocks that can be allocated.  This was done
//              to avoid managing the memory required by an STL container class.
//
// Parameters:  size - amount of memory requested; this is only used to verify
//                     the size is the same as what the pool is configured for
//
// Return:      pointer to allocated memory on success
//              NULL on failure
//
//==============================================================================
void* FBMemoryPool::Allocate( size_t size )
{
    rAssert( size <= mSize );

    void* pMemory = NULL;
    
    //
    // Allocate memory from our pool.
    //
    if( mpHeadOfFreeList != NULL )
    {
        //
        // Allocate from the free list.
        //
        pMemory = static_cast<void*>( mpHeadOfFreeList );
        mpHeadOfFreeList = mpHeadOfFreeList->mpNext;
    }
    else
    {
        //
        // Need to allocate a new block of memory.
        //
        if( mCurrentBlock == MAX_BLOCKS )
        {
            //
            // Array to store blocks is full. 
            //
            rTuneString( "FBMemoryPool - Max blocks exceeded, tune pool config.\n");
            return( NULL );
        }

        void* pNewBlock = ::operator new( (mBlockCapacity * mSize), mGMAllocator );
        rAssert( pNewBlock != 0 );

        //
        // Store the pointer so we can delete the block later.
        //
        mpBlockArray[mCurrentBlock] = pNewBlock;
        ++mCurrentBlock;


        //
        // Make a linked list out of the block.
        //
        for( int i = 0; i < mBlockCapacity - 1; ++i )
        {
            char* pCurrent = static_cast<char*>(pNewBlock) + (i * mSize);
            MemoryPoolList* pList = reinterpret_cast<MemoryPoolList*>( pCurrent );
            char* pNext = static_cast<char*>(pNewBlock) + ( (i + 1) * mSize);
            pList->mpNext = reinterpret_cast<MemoryPoolList*>( pNext );
        }
        
        //
        // Mark the end of the list with a NULL.
        //
        char* pLast = static_cast<char*>(pNewBlock) + ( (mBlockCapacity - 1) * mSize);
        MemoryPoolList* pListEnd = reinterpret_cast<MemoryPoolList*>( pLast );
        pListEnd->mpNext = NULL;

        //
        // Return this piece of memory.
        //
        pMemory = pNewBlock;
        
        //
        // This is the new head of the free list.
        //
        mpHeadOfFreeList = reinterpret_cast<MemoryPoolList*>( pNewBlock )->mpNext;
    }

    ++mCurrentAllocs;


#ifndef RAD_RELEASE

    ++mTotalAllocs;
    
    if( mTotalAllocs > mPeakAllocs )
    {
        mPeakAllocs = mTotalAllocs;
    }

#endif // !RAD_RELEASE
    
    return( pMemory );
}


//==============================================================================
// FBMemoryPool::Free
//==============================================================================
//
// Description: Return memory to the pool.  When all of the memory has been
//              returned to the pool, all of the memory blocks are released.
//
// Parameters:  mem - pointer to memory being returned to the pool
//              size - size of memory being returned.
//
// Return:      None.
//
//==============================================================================
void FBMemoryPool::Free( void* mem, size_t size )
{
    //
    // Calling delete on a NULL pointer is "legal" so better handle it.
    //
    if( mem == NULL )
    {
        return;
    }

    rAssert( size <= mSize );

    //
    // Return the memory to the free list.
    //
    MemoryPoolList* pCarcass = static_cast<MemoryPoolList*>( mem );

    pCarcass->mpNext = mpHeadOfFreeList;
    mpHeadOfFreeList = pCarcass;

    --mCurrentAllocs;

    //
    // If there are no outstanding alloctions, free all the memory blocks.
    //
    if( mCurrentAllocs == 0 )
    {
        int i;
        for( i = 0; i < MAX_BLOCKS; ++i )
        {
            if( mpBlockArray[i] != NULL )
            {
                ::operator delete( mpBlockArray[i], mGMAllocator );
                mpBlockArray[i] = NULL;
            }
        }

        mpHeadOfFreeList = NULL;
        mCurrentBlock = 0;
    }

#ifndef RAD_RELEASE

    ++mTotalFrees;
    
    if( mCurrentAllocs == 0 )
    {
        this->DumpStats();
    }

#endif // !RAD_RELEASE
}



//==============================================================================
// FBMemoryPool::ChangeAllocator
//==============================================================================
//
// Description: Can only switch heaps if there are no outstanding allocations.
//
// Parameters:  allocator - switch to allocating from this heap
//
// Return:      true if successful
//              false otherwise
//
//==============================================================================
bool FBMemoryPool::ChangeAllocator( GameMemoryAllocator allocator )
{
    if( mCurrentAllocs == 0 )
    {
        mGMAllocator = allocator;
        return( true );
    }
    else
    {
        rAssertMsg( false, "Failed to change allocators" );
        return( false );
    }
}

bool FBMemoryPool::OneOfOurs(void* mem)
{
    for( int i = 0; i < MAX_BLOCKS; ++i )
    {
        if( mpBlockArray[i] != NULL )
        {
            if((mem >= mpBlockArray[i]) && (mem < (((char*)mpBlockArray[i]) + (mBlockCapacity * mSize))))
            {
                return true;
            }
        }
    }
    return false;
}


//==============================================================================
// FBMemoryPool::DumpStats
//==============================================================================
//
// Description: Debug spew.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void FBMemoryPool::DumpStats()
{
    rTunePrintf( "FBMemoryPool Stats: (Element Size: %d ; Block Capacity: %d)\n",
                 mSize, mBlockCapacity );
    rTunePrintf( "\tCurrent Allocs: %d\n", mCurrentAllocs );
#ifndef RAD_RELEASE
    rTunePrintf( "\tTotal Allocs: %d\n", mTotalAllocs );
    rTunePrintf( "\tTotal Frees: %d\n", mTotalFrees );
    rTunePrintf( "\tPeak Allocs: %d\n", mPeakAllocs );
#endif // !RAD_RELEASE
}

