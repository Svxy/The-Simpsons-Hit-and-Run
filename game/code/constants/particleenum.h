//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   None, header only
//
// Description: Enumeration names for all particle systems in the game
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef PARTICLEENUM_H
#define PARTICLEENUM_H

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

namespace ParticleEnum
{
    enum ParticleID
    {
        eNull = -1,                   // No Particle Effect. Assertion if this is attempted to play
		eShrub = 3,
        eGarbage = 4,
        eOakTreeLeaves = 5,
		eMail = 6,
		ePineTreeNeedles = 7,
		eStars = 8,
		eSmokeSpray = 9,
		eDirtSpray = 10,                   // 10
        eGrassSpray = 11,
        eWaterSpray = 12,
        eEngineSmokeLight = 13,
        eEngineSmokeHeavy = 14,
        eEngineSmokeMedium = 16,
		ePowerBoxExplosion = 17,
		eFrinksCarSpecialEffect = 18,
        eFireSpray = 19,
        eAlienCameraExplosion = 20,
        eHoverBikeFlame = 21,
        eCoconutsDroppingShort = 22,
        eCoconutsDroppingTall = 23,
        eParkingMeter = 24,
        eCarExplosion = 25,
        ePopsicles = 26,
        eNumParticleTypes = 27

    };
}
#endif