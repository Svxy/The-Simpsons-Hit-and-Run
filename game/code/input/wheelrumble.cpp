//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wheelrumble.cpp
//
// Description: Implement WheelRumble
//
// History:     6/25/2003 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>


//========================================
// Project Includes
//========================================
#include <input/wheelrumble.h>
#include <input/wheeldefines.h>
#include <input/inputmanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// WheelRumble::WheelRumble
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
WheelRumble::WheelRumble()
{
#ifdef RAD_WIN32
    m_diPeriodic.dwMagnitude             = 0;
    m_diPeriodic.lOffset                 = 0;
    m_diPeriodic.dwPhase                 = 0;
    m_diPeriodic.dwPeriod                = 80;

    m_diEnvelope.dwSize                  = sizeof(DIENVELOPE);
    m_diEnvelope.dwAttackLevel           = 0;
    m_diEnvelope.dwAttackTime            = (DWORD)(0.5 * DI_SECONDS);
    m_diEnvelope.dwFadeLevel             = 0;
    m_diEnvelope.dwFadeTime              = (DWORD)(1 * DI_SECONDS);

    mForceEffect.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    mForceEffect.dwDuration              = INFINITE;
    mForceEffect.dwSamplePeriod          = 0;
    mForceEffect.dwGain                  = DI_FFNOMINALMAX;
    mForceEffect.dwTriggerButton         = DIEB_NOTRIGGER;
    mForceEffect.dwTriggerRepeatInterval = 0;
    mForceEffect.cAxes                   = 1;
    mForceEffect.rgdwAxes                = m_rgdwAxes;
    mForceEffect.rglDirection            = m_rglDirection;
    mForceEffect.lpEnvelope              = &m_diEnvelope;
    mForceEffect.cbTypeSpecificParams    = sizeof(DIPERIODIC);
    mForceEffect.lpvTypeSpecificParams   = &m_diPeriodic;
    mForceEffect.dwStartDelay            = 0;
#else
    mForceEffect.type                           = LG_TYPE_TRIANGLE;
    mForceEffect.duration                       = 500;
    mForceEffect.startDelay                     = 0;
    mForceEffect.p.periodic.magnitude           = 0;
    mForceEffect.p.periodic.direction           = 90;
    mForceEffect.p.periodic.period              = 80;
    mForceEffect.p.periodic.phase               = 0;
    mForceEffect.p.periodic.offset              = 0;
    mForceEffect.p.periodic.envelope.attackTime  = 0;
    mForceEffect.p.periodic.envelope.fadeTime    = 0;
    mForceEffect.p.periodic.envelope.attackLevel = 0;
    mForceEffect.p.periodic.envelope.fadeLevel   = 0;
#endif
}

//=============================================================================
// WheelRumble::~WheelRumble
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
WheelRumble::~WheelRumble()
{
}

//=============================================================================
// WheelRumble::OnInit
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WheelRumble::OnInit()
{
#ifdef RAD_WIN32
    m_diPeriodic.dwMagnitude = 0;
#else
    mForceEffect.p.periodic.magnitude = 0;
#endif
}

//=============================================================================
// WheelRumble::SetMagDir
//=============================================================================
// Description: Comment
//
// Parameters:  ( u8 mag, u16 dir )
//
// Return:      void 
//
//=============================================================================
#ifdef RAD_WIN32
void WheelRumble::SetMagDir( u16 mag, u16 dir )
#else
void WheelRumble::SetMagDir( u8 mag, u16 dir )
#endif
{
#ifdef RAD_WIN32
    LONG rglDirection[2]      = { dir, 0 };
    m_diPeriodic.dwMagnitude = mag;
    mForceEffect.rglDirection = rglDirection;
#else
    mForceEffect.p.periodic.magnitude = mag;
    mForceEffect.p.periodic.direction = dir;
#endif

    mEffectDirty = true;
}

//=============================================================================
// WheelRumble::SetPPO
//=============================================================================
// Description: Comment
//
// Parameters:  ( u16 per, u16 phas, s16 offset )
//
// Return:      void 
//
//=============================================================================
void WheelRumble::SetPPO( u16 per, u16 phas, s16 offset )
{
#ifdef RAD_WIN32
    m_diPeriodic.dwPeriod                = per;
    m_diPeriodic.dwPhase                 = phas;
    m_diPeriodic.lOffset                 = offset;
#else
    mForceEffect.p.periodic.period = per;
    mForceEffect.p.periodic.phase = phas;
    mForceEffect.p.periodic.offset = offset;
#endif

    mEffectDirty = true;
}

//=============================================================================
// WheelRumble::SetRumbleType
//=============================================================================
// Description: Comment
//
// Parameters:  ( u8 type )
//
// Return:      void 
//
//=============================================================================
void WheelRumble::SetRumbleType( u8 type ) 
{
#ifdef RAD_WIN32
    
#else
    mForceEffect.type = type;
#endif
     
};

#ifdef RAD_WIN32
void WheelRumble::Update(unsigned timeins)
{
    m_currentTime += timeins;
    if( m_currentTime > m_effectTime )
    {
        SetMagDir(0,0);
        m_currentTime = 0;
    }
    ForceEffect::Update();
}
#endif

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
