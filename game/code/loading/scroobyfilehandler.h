//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ScroobyFileHandler.h
//
// Description: Declaration of ConsoleFileHandler class.
//
// History:     07/19/2002 + Created -- Tony Chu
//
//=============================================================================

#ifndef SCROOBYFILEHANDLER_H
#define SCROOBYFILEHANDLER_H

//========================================
// Nested Includes
//========================================
#include <loading/filehandler.h>
#include <app.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis: File handler for loading Scrooby project files.
//
//=============================================================================
class ScroobyFileHandler : public FileHandler,
                           public Scrooby::LoadProjectCallback
{
    public:

        ScroobyFileHandler();
        virtual ~ScroobyFileHandler();

        //
        // Implement FileHandler interface.
        //
        virtual void LoadFile( const char* filename, 
                               FileHandler::LoadFileCallback* pCallback,
                               void* pUserData,
                               GameMemoryAllocator heap );

        virtual void LoadFileSync( const char* filename );

        //
        // Implements Scrooby::LoadProjectCallback interface.
        //
        virtual void OnProjectLoadComplete( Scrooby::Project* pProject );

        //
        // Specify which Scrooby inventory section to load the project.
        //
        void SetSectionName( const char* sectionName );
        const char* GetSectionName() { return( mcSectionName ); }

    private:

        // Prevent wasteful constructor creation.
        ScroobyFileHandler( const ScroobyFileHandler& scroobyFileHandler );
        ScroobyFileHandler& operator=( const ScroobyFileHandler& scroobyFileHandler );

        char mcSectionName[ 32 ];

};


#endif // SCROOBYFILEHANDLER_H
