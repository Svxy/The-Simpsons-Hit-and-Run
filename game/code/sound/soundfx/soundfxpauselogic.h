//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfxpauselogic.h
//
// Description: Declaration for the SoundFXPauseLogic class, which handles
//              the translation of events into sound effects for the pause
//              menu.
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDFXPAUSELOGIC_H
#define SOUNDFXPAUSELOGIC_H

//========================================
// Nested Includes
//========================================
#include <sound/soundfx/soundfxlogic.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    SoundFXPauseLogic
//
//=============================================================================

class SoundFXPauseLogic : public SoundFXLogic
{
    public:
        SoundFXPauseLogic();
        virtual ~SoundFXPauseLogic();

        void RegisterEventListeners();

        void HandleEvent( EventEnum id, void* pEventData );

    private:
        //Prevent wasteful constructor creation.
        SoundFXPauseLogic( const SoundFXPauseLogic& original );
        SoundFXPauseLogic& operator=( const SoundFXPauseLogic& rhs );
};


#endif // SOUNDFXPAUSELOGIC_H

