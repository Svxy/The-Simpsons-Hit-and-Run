//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        p3dfilehandler.h
//
// Description: Declaration of P3DFileHandler class.
//
// History:     3/25/2002 + Created -- Darwin Chau
//
//=============================================================================

#ifndef P3DFILEHANDLER_H
#define P3DFILEHANDLER_H

//========================================
// Nested Includes
//========================================
#include <p3d/loadmanager.hpp>
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
class P3DFileHandler : public tLoadRequest::Callback,
					   public FileHandler
                       
{
    public:

        P3DFileHandler();
        virtual ~P3DFileHandler();

        //
        // Implement FileHandler interface.
        //
        virtual void LoadFile( const char* filename, 
                               FileHandler::LoadFileCallback* pCallback,
                               void* pUserData,
                               GameMemoryAllocator heap );
        
        virtual void LoadFileSync( const char* filename );

        //
        // Implement tLoadRequest::Callback interface.
        //
        virtual void Done( tLoadStatus status, tLoadRequest *load );
        virtual void AddRef( void )
        {
			radLoadObject::AddRef();
        }
		virtual void Release( void )
        {
			radLoadObject::Release();
        }  

        //
        // Specify which P3D inventory section to load the file into.
        //
        void SetSectionName( const char* sectionName );
        const char* GetSectionName() { return( mcSectionName ); }

    private:
  
        // Prevent wasteful constructor creation.
        P3DFileHandler( const P3DFileHandler& p3dfilehandler );
        P3DFileHandler& operator=( const P3DFileHandler& p3dfilehandler );

        char mcSectionName[32];
        void* mpUserData;
        int m_RefCount;
};


#endif //P3DFILEHANDLER_H