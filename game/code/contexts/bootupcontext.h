//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        bootupcontext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef BOOTUPCONTEXT_H
#define BOOTUPCONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/context.h>   // is-a Context

#include <loading/loadingmanager.h>
#include <presentation/presevents/presentationevent.h>

//========================================
// Forward References
//========================================
class HeadToHeadManager;
class pddiShader;

//=============================================================================
//
// Synopsis:    
//
//=============================================================================
class BootupContext : public Context,
                      public LoadingManager::ProcessRequestsCallback,
                      public PresentationEvent::PresentationEventCallBack
{
    public:

        // Static Methods for accessing this singleton.
        static BootupContext* GetInstance();

        void StartMovies();
        void StartLoadingSound();
        void ResetLicenseScreenDisplayTime() { m_elapsedTime = 0; }

#ifdef RAD_WIN32
        void LoadConfig();
#endif

        pddiShader* GetSharedShader( void ) { return m_pSharedShader; }

    protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

        virtual void OnHandleEvent( EventEnum id, void* pEventData );

        virtual void OnProcessRequestsComplete( void* pUserData );   

        virtual void OnPresentationEventBegin( PresentationEvent* pEvent );
        virtual void OnPresentationEventLoadComplete( PresentationEvent* pEvent );
        virtual void OnPresentationEventEnd( PresentationEvent* pEvent );

    private:

        // constructor and destructor are protected to force singleton implementation
        BootupContext();
        virtual ~BootupContext();

        // Declared but not defined to prevent copying and assignment.
        BootupContext( const BootupContext& );
        BootupContext& operator=( const BootupContext& );

        // Pointer to the one and only instance of this singleton.
        static BootupContext* spInstance;

        int m_elapsedTime;
        bool m_bootupLoadCompleted : 1;
        bool m_soundLoadCompleted : 1;

        pddiShader* m_pSharedShader;
};

// A little syntactic sugar for getting at this singleton.
inline BootupContext* GetBootupContext() { return( BootupContext::GetInstance() ); }


#endif
