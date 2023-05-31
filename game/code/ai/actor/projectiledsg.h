//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ProjectileDSG
//
// Description: ProjectileDSG is a statepropdsg with one minor difference
//              upon contact with an object in prereact, it destroys the other
//              object
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef PROJECTILEDSG_H
#define PROJECTILEDSG_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/actordsg.h>

//===========================================================================
// Forward References
//===========================================================================


//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================




//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Exactly the same as a StatePropDSG, but with the ability to 
//      destroy the object it touches in PreReactToCollision
//
// Constraints:
//
//
//===========================================================================
class ProjectileDSG : public ActorDSG
{
    public:
        ProjectileDSG();
        virtual ~ProjectileDSG();
        virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
        virtual sim::Solving_Answer PostReactToCollision( rmt::Vector& impulse, sim::Collision& inCollision );

        bool WasHit()const{ return mHasHit; }
        void SetWasHit( bool wasHit ){ mHasHit = wasHit; }



    protected:

        bool mHasHit : 1;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow XxxClassName from being copied and assigned.
        ProjectileDSG( const ProjectileDSG& );
        ProjectileDSG& operator=( const ProjectileDSG& );

 
};








#endif