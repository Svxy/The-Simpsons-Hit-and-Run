//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundtuner.cpp
//
// Subsystem:   Dark Angel - Sound Tuner System
//
// Description: Implementation of the sound tuner
//
// Revisions:
//  + Created October 4, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>
#include <radnamespace.hpp>

#include <radsound.hpp>
#include <radsound_hal.hpp>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundresourcemanager.h>
#include <sound/soundrenderer/playermanager.h>
#include <sound/soundrenderer/soundtuner.h>

#include <sound/soundmanager.h>

#include <memory/srrmemory.h>

//=============================================================================
// Static Variables (outside namespace)
//=============================================================================

short Sound::daSoundTuner::s_groupWirings[NUM_SOUND_GROUPS];

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Class Implementations
//=============================================================================

//=============================================================================
// daSoundTuner_ActiveFadeInfo Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundTuner_ActiveFadeInfo::daSoundTuner_ActiveFadeInfo
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundTuner_ActiveFadeInfo::daSoundTuner_ActiveFadeInfo
(
    Fader*                          pFader,
    bool                            fadingIn,
    IDaSoundFadeState*              pDoneCallback,
    void*                           pCallbackUserData,
    DuckVolumeSet*                  initialVolumes,
    DuckVolumeSet*                  targetVolumes
)
    :
    m_pFader( pFader ),
    m_FadingIn( fadingIn ),
    m_pDoneCallback( pDoneCallback ),
    m_pCallbackUserData( pCallbackUserData )
{
    // Reference count some items
    rAssert( m_pFader != NULL );

    m_pFader->AddRef( );
    if( m_pDoneCallback != NULL )
    {
        m_pDoneCallback->AddRef( );
    }

    // Invoke the fader
    m_pFader->Fade( m_FadingIn, initialVolumes, targetVolumes );
}


//=============================================================================
// Function:    daSoundTuner_ActiveFadeInfo::~daSoundTuner_ActiveFadeInfo
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundTuner_ActiveFadeInfo::~daSoundTuner_ActiveFadeInfo( )
{
    // Release our objects
    rAssert( m_pFader != NULL );

    m_pFader->Stop();
    m_pFader->Release( );
    if( m_pDoneCallback != NULL )
    {
        m_pDoneCallback->Release( );
    }
}

//=============================================================================
// Function:    daSoundTuner_ActiveFadeInfo::ProcessFader
//=============================================================================
// Description: Process a fader's state progress
//
// Returns: true if we're done with the fader and are ready for destruction,
//          false otherwise
//
//-----------------------------------------------------------------------------

bool daSoundTuner_ActiveFadeInfo::ProcessFader()
{
    // Is the fader done fading yet?
    bool doneFading = false;
    switch( m_pFader->GetState( ) )
    {
    case Fader::FadedIn:
    case Fader::FadedOut:
        {
            // Must be done (what ever it was)
            doneFading = true;
            break;
        }
    case Fader::FadingIn:
        {
            // If we're supposed to be fading out, we must be done
            if( !m_FadingIn )
            {
                doneFading = true;
            }
            break;
        }
    case Fader::FadingOut:
        {
            // If we're supposed to be fading in, we must be done
            if( m_FadingIn )
            {
                doneFading = true;
            }
            break;
        }
    default:
        rAssert( 0 );
        break;
    }

    // If done, disconnect it, destroy it, and call its callback
    if( doneFading )
    {
        // Remember the callback
        IDaSoundFadeState* pCallback = m_pDoneCallback;
        void* pUserData = m_pCallbackUserData;
        if( pCallback != NULL )
        {
            pCallback->AddRef( );
        }

        // Call the callback
        if( pCallback != NULL )
        {
            pCallback->OnFadeDone( pUserData );
            pCallback->Release( );
        }
    }

    return( doneFading );
}

