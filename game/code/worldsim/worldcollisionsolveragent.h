//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        worldcollisionsolveragent.h
//
// Description: Blahblahblah
//
// History:     6/14/2002 + Created -- TBJ
//
//=============================================================================

#ifndef WORLDCOLLISIONSOLVERAGENT_H
#define WORLDCOLLISIONSOLVERAGENT_H

//========================================
// Nested Includes
//========================================
#include <simcollision/impulsebasedcollisionsolver.hpp>

//========================================
// Forward References
//========================================
class RedBrickCollisionSolverAgent;

using namespace sim;
//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================
class WorldCollisionSolverAgent
:
public CollisionSolverAgent
{
public:
    virtual void ResetCollisionFlags() {};
};

class WorldCollisionSolverAgentManager
: 
public CollisionSolverAgent
{
public:
    WorldCollisionSolverAgentManager();
	~WorldCollisionSolverAgentManager();

    // the key method to override
    Solving_Answer PreCollisionEvent(Collision& inCollision, int inPass);
    Solving_Answer TestImpulse(rmt::Vector& mImpulse, Collision& inCollision);    
    Solving_Answer TestCache(SimState* inSimState, int inIndex);

    
    Solving_Answer EndObjectCollision(SimState* inSimState, int inIndex);


    // need to override this so that the sim library version doesn't automatically 
    // switch from ai to sim ctrl when objects are hit

    // this method allows to trigger sounds, animation and modify the objects state.
    Solving_Answer CollisionEvent(  SimState* inSimStateA, int indexA, 
                                    SimState* inSimStateB, int indexB, 
                                    const rmt::Vector& inPos, float inDvN, float inDvT,
                                    SimulatedObject** simA, SimulatedObject** simB);




    void ResetCollisionFlags();
private:

    //Prevent wasteful constructor creation.
	WorldCollisionSolverAgentManager( const WorldCollisionSolverAgent& worldcollisionsolveragent );
	WorldCollisionSolverAgentManager& operator=( const WorldCollisionSolverAgentManager& worldcollisionsolveragent );

    static const int NUM_SOLVERS = 1;
    WorldCollisionSolverAgent* mpCollisionSolverAgentArray[ NUM_SOLVERS ];
};


#endif //WORLDCOLLISIONSOLVERAGENT_H
