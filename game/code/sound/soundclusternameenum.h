//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundclusternameenum.h
//
// Description: Definition for SoundClusterName enum
//
// History:     11/18/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDCLUSTERNAMEENUM_H
#define SOUNDCLUSTERNAMEENUM_H

#include <constants/vehicleenum.h>

enum SoundClusterName
{
    SC_ALWAYS_LOADED,
    SC_FRONTEND,
    SC_INGAME,

    SC_LEVEL_SUBURBS,
    SC_LEVEL_DOWNTOWN,
    SC_LEVEL_SEASIDE,

    SC_LEVEL1,
    SC_LEVEL2,
    SC_LEVEL3,
    SC_LEVEL4,
    SC_LEVEL5,
    SC_LEVEL6,
    SC_LEVEL7,
    SC_MINIGAME,

    SC_NEVER_LOADED,

    SC_CHAR_APU,
    SC_CHAR_BART,
    SC_CHAR_HOMER,
    SC_CHAR_LISA,
    SC_CHAR_MARGE,

    SC_CAR_BASE,

    SC_MAX_CLUSTERS = SC_CAR_BASE + VehicleEnum::NUM_VEHICLES  // SC_CAR_BASE + 53 cars
};


#endif // SOUNDCLUSTERNAMEENUM_H