void daSoundTuner_ActiveFadeInfo::StoreCurrentVolumes( DuckVolumeSet& volumeSet )
{
    unsigned int i;

    rAssert( m_pFader != NULL );

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        volumeSet.duckVolume[i] = m_pFader->GetCurrentVolume( static_cast<DuckVolumes>(i) );
    }
}

void daSoundTuner_ActiveFadeInfo::StoreTargetSettings( DuckVolumeSet& volumeSet )
{
    unsigned int i;

    rAssert( m_pFader != NULL );

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        volumeSet.duckVolume[i] = m_pFader->GetTargetSettings( static_cast<DuckVolumes>(i) );
    }
}

//=============================================================================
// daSoundTuner Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundTuner::daSoundTuner
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundTuner::daSoundTuner( )
    :
    radRefCount( 0 ),
    m_pDuckFade( NULL ),
    m_MasterVolume( 1.0f ),
    m_activeFadeInfo( NULL ),
    m_NISTrim( 1.0f )
{
    daSoundPlayerManager* playerMgr;
    int i, j;

    //
    // The tuner makes use of several fader objects.  These objects may be
    // customized by scripts, so they must be added to the sound namespace.
    //

    // Duck fader
    playerMgr = daSoundRenderingManagerGet()->GetPlayerManager();
    rAssert( playerMgr != NULL );

    m_pDuckFade = new( GetThisAllocator() ) Fader( NULL, DUCK_FULL_FADE, *playerMgr, *this );
    rAssert( m_pDuckFade != NULL );

    for( i = 0; i < NUM_DUCK_SITUATIONS; i++ )
    {
        m_situationFaders[i] = NULL;
    }

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        m_userVolumes.duckVolume[i] = 1.0f;
        m_finalDuckLevels.duckVolume[i] = 1.0f;
    }

    for( i = 0; i < NUM_DUCK_SITUATIONS; i++ )
    {
        for( j = 0; j < NUM_DUCK_VOLUMES; j++ )
        {
            m_duckLevels[i].duckVolume[j] = 1.0f;
        }
    }

    //
    // Initialize the sound group wirings (IMPORTANT: this assumes that
    // daSoundTuner is a singleton, we only want to do this once)
    //
    for( i = 0; i < NUM_SOUND_GROUPS; i++ )
    {
        s_groupWirings[i] = ( 1 << i ) | ( 1 << MASTER );
    }
}

//=============================================================================
// Function:    daSoundTuner::~daSoundTuner
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundTuner::~daSoundTuner( )
{
    int i;

    // Release our duck faders
    if( m_pDuckFade != NULL )
    {
        m_pDuckFade->Release( );
        m_pDuckFade = NULL;
    }

    for( i = 0; i < NUM_DUCK_SITUATIONS; i++ )
    {
        if( m_situationFaders[i] != NULL )
        {
            m_situationFaders[i]->Release( );
            m_situationFaders[i] = NULL;
        }
    }

    if( m_activeFadeInfo != NULL )
    {
        delete m_activeFadeInfo;
    }
}

//=============================================================================
// Function:    daSoundTuner::Initialize
//=============================================================================
// Description: Initialize the sound tuner.  This can only be done once
//              resources have been locked down.
//
// Parameters:  outputMode - the output mode desired
//
// Returns:     n/a
//
// Notes:       This class does not support re-initialization
//
//-----------------------------------------------------------------------------

void daSoundTuner::Initialize( void )
{
    // Make sure resources are locked down
    //rAssert( Sound::daSoundRenderingManagerGet( )->GetResourceManager( )->GetResourceLockdown( ) );

    // Generate each of the wiring groups
    Sound::daSoundTunerWireSystem( this );
}

