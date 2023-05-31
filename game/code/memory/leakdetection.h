#ifndef LEAK_DETECTION_HPP
#define LEAK_DETECTION_HPP

#include "main/commandlineoptions.h"
#include <radmemorymonitor.hpp>
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

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

//
// Use these macros for accessing the leak detection
//
#ifdef RAD_RELEASE
    #define LEAK_DETECTION_DISABLE
#endif

#ifndef LEAK_DETECTION_DISABLE
    // Begin tracking allocations.  If any allocations are outstanding from the last
    // time this macro is called a warning or assert will happen
    #define LEAK_DETECTION_CHECKPOINT( ) { LeakDetectionStop( ); LeakDetectionStart( ); }

    // Declare allocation
    #define LEAK_DETECTION_ADD_ALLOCATION( pMemory, size, heap ) { LeakDetectionAddRecord( pMemory, size, heap ); }

    // Rescind allocation
    #define LEAK_DETECTION_REMOVE_ALLOCATION( pMemory ) { LeakDetectionRemoveRecord( pMemory ); }
#else
    // Compile these things out in final 
    #define LEAK_DETECTION_CHECKPOINT( )                          ( ( void ) 0 )
    #define LEAK_DETECTION_ADD_ALLOCATION( pMemory , size, heap ) ( ( void ) 0 )
    #define LEAK_DETECTION_REMOVE_ALLOCATION( pMemory )           ( ( void ) 0 )
#endif // FINAL

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

#ifndef LEAK_DETECTION_DISABLE
    // Start / stop leak tracking
    void LeakDetectionStart( void );
    void LeakDetectionStop( void );

    // Declare / rescind allocation
    void LeakDetectionAddRecord( const void* pMemory, const unsigned int size, const radMemoryAllocator heap );
    void LeakDetectionRemoveRecord( void* pMemory );
#endif // FINAL

#endif // LEAK_DETECTION_HPP

