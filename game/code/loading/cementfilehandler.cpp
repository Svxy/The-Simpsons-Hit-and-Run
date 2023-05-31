//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        cementfilehandler.cpp
//
// Description: Implements CementFileHandler class, which allows us to add
//              cement file registration to the loading manager file queue
//
// History:     30/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <loading/cementfilehandler.h>

#include <main/commandlineoptions.h>


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
// CementFileHandler::CementFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters:	library - Cement library that we're waiting on for registration
//
// Return:      N/A.
//
//==============================================================================
CementFileHandler::CementFileHandler( LoadingManager::CementLibraryStruct* libraryStruct ) :
    m_libraryStruct( libraryStruct )
{
}

//==============================================================================
// CementFileHandler::~CementFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
CementFileHandler::~CementFileHandler()
{
}

//=============================================================================
// CementFileHandler::LoadFile
//=============================================================================
// Description: Start looking at the cement library to see if it's registered
//
// Parameters:  filename - ignored, since we're not really loading a file
//              pCallback - loading manager, to be signalled on registration
//                          completion
//              pUserData - user data to be passed back in callback
//
// Return:      void 
//
//=============================================================================
void CementFileHandler::LoadFile( const char* filename, 
                                  FileHandler::LoadFileCallback* pCallback, 
                                  void* pUserData,
                                  GameMemoryAllocator heap )
{
    radCementLibraryPriority priority;

    mpCallback = pCallback;
    mpUserData = pUserData;

    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
        priority = radCementLibraryBeforeDrive;
    }
    else
    {
        priority = radCementLibraryAfterDrive;
    }

    ::radFileRegisterCementLibrary( &(m_libraryStruct->library),
                                    filename,
                                    priority,
                                    0,
                                    GMA_PERSISTENT );

    m_libraryStruct->library->SetCompletionCallback( this, NULL );
}

//=============================================================================
// CementFileHandler::LoadFileSync
//=============================================================================
// Description: Unused synchronous loading function
//
// Parameters:  Ignored
//
// Return:      void 
//
//=============================================================================
void CementFileHandler::LoadFileSync( const char* filename )
{
    //
    // RadFile doesn't offer synchronous cement file registration, so we shouldn't
    // see this function called
    //
    rAssert( false );
}

//=============================================================================
// CementFileHandler::OnCementLibraryRegistered
//=============================================================================
// Description: Called by the cement library when registration is complete
//
// Parameters:  Unused
//
// Return:      void 
//
//=============================================================================
void CementFileHandler::OnCementLibraryRegistered( void* pUserData )
{
    m_libraryStruct->isLoading = false;

    mpCallback->OnLoadFileComplete( mpUserData );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
