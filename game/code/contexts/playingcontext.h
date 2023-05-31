//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        PlayingContext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef PLAYINGCONTEXT_H
#define PLAYINGCONTEXT_H

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
class PlayingContext : public Context
{
    public:
        virtual void OnHandleEvent( EventEnum id, void* pEventData );

    protected:
        // constructor and destructor are protected to force singleton implementation
        PlayingContext();
        virtual ~PlayingContext();

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        bool mQuitting;

    private:
        // Declared but not defined to prevent copying and assignment.
        PlayingContext( const PlayingContext& );
        PlayingContext& operator=( const PlayingContext& );

};

#endif // PLAYINGCONTEXT_H
