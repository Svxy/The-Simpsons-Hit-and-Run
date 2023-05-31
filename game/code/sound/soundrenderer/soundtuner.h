//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundtuner.hpp
//
// Subsystem:   Dark Angel - Sound Tuner System
//
// Description: Description of the DA sound tuner
//
// Revisions:
//  + Created October 4, 2001 -- breimer
//
//=============================================================================

#ifndef _SOUNDTUNER_HPP
#define _SOUNDTUNER_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <raddebug.hpp>
#include <radlinkedclass.hpp>

#include <sound/soundrenderer/dasoundgroup.h>

#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/fader.h>
#include <sound/soundrenderer/tunerdebugpage.h>

//=============================================================================
// Global namespace forward declarations
//=============================================================================

struct IDaSoundResource;

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Prototypes
//=============================================================================

class daSoundTuner;

//=============================================================================
// Forward declarations
//=============================================================================

//=============================================================================
// Class Declarations
//=============================================================================

//
// A fade state information structure stores necessary information about
// an active fade.
//
class daSoundTuner_ActiveFadeInfo
{
public:

    // Constructor and destructor
    daSoundTuner_ActiveFadeInfo
    (
        Fader*                              pFader,
        bool                                pFadingIn,
        IDaSoundFadeState*                  pDoneCallback,
        void*                               pCallbackUserData,
        DuckVolumeSet*                      initialVolumes,
        DuckVolumeSet*                      targetVolumes
    );
    virtual ~daSoundTuner_ActiveFadeInfo( );

    // Process the fader
    bool ProcessFader();

    void StoreCurrentVolumes( DuckVolumeSet& volumeSet );
    void StoreTargetSettings( DuckVolumeSet& volumeSet );

    Sound::DuckSituations GetSituation() { return( m_pFader->GetSituation() ); }

private:
    // Store the raw data
    Fader*                                  m_pFader;
    bool                                    m_FadingIn;
    IDaSoundFadeState*                      m_pDoneCallback;
    void*                                   m_pCallbackUserData;
};

//
// The sound tuner.  All controls associated with the tuner have
// a scripted sister componenet available for composers to modify
// using radtuner.
//
class daSoundTuner : public IDaSoundTuner,
                     public radRefCount
{
public:
    IMPLEMENT_REFCOUNTED( "daSoundTuner" );

    //
    // Constructor and destructor
    //
    daSoundTuner( );
    virtual ~daSoundTuner( );

    //
    // IDaSoundTuner
    //
    void Initialize( void );
    void PostScriptLoadInitialize();
    void ServiceOncePerFrame( unsigned int elapsedTime );

    void SetSoundOutputMode
    (
        SoundOutputMode outputMode
    );
    SoundOutputMode GetSoundOutputMode( void );

    void ActivateDuck
    (
        IDaSoundFadeState* pObject,
        void* pUserData,
        bool fadeIn
    );

    void ActivateSituationalDuck( IDaSoundFadeState* pObject,
        DuckSituations situation,
        void* pUserData,
        bool fadeIn );

    void ResetDuck();

    void SetMasterVolume( daTrimValue volume );
    daTrimValue GetMasterVolume( void );

    void SetDialogueVolume( daTrimValue volume );
    daTrimValue GetDialogueVolume( void );

    void SetMusicVolume( daTrimValue volume );
    daTrimValue GetMusicVolume( void );

    void SetAmbienceVolume( daTrimValue volume );
    daTrimValue GetAmbienceVolume( void );

    void SetSfxVolume( daTrimValue volume );
    daTrimValue GetSfxVolume( void );

    void SetCarVolume( daTrimValue volume );
    daTrimValue GetCarVolume( void );

    daTrimValue GetGroupTrim( daSoundGroup group );
    daTrimValue GetFaderGroupTrim( daSoundGroup group );

    void MuteNIS();
    void UnmuteNIS();

    void SetFaderGroupTrim( Sound::DuckVolumes group, daTrimValue trim );

    void FadeSounds( IDaSoundFadeState* pObject,
                     void* pUserData,
                     Fader* pFader,
                     bool fadeIn,
                     DuckVolumeSet* initialVolumes = NULL );

    //
    // IDaSoundWiring
    //
    void WirePath
    (
        daSoundGroup soundGroup,
        const char* path
    );

    void WireGroup( daSoundGroup slaveGroup, daSoundGroup masterGroup );

    //
    // Sound group info
    //
    bool IsSlaveGroup( daSoundGroup slave, daSoundGroup master );

protected:
    //
    // Helper function for wiring knob paths
    //
    struct WirePathInfo
    {
        const char*                         m_Path;
        size_t                              m_PathLen;
        daSoundGroup                        m_SoundGroup;
    };
    static void WireKnobToPathHelper
    (
        IDaSoundResource* pRes,
        void* pUserData
    );

private:

    void activateDuckInternal( IDaSoundFadeState* pObject,
                               void* pUserData,
                               bool fadeOut,
                               Fader* faderObj );
    void calculateDuckedVolumes( DuckVolumeSet& volumes );

    void refreshFaderSettings();

    void serviceDebugInfo();

#ifdef SOUND_DEBUG_INFO_ENABLED
    TunerDebugPage m_debugPage;
#endif

    //
    // How many ducks to we have in progress?
    //
    DuckVolumeSet m_duckLevels[NUM_DUCK_SITUATIONS];

    DuckVolumeSet m_finalDuckLevels;

    //
    // Store our duck faders
    //
    Fader* m_pDuckFade;

    Fader* m_situationFaders[NUM_DUCK_SITUATIONS];

    //
    // Group volume settings
    //
    float m_MasterVolume;

    DuckVolumeSet m_userVolumes;

    //
    // Our static sound group wirings
    //
    static short s_groupWirings[NUM_SOUND_GROUPS];

    daSoundTuner_ActiveFadeInfo* m_activeFadeInfo;

    //
    // NIS hack
    //
    float m_NISTrim;
};

} // Sound Namespace
#endif //_SOUNDTUNER_HPP

