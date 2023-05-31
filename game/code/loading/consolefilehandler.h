//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        consolefilehandler.h
//
// Description: Declaration of ConsoleFileHandler class.
//
// History:     3/25/2002 + Created -- Darwin Chau
//
//=============================================================================

#ifndef CONSOLEFILEHANDLER_H
#define CONSOLEFILEHANDLER_H

//========================================
// Nested Includes
//========================================
#include <loading/filehandler.h>
#include <radfile.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis: File handler for loading Pure3D files.
//
//=============================================================================
class ConsoleFileHandler : public FileHandler,
                           public IRadFileCompletionCallback
{
    public:

        ConsoleFileHandler();
        virtual ~ConsoleFileHandler();

        //
        // Implement FileHandler interface.
        //
        virtual void LoadFile( const char* filename, 
                               FileHandler::LoadFileCallback* pCallback,
                               void* pUserData,
                               GameMemoryAllocator heap );
        
        virtual void LoadFileSync( const char* filename );

        //
        // Implement IRadFileCompletionCallback interface.
        //
        virtual void OnFileOperationsComplete( void* pUserData );

        virtual void AddRef() {FileHandler::AddRef();}
        virtual void Release() {FileHandler::Release();}

    private:

        // Prevent wasteful constructor creation.
        ConsoleFileHandler( const ConsoleFileHandler& ConsoleFileHandler );
        ConsoleFileHandler& operator=( const ConsoleFileHandler& ConsoleFileHandler );

        IRadFile* mpConsoleFile;

        enum AsyncLoadState
        {
            NONE,
            OPENFILE,
            READDATA,
            DONE
        };

        AsyncLoadState mAsyncLoadState;

        char* mFileDataBuffer;
};


#endif // CONSOLEFILEHANDLER_H