//=============================================================================
// daSoundTuner::PostScriptLoadInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void daSoundTuner::PostScriptLoadInitialize()
{
    IRadNameSpace* nameSpace;
    globalSettings* settingsObj;
    daSoundPlayerManager* playerMgr;
    int i;

    playerMgr = daSoundRenderingManagerGet()->GetPlayerManager();
    rAssert( playerMgr != NULL );

    //
    // Get the globalSettings object for Fader use
    //
    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );

    settingsObj = reinterpret_cast<globalSettings*>( nameSpace->GetInstance( "tuner" ) );
    rAssert( settingsObj != NULL );

    for( i = 0; i < NUM_DUCK_SITUATIONS; i++ )
    {
        m_situationFaders[i] = new( GetThisAllocator() ) Fader( settingsObj,
                                                                static_cast<DuckSituations>(i),
                                                                *playerMgr,
                                                                *this );
        rAssert( m_situationFaders[i] != NULL );
    }

#ifdef SOUND_DEBUG_INFO_ENABLED
    m_debugPage.LazyInitialization( 4, GetSoundManager()->GetDebugDisplay() );
#endif
}

//=============================================================================
// Function:    daSoundTuner::ServiceOncePerFrame
//=============================================================================
// Description: Service the sound tuner.  This should be done once per frame.
//
// Parameters:  none
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundTuner::ServiceOncePerFrame( unsigned int elapsedTime )
{
    bool faderDone;

    //
    // Process the faders
    //
    Fader::UpdateAllFaders( elapsedTime );

    //
    // Process the fade info stuff that monitors the faders (hmmm, this is
    // looking like a pretty lightweight class these days.  Candidate for
    // removal?)
    //
    if( m_activeFadeInfo != NULL )
    {
        faderDone = m_activeFadeInfo->daSoundTuner_ActiveFadeInfo::ProcessFader();

        if( faderDone )
        {
            delete m_activeFadeInfo;
            m_activeFadeInfo = NULL;
        }
    }

    serviceDebugInfo();
}

//=============================================================================
// Function:    daSoundTuner::SetSoundOutputMode
//=============================================================================
// Description: Set the sound system output mode (stereo, mono, surround)
//
// Parameters:  outputMode - the output mode desired
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundTuner::SetSoundOutputMode
(
    IDaSoundTuner::SoundOutputMode outputMode
)
{
    radSoundOutputMode rsdOutputMode = radSoundOutputMode_Stereo;
    if( outputMode == MONO )
    {
        rsdOutputMode = radSoundOutputMode_Mono;
    }
    else if( outputMode == STEREO )
    {
        rsdOutputMode = radSoundOutputMode_Stereo;
    }
    else if( outputMode == SURROUND )
    {
        rsdOutputMode = radSoundOutputMode_Surround;
    }
    else
    {
        rAssertMsg( 0, "Invalid sound output mode" );
    }

    ::radSoundHalSystemGet( )->SetOutputMode( rsdOutputMode );
}

//=============================================================================
// Function:    daSoundTuner::GetSoundOutputMode
//=============================================================================
// Description: Get the sound system output mode (stereo, mono, surround)
//
// Parameters:  none
//
// Returns:     Returns the current sound output mode
//
//-----------------------------------------------------------------------------

IDaSoundTuner::SoundOutputMode daSoundTuner::GetSoundOutputMode
(
    void
)
{
    IDaSoundTuner::SoundOutputMode outputMode = STEREO;
    radSoundOutputMode rsdOutputMode =
        ::radSoundHalSystemGet( )->GetOutputMode( );
    if( rsdOutputMode == radSoundOutputMode_Mono )
    {
        outputMode = MONO;
    }
    else if( rsdOutputMode == radSoundOutputMode_Stereo )
    {
        outputMode = STEREO;
    }
    else if( rsdOutputMode == radSoundOutputMode_Surround )
    {
        outputMode = SURROUND;
    }
    else
    {
        rAssertMsg( 0, "Unrecognized sound output mode" );
    }

    return outputMode;
}

