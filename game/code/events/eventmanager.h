//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        eventmanager.h
//
// Description: EventManager class declaration.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

//========================================
// System Includes
//========================================
#include <vector>

//========================================
// Project Includes
//========================================
#include <events/eventenum.h>
#include <memory/stlallocators.h>

//Uncomment this if you wanna see how many listeners there are to a given event.
#define TRACK_LISTENERS

//========================================
// Forward References
//========================================
class EventListener;

//==============================================================================
//
// Synopsis: Used to trigger events and route them to the listeners.
//
//==============================================================================
class EventManager
{
    public:

        // Static Methods for accessing this singleton.
        static EventManager* CreateInstance();
        static EventManager* GetInstance();
        static void DestroyInstance();

        // Methods to add/remove listeners.
        void AddListener( EventListener* pListener, EventEnum id );
        void RemoveListener( EventListener* pListener, EventEnum id );
        void RemoveAll( EventListener* pListener );

        // Trigger an event.
        int TriggerEvent( EventEnum id, void* pEventData = NULL ) const;

    private:
        
        // No public access to these, use singleton interface.
        EventManager();
        ~EventManager();

        // Declared but not defined to prevent copying and assignment.
        EventManager( const EventManager& );
        EventManager& operator=( const EventManager& );

        // Pointer to the one and only instance of this singleton.
        static EventManager* spInstance;

        // Each unique event has a container of attached listeners.
        typedef std::vector< EventListener*, s2alloc<EventListener*> > ListenerVector;
        ListenerVector mListenerMap[NUM_EVENTS];

#ifdef TRACK_LISTENERS
        struct EventStats {
            EventStats() : numListening( 0 ), highWater( 0 ) {};
            unsigned int numListening;
            unsigned int highWater;
        };

        EventStats mStats[NUM_EVENTS];
#endif
};


// A little syntactic sugar for getting at this singleton.
inline EventManager* GetEventManager() { return( EventManager::GetInstance() ); }


#endif // EVENTMANAGER_H

