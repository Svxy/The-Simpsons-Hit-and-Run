//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        LoadingDemoContext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef LOADINGDEMOCONTEXT_H
#define LOADINGDEMOCONTEXT_H

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
class LoadingDemoContext : public LoadingContext
{
    public:
        // Static Methods for accessing this singleton.
        static LoadingDemoContext* GetInstance();

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
        LoadingDemoContext();
        virtual ~LoadingDemoContext();

        // Declared but not defined to prevent copying and assignment.
        LoadingDemoContext( const LoadingDemoContext& );
        LoadingDemoContext& operator=( const LoadingDemoContext& );

        // Pointer to the one and only instance of this singleton.
        static LoadingDemoContext* spInstance;

};

// A little syntactic sugar for getting at this singleton.
inline LoadingDemoContext* GetLoadingDemoContext() { return( LoadingDemoContext::GetInstance() ); }


#endif // LOADINGDEMOCONTEXT_H
