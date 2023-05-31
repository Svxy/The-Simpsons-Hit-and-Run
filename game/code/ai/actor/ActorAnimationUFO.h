//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   actoranimationufo
//
// Description: Procedural animation for the UFO
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ACTORANIMATIONUFO_H
#define ACTORANIMATIONUFO_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/actoranimation.h>

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
//
//
// Constraints:
//
//===========================================================================
class ActorAnimationUFO : public ActorAnimation
{
    public:
        ActorAnimationUFO();
        virtual ~ActorAnimationUFO();

        virtual void SetState( int state ){}
        // Apply animation, return true/false indicating whether or not the returned animation is different
        // from the given one
        virtual bool Update( const rmt::Matrix& currTransform, rmt::Matrix* newTransform, float deltaTime, tCompositeDrawable* = NULL );

        int m_NumUpdates;
        
    protected:

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow ActorAnimationUFO from being copied and assigned.
        ActorAnimationUFO( const ActorAnimationUFO& );
        ActorAnimationUFO& operator=( const ActorAnimationUFO& );


};

#endif