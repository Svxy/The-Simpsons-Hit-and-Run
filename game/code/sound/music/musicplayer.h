//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        musicplayer.h
//
// Description: Declaration of the MusicPlayer class.  Plays all music in
//              the game using RadMusic.
//
// History:     17/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>

#include <events/eventlistener.h>
#include <render/Enums/RenderEnums.h>

//========================================
// Forward References
//========================================

namespace Sound
{
    struct IDaSoundTuner;
}
namespace radmusic
{
    struct performance;
    struct composition;
}

class SoundFileHandler;
class radLoadRequest;
class Actor;

//
// Events for interactive music.
//
enum MusicEventList
{
    MEVENT_MOVIE,

    MEVENT_PAUSE,
    MEVENT_UNPAUSE,

    MEVENT_FE,

    MEVENT_LOADING_SCREEN,
    MEVENT_NEWSPAPER_SPIN,

    MEVENT_SUPERSPRINT,
    MEVENT_SUPERSPRINT_WIN,
    MEVENT_SUPERSPRINT_LOSE,

    MEVENT_LEVEL_INTRO,

    MEVENT_SUNDAY_DRIVE_START,
    MEVENT_SUNDAY_DRIVE_GET_OUT_OF_CAR,

    MEVENT_STORE,

    MEVENT_ENTER_STONECUTTER_TUNNEL,
    MEVENT_EXIT_STONECUTTER_TUNNEL,

    MEVENT_OF_EXPLORE_MISSION,
    MEVENT_OF_FOUND_CARD,
    MEVENT_OF_TIME_OUT,

    MEVENT_APU_OASIS,

    MEVENT_INTERIOR_HOUSE,
    MEVENT_INTERIOR_KWIK_E_MART,
    MEVENT_INTERIOR_SCHOOL,
    MEVENT_INTERIOR_MOES,
    MEVENT_INTERIOR_DMV,
    MEVENT_INTERIOR_ANDROID_DUNGEON,
    MEVENT_INTERIOR_OBSERVATORY,

    MEVENT_WIN_3_RACES,
    MEVENT_LEVEL_COMPLETED,
    MEVENT_DESTROY_CAMERA_BONUS,

    MEVENT_STONECUTTER_SONG,
    MEVENT_LBC_SONG,
    MEVENT_DUFF_SONG,

    MEVENT_HIT_AND_RUN_START,
    MEVENT_HIT_AND_RUN_CAUGHT,

    MEVENT_WASP_ATTACK,

    MEVENT_GATED_MISSION,

    MEVENT_SCARYMUSIC,

    MEVENT_CREDITS,

    MEVENT_END_STANDARD_EVENTS,
    
    MEVENT_MISSION_START,
    MEVENT_MISSION_DRAMA,
    MEVENT_WIN_MISSION,
    MEVENT_LOSE_MISSION,
    MEVENT_GET_OUT_OF_CAR,
    MEVENT_10_SEC_TO_GO,

    MEVENT_END_MISSION_EVENTS,

    MEVENT_STREETRACE_START,
    MEVENT_STREETRACE_WIN,
    MEVENT_STREETRACE_LOSE,
    MEVENT_STREETRACE_GET_OUT_OF_CAR,

    MEVENT_END_RACE_EVENTS,

    MEVENT_NUM_EVENTS
};

//
// Events for ambient sound script.  Not actually stored anywhere, but it gives
// me some identifiers to play with
//
enum AmbientEventList
{
    AEVENT_FRONTEND,

    AEVENT_MOVIE,
    AEVENT_PAUSE,
    AEVENT_UNPAUSE,

    AEVENT_TRIGGER_START,

    AEVENT_TRIGGER_END = 73,

    AEVENT_INTERIOR_KWIK_E_MART,
    AEVENT_INTERIOR_SCHOOL,
    AEVENT_INTERIOR_HOUSE,
    AEVENT_INTERIOR_KRUSTYLU,
    AEVENT_INTERIOR_DMV,

    AEVENT_NUM_EVENTS
};

//=============================================================================
//
// Synopsis:    MusicPlayer
//
//=============================================================================

class MusicPlayer : public EventListener
{
    public:
        MusicPlayer( Sound::IDaSoundTuner& tuner );
        virtual ~MusicPlayer();

