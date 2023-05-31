//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        avatarvehicleposnplayer.h
//
// Description: Plays positional idle for avatar vehicle
//
// History:     3/7/2003 + Created -- Esan
//
//=============================================================================

#ifndef AVATARVEHICLEPOSNPLAYER_H
#define AVATARVEHICLEPOSNPLAYER_H

//========================================
// Nested Includes
//========================================
#include <sound/movingpositional/vehicleposnsoundplayer.h>

#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    AvatarVehiclePosnPlayer
//
//=============================================================================

class AvatarVehiclePosnPlayer : public VehiclePositionalSoundPlayer,
                                public EventListener
{
    public:
        AvatarVehiclePosnPlayer();
        virtual ~AvatarVehiclePosnPlayer();

        //
        // EventListener functions
        //
        void HandleEvent( EventEnum id, void* pEventData );

        void StartPositionalIdle( Vehicle* carPtr = NULL );

    private:
        //Prevent wasteful constructor creation.
        AvatarVehiclePosnPlayer( const AvatarVehiclePosnPlayer& avatarvehicleposnplayer );
        AvatarVehiclePosnPlayer& operator=( const AvatarVehiclePosnPlayer& avatarvehicleposnplayer );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //AVATARVEHICLEPOSNPLAYER_H
