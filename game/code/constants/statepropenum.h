//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   None, header only
//
// Description: A listing of all the events and callbacks in use by the Srr2 
//              stateprop implementation
//
//              "Stateprops are awesome, everything should be a stateprop. - Aryan"
//                                              
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef STATEPROPENUM_H
#define STATEPROPENUM_H

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

namespace StatePropEnum
{
    enum Events
    {
        
    };

    enum Callbacks
    {
        eStateChange = -1, // The only callback thats reserved for use by the stateprop system
        eRemoveFromWorld = 0,
        eSpawn5Coins = 1,
        eRemoveCollisionVolume = 2,
        eFireEnergyBolt = 3,
        eKillSpeed = 4,
        eSpawn10Coins = 5,
        eSpawn15Coins = 6,
        eSpawn20Coins = 7,
        eRadiateForce = 8,
        eEmitLeaves = 9,
        eObjectDestroyed = 10,
        eSpawn5CoinsZ = 11,
        eSpawn1Coin = 12,
        eColaDestroyed = 13,
        eCamShake = 14,
        eRemoveFirstCollisionVolume = 15,
        eRemoveSecondCollisionVolume = 16,
        eRemoveThirdCollisionVolume = 17
    };
}
#endif