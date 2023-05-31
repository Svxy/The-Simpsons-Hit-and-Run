//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Loadingcontext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef LOADINGCONTEXT_H
#define LOADINGCONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/context.h>   // is-a Context
#include <loading/loadingmanager.h> // also is-a ProcessRequests thingy

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    
//
//=============================================================================
class LoadingContext : public Context,
                       public LoadingManager::ProcessRequestsCallback
{
    public:
        virtual void OnHandleEvent( EventEnum id, void* pEventData );

        //
        // LoadingManager::ProcessRequestsCallback
        //
        virtual void OnProcessRequestsComplete( void* pUserData );

    protected:
        // constructor and destructor are protected to force singleton implementation
        LoadingContext();
        virtual ~LoadingContext();

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        virtual void PrepareNewHeaps() = 0;

    private:
        // Declared but not defined to prevent copying and assignment.
        LoadingContext( const LoadingContext& );
        LoadingContext& operator=( const LoadingContext& );

};

#endif // LOADINGCONTEXT_H
