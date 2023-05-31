//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wrecklesseventlistener.h
//
// Description: Blahblahblah
//
// History:     13/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef WRECKLESSEVENTLISTENER_H
#define WRECKLESSEVENTLISTENER_H

//========================================
// Nested Includes
//========================================
#include <events/eventlistener.h>

#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================
class Locator;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WrecklessEventListener : public EventListener
{
public:
    WrecklessEventListener();
    virtual ~WrecklessEventListener();

    void HandleEvent( EventEnum id, void* pEventData );

    void GetLastPosition( rmt::Vector* pos );

    void SetPlayerID( int playerID );

private:

    rmt::Vector mLastPosition;
    int mPlayerID;

    //Prevent wasteful constructor creation.
    WrecklessEventListener( const WrecklessEventListener& wrecklesseventlistener );
    WrecklessEventListener& operator=( const WrecklessEventListener& wrecklesseventlistener );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// WrecklessEventListener::GetLastPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* pos )
//
// Return:      void 
//
//=============================================================================
inline void WrecklessEventListener::GetLastPosition( rmt::Vector* pos )
{
    *pos = mLastPosition;
}

//=============================================================================
// WrecklessEventListener::SetPlayerID
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID )
//
// Return:      void 
//
//=============================================================================
inline void WrecklessEventListener::SetPlayerID( int playerID )
{
    mPlayerID = playerID;
}

#endif //WRECKLESSEVENTLISTENER_H
