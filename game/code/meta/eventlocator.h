//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        eventlocator.h
//
// Description: Blahblahblah
//
// History:     04/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef EVENTLOCATOR_H
#define EVENTLOCATOR_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#include <meta/triggerlocator.h>
#include <meta/locatortypes.h>
#include <meta/locatorevents.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class EventLocator : public TriggerLocator
{
public:
    EventLocator();
    virtual ~EventLocator();
  
    virtual LocatorType::Type   GetDataType() const;

    LocatorEvent::Event GetEventType() const;
    void SetEventType( LocatorEvent::Event eventType );

    void SetMatrix( const rmt::Matrix& mat );
    rmt::Matrix& GetMatrix();

private:
    virtual void OnTrigger( unsigned int playerID );

    //Prevent wasteful constructor creation.
    EventLocator( const EventLocator& eventlocator );
    EventLocator& operator=( const EventLocator& eventlocator );

    LocatorEvent::Event mEventType;

    rmt::Matrix mMatrix;
};

//=============================================================================
// EventLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type EventLocator::GetDataType() const
{
    return( LocatorType::EVENT );
}

//=============================================================================
// EventLocator::SetEventType
//=============================================================================
// Description: Comment
//
// Parameters:  ( LocatorEvent eventType )
//
// Return:      inline 
//
//=============================================================================
inline void EventLocator::SetEventType( LocatorEvent::Event eventType )
{
    mEventType = eventType;
}

//=============================================================================
// EventLocator::GetEventType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorEvent::Event EventLocator::GetEventType() const
{
    return( mEventType );
}

//=============================================================================
// EventLocator::SetMatrix
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Matrix& mat )
//
// Return:      void 
//
//=============================================================================
inline void EventLocator::SetMatrix( const rmt::Matrix& mat )
{
    mMatrix = mat;
}

//=============================================================================
// EventLocator::GetMatrix
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      rmt
//
//=============================================================================
inline rmt::Matrix& EventLocator::GetMatrix()
{
    return mMatrix;
}

#endif //EVENTLOCATOR_H
