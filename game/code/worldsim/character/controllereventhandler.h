//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        controllereventhandler.h
//
// Description: Blahblahblah
//
// History:     09/08/2002 + Created -- NAME
//
//=============================================================================

#ifndef CONTROLLEREVENTHANDLER_H_
#define CONTROLLEREVENTHANDLER_H_

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================
#include <events/eventlistener.h>

class CameraRelativeCharacterController;

class CameraRelativeCharacterControllerEventHandler
:
public EventListener
{
public:
    CameraRelativeCharacterControllerEventHandler( CameraRelativeCharacterController* pParent )
        :
    mpParent( pParent )
    {
    }
    // Derived classes must implement this method to receive
    // event notification.
    virtual void HandleEvent( EventEnum id, void* pEventData )
    {
        mpParent->HandleEvent( id, pEventData );
    }
protected:
private:
    CameraRelativeCharacterControllerEventHandler( void );
    CameraRelativeCharacterController* mpParent;
};

#endif //CONTROLLEREVENTHANDLER_H_