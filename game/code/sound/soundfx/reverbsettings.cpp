//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        reverbsettings.cpp
//
// Description: Definition for the reverbSettings class, which stores sets
//              of reverb parameters to be applied whenever we want that
//              reverby sound thing happening.  NOTE: lower-case "r" needed
//              to make RadScript happy.
//
// History:     11/5/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/soundfx/reverbsettings.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Initialially the list is empty
//
reverbSettings* radLinkedClass< reverbSettings >::s_pLinkedClassHead = NULL;
reverbSettings* radLinkedClass< reverbSettings >::s_pLinkedClassTail = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// reverbSettings::reverbSettings
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
reverbSettings::reverbSettings() :
    radRefCount( 0 ),

    m_gain( 0.0f ),
    m_fadeInTime( 0.0f ),
    m_fadeOutTime( 0.0f ),

    m_xboxRoom( 0 ),
    m_xboxRoomHF( 0 ),
    m_xboxRoomRolloff( 0.0f ),
    m_xboxDecay( 0.0f ),
    m_xboxDecayHFRatio( 0.0f ),
    m_xboxReflections( 0 ),
    m_xboxReflectionsDelay( 0.0f ),
    m_xboxReverb( 0 ),
    m_xboxReverbDelay( 0.0f ),
    m_xboxDiffusion( 0.0f ),
    m_xboxDensity( 0.0f ),
    m_xboxHFReference( 0.0f ),

    m_ps2ReverbMode( 0 ),
    m_ps2Delay( 0.0f ),
    m_ps2Feedback( 0.0f ),

    m_gcPreDelay( 0.0f ),
    m_gcReverbTime( 0.0f ),
    m_gcColoration( 0.0f ),
    m_gcDamping( 0.0f ),

    m_winEnvironmentDiffusion( 1.0f ),
    m_winAirAbsorptionHF( -5.0f )
{
}

//==============================================================================
// reverbSettings::~reverbSettings
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
reverbSettings::~reverbSettings()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//******************************************************************************
// Factory functions
//******************************************************************************

//==============================================================================
// ReverbSettingsObjCreate
//==============================================================================
// Description: Factory function for creating reverbSettings objects.
//              Called by RadScript.
//
// Parameters:	ppSettings - Address of ptr to new object
//              allocator - FTT pool to allocate object within
//
// Return:      N/A.
//
//==============================================================================
void ReverbSettingsObjCreate
(
    IReverbSettings** ppSettings,
    radMemoryAllocator allocator
)
{
    rAssert( ppSettings != NULL );
    (*ppSettings) = new ( allocator ) reverbSettings( );
    (*ppSettings)->AddRef( );
}

