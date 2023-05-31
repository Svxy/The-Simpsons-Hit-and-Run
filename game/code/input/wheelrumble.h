//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wheelrumble.h
//
// Description: Blahblahblah
//
// History:     6/25/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef WHEELRUMBLE_H
#define WHEELRUMBLE_H

//========================================
// Nested Includes
//========================================
#include <input/forceeffect.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WheelRumble : public ForceEffect
{
public:
    WheelRumble();
    virtual ~WheelRumble();

    void OnInit();

#ifdef RAD_WIN32
    void SetMagDir( u16 mag, u16 dir );
#else
    void SetMagDir( u8 mag, u16 dir );
#endif
    void SetPPO( u16 per, u16 phas, s16 offset );

    void SetRumbleType( u8 type );
#ifdef RAD_WIN32
    void Update(unsigned timeins = 0);
#endif

private:

#ifdef RAD_WIN32
    DIPERIODIC m_diPeriodic;
    DIENVELOPE m_diEnvelope;
#endif
    //Prevent wasteful constructor creation.
    WheelRumble( const WheelRumble& wheelrumble );
    WheelRumble& operator=( const WheelRumble& wheelrumble );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //WHEELRUMBLE_H
