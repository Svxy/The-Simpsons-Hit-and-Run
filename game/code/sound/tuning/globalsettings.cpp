//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        globalsettings.cpp
//
// Description: Implementation of globalSettings, which sets global sound values
//              in the game (e.g. master volume, sound defaults).  Created
//              using RadScript, hence the lower-case g.
//
// History:     07/08/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/tuning/globalsettings.h>

#include <sound/soundmanager.h>
#include <memory/srrmemory.h>

using namespace Sound;

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Initialially the list is empty
//
globalSettings* radLinkedClass< globalSettings >::s_pLinkedClassHead = NULL;
globalSettings* radLinkedClass< globalSettings >::s_pLinkedClassTail = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// globalSettings::globalSettings
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
globalSettings::globalSettings() :
    radRefCount( 0 ),
    m_peeloutMin( 0.0f ),
    m_peeloutMax( 1.0f ),
    m_peeloutMaxTrim( 1.0f ),
    m_roadSkidClip( NULL ),
    m_dirtSkidClip( NULL ),
    m_roadFootstepClip( NULL ),
    m_metalFootstepClip( NULL ),
    m_woodFootstepClip( NULL ),
    m_coinPitchCount( 0 ),
    m_ambienceVolume( 0.0f ),
    m_musicVolume( 0.0f ),
    m_sfxVolume( 0.0f ),
    m_dialogueVolume( 0.0f ),
    m_carVolume( 0.0f )
{
    unsigned int i, j;

    for( i = 0; i < NUM_DUCK_SITUATIONS; i++ )
    {
        for( j = 0; j < NUM_DUCK_VOLUMES; j++ )
        {
            m_duckVolumes[i].duckVolume[j] = 0.0f;
        }
    }

    for( i = 0; i < s_maxCoinPitches; i++ )
    {
        m_coinPitches[i] = 1.0f;
    }
}

//==============================================================================
// globalSettings::~globalSettings
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
globalSettings::~globalSettings()
{
    if( m_roadSkidClip != NULL )
    {
        delete( GMA_AUDIO_PERSISTENT, m_roadSkidClip );
    }
    if( m_dirtSkidClip != NULL )
    {
        delete( GMA_AUDIO_PERSISTENT, m_dirtSkidClip );
    }

    if( m_roadFootstepClip != NULL )
    {
        delete( GMA_AUDIO_PERSISTENT, m_roadFootstepClip );
    }
    if( m_metalFootstepClip != NULL )
    {
        delete( GMA_AUDIO_PERSISTENT, m_metalFootstepClip );
    }
    if( m_woodFootstepClip != NULL )
    {
        delete( GMA_AUDIO_PERSISTENT, m_woodFootstepClip );
    }
}

//=============================================================================
// globalSettings::SetMasterVolume
//=============================================================================
// Description: Sets master volume, obviously
//
// Parameters:  volume - new volume level
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetMasterVolume( float volume )
{
    GetSoundManager()->SetMasterVolume( volume );
}

//=============================================================================
// globalSettings::SetSfxVolume
//=============================================================================
// Description: Sets sfx volume, obviously
//
// Parameters:  volume - new volume level
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetSfxVolume( float volume )
{
    m_sfxVolume = volume;
}

//=============================================================================
// globalSettings::SetCarVolume
//=============================================================================
// Description: Sets car volume, obviously
//
// Parameters:  volume - new volume level
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetCarVolume( float volume )
{
    m_carVolume = volume;
}

//=============================================================================
// globalSettings::SetMusicVolume
//=============================================================================
// Description: Sets music volume, obviously
//
// Parameters:  volume - new volume level
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetMusicVolume( float volume )
{
    m_musicVolume = volume;
}

//=============================================================================
// globalSettings::SetDialogueVolume
//=============================================================================
// Description: Sets dialogue volume, obviously
//
// Parameters:  volume - new volume level
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetDialogueVolume( float volume )
{
    m_dialogueVolume = volume;
}

//=============================================================================
// globalSettings::SetAmbienceVolume
//=============================================================================
// Description: Sets ambience volume, obviously
//
// Parameters:  volume - new volume level
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetAmbienceVolume( float volume )
{
    m_ambienceVolume = volume;
}

//=============================================================================
// globalSettings::SetPeeloutMin
//=============================================================================
// Description: Set minimum peelout value at which we play sound
//
// Parameters:  min - peelout value, 0-1
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetPeeloutMin( float min )
{
    rAssert( min >= 0.0f );
    rAssert( min <= 1.0f );

    m_peeloutMin = min;
}

