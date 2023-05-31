//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Flying Actor
//
// Description: Flying actors - bee cameras
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef FLYING_ACTOR_H
#define FLYING_ACTOR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/actor.h>
#include <render/culling/swaparray.h>
#include <ai/actor/behaviour.h>

//===========================================================================
// Forward References
//===========================================================================

class StatePropDSG;
class ActorAnimation;

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
// Constraints:
//
//===========================================================================
class FlyingActor : public Actor,
                    public CStatePropListener
{
    public:
        FlyingActor();
        virtual ~FlyingActor();

        virtual bool Init( const char* statePropname, const char* instanceName );
        virtual void Update( unsigned int timeInMS );
        virtual void AddBehaviour( Behaviour* );
        virtual void Activate();
        virtual void DeactivateBehaviours();

        // Set speed in degrees per second
        virtual void SetRotationSpeed( float rotationSpeed );
        // Give it a look at target, the actor will
        // automatically reorient itself to look at the given position
        virtual void LookAt( const rmt::Vector& position, unsigned int timeInMS );
        virtual void ReleaseBehaviours();

        void SetDesiredHeight( float desiredheight ){  m_DesiredHeight = desiredheight; }
        float GetDesiredHeight() const              { return m_DesiredHeight; }
        void SetDesiredHeightEnabled( bool enable ) { m_DesiredHeightEnabled = enable; }

        virtual void MoveTo( const rmt::Vector& destination, float speed );
        virtual bool IsMoving()const { return m_IsMoving; }

        //
        // Needed to trap state transitions for sound player -- Esan
        //
        void RecieveEvent( int callback , CStateProp* stateProp );

    protected:

        bool ChangeBehaviour( Behaviour* newBehaviour );
        bool FindGroundHeight( float x, float z, float* out_height )const;
        // If a movement destination was specified, updatemovement will move along that path
        // returns true if a new position was computed
        bool UpdateMovement( unsigned int timeInMS, rmt::Vector* out_newPosition );
        bool FindGroundIntersection( float* height, rmt::Vector* normal );    
        void FindWaypoint( const rmt::Vector& start, const rmt::Vector& end, int depth );

        // Randomizer
        static rmt::Randomizer s_Randomizer;
        static bool s_RandomizerSeeded;

        // STATE information
        // -----------------
        Behaviour* m_CurrentBehaviour;
        Behaviour* m_AttackBehaviour;
        Behaviour* m_EvadeBehaviour;
        Behaviour* m_AttractionBehaviour;

        // Animation for updating the root position of the object 
        // (procedural idle animation)
        ActorAnimation* m_ActorAnimation;

        // Movement information
    
        float m_DesiredHeight;
        bool m_DesiredHeightEnabled;
        bool m_IsMoving;
        rmt::Vector m_MovementDestination;
        float m_Speed;

        std::vector< rmt::Vector, s2alloc<rmt::Vector> > m_Waypoints;
        unsigned int m_CurrentWaypoint;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow FlyingActor from being copied and assigned.
        FlyingActor( const FlyingActor& );
        FlyingActor& operator=( const FlyingActor& );

    private:
        

        float m_RotationSpeed; // in radians per millisecond
        float m_GroundIntersectionHeight; // ground intersect height
        float m_HighestIntersectHeight;  // ground + object intersect height
        rmt::Vector m_HighestIntersectNormal; // normal of the highest intersect position
};

#endif