//=============================================================================
// Function:    daSoundTuner::ActivateDuck
//=============================================================================
// Description: Start/stop ducking of sounds
//
// Parameters:  pObject - the object to receive fade state events
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundTuner::ActivateDuck
(
    IDaSoundFadeState* pObject,
    void* pUserData,
    bool fadeIn
)
{
    activateDuckInternal( pObject, pUserData, fadeIn, m_pDuckFade );
}

//=============================================================================
// Function:    daSoundTuner::StartSituationalDuck
//=============================================================================
// Description: Start ducking of sounds
//
// Parameters:  pObject - the object to receive fade state events
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundTuner::ActivateSituationalDuck( IDaSoundFadeState* pObject,
                                           DuckSituations situation,
                                           void* pUserData,
                                           bool fadeIn )
{
    activateDuckInternal( pObject, pUserData, fadeIn, m_situationFaders[situation] );
}

//=============================================================================
// daSoundTuner::ResetDuck
//=============================================================================
// Description: Stop all ducking
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void daSoundTuner::ResetDuck()
{
    unsigned int i, j;

    //
    // Return all the duck values to max, then do a fade in
    //
    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        for( j = 0; j < NUM_DUCK_SITUATIONS; j++ )
        {
            m_duckLevels[j].duckVolume[i] = 1.0f;
        }
    }

    activateDuckInternal( NULL, NULL, true, m_pDuckFade );
}

//=============================================================================
// Function:    daSoundTuner::SetMasterVolume
//=============================================================================
// Description: Set the master volume
//
//-----------------------------------------------------------------------------

void daSoundTuner::SetMasterVolume
(
    daTrimValue volume
)
{
    m_MasterVolume = volume;

    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( MASTER, m_MasterVolume );
}

//=============================================================================
// Function:    daSoundTuner::GetMasterVolume
//=============================================================================
// Description: Get the master volume
//
//-----------------------------------------------------------------------------

daTrimValue daSoundTuner::GetMasterVolume( void )
{
    return( m_MasterVolume );
}

//=============================================================================
// Function:    daSoundTuner::SetDialogueVolume
//=============================================================================
// Description: Set the dialogue volume
//
//-----------------------------------------------------------------------------

void daSoundTuner::SetDialogueVolume
(
    daTrimValue volume
)
{
    m_userVolumes.duckVolume[DUCK_DIALOG] = volume;

    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( DIALOGUE, volume );
}

//=============================================================================
// Function:    daSoundTuner::GetDialogueVolume
//=============================================================================
// Description: Get the dialogue volume
//
//-----------------------------------------------------------------------------

daTrimValue daSoundTuner::GetDialogueVolume( void )
{
    return( m_userVolumes.duckVolume[DUCK_DIALOG] );
}

//=============================================================================
// Function:    daSoundTuner::SetMusicVolume
//=============================================================================
// Description: Set the music volume
//
//-----------------------------------------------------------------------------

void daSoundTuner::SetMusicVolume
(
    daTrimValue volume
)
{
    m_userVolumes.duckVolume[DUCK_MUSIC] = volume;

    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( MUSIC, volume );
}

//=============================================================================
// Function:    daSoundTuner::GetMusicVolume
//=============================================================================
// Description: Get the music volume
//
//-----------------------------------------------------------------------------

daTrimValue daSoundTuner::GetMusicVolume( void )
{
    return( m_userVolumes.duckVolume[DUCK_MUSIC] );
}

//=============================================================================
// Function:    daSoundTuner::SetAmbienceVolume
//=============================================================================
// Description: Set the music volume
//
//-----------------------------------------------------------------------------

void daSoundTuner::SetAmbienceVolume
(
    daTrimValue volume
)
{
    m_userVolumes.duckVolume[DUCK_AMBIENCE] = volume;

    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( AMBIENCE, volume );
}

//=============================================================================
// Function:    daSoundTuner::GetAmbienceVolume
//=============================================================================
// Description: Get the music volume
//
//-----------------------------------------------------------------------------

daTrimValue daSoundTuner::GetAmbienceVolume( void )
{
    return( m_userVolumes.duckVolume[DUCK_AMBIENCE] );
}

