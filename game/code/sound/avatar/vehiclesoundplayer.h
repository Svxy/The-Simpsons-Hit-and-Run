//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclesoundplayer.h
//
// Description: Declaration of the VehicleSoundPlayer class, which plays sounds
//              for user-controlled vehicles in game.
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef VEHICLESOUNDPLAYER_H
#define VEHICLESOUNDPLAYER_H

//========================================
// Nested Includes
//========================================

#include <sound/simpsonssoundplayer.h>
#include <sound/avatar/vehiclesounddebugpage.h>
#include <render/IntersectManager/IntersectManager.h>

//========================================
// Forward References
//========================================

class Vehicle;
class carSoundParameters;
class globalSettings;
class VehicleSoundPlayer;
class EngineState;

//
// Enumerated list of temporary engine states
//
enum EngineStateEnum
{
    ENGINE_STATE_NORMAL,
    ENGINE_STATE_UPSHIFTING,
    ENGINE_STATE_DOWNSHIFTING,
    ENGINE_STATE_IN_AIR,
    ENGINE_STATE_REVERSE,
    ENGINE_STATE_IDLING,
    ENGINE_STATE_SKIDDING,

    NUM_ENGINE_STATES,

    ENGINE_STATE_INVALID
};

//=============================================================================
//
// Synopsis:    VehicleSoundPlayer
//
//=============================================================================

class VehicleSoundPlayer : public SimpsonsSoundPlayerCallback
{
    public:
        VehicleSoundPlayer();
        virtual ~VehicleSoundPlayer();

        //
        // Update routines
        //
        void UpdateOncePerFrame( unsigned int elapsedTime );

        void UpdateSoundParameters( unsigned int elapsedTime );
        
        //
        // Check the vehicle to see if we should be playing a skid noise
        //
        void CheckForSkid( unsigned int elapsedTime );

        //
        // Check the vehicle controller to see if we should be playing
        // a horn noise
        //
        void CheckHorn();

        //
        // Start and stop sound when the player gets in and out of the car
        //
        void StartCarSounds( Vehicle* newVehicle );
        void StopCarSounds();

        //
        // Play door sounds
        //
        void PlayDoorOpen();
        void PlayDoorClose();

        //
        // Called when gearshift is done playing
        //
        void OnPlaybackComplete();

        //
        // Needed to complete SimpsonsSoundPlayerCallback interface
        //
        void OnSoundReady();

        //
        // Proximity testing for AI vehicle dialog (better here than
        // anywhere else)
        //
        void AddAIVehicleProximityTest( Vehicle* aiVehicle );
        void DeleteAIVehicleProximityTest( Vehicle* aiVehicle );

    private:
        //Prevent wasteful constructor creation.
        VehicleSoundPlayer( const VehicleSoundPlayer& original );
        VehicleSoundPlayer& operator=( const VehicleSoundPlayer& rhs );

        bool carSoundIsActive();

        void checkDamage();
        void checkProximity();

        const char* getSkidResourceForTerrain( eTerrainType terrain );

        enum CarSoundPlayers
        {
            CARPLAYER_ENGINE,
            CARPLAYER_SHIFT,
            CARPLAYER_SKID,
            CARPLAYER_HORNPLAYER,
            CARPLAYER_DAMAGE,
            CARPLAYER_OVERLAY,
            CARPLAYER_BACKUP_BEEP,
            CARPLAYER_DOOR,

            CARPLAYER_NUMPLAYERS
        };

#ifdef SOUND_DEBUG_INFO_ENABLED
        VehicleSoundDebugPage m_debugInfo;
#endif

        SimpsonsSoundPlayer m_soundPlayers[CARPLAYER_NUMPLAYERS];
        EngineState* m_engineStates[NUM_ENGINE_STATES];

        Vehicle* m_vehicle;
        carSoundParameters* m_parameters;
        globalSettings* m_peeloutSettings;

        bool m_isSkidding;
        bool m_hornPlaying;
        bool m_oneTimeHorn;
        float m_powerslideTrim;

        bool m_playingDamage;

        Vehicle* m_proximityAIVehicle;

        eTerrainType m_terrainType;
};


#endif // VEHICLESOUNDPLAYER_H

