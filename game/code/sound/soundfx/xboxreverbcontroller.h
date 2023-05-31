//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        xboxreverbcontroller.h
//
// Description: Declaration for the XboxReverbController class, which provides
//              the Xbox-specific reverb control
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

#ifndef XBOXREVERBCONTROLLER_H
#define XBOXREVERBCONTROLLER_H

//========================================
// Nested Includes
//========================================
#include <sound/soundfx/reverbcontroller.h>

//========================================
// Forward References
//========================================
struct IRadSoundHalEffectI3DL2ReverbXBox;

//=============================================================================
//
// Synopsis:    XboxReverbController
//
//=============================================================================

class XboxReverbController : public ReverbController
{
    public:
        XboxReverbController();
        virtual ~XboxReverbController();

        void SetReverbOn( reverbSettings* settings );
        void SetReverbOff();

    private:
        //Prevent wasteful constructor creation.
        XboxReverbController( const XboxReverbController& original );
        XboxReverbController& operator=( const XboxReverbController& rhs );

        //
        // Radsound's Xbox reverb interface
        //
        IRadSoundHalEffectI3DL2ReverbXBox* m_reverbInterface;
};


#endif // XBOXREVERBCONTROLLER_H