//=============================================================================
// Function:    daSoundTuner::SetSfxVolume
//=============================================================================
// Description: Set the sound effects volume
//
//-----------------------------------------------------------------------------

void daSoundTuner::SetSfxVolume
(
    daTrimValue volume
)
{
    m_userVolumes.duckVolume[DUCK_SFX] = volume;

    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( SOUND_EFFECTS, volume );
}

//=============================================================================
// Function:    daSoundTuner::GetSfxVolume
//=============================================================================
// Description: Get the sound effects volume
//
//-----------------------------------------------------------------------------

daTrimValue daSoundTuner::GetSfxVolume( void )
{
    return( m_userVolumes.duckVolume[DUCK_SFX] );
}

//=============================================================================
// Function:    daSoundTuner::SetCarVolume
//=============================================================================
// Description: Set the sound effects volume
//
//-----------------------------------------------------------------------------

void daSoundTuner::SetCarVolume( daTrimValue volume )
{
    m_userVolumes.duckVolume[DUCK_CAR] = volume;

    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( CARSOUND, volume );
}

//=============================================================================
// Function:    daSoundTuner::GetCarVolume
//=============================================================================
// Description: Get the sound effects volume
//
//-----------------------------------------------------------------------------

daTrimValue daSoundTuner::GetCarVolume( void )
{
    return( m_userVolumes.duckVolume[DUCK_CAR] );
}

//=============================================================================
// Function:    daSoundTuner::FadeSounds
//=============================================================================
// Description: Fade a particular group of sounds
//
// Parameters:  pKnob - the knob to fade
//              pObject - the fade state change object
//              pUserData - user data for the state change callback
//              pFader - a pointer to an actual fader to use
//              fadeIn - true if we are fading in
//
//-----------------------------------------------------------------------------

void daSoundTuner::FadeSounds
(
    IDaSoundFadeState* pObject,
    void* pUserData,
    Fader* pFader,
    bool fadeIn,
    DuckVolumeSet* initialVolumes
)
{
    unsigned int i;
    DuckVolumeSet currentVolumes;
    DuckVolumeSet* initVolumePtr = initialVolumes;
    DuckVolumeSet targetVolumes;

    rAssert( m_activeFadeInfo == NULL );
    rAssert( pFader != NULL );
    if( pFader == NULL )
    {
        return;
    }

    HeapMgr()->PushHeap( static_cast<GameMemoryAllocator>(GetThisAllocator()) );

    //
    // Store the intended targets for this fader.
    //
    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        if( fadeIn )
        {
            m_duckLevels[pFader->GetSituation()].duckVolume[i] = 1.0f;
        }
        else
        {
            m_duckLevels[pFader->GetSituation()].duckVolume[i] = 
                pFader->GetTargetSettings( static_cast<Sound::DuckVolumes>(i) );
        }
    }

    if( initVolumePtr == NULL )
    {
        //
        // Initial volumes aren't supplied, so use the current settings 
        // as we've recorded them here
        //
        initVolumePtr = &currentVolumes;

        for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
        {
            currentVolumes.duckVolume[i] = m_userVolumes.duckVolume[i];
        }
    }

    calculateDuckedVolumes( targetVolumes );

    // Create the temporary fade object.  It will destroy itself when done
    m_activeFadeInfo = new daSoundTuner_ActiveFadeInfo
    (
        pFader,
        fadeIn,
        pObject,
        pUserData,
        initialVolumes,
        &targetVolumes
    );

    HeapMgr()->PopHeap( static_cast<GameMemoryAllocator>( GetThisAllocator() ) );
}


