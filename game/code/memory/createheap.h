//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        createheap.h
//
// Description: this set of functions creates heaps
//
// History:     2003/04/13 + Created -- Ian Gipson
//
//=============================================================================

#ifndef CREATEHEAP_H
#define CREATEHEAP_H

#include <memory/srrmemory.h>

//=============================================================================
// Enumerated Types
//=============================================================================
enum HeapType
{
    HEAP_TYPE_STATIC,
    HEAP_TYPE_TRACKING,
    HEAP_TYPE_DOUG_LEA,
    HEAP_TYPE_NONE
};

//=============================================================================
// Functions
//=============================================================================
HeapType         AllocatorType( GameMemoryAllocator allocator );
void             CreateHeap ( GameMemoryAllocator allocator, const unsigned int size );
void             DestroyHeapA( GameMemoryAllocator allocator );
IRadMemoryAllocator*  GetAllocator( GameMemoryAllocator allocator );
IRadMemoryHeap** GetHeapReference( GameMemoryAllocator allocator );
size_t           GetTotalMemoryFreeInAllHeaps();
GameMemoryAllocator WhichAllocator( const IRadMemoryAllocator* heap );

#endif //CREATEHEAP_H
