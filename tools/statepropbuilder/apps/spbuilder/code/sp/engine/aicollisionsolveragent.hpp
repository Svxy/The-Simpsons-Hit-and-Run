#ifndef _AICOLLISIONSOLVERAGENT_HPP_
#define _AICOLLISIONSOLVERAGENT_HPP_

#include "simcommon/tlist.hpp"
#include "simcollision/impulsebasedcollisionsolver.hpp"
#include "simcollision/collisionanalyser.hpp"
#include "simcollision/collisionanalyserdata.hpp"

class AICollisionSolverAgent
: public sim::CollisionSolverAgent
{
public:
    AICollisionSolverAgent();
    ~AICollisionSolverAgent();

   //
   // this method is called at the beginning before anything is done with the collision
   // returning Solving_Aborted would cause the collision to be ignored.
   //
   sim::Solving_Answer PreCollisionEvent(sim::Collision& inCollision, int inPass);

   //
   // this method allows to trigger sounds, animation and modify the objects state.
   //
   sim::Solving_Answer CollisionEvent( sim::SimState* inSimStateA, int indexA, 
                                       sim::SimState* inSimStateB, int indexB, 
                                       const rmt::Vector& inPos, float inDvN, float inDvT);

   //
   // this is called everytime an impulse is computed, before it gets added to the objects
   //
   sim::Solving_Answer TestImpulse(rmt::Vector& mImpulse, sim::Collision& inCollision);

   //
   // the impulse has been added to the object's cache, testing that cache tells 
   // the consequence of adding the cache to the object's state. It is also a good 
   // time for adding the cache of the root node of an articulated object to its
   // virtual center of mass.
   //
   sim::Solving_Answer EndObjectCollision(sim::SimState* inSimState, int inIndex);
};

#endif