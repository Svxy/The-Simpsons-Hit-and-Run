//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wiresystem.cpp
//
// Subsystem:   Dark Angel - Sound Tuner System
//
// Description: Wire the tuner
//
// Revisions:
//  + Created October 24, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>

#include <radsound.hpp>
#include <radsound_hal.hpp>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/soundtuner.h>

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Public functions
//=============================================================================

//=============================================================================
// Function:    ::daSoundTunerWireSystem
//=============================================================================
// Description: Wire the sound system
//
// NOTE: Music and ambience are ignored here, because they're controlled by
//       Radmusic and not us.
//
//-----------------------------------------------------------------------------

void daSoundTunerWireSystem
(
    IDaSoundWiring* pWiring
)
{
    // PATHS //////////////////////////////////////////////////////////////////

    //
    // Start by wiring up everything as dialogue by default.  This is because
    // we strip a little path info out of the dialogue files, so that we can
    // easily switch between languages.
    //
    pWiring->WirePath( DIALOGUE, "" );

    // Character
    pWiring->WirePath
    (
        CARSOUND,
        "sound\\carsound"
    );

    // Collision
    pWiring->WirePath
    (
        NIS,
        "sound\\nis"
    );

    pWiring->WirePath
    (
        SOUND_EFFECTS,
        "sound\\soundfx"
    );

    pWiring->WirePath
    (
        OPTIONS_MENU_STINGERS,
        "sound\\soundfx\\optionsmenu"
    );

    // SPECIAL GROUPS /////////////////////////////////////////////////////////

    pWiring->WireGroup( NIS, DIALOGUE );

    pWiring->WireGroup( DIALOGUE, DUCKABLE );
    pWiring->WireGroup( SOUND_EFFECTS, DUCKABLE );
    pWiring->WireGroup( CARSOUND, DUCKABLE );
    pWiring->WireGroup( OPTIONS_MENU_STINGERS, DUCKABLE );

    pWiring->WireGroup( DIALOGUE, DIALOGUE_TUNE );
    pWiring->WireGroup( SOUND_EFFECTS, SOUND_EFFECTS_TUNE );
    pWiring->WireGroup( MASTER, MASTER_TUNE );
}


} // Sound Namespace
