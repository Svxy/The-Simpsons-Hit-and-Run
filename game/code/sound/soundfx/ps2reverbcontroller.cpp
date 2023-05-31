//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps2reverbcontroller.cpp
//
// Description: Implementation for the PS2ReverbController class, which provides
//              the PS2-specific reverb control
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radsound_ps2.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/ps2reverbcontroller.h>

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
// PS2ReverbController::PS2ReverbController
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PS2ReverbController::PS2ReverbController()
{
    m_reverbInterface = ::radSoundCreateEffectPs2( GMA_PERSISTENT );

    //
    // Wheee!!!!
    //
    m_reverbInterface->SetMode( IRadSoundEffectPs2::Off );

    registerReverbEffect( m_reverbInterface );
}

//==============================================================================
// PS2ReverbController::~PS2ReverbController
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PS2ReverbController::~PS2ReverbController()
{
    m_reverbInterface->Release();
    m_reverbInterface = NULL;
}

//=============================================================================
// PS2ReverbController::SetReverbOn
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void PS2ReverbController::SetReverbOn( reverbSettings* settings )
{
    if( settings != NULL )
    {
        m_reverbInterface->SetMode( static_cast<IRadSoundEffectPs2::Mode>(settings->GetPS2ReverbMode()) );
        m_reverbInterface->SetGain( settings->GetGain() );
        m_reverbInterface->SetDelay( settings->GetPS2Delay() );
        m_reverbInterface->SetFeedback( settings->GetPS2Feedback() );

        prepareFadeSettings( settings->GetGain(), settings->GetFadeInTime(),
                             settings->GetFadeOutTime() );
    }
    else
    {
        rReleaseString( "Settings is NULL\n" );
    }
}

//=============================================================================
// PS2ReverbController::SetReverbOff
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void PS2ReverbController::SetReverbOff()
{
    startFadeOut();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
