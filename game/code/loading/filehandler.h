//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        filehandler.h
//
// Description: Declaration of FileHandler abstract base class.
//
// History:     3/25/2002 + Created -- Darwin Chau
//
//=============================================================================

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

//========================================
// Nested Includes
//========================================
#include <radobject.hpp>
#include <radload/utility/object.hpp>
#include "memory/srrmemory.h"

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis: Derive from this abstract base class to implement custom handlers
//           for loading specific file types.
//
//=============================================================================
class FileHandler
//:
//public IRefCount
{
    public:
      
        FileHandler(): m_RefCount( 0 ) {}
        virtual ~FileHandler() {}

        //----------------------------------------------------------------------
        // ASYNCHRONOUS LOADING
        //----------------------------------------------------------------------

        //
        // Clients must implement this callback to be notified when the
        // async load completes.
        //
        struct LoadFileCallback
        {
            virtual void OnLoadFileComplete( void* pUserData ) = 0;        
        };
        
        //
        // Load file asynchronously.
        //
        virtual void LoadFile( const char* filename, 
                               FileHandler::LoadFileCallback* pCallback,
                               void* pUserData,
                               GameMemoryAllocator heap ) = 0;

        //----------------------------------------------------------------------
        // SYNCHRONOUS LOADING
        //----------------------------------------------------------------------
        
        //
        // Load file synchronously.
        //
        virtual void LoadFileSync( const char* filename ) = 0;
    
        virtual void AddRef( void )
        {
            m_RefCount++;
        }
		virtual void Release( void )
        {
            // Copy and paste from radobject.hpp
            //
            rAssert( m_RefCount > 0 && m_RefCount <  MAX_REFCOUNT );
            m_RefCount--;
            if (m_RefCount == 0 )
            {
			    // Must avoid recursive destruction, set refcount to some high
			    // value.

			    m_RefCount = MAX_REFCOUNT / 2;

                delete this;
            }           
        }
    protected:

        LoadFileCallback* mpCallback;
        void* mpUserData;

    private:

        // Declared but not defined to prevent copying and assignment.
        FileHandler( const FileHandler& );
        FileHandler& operator=( const FileHandler& );

        int m_RefCount;
};
                   
#endif // FILEHANDLER_H
