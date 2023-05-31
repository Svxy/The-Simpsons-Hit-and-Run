//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfilehandler.cpp
//
// Description: Implement SoundFileHandler, which represents sound in the
//              loading queue
//
// History:     19/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <loading/soundfilehandler.h>

#include <sound/soundmanager.h>
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
// SoundFileHandler::SoundFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFileHandler::SoundFileHandler()
{
}

//==============================================================================
// SoundFileHandler::~SoundFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFileHandler::~SoundFileHandler()
{
}

//=============================================================================
// SoundFileHandler::LoadFile
//=============================================================================
// Description: Load sound file asynchronously
//
// Parameters:  filename - name of file to load
//              pCallback - callback to invoke when loading complete
//              pUserData - user data, unused
//
// Return:     void 
//
//=============================================================================
void SoundFileHandler::LoadFile( const char* filename, 
                                 FileHandler::LoadFileCallback* pCallback,
                                 void* pUserData,
                                 GameMemoryAllocator heap )
{
    mpCallback = pCallback;
    mpUserData = pUserData;

    //
    // Pass the load request on to the sound system, giving it this object
    // for notification of completion
    //
    GetSoundManager()->LoadSoundFile( filename, this );
}

//=============================================================================
// SoundFileHandler::LoadFileSync
//=============================================================================
// Description: Load sound file synchronously
//
// Parameters:  filename - name of file to load
//
// Return:      void 
//
//=============================================================================
void SoundFileHandler::LoadFileSync( const char* filename )
{
    //
    // This shouldn't get called.  We don't do synchronous in sound.
    //
    rAssert( false );
}

//=============================================================================
// SoundFileHandler::LoadCompleted
//=============================================================================
// Description: Inform loading manager when asynchronous load completed
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFileHandler::LoadCompleted()
{
    rAssert( mpCallback != NULL );
    mpCallback->OnLoadFileComplete( mpUserData );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
