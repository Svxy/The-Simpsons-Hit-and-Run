//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        nissoundplayer.h
//
// Description: Declare NISSoundPlayer, which interacts with the dialog
//              system to provide NIS sounds.
//
// History:     10/5/2002 + Created -- Darren
//
//=============================================================================

#ifndef NISSOUNDPLAYER_H
#define NISSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#include <sound/nisenum.h>
#include <sound/positionalsoundplayer.h>

#include <events/eventlistener.h>

//========================================
// Forward References
//========================================
struct NISSoundLoadedCallback;
struct NISSoundPlaybackCompleteCallback;

//=============================================================================
//
// Synopsis:    NISSoundPlayer
//
//=============================================================================

class NISPlayerGroup : public SimpsonsSoundPlayerCallback
{
    public:
        NISPlayerGroup();
        virtual ~NISPlayerGroup();

        void LoadSound( radKey32 soundID, NISSoundLoadedCallback* callback );
        void PlaySound( rmt::Box3D* box, NISSoundPlaybackCompleteCallback* callback );
        void StopAndDumpSound();
        void Pause() { m_player.Pause(); }
        void Continue();

        bool IsSoundIDLoaded( radKey32 soundID );
        bool IsFree() { return( m_soundID == 0 ); }

        //
        // SimpsonsSoundPlayerCallback interface functions
        //
        void OnSoundReady();
        void OnPlaybackComplete();

    protected:
    private:
        //Prevent wasteful constructor creation.
        NISPlayerGroup( const NISPlayerGroup& original );
        NISPlayerGroup& operator=( const NISPlayerGroup& rhs );

        PositionalSoundPlayer m_player;

        radKey32 m_soundID;

        NISSoundLoadedCallback* m_loadCallback;
        NISSoundPlaybackCompleteCallback* m_playCallback;

        bool m_soundQueued;
};

//=============================================================================
//
// Synopsis:    NISSoundPlayer
//
//=============================================================================

class NISSoundPlayer : public EventListener
{
    public:
        NISSoundPlayer();
        virtual ~NISSoundPlayer();

        void LoadNISSound( radKey32 NISSoundID, NISSoundLoadedCallback* callback );
        void PlayNISSound( radKey32 NISSoundID, rmt::Box3D* box, NISSoundPlaybackCompleteCallback* callback );
        void StopAndDumpNISSound( radKey32 NISSoundID );

        void PauseAllNISPlayers();
        void ContinueAllNISPlayers();

        //
        // EventListener functions
        //
        void HandleEvent( EventEnum id, void* pEventData );

    private:
        //Prevent wasteful constructor creation.
        NISSoundPlayer( const NISSoundPlayer& original );
        NISSoundPlayer& operator=( const NISSoundPlayer& rhs );

        void loadFEGag( radKey32 gagKey );

        static const unsigned int NUM_NIS_PLAYERS = 6;

        NISPlayerGroup m_NISPlayers[NUM_NIS_PLAYERS];

        radKey32 m_currentFEGag;
};


#endif // NISSOUNDPLAYER_H

