//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SuperSprintFEContext.h
//
// Description: Blahblahblah
//
// History:     03/26/2003 + Created -- Tony Chu
//
//=============================================================================

#ifndef SUPERSPRINTFECONTEXT_H
#define SUPERSPRINTFECONTEXT_H

//========================================
// Nested Includes
//========================================
#include <contexts/context.h> // is-a Context

#include <loading/loadingmanager.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SuperSprintFEContext : public Context,
                             public LoadingManager::ProcessRequestsCallback
{
public:
    // Static Methods for accessing this singleton.
    static SuperSprintFEContext* GetInstance();

protected:
    virtual void OnStart( ContextEnum previousContext );
    virtual void OnStop( ContextEnum nextContext );
    virtual void OnUpdate( unsigned int elapsedTime );

    virtual void OnSuspend();
    virtual void OnResume();

    virtual void OnHandleEvent( EventEnum id, void* pEventData );
    virtual void OnProcessRequestsComplete( void* pUserData );   

private:
    static SuperSprintFEContext* spInstance;

    SuperSprintFEContext();
    virtual ~SuperSprintFEContext();

    //Prevent wasteful constructor creation.
    SuperSprintFEContext( const SuperSprintFEContext& SuperSprintFEContext );
    SuperSprintFEContext& operator=( const SuperSprintFEContext& SuperSprintFEContext );

};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

inline SuperSprintFEContext* GetSuperSprintFEContext()
{
    return SuperSprintFEContext::GetInstance();
};

#endif // SUPERSPRINTFECONTEXT_H
