//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ActorAnimationWasp
//
// Description: Actor animation driver for wasps. Encodes the figure eight
//              pattern that they fly procedurally
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ACTORANIMATIONWASP_H
#define ACTORANIMATIONWASP_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/actoranimation.h>

//===========================================================================
// Forward References
//===========================================================================
class tCompositeDrawable;
class rmt::Vector;
class rmt::Quaternion;

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
class ActorAnimationWasp : public ActorAnimation
{
    public:
        ActorAnimationWasp();
        virtual ~ActorAnimationWasp();
        virtual void SetState( int state );
        virtual bool Update( const rmt::Matrix& currTransform, rmt::Matrix* newTransform, float deltaTime, tCompositeDrawable* = NULL );

    protected:

        // Which state the stateprop is currently in
        int m_CurrentState;


        float m_CurrentYOffset;
        rmt::Vector m_CurrentXOffset;
        float m_YBias;
        float m_XBias;       



    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow ActorAnimationWasp from being copied and assigned.
};

class WingAnimator : public StatePropDSGProcAnimator
{
public:
    // Note that frame rate is in animation frame rate, not the game frame rate.
    WingAnimator( float FrameRate = 30.0f );
    virtual ~WingAnimator();
    virtual void Advance( float Deltams );
    virtual void UpdateForRender( tCompositeDrawable* Drawable );

protected:
    rmt::Vector* mTranKeys;
    rmt::Quaternion* mRotKeys;
    float mAnimSpeed;
    float mTime;
    float mFrame;
    char mRWingIndex;
    char mLWingIndex;
    char mNumKeys;
};

#endif