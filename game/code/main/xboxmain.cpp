//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        xboxmain.cpp
//
// Description: This file contains the main enrty point to the game.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
// Standard Library
#include <string.h>
// Foundation Tech
#include <raddebug.hpp>
#include <radobject.hpp>

//========================================
// Project Includes
//========================================
#include <main/game.h>
#include <main/xboxplatform.h>
#include <main/singletons.h>
#include <main/commandlineoptions.h>
#include <memory/memoryutilities.h>
#include <memory/srrmemory.h>

//========================================
// Forward Declarations
//========================================
static void ProcessCommandLineArguments();

static void ProcessCommandLineArgumentsFromFile();


//=============================================================================
// Function:    main
//=============================================================================
//
// Description: Main entry point.
// 
// Parameters:  None.
//
// Returns:     None.
//
//=============================================================================
void main()
{
    //
    // Pick out and store command line settings.
    //
    CommandLineOptions::InitDefaults();
    ProcessCommandLineArguments();

    //
    // Have to get FTech setup first so that we can use all the memory services.
    //
    XboxPlatform::InitializeFoundation();
    
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
    XboxPlatform* pPlatform = XboxPlatform::CreateInstance();
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
    XboxPlatform::DestroyInstance();
    
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
}


//=============================================================================
// Function:    ProcessCommandLineArguments
//=============================================================================
//
// Description: Pick out the command line options and store them.
// 
// Parameters:  None.
//
// Returns:     None.
//
//=============================================================================
void ProcessCommandLineArguments()
{
    //
    // None of this gets into the shipping code.
    //
#ifndef FINAL

    DWORD dwLaunchMethod;
    LAUNCH_DATA launchData;

    XGetLaunchInfo( &dwLaunchMethod, &launchData );

    if( LDT_FROM_DEBUGGER_CMDLINE == dwLaunchMethod )
    {
        LD_FROM_DEBUGGER_CMDLINE* pCmdLine = (LD_FROM_DEBUGGER_CMDLINE*)&launchData;

        char* argument;
        argument = strtok( pCmdLine->szCmdLine, " " );    

        rDebugPrintf( "*************************************************************************\n" );
        rDebugPrintf( "Command Line Args:\n" );

        //
        // Pick out all the command line options and store them in GameDB.
        // Also dump them to the output for handy dandy viewing.
        //
        int i = 0;
        while( NULL != argument )
        {
            rDebugPrintf( "arg%d: %s\n", i++, argument );

            CommandLineOptions::HandleOption( argument );
        
            argument = strtok( NULL, " " );
        }

        if( !CommandLineOptions::Get( CLO_ART_STATS ) )
        {
            //CommandLineOptions::HandleOption( "noheaps" );
        }

       

        
    
        rDebugPrintf( "*************************************************************************\n" );
    }

#endif // FINAL
}



void ProcessCommandLineArgumentsFromFile()
{
#ifndef FINAL

         //Chuck: looking for additional command line args being passed in from a file
     //its for QA testing etc.

    IRadFile* pfile =NULL;
    
	::radFileOpenSync(&pfile,"command.txt", false, OpenExisting );

    if (pfile != NULL)
    {
        char commandlinestring [256];
        memset( commandlinestring, 0, sizeof( commandlinestring ) );

		unsigned int fileSize = 0;
		pfile->GetSizeSync( &fileSize );
		pfile->ReadSync(commandlinestring, fileSize);

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
