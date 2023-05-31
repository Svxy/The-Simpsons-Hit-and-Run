//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundmanager.h
//
// Description: Manager interface that the other game components use to
//              interact with sound.
//
// History:     01/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef _SOUNDMANAGER_H
#define _SOUNDMANAGER_H

#ifndef RAD_RELEASE
#define SOUNDDEBUG_ENABLED
#endif


//
// Debug display macros
//
#ifndef SOUNDDEBUG_ENABLED

#define SOUNDDEBUG_RENDER()

#else

#define SOUNDDEBUG_RENDER()  GetSoundManager()->DebugRender()

#endif

//========================================
// Nested Includes
//========================================

#include <enums.h>

#include <sound/soundloader.h>
#include <sound/avatar/avatarsoundplayer.h>
#include <sound/listener.h>
#include <sound/nis/nissoundplayer.h>

#include <data/gamedata.h>
#include <events/eventlistener.h>
#include <contexts/contextenum.h>

#ifdef RAD_WIN32
#include <data/config/gameconfig.h>
#endif

//========================================
// Forward References
//========================================

class MusicPlayer;
class DialogCoordinator;
class SoundDebugDisplay;
class MovingSoundManager;
class Vehicle;
class Character;
class SoundEffectPlayer;
struct IRadSoundClip;
struct IRadSoundClipPlayer;

//=============================================================================
//
// Synopsis:    NIS loading and playback completion callbacks
//
//=============================================================================

struct NISSoundLoadedCallback
{
    virtual void NISSoundLoaded() = 0;
};

struct NISSoundPlaybackCompleteCallback
{
    virtual void NISSoundPlaybackComplete() = 0;
};

enum SoundMode
{
    SOUND_MONO,
    SOUND_STEREO,
    SOUND_SURROUND
};

//=============================================================================
//
// Synopsis:    SoundManager class declaration
//
//=============================================================================

