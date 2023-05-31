//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        simpsonssoundplayer.h
//
// Description: Declaration for SimpsonsSoundPlayer class, which interacts with
//              the sound renderer to play sounds, and tracks the player
//              resources in use.
//
// History:     29/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef SIMPSONSSOUNDPLAYER_H
#define SIMPSONSSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <sound/soundrenderer/soundsystem.h>

//========================================
// Forward References
//========================================
namespace Sound
{
    class daSoundResourceManager;
    class daSoundClipStreamPlayer;
    class daSoundPlayerManager;
}

struct IDaSoundResource;
class SoundLoader;
class SoundRenderingPlayerCallback;
struct IRadSoundHalPositionalGroup;

//=============================================================================
//
// Synopsis:    SimpsonsSoundPlayerCallback
//
//=============================================================================

struct SimpsonsSoundPlayerCallback
{
    virtual void OnSoundReady() = 0;
    virtual void OnPlaybackComplete() = 0;
};

//=============================================================================
//
// Synopsis:    SimpsonsSoundPlayer
//
//=============================================================================

class SimpsonsSoundPlayer
{
    public:
        SimpsonsSoundPlayer();
        virtual ~SimpsonsSoundPlayer();

        bool PlaySound( const char* resourceName, SimpsonsSoundPlayerCallback* callback = NULL );
                                                 
        bool PlaySound( Sound::daResourceKey resourceKey, SimpsonsSoundPlayerCallback* callback = NULL );                   
                        
        virtual bool PlayResource( IDaSoundResource* resource,
                                   SimpsonsSoundPlayerCallback* callback = NULL );
        
        bool QueueSound( const char* resourceName,
                         SimpsonsSoundPlayerCallback* callback = NULL )
            { return( QueueSound( ::radMakeKey32( resourceName ), callback ) ); }
            
        bool QueueSound( radKey32 resourceKey,
                         SimpsonsSoundPlayerCallback* callback = NULL );
                         
        bool QueueSound( IDaSoundResource* resource,
                         SimpsonsSoundPlayerCallback* callback = NULL );
                         
        virtual void PlayQueuedSound( SimpsonsSoundPlayerCallback* callback = NULL );

        void Stop();
        void Pause();
        void Continue();
        bool IsPaused();

        void OnPlaybackComplete();
        
        bool IsInUse() { return( m_playa != NULL ); }
        
        void SetPitch( float pitch );
        void SetTrim( float trim );

    protected:        

        //
        // Sound renderer's player object
        //
        Sound::daSoundClipStreamPlayer* m_playa;

        //
        // Call when we're done with the sound renderer player object
        //
        virtual void dumpSoundPlayer();

    protected:
        
        enum Type { Type_Positional, Type_NonPositional } m_Type;
           
    private:
        //Prevent wasteful constructor creation.
        SimpsonsSoundPlayer( const SimpsonsSoundPlayer& original );
        SimpsonsSoundPlayer& operator=( const SimpsonsSoundPlayer& rhs );

        //
        // Sound renderer resource manager
        //
        static Sound::daSoundResourceManager* s_resourceManager;
        static Sound::daSoundPlayerManager* s_playerManager;
        static SoundLoader* s_soundLoader;

        //
        // Statistics on players in use
        //
        static unsigned int s_playersCreated;
        static unsigned int s_clipPlayersInUse;
        static unsigned int s_streamPlayersInUse;

        //
        // Callback object for playback completion
        //
        SoundRenderingPlayerCallback* m_callback;

};


#endif // SIMPSONSSOUNDPLAYER_H

