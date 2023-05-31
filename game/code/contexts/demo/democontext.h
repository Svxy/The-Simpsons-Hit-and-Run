//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        DemoContext.h
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef DEMOCONTEXT_H
#define DEMOCONTEXT_H

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
class DemoContext : public PlayingContext
{
    public:

        // Static Methods for accessing this singleton.
        static DemoContext* GetInstance();

        void EndDemo(void) { m_elapsedTime = m_demoLoopTime + 1; }

        void SetDemoLoopTime( unsigned int timems ) { m_demoLoopTime = timems; };

protected:

        virtual void OnStart( ContextEnum previousContext );
        virtual void OnStop( ContextEnum nextContext );
        virtual void OnUpdate( unsigned int elapsedTime );
        
        virtual void OnSuspend();
        virtual void OnResume();

    private:

        // constructor and destructor are protected to force singleton implementation
        DemoContext();
        virtual ~DemoContext();

        // Declared but not defined to prevent copying and assignment.
        DemoContext( const DemoContext& );
        DemoContext& operator=( const DemoContext& );

        // Pointer to the one and only instance of this singleton.
        static DemoContext* spInstance;

        unsigned int m_demoLoopTime;
        int m_elapsedTime;

};

// A little syntactic sugar for getting at this singleton.
inline DemoContext* GetDemoContext() { return( DemoContext::GetInstance() ); }


#endif // DEMOCONTEXT_H