//=============================================================================
// Function:    daSoundTuner::WireKnobToPathHelper
//=============================================================================
// Description: Helper function to wire a knob to a path for a resource.
//              This function is automattically called on every
//              sound resource.  If the path matches that of a file
//              in the resource, the resource's sound group is automattically
//              wired to the sound group specified by the user data.
//
// Notes:       Trying to put a resource in more than one group will not work!
//              If a resource contains files from more than one fundamental
//              path and its wiring must reflect this, then a new
//              allocation stratagy should be used.
//
//-----------------------------------------------------------------------------

void daSoundTuner::WireKnobToPathHelper
(
    IDaSoundResource* pRes,
    void* pUserData
)
{
    char filenameBuffer[256];

    // Get the wiring info
    WirePathInfo* pInfo = (WirePathInfo*)pUserData;

    unsigned int i = 0;
    for( i = 0; i < pRes->GetNumFiles( ); i++ )
    {
        pRes->GetFileNameAt( i, filenameBuffer, 256 );
        unsigned int j = 0;
        bool match = true;
        for( j = 0; j < pInfo->m_PathLen; j++ )
        {
            char a = filenameBuffer[j];
            char b = pInfo->m_Path[j];
            
            if( a == '/' )
            {
                a = '\\';
            }
            if( b == '/' )
            {
                b = '\\';
            }
            if( a != b )
            {
                // Notice that this supports the case when the length
                // of the filename is less than the test path because,
                // in that case, the '\0' character won't match
                match = false;
                break;
            }
        }
        if( match )
        {
            pRes->SetSoundGroup( pInfo->m_SoundGroup );
            break;
        }
    }
}

//=============================================================================
// Function:    daSoundTuner::WirePath
//=============================================================================
// Description: Wire a path to a particular sound group
//
//-----------------------------------------------------------------------------

void daSoundTuner::WirePath
(
    daSoundGroup soundGroup,
    const char* path
)
{
    // Find all resource that are in this path and relate them to this knob
    WirePathInfo wirePathInfo;
    wirePathInfo.m_Path = path;
    wirePathInfo.m_PathLen = strlen( wirePathInfo.m_Path );
    wirePathInfo.m_SoundGroup = soundGroup;
    
    unsigned int numResources =
        daSoundResourceManager::GetInstance( )->GetNumResourceDatas( );
    
    for( unsigned int r = 0; r < numResources; r ++ )
    {
        WireKnobToPathHelper(
            daSoundResourceManager::GetInstance( )->GetResourceDataAt( r ),
            & wirePathInfo );
    }
}

//=============================================================================
// daSoundTuner::WireGroup
//=============================================================================
// Description: Mark a sound group as changing in sync with the master group
//
// Parameters:  slaveGroup - slave that changes with master group
//              masterGroup - controller group
//
// Return: void
//
//=============================================================================
void daSoundTuner::WireGroup( daSoundGroup slaveGroup, daSoundGroup masterGroup )
{
    unsigned int i;

    for( i = 0; i < NUM_SOUND_GROUPS; i++ )
    {
        if( s_groupWirings[i] & ( 1 << slaveGroup ) )
        {
            s_groupWirings[i] |= 1 << masterGroup;
        }
    }
}

//=============================================================================
// daSoundTuner::IsSlaveGroup
//=============================================================================
// Description: Indicates whether one group is slaved to another
//
// Parameters:  slave - proposed slave sound group
//              master - proposed master sound group
//
// Return:      True if slave is affected by master, false otherwise
//
//=============================================================================
bool daSoundTuner::IsSlaveGroup( daSoundGroup slave, daSoundGroup master )
{
    if( s_groupWirings[slave] & ( 1 << master ) )
    {
        return( true );
    }

    return( false );
}

