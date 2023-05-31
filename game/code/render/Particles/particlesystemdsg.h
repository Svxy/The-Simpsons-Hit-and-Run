//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   particlesystemdsg
//
// Description: A DSG (InstStatEntityDSG) that contains a particle system
//              for insertion into the DSG tree
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef PARTICLESYSTEMDSG_H
#define PARTICLESYSTEMDSG_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/dsg/inststatentitydsg.h>

//===========================================================================
// Forward References
//===========================================================================

class tParticleSystem;
class tParticleSystemFactory;
class tEffectController;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Generally, describe what behaviour this class possesses that
//      clients can rely on, and the actions that this service
//      guarantees to clients.
//
// Constraints:
//      Describe here what you require of the client which uses or
//      has this class - essentially, the converse of the description
//      above. A constraint is an expression of some semantic
//      condition that must be preserved, an invariant of a class or
//      relationship that must be preserved while the system is in a
//      steady state.
//
//===========================================================================
class ParticleSystemDSG : public InstStatEntityDSG
{
    public:
        ParticleSystemDSG();
        ~ParticleSystemDSG();

        virtual void Display();
		virtual void DisplayBoundingBox( tColour colour );
        virtual void GetBoundingBox( rmt::Box3D* box );
        virtual void GetBoundingSphere( rmt::Sphere* sphere );

        virtual rmt::Vector* pPosition(){ rAssert( false ); return NULL; } 
        virtual const rmt::Vector& rPosition();
        virtual void GetPosition( rmt::Vector* ipPosn );
                
        void Init( tParticleSystemFactory* pFactory, tEffectController* pController);
        void SetVelocity( const rmt::Vector& velocity );

        void SetBias(unsigned bias, float b);
        int GetNumLiveParticles()const;
        int LastFrameReached()const;
        void SetTransform( const rmt::Matrix& transform );
        // Reset animation to start
        void Reset();
        void Update( float deltaTime );


    protected:

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow ParticleSystemDSG from being copied and assigned.
        ParticleSystemDSG( const ParticleSystemDSG& );
        ParticleSystemDSG& operator=( const ParticleSystemDSG& );

        rmt::Vector mPosition;
        tParticleSystem* mpSystem;
        tEffectController* mpController;
};



#endif