//=============================================================================
// globalSettings::SetPeeloutMax
//=============================================================================
// Description: Set peelout value at which sound is at maximum volume
//
// Parameters:  max - peelout value, 0-1
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetPeeloutMax( float max )
{
    rAssert( max >= 0.0f );
    rAssert( max <= 1.0f );

    m_peeloutMax = max;
}

//=============================================================================
// globalSettings::SetPeeloutMaxTrim
//=============================================================================
// Description: Set maximum trim applied to peelout sound
//
// Parameters:  trim - max trim value
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetPeeloutMaxTrim( float trim )
{
    rAssert( trim >= 0.0f );

    m_peeloutMaxTrim = trim;
}

//=============================================================================
// globalSettings::SetSkidRoadClipName
//=============================================================================
// Description: Set name of sound resource for road skids
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetSkidRoadClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    m_roadSkidClip = new char[strlen(clipName)+1];
    strcpy( m_roadSkidClip, clipName );

    HeapMgr()->PopHeap(GMA_AUDIO_PERSISTENT);
}

//=============================================================================
// globalSettings::SetSkidDirtClipName
//=============================================================================
// Description: Set name of sound resource for dirt skids
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetSkidDirtClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    m_dirtSkidClip = new char[strlen(clipName)+1];
    strcpy( m_dirtSkidClip, clipName );

    HeapMgr()->PopHeap(GMA_AUDIO_PERSISTENT);
}

//=============================================================================
// globalSettings::SetFootstepRoadClipName
//=============================================================================
// Description: Set name of sound resource for footsteps on road surfaces
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetFootstepRoadClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    m_roadFootstepClip = new char[strlen(clipName)+1];
    strcpy( m_roadFootstepClip, clipName );

    HeapMgr()->PopHeap(GMA_AUDIO_PERSISTENT);
}

//=============================================================================
// globalSettings::SetFootstepMetalClipName
//=============================================================================
// Description: Set name of sound resource for footsteps on metal surfaces
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetFootstepMetalClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    m_metalFootstepClip = new char[strlen(clipName)+1];
    strcpy( m_metalFootstepClip, clipName );

    HeapMgr()->PopHeap(GMA_AUDIO_PERSISTENT);
}

//=============================================================================
// globalSettings::SetFootstepWoodClipName
//=============================================================================
// Description: Set name of sound resource for footsteps on wood surfaces
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetFootstepWoodClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    m_woodFootstepClip = new char[strlen(clipName)+1];
    strcpy( m_woodFootstepClip, clipName );

    HeapMgr()->PopHeap(GMA_AUDIO_PERSISTENT);
}

//=============================================================================
// globalSettings::SetCoinPitch
//=============================================================================
// Description: Set pitch for coin pickup sound in sequence (so we can make
//              a tune out of the coin sounds or something)
//
// Parameters:  pitch - pitch for next coin playback in sequence
//
// Return:      void 
//
//=============================================================================
void globalSettings::SetCoinPitch( float pitch )
{
    if( m_coinPitchCount < s_maxCoinPitches )
    {
        m_coinPitches[m_coinPitchCount++] = pitch;
    }
    else
    {
        rDebugString( "Too many coin pitches specified in script\n" );
    }
}

//=============================================================================
// globalSettings::GetCoinPitch
//=============================================================================
// Description: Get a particular pitch within coin sequence
//
// Parameters:  index - index into coin pitch sequence
//
// Return:      the pitch 
//
//=============================================================================
float globalSettings::GetCoinPitch( unsigned int index )
{
    rAssert( index < s_maxCoinPitches );

    return( m_coinPitches[index] );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// globalSettings::setDuckVolume
//=============================================================================
// Description: Store the ducking volume for a particular volume setting and
//              ducking situation
//
// Parameters:  situation - ducking situation
//              volumeToSet - which volume to set within that situation
//              volume - volume value to set
//
// Return:      void 
//
//=============================================================================
void globalSettings::setDuckVolume( DuckSituations situation, DuckVolumes volumeToSet, float volume )
{
    m_duckVolumes[situation].duckVolume[volumeToSet] = volume;
}

//******************************************************************************
// Factory functions
//******************************************************************************

//==============================================================================
// GlobalSettingsObjCreate
//==============================================================================
// Description: Factory function for creating globalSettings objects.
//              Called by RadScript.
//
// Parameters:	ppParametersObj - Address of ptr to new object
//              allocator - FTT pool to allocate object within
//
// Return:      N/A.
//
//==============================================================================
void GlobalSettingsObjCreate
(
    IGlobalSettings** ppParametersObj,
    radMemoryAllocator allocator
)
{
    rAssert( ppParametersObj != NULL );
    (*ppParametersObj) = new ( allocator ) globalSettings( );
    (*ppParametersObj)->AddRef( );
}

