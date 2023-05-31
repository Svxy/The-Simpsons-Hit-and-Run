//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        memoryutilities.h
//
// Description: functions for telling you things about memory
//
// History:     2002/12/03 + Created -- Ian Gipson
//
//=============================================================================

#ifndef MEMORYUTILITIES_H
#define MEMORYUTILITIES_H

//========================================
// Nested Includes
//========================================
struct IRadMemoryAllocator;

//==============================================================================
//
// Synopsis: This class allows you to tag allocations for reporting in the 
//           FTech Memory Monitor.
//
//==============================================================================
namespace Memory
{
    void   InitializeMemoryUtilities();
    float  GetFreeMemoryEntropy( IRadMemoryAllocator* allocator );
    size_t GetFreeMemoryProfile();
    size_t GetLargestFreeBlock();
    size_t GetLargestFreeBlock( IRadMemoryAllocator* allocator );
    void   GetLargestNFreeBlocks( IRadMemoryAllocator* allocator, const int n, size_t blocks[] );
    size_t GetMaxFreeMemory();
    size_t GetTotalMemoryFree();
    size_t GetTotalMemoryFreeLowWaterMark();
    size_t GetTotalMemoryUnavailable();
    size_t GetTotalMemoryUsed();
    void   PrintMemoryStatsToTty();
}

#endif //MEMORYTAGGER_H