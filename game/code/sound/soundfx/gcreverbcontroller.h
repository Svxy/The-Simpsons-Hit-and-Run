//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gcreverbcontroller.h
//
// Description: Declaration for the GCReverbController class, which provides
//              the GC-specific reverb control
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

#ifndef GCREVERBCONTROLLER_H
#define GCREVERBCONTROLLER_H

//========================================
// Nested Includes
//========================================
#include <sound/soundfx/reverbcontroller.h>

//========================================
// Forward References
//========================================
struct IRadSoundEffectStdReverbGcn;

//=============================================================================
//
// Synopsis:    GCReverbController
//
//=============================================================================

class GCReverbController : public ReverbController
{
    public:
        GCReverbController();
        virtual ~GCReverbController();

        void SetReverbOn( reverbSettings* settings );
        void SetReverbOff();

    private:
        //Prevent wasteful constructor creation.
        GCReverbController( const GCReverbController& original );
        GCReverbController& operator=( const GCReverbController& rhs );

        //
        // Radsound's GC reverb interface
        //
        IRadSoundEffectStdReverbGcn* m_reverbInterface;
};


#endif // GCREVERBCONTROLLER_H

