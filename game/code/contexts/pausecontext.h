//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        PauseContext.h
//
// Description: 
//
// History:     + Created -- Tony Chu
//
//=============================================================================

#ifndef PAUSECONTEXT_H
#define PAUSECONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/context.h>   // is-a Context
#include <input/controller.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    
//
//=============================================================================
class PauseContext : public Context
{
    public:

        // Static Methods for accessing this singleton.
        static PauseContext* GetInstance();

        bool IsWaitingForContextSwitch() { return m_waitingForContextSwitch; }
        void SetWaitingForContextSwitch( bool tf ) { m_waitingForContextSwitch = tf; }

    protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        virtual void OnHandleEvent( EventEnum id, void* pEventData );

    private:

        // constructor and destructor are protected to force singleton implementation
        PauseContext();
        virtual ~PauseContext();

        // Declared but not defined to prevent copying and assignment.
        PauseContext( const PauseContext& );
        PauseContext& operator=( const PauseContext& );

        // Pointer to the one and only instance of this singleton.
        static PauseContext* spInstance;

        Input::ActiveState mOldState;
        bool m_quitGamePending : 1;
        bool m_waitingForContextSwitch;        
};

// A little syntactic sugar for getting at this singleton.
inline PauseContext* GetPauseContext() { return( PauseContext::GetInstance() ); }


#endif // PAUSECONTEXT_H
