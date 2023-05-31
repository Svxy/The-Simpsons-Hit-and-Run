//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AttackBehaviour
//
// Description: Atta
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ATTACKBEHAVIOUR_H
#define ATTACKBEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/behaviour.h>
#include <radmath/random.hpp>
#include <events/eventlistener.h>

//===========================================================================
// Forward References
//===========================================================================



//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================


class AttackBehaviour : public Behaviour, public EventListener
{
    public:
        // maximum firing range in meters, forward firing arc in degrees
        AttackBehaviour( float maxFiringRange, float firingArc );
        virtual ~AttackBehaviour();
        virtual void Apply( Actor*, unsigned int timeInMS );
        
        void SetMaxFiringRange( float meters );
        void SetFiringArc( float degrees );
        void SetActorMoveSpeed( float kph );
        void SetMovementIntervals( float seconds );
        bool IsMovementDisabled()const;

        // Enable this behaviour
        virtual void Activate();
        // Disable this behaviour
        virtual void Deactivate();

        virtual void HandleEvent( EventEnum id, void* pEventData );

    protected:

        bool WithinFiringRange( const rmt::Vector& actorPos, const rmt::Vector& target )const;
        bool WithinFiringArc( const rmt::Vector& actorPos, const rmt::Vector& actorFacing, const rmt::Vector& target )const;
        bool IsTooClose( const rmt::Vector& actorPos, const rmt::Vector& target )const;

        void MoveIntoAttackRange( Actor* actor, const rmt::Vector& target );
        void MoveAway( Actor* actor, const rmt::Vector& target );

        float m_MaxFiringRange;     // in meters
        float m_MaxFiringRangeSqr;  
        float m_FiringArc;          // half-arc radius, units = cos(angle)
        float m_Speed;              // meters per milliseconds

        float m_MovementIntervals;
        unsigned int m_TimeOfLastMove;

        int m_ActiveGagCount;
        bool m_InConversation;

        static rmt::Randomizer s_Randomizer;
        static bool s_RandomizerSeeded;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow AttackBehaviour from being copied and assigned.
        AttackBehaviour( const AttackBehaviour& );
        AttackBehaviour& operator=( const AttackBehaviour& );
};



#endif