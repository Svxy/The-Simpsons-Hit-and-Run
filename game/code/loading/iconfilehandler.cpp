//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        IconFileHandler.cpp
//
// Description: Implement IconFileHandler
//
// History:     01/21/2002 + Created -- Tony Chu
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
#include <loading/iconfilehandler.h>
#include <memory/srrmemory.h>

#include <data/memcard/memorycardmanager.h>

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
// IconFileHandler::IconFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
IconFileHandler::IconFileHandler()
    :
    mpIconFile( 0 ),
    mFileDataBuffer( 0 ),
    mHeap( GMA_DEFAULT ),
    mAsyncLoadState( NONE )
{
}

//==============================================================================
// IconFileHandler::~IconFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
IconFileHandler::~IconFileHandler()
{
}


//==============================================================================
// IconFileHandler::LoadFile 
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
void IconFileHandler::LoadFile 
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
    mHeap = heap;

    // by default, use FTT IRadFiles
    //
    mAsyncLoadState = OPENFILE;

    radFileOpen( &mpIconFile,
                 filename,
                 false,
                 OpenExisting,
                 NormalPriority,
                 0,
                 heap );

    rAssert( mpIconFile != 0 );
    
    mpIconFile->AddCompletionCallback( this, pUserData );
}


//==============================================================================
// IconFileHandler::OnFileOperationsComplete
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
void IconFileHandler::OnFileOperationsComplete( void* pUserData )
{
    switch( mAsyncLoadState )
    {
        case OPENFILE:
        {
MEMTRACK_PUSH_GROUP( "IconFileHandler" );
            unsigned int length = mpIconFile->GetSize();
            mFileDataBuffer = new( mHeap ) char[ length ];

            mpIconFile->ReadAsync( mFileDataBuffer, length );
            mpIconFile->AddCompletionCallback( this, pUserData );

            mAsyncLoadState = READDATA;
MEMTRACK_POP_GROUP("IconFileHandler");
        }
        break;
        
        case READDATA:
        {
            unsigned int length = mpIconFile->GetSize();

            GetMemoryCardManager()->SetMemcardIconData( mFileDataBuffer, length );

            // it's up to the client to free the file data buffer!
            //
            mFileDataBuffer = 0;

            mpIconFile->Release();
            mpIconFile = 0;

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
// IconFileHandler::LoadFileSync 
//==============================================================================
//
// Description: Load a Pure3D file synchronously.
//
// Parameters:  filename - fully qualified path and filename
//
// Return:      None.
//
//==============================================================================
void IconFileHandler::LoadFileSync( const char* filename )
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

