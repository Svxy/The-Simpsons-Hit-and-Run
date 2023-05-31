//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        filehandlerfactory.cpp
//
// Description: Implement FileHandlerFactory
//
// History:     3/27/2002 + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <loading/filehandlerfactory.h>
#include <loading/p3dfilehandler.h>
#include <loading/choreofilehandler.h>
#include <loading/consolefilehandler.h>
#include <loading/iconfilehandler.h>
#include <loading/scroobyfilehandler.h>
#include <loading/soundfilehandler.h>
#include <memory/srrmemory.h>

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
// FileHandlerFactory::CreateFileHandler
//==============================================================================
//
// Description: Instantiates and returns a file handler of the specified type.
//
// Parameters:  handlerType - the type of file handler to create
//              allocator - the memory heap to use
//
//              secitonName - hack to allow p3d inventory sections to be created
//                            based on filenames
//
// Return:      pointer to the file handler
//
// Constraints: The client is responsible for deleting the returned file handler.
//
//==============================================================================
FileHandler* FileHandlerFactory::CreateFileHandler
( 
    FileHandlerEnum handlerType,
    const char* sectionName
)
{
MEMTRACK_PUSH_GROUP( "FileHandler Factory" );

    FileHandler* pHandler = NULL;

    HeapMgr()->PushHeap(GMA_TEMP);

    switch( handlerType )
    {
        case FILEHANDLER_PURE3D:
        {
            pHandler = new P3DFileHandler();
            
            if( sectionName != 0 )
            {
                static_cast<P3DFileHandler*>(pHandler)->SetSectionName( sectionName );
            }
            else
            {
                static_cast<P3DFileHandler*>(pHandler)->SetSectionName( "Default" );
            }

            break;
        }

        case FILEHANDLER_LEVEL:
        {
            pHandler = new P3DFileHandler();
            if( sectionName != 0 )
            {
                static_cast<P3DFileHandler*>(pHandler)->SetSectionName( sectionName );
            }
            else
            {
                static_cast<P3DFileHandler*>(pHandler)->SetSectionName( "Level" );
            }
            break;
        }
        case FILEHANDLER_MISSION:
        {
            pHandler = new P3DFileHandler();
            static_cast<P3DFileHandler*>(pHandler)->SetSectionName( "Mission" );
    
            break;
        }
        case FILEHANDLER_ANIMATION:
        {
            pHandler = new P3DFileHandler();
            static_cast<P3DFileHandler*>(pHandler)->SetSectionName( sectionName );

            break;
        }
        case FILEHANDLER_CHOREO:
        {
            pHandler = new ChoreoFileHandler();
            static_cast<ChoreoFileHandler*>(pHandler)->SetSectionName( sectionName );

            break;
        }
        case FILEHANDLER_CONSOLE:
        {
            pHandler = new ConsoleFileHandler();

            break;
        }
        case FILEHANDLER_SCROOBY:
        {
            pHandler = new ScroobyFileHandler();
            static_cast<ScroobyFileHandler*>(pHandler)->SetSectionName( sectionName );

            break;
        }
        case FILEHANDLER_SOUND:
        {
            pHandler = new SoundFileHandler();

            break;
        }
        case FILEHANDLER_TEMP:
        {
            pHandler = new P3DFileHandler();
            static_cast<P3DFileHandler*>(pHandler)->SetSectionName( "Temp" );

            break;
        }
        case FILEHANDLER_ICON:
        {
            pHandler = new IconFileHandler();

            break;
        }
        default:
        {
            rAssert( 0 );
        }
    }

    HeapMgr()->PopHeap(GMA_TEMP);

MEMTRACK_POP_GROUP("FileHandler Factory");
    return( pHandler );
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
