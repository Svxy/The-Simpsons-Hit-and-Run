//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        loadingmanager.cpp
//
// Description: Implementation for the LoadingManager class.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
// Ftech
#include <raddebug.hpp>
#include <radtime.hpp>
#include <radfile.hpp>
#include <string.h>

//========================================
// Project Includes
//========================================
#include <loading/loadingmanager.h>
#include <loading/filehandlerfactory.h>
#include <loading/cementfilehandler.h>
#include <memory/srrmemory.h>
#include <debug/profiler.h>
#include <radtextdisplay.hpp>
#include <main/game.h>

#include <cheats/cheatinputsystem.h>

#include <mission/gameplaymanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
LoadingManager* LoadingManager::spInstance = NULL;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// LoadingManager::GetInstance
//==============================================================================
//
// Description: Access point for the LoadingManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the LoadingManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
LoadingManager* LoadingManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// LoadingManager::CreateInstance
//==============================================================================
//
// Description: Constructs the LoadingManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the LoadingManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
LoadingManager* LoadingManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "LoadingManager" );
    rAssert( spInstance == NULL );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif
    spInstance = new LoadingManager;
    rAssert( spInstance );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
MEMTRACK_POP_GROUP("LoadingManager");
    
    return spInstance;
}


//==============================================================================
// LoadingManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the LoadingManager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void LoadingManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}



void LoadingManager::AddCallback( LoadingManager::ProcessRequestsCallback* pCallback, void* pUserData)
{
    if ( mCancellingLoads )
    {
        return;
    }

    if(mRequestHead == mRequestTail && !mLoading )
    {
        pCallback->OnProcessRequestsComplete(pUserData);
        return;
    }

    int newTail = (mRequestTail + 1) % MAX_REQUESTS;

    rAssert( newTail != mRequestHead );
 
    //rAssert(!mRequests[lastAddedRequest].pCallback);
    mRequests[mRequestTail].pCallback = pCallback;
    mRequests[mRequestTail].pUserData = pUserData;
    mRequests[mRequestTail].filename[0] = '\0';

    mRequestTail = newTail;
}

//==============================================================================
// LoadingManager::AddRequest
//==============================================================================
//
// Description: Clients use this method to submit a loading request.  
//              This request is not serviced immediately.  The request remains
//              queued until LoadingManager::ProcessRequests() is invoked.
//
// Parameters:  handlerType - an enumeration is used to specify which 
//              file handler to use for loading and processing the data 
//              (the use of an enumeration keeps the clients of LoadingManager
//              from being dependant on any FileHandler classes.
//
//              filename - fully qualified path and name of file.
//
//              secitonName - hack to allow p3d inventory sections to be created
//                            based on filenames
//
//              groupName - this allows a memtag section to be declared.
//
// Return:      true - on success
//              false - request queue is full or loading is in progress
//
//==============================================================================
void LoadingManager::AddRequest
( 
    FileHandlerEnum handlerType, 
    const char* filename,
    GameMemoryAllocator heap,
    const char* sectionName,
    const char* groupTag,
    LoadingManager::ProcessRequestsCallback* pCallback, 
    void* pUserData
)
{
    if ( mCancellingLoads )
    {
        return;
    }

    unsigned newTail = (mRequestTail + 1) % MAX_REQUESTS;
    
#ifndef FINAL
    rReleaseAssertMsg( static_cast< int >( newTail ) != mRequestHead, "Too many load requests already!\n");

    // Dusit [Dec 2, 2002]:
    // Bad if we're overwriting the other load request. This is a fatal error.
    if( static_cast<int>(newTail) == mRequestHead )
    {
        IRadTextDisplay* textDisplay;
        ::radTextDisplayGet( &textDisplay, GMA_DEFAULT );
        if ( textDisplay )
        {
            textDisplay->SetBackgroundColor( 0 );
            textDisplay->SetTextColor( 0xffffffff );
            textDisplay->Clear();
            textDisplay->TextOutAt( "TOO MANY LOAD REQUESTS!", 20, 10 );
            textDisplay->TextOutAt( ">:-8", 20, 14 );
            if ( CommandLineOptions::Get( CLO_DEMO_TEST ) ||
                GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_DEMO_TEST ) )
            {
                char buffy[32];
                sprintf( buffy, "Demo Count: %d", GetGame()->GetDemoCount() );
                textDisplay->TextOutAt( buffy, 20, 16 );

                unsigned int time = GetGame()->GetTime();
                unsigned int hours = time / 3600000;
                unsigned int deltaTime = time % 3600000;

                unsigned int minutes = deltaTime / 60000;
                deltaTime = deltaTime % 60000;

                unsigned int seconds = deltaTime / 1000;
                deltaTime = deltaTime % 1000;
                sprintf( buffy, "Time: %d:%d:%d.%d", hours, minutes, seconds, deltaTime );
                textDisplay->TextOutAt( buffy, 20, 18 );

                if ( GetGameplayManager() )
                {
                    sprintf( buffy, "Level %d", GetGameplayManager()->GetCurrentLevelIndex() );
                    textDisplay->TextOutAt( buffy, 20, 20 );
                }
            }
            textDisplay->SwapBuffers();
            textDisplay->Release();
        }
        rReleaseBreak();
    }
