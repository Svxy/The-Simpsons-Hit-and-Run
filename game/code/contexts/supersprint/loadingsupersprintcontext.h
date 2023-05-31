//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        LoadingSuperSprintContext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef LoadingSuperSprintContext_H
#define LoadingSuperSprintContext_H

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
class LoadingSuperSprintContext : public LoadingContext
{
    public:
        // Static Methods for accessing this singleton.
        static LoadingSuperSprintContext* GetInstance();

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
        LoadingSuperSprintContext();
        virtual ~LoadingSuperSprintContext();

        // Declared but not defined to prevent copying and assignment.
        LoadingSuperSprintContext( const LoadingSuperSprintContext& );
        LoadingSuperSprintContext& operator=( const LoadingSuperSprintContext& );

        // Pointer to the one and only instance of this singleton.
        static LoadingSuperSprintContext* spInstance;
        
        bool mUnQueuedLoadRequests;
};

// A little syntactic sugar for getting at this singleton.
inline LoadingSuperSprintContext* GetLoadingSuperSprintContext() { return( LoadingSuperSprintContext::GetInstance() ); }


#endif // LoadingSuperSprintContext_H
