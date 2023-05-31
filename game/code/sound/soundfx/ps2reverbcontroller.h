//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps2reverbcontroller.h
//
// Description: Declaration for the PS2ReverbController class, which provides
//              the PS2-specific reverb control
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

#ifndef PS2REVERBCONTROLLER_H
#define PS2REVERBCONTROLLER_H

//========================================
// Nested Includes
//========================================
#include <sound/soundfx/reverbcontroller.h>

//========================================
// Forward References
//========================================
struct IRadSoundEffectPs2;

//=============================================================================
//
// Synopsis:    PS2ReverbController
//
//=============================================================================

class PS2ReverbController : public ReverbController
{
    public:
        PS2ReverbController();
        virtual ~PS2ReverbController();

        void SetReverbOn( reverbSettings* settings );
        void SetReverbOff();

    private:
        //Prevent wasteful constructor creation.
        PS2ReverbController( const PS2ReverbController& original );
        PS2ReverbController& operator=( const PS2ReverbController& rhs );

        //
        // Radsound's PS2 reverb interface
        //
        IRadSoundEffectPs2* m_reverbInterface;
};


#endif // PS2REVERBCONTROLLER_H

