//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudEventHandler
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/hudeventhandler.h>

// Scrooby
#include <group.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

HudEventHandler::HudEventHandler( Scrooby::Group* drawableGroup )
:   m_drawableGroup( drawableGroup ),
    m_elapsedTime( 0.0f ),
    m_currentState( STATE_IDLE )
{
    rAssert( drawableGroup != NULL );
    drawableGroup->SetVisible( false );
}

HudEventHandler::~HudEventHandler()
{
}

void
HudEventHandler::OnStart()
{
    if( m_currentState != STATE_RUNNING )
    {
        // show all drawable elements in group
        //
        rAssert( m_drawableGroup != NULL );
        m_drawableGroup->SetVisible( true );

        // reset elapsed time
        //
        m_elapsedTime = 0.0;

        // update current state
        //
        m_currentState = STATE_RUNNING;
    }
}

void
HudEventHandler::OnStop()
{
    if( m_currentState != STATE_IDLE )
    {
        // hide all drawable elements in group
        //
        rAssert( m_drawableGroup != NULL );
        m_drawableGroup->SetVisible( false );

        // update current state
        //
        m_currentState = STATE_IDLE;
    }
}

