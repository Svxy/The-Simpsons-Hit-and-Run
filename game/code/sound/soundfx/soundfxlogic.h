//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfxlogic.h
//
// Description: Declaration for the SoundFXLogic class, which is an abstract
//              base class for objects that translate events into sound effects
//              in the different game states
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDFXLOGIC_H
#define SOUNDFXLOGIC_H

//========================================
// Nested Includes
//========================================
#include <events/eventlistener.h>
#include <sound/simpsonssoundplayer.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    SFXPlayer
//
// Structure that maps the sound player to a flag indicating whether
// we can kill the sound effect for something higher priority
//
//=============================================================================
struct SFXPlayer
{
    SimpsonsSoundPlayer soundPlayer;
    bool isKillable;
};

//=============================================================================
//
// Synopsis:    SoundFXLogic
//
//=============================================================================

class SoundFXLogic : public EventListener,
                     public SimpsonsSoundPlayerCallback
{
    public:
        SoundFXLogic();
        virtual ~SoundFXLogic();

        virtual void RegisterEventListeners() = 0;
        void UnregisterEventListeners();

        SFXPlayer* GetAvailableSFXPlayer( unsigned int* index = NULL );

        virtual void ServiceOncePerFrame( unsigned int elapsedTime );

        virtual void Cleanup();

        //
        // SimpsonsSoundPlayerCallback functions
        //
        void OnSoundReady();
        virtual void OnPlaybackComplete();

    protected:
        // Number of SFXPlayers
        static const unsigned int s_numSFXPlayers = 6;

        //
        // Sound players
        //
        SFXPlayer m_soundPlayers[s_numSFXPlayers];

        bool playSFXSound( const char* resource, bool killable, bool useCallback = false,
                           unsigned int* index = NULL, float trim = 1.0f, float pitch = 1.0f );

        void playCreditLine( int lineNumber );

    private:
        //Prevent wasteful constructor creation.
        SoundFXLogic( const SoundFXLogic& original );
        SoundFXLogic& operator=( const SoundFXLogic& rhs );
};


#endif // SOUNDFXLOGIC_H

