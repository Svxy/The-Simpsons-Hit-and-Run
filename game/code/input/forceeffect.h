//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        forceeffect.h
//
// Description: Blahblahblah
//
// History:     6/25/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FORCEEFFECT_H
#define FORCEEFFECT_H

//========================================
// Nested Includes
//========================================
#include <radcontroller.hpp>

#ifdef RAD_GAMECUBE
#include <dolphin/lg.h>
#endif

#ifdef RAD_PS2
#include <liblgdev.h>
#endif

#ifdef RAD_WIN32
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

#if defined(RAD_PS2) || defined(RAD_WIN32)
typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
#endif

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ForceEffect
{
public:
    ForceEffect();
    virtual ~ForceEffect();

#ifdef RAD_WIN32
    void SetForceID( u8 forceID ) { m_index = forceID; }
    u8   GetForceID() const { return m_index; }
    
#endif

    void Init( IRadControllerOutputPoint* outputPoint );
    bool IsInit() const { return ( mOutputPoint != NULL ); };

    void Start();
    void Stop();

#ifdef RAD_WIN32
    virtual void Update(unsigned timeins = 0);
    void ShutDownEffects();
    void SetResetTime( DWORD dwMilliSeconds );
#else
    void Update();
#endif

protected:
    IRadControllerOutputPoint* mOutputPoint;
#ifdef RAD_WIN32
    DIEFFECT      mForceEffect;
    DWORD         m_rgdwAxes[2];
    LONG          m_rglDirection[2];
    u8            m_index;         // the index ID of this force.
    DWORD         m_currentTime;
    DWORD         m_effectTime;
#else
    LGForceEffect mForceEffect;
#endif
    bool mEffectDirty;

    virtual void OnInit() = 0;

    //Prevent wasteful constructor creation.
    ForceEffect( const ForceEffect& forceeffect );
    ForceEffect& operator=( const ForceEffect& forceeffect );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //FORCEEFFECT_H
