//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fader.h
//
// Description: Declaration for Fader class which brings down volume on
//              associated multi-input knob.  Replaces IRadSoundFade
//              objects from radsound/util.
//
// History:     13/08/2002 + Created -- Darren
//
//=============================================================================

#ifndef FADER_H
#define FADER_H

//========================================
// Nested Includes
//========================================
#include <radobject.hpp>

#include <sound/soundrenderer/dasoundgroup.h>
#include <sound/tuning/globalsettings.h>

//========================================
// Forward References
//========================================

struct FaderStateChangeCallback;

namespace Sound
{
    class daSoundPlayerManager;
    struct IDaSoundTuner;
}

//=============================================================================
//
// Synopsis:    Fader
//
//=============================================================================

class Fader : public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "Fader" );

        Fader( globalSettings* duckSettings,
               Sound::DuckSituations situation,
               Sound::daSoundPlayerManager& playerMgr,
               Sound::IDaSoundTuner& tuner );
        ~Fader();

        void Update( unsigned int elapsed );
        static void UpdateAllFaders( unsigned int elapsedTime );

        void Stop();
        
        void SetTime( unsigned int milliseconds );
        unsigned int GetTime( void );

        void Fade( bool in, Sound::DuckVolumeSet* initialVolumes = NULL, Sound::DuckVolumeSet* targetVolumes = NULL );

        float GetCurrentVolume( Sound::DuckVolumes volumeKnob ) { return( m_currentVolumes.duckVolume[volumeKnob] ); }
        float GetTargetSettings( Sound::DuckVolumes volumeKnob ) { return( m_globalDuckSettings.duckVolume[volumeKnob] ); }

        enum State { FadedIn, FadedOut, FadingIn, FadingOut };

        void RegisterStateCallback( FaderStateChangeCallback* callback );
        void UnRegisterStateCallback( FaderStateChangeCallback* callback );

        State GetState( void );

        void BroadCast( void );

        Sound::DuckSituations GetSituation( void ) { return( m_duckSituation ); }

        void ReinitializeFader( globalSettings* settingObj );

    private:
        //Prevent wasteful constructor creation.
        Fader();
        Fader( const Fader& original );
        Fader& operator=( const Fader& rhs );

        void setState();
        
        //
        // Add and remove fader from the update list.  We should only update
        // the fader if it's not on its target value yet.
        //
        void addToUpdateList();
        void removeFromUpdateList();
        bool faderInUpdateList();

        unsigned int m_Time;
        bool         m_In;
        State        m_State;

        Sound::DuckVolumeSet m_currentVolumes;
        float m_stepValues[Sound::NUM_DUCK_VOLUMES];
        Sound::DuckVolumeSet m_targetVolumes;
        Sound::DuckVolumeSet m_globalDuckSettings;

        Sound::DuckSituations m_duckSituation;

        //
        // Callback object for state change notification.  Was a list of objects
        // in radSoundFade
        //
        FaderStateChangeCallback* m_callback;

        //
        // Static fader list, used for updates
        //
        static Fader* s_faderUpdateList;

        //
        // Pointer used to hold place in update list
        //
        Fader* m_nextUpdatableFader;

        //
        // Player manager, used to actually do the fading
        //
        Sound::daSoundPlayerManager& m_playerManager;

        //
        // Tuner, stores the results
        //
        Sound::IDaSoundTuner& m_tuner;
};

//=============================================================================
//
// Synopsis:    FaderStateChangeCallback
//
//=============================================================================
struct FaderStateChangeCallback
{
    virtual void OnStateChange( Fader::State newState );
};


#endif // FADER_H

