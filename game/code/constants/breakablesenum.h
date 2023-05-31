//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   None, header only
//
// Description: Enumeration names for all breakables in the game
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef BREAKABLESENUM_H
#define BREAKABLESENUM_H

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

namespace BreakablesEnum
{
    enum BreakableID
    {
        eNull = -1,                   // No breakable. Assertion if this is attempted to play
        eHydrantBreaking = 3,
        eMailboxBreaking = 5 ,            
        eParkingMeterBreaking = 6,
        eWoodenCratesBreaking = 7,
        eTommacoPlantsBreaking = 8,
        ePowerCouplingBreaking = 9,
		ePineTreeBreaking = 14,
        eOakTreeBreaking = 15,
		eBigBarrierBreaking = 16,
		eRailCrossBreaking  = 17,
		eSpaceNeedleBreaking = 18,
        eKrustyGlassBreaking = 19,
        eCypressTreeBreaking = 20,
        eDeadTreeBreaking = 21,
        eSkeletonBreaking = 22,
        eWillow = 23,
        eCarExplosion = 24,
        eGlobeLight = 25,
        eTreeMorn = 26,
        ePalmTreeSmall = 27,
        ePalmTreeLarge = 28,
        eStopsign = 29,
        ePumpkin = 30,
        ePumpkinMed = 31,
        ePumpkinSmall = 32,
        eCasinoJump = 33,
        eNumBreakables = 34
    };

    enum { eMaxBreakableNames = 10 };
}
#endif