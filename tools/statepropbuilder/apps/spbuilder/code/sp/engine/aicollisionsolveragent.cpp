

#include "aicollisionsolveragent.hpp"
#include "simcollision/collisionanalyserdata.hpp"

AICollisionSolverAgent::AICollisionSolverAgent()
{
    EnableCollisionAnalyser();
}

AICollisionSolverAgent::~AICollisionSolverAgent()
{
    DisableCollisionAnalyser();
}


// =======================================================
// PRE_COLLISION_EVENT
//
sim::Solving_Answer AICollisionSolverAgent::PreCollisionEvent(sim::Collision& inCollision, int inPass)
{
    return sim::Solving_Continue;
}


// =======================================================
// COLLISION_EVENT
//
sim::Solving_Answer AICollisionSolverAgent::CollisionEvent( 
                               sim::SimState* inSimStateA, int indexA, 
                               sim::SimState* inSimStateB, int indexB, 
                               const rmt::Vector& inPos, float inDvN, float inDvT)
{

    return sim::Solving_Continue;
}


// =======================================================
// TEST_IMPULSE
//
sim::Solving_Answer AICollisionSolverAgent::TestImpulse(rmt::Vector& mImpulse,
                                                        sim::Collision& inCollision)
{
    sim::SimState* inSimStateA = inCollision.mCollisionObjectA->GetSimState();
    sim::SimState* inSimStateB = inCollision.mCollisionObjectB->GetSimState();

    return sim::Solving_Continue;
}

// =======================================================
// END_OBJECT_COLLISION
//
sim::Solving_Answer AICollisionSolverAgent::EndObjectCollision(
                                                    sim::SimState* inSimState,
                                                    int inIndex)
{
    return sim::Solving_Continue;
}