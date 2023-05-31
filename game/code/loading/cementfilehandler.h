//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        cementfilehandler.h
//
// Description: Declares CementFileHandler class, which allows us to add
//              cement file registration to the loading manager file queue
//
// History:     30/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef CEMENTFILEHANDLER_H
#define CEMENTFILEHANDLER_H

//========================================
// Nested Includes
//========================================
#include <radfile.hpp>

#include <loading/filehandler.h>
#include <loading/loadingmanager.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    CementFileHandler
//
//=============================================================================

class CementFileHandler : public FileHandler,
                          public IRadCementLibraryCompletionCallback
{
    public:
        CementFileHandler( LoadingManager::CementLibraryStruct* library );
        virtual ~CementFileHandler();

        //
        // Load file asynchronously.
        //
        void LoadFile( const char* filename, 
                       FileHandler::LoadFileCallback* pCallback,
                       void* pUserData,
                       GameMemoryAllocator heap );

        //
        // Load file synchronously (unused)
        //
        void LoadFileSync( const char* filename );

        //
        // Cement file registration callback
        //
        void OnCementLibraryRegistered( void* pUserData );

        //
        // Playin' fast and loose with reference counting
        //
        virtual void AddRef() {FileHandler::AddRef();}
        virtual void Release() {FileHandler::Release();}

    private:
        // Prevent wasteful constructor creation.
        CementFileHandler();
        CementFileHandler( const CementFileHandler& original );
        CementFileHandler& operator=( const CementFileHandler& rhs );

        LoadingManager::CementLibraryStruct* m_libraryStruct;
};


#endif // CEMENTFILEHANDLER_H

