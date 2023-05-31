//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        win32reverbcontroller.h
//
// Description: Declaration for the Win32ReverbController class, which provides
//              the Windows-specific reverb control
//
// History:     03/25/2003 + Created -- Ziemek
//
//=============================================================================

#ifndef WIN32REVERBCONTROLLER_H
#define WIN32REVERBCONTROLLER_H

//========================================
// Nested Includes
//========================================
#include <sound/soundfx/reverbcontroller.h>

//========================================
// Forward References
//========================================
struct IRadSoundHalEffectEAX2Reverb;

//=============================================================================
//
// Synopsis:    Win32ReverbController
//
//=============================================================================

class Win32ReverbController : public ReverbController
{
public:
    Win32ReverbController();
    virtual ~Win32ReverbController();

    void SetReverbOn( reverbSettings* settings );
    void SetReverbOff();

private:
    //Prevent wasteful constructor creation.
    Win32ReverbController( const Win32ReverbController& original );
    Win32ReverbController& operator=( const Win32ReverbController& rhs );

    //
    // Radsound's Win32 reverb interface
    //
    IRadSoundHalEffectEAX2Reverb* m_reverbInterface;
};


#endif // WIN32REVERBCONTROLLER_H

