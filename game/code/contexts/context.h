//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        context.h
//
// Description: Context base class declaration.
//
// History:     + Stolen and cleaned up from Penthouse -- Darwin Chau
//
//=============================================================================

#ifndef CONTEXT_H
#define CONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/contextenum.h>
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    This is the Context Controller base class. It will be used as
//              a base class for all context controllers in the game, such as
//              PlayContext and PauseContext. These will
//              be updated from the GameFlow.
//
//=============================================================================
class Context : public EventListener
{
    public:

        void DestroyInstance();

        // gameflow should call these functions to manipulate the context controller; 
        // derived classes must NOT over-ride the following non-virtual functions; 
        // these functions will call the corresponding protected virtual functions
        void Start( ContextEnum previousContext );
        void Stop( ContextEnum nextContext );
        void Update( unsigned int elapsedTime );
        
        // The game must halt when the disc door is opened on the GameCube.
        void Suspend();
        void Resume();
        bool IsSuspended() const {return m_state == S_SUSPENDED;};
        
        virtual void HandleEvent( EventEnum id, void* pEventData );

    protected:

        // constructor and destructor are protected to force singleton implementation
        Context();
        virtual ~Context();

        // derived classes MUST over-ride the following virtual functions to implement 
        // custom behaviour; these functions are called by the corresponding public 
        // non-virtual functions
        virtual void OnStart( ContextEnum previousContext ) = 0;
        virtual void OnStop( ContextEnum nextContext ) = 0;
        virtual void OnUpdate( unsigned int elapsedTime ) = 0;
        
        virtual void OnSuspend() = 0;
        virtual void OnResume() = 0;

        virtual void OnHandleEvent( EventEnum id, void* pEventData ) = 0;

        enum StateEnum
        {
            S_NONE, 
            S_READY, 
            S_ACTIVE, 
            S_SUSPENDED, 
            S_EXIT, 
            MAX_STATES
        };
        
        StateEnum m_state;

    private:

        // Declared but not defined to prevent copying and assignment.
        Context( const Context& );
        Context& operator=( const Context& );
};

#endif
