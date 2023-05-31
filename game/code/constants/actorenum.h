//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   None, header only
//
// Description: Enumeration names actor states in the game
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ACTORENUM_H
#define ACTORENUM_H

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

namespace ActorEnum
{
    enum FlyingActorStates
    {
        eFadeIn = 0,
        eUnaggressive = 1,
        eTransitionToReadyToAttack = 2,
        eIdleReadyToAttack = 3,
        eAttacking = 4,
        eDestroyed = 5
    };
}
#endif