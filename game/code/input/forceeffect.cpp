//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        forceeffect.cpp
//
// Description: Implement ForceEffect
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
#include <input/forceeffect.h>


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
// ForceEffect::ForceEffect
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ForceEffect::ForceEffect() :
mOutputPoint( NULL ),
#ifdef RAD_WIN32
m_effectTime(0),
m_currentTime(0),
#endif
mEffectDirty( true )
{
#ifdef RAD_WIN32
    ZeroMemory( &mForceEffect, sizeof(mForceEffect) );
    ZeroMemory( &m_rglDirection, sizeof(m_rglDirection) );

    mForceEffect.dwSize                  = sizeof(DIEFFECT);

    m_rgdwAxes[0] = DIJOFS_X;
    m_rgdwAxes[1] = DIJOFS_Y;
#endif
}

//=============================================================================
// ForceEffect::~ForceEffect
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ForceEffect::~ForceEffect()
{
    if ( mOutputPoint )
    {
        mOutputPoint->Stop();
        mOutputPoint->Release();
        mOutputPoint = NULL;
    }
}

//=============================================================================
// ForceEffect::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ( IRadControllerOutputPoint* outputPoint )
//
// Return:      void 
//
//=============================================================================
void ForceEffect::Init( IRadControllerOutputPoint* outputPoint )
{
#ifdef RAD_WIN32
    rAssertMsg( outputPoint != NULL, "Attempt to set the outputPoint with a NULL pointer." );
    // mOutputPoint is dirty.
    if ( mOutputPoint )
    {
        ShutDownEffects();
    }
#endif
    mOutputPoint = outputPoint;
    mOutputPoint->AddRef();

    OnInit();

    mOutputPoint->UpdateEffect( &mForceEffect );
}

//=============================================================================
// ForceEffect::Start
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ForceEffect::Start()
{
    //Clear out current effects.
    OnInit();
    mEffectDirty = true;

    if ( mOutputPoint )
    {
        mOutputPoint->Start();
    }
}

//=============================================================================
// ForceEffect::Stop
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ForceEffect::Stop()
{
    if ( mOutputPoint )
    {
        mOutputPoint->Stop();
    }
}

//=============================================================================
// ForceEffect::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
#ifdef RAD_WIN32
void ForceEffect::Update(unsigned timeins)
#else
void ForceEffect::Update()
#endif
{
    if ( mEffectDirty && mOutputPoint )
    {
        mOutputPoint->UpdateEffect( &mForceEffect );
        mEffectDirty = false;
    }
}
#ifdef RAD_WIN32
void ForceEffect::ShutDownEffects()
{
    if ( mOutputPoint )
    {
        mOutputPoint->ReleaseEffect();
        mOutputPoint->Release();
        mOutputPoint = NULL;
    }
}

void ForceEffect::SetResetTime( DWORD dwMilliSeconds )
{
    m_effectTime = dwMilliSeconds;
}
#endif
//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
