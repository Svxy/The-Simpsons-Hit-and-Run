//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        win32reverbcontroller.cpp
//
// Description: Implementation for the Win32ReverbController class, which provides
//              the Windows-specific reverb control
//
// History:     03/25/2003 + Created -- Ziemek
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radsound_win32.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/win32reverbcontroller.h>

#include <sound/soundfx/reverbsettings.h>

#include <memory/srrmemory.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Win32ReverbController::Win32ReverbController
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Win32ReverbController::Win32ReverbController()
{
    m_reverbInterface = ::radSoundHalEffectEAX2ReverbCreate( GMA_PERSISTENT );
    m_reverbInterface->AddRef();

    registerReverbEffect( m_reverbInterface );
}

//==============================================================================
// Win32ReverbController::~Win32ReverbController
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Win32ReverbController::~Win32ReverbController()
{
    m_reverbInterface->Release();
    m_reverbInterface = NULL;
}

//=============================================================================
// Win32ReverbController::SetReverbOn
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void Win32ReverbController::SetReverbOn( reverbSettings* settings )
{
    if( settings != NULL )
    {
        rReleaseString( "Settings not null\n" );
        SetReverbGain( settings->GetGain() );
        m_reverbInterface->SetRoom( settings->GetXboxRoom() );
        m_reverbInterface->SetRoomHF( settings->GetXboxRoomHF() );
        m_reverbInterface->SetRoomRolloffFactor( settings->GetXboxRoomRolloffFactor() );
        m_reverbInterface->SetDecayTime( settings->GetXboxDecayTime() );
        m_reverbInterface->SetDecayHFRatio( settings->GetXboxDecayHFRatio() );
        m_reverbInterface->SetReflections( settings->GetXboxReflections() );
        m_reverbInterface->SetReflectionsDelay( settings->GetXboxReflectionsDelay() );
        m_reverbInterface->SetReverb( settings->GetXboxReverb() );
        m_reverbInterface->SetReverbDelay( settings->GetXboxReverbDelay() );
        m_reverbInterface->SetEnvironmentDiffusion( settings->GetWinEnvironmentDiffusion() );
        m_reverbInterface->SetAirAbsorptionHF( settings->GetWinAirAbsorptionHF() );

        prepareFadeSettings( settings->GetGain(), settings->GetFadeInTime(),
            settings->GetFadeOutTime() );
    }
}

//=============================================================================
// Win32ReverbController::SetReverbOff
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void Win32ReverbController::SetReverbOff()
{
    startFadeOut();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
