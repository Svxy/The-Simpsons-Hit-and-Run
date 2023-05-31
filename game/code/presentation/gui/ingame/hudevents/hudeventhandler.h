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

#ifndef HUDEVENTHANDLER_H
#define HUDEVENTHANDLER_H

//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Group;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class HudEventHandler
{
public:
    HudEventHandler( Scrooby::Group* drawableGroup );
    virtual ~HudEventHandler();

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Update( float elapsedTime ) = 0;

    bool IsActive() const;

protected:
    void OnStart();
    void OnStop();

    Scrooby::Group* m_drawableGroup;
    float m_elapsedTime;

    enum eState
    {
        STATE_IDLE,
        STATE_RUNNING,

        NUM_STATES
    };

    eState m_currentState;

};

inline bool HudEventHandler::IsActive() const
{
    return( m_currentState != STATE_IDLE );
}

#endif // HUDEVENTHANDLER_H
