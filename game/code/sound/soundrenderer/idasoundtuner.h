//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        idasoundtuner.hpp
//
// Subsystem:   Dark Angel - Sound Tuner System
//
// Description: Description of the DA sound tuner interface
//
// Revisions:
//  + Created October 4, 2001 -- breimer
//
//=============================================================================

#ifndef _IDASOUNDTUNER_HPP
#define _IDASOUNDTUNER_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>

#include <sound/soundrenderer/dasoundgroup.h>
#include <sound/soundrenderer/soundsystem.h>

#include <sound/tuning/globalsettings.h>

//=============================================================================
// Global namespace forward declarations
//=============================================================================

struct IRadSoundMultiInputKnob;
class Fader;

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Prototypes
//=============================================================================

struct IDaSoundWiring;
struct IDaSoundTuner;
struct IDaSoundFadeState;

//=============================================================================
// Interfaces
//=============================================================================

//
// Wire together various sound groups that take the form of multi-input
// knobs.
//
struct IDaSoundWiring : public IRefCount
{
    //
    // Wire a sound group to a path
    //
    virtual void WirePath
    (
        daSoundGroup soundGroup,
        const char* path
    ) = 0;

    //
    // Initialize the sound groups
    //
    virtual void WireGroup( daSoundGroup slaveGroup, daSoundGroup masterGroup ) = 0;
};

//
// The sound tuner is responsible for managing global settings and various
// "wirings" of the sound system.  It allows the generation of tuner instances
// for controlling various game parameters, and it manages common global
// sound settings (like ducking, master volume, pause/continue/cancel,
// mono/stereo).
//
struct IDaSoundTuner : public IDaSoundWiring
{
    //
    // Initialize and perform all wiring.  This will lock down the resources
    // if they have not been locked already.
    //
    virtual void Initialize( void ) = 0;

    //
    // Create faders after we've got scripts to latch them to
    //
    virtual void PostScriptLoadInitialize() = 0;

    //
    // Service the tuner
    //
    virtual void ServiceOncePerFrame( unsigned int elapsedTime ) = 0;

    //
    // Modify the sound output mode
    //
    enum SoundOutputMode
    {
        MONO,
        STEREO,
        SURROUND
    };
    virtual void SetSoundOutputMode
    (
        SoundOutputMode outputMode
    ) = 0;
    virtual SoundOutputMode GetSoundOutputMode( void ) = 0;

    // SPECIALIZED SOUND SETTINGS FOR COMMON CONTROL

    //
    // Duck sounds
    //
    virtual void ActivateDuck( IDaSoundFadeState* pObject,
                               void* pUserData,
                               bool fadeIn ) = 0;

    virtual void ActivateSituationalDuck( IDaSoundFadeState* pObject,
                                          DuckSituations situation,
                                          void* pUserData,
                                          bool fadeIn ) = 0;
    virtual void ResetDuck() = 0;

    //
    // Common volume controls
    //
    virtual void SetMasterVolume( daTrimValue volume ) = 0;
    virtual daTrimValue GetMasterVolume( void ) = 0;

    virtual void SetDialogueVolume( daTrimValue volume ) = 0;
    virtual daTrimValue GetDialogueVolume( void ) = 0;

    virtual void SetMusicVolume( daTrimValue volume ) = 0;
    virtual daTrimValue GetMusicVolume( void ) = 0;

    virtual void SetAmbienceVolume( daTrimValue volume ) = 0;
    virtual daTrimValue GetAmbienceVolume( void ) = 0;

    virtual void SetSfxVolume( daTrimValue volume ) = 0;
    virtual daTrimValue GetSfxVolume( void ) = 0;

    virtual void SetCarVolume( daTrimValue volume ) = 0;
    virtual daTrimValue GetCarVolume( void ) = 0;

    virtual daTrimValue GetGroupTrim( daSoundGroup group ) = 0;
    virtual daTrimValue GetFaderGroupTrim( daSoundGroup group ) = 0;

    virtual void MuteNIS() = 0;
    virtual void UnmuteNIS() = 0;

    virtual void SetFaderGroupTrim( Sound::DuckVolumes group, daTrimValue trim ) = 0;

    // GENERAL SOUND SETTINGS

    //
    // Fade a sound group
    //
    virtual void FadeSounds( IDaSoundFadeState* pObject,
                             void* pUserData,
                             Fader* pFader,
                             bool fadeIn,
                             DuckVolumeSet* initialVolumes = NULL ) = 0;


    //
    // Sound group info
    //
    virtual bool IsSlaveGroup( daSoundGroup slave, daSoundGroup master ) = 0;

};

//=============================================================================
// Public functions
//=============================================================================

//
// Do the wiring of the sound system
//
void daSoundTunerWireSystem
(
    IDaSoundWiring* pWiring
);

//=============================================================================
// Factory functions
//=============================================================================

//
// Create the tuner manager
//
void daSoundTunerCreate
(
    IDaSoundTuner** ppTuner,
    radMemoryAllocator allocator
);

} // Sound Namespace
#endif //_IDASOUNDTUNER_HPP

