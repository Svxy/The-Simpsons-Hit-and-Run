//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        tutorialmode.h
//
// Description: this system controls the operation of tutorial mode
//
// History:     19/12/2002 + Created -- Ian Gipson
//
//=============================================================================

#ifndef TUTORIALMODE_H
#define TUTORIALMODE_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

//========================================
// typedefs
//========================================
enum TutorialMode
{
    TUTORIAL_BREAK_CAMERA,
    TUTORIAL_BONUS_MISSION,
    TUTORIAL_START_GAME,
    TUTORIAL_GETTING_INTO_PLAYER_CAR,
    TUTORIAL_GETTING_INTO_TRAFFIC_CAR,
    TUTORIAL_INTERACTIVE_GAG,
    TUTORIAL_RACE,
    TUTORIAL_COLLECTOR_CARD,
    TUTORIAL_COLLECTOR_COIN,
    TUTORIAL_REWARD,
    TUTORIAL_GETTING_OUT_OF_CAR,
    TUTORIAL_BREAKABLE_APPROACHED,
    TUTORIAL_BREAKABLE_DESTROYED,
    TUTORIAL_INTERIOR_ENTERED,
    TUTORIAL_COIN_COLLECTED,                            //never triggered?
    TUTORIAL_VEHICLE_DESTROYED,
    TUTORIAL_UNLOCKED_CAR,
    TUTORIAL_WRENCH,
    TUTORIAL_AT_CAR_DOOR,
    TUTORIAL_INTERACTIVE_GAG_APPROACHED,                //never triggered
    TUTORIAL_WAGER_MISSION,
    TUTORIAL_INVALID,
    TUTORIAL_MAX = TUTORIAL_INVALID
};

//=============================================================================
//
// Synopsis:    
//
//=============================================================================


#endif //TUTORIALMODE_H