//=============================================================================
// daSoundTuner::GetGroupTrim
//=============================================================================
// Description: Get the trim associated with a particular sound group.
//              Actually, our "group" is currently one of four settings,
//              even though we break it down more in daSoundGroup for
//              future expansion.
//
// Parameters:  group - group that we want trim for
//
// Return:      trim value for that group, or 1.0f (max) if it doesn't fit
//
//=============================================================================
daTrimValue daSoundTuner::GetGroupTrim( daSoundGroup group )
{
    if( IsSlaveGroup( group, SOUND_EFFECTS ) )
    {
        return( m_userVolumes.duckVolume[DUCK_SFX] );
    }
    else if( IsSlaveGroup( group, MUSIC ) )
    {
        return( m_userVolumes.duckVolume[DUCK_MUSIC] );
    }
    else if( IsSlaveGroup( group, DIALOGUE ) )
    {
        return( m_userVolumes.duckVolume[DUCK_DIALOG] );
    }
    else if( IsSlaveGroup( group, AMBIENCE ) )
    {
        return( m_userVolumes.duckVolume[DUCK_AMBIENCE] );
    }
    else if( IsSlaveGroup( group, CARSOUND ) )
    {
        return( m_userVolumes.duckVolume[DUCK_CAR] );
    }
    else if( IsSlaveGroup( group, OPTIONS_MENU_STINGERS ) )
    {
        //
        // Special group for options menu, not affected by ducking
        //
        return( 1.0f );
    }
    else
    {
        //
        // None of the above.  We shouldn't get here
        //
        rAssert( false );
        return( 1.0f );
    }
}

//=============================================================================
// daSoundTuner::SetFaderGroupTrim
//=============================================================================
// Description: Comment
//
// Parameters:  ( Sound::DuckVolumes group, daTrimValue trim )
//
// Return:      void 
//
//=============================================================================
void daSoundTuner::SetFaderGroupTrim( Sound::DuckVolumes group, daTrimValue trim )
{
    m_finalDuckLevels.duckVolume[group] = trim;
}

//=============================================================================
// daSoundTuner::GetFaderGroupTrim
//=============================================================================
// Description: Get the fader trim associated with a particular sound group.
//              Actually, our "group" is currently one of four settings,
//              even though we break it down more in daSoundGroup for
//              future expansion.
//
// Parameters:  group - group that we want trim for
//
// Return:      trim value for that group, or 1.0f (max) if it doesn't fit
//
//=============================================================================
daTrimValue daSoundTuner::GetFaderGroupTrim( daSoundGroup group )
{
    if( IsSlaveGroup( group, SOUND_EFFECTS ) )
    {
        return( m_finalDuckLevels.duckVolume[DUCK_SFX] );
    }
    else if( IsSlaveGroup( group, MUSIC ) )
    {
        return( m_finalDuckLevels.duckVolume[DUCK_MUSIC] );
    }
    else if( IsSlaveGroup( group, DIALOGUE ) )
    {
        return( m_finalDuckLevels.duckVolume[DUCK_DIALOG] );
    }
    else if( IsSlaveGroup( group, AMBIENCE ) )
    {
        return( m_finalDuckLevels.duckVolume[DUCK_AMBIENCE] );
    }
    else if( IsSlaveGroup( group, CARSOUND ) )
    {
        return( m_finalDuckLevels.duckVolume[DUCK_CAR] );
    }
    else if( IsSlaveGroup( group, OPTIONS_MENU_STINGERS ) )
    {
        //
        // Special group for options menu, not affected by ducking
        //
        return( 1.0f );
    }
    else
    {
        //
        // None of the above.  We shouldn't get here
        //
        rAssert( false );
        return( 1.0f );
    }
}

void daSoundTuner::MuteNIS()
{
    m_NISTrim = GetGroupTrim( NIS );
    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( NIS, 0.0f );
}

void daSoundTuner::UnmuteNIS()
{
    daSoundRenderingManagerGet()->GetPlayerManager()->PlayerVolumeChange( NIS, m_NISTrim );
}

//=============================================================================
// Private functions
//=============================================================================

