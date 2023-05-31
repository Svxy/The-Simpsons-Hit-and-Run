//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps2main.cpp
//
// Description: This file contains the main enrty point to the game.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <string.h>
// Foundation Tech
#include <raddebug.hpp>
#include <radobject.hpp>
#include <radthread.hpp>
#include <radtextdisplay.hpp>

#include <pddi/ps2/ps2context.hpp>

//========================================
// Project Includes
//========================================
#include <main/game.h>
#include <main/ps2platform.h>
#include <main/singletons.h>
#include <main/commandlineoptions.h>
#include <memory/memoryutilities.h>
#include <memory/srrmemory.h>

#ifdef RAD_MW 
    #include <mwutils.h>
#endif

//========================================
// Forward Declarations
//========================================
static void ProcessCommandLineArguments( int argc, char* argv[] );

static void ProcessCommandLineArgumentsFromFile( );



void OutputHandler (const char * pString )
{
    static int ypos = 0;
    static IRadTextDisplay* textDisplay = 0;


    if (!textDisplay || (ypos > 20))
    {
        if (textDisplay)
        {
            textDisplay->Release ();
        }
        ::radTextDisplayGet( &textDisplay, GMA_TEMP );
        textDisplay->SetBackgroundColor( 0 );
        textDisplay->SetTextColor( 0xffffffff );
        textDisplay->Clear();
        ypos = 0;
    }

    if ( textDisplay )
    {
        textDisplay->TextOutAt( pString, 0, ypos );
        textDisplay->SwapBuffers();
        ypos++;
    }

    radThreadSleep (200);
}


//=============================================================================
// Function:    main
//=============================================================================
//
// Description: Main entry point.
// 
// Parameters:  PS2EE argc, number of command line argurments
//                    argv, array of pointes to these tokens.
//
// Returns:     0 success, non zero error.
//
//=============================================================================
int main( int argc, char* argv[] )
{
    //::rDebugSetOutputHandler (OutputHandler);

    // Get rid of junk in the frame buffer, 
    // before pddi gets initialised
    sceGsSyncV(0);
    *GS_PMODE = SCE_GS_SET_PMODE(0,0,0,0,0,0,0);
    sceGsResetPath();
    ps2_clearvram();

#ifdef RAD_MW
    mwInit();
#endif

    /*
    int bytes = static_cast<int>(27.0f * 1024.0 * 1024.0);
    char* p = (char*)malloc (bytes);
    memset (p, 0x00, bytes);
    free (p);
    */

    /*
#ifdef RAD_RELEASE
    if (CommandLineOptions::Get (CLO_CD_FILES_ONLY))
    {
        void* p = malloc (96 * 1024 * 1024);
    }
#endif
    */

    //
    // All settings get stored in GameDB.
    //
    CommandLineOptions::InitDefaults();
    ProcessCommandLineArguments( argc, argv );

    //
    // Have to get FTech setup first so that we can use all the memory services.
    //
    PS2Platform::InitializeFoundation();

    srand (Game::GetRandomSeed ());

#ifndef RAD_RELEASE
    tName::SetAllocator (GMA_DEBUG);

    // Now disable the default heap
    //
    //g_HeapActivityTracker.EnableHeapAllocs (GMA_DEFAULT, false);
    //g_HeapActivityTracker.EnableHeapFrees (GMA_DEFAULT, false);

    // Can never free from the persistent heap
    //
    //g_HeapActivityTracker.EnableHeapFrees (GMA_PERSISTENT, false);
#endif

    HeapMgr()->PushHeap (GMA_PERSISTENT);

	//Process any commandline options from the command.txt file
	ProcessCommandLineArgumentsFromFile();

    //
    // Instantiate all the singletons before doing anything else.
    //
    CreateSingletons();

    //
    // Construct the platform object.
    //
    PS2Platform* pPlatform = PS2Platform::CreateInstance();
    rAssert( pPlatform != NULL );

    //
    // Create the game object.
    //
    Game* pGame = Game::CreateInstance( pPlatform );
    rAssert( pGame != NULL );
    

	


    //
    // Initialize the game.
    //
    pGame->Initialize();

    HeapMgr()->PopHeap (GMA_PERSISTENT);

    //
    // Run it!  Control will not return from here until the game is stopped.
    //
    pGame->Run();
    
    //
    // Terminate the game (this frees all resources allocated by the game).
    //
    pGame->Terminate();
    
    //
    // Destroy the game object.
    //
    Game::DestroyInstance();
    
    //
    // Destroy the game and platform (do it in this order in case the game's 
    // destructor references the platform.
    //
    PS2Platform::DestroyInstance();
    
    //
    // Show some debug output
    //
#ifdef RAD_DEBUG
    radObject::DumpObjects();
#endif;

    //
    // Dump all the singletons.
    //
    DestroySingletons();
    
    //
    // Pass any error codes back to the operating system.
    //
    return( 0 );
}


//=============================================================================
// Function:    ProcessCommandLineArguments
//=============================================================================
//
// Description: Pick out the command line options and stuff them in
//              the game database.
// 
// Parameters:  PS2EE argc, number of command line argurments
//                    argv, array of pointes to these tokens.
//
// Returns:     None.
//
//=============================================================================
void ProcessCommandLineArguments( int argc, char* argv[] )
{
//#ifndef FINAL

    rDebugPrintf( "*************************************************************\n" );
    rDebugPrintf( "Command Line Args:\n" );

    //
    // Pick out all the command line options and store them in GameDB.
    // Also dump them to the output for handy dandy viewing.
    //
    int i;
    for( i = 0; i < argc; ++i )
    {
        char* argument = argv[i];
        
        rDebugPrintf( "arg%d: %s\n", i, argument );
        
        CommandLineOptions::HandleOption( argument );

    }
    if( !CommandLineOptions::Get( CLO_ART_STATS ) )
    {
        //CommandLineOptions::HandleOption( "noheaps" );
    }


    rDebugPrintf( "*************************************************************\n" );

//#endif // FINAL
}



void ProcessCommandLineArgumentsFromFile()
{
#ifndef FINAL

      //Chuck: looking for additional command line args being passed in from a file
     //its for QA testing etc.

    IRadFile* pfile =NULL;
    
    ::radFileOpenSync(&pfile,"command.txt");

    if (pfile != NULL)
    {
        char commandlinestring [256];
        commandlinestring[ 0 ] = '\0';

        pfile->ReadSync(commandlinestring,255);

        //QA created the command line file and wants to pass additional arguements
            
        char* argument = strtok(commandlinestring," ");
        while (argument != NULL)
        {
            CommandLineOptions::HandleOption(argument);
            argument=strtok(NULL," ");
        }
		pfile->Release();
    }
#endif //FINAL
} //end of Function