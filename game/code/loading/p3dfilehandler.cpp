//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        p3dfilehandler.cpp
//
// Description: Implement P3DFileHandler
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
#include <loading/p3dfilehandler.h>

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
// P3DFileHandler::P3DFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
P3DFileHandler::P3DFileHandler() : m_RefCount( 0 )
{
}

//==============================================================================
// P3DFileHandler::~P3DFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
P3DFileHandler::~P3DFileHandler()
{
}


//==============================================================================
// P3DFileHandler::LoadFile 
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
void P3DFileHandler::LoadFile 
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

    //
    // Ensure that the specified inventory section exists before loading
    //
    HeapMgr()->PushHeap (heap);
    HeapMgr()->PushHeap (GMA_TEMP);
    p3d::inventory->AddSection( mcSectionName );
    HeapMgr()->PopHeap (GMA_TEMP);
    p3d::loadAsync( filename, mcSectionName, this, pUserData, heap );
    HeapMgr()->PopHeap (heap);
}


//==============================================================================
// P3DFileHandler::Done
//==============================================================================
//
// Description: Pure3D (via FTech) invokes this callback when the async load
//              is complete.
//
// Parameters:  tLoadStatus status, tLoadRequest *load
//
// Return:      None.
//
//==============================================================================
void P3DFileHandler::Done( tLoadStatus status, tLoadRequest *load )
{
    //
    // Percolate the callback up to the client.
    //
    mpCallback->OnLoadFileComplete( mpUserData );
}


//==============================================================================
// P3DFileHandler::LoadFileSync 
//==============================================================================
//
// Description: Load a Pure3D file synchronously.
//
// Parameters:  filename - fully qualified path and filename
//
// Return:      None.
//
//==============================================================================
void P3DFileHandler::LoadFileSync( const char* filename )
{
    rAssert( filename );

    rReleasePrintf("Synchronous File Load. Bastard! %s\n", filename);
    rAssert( false );

    p3d::inventory->PushSection();

    //
    // Ensure that the specified inventory section exists before loading
    //
    p3d::inventory->AddSection( mcSectionName );
    p3d::inventory->SelectSection( mcSectionName );
    
    // SetInventorySection(mcSectionName );

    tLoadRequest* pLR = new tLoadRequest(p3d::openFile(filename));
    pLR->SetInventorySection(mcSectionName);
    tLoadStatus result = p3d::loadManager->Load(pLR);

    // bool result = p3d::load( filename );

    p3d::inventory->PopSection();

    rAssert( result == LOAD_OK );
}



//==============================================================================
// P3DFileHandler::SetSectionName
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void P3DFileHandler::SetSectionName( const char* sectionName )
{
    rAssert( sectionName );

    strcpy( mcSectionName, sectionName );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************






