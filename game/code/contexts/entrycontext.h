//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        entrycontext.h
//
// Description: EntryContext class declaration.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef ENTRYCONTEXT_H
#define ENTRYCONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/context.h>   // is-a Context

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis: The game is initialized to this empty context.  It doesn't
//           do anything!    
//
//=============================================================================
class EntryContext : public Context
{
    public:

        // Static Methods for accessing this singleton.
        static EntryContext* GetInstance();

    protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        virtual void OnHandleEvent( EventEnum id, void* pEventData );

    private:

        // constructor and destructor are protected to force singleton implementation
        EntryContext();
        virtual ~EntryContext();

        // Declared but not defined to prevent copying and assignment.
        EntryContext( const EntryContext& );
        EntryContext& operator=( const EntryContext& );

        // Pointer to the one and only instance of this singleton.
        static EntryContext* spInstance;
};

//
// A little syntactic sugar for getting at this singleton.
//
inline EntryContext* GetEntryContext() { return( EntryContext::GetInstance() ); }


#endif // ENTRYCONTEXT_H
