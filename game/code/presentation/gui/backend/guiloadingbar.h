//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/22      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUILOADINGBAR_H
#define GUILOADINGBAR_H

//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// External Constants
//===========================================================================

const int MB = 1024 * 1024; // bytes

#ifdef RAD_GAMECUBE
    const float TOTAL_INGAME_MEMORY_USAGE = 9.6f * MB;
    const float TOTAL_FE_MEMORY_USAGE = 8.8f * MB;
    const float TOTAL_SUPERSPRINT_MEMORY_USAGE = 3.0f * MB;
    const float TOTAL_DEMO_MEMORY_USAGE = TOTAL_INGAME_MEMORY_USAGE;
#endif

#ifdef RAD_PS2
    const float TOTAL_INGAME_MEMORY_USAGE = 15.2f * MB;
    const float TOTAL_FE_MEMORY_USAGE = 13.0f * MB;
    const float TOTAL_SUPERSPRINT_MEMORY_USAGE = 5.6f * MB;
    const float TOTAL_DEMO_MEMORY_USAGE = TOTAL_INGAME_MEMORY_USAGE;
#endif

#ifdef RAD_XBOX
    const float TOTAL_INGAME_MEMORY_USAGE = 16.7f * MB;
    const float TOTAL_FE_MEMORY_USAGE = 13.1f * MB;
    const float TOTAL_SUPERSPRINT_MEMORY_USAGE = 5.5f * MB;
    const float TOTAL_DEMO_MEMORY_USAGE = TOTAL_INGAME_MEMORY_USAGE;
#endif

#ifdef RAD_WIN32  
    // These settings are for release mode, which has difft memory behaviour than Tune,
    // only because Tune has debug information.
    const float TOTAL_INGAME_MEMORY_USAGE = 1.3f * MB;
    const float TOTAL_FE_MEMORY_USAGE = 1.9f * MB;
    const float TOTAL_SUPERSPRINT_MEMORY_USAGE = 1.3f * MB;
    const float TOTAL_DEMO_MEMORY_USAGE = TOTAL_INGAME_MEMORY_USAGE;

    // Our system for windows because the memory tracking doesn't work.
    const float TOTAL_INGAME_FILES = 56;
    // no fe.. it doesn't work with this system.
    const float TOTAL_SUPERSPRINT_FILES = 34;
    const float LOAD_BLEND_FACTOR = 20;
    const float LOAD_MIN_SPEED = 0.003f;
#endif

#endif // GUILOADINGBAR_H
