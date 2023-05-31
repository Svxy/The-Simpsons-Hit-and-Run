//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gcreverbcontroller.cpp
//
// Description: Implementation for the GCReverbController class, which provides
//              the GC-specific reverb control
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radsound_gcn.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/gcreverbcontroller.h>



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
// GCReverbController::GCReverbController
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GCReverbController::GCReverbController()
{
    m_reverbInterface = ::radSoundEffectStdReverbGcnCreate( GMA_PERSISTENT );

    registerReverbEffect( m_reverbInterface );
}

//==============================================================================
// GCReverbController::~GCReverbController
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GCReverbController::~GCReverbController()
{
    m_reverbInterface->Release();
    m_reverbInterface = NULL;
}

//=============================================================================
// GCReverbController::SetReverbOn
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void GCReverbController::SetReverbOn( reverbSettings* settings )
{
    SetReverbGain( settings->GetGain() );
    m_reverbInterface->SetPreDelay( settings->GetGCPreDelay() );
    m_reverbInterface->SetReverbTime( settings->GetGCReverbTime() );
    m_reverbInterface->SetColoration( settings->GetGCColoration() );
    m_reverbInterface->SetDamping( settings->GetGCDamping() );
}

//=============================================================================
// GCReverbController::SetReverbOff
//=============================================================================
// Description: Self-explanatory
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void GCReverbController::SetReverbOff()
{
    SetReverbGain( 0.0f );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
