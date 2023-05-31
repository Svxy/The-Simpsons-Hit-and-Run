//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        consolefilehandler.cpp
//
// Description: Implement ConsoleFileHandler
//
// History:     3/25/2002 + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <string.h>
// Pure 3D
#include <p3d/utility.hpp>
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <loading/consolefilehandler.h>
#include <memory/srrmemory.h>
#include <console/console.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ConsoleFileHandler::ConsoleFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ConsoleFileHandler::ConsoleFileHandler()
    :
    mpConsoleFile( 0 ),
    mAsyncLoadState( NONE ),
    mFileDataBuffer( 0 )
{
}

//==============================================================================
// ConsoleFileHandler::~ConsoleFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ConsoleFileHandler::~ConsoleFileHandler()
{
}


//==============================================================================
// ConsoleFileHandler::LoadFile 
//==============================================================================
//
// Description: Load a Pure3D file asynchronously.
//
// Parameters:  filename - fully qualified path and filename
//              pCallback - client callback to invoke when load is complete
//              pUserData - optional client supplied user data
//
// Return:      None.
//
//==============================================================================
void ConsoleFileHandler::LoadFile 
(
    const char* filename, 
    FileHandler::LoadFileCallback* pCallback,
    void* pUserData,
    GameMemoryAllocator heap
)
{
    rAssert( filename );
    rAssert( pCallback );

    mpCallback = pCallback;

    // by default, use FTT IRadFiles
    //
    mAsyncLoadState = OPENFILE;

    radFileOpen( &mpConsoleFile,
                 filename,
                 false,
                 OpenExisting,
                 NormalPriority,
                 0,
                 GMA_TEMP );

    rAssert( mpConsoleFile != 0 );
    
    mpConsoleFile->AddCompletionCallback( this, 
                                          reinterpret_cast<void*>(pUserData) );
}


//==============================================================================
// ConsoleFileHandler::OnFileOperationsComplete
//==============================================================================
//
// Description: Pure3D (via FTech) invokes this callback when the async load
//              is complete.
//
// Parameters:  pUserData - optional client supplied user data
//
// Return:      None.
//
//==============================================================================
void ConsoleFileHandler::OnFileOperationsComplete( void* pUserData )
{
    switch( mAsyncLoadState )
    {
        case OPENFILE:
        {
            
MEMTRACK_PUSH_GROUP( "ConsoleFileHandler" );
            unsigned int length = mpConsoleFile->GetSize();
            mFileDataBuffer = new(GMA_TEMP) char[length + 1];
            
            mpConsoleFile->ReadAsync( mFileDataBuffer, length );
            mpConsoleFile->AddCompletionCallback( this, (void*)pUserData );
            
            mAsyncLoadState = READDATA;
MEMTRACK_POP_GROUP("ConsoleFileHandler");
        }
        break;
        
        case READDATA:
        {
            unsigned int length = mpConsoleFile->GetSize();
            mFileDataBuffer[length] = '\0';

            GetConsole()->Evaluate( mFileDataBuffer, mpConsoleFile->GetFilename() );


            delete[]( GMA_TEMP, mFileDataBuffer );
            mFileDataBuffer = 0;
            
            mpConsoleFile->Release();
            mpConsoleFile = 0;
            
            mAsyncLoadState = DONE;

            //
            // Percolate the callback up to the client. This must be done last!!!
            //
            mpCallback->OnLoadFileComplete( pUserData );
        }
        break;

        default:
        {
            rAssert( 0 );
        }
    }
}


//==============================================================================
// ConsoleFileHandler::LoadFileSync 
//==============================================================================
//
// Description: Load a Pure3D file synchronously.
//
// Parameters:  filename - fully qualified path and filename
//
// Return:      None.
//
//==============================================================================
void ConsoleFileHandler::LoadFileSync( const char* filename )
{
MEMTRACK_PUSH_GROUP( "ConsoleFileHandler" );
    rAssert( filename );

    radFileOpen( &mpConsoleFile,
                 filename,
                 false,
                 OpenExisting,
                 NormalPriority,
                 0,
                 GMA_TEMP );

    rAssert( mpConsoleFile != 0 );

    mpConsoleFile->WaitForCompletion();

    unsigned int length = mpConsoleFile->GetSize();
    mFileDataBuffer = new(GMA_TEMP) char[length + 1];
    
    mpConsoleFile->ReadAsync( mFileDataBuffer, length );
    mpConsoleFile->WaitForCompletion();

    mFileDataBuffer[length] = '\0';

    GetConsole()->Evaluate( mFileDataBuffer, filename );

    delete[]( GMA_TEMP, mFileDataBuffer );
    mFileDataBuffer = 0;
    
    mpConsoleFile->Release();
    mpConsoleFile = 0;
MEMTRACK_POP_GROUP("ConsoleFileHandler");
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************