#endif

    //mRequests[mNumRequests].filename = filename;
    rAssert( strlen( filename ) < LoadingRequest::LOADING_FILENAME_LENGTH );
    strcpy( mRequests[mRequestTail].filename, filename );

    mRequests[mRequestTail].pFileHandler = FileHandlerFactory::CreateFileHandler( handlerType, sectionName );
    mRequests[mRequestTail].pFileHandler->AddRef();

#ifdef MEMORYTRACKER_ENABLED
    if( groupTag == NULL )
    {
        //
        // Use the filename by default
        //
        rAssert( strlen( filename ) < LoadingRequest::LOADING_FILENAME_LENGTH );
        strcpy( mRequests[mRequestTail].groupTag, filename );
    }
    else
    {
        rAssert( strlen( groupTag ) < LoadingRequest::LOADING_FILENAME_LENGTH );
        strcpy( mRequests[mRequestTail].groupTag, groupTag );
    }
#endif
    
    mRequests[mRequestTail].heap = heap;
    mRequests[mRequestTail].pCallback = pCallback;
    mRequests[mRequestTail].pUserData = pUserData;
    mRequestTail = newTail;

    this->ProcessNextRequest();
}

//==============================================================================
// LoadingManager::OnLoadFileComplete
//==============================================================================
//
// Description: FileHandler's will invoke this callback when an asynchronous
//              load completes.
//
// Parameters:  pUserData - optional user data that was passed in when the
//                          load was requested
//
// Return:      None.
//
//==============================================================================
void LoadingManager::OnLoadFileComplete( void* pUserData )
{
    rAssert( (int)pUserData == mRequestHead );

    // Display some debug info.
    LoadingRequest& request = mRequests[mRequestHead ];
    
    extern bool gLoadingSpew;
    
    if ( !(CommandLineOptions::Get( CLO_NO_LOADING_SPEW )) )
    {
        rReleasePrintf( "<< END >> Async Loading: %s (%u msecs)\n", 
                    request.filename,
                    radTimeGetMilliseconds() - request.startTime );
    }
    
    // Continue onto the next request.
    mRequestHead = (mRequestHead + 1) % MAX_REQUESTS;
    mLoading = false;

    if(request.pCallback)
    {
        request.pCallback->OnProcessRequestsComplete(request.pUserData);
    }

    request.pFileHandler->Release();

    this->ProcessNextRequest();
}


