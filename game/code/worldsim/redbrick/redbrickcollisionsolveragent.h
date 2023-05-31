/*===========================================================================
   redbrickcollisionsolveragent.h

   created Jan 28, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.

===========================================================================*/

#ifndef _REDBRICKCOLLISIONSOLVERAGENT_H
#define _REDBRICKCOLLISIONSOLVERAGENT_H

#include <worldsim/worldcollisionsolveragent.h>

#include <simcommon/tlist.hpp>
#include <simcollision/collisionanalyser.hpp>
#include <simcollision/collisionanalyserdata.hpp>

using namespace sim;

// TODO - looking for the best place to put these
// a necessary part of the RedBrick interface

// TBJ moved 'em to <worldsim/physicsairef.h>
//
//enum RedBrickPhizAITypes { redBrickPhizDefault = 0, redBrickVehicle, redBrickPhizVehicleGroundPlane, redBrickPhizStatic, redBrickPhizMoveable, redBrickPhizMoveableAnim, redBrickPhizLast }; // phizGround, phizStatic, phizMoveableAnim, phizMoveable, phizCamera, phizLast };

class RedBrickCollisionSolverAgent : public WorldCollisionSolverAgent
{
public:

    RedBrickCollisionSolverAgent();
    ~RedBrickCollisionSolverAgent();

    // the key method to override
    Solving_Answer PreCollisionEvent(Collision& inCollision, int inPass);
    Solving_Answer TestImpulse(rmt::Vector& mImpulse, Collision& inCollision);    
    Solving_Answer TestCache(SimState* inSimState, int inIndex);

    Solving_Answer EndObjectCollision(SimState* inSimState, int inIndex);

    Solving_Answer CarOnCarPreTest(Collision& inCollision, int inPass);

    virtual void ResetCollisionFlags();

    bool mBottomedOut;
    float mBottomedOutScale;

    bool mWheelSidewaysHit;
    float mWheelSidewaysHitScale;


};

#endif  // _REDBRICKCOLLISIONSOLVERAGENT_H
