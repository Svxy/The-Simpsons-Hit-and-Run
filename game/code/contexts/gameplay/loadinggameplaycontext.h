//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        LoadingGameplayContext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef LOADINGGAMEPLAYCONTEXT_H
#define LOADINGGAMEPLAYCONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/loadingcontext.h>   // is-a LoadingContext

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    
//
//=============================================================================
class LoadingGameplayContext : public LoadingContext
{
    public:
        // Static Methods for accessing this singleton.
        static LoadingGameplayContext* GetInstance();

        virtual void OnProcessRequestsComplete( void* pUserData );

    protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        virtual void PrepareNewHeaps();

    private:

        // constructor and destructor are protected to force singleton implementation
        LoadingGameplayContext();
        virtual ~LoadingGameplayContext();

        // Declared but not defined to prevent copying and assignment.
        LoadingGameplayContext( const LoadingGameplayContext& );
        LoadingGameplayContext& operator=( const LoadingGameplayContext& );

        // Pointer to the one and only instance of this singleton.
        static LoadingGameplayContext* spInstance;
        
        bool mUnQueuedLoadRequests;
};

// A little syntactic sugar for getting at this singleton.
inline LoadingGameplayContext* GetLoadingGameplayContext() { return( LoadingGameplayContext::GetInstance() ); }


#endif // LOADINGGAMEPLAYCONTEXT_H
