//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ParticleManager
//
// Description: Singleton class that encapsulates handles free floating particle systems
//              e.g. particles generated from collisions and other one-off entries
//              Particle Systems are added into the DSG
//
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <vector>
#include <memory\srrmemory.h> // Needed for my STL allocations to go on the right heap
#include <memory/stlallocators.h>
#include <p3d/p3dtypes.hpp> 
#include <radmath/radmath.hpp>
#include <radmath/vector.hpp>
#include <render/Enums/RenderEnums.h>
#include <p3d/effects/effect.hpp>
#include <constants/particleenum.h>
#include <render/particles/particlesystemdsg.h>
#include <p3d/array.hpp>

//===========================================================================
// Forward References
//===========================================================================

class tParticleSystemFactory; 
class tEffectController;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

typedef int ParticlePlayerID;


//===========================================================================
// Interface Definitions
//===========================================================================


// Structure passed to ParticleManager::Add that describes what type of 
// particle system to create
struct ParticleAttributes
{

    ParticleAttributes() :
        mEmissionBias ( 1.0f ),
        mVelocity( 0, 0, 0) 
        {   }
    ParticleEnum::ParticleID mType;

    // only one bias so far, the emission bias
    // other biases would be simple to put in 
    // NumParticles/Life/Speed/Weight/Gravity/Drag/Size/Spin
    float mEmissionBias;
    
    rmt::Vector mVelocity;

};

//===========================================================================
//
// Description: 
//      ParticleManager provides a way to centalize adding/updating/rendering
//      of particle systems. Particles are inserted into the scene graph at the
//      specified layer and rendered when that specific layer is rendered.
//
// Constraints:
//
//===========================================================================

class ParticleManager
{
   
    public:

        // Static Methods (for creating, destroying and acquiring an instance 
        // of the ParticleManager)
        static ParticleManager* CreateInstance();
        static ParticleManager* GetInstance();
        static void DestroyInstance();


		// Removes all particles from the DSG. Doesn't free any of them
		void DeactiveateAll();

        // Systems are allocated into the ParticleManager via the InitializeSystem functions
        // These should be called from inside the particle system and breakable object loaders
        void InitializeSystem( ParticleEnum::ParticleID type, tParticleSystemFactory* factory, tEffectController* controller, int maxInstances, bool isDynamic = false);

        // Deletes a system, and frees all particles associated with it
        void DeleteSystem( ParticleEnum::ParticleID type );

        // Kills all the particles of any type that was Initialized with isDynamic to true
        void DumpDynaLoad();

        // Returns an identifier that can be used to reference
        // a continuously playing particle effect
        ParticlePlayerID GetUniqueID()const;

        // Tells the PM to play a cycling particle system. Note that this function
        // must be called every frame to have the emitter keep adding particles to
        // it. Otherwise, EMISSION bias is set to zero. Alive particles will gradually
        // fall to earth and disappear.
        void PlayCyclic( ParticlePlayerID id, const ParticleAttributes& attr, const rmt::Matrix& localMatrix );
        void PlayCyclic( ParticlePlayerID id, const ParticleAttributes& attr, const rmt::Vector& position );

        // Add a new particle system to the manager and insert it into the scene graph
        void Add ( const ParticleAttributes& attr, const rmt::Matrix& localMatrix );
        void Add ( const ParticleAttributes& attr, const rmt::Vector& position );

        // Temporary force render function for debugging only
        void DebugRender ();

        // Update animations on all managed particle systems
        void Update ( unsigned int deltaTime);

        // Remove all particle systems from the manager
        void ClearSystems ();

    protected:

    private:

        // Disable all constructors and the copy assignment operator, the singleton functions 
        // CreateInstance and DestroyInstance handle this for the user
        ParticleManager();
        ~ParticleManager();    
        ParticleManager( const ParticleManager& );
        ParticleManager& operator=( const ParticleManager& );

        // Reorients the transform matrix so that Up is always <0,1,0>
        // needed for particle system's gravity to always work the right way
        void ReorientUpAxis( const rmt::Matrix& in, rmt::Matrix* out );

        class ManagedParticleSystem
        {
        public:
            ManagedParticleSystem( tParticleSystemFactory*, tEffectController* );
            ~ManagedParticleSystem();
            void Display()const { mpSystem->Display(); }
            void Update( float deltaTime );
        
            void Unlock() { mUserID = -1; }
            bool IsLocked()const { return (mUserID >= 0); }
            void SetBias( unsigned int bias, float value ){ mpSystem->SetBias( bias, value ); }
            void SetActive( bool isActive );
            void Reset();
            void SetTransform( const rmt::Matrix& );
            int LastFrameReached() const;
            ParticlePlayerID GetUserID()const { return mUserID; }
            void ThrowUpNewParticles() { mThrowUpNewParticles = true; }
            bool IsActive()const { return mIsActive; }
            void Lock( ParticlePlayerID id ) { mUserID = id; }
            void SetVelocity( const rmt::Vector& velocity ) { mpSystem->SetVelocity( velocity ); }

        private:
            RenderEnums::LayerEnum mLayer;
            ParticlePlayerID mUserID;
            bool mIsActive;      
            float mEmissionBias;
            bool mThrowUpNewParticles;
			bool mIsInDSG;
            ParticleSystemDSG* mpSystem;
        };

        // List of all active particle systems within the manager
        // Use a list for constant time random access deletions
 //       typedef std::vector< ManagedParticleSystem*, s2alloc<ManagedParticleSystem*> > MPSVector;
 //       typedef std::vector< MPSVector, s2alloc<MPSVector> > MPSVectorVector;
        
        typedef tPtrDynamicArray< ManagedParticleSystem* > MPSVector;
        typedef tPtrDynamicArray< MPSVector > MPSVectorVector;


        MPSVectorVector mActiveSystems;    

        // List of all particle types that are dynamically loaded
        std::vector< bool, s2alloc<bool> > mIsParticleTypeDynamicallyLoaded;
        
        // Static instance of the ParticleManager singleton
        static ParticleManager* spInstance;
};

// A little syntactic sugar for getting at this singleton.
inline ParticleManager* GetParticleManager() 
{ 
   return( ParticleManager::GetInstance() ); 
}

#endif