//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supersprintcontext.h
//
// Description: Blahblahblah
//
// History:     2/8/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SUPERSPRINTCONTEXT_H
#define SUPERSPRINTCONTEXT_H

//========================================
// Nested Includes
//========================================
#include <contexts/playingcontext.h>   // is-a PlayingContext

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SuperSprintContext : public PlayingContext
{
public:
    // Static Methods for accessing this singleton.
    static SuperSprintContext* GetInstance();

protected:
    virtual void OnStart( ContextEnum previousContext );
    virtual void OnStop( ContextEnum nextContext );
    virtual void OnUpdate( unsigned int elapsedTime );

    virtual void OnSuspend();
    virtual void OnResume();

private:
    static SuperSprintContext* spInstance;

    SuperSprintContext();
    virtual ~SuperSprintContext();

    //Prevent wasteful constructor creation.
    SuperSprintContext( const SuperSprintContext& supersprintcontext );
    SuperSprintContext& operator=( const SuperSprintContext& supersprintcontext );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

inline SuperSprintContext* GetSPCTX() { return SuperSprintContext::GetInstance(); };

#endif //SUPERSPRINTCONTEXT_H
