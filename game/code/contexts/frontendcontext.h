//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        FrontEndcontext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef FRONTENDCONTEXT_H
#define FRONTENDCONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/context.h>   // is-a Context

#include <loading/loadingmanager.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    
//
//=============================================================================
class FrontEndContext : public Context,
                        public LoadingManager::ProcessRequestsCallback
{
    public:

        // Static Methods for accessing this singleton.
        static FrontEndContext* GetInstance();
        
    protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        virtual void OnHandleEvent( EventEnum id, void* pEventData );
        virtual void OnProcessRequestsComplete( void* pUserData );   

    private:

        void StartFrontEnd( unsigned int initialScreen );

        // constructor and destructor are protected to force singleton implementation
        FrontEndContext();
        virtual ~FrontEndContext();

        // Declared but not defined to prevent copying and assignment.
        FrontEndContext( const FrontEndContext& );
        FrontEndContext& operator=( const FrontEndContext& );

        // Pointer to the one and only instance of this singleton.
        static FrontEndContext* spInstance;
};

// A little syntactic sugar for getting at this singleton.
inline FrontEndContext* GetFrontEndContext() { return( FrontEndContext::GetInstance() ); }


#endif // FRONTENDCONTEXT_H
