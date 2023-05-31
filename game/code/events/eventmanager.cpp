//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        eventmanager.cpp
//
// Description: Implementation for the EventManager class.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
// STL
#include <algorithm>
// Ftech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <events/eventmanager.h>
#include <events/eventlistener.h>
#include <memory/srrmemory.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
EventManager* EventManager::spInstance = NULL;

#ifdef DEBUGWATCH
    unsigned int g_simulatedEventID = 0;
    static void TriggerSimulatedEvent()
    {
        GetEventManager()->TriggerEvent( static_cast<EventEnum>( g_simulatedEventID ) );
    }
#endif // DEBUGWATCH

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// EventManager::CreateInstance
//==============================================================================
//
// Description: Creates the EventManager.
//
// Parameters:	None.
//
// Return:      Pointer to the EventManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
EventManager* EventManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "EventManager" );
    rAssert( spInstance == NULL );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

    spInstance = new EventManager;
    rAssert( spInstance );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif

MEMTRACK_POP_GROUP( "EventManager" );
    
    return spInstance;
}

//==============================================================================
// EventManager::GetInstance
//==============================================================================
//
// Description: - Access point for the EventManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the EventManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
EventManager* EventManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// EventManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the EventManager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void EventManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}


//==============================================================================
// EventManager::AddListener
//==============================================================================
//
// Description: Register a listener for event notification.
//
// Parameters:  pListener - the object to receive event notification
//              id - the EventEnum that will trigger notification
//
// Return:      None.
//
//==============================================================================
void EventManager::AddListener( EventListener* pListener, EventEnum id )
{
    rAssert( NULL != pListener );

MEMTRACK_PUSH_GROUP( "EventManager" );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif


    ListenerVector& listeners = mListenerMap[id];

    // check for duplicates.
    ListenerVector::iterator result = std::find( listeners.begin(),
                                                  listeners.end(),
                                                  pListener );
    if( result == listeners.end() )
    {
        listeners.push_back( pListener );
    }
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif

#ifdef TRACK_LISTENERS
        mStats[id].numListening += 1;
        if ( mStats[id].numListening > mStats[id].highWater )
        { 
            mStats[id].highWater = mStats[id].numListening;
        }
#endif
MEMTRACK_POP_GROUP( "EventManager" );
}


//==============================================================================
// EventManager::RemoveListener
//==============================================================================
//
// Description: Remove a listener from event notification.
//
// Parameters:  pListener - the listener to be removed
//              id - the EventEnum it was listening for
//
// Return:      None.
//
//==============================================================================
void EventManager::RemoveListener( EventListener* pListener, EventEnum id )
{
    rAssert( NULL != pListener );

    ListenerVector& listeners = mListenerMap[id];

    ListenerVector::iterator result = std::find( listeners.begin(),
                                                 listeners.end(),
                                                 pListener );
    // Did we find the listener?
    if( result != listeners.end() )
    {
        listeners.erase( result );

#ifdef TRACK_LISTENERS
        mStats[id].numListening--;
#endif
    }
}


//==============================================================================
// EventManager::RemoveAll
//==============================================================================
//
// Description: Remove a listener from *ALL* event notifications.
//
// Parameters:  pListener - the listener to be removed.
//
// Return:      None.
//
//==============================================================================
void EventManager::RemoveAll( EventListener* pListener )
{
    // Iterate through all the listener arrays.
    int i = 0;
    for( ; i < NUM_EVENTS; ++i )
    {
        this->RemoveListener( pListener, static_cast<EventEnum>(i) );
    }
}


//==============================================================================
// EventManager::TriggerEvent
//==============================================================================
//
// Description: Trigger an event. All registered listeners for that event
//              are notified.
//
// Parameters:  id - the event to trigger
//              pEventData - any custom data associated with the event
//
// Return:      Number of listeners notified.
//
//==============================================================================
int EventManager::TriggerEvent( EventEnum id, void* pEventData ) const
{
    const ListenerVector& listeners = mListenerMap[id];
    
    ListenerVector::const_iterator iter = listeners.begin();

    // Notify all listeners of the event.
    size_t size = listeners.size();
    size_t i;
    for( i = 0; i < size; ++i )
    {
        EventListener* el = listeners[ i ];
        rAssert( el != NULL );
        el->HandleEvent( id, pEventData );
        size = listeners.size();    //the event may have added or removed listeners
        
        // if the current event was removed we need to detect that
        EventListener* newel = listeners[ i ];

        // Don't want to do anything if we are now past the end of the list, 
        // or if the current event hasn't changed
        if((i < size) && (newel != el))
        {
            // we have removed the current event
            // back the array index off one so that we will 
            // trigger the same event index again
            i--;
        }
    }

    return listeners.size();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// EventManager::EventManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
EventManager::EventManager()
{
    //TODO:  When we're leak free, make this smaller and see where we get.
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif
    unsigned int i;
    for ( i = 0; i < NUM_EVENTS; ++i )
    {
        mListenerMap[i].reserve( 32 );
    }
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
#ifdef DEBUGWATCH
    radDbgWatchAddUnsignedInt( &g_simulatedEventID,
                               "Simulated Event ID",
                               "Event Manager",
                               (RADDEBUGWATCH_CALLBACK)TriggerSimulatedEvent,
                               NULL,
                               0,
                               NUM_EVENTS - 1 );
#endif // DEBUGWATCH
}



//==============================================================================
// EventManager::~EventManager
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
EventManager::~EventManager()
{
#ifdef RAD_DEBUG
    // Iterate through all the listener arrays and check that all listeners
    // have been removed.
    int i = 0;
    for( ; i < NUM_EVENTS; ++i )
    {
        const ListenerVector& listeners = mListenerMap[i];
        rAssertMsg( 0 == listeners.size(), "EventListener has not been removed!\n" );
    }
#endif // RAD_DEBUG

#ifdef DEBUGWATCH
    radDbgWatchDelete( &g_simulatedEventID );
#endif // DEBUGWATCH

#ifdef TRACK_LISTENERS
    for ( int j = 0; j < NUM_EVENTS; ++j )
    {
        rReleasePrintf( "EventID: %d HighWater: %d\n", j, mStats[j].highWater );
    }
#endif
}
