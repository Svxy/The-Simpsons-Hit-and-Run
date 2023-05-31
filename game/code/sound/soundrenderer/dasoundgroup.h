//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dasoundgroup.hpp
//
// Subsystem:   Dark Angel - Sound Grouping
//
// Description: Contains definitions, enumerations, and interfaces for a
//              Dark Angel sound group.
//
// Revisions:
//  + Created October 18, 2001 -- breimer
//
//=============================================================================

#ifndef _DASOUNDGROUP_HPP
#define _DASOUNDGROUP_HPP

//=============================================================================
// Included Files
//=============================================================================

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Typedefs and Enumerations
//=============================================================================

//
// The list of possible sound groups
//

enum daSoundGroup {
    // BASIC WIRING GROUPS
    CARSOUND,
    NIS,

    NUM_BASIC_SOUND_GROUPS,

    // SYSTEM WIRING GROUPS
    // Warning: Do not modify
    DIALOGUE = NUM_BASIC_SOUND_GROUPS,
    DIALOGUE_TUNE,
    DUCKABLE,
    MASTER,
    MASTER_TUNE,
    MUSIC,
    MUSIC_TUNE,
    AMBIENCE,
    AMBIENCE_TUNE,
    SOUND_EFFECTS,
    SOUND_EFFECTS_TUNE,
    OPTIONS_MENU_STINGERS,

    NUM_SOUND_GROUPS
};

enum DuckSituations
{
    DUCK_FULL_FADE,

    DUCK_SIT_PAUSE,
    DUCK_SIT_MISSION,
    DUCK_SIT_LETTERBOX,
    DUCK_SIT_DIALOG,
    DUCK_SIT_STORE,
    DUCK_SIT_ONFOOT,
    DUCK_SIT_MINIGAME,
    DUCK_SIT_JUST_MUSIC,
    DUCK_SIT_CREDITS,

    NUM_DUCK_SITUATIONS
};

enum DuckVolumes
{
    DUCK_SFX,
    DUCK_CAR,
    DUCK_MUSIC,
    DUCK_DIALOG,
    DUCK_AMBIENCE,

    NUM_DUCK_VOLUMES
};

//
// A structure for holding a collection of volumes
//
struct DuckVolumeSet
{
    float duckVolume[NUM_DUCK_VOLUMES];
};

} // Namespace
#endif //_DASOUNDGROUP_HPP

