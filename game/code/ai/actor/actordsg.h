//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ActorDSG
//
// Description: ActorDSG is a statepropdsg that has an id that will indicate the
//              relationship between an object and the objects that it can shoot
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ACTORDSG_H
#define ACTORDSG_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/DSG/StatePropDSG.h>


//===========================================================================
// Forward References
//===========================================================================

namespace sim
{
    class PhysicsProperties;
};

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
//
//===========================================================================
class ActorDSG : public StatePropDSG
{
    public:
        ActorDSG();
        virtual ~ActorDSG();
        virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
        virtual sim::Solving_Answer PostReactToCollision( rmt::Vector& impulse, sim::Collision& inCollision );
        virtual void ApplyForce( const rmt::Vector& direction, float force );

        // ID for each stateprop. Basic idea: every stateprop gets a unique
        // integer identifier, except that projectiles that get fired have the same
        // id as the object that launched them. In prereact, if the two collision
        // objects have the same ID, solving is aborted.
        void SetID( int id ) { m_ID = id; }
        unsigned int GetID()const { return m_ID; }
        virtual void RecieveEvent( int callback , CStateProp* stateProp );
        void HandleEvent( StatePropDSG::Event );
        virtual void Display();
        virtual void AdvanceAnimation( float timeInMS );
        virtual int GetAIRef() { return PhysicsAIRef::ActorStateProp; }
        
        // Adding a shield to the prop
        bool LoadShield( const char* statePropName );
        void RestoreShield();
        bool HasShield()const { return m_ShieldEnabled; }
        // Tractor beams
        bool LoadTractorBeam( const char* statePropName );
        void ActivateTractorBeam();
        void DeactivateTractorBeam();
        bool IsTractorBeamOn()const;
        virtual void SetRank(rmt::Vector& irRefPosn, rmt::Vector& mViewVector);

        void SetPhysicsProperties( float mass, float friction, float rest, float tang );
        virtual void LoadSetup( CStatePropData* statePropData, 
                         int startState, 
                         const rmt::Matrix& transform,
                         CollisionAttributes* ipCollAttr,
                         bool isMoveable,
                         bool isDynaLoaded,
                         tEntityStore* ipSearchStore = NULL );

        // Dont use the stateprop shadow system, use the one provided
        // by StaticPhysDSG
        virtual int  CastsShadow(); 
        virtual void DisplaySimpleShadow()  { StaticPhysDSG::DisplaySimpleShadow(); }
               
        virtual void GenerateCoins( int numCoins );

    protected:

        void DestroyShield();
        void PlayShieldGettingHit();

        static unsigned int s_IDCounter;
        unsigned int m_ID;

        CStateProp* m_ShieldProp;
        CStateProp* m_TractorBeamProp;
        bool m_ShieldEnabled;
        sim::PhysicsProperties* mPhysicsProperties;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow ActorDSG from being copied and assigned.
        ActorDSG( const ActorDSG& );
        ActorDSG& operator=( const ActorDSG& );

 
};







#endif