//==============================================================================
// LoadingManager::LoadSync
//==============================================================================
//
// Description: Load a file synchronously.
//
// Parameters:  handlerType - an enumeration is used to specify which 
//              file handler to use for loading and processing the data 
//              (the use of an enumeration keeps the clients of LoadingManager
//              from being dependant on any FileHandler classes.
//
//              filename - fully qualified path and name of file.
//
// Return:      None.
//
//==============================================================================
void LoadingManager::LoadSync
( 
    FileHandlerEnum handlerType, 
    const char* filename,
    GameMemoryAllocator heap,
    const char* sectionName
)
{
    rReleasePrintf("\n\n!!!!!! TRC VIOLATION, USE ASYNC!!!!!!!\n\n");
//    rAssert( false );

    unsigned int startTime = radTimeGetMilliseconds();

    if ( !(CommandLineOptions::Get( CLO_NO_LOADING_SPEW )) )
    {    
        rDebugPrintf( "<<START>> Sync Loading: %s\n", filename );
    }


    HeapMgr()->PushHeap( GMA_TEMP );
    FileHandler* pHandler = FileHandlerFactory::CreateFileHandler( handlerType, sectionName );

    HeapMgr()->PopHeap( GMA_TEMP );
    pHandler->AddRef( );
    rAssert( pHandler );

    HeapMgr()->PushHeap( heap );
    pHandler->LoadFileSync( filename );
    HeapMgr()->PopHeap( heap );

    pHandler->Release( );
    pHandler = 0;

    if ( !(CommandLineOptions::Get( CLO_NO_LOADING_SPEW )) )
    {
        rReleasePrintf( "<< END >> Sync Loading: %s (%u msecs)\n", 
                    filename,
                    radTimeGetMilliseconds() - startTime );
    }                    
}

//=============================================================================
// LoadingManager::CancelPendingRequests
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoadingManager::CancelPendingRequests()
{
    if ( mRequestHead != mRequestTail )
    {
        mRequestTail = (mRequestHead + 1) % MAX_REQUESTS;  //Catch up and lose the rest of the loads.
    }

    mCancellingLoads = true;

    while ( mLoading )
    {
        ::radFileService();
        p3d::loadManager->SwitchTask();
        p3d::loadManager->TriggerCallbacks();      
    }

    //Close all the open cement files.
    CementFileHandle i;
    for ( i = 0; i < MAX_CEMENT_LIBRARIES; ++i )
    {
        if ( mCementLibraries[ i ].library != NULL )
        {
            UnregisterCementLibrary( i );
        }
    }
}

//=============================================================================
// LoadingManager::RegisterCementLibrary
//=============================================================================
// Description: Registers the named cement library with RadFile.
//
// Parameters:  filename - name of cement file
//
// Return:      CementFileHandle - internally, its the index of the cement
//                                 library pointer.  Used for indicating which
//                                 cement library to release later on
//
//=============================================================================
CementFileHandle LoadingManager::RegisterCementLibrary( const char* filename )
{
    if ( mCancellingLoads )
    {
        return 0;
    }

    int i;

    for( i = 0; i < MAX_CEMENT_LIBRARIES; i++ )
    {
        if( ( mCementLibraries[i].library == NULL ) 
            && ( mCementLibraries[i].isLoading == false ) )
        {
            break;
        }
    }

    rAssertMsg( ( i < MAX_CEMENT_LIBRARIES ), "Too many cement libraries\n" );

    //
    // Put a file handler into the queue so that we'll wait until this
    // thing is fully registered.  This file handler is slightly different
    // than the rest, and we're in a special-purpose function anyway, so don't 
    // go through the factory.
    //
    int newTail = (mRequestTail + 1) % MAX_REQUESTS;

    if( newTail != mRequestHead)
    {
        rAssert( strlen( filename ) < LoadingRequest::LOADING_FILENAME_LENGTH );
        strcpy( mRequests[mRequestTail].filename, filename );
#ifdef MEMORYTRACKER_ENABLED
        strcpy( mRequests[mRequestTail].groupTag, filename );
#endif
#ifdef RAD_GAMECUBE
        mRequests[mRequestTail].heap = GMA_GC_VMM;
#else
        mRequests[mRequestTail].heap = GMA_PERSISTENT;          // Cement library registrations should be around for the duration --jdy
#endif

MEMTRACK_PUSH_GROUP( "LoadingManager" );
        HeapMgr()->PushHeap (GMA_TEMP);

        mRequests[mRequestTail].pFileHandler = new(GMA_TEMP) CementFileHandler( &(mCementLibraries[i]) );
        mRequests[mRequestTail].pFileHandler->AddRef();

        mCementLibraries[i].isLoading = true;

        HeapMgr()->PopHeap (GMA_TEMP);
MEMTRACK_POP_GROUP("LoadingManager");

        mRequests[mRequestTail].pCallback = NULL;

        mRequestTail = newTail;

        this->ProcessNextRequest();

        return( i );
    }
    else
    {
        rWarningMsg( 0, "Failed to add cement library request" );
        return( -1 );
    }
}

