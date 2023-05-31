//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        iglobalsettings.h
//
// Description: Declaration of interface class IGlobalSettings, which sets 
//              global sound values in the game (e.g. master volume, 
//              sound defaults).  Created using RadScript.
//
// History:     07/08/2002 + Created -- Darren
//
//=============================================================================

#ifndef IGLOBALSETTINGS_H
#define IGLOBALSETTINGS_H

//========================================
// Nested Includes
//========================================
#include <radobject.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    IGlobalSettings
//
//=============================================================================

class IGlobalSettings : public IRefCount
{
    public:
        //
        // Volume controls
        //
        virtual void SetMasterVolume( float volume ) = 0;

        virtual void SetSfxVolume( float volume ) = 0;
        virtual void SetCarVolume( float volume ) = 0;
        virtual void SetMusicVolume( float volume ) = 0;
        virtual void SetDialogueVolume( float volume ) = 0;
        virtual void SetAmbienceVolume( float volume ) = 0;

        //
        // Ducking controls
        //
        virtual void SetPauseSfxVolume( float volume ) = 0;
        virtual void SetPauseCarVolume( float volume ) = 0;
        virtual void SetPauseMusicVolume( float volume ) = 0;
        virtual void SetPauseDialogueVolume( float volume ) = 0;
        virtual void SetPauseAmbienceVolume( float volume ) = 0;

        virtual void SetMissionScreenSfxVolume( float volume ) = 0;
        virtual void SetMissionScreenCarVolume( float volume ) = 0;
        virtual void SetMissionScreenMusicVolume( float volume ) = 0;
        virtual void SetMissionScreenDialogueVolume( float volume ) = 0;
        virtual void SetMissionScreenAmbienceVolume( float volume ) = 0;

        virtual void SetLetterboxSfxVolume( float volume ) = 0;
        virtual void SetLetterboxCarVolume( float volume ) = 0;
        virtual void SetLetterboxMusicVolume( float volume ) = 0;
        virtual void SetLetterboxDialogueVolume( float volume ) = 0;
        virtual void SetLetterboxAmbienceVolume( float volume ) = 0;

        virtual void SetDialogueSfxVolume( float volume ) = 0;
        virtual void SetDialogueCarVolume( float volume ) = 0;
        virtual void SetDialogueMusicVolume( float volume ) = 0;
        virtual void SetDialogueDialogueVolume( float volume ) = 0;
        virtual void SetDialogueAmbienceVolume( float volume ) = 0;

        virtual void SetStoreSfxVolume( float volume ) = 0;
        virtual void SetStoreCarVolume( float volume ) = 0;
        virtual void SetStoreMusicVolume( float volume ) = 0;
        virtual void SetStoreDialogueVolume( float volume ) = 0;
        virtual void SetStoreAmbienceVolume( float volume ) = 0;

        virtual void SetOnFootSfxVolume( float volume ) = 0;
        virtual void SetOnFootCarVolume( float volume ) = 0;
        virtual void SetOnFootMusicVolume( float volume ) = 0;
        virtual void SetOnFootDialogueVolume( float volume ) = 0;
        virtual void SetOnFootAmbienceVolume( float volume ) = 0;

        virtual void SetMinigameSfxVolume( float volume ) = 0;
        virtual void SetMinigameCarVolume( float volume ) = 0;
        virtual void SetMinigameMusicVolume( float volume ) = 0;
        virtual void SetMinigameDialogueVolume( float volume ) = 0;
        virtual void SetMinigameAmbienceVolume( float volume ) = 0;

        virtual void SetJustMusicSfxVolume( float volume ) = 0;
        virtual void SetJustMusicCarVolume( float volume ) = 0;
        virtual void SetJustMusicMusicVolume( float volume ) = 0;
        virtual void SetJustMusicDialogueVolume( float volume ) = 0;
        virtual void SetJustMusicAmbienceVolume( float volume ) = 0;

        virtual void SetCreditsSfxVolume( float volume ) = 0;
        virtual void SetCreditsCarVolume( float volume ) = 0;
        virtual void SetCreditsMusicVolume( float volume ) = 0;
        virtual void SetCreditsDialogueVolume( float volume ) = 0;
        virtual void SetCreditsAmbienceVolume( float volume ) = 0;

        //
        // Car controls
        //
        virtual void SetPeeloutMin( float min ) = 0;
        virtual void SetPeeloutMax( float max ) = 0;
        virtual void SetPeeloutMaxTrim( float trim ) = 0;

        virtual void SetSkidRoadClipName( const char* clipName ) = 0;
        virtual void SetSkidDirtClipName( const char* clipName ) = 0;

        //
        // Footstep sounds
        //
        virtual void SetFootstepRoadClipName( const char* clipName ) = 0;
        virtual void SetFootstepMetalClipName( const char* clipName ) = 0;
        virtual void SetFootstepWoodClipName( const char* clipName ) = 0;

        //
        // Coin pitches
        //
        virtual void SetCoinPitch( float pitch ) = 0;
};


#endif // IGLOBALSETTINGS_H

