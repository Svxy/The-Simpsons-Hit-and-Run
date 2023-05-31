//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Gameplaycontext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef GAMEPLAYCONTEXT_H
#define GAMEPLAYCONTEXT_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <contexts/playingcontext.h>   // is-a PlayingContext

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    
//
//=============================================================================
class GameplayContext : public PlayingContext
{
    public:

        // Static Methods for accessing this singleton.
        static GameplayContext* GetInstance();
        void         PauseAllButPresentation( const bool pause );
        bool IsPaused() { return m_PausedAllButPresentation; }

    protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

    private:

        // constructor and destructor are protected to force singleton implementation
        GameplayContext();
        virtual ~GameplayContext();

        // Declared but not defined to prevent copying and assignment.
        GameplayContext( const GameplayContext& );
        GameplayContext& operator=( const GameplayContext& );

        // Pointer to the one and only instance of this singleton.
        static GameplayContext* spInstance;
        
#ifdef DEBUGWATCH
        unsigned int mDebugPhysTiming, mDebugTimeDelta, mDebugOnUpdateDT;
#endif
        bool    m_PausedAllButPresentation;
        
        bool mSlowMoHack;

};

// A little syntactic sugar for getting at this singleton.
inline GameplayContext* GetGameplayContext() { return( GameplayContext::GetInstance() ); }


#endif // GAMEPLAYCONTEXT_H
