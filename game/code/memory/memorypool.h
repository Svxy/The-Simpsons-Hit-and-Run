//==============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        memorypool.h
//
// Description: Implementation of the memory pool based on the one described
//              in "Effective C++".
//
// History:     + 2001/12/04 Created -- Darwin Chau
//              + 2002/06/19 Adapted form Simpsons2 -- Darwin Chau
//
//==============================================================================

#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

//========================================
// Nested Includes
//========================================
#include <memory/srrmemory.h>

//========================================
// Forward References
//========================================

//==============================================================================
//
// Synopsis: 
//
//==============================================================================
class FBMemoryPool
{
    public:

        FBMemoryPool( size_t size, 
                      int blockCapacity,
                      GameMemoryAllocator allocator );
        ~FBMemoryPool();

        void* Allocate( size_t size );
        void Free( void* mem, size_t size );

        bool ChangeAllocator( GameMemoryAllocator allocator );
        void DumpStats();

        bool OneOfOurs(void*);

    private:

        // Declared but not defined to prevent copying and assignment.
        FBMemoryPool( const FBMemoryPool& );
        FBMemoryPool& operator=( const FBMemoryPool& );

        struct MemoryPoolList
        {
            MemoryPoolList* mpNext;
        };

        size_t mSize;
        int mBlockCapacity;
        GameMemoryAllocator mGMAllocator;

        MemoryPoolList* mpHeadOfFreeList;
        int mCurrentAllocs;

        enum{ MAX_BLOCKS = 16 };
        void* mpBlockArray[MAX_BLOCKS];
        int mCurrentBlock;

#ifndef RAD_RELEASE
        int mTotalAllocs;
        int mTotalFrees;
        int mPeakAllocs;
#endif // !RAD_RELEASE
};

#endif // MEMORYPOOL_H


