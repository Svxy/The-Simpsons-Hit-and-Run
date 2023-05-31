//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundeffectplayer.h
//
// Description: Declaration for SoundEffectPlayer class, which switches between
//              the objects that direct the soundfx logic for various game
//              states
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDEFFECTPLAYER_H
#define SOUNDEFFECTPLAYER_H

//========================================
// Nested Includes
//========================================

#include <sound/simpsonssoundplayer.h>

//========================================
// Forward References
//========================================

class SoundFXLogic;
class ReverbController;

//=============================================================================
//
// Synopsis:    SoundEffectPlayer
//
//=============================================================================

class SoundEffectPlayer
{
    public:
        SoundEffectPlayer();
        virtual ~SoundEffectPlayer();

        //
        // Start playing front end sounds
        //
        void OnFrontEndStart() { setSFXState( FXSTATE_FRONTEND ); }
        
        //
        // Start playing gameplay sounds
        //
        void OnGameplayStart() { setSFXState( FXSTATE_GAMEPLAY ); }
        void OnGameplayEnd() { doCleanup(); }

        void OnPauseStart();
        void OnPauseEnd();

        void ServiceOncePerFrame( unsigned int elapsedTime );

        void PlayCarOptionStinger( float trim );
        void PlayDialogOptionStinger( float trim );
        void PlayMusicOptionStinger( float trim );
        void PlaySfxOptionStinger( float trim );

    private:
        //Prevent wasteful constructor creation.
        SoundEffectPlayer( const SoundEffectPlayer& original );
        SoundEffectPlayer& operator=( const SoundEffectPlayer& rhs );

        void initialize();

        //
        // Game states in which sound effects are played
        //
        enum SFXState
        {
            FXSTATE_FRONTEND,
            FXSTATE_GAMEPLAY,
            FXSTATE_PAUSED,

            FXSTATE_MAX_STATES,

            FXSTATE_INVALID
        };

        //
        // Set a new SFX state
        //
        void setSFXState( SFXState newState );

        //
        // Shut down anything that might still be playing
        //
        void doCleanup();

        //
        // Play a stinger (duh)
        //
        void playStinger( const char* stingerName, float trim );

        //
        // FX logic objects, one for each state
        //
        SoundFXLogic* m_logicObjects[FXSTATE_MAX_STATES];

        //
        // Reverb controller
        //
        ReverbController* m_reverbController;

        //
        // Current SFX state
        //
        SFXState m_currentState;

        //
        // Options menu stinger player
        //
        SimpsonsSoundPlayer m_stingerPlayer;
};


#endif // SOUNDEFFECTPLAYER_H

