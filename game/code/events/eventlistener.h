//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        eventlistener.h
//
// Description: EventManager class declaration.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#ifdef WORLD_BUILDER
#include "eventenum.h"
#else
#include <events/eventenum.h>
#endif

//========================================
// Forward References
//========================================

//==============================================================================
//
// Synopsis: Abstract base class for all classes that want to be notified
//           of events.
//
//==============================================================================
class EventListener
{
    public:
    
        EventListener();
        virtual ~EventListener();

        // Derived classes must implement this method to receive
        // event notification.
        virtual void HandleEvent( EventEnum id, void* pEventData ) = 0;

    private:
        
        // Declared but not defined to prevent copying and assignment.
        EventListener( const EventListener& );
        EventListener& operator=( const EventListener& );
};


#endif // EVENTLISTENER_H

