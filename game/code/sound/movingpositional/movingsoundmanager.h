//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        movingsoundmanager.h
//
// Description: Manages the playing of moving positional sounds
//
// History:     1/4/2003 + Created -- Esan
//
//=============================================================================

#ifndef MOVINGSOUNDMANAGER_H
#define MOVINGSOUNDMANAGER_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>

#include <events/eventlistener.h>
#include <sound/movingpositional/trafficsoundplayer.h>
#include <sound/movingpositional/aivehiclesoundplayer.h>
#include <sound/movingpositional/avatarvehicleposnplayer.h>
#include <sound/movingpositional/waspsoundplayer.h>
#include <sound/movingpositional/platformsoundplayer.h>
#include <sound/movingpositional/animobjsoundplayer.h>

//========================================
// Forward References
//========================================
class Vehicle;
class carSoundParameters;
struct AnimSoundData;
struct AnimSoundDSGData;
class AnimCollisionEntityDSG;

namespace ActionButton
{
    class AnimSwitch;
};

//=============================================================================
//
// Synopsis:    MovingSoundManager
//
//=============================================================================

class MovingSoundManager : public EventListener
{
    public:
        MovingSoundManager();
        virtual ~MovingSoundManager();

        void ServiceOncePerFrame();

        //
        // EventListener functions
        //
        void HandleEvent( EventEnum id, void* pEventData );

    private:
        //Prevent wasteful constructor creation.
        MovingSoundManager( const MovingSoundManager& original );
        MovingSoundManager& operator=( const MovingSoundManager& rhs );

        void addTrafficSound( const char* soundName, Vehicle* vehiclePtr, bool tiePitchToVelocity );
        void stopTrafficSound( Vehicle* vehiclePtr );
        void addAISound( const char* soundName, Vehicle* vehiclePtr, bool tiePitchToVelocity );
        void stopAISound( Vehicle* vehiclePtr );
        void handleTrafficHornEvent( Vehicle* vehiclePtr );
        void makeCarGoBoom( Vehicle* vehiclePtr );
        void startPlatformSound( AnimSoundData* soundData );
        void stopPlatformSound( ActionButton::AnimSwitch* soundObject );
        void stopAllPlatforms();
        void startAnimObjSound( AnimSoundDSGData* soundData );
        void stopAnimObjSound( AnimCollisionEntityDSG* soundObject );
        void startWaspSound( Actor* wasp );
        bool hasOverlayClip( Vehicle* vehiclePtr, carSoundParameters** parameters );
        void toggleOverlayClip( Vehicle* vehiclePtr );
        const char* getPositionalSettingName( Vehicle* vehiclePtr, bool isMissionVehicle );

        const static int NUM_TRAFFIC_SOUND_PLAYERS = 5;
        TrafficSoundPlayer m_trafficPlayer[NUM_TRAFFIC_SOUND_PLAYERS];

        const static int NUM_AI_SOUND_PLAYERS = 5;
        AIVehicleSoundPlayer m_aiPlayer[NUM_AI_SOUND_PLAYERS];

        AvatarVehiclePosnPlayer m_avatarVehiclePlayer;

        const static int NUM_WASP_SOUND_PLAYERS = 2;
        WaspSoundPlayer m_waspPlayer[NUM_WASP_SOUND_PLAYERS];

        const static int NUM_PLATFORM_SOUND_PLAYERS = 8;
        PlatformSoundPlayer m_platformPlayer[NUM_PLATFORM_SOUND_PLAYERS];

        const static int NUM_ANIM_OBJ_SOUND_PLAYERS = 5;
        AnimObjSoundPlayer m_animObjPlayer[NUM_ANIM_OBJ_SOUND_PLAYERS];

        static tUID s_waspUID;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //MOVINGSOUNDMANAGER_H
