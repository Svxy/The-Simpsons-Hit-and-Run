//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        collisionentitydsg.h
//
// Description: Blahblahblah
//
// History:     6/14/2002 + Created -- TBJ
//
//=============================================================================

#ifndef COLLISIONENTITYDSG_H
#define COLLISIONENTITYDSG_H

//========================================
// Nested Includes
//========================================

//=================================================
// System Includes
//=================================================
#include <string.h>
#include <p3d/refcounted.hpp>
#include <radmath/radmath.hpp>

#include <simcollision/impulsebasedcollisionsolver.hpp>
#include <simcollision/collision.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/DSG/IEntityDSG.h>
#include <constants/physprop.h>
#include <constants/particleenum.h>
#include <constants/breakablesenum.h>
#include <render/Enums/RenderEnums.h>

namespace sim
{
    class Collision;
    class SimState;
};


//=============================
//Forward Reference
//=============================
class tParticleSystem;




// Contains attributes for collision reactions.
// Particle system, sound fx.
//
class CollisionAttributes
:
public tRefCounted
{
public:
    
    CollisionAttributes( void );
    CollisionAttributes(char* p_sound,char* p_particle,char* p_animation,float friction,float mass,float elasticity,unsigned int classtypeid, float volume);
    ~CollisionAttributes( void );
    void SetSound(char * p_sound);
    void SetParticle(ParticleEnum::ParticleID p_particle);
    void SetBreakable( BreakablesEnum::BreakableID type );
    void SetAnimation(char* p_animation);
    void SetFriction(const float friction);
    void SetMass(const float mass);
    void SetElasticity(const float elasticity);
    void SetClasstypeid(unsigned int classtypeid);

    char* GetSound( void);
    char* GetAnimation(void);
    ParticleEnum::ParticleID GetParticle(void);
    BreakablesEnum::BreakableID GetBreakable(void);
    unsigned int GetClasstypeid(void);
    float GetMass(void);
    float GetFriction(void);
    float GetElasticity(void);

    sim::PhysicsProperties* GetPhysicsProperties() {return mPhizProp;} 
 
private:
    
    //char* mp_Sound;
    //char* mp_Animation;
    char mp_Sound[32];
    char mp_Animation[32];

    ParticleEnum::ParticleID mp_Particle;
    BreakablesEnum::BreakableID mBreakableID;
    float mMass;
    float mFriction;
    float mElasticity;
    enClasstypeID mClasstypeid;

    sim::PhysicsProperties* mPhizProp;
};

class CollisionEntityDSG
:
public IEntityDSG
{
public:
    CollisionEntityDSG( void );
    virtual ~CollisionEntityDSG( void );
    
    // Implement this method to handle collisions between ICollisionEntityDSG objects.
    //
    // All sim::SimState::mAIRefPointer's would point to a ICollisionEntityDSG.
    //
    // recall:
    // enum Solving_Answer { Solving_Continue = 0, Solving_Aborted };

    virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision ) = 0;

    // the PostReactToCollision doesn't need to be pure virtual
    //
    // many of the different classes will want to do the exact same thing here, so they can just explicitly call the 
    // implementation of the base class...
    virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);

      
    void SetSimState( sim::SimState* ipCollObj );

    // Returns a pointer to a table of collision attributes.
    // Attributes such as particle system, sound effects would be found here.
    //
    CollisionAttributes* GetCollisionAttributes( void ) const;
    void SetCollisionAttributes( CollisionAttributes* pCollisionAttributes );

    virtual int GetAIRef() = 0;

    // keep track of what simulation tick this object was last updated on
    // (to aviod duplicate updating when it is in multiple collision lists)
    unsigned GetLastUpdate() { return lastUpdate;}
    void SetLastUpdate(unsigned l) { lastUpdate = l;}

    // Tells the object what render layer it is currently occupying
    // Note - the default layer if this function is not used is the Level layer
    void SetRenderLayer( RenderEnums::LayerEnum renderLayer );
    // Returns the render layer enumeration that the object resides in
    RenderEnums::LayerEnum GetRenderLayer() const { return mRenderLayer; }

    short mPersistentObjectID;

protected:
    unsigned lastUpdate;

    virtual void OnSetSimState( sim::SimState* ipCollObj ) 
    {
    }
//private:
    CollisionAttributes* mpCollisionAttributes; // this should be protected I think, not private

	// Michael Riegger - indicates whether a particle effect has bee triggered. This does not
	// apply to breakables, but instead to one time things like mail flying out of a mailbox or
	// garbage from a garbage can
    bool mWasParticleEffectTriggered;
    // The Current render layer, needed for when the objects want to move themselves in the DSG
    RenderEnums::LayerEnum mRenderLayer;
};

#endif //COLLISIONENTITYDSG_H