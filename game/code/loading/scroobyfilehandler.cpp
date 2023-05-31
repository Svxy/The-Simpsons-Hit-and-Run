//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ScroobyFileHandler.cpp
//
// Description: Implement ConsoleFileHandler
//
// History:     07/19/2002 + Created -- Tony Chu
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radtime.hpp>
#include <string.h>

//========================================
// Project Includes
//========================================
#include <loading/scroobyfilehandler.h>
#include <presentation/gui/guisystem.h>


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
// ScroobyFileHandler::ScroobyFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ScroobyFileHandler::ScroobyFileHandler()
{
    mpCallback = NULL;
    mpUserData = NULL;
    mcSectionName[ 0 ] = '\0';
}

//==============================================================================
// ScroobyFileHandler::~ScroobyFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ScroobyFileHandler::~ScroobyFileHandler()
{
}


//==============================================================================
// ScroobyFileHandler::LoadFile 
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
void ScroobyFileHandler::LoadFile 
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
    mpUserData = pUserData;

    HeapMgr()->PushHeap (GMA_PERSISTENT);

    // load scrooby project
    Scrooby::App::GetInstance()->LoadProject( filename, this, mcSectionName, heap );

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}


//==============================================================================
// ScroobyFileHandler::OnProjectLoadComplete
//==============================================================================
//
// Description: Scrooby invokes this callback when the async load
//              is complete.
//
// Parameters:  pUserData - optional client supplied user data
//
// Return:      None.
//
//==============================================================================
void ScroobyFileHandler::OnProjectLoadComplete( Scrooby::Project* pProject )
{
    rAssert( mpCallback );

    // notify GUI system that the project is loaded, and pass reference to
    // project using the first message parameter
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_PROJECT_LOAD_COMPLETE, (unsigned int)pProject );

    // notify client that async loading is completed
    mpCallback->OnLoadFileComplete( mpUserData );
}


//==============================================================================
// ScroobyFileHandler::LoadFileSync 
//==============================================================================
//
// Description: Load a Pure3D file synchronously.
//
// Parameters:  filename - fully qualified path and filename
//
// Return:      None.
//
//==============================================================================
void ScroobyFileHandler::LoadFileSync( const char* filename )
{
    rAssertMsg( 0, "ERROR: Synchronous loading not supported by Scrooby!\n" );
}

//==============================================================================
// ScroobyFileHandler::SetSectionName
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ScroobyFileHandler::SetSectionName( const char* sectionName )
{
    rAssert( sectionName );

    strcpy( mcSectionName, sectionName );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