        void Service();

        void QueueRadmusicScriptLoad();
        void QueueMusicLevelLoad( RenderEnums::LevelEnum level );
        
        //
        // Notify loading system when script file load complete
        //
        void LoadRadmusicScript( const char* filename, SoundFileHandler* fileHandler );
        void UnloadRadmusicScript();

        //
        // Look for notification when the player gets in and out of the car
        //
        void HandleEvent( EventEnum id, void* pEventData );

        //
        // Functions for notification of change in game state 
        //
        void OnFrontEndStart();
        void OnFrontEndFinish();

        void OnGameplayStart( bool playerInCar );
        void OnGameplayFinish();

        void OnPauseStart();
        void OnPauseEnd();

        void OnStoreStart();
        void OnStoreEnd();

        void StopForMovie();
        void ResumeAfterMovie();

        bool IsStoppedForMovie();

        void RestartSupersprintMusic();

        //
        // Volume controls
        //
        float GetVolume();
        void SetVolume( float volume );
        float GetAmbienceVolume();
        void SetAmbienceVolume( float volume );

        //
        // Beat values
        //
        float GetBeatValue();

    private:
        //Prevent wasteful constructor creation.
        MusicPlayer();
        MusicPlayer( const MusicPlayer& original );
        MusicPlayer& operator=( const MusicPlayer& rhs );
        
        void SetUpPerformance(
            radmusic::performance **,
            radmusic::composition **,
            const char * searchPath );
            
        void TriggerMusicEvent( MusicEventList event );
        void TriggerAmbientEvent( unsigned int event );
        void triggerMusicMissionEventByName( radKey32* key );
        
        unsigned int m_lastServiceTime;

        // Needed for polling the composition loader
        bool m_isLoadingMusic;

        // True if player currently in car, false otherwise
        bool m_isInCar;

        // Composition loader
        
        radLoadRequest* m_radLoadRequest;
        char n_currentLoadName[ 64 ]; // hack for radload memory leak.

        // Performance object
        radmusic::performance * m_musicPerformance;
        radmusic::composition * m_musicComposition;

        //
        // Ambient script stuff
        //
        bool m_isLoadingAmbient;
        
        radmusic::performance * m_ambientPerformance;
        radmusic::composition * m_ambientComposition;

        // File load completion callback object
        SoundFileHandler* m_loadCompleteCallback;

        // Current ambient sound
        unsigned int m_currentAmbient;
        bool m_ambiencePlaying;

        //
        // Special music cases
        //
        bool m_onApuRooftop;
        bool m_stoneCutterSong;
        bool m_LBCSong;

        //
        // Delayed music start
        //
        bool m_delayedMusicStart;

        //
        // Wasp that triggered music
        //
        Actor* m_wasp;

        //
        // Calculate offset for current level
        //
        int calculateLevelIndex();

        //
        // Calculate offset for current mission
        //
        int calculateMissionIndex();

        //
        // Calculate offset for interior we're about to enter
        //
        int calculateInteriorIndex( tUID interiorID );

        //
        // Returns flag indicating whether we use ambient on-foot stuff for
        // this stage
        //
        bool musicLockedOnForStage();

        //
        // Trigger the events to start music
        //
        void startMusic();

        //
        // Figure out what to do after the mission ends
        //
        void playPostMissionSounds();

        //
        // Turn ambient sound on and off
        //
        void turnAmbienceOn( unsigned int event );
        void turnAmbienceOff( unsigned int event );

        //
        // True if current mission is a race, false otherwise
        //
        bool currentMissionIsRace();

        //
        // True if we're in Sunday Drive, false otherwise
        //
        bool currentMissionIsSundayDrive();

        //
        // Construct tables for faster lookup of music events in script
        //
        void initializeTableNameKeys();
        void buildEventTables();

        //
        // Change the radMusic state
        //
        void triggerMusicStateChange( radKey32 stateKey, radKey32 stateEventKey );

        int findStandardMusicEvent( MusicEventList event );
        int findMissionEvent( MusicEventList event, int mission );
        int findRaceEvent( MusicEventList event, int race );

        void triggerCurrentInteriorAmbience();
};


#endif // MUSICPLAYER_H

