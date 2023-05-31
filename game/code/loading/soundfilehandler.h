//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfilehandler.h
//
// Description: Declaration for sound file loader class
//
// History:     19/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDFILEHANDLER_H
#define SOUNDFILEHANDLER_H

//========================================
// Nested Includes
//========================================
#include <loading/filehandler.h>

//========================================
// Forward References
//========================================

class SoundAsyncFileLoader;

//=============================================================================
//
// Synopsis:    SoundFileHandler
//
//=============================================================================

class SoundFileHandler : public FileHandler
{
    public:
        SoundFileHandler();
        virtual ~SoundFileHandler();

        //
        // Load file asynchronously.
        //
        void LoadFile( const char* filename, 
                       FileHandler::LoadFileCallback* pCallback,
                       void* pUserData,
                       GameMemoryAllocator heap );

        //
        // Load file synchronously.
        //
        void LoadFileSync( const char* filename );

        //
        // Called by sound system on load completion
        //
        void LoadCompleted();

    private:
        //Prevent wasteful constructor creation.
        SoundFileHandler( const SoundFileHandler& original );
        SoundFileHandler& operator=( const SoundFileHandler& rhs );

        SoundAsyncFileLoader* m_subtypeFileLoader;
};


#endif // SOUNDFILEHANDLER_H

