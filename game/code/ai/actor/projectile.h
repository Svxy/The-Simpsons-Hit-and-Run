//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   projectile
//
// Description: a weapon actor
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef PROJECTILE_H
#define PROJECTILE_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/refcounted.hpp>
#include <render/DSG/StatePropDSG.h>
#include <ai/actor/actor.h>
#include <ai/actor/intersectionlist.h>

//===========================================================================
// Forward References
//===========================================================================

class Behaviour;
class StatePropDSG;


//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Projectiles - weapons such as energy bolts fired from wasps
//
// Constraints:
//      
//
//===========================================================================
class Projectile : public Actor
{
    public:
        Projectile();
        virtual ~Projectile();

        static void SetSpeed( float kph );
        
        virtual bool Init( const char* statePropname, const char* instanceName );
        virtual void Update( unsigned int timeInMS );
        virtual void AddBehaviour( Behaviour* );
        // Fixed path weapon, no turning in flight
        virtual void LookAt( const rmt::Vector&, unsigned int timeInMS ){}
        // Fixed path weapon, no turning in flight
        virtual void SetRotationSpeed( float degreesPerSecond ) {}
        virtual void ReleaseBehaviours(){}
        virtual void MoveTo( const rmt::Vector& destination, float speed ){}
        void Fire();
        void CalculateIntersections();

    protected:

        // Virtually all collision volumes stick out over the actual geometry
        // this function pushes a given intersection into the object more
        void AdjustCollisionPosition( const rmt::Vector& intersection, 
                                      const rmt::Vector& currPos,
                                      const rmt::Vector& prevPos,
                                      rmt::Vector* newIntersection );


        void ApplyDamage( DynaPhysDSG* object );
        // Joes test, is the given object a player thats currently jumping??
        bool IsJumpingPlayer( DynaPhysDSG* object );

        CStateProp* mpStateProp;

        static float s_Speed;
        float m_Speed;
        rmt::Vector m_CurrentPosition;

        
        bool m_WillHitStatic;
        rmt::Vector m_StaticIntersectionPoint;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow Actor from being copied and assigned.
        Projectile( const Projectile& );
        Projectile& operator=( const Projectile& );


};


#endif