//=============================================================================
// LoadingManager::UnregisterCementLibrary
//=============================================================================
// Description: Unregister a cement library (hence the name)
//
// Parameters:  handle - in practice, the array index for the cement library
//
// Return:      void 
//
//=============================================================================
void LoadingManager::UnregisterCementLibrary( CementFileHandle handle )
{
    rAssert( handle >= 0 );
    //rAssert( mCementLibraries[handle].library != NULL );

    //
    // Just to be safe, make sure that the cement library is inactive before
    // we release it.  Synchronous wait okay here?
    //
    if( mCementLibraries[handle].library != NULL )
    {
        mCementLibraries[handle].library->WaitForCompletion();

        mCementLibraries[handle].library->Release();
        mCementLibraries[handle].library = NULL;
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// LoadingManager::LoadingManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//============================================================================== 
LoadingManager::LoadingManager()
:
    mRequestHead( 0 ),
    mRequestTail( 0 ),
    mLoading(false),
    mCancellingLoads( false )
{
    int i;

    for( i = 0; i < MAX_CEMENT_LIBRARIES; i++ )
    {
        mCementLibraries[i].library = NULL;
        mCementLibraries[i].isLoading = false;
    }

#ifdef RAD_WIN32
    mRequestsProcessed = 0;
#endif
}


//==============================================================================
// LoadingManager::~LoadingManager
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//============================================================================== 
LoadingManager::~LoadingManager()
{
    int i;

    for( i = 0; i < MAX_CEMENT_LIBRARIES; i++ )
    {
        if( mCementLibraries[i].library != NULL )
        {
            mCementLibraries[i].library->Release();
        }
    }
}


//==============================================================================
// LoadingManager::ProcessNextRequest
//==============================================================================
//
// Description: Handle the next load request in the queue.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void LoadingManager::ProcessNextRequest()
{
    // Are we done?
    if(!mLoading)
    {
        if(mRequestHead != mRequestTail)
        {
            // Start loading the next file.
            LoadingRequest& request = mRequests[mRequestHead];
        
            if ( request.filename[0] != '\0' )
            {
                if ( !(CommandLineOptions::Get( CLO_NO_LOADING_SPEW )) )
                {            
                    rReleasePrintf( "<<START>> Async Loading: %s\n", request.filename );
                }
#ifdef RAD_WIN32
                mRequestsProcessed++;
#endif

                request.startTime = radTimeGetMilliseconds();
                mLoading = true;
                GameMemoryAllocator heap = request.heap;

#ifndef RAD_RELEASE
                if (HeapManager::s_bSpecialRoute)
                {
                    heap = GMA_SPECIAL;
                }
#endif

                request.pFileHandler->LoadFile( request.filename, 
                    this, 
                    (void*)mRequestHead,
                    heap );
            }
            else
            {
                rAssert( request.pCallback != NULL );

                mRequestHead = (mRequestHead + 1) % MAX_REQUESTS;
                mLoading = false;

                request.pCallback->OnProcessRequestsComplete(request.pUserData);
                ProcessNextRequest();
            }
        }
    }
}