class SoundManager : public EventListener,
                     #ifdef RAD_WIN32
                     public GameConfigHandler,  //ziemek: this is ugly..doh
                     #endif
                     public GameDataHandler
{
    public:
        //
        // Singleton accessors
        //
        static SoundManager* CreateInstance( bool muteSound, bool noMusic, 
                                             bool noEffects, bool noDialogue );
        static SoundManager* GetInstance();
        static void DestroyInstance();

        //
        // EventListener interface
        //
        void HandleEvent( EventEnum id, void* pEventData );
        
        //
        // All-purpose sound file loading.  Coordinates the sound system
        // with the loading system
        //
        void LoadSoundFile( const char* filename, SoundFileHandler* callbackObj );

        //
        // Update routines.
        //
        void Update();
        // This one is for expensive stuff like positional sound calculations
        // that we can get away with doing once per frame
        void UpdateOncePerFrame( unsigned int elapsedTime, 
                                 ContextEnum context, 
                                 bool useContext = true,
                                 bool isPausedForErrors = false );
        
        // Prepare to load level sounds
        void QueueLevelSoundLoads();

        // Load the sounds associated with a car
        void LoadCarSound( Vehicle* theCar, bool unloadOtherCars );

        // Called when bootup context starts and ends
        void OnBootupStart();
        void OnBootupComplete();

        // Called when front end starts and ends
        void OnFrontEndStart();
        void OnFrontEndEnd();
        
        // Called when gameplay starts and ends
        void OnGameplayStart();
        void OnGameplayEnd( bool goingToFE );

        // Called when pause menu starts and ends (ends going back to gameplay,
        // not loading)
        void OnPauseStart();
        void OnPauseEnd();

        // Called for pseudo-pause stuff like clothing and phone booth
        //
        void OnStoreScreenStart( bool playMusic = true );
        void OnStoreScreenEnd();

        // Called when we want to kill everything but music (e.g. loading,
        // minigame standings)
        void DuckEverythingButMusicBegin( bool playMuzak = false );
        void DuckEverythingButMusicEnd( bool playMuzak = false );

        // Called when mission briefing screen starts and ends
        void OnMissionBriefingStart();
        void OnMissionBriefingEnd();

        // Called when in-game credits start
        //
        void DuckForInGameCredits();

        // Call these before and after FMVs
        void StopForMovie();
        void ResumeAfterMovie();
        bool IsStoppedForMovie();

        // Hack!  Need to mute gags during conversations
        void MuteNISPlayers();
        void UnmuteNISPlayers();

        //
        // Supersprint
        //
        void RestartSupersprintMusic();

        // Surround sound control
        void SetSoundMode( SoundMode mode );
        SoundMode GetSoundMode();

        // Function for getting that funky beat.  Values from 0.0f to 4.0f, assuming
        // that Marc doesn't write any waltzes
        float GetBeatValue();

        // Special case dialog handling
        static bool IsFoodCharacter( Character* theGuy );
        
        //
        // Volume controls.  Values range from 0.0f to 1.0f
        //
        void SetMasterVolume( float volume );
        float GetMasterVolume();
        
        void SetSfxVolume( float volume );
        float GetSfxVolume();

        void SetCarVolume( float volume );
        float GetCarVolume();
        
        void SetMusicVolume( float volume );
        float GetMusicVolume();
        
        void SetAmbienceVolume( float volume );
        float GetAmbienceVolume();
        
        void SetDialogueVolume( float volume );
        float GetDialogueVolume();

        float GetCalculatedAmbienceVolume();

        //
        // Option menu stinger stuff
        //
        void PlayCarOptionMenuStinger();
        void PlayDialogueOptionMenuStinger();
        void PlayMusicOptionMenuStinger();
        void PlaySfxOptionMenuStinger();

        //
        // Ducking stuff
        //
        void ResetDucking();

        //
        // NIS Interface
        //
        void LoadNISSound( radKey32 NISSoundID, NISSoundLoadedCallback* callback = NULL );
        void PlayNISSound( radKey32 NISSoundID, rmt::Box3D* boundingBox, NISSoundPlaybackCompleteCallback* callback = NULL );
        void StopAndDumpNISSound( radKey32 NISSoundID );

        //
        // Language interface
        //
        void SetDialogueLanguage( Scrooby::XLLanguage language );

        //
        // Sound debug functions
        //
        void DebugRender();
        
        //
        // TODO: move these functions, they're not intended for use outside
        // of the sound system
        //
        SoundLoader* GetSoundLoader() { return( m_soundLoader ); }
        SoundDebugDisplay* GetDebugDisplay() { return( m_debugDisplay ); }

        //
        // This should NOT be called outside the sound system.  Unfortunately,
        // to keep things clean, what I should do is split the MusicPlayer class
        // and move a low-level controller into the soundrenderer layer.  I don't
        // have time for this.  Things to do for the next round.
        //
        void SetMusicVolumeWithoutTuner( float volume );
        void SetAmbienceVolumeWithoutTuner( float volume );

        // Implements GameDataHandler
        //
        virtual void LoadData( const GameDataByte* dataBuffer, unsigned int numBytes );
        virtual void SaveData( GameDataByte* dataBuffer, unsigned int numBytes );
        virtual void ResetData();

        #ifdef RAD_WIN32
        // Implementation of the GameConfigHandler interface
        virtual const char* GetConfigName() const;
        virtual int GetNumProperties() const;
        virtual void LoadDefaults();
        virtual void LoadConfig( ConfigString& config );
        virtual void SaveConfig( ConfigString& config );
        #endif

        DialogCoordinator* m_dialogCoordinator;
        
    protected:
        //
        // Hide the SoundManager constructor and destructor so everyone
        // is forced to use singleton accessors
        //
        SoundManager( bool noSound, bool noMusic, bool noEffects, bool noDialogue );
        ~SoundManager();

        void initialize();

    private:
        //Prevent wasteful constructor creation.
        SoundManager( const SoundManager& original );
        SoundManager& operator=( const SoundManager& rhs );

        //
        // Hack!
        //
        void prepareStartupSounds();
        void playStartupAcceptSound();
        void playStartupScrollSound();
        void dumpStartupSounds();

        // Pointer to the one and only instance of this singleton.
        static SoundManager* spInstance;

        struct SoundSettings
        {
            float musicVolume;
            float sfxVolume;
            float carVolume;
            float dialogVolume;
            bool isSurround;
        };
        
        // Sound loading subsystem
        SoundLoader* m_soundLoader;

        // Avatar sound subsystem
        AvatarSoundPlayer m_avatarSoundPlayer;

        // Music player subsystem
        MusicPlayer* m_musicPlayer;

        // Sound effect subsystem
        SoundEffectPlayer* m_soundFXPlayer;
        
        // Dialog subsystem


        // NIS subsystem
        NISSoundPlayer* m_NISPlayer;
        
        // RadSound listener update
        Listener m_listener;

        // AI Vehicle sound subsystem
        MovingSoundManager* m_movingSoundManager;

        // Sound debug display subsystem
        SoundDebugDisplay* m_debugDisplay;
        
        // Mute options
        bool m_isMuted;

        bool m_noMusic;
        bool m_noEffects;
        bool m_noDialogue;

        // Pointer to sound rendering interface
        Sound::daSoundRenderingManager* m_pSoundRenderMgr;
        
        // [ps] avoid hammering on pause.
        bool m_stoppedForMovie;

        //
        // Hack for stinky pre-script-loading menu sounds
        //
        IRadSoundClip* m_selectSoundClip;
        IRadSoundClip* m_scrollSoundClip;

        IRadSoundClipPlayer* m_selectSoundClipPlayer;
        IRadSoundClipPlayer* m_scrollSoundClipPlayer;

        
        SoundMode m_soundMode;
};

// A little syntactic sugar for getting at this singleton.
inline SoundManager* GetSoundManager() { return( SoundManager::GetInstance() ); }

#endif //SOUNDMANAGER_H

