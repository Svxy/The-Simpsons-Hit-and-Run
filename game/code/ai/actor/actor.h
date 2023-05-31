//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   actor
//
// Description: actor implementation
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ACTOR_H
#define ACTOR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/refcounted.hpp>
#include <render/DSG/StatePropDSG.h>
#include <ai/actor/intersectionlist.h>

//===========================================================================
// Forward References
//===========================================================================

class Behaviour;
class ActorDSG;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Interface class for AI entities. Namely bee cameras and the UFO boss
//
// Constraints:
//      
//
//===========================================================================
class Actor : public tRefCounted
{
    public:
        Actor();
        virtual ~Actor();

        
        virtual bool Init( const char* statePropname, const char* instanceName )= 0;
        virtual void Update( unsigned int timeInMS )=0;
        virtual void AddToDSG();
        virtual void RemoveFromDSG();
        virtual void AddBehaviour( Behaviour* ) = 0;
        virtual void Activate(){}
        virtual void DeactivateBehaviours(){}
        virtual void SetPosition( const rmt::Vector& position );
        virtual void GetPosition( rmt::Vector* pPosition )const;
        virtual void GetTransform( rmt::Matrix* pTransform )const;
        virtual const ActorDSG* GetDSG()const;
        virtual void SetTransform( const rmt::Matrix& transform );
        virtual void SetState( int state );
        // Set the ID associated with every StatePropDSG, used to determine 
        // relationship with projectile/weapon so they won't hit each other
        virtual void SetStatePropID( unsigned id );
        virtual unsigned int GetState()const;
        virtual const char* GetName()const;
        virtual tUID GetUID()const;
        virtual void SetUID( tUID );
        virtual tUID GetStatePropUID()const;
        virtual void LookAt( const rmt::Vector&, unsigned int timeInMS )=0;
        virtual void SetRotationSpeed( float degreesPerSecond )=0;
        virtual void ReleaseBehaviours()=0;
        virtual void MoveTo( const rmt::Vector& destination, float speed )=0;
        virtual bool IsMoving()const { return false; }
        virtual void LoadShield( const char* statePropName );
        virtual void LoadTractorBeam( const char* statePropName );
        void ActivateTractorBeam();
        void DeactivateTractorBeam();
        bool IsTractorBeamOn()const;
        // Sets whether or not this object should despawn on
        // is they get to far away from the avatar. defaults to true
        void SetShouldDespawn( bool despawn ){ m_ShouldDespawn = despawn; }
        bool ShouldDespawn()const { return m_ShouldDespawn; }
        
        // Queries all statics and dynamics that reside within the given position/radius
        // and add them to the m_IntersectionList
        void FillIntersectionList( const rmt::Vector& position, float radius );
        // Queries all the dynamics around the given point and adds them to the m_IntersectinList
        void FillIntersectionListDynamics( const rmt::Vector& position, float radius );
        // Ditto for statics
        void FillIntersectionListStatics( const rmt::Vector& position, float radius );
        void FillIntersectionListFence( const rmt::Vector& position, float radius );

    protected:
        
        ActorDSG* mp_StateProp;
        bool m_ShouldDespawn;
        
        // Holds all the collision volumes encompassed by the m_IntersectionSphere
        // Used for collision detection / movement
        IntersectionList m_IntersectionList;
        // Determines what the IntersectionList holds
        rmt::Sphere m_IntersectionSphere;                
        rmt::Vector m_PreviousPosition;

        bool m_IsInDSG;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow Actor from being copied and assigned.
        Actor( const Actor& );
        Actor& operator=( const Actor& );


};


#endif