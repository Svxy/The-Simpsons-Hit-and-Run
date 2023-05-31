//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ActorAnimation
//
// Description: Actor animation driver
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ACTORANIMATION_H
#define ACTORANIMATION_H


//===========================================================================
// Nested Includes
//===========================================================================

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
//      A class that Actors can use to have a hardcoded, procedural animation implementation
//      instead of a long memory-expensive one exported from Maya
//
// Constraints:
//
//
//===========================================================================
class ActorAnimation
{
    public:
        ActorAnimation(){}
        virtual ~ActorAnimation(){ }
        virtual void SetState( int state )=0;
        // Apply animation, return true/false indicating whether or not the returned animation is different
        // from the given one
        virtual bool Update( const rmt::Matrix& currTransform, rmt::Matrix* newTransform, float deltaTime, tCompositeDrawable* = NULL )=0;

    protected:
        
            
    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow ActorAnimation from being copied and assigned.
};

class StatePropDSGProcAnimator
{
public:
    StatePropDSGProcAnimator() {};
    virtual ~StatePropDSGProcAnimator() {};
    virtual void Advance( float Deltams ) {};
    virtual void UpdateForRender( tCompositeDrawable* Drawable ) {};
};

#endif