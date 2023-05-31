//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        exitcontext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef EXITCONTEXT_H
#define EXITCONTEXT_H

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
// Synopsis:    
//
//=============================================================================
class ExitContext : public Context
{
    public:

        // Static Methods for accessing this singleton.
        static ExitContext* GetInstance();

    protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        virtual void OnHandleEvent( EventEnum id, void* pEventData );

    private:

        // constructor and destructor are protected to force singleton implementation
        ExitContext();
        virtual ~ExitContext();

        // Declared but not defined to prevent copying and assignment.
        ExitContext( const ExitContext& );
        ExitContext& operator=( const ExitContext& );

        // Pointer to the one and only instance of this singleton.
        static ExitContext* spInstance;
};

// A little syntactic sugar for getting at this singleton.
inline ExitContext* GetExitContext() { return( ExitContext::GetInstance() ); }


#endif
