//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GameFlow   
//
// Description: The GameFlow Controller orchestrates the overall game execution.
//
// History:     + Stolen and cleaned up from Penthouse -- Darwin Chau
//
//=============================================================================

#ifndef GAMEFLOW_H
#define GAMEFLOW_H

//========================================
// System Includes
//========================================
#include <vector>
#include <stack>

#include <radtime.hpp>  // IRadTimerCallback

//========================================
// Project Includes
//========================================
#include <contexts/contextenum.h>
#include <memory/stlallocators.h>

//========================================
// Forward References
//========================================
class Context;


//=============================================================================
//
// Synopsis:    The game "loop"
//
//=============================================================================
class GameFlow : public IRadTimerCallback
{
    public:

        // Static Methods (for creating and getting an instance of the game)
        static GameFlow* CreateInstance();
        static GameFlow* GetInstance();
        static void  DestroyInstance();

        // Functions to change the context.
        void PushContext( ContextEnum context );
        void PopContext();
        void SetContext( ContextEnum context );

        // Implement IRadTimerCallback interface.
        // This member is called whenever the timer expires.
        void OnTimerDone( unsigned int elapsedtime, void* pUserData );
        
        ContextEnum GetCurrentContext() const { return mCurrentContext; }
        ContextEnum GetNextContext() const { return mNextContext; }

        Context* GetContext( ContextEnum which ) const { return mpContexts[which];  }
/*
        bool GetQuickStartLoading( void ) const { return mQuickStartLoading; }
        void SetQuickStartLoading( bool IsQuickStartLoading ) { mQuickStartLoading = IsQuickStartLoading; }
*/
    private:

        // Declared but not defined to prevent copying and assignment.
        GameFlow( const GameFlow& );
        GameFlow& operator=( const GameFlow& );

        // Constructor - these are private to prevent anybody else from 
        // creating me.
        GameFlow();
        virtual ~GameFlow();

        // This member is called when the gameflow is being initialized.
        void Initialize();

        // The one and only GameFlow instance.
        static GameFlow* spInstance;

        // Timer for gameflow updates.
        IRadTimer* mpITimer;

        // Contexts
        ContextEnum mCurrentContext;
        ContextEnum mNextContext;
        
        typedef std::vector< ContextEnum, s2alloc<ContextEnum> > ContextEnumSequence;
        typedef std::stack< ContextEnum, ContextEnumSequence > ContextStack;

        ContextStack mContextStack;
        
        Context* mpContexts[NUM_CONTEXTS];
        bool mQuickStartLoading : 1;
};


//
// A little syntactic sugar for getting at this singleton.
//
inline GameFlow* GetGameFlow() { return( GameFlow::GetInstance() ); }


#endif
