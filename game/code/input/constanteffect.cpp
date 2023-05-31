//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        constanteffect.cpp
//
// Description: Implement ConstantEffect
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
#include <input/constanteffect.h>
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
// ConstantEffect::ConstantEffect
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ConstantEffect::ConstantEffect()
{
    //Setup the respective force effect structures.
#ifdef RAD_WIN32
    m_diConstant.lMagnitude              = 0;
    m_diEnvelope.dwSize                  = sizeof(DIENVELOPE);
    m_diEnvelope.dwAttackLevel           = 0;
    m_diEnvelope.dwAttackTime            = 0;
    m_diEnvelope.dwFadeLevel             = 0;
    m_diEnvelope.dwFadeTime              = 0;

    mForceEffect.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    mForceEffect.dwDuration              = 500;
    mForceEffect.dwGain                  = DI_FFNOMINALMAX;
    mForceEffect.dwTriggerButton         = DIEB_NOTRIGGER;
    mForceEffect.cAxes                   = 1;
    mForceEffect.rgdwAxes                = m_rgdwAxes;
    mForceEffect.rglDirection            = m_rglDirection;
    mForceEffect.lpEnvelope              = NULL;
    mForceEffect.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
    mForceEffect.lpvTypeSpecificParams   = &m_diConstant;
    mForceEffect.dwStartDelay            = 0;

#else
    mForceEffect.type                           = LG_TYPE_CONSTANT;
    mForceEffect.duration                       = 500;
    mForceEffect.startDelay                     = 0;
    mForceEffect.p.constant.magnitude           = 0;
    mForceEffect.p.constant.direction           = 0;
    mForceEffect.p.constant.envelope.attackTime  = 0;
    mForceEffect.p.constant.envelope.fadeTime    = 0;
    mForceEffect.p.constant.envelope.attackLevel = 0;
    mForceEffect.p.constant.envelope.fadeLevel   = 0;
#endif
}

//=============================================================================
// ConstantEffect::~ConstantEffect
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ConstantEffect::~ConstantEffect()
{
}

//=============================================================================
// ConstantEffect::OnInit
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ConstantEffect::OnInit()
{
#ifdef RAD_WIN32
    m_diConstant.lMagnitude = 0;
#else
    mForceEffect.p.constant.magnitude = 0;
#endif
}

//=============================================================================
// ConstantEffect::SetMagnitude
//=============================================================================
// Description: Comment
//
// Parameters:  ( s16 magnitude )
//
// Return:      void 
//
//=============================================================================
void ConstantEffect::SetMagnitude( s16 magnitude )
{
#ifdef RAD_WIN32
    m_diConstant.lMagnitude = magnitude;
#else
    mForceEffect.p.constant.magnitude = magnitude;
#endif
    mEffectDirty = true;
}

//=============================================================================
// ConstantEffect::SetDirection
//=============================================================================
// Description: Comment
//
// Parameters:  ( u16 direction )
//
// Return:      void 
//
//=============================================================================
void ConstantEffect::SetDirection( u16 direction )
{
#ifdef RAD_WIN32
    LONG rglDirection[2]      = { direction, 0 };
    mForceEffect.rglDirection = rglDirection;
#else
    mForceEffect.p.constant.direction = direction;
#endif

    mEffectDirty = true;
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
