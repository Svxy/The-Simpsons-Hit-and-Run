//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        aivehiclesoundplayer.h
//
// Description: Administers the playing of sound for an AI-controlled vehicle
//
// History:     1/4/2003 + Created -- Darren
//
//=============================================================================

#ifndef AIVEHICLESOUNDPLAYER_H
#define AIVEHICLESOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <radtime.hpp>

#include <sound/movingpositional/vehicleposnsoundplayer.h>

//========================================
// Forward References
//========================================
class Vehicle;

//=============================================================================
//
// Synopsis:    AIVehicleSoundPlayer
//
//=============================================================================

class AIVehicleSoundPlayer : public VehiclePositionalSoundPlayer
{
    public:
        AIVehicleSoundPlayer();
        virtual ~AIVehicleSoundPlayer();

        void ServiceOncePerFrame();

    private:
        //Prevent wasteful constructor creation.
        AIVehicleSoundPlayer( const AIVehicleSoundPlayer& aivehiclesoundplayer );
        AIVehicleSoundPlayer& operator=( const AIVehicleSoundPlayer& aivehiclesoundplayer );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //AIVEHICLESOUNDPLAYER_H
