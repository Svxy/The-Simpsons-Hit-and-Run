//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   UFOAttackBehaviour
//
// Description: Atta
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef UFOATTACKBEHAVIOUR_H
#define UFOATTACKBEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/ufobehaviour.h>
#include <radmath/random.hpp>
#include <render/culling/SwapArray.h>
#include <stateprop/stateprop.hpp>
#include <render/DSG/DynaPhysDSG.h>

//===========================================================================
// Forward References
//===========================================================================

class ActorDSG;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================


class UFOAttackBehaviour : public UFOBehaviour
{
    public:
        // maximum firing range in meters
        UFOAttackBehaviour( float maxFiringRange );
        virtual ~UFOAttackBehaviour();
        virtual void Apply( Actor*, unsigned int timeInMS );
        
        void SetMaxFiringRange( float meters );
        void SetActorMoveSpeed( float kph );

        // Enable this behaviour
        virtual void Activate();
        // Disable this behaviour
        virtual void Deactivate();

        enum State
        {
            eSearching,
            eAttacking,
            eUsingTractorBeam,
            eDamaged,
            eDestroyed
        };


    protected:

        bool FindPositionInAttackRange( const rmt::Vector& actorPos, const rmt::Vector& targetPos, rmt::Vector* out_destination );
        bool WithinFiringRange( const rmt::Vector& actorPos, const rmt::Vector& target )const;
        void MoveIntoAttackRange( const Actor& actor, const rmt::Vector& target );
        // Searches the DSG for available targets
        int ScanForTargets( const rmt::Vector& actorPosition, float maxRangeSqr, SwapArray< DynaPhysDSG* >* pTargetList );
      
        // Finds a new target (currently just peds) thats closet to the
        // given actor position
        DynaPhysDSG* FindTarget( const rmt::Vector& actorPosition, const ActorDSG* dsg );




        float m_MaxFiringRange;     // in meters
        float m_MaxFiringRangeSqr;  
        float m_Speed;              // meters per milliseconds

        SwapArray< DynaPhysDSG* > m_TargetList;
        DynaPhysDSG* m_CurrentTarget;
        State m_CurrentState;

        static rmt::Randomizer s_Randomizer;
        static bool s_RandomizerSeeded;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow UFOAttackBehaviour from being copied and assigned.
        UFOAttackBehaviour( const UFOAttackBehaviour& );
        UFOAttackBehaviour& operator=( const UFOAttackBehaviour& );
};



#endif