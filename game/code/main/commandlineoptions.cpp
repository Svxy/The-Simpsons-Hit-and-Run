//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        commandlineoptions.cpp
//
// Description: CommandLineOptions class implementation.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
// Standard C Library
#include <string.h>
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>

//#ifdef RAD_GAMECUBE
//#define strupr(x) (x)
//#pragma message("strupr not defined on the game cube")
//#endif


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Bit flag for options.
//
#ifdef NEALL_DEMOTEST
simpsonsUInt64 CommandLineOptions::sOptions = ( 1 << CLO_DEMO_TEST | 1 << CLO_NO_TRAFFIC );
#else
simpsonsUInt64 CommandLineOptions::sOptions = 0;
#endif

short CommandLineOptions::s_defaultLevel = 0;
short CommandLineOptions::s_defaultMission = 0;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// CommandLineOptions::InitDefaults
//==============================================================================
//
// Description: Initialize default command-line options.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void CommandLineOptions::InitDefaults()
{
    simpsonsUInt64 orValue = 1;

#ifdef RAD_RELEASE
    // enable CLO_CD_FILES_ONLY in release builds for all platforms
    //
    orValue = 1;
    orValue <<= CLO_CD_FILES_ONLY;
    sOptions |= orValue;
#endif

#if defined( RAD_GC ) || defined( RAD_PS2 )
    // enable CLO_NO_HEAPS for GC and PS2 only
    //
    orValue = 1;
    orValue <<= CLO_NO_HEAPS;
    sOptions |= orValue;
#endif
}

//==============================================================================
// CommandLineOptions::HandleOption
//==============================================================================
//
// Description: Interpret the command line string token.
//
// Parameters:  option - command line string token.
//
// Return:      None.
//
//==============================================================================
void CommandLineOptions::HandleOption( const char* const optionIn )
{
    simpsonsUInt64 andValue;
    simpsonsUInt64 orValue = 1;
    bool optionFound = true;

    rReleaseAssertMsg( NUM_CLO < (sizeof( sOptions ) * 8),
                       "*** WARNING: Too many commandline options!" );

    char option[ 256 ];
    strcpy( option, optionIn );
    rReleasePrintf( "Commandline Option: %s\n", option );

    if( strcmp( strupr(option), "NOMUSIC" ) == 0 )
    {
        orValue <<= CLO_NO_MUSIC;
    }
    else if( strcmp( strupr(option), "NOEFFECTS" ) == 0 )
    {
        orValue <<= CLO_NO_EFFECTS;
    }
    else if( strcmp( strupr(option), "NODIALOG" ) == 0 )
    {
        orValue <<= CLO_NO_DIALOG;
    }
    else if( strcmp( strupr(option), "MUTE" ) == 0 )
    {
        orValue <<= CLO_MUTE;
    }
    else if( strcmp( strupr(option), "SKIPMOVIE" ) == 0 )
    {
        orValue <<= CLO_SKIP_MOVIE;
    }
    else if( strcmp( strupr(option), "MEMMONITOR" ) == 0 )
    {
        orValue <<= CLO_MEMORY_MONITOR;
    }
    else if( strcmp( strupr(option), "HEAPSTATS" ) == 0 )
    {
        orValue <<= CLO_HEAP_STATS;
    }
    else if( strcmp( strupr(option), "CDFILES" ) == 0 )
    {
        orValue <<= CLO_CD_FILES_ONLY;
    }
    else if( strcmp( strupr(option), "HOSTFILES" ) == 0 )
    {
        //
        // Careful to avoid the PS2-hated 64-bit constant
        //
        andValue = 1;
        andValue <<= CLO_CD_FILES_ONLY;
        andValue = ~andValue;

        sOptions = sOptions & andValue;

        //
        // Skip the or operation below
        //
        optionFound = false;
    }
    else if( strcmp( strupr(option), "FIREWIRE" ) == 0 )
    {
        orValue <<= CLO_FIREWIRE;
    }
    else if( strcmp( strupr(option), "SNPROFILER" ) == 0 )
    {
        orValue <<= CLO_SN_PROFILER;
    }
    else if( strcmp( strupr(option), "ARTSTATS" ) == 0 )
    {
        orValue <<= CLO_ART_STATS;
    }
    else if( strcmp( strupr(option), "PROPSTATS" ) == 0 )
    {
        orValue <<= CLO_PROP_STATS;
    }
    else if( strcmp( strupr(option), "FEUNJOINED" ) == 0 )
    {
        orValue <<= CLO_FE_UNJOINED;
    }
    else if( strcmp( strupr(option), "SPEEDOMETER" ) == 0 )
    {
        orValue <<= CLO_SHOW_SPEED;
    }
    else if( strcmp( strupr(option), "NOHUD" ) == 0 )
    {
        orValue <<= CLO_NO_HUD;
    }
    else if( strcmp( strupr(option), "DEBUGBV" ) == 0 )
    {
        orValue <<= CLO_DEBUGBV;
    }
    else if( strcmp( strupr(option), "NOTRAFFIC" ) == 0 )
    {
        orValue <<= CLO_NO_TRAFFIC;
    }
    else if( strcmp( strupr(option), "SKIPSUNDAY" ) == 0 )
    {
        orValue <<= CLO_SKIP_SUNDAY;
    }
    else if( strcmp( strupr(option), "SKIPFE" ) == 0 )
    {
        orValue <<= CLO_SKIP_FE;
    }
    else if( strcmp( strupr(option), "FEGAGS" ) == 0 )
    {
        orValue <<= CLO_FE_GAGS_TEST;
    }
    else if( strcmp( strupr(option), "FPS" ) == 0 )
    {
        orValue <<= CLO_FPS;
    }
    else if( strcmp( strupr(option), "DESIGNER" ) == 0 )
    {
        orValue <<= CLO_DESIGNER;
    }
    else if( strcmp( strupr(option), "DETECTLEAKS" ) == 0 )
    {
        orValue <<= CLO_DETECT_LEAKS;
    }
    else if( strcmp( strupr(option), "NOHEAPS" ) == 0 )
    {
        orValue <<= CLO_NO_HEAPS;
    }
    else if( strcmp( strupr(option), "PRINTMEMORY" ) == 0 )
    {
        orValue <<= CLO_PRINT_MEMORY;
    }
    else if( strcmp( strupr(option), "DEMOTEST" ) == 0 )
    {
        orValue <<= CLO_DEMO_TEST;
    }
    else if( strcmp( strupr(option), "NOSPLASH" ) == 0 )
    {
        orValue <<= CLO_NO_SPLASH;
    }
    else if( strcmp( strupr(option), "LANGUAGE" ) == 0 )
    {
        orValue <<= CLO_LANG_PROMPT;
    }
    else if( strcmp( strupr(option), "SKIPMEMCHECK" ) == 0 )
    {
        orValue <<= CLO_SKIP_MEMCHECK;
    }
    else if( strcmp( strupr(option), "NOHAPTIC" ) == 0 )
    {
        orValue <<= CLO_NO_HAPTIC;
    }
    else if( strcmp( strupr(option), "RANDOMBUTTONS" ) == 0 )
    {
        orValue <<= CLO_RANDOM_BUTTONS;
    }
    else if( strcmp( strupr(option), "SEQUENTIALDEMO" ) == 0 )
    {
        orValue <<= CLO_SEQUENTIAL_DEMO;
    }
    else if ( strcmp( strupr(option), "PCTEST" ) == 0 )
    {
        orValue <<= CLO_PARKED_CAR_TEST;
    }
    else if ( strcmp( strupr(option), "NOAVRIL" ) == 0 )
    {
        orValue <<= CLO_NO_AVRIL;
    }
    else if ( strcmp( strupr(option), "SHORTDEMO" ) == 0 )
    {
        orValue <<= CLO_SHORT_DEMO;
    }
    else if( strcmp( strupr( option), "PRINTLOADTIME" ) == 0 )
    {
        orValue <<= CLO_PRINT_LOAD_TIME;
    }
    else if( strcmp( strupr( option), "PRINTFRAMERATE" ) == 0 )
    {
        orValue <<= CLO_PRINT_FRAMERATE;
    }
    else if ( strcmp( strupr( option ), "SHOWDYNALOAD" ) == 0 )
    {
        orValue <<= CLO_SHOW_DYNA_ZONES;
    }
    else if ( strcmp( strupr( option ), "NOPEDS" ) == 0 )
    {
        orValue <<= CLO_NO_PEDS;
    }
    else if ( strcmp( strupr( option ), "MANUALRESETDAMAGE" ) == 0 )
    {
        orValue <<= CLO_MANUAL_RESET_DAMAGE;
    }
    else if ( strcmp( strupr( option ), "WINDOW" ) == 0 )
    {
        orValue <<= CLO_WINDOW_MODE;
    }
    else if ( strcmp( strupr( option ), "NOTUTORIAL" ) == 0 )
    {
        orValue <<= CLO_NO_TUTORIAL;
    }
    else if ( strcmp( strupr( option ), "COINS" ) == 0 )
    {
        orValue <<= CLO_COINS;
    }
    else if ( strcmp( strupr( option ), "PROGSCAN" ) == 0 )
    {
        orValue <<= CLO_PROGRESSIVE_SCAN;
    }
    else if ( strcmp( strupr( option ), "LARGEHEAPS" ) == 0 )
    {
        orValue <<= CLO_LARGEHEAPS;
    }
    else if ( strcmp( strupr( option ), "MEMCARDCHEAT" ) == 0 )
    {
        orValue <<= CLO_MEMCARD_CHEAT;
    }
    else if ( strcmp( strupr( option ), "TOOL" ) == 0 )
    {
        orValue <<= CLO_PS2_TOOL;
    }
    else if ( strcmp( strupr( option ), "FILENOTFOUND" ) == 0 )
    {
        orValue <<= CLO_FILE_NOT_FOUND;
    }
    else if ( strcmp( strupr( option ), "NOLOADINGSPEW" ) == 0 )
    {
        orValue <<= CLO_NO_LOADING_SPEW;
    }    
    else if ( strcmp( strupr( option ), "AUDIOSPEW" ) == 0 )
    {
        orValue <<= CLO_AUDIO_LOADING_SPEW;
    }    
    else if ( strcmp( strupr( option ), "RELEASEPRINT" ) == 0 )
    {
        extern bool g_AllowDebugOutput;
        g_AllowDebugOutput = true;
        optionFound = false;     
    }
    else if ( strcmp( strupr( option ), "NOFRUITLESS" ) == 0 )
    {
        extern bool gFruitless;
        gFruitless = false;
        optionFound = false;        
    }
    else if ( strcmp( strupr( option ), "RADTUNER" ) == 0 )
    {
        extern bool gTuneSound;
        gTuneSound = true;
        optionFound = false;
    }

    else
    {
        // special case for "l<N>" and "m<N>" commandline options
        //
        optionFound = false;
        int stringLength = strlen( option );
        char lastChar = option[ stringLength - 1 ];
        option[ stringLength - 1 ] = '\0';

        if( strcmp( strupr(option), "L" ) == 0 )
        {
            s_defaultLevel = lastChar - '1';
        }
        else if( strcmp( strupr(option), "M" ) == 0 )
        {
            s_defaultMission = lastChar - '1';
        }
    }

    if( optionFound )
    {
        sOptions = sOptions | orValue;
    }
    else
    {
        rDebugPrintf( "Unhandled command line option: %s\n", option );
    }
}

  
//==============================================================================
// CommandLineOptions::Get
//==============================================================================
//
// Description: Retrieve the specified option.
//
// Parameters:  eOption - the enumerated command line option.
//
// Return:      bool - true if the option is enabled, false otherwise.
//
//==============================================================================
bool CommandLineOptions::Get( CmdLineOptionEnum eOption )
{
    simpsonsUInt64 andValue = 1;

    rAssert( NUM_CLO <= 64 );

    andValue <<= eOption;
    return( ( sOptions & andValue ) != 0 );
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
