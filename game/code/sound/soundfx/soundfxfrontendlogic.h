//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfxfrontendlogic.h
//
// Description: Declaration for the SoundFXFrontEndLogic class, which handles
//              the translation of events into sound effects for the front
//              end.
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDFXFRONTENDLOGIC_H
#define SOUNDFXFRONTENDLOGIC_H

//========================================
// Nested Includes
//========================================
#include <sound/soundfx/soundfxlogic.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    SoundFXFrontEndLogic
//
//=============================================================================

class SoundFXFrontEndLogic : public SoundFXLogic
{
    public:
        SoundFXFrontEndLogic();
        virtual ~SoundFXFrontEndLogic();

        void RegisterEventListeners();

        virtual void HandleEvent( EventEnum id, void* pEventData );

    private:
        //Prevent wasteful constructor creation.
        SoundFXFrontEndLogic( const SoundFXFrontEndLogic& original );
        SoundFXFrontEndLogic& operator=( const SoundFXFrontEndLogic& rhs );
};


#endif // SOUNDFXFRONTENDLOGIC_H

