//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        choreofilehandler.cpp
//
// Description: Implement ChoreoFileHandler
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
// Choreo
#include <choreo/load.hpp>

//========================================
// Project Includes
//========================================
#include <loading/choreofilehandler.h>
#include <memory/srrmemory.h>
#include <worldsim/character/charactermanager.h>

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
// ChoreoFileHandler::ChoreoFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ChoreoFileHandler::ChoreoFileHandler()
:
m_state( NONE ),
mScriptString( 0 ),
mpScriptFile( 0 )
{
}

//==============================================================================
// ChoreoFileHandler::~ChoreoFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ChoreoFileHandler::~ChoreoFileHandler()
{
}


//==============================================================================
// ChoreoFileHandler::LoadFile 
//==============================================================================
//
// Description: Load a choreo file asynchronously.
//
// Parameters:  filename - fully qualified path and filename
//              pCallback - client callback to invoke when load is complete
//              pUserData - optional client supplied user data
//
// Return:      None.
//
//==============================================================================
void ChoreoFileHandler::LoadFile 
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
    m_state = OPENFILE;
    choreo::RegisterDefaultScriptHandlers();
    radFileOpen(&mpScriptFile,
                filename,
                false,
                OpenExisting,
                NormalPriority);
    P3DASSERT(mpScriptFile != 0);
    mpScriptFile->AddCompletionCallback( this, (void*)pUserData );
    

}


//==============================================================================
// ChoreoFileHandler::OnFileOperationsComplete
//==============================================================================
//
// Description: FTech invokes this callback when the async load
//              is complete.
//
// Parameters:  pUserData - optional client supplied user data
//
// Return:      None.
//
//==============================================================================
void ChoreoFileHandler::OnFileOperationsComplete( void* pUserData )
{
    switch ( m_state )
    {
    case OPENFILE:
        {
            
MEMTRACK_PUSH_GROUP( "ChoreoFileHandler" );
            unsigned length = mpScriptFile->GetSize();
            mScriptString = new(GMA_TEMP) char [length + 1];
            mpScriptFile->ReadAsync( mScriptString, length );
            mpScriptFile->AddCompletionCallback( this, (void*)pUserData );
            m_state = READDATA;
MEMTRACK_POP_GROUP("ChoreoFileHandler");

            break;
        }
    case READDATA:
        {
            unsigned length = mpScriptFile->GetSize();
            mScriptString[length] = '\0';
            
            p3d::inventory->PushSection( );
            p3d::inventory->SelectSection( GetSectionName() );
            HeapMgr()->PushHeap(GMA_LEVEL_ZONE);
            bool rc = choreo::ReadFromScriptString(mScriptString, mpScriptFile->GetFilename(), p3d::inventory );
            HeapMgr()->PopHeap(GMA_LEVEL_ZONE);
            p3d::inventory->PopSection();
            rAssertMsg( rc, "Choreo Script load error\n" );

            delete[] mScriptString;
            mScriptString = 0;
            mpScriptFile->Release();
            mpScriptFile = 0;
            m_state = DONE;

            //
            // Percolate the callback up to the client. This must be done last!!!
            //
            mpCallback->OnLoadFileComplete( pUserData );

            break;
        }
    default:
        {
            rAssert( 0 );
            break;
        }
    }

}


//==============================================================================
// ChoreoFileHandler::LoadFileSync 
//==============================================================================
//
// Description: Load a Pure3D file synchronously.
//
// Parameters:  filename - fully qualified path and filename
//
// Return:      None.
//
//==============================================================================
void ChoreoFileHandler::LoadFileSync( const char* filename )
{
    rReleasePrintf("\n\n!!!!!! TRC VIOLATION, USE ASYNC!!!!!!!\n\n");
    rAssert( false );

    rAssert( filename );

    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( GetSectionName() );
    bool result = p3d::load( filename );
    p3d::inventory->PopSection();

    rAssert( result = true );
}



//==============================================================================
// ChoreoFileHandler::SetSectionName
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ChoreoFileHandler::SetSectionName( const char* sectionName )
{
    rAssert( sectionName );

    strcpy( mcSectionName, sectionName );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************






