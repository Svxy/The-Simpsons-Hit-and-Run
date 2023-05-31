//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        IconFileHandler.h
//
// Description: Declaration of IconFileHandler class.
//
// History:     01/21/2003 + Created -- Tony Chu
//
//=============================================================================

#ifndef ICONFILEHANDLER_H
#define ICONFILEHANDLER_H

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
class IconFileHandler : public FileHandler,
                        public IRadFileCompletionCallback
{
    public:

        IconFileHandler();
        virtual ~IconFileHandler();

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
        IconFileHandler( const IconFileHandler& iconFileHandler );
        IconFileHandler& operator=( const IconFileHandler& iconFileHandler );

        IRadFile* mpIconFile;
        char* mFileDataBuffer;
        GameMemoryAllocator mHeap;

        enum AsyncLoadState
        {
            NONE,
            OPENFILE,
            READDATA,
            DONE
        };

        AsyncLoadState mAsyncLoadState;

};


#endif // ICONFILEHANDLER_H
