//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        xboxreverbcontroller.cpp
//
// Description: Implementation for the XboxReverbController class, which provides
//              the Xbox-specific reverb control
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radsound_xbox.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/xboxreverbcontroller.h>

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
// XboxReverbController::XboxReverbController
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
XboxReverbController::XboxReverbController()
{
    m_reverbInterface = ::radSoundHalEffectI3DL2ReverbXBoxCreate( GMA_PERSISTENT );

    registerReverbEffect( m_reverbInterface );
}

//==============================================================================
// XboxReverbController::~XboxReverbController
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
XboxReverbController::~XboxReverbController()
{
    m_reverbInterface->Release();
    m_reverbInterface = NULL;
}

//=============================================================================
// XboxReverbController::SetReverbOn
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void XboxReverbController::SetReverbOn( reverbSettings* settings )
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
        m_reverbInterface->SetDiffusion( settings->GetXboxDiffusion() );
        m_reverbInterface->SetDensity( settings->GetXboxDensity() );
        m_reverbInterface->SetHFReference( settings->GetXboxHFReference() );

        prepareFadeSettings( settings->GetGain(), settings->GetFadeInTime(),
                             settings->GetFadeOutTime() );
    }
}

//=============================================================================
// XboxReverbController::SetReverbOff
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void XboxReverbController::SetReverbOff()
{
    startFadeOut();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
