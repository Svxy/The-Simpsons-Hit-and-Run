//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trafficsoundplayer.h
//
// Description: Administers the playing of sound for a traffic vehicle
//
// History:     1/4/2003 + Created -- Darren
//
//=============================================================================

#ifndef TRAFFICSOUNDPLAYER_H
#define TRAFFICSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <radtime.hpp>

#include <sound/movingpositional/vehicleposnsoundplayer.h>

//========================================
// Forward References
//========================================
class Vehicle;
class carSoundParameters;

//=============================================================================
//
// Synopsis:    TrafficSoundPlayer
//
//=============================================================================

class TrafficSoundPlayer : public VehiclePositionalSoundPlayer,
                           public IRadTimerCallback
{
    public:
        TrafficSoundPlayer( );
        virtual ~TrafficSoundPlayer();

        static void InitializeClass( unsigned int numVehicles );

        void Activate( positionalSoundSettings* soundSettings,
                       const char* resourceName,
                       Vehicle* theCar );
        void Deactivate();

        void ServiceOncePerFrame();
        static void ServiceTimerList();

        void HonkHorn();
        void AddOverlayClip( carSoundParameters* parameters, const char* posnSettingsName );
        void ToggleOverlayClip( carSoundParameters* parameters, const char* posnSettingsName );

        //
        // IRadTimerCallback
        //
        void OnTimerDone( unsigned int elapsedTime, void* pUserData );

    private:
        //Prevent wasteful constructor creation.
        TrafficSoundPlayer( const TrafficSoundPlayer& trafficsoundplayer );
        TrafficSoundPlayer& operator=( const TrafficSoundPlayer& trafficsoundplayer );

        IRadTimer* m_hornTimer;
        PositionalSoundPlayer m_hornPlayer;

        carSoundParameters* m_vehicleParameters;
        PositionalSoundPlayer m_overlayPlayer;

        unsigned int m_honkCount;
        float m_pitchMultiplier;

        static IRadTimerList* s_timerList;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //TRAFFICSOUNDPLAYER_H
