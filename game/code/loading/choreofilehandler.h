//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Choreofilehandler.h
//
// Description: Declaration of ChoreoFileHandler class.
//
// History:     3/25/2002 + Created -- Darwin Chau
//
//=============================================================================

#ifndef CHOREOFILEHANDLER_H
#define CHOREOFILEHANDLER_H

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
class ChoreoFileHandler : public FileHandler,
                       public IRadFileCompletionCallback
{
    public:

        ChoreoFileHandler();
        virtual ~ChoreoFileHandler();

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

        //
        // Specify which Choreo inventory section to load the file into.
        //
        void SetSectionName( const char* sectionName );
        const char* GetSectionName() { return( mcSectionName ); }

    private:
        enum ChoreoFileState
        {
            NONE,
            OPENFILE,
                READDATA,
                DONE
        };
        // Async Load State.
        //
        ChoreoFileState m_state;
        // Data buffer.
        //
        char* mScriptString;
        
        // Prevent wasteful constructor creation.
        ChoreoFileHandler( const ChoreoFileHandler& Choreofilehandler );
        ChoreoFileHandler& operator=( const ChoreoFileHandler& Choreofilehandler );

        char mcSectionName[32];
        IRadFile* mpScriptFile;
};


#endif //CHOREOFILEHANDLER_H