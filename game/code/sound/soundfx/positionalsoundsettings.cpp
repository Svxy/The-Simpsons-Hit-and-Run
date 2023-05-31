//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        positionalsoundsettings.cpp
//
// Description: Implement positionalSoundSettings
//
// History:     12/20/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <string.h>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/positionalsoundsettings.h>

#include <memory/srrmemory.h>


//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//
// Initialially the list is empty
//
positionalSoundSettings* radLinkedClass< positionalSoundSettings >::s_pLinkedClassHead = NULL;
positionalSoundSettings* radLinkedClass< positionalSoundSettings >::s_pLinkedClassTail = NULL;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// positionalSoundSettings::positionalSoundSettings
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
positionalSoundSettings::positionalSoundSettings() :
    radRefCount( 0 ),
    m_clipName( NULL ),
    m_minDist( 10.0f ),
    m_maxDist( 100.0f ),
    m_playProbability( 1.0f )
{
}

//=============================================================================
// positionalSoundSettings::~positionalSoundSettings
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
positionalSoundSettings::~positionalSoundSettings()
{
    delete m_clipName;
}

//=============================================================================
// positionalSoundSettings::SetClipName
//=============================================================================
// Description: Set name of the sound resource to be played at this position
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void positionalSoundSettings::SetClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    m_clipName = new(GMA_PERSISTENT) char[strlen(clipName)+1];
    strcpy( m_clipName, clipName );
}

//=============================================================================
// positionalSoundSettings::SetMinDistance
//=============================================================================
// Description: Set distance at which volume is maxed (kinda counterintuitive,
//              but that seems to be the convention)
//
// Parameters:  min - distance to set
//
// Return:      void 
//
//=============================================================================
void positionalSoundSettings::SetMinDistance( float min )
{
    rAssert( min >= 0.0f );

    m_minDist = min;
}

//=============================================================================
// positionalSoundSettings::SetMaxDistance
//=============================================================================
// Description: Set the distance at which the volume reaches zero
//
// Parameters:  max - distance to set
//
// Return:      void 
//
//=============================================================================
void positionalSoundSettings::SetMaxDistance( float max )
{
    rAssert( max >= 0.0f );

    m_maxDist = max;
}

//=============================================================================
// positionalSoundSettings::SetPlaybackProbability
//=============================================================================
// Description: Set probability that sound will be played when we enter
//              the trigger volume
//
// Parameters:  prob - probability on scale of 0.0 to 1.0
//
// Return:      void 
//
//=============================================================================
void positionalSoundSettings::SetPlaybackProbability( float prob )
{
    rAssert( prob >= 0.0f );
    rAssert( prob <= 1.0f );

    m_playProbability = prob;
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//******************************************************************************
// Factory functions
//******************************************************************************

//==============================================================================
// PositionalSettingsObjCreate
//==============================================================================
// Description: Factory function for creating positionalSoundSettings objects.
//              Called by RadScript.
//
// Parameters:	ppParametersObj - Address of ptr to new object
//              allocator - FTT pool to allocate object within
//
// Return:      N/A.
//
//==============================================================================
void PositionalSettingsObjCreate
(
    IPositionalSoundSettings** ppParametersObj,
    radMemoryAllocator allocator
)
{
    rAssert( ppParametersObj != NULL );
    (*ppParametersObj) = new ( allocator ) positionalSoundSettings( );
    (*ppParametersObj)->AddRef( );
}