void daSoundTuner::activateDuckInternal( IDaSoundFadeState* pObject,
                                         void* pUserData,
                                         bool fadeOut,
                                         Fader* faderObj )
{
    DuckVolumeSet currentVolumes;

#ifndef RAD_RELEASE
    //
    // Hack for fader tuning.  The tuners usually only read the fader
    // settings at startup, but we want them to pick up changes in radTuner
    // on the fly.  We'll make it refresh on each duck attempt in debug
    // and tune builds.
    //
    refreshFaderSettings();
#endif

    if( m_activeFadeInfo != NULL )
    {
        //
        // Get the current fader's settings and throw it on the stack
        //
        m_activeFadeInfo->StoreCurrentVolumes( currentVolumes );

        //
        // Now we can get rid of the old fader
        //
        delete m_activeFadeInfo;
        m_activeFadeInfo = NULL;
    }
    else
    {
        calculateDuckedVolumes( currentVolumes );
    }

    // Use our duck fader...
    FadeSounds( pObject,
                pUserData,
                faderObj,
                fadeOut,
                &currentVolumes );
}

//=============================================================================
// daSoundTuner::calculateDuckedVolumes
//=============================================================================
// Description: Comment
//
// Parameters:  ( DuckVolumeSet& volumes )
//
// Return:      void 
//
//=============================================================================
void daSoundTuner::calculateDuckedVolumes( DuckVolumeSet& volumes )
{
    unsigned int i, j;

    //
    // Calculate target volumes
    //
    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        volumes.duckVolume[i] = 1.0f;
    }

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        for( j = 0; j < NUM_DUCK_SITUATIONS; j++ )
        {
            if( m_duckLevels[j].duckVolume[i] < volumes.duckVolume[i] )
            {
                volumes.duckVolume[i] = m_duckLevels[j].duckVolume[i];
            }
        }
    }
}

//=============================================================================
// daSoundTuner::refreshFaderSettings
//=============================================================================
// Description: Reset the duck settings for each fader
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void daSoundTuner::refreshFaderSettings()
{
    IRadNameSpace* nameSpace;
    globalSettings* settingsObj;
    unsigned int i;

    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );

    settingsObj = reinterpret_cast<globalSettings*>( nameSpace->GetInstance( "tuner" ) );
    rAssert( settingsObj != NULL );

    for( i = 0; i < NUM_DUCK_SITUATIONS; i++ )
    {
        m_situationFaders[i]->ReinitializeFader( settingsObj );
    }
}

//=============================================================================
// daSoundTuner::serviceDebugInfo
//=============================================================================
// Description: Send some debug stuff to be dumped on the screen
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void daSoundTuner::serviceDebugInfo()
{
#ifdef SOUND_DEBUG_INFO_ENABLED
    unsigned int i;
    unsigned int j;

    for( i = 0; i < NUM_DUCK_SITUATIONS; i++ )
    {
        for( j = 0; j < NUM_DUCK_VOLUMES; j++ )
        {
            m_debugPage.SetDuckLevel( static_cast<Sound::DuckSituations>(i),
                                      static_cast<Sound::DuckVolumes>(j), 
                                      m_duckLevels[i].duckVolume[j] );
        }
    }

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        m_debugPage.SetFinalDuckLevel( static_cast<Sound::DuckVolumes>(i), m_finalDuckLevels.duckVolume[i] );
    }

    for( i = 0; i < NUM_DUCK_VOLUMES; i++ )
    {
        m_debugPage.SetUserVolume( static_cast<Sound::DuckVolumes>(i), m_userVolumes.duckVolume[i] );
    }
#endif
}

//=============================================================================
// Factory functions
//=============================================================================

//=============================================================================
// Function:    daSoundTunerCreate
//=============================================================================
// Description: Create the tuner
//
//-----------------------------------------------------------------------------

void daSoundTunerCreate
(
    IDaSoundTuner** ppTuner,
    radMemoryAllocator allocator
)
{
    rAssert( ppTuner != NULL );
    (*ppTuner) = new ( allocator ) daSoundTuner( );
    (*ppTuner)->AddRef( );
}

} // Sound Namespace
