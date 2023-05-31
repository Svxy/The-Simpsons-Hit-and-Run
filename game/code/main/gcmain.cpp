//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        GCmain.cpp
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

//========================================
// Project Includes
//========================================
#include <main/game.h>
#include <main/gcplatform.h>
#include <main/singletons.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>

//========================================
// Forward Declarations
//========================================
static void ProcessCommandLineArguments( int argc, char* argv[] );

static void ProcessCommandLineArgumentsFromFile( );

//=============================================================================
// Function:    main
//=============================================================================
//
// Description: Main entry point.
// 
// Parameters:  GC    argc, number of command line argurments
//                    argv, array of pointes to these tokens.
//
// Returns:     0 success, non zero error.
//
//=============================================================================
int main( int argc, char* argv[] )
{
    //
    // Pick out and store command line settings.
    //
    CommandLineOptions::InitDefaults();
    ProcessCommandLineArguments( argc, argv );

    //
    // Have to get FTech setup first so that we can use all the memory services.
    //
    GCPlatform::InitializeFoundation();


    srand (Game::GetRandomSeed ());


    // Now disable the default heap
    //
#ifndef RAD_RELEASE
    tName::SetAllocator (GMA_DEBUG);

    //g_HeapActivityTracker.EnableHeapAllocs (GMA_DEFAULT, false);
    //g_HeapActivityTracker.EnableHeapFrees (GMA_DEFAULT, false);
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
    GCPlatform* pPlatform = GCPlatform::CreateInstance();
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
    GCPlatform::DestroyInstance();
    
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
// Parameters:  GC    argc, number of command line argurments
//                    argv, array of pointes to these tokens.
//
// Returns:     None.
//
//=============================================================================
void ProcessCommandLineArguments( int argc, char* argv[] )
{
#ifndef FINAL

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
        CommandLineOptions::HandleOption( "noheaps" );
    }


    rDebugPrintf( "*************************************************************\n" );

#endif // FINAL
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
