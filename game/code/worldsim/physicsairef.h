//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        physicsairef.h
//
// Description: Blahblahblah
//
// History:     6/17/2002 + Created -- TBJ
//
//=============================================================================

#ifndef PHYSICSAIREF_H
#define PHYSICSAIREF_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

namespace PhysicsAIRef
{
    enum 
    { 
        redBrickPhizDefault = 0,
            redBrickVehicle, 
            redBrickPhizVehicleGroundPlane, 
            redBrickPhizMoveableGroundPlane,    // might not need to differnetiate here!
            redBrickPhizFence,    // might not need to differnetiate here!
            redBrickPhizStatic = 1 << 16,
            redBrickPhizMoveable = 1 << 17,   // don't think we actually need to differentiate between moveable and moveableinstance
            PlayerCharacter,
            NPCharacter,
            redBrickPhizMoveableAnim = 1 << 18, 
            CameraSphere,
            StateProp,
            ActorStateProp,
            redBrickPhizLast 
    }; // phizGround, phizStatic, phizMoveableAnim, phizMoveable, phizCamera, phizLast };
};


#endif //PHYSICSAIREF_H
