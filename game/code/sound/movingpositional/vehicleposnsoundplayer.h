//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclepositionalsoundplayer.h
//
// Description: Base class for moving vehicle sounds.  This includes traffic
//              vehicles and positional idle on the user vehicle
//
// History:     3/7/2003 + Created -- Esan
//
//=============================================================================

#ifndef VEHICLEPOSITIONALSOUNDPLAYER_H
#define VEHICLEPOSITIONALSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <sound/positionalsoundplayer.h>

//========================================
// Forward References
//========================================
class Vehicle;

//=============================================================================
//
// Synopsis:    VehiclePositionalSoundPlayer
//
//=============================================================================

class VehiclePositionalSoundPlayer : public PositionCarrier
{
    public:
        VehiclePositionalSoundPlayer( );
        virtual ~VehiclePositionalSoundPlayer();

        bool IsActive() { return( m_vehicle != NULL ); }
        void ActivateByName( const char* soundName, Vehicle* theCar );
        virtual void Activate( positionalSoundSettings* soundSettings,
                               const char* resourceName,
                               Vehicle* theCar );
        virtual void Deactivate();

        bool UsesVehicle( Vehicle* car );

        virtual void ServiceOncePerFrame();

        void TiePitchToVelocity( bool flag ) { m_tiePitchToVelocity = flag; }

        void BlowUp();

        //
        // PositionCarrier functions
        //
        void GetPosition( radSoundVector& position );
        void GetVelocity( radSoundVector& velocity );

    protected:
        Vehicle* m_vehicle;

        PositionalSoundPlayer m_player;

        bool m_tiePitchToVelocity;


    private:
        //Prevent wasteful constructor creation.
        VehiclePositionalSoundPlayer( const VehiclePositionalSoundPlayer& vehiclepositionalsoundplayer );
        VehiclePositionalSoundPlayer& operator=( const VehiclePositionalSoundPlayer& vehiclepositionalsoundplayer );

        PositionalSoundPlayer m_explosionPlayer;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //VEHICLEPOSITIONALSOUNDPLAYER_H
