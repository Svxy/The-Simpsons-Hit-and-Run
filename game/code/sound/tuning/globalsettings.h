//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        globalsettings.h
//
// Description: Declaration of globalSettings, which sets global sound values
//              in the game (e.g. master volume, sound defaults).  Created
//              using RadScript, hence the lower-case g.
//
// History:     07/08/2002 + Created -- Darren
//
//=============================================================================

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

//========================================
// Nested Includes
//========================================
#include <radlinkedclass.hpp>

#include <sound/tuning/iglobalsettings.h>
#include <sound/soundrenderer/dasoundgroup.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    globalSettings
//
//=============================================================================

class globalSettings : public IGlobalSettings,
                       public radLinkedClass< globalSettings >,
                       public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "globalSettings" );

        globalSettings();
        virtual ~globalSettings();

        //
        // Volume controls
        //
        void SetMasterVolume( float volume );

        void SetSfxVolume( float volume );
        float GetSfxVolume() { return( m_sfxVolume ); }

        void SetCarVolume( float volume );
        float GetCarVolume() { return( m_carVolume ); }

        void SetMusicVolume( float volume );
        float GetMusicVolume() { return( m_musicVolume ); }

        void SetDialogueVolume( float volume );
        float GetDialogueVolume() { return( m_dialogueVolume ); }

        void SetAmbienceVolume( float volume );
        float GetAmbienceVolume() { return( m_ambienceVolume ); }
        
        //
        // Ducking controls
        //
        float GetDuckVolume( Sound::DuckSituations situation, Sound::DuckVolumes volume ) { return( m_duckVolumes[situation].duckVolume[volume] ); }

        void SetPauseSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_PAUSE, Sound::DUCK_SFX, volume ); }
        void SetPauseCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_PAUSE, Sound::DUCK_CAR, volume ); }
        void SetPauseMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_PAUSE, Sound::DUCK_MUSIC, volume ); }
        void SetPauseDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_PAUSE, Sound::DUCK_DIALOG, volume ); }
        void SetPauseAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_PAUSE, Sound::DUCK_AMBIENCE, volume ); }

        void SetMissionScreenSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MISSION, Sound::DUCK_SFX, volume ); }
        void SetMissionScreenCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MISSION, Sound::DUCK_CAR, volume ); }
        void SetMissionScreenMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MISSION, Sound::DUCK_MUSIC, volume ); }
        void SetMissionScreenDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MISSION, Sound::DUCK_DIALOG, volume ); }
        void SetMissionScreenAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MISSION, Sound::DUCK_AMBIENCE, volume ); }

        void SetLetterboxSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_LETTERBOX, Sound::DUCK_SFX, volume ); }
        void SetLetterboxCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_LETTERBOX, Sound::DUCK_CAR, volume ); }
        void SetLetterboxMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_LETTERBOX, Sound::DUCK_MUSIC, volume ); }
        void SetLetterboxDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_LETTERBOX, Sound::DUCK_DIALOG, volume ); }
        void SetLetterboxAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_LETTERBOX, Sound::DUCK_AMBIENCE, volume ); }

        void SetDialogueSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_DIALOG, Sound::DUCK_SFX, volume ); }
        void SetDialogueCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_DIALOG, Sound::DUCK_CAR, volume ); }
        void SetDialogueMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_DIALOG, Sound::DUCK_MUSIC, volume ); }
        void SetDialogueDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_DIALOG, Sound::DUCK_DIALOG, volume ); }
        void SetDialogueAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_DIALOG, Sound::DUCK_AMBIENCE, volume ); }

        void SetStoreSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_STORE, Sound::DUCK_SFX, volume ); }
        void SetStoreCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_STORE, Sound::DUCK_CAR, volume ); }
        void SetStoreMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_STORE, Sound::DUCK_MUSIC, volume ); }
        void SetStoreDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_STORE, Sound::DUCK_DIALOG, volume ); }
        void SetStoreAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_STORE, Sound::DUCK_AMBIENCE, volume ); }

        void SetOnFootSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_ONFOOT, Sound::DUCK_SFX, volume ); }
        void SetOnFootCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_ONFOOT, Sound::DUCK_CAR, volume ); }
        void SetOnFootMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_ONFOOT, Sound::DUCK_MUSIC, volume ); }
        void SetOnFootDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_ONFOOT, Sound::DUCK_DIALOG, volume ); }
        void SetOnFootAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_ONFOOT, Sound::DUCK_AMBIENCE, volume ); }

        void SetMinigameSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MINIGAME, Sound::DUCK_SFX, volume ); }
        void SetMinigameCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MINIGAME, Sound::DUCK_CAR, volume ); }
        void SetMinigameMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MINIGAME, Sound::DUCK_MUSIC, volume ); }
        void SetMinigameDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MINIGAME, Sound::DUCK_DIALOG, volume ); }
        void SetMinigameAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_MINIGAME, Sound::DUCK_AMBIENCE, volume ); }

        void SetJustMusicSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_JUST_MUSIC, Sound::DUCK_SFX, volume ); }
        void SetJustMusicCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_JUST_MUSIC, Sound::DUCK_CAR, volume ); }
        void SetJustMusicMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_JUST_MUSIC, Sound::DUCK_MUSIC, volume ); }
        void SetJustMusicDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_JUST_MUSIC, Sound::DUCK_DIALOG, volume ); }
        void SetJustMusicAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_JUST_MUSIC, Sound::DUCK_AMBIENCE, volume ); }

        void SetCreditsSfxVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_CREDITS, Sound::DUCK_SFX, volume ); }
        void SetCreditsCarVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_CREDITS, Sound::DUCK_CAR, volume ); }
        void SetCreditsMusicVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_CREDITS, Sound::DUCK_MUSIC, volume ); }
        void SetCreditsDialogueVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_CREDITS, Sound::DUCK_DIALOG, volume ); }
        void SetCreditsAmbienceVolume( float volume ) { setDuckVolume( Sound::DUCK_SIT_CREDITS, Sound::DUCK_AMBIENCE, volume ); }

        //
        // Car controls
        //
        void SetPeeloutMin( float min );
        float GetPeeloutMin() { return( m_peeloutMin ); }

        void SetPeeloutMax( float max );
        float GetPeeloutMax() { return( m_peeloutMax ); }

        void SetPeeloutMaxTrim( float trim );
        float GetPeeloutMaxTrim() { return( m_peeloutMaxTrim ); }

        void SetSkidRoadClipName( const char* clipName );
        const char* GetSkidRoadClipName() { return( m_roadSkidClip ); }

        void SetSkidDirtClipName( const char* clipName );
        const char* GetSkidDirtClipName() { return( m_dirtSkidClip ); }

        //
        // Footstep sounds
        //
        void SetFootstepRoadClipName( const char* clipName );
        const char* GetFootstepRoadClipName() { return( m_roadFootstepClip ); }

        void SetFootstepMetalClipName( const char* clipName );
        const char* GetFootstepMetalClipName() { return( m_metalFootstepClip ); }

        void SetFootstepWoodClipName( const char* clipName );
        const char* GetFootstepWoodClipName() { return( m_woodFootstepClip ); }

        //
        // Coin pitches
        //
        void SetCoinPitch( float pitch );
        float GetCoinPitch( unsigned int index );
        unsigned int GetNumCoinPitches() { return( m_coinPitchCount ); }

    private:

        //Prevent wasteful constructor creation.
        globalSettings( const globalSettings& original );
        globalSettings& operator=( const globalSettings& rhs );

        void setDuckVolume( Sound::DuckSituations situation, Sound::DuckVolumes volumeToSet, float volume );

        //
        // Ducking settings
        //
        Sound::DuckVolumeSet m_duckVolumes[Sound::NUM_DUCK_SITUATIONS];

        //
        // Car settings
        //
        float m_peeloutMin;
        float m_peeloutMax;
        float m_peeloutMaxTrim;

        char* m_roadSkidClip;
        char* m_dirtSkidClip;

        //
        // Footsteps
        //
        char* m_roadFootstepClip;
        char* m_metalFootstepClip;
        char* m_woodFootstepClip;

        //
        // Coin pitches
        //
        static const unsigned int s_maxCoinPitches = 10;
        float m_coinPitches[s_maxCoinPitches];
        unsigned int m_coinPitchCount;

        //
        // Hack!!
        //
        float m_ambienceVolume;
        float m_musicVolume;
        float m_sfxVolume;
        float m_dialogueVolume;
        float m_carVolume;
};

//=============================================================================
// Factory Functions
//=============================================================================

//
// Create a CarSoundParameters object
//
void GlobalSettingsObjCreate
(
    IGlobalSettings** ppSoundResource,
    radMemoryAllocator allocator
);



#endif // GLOBALSETTINGS_H

