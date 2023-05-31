//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   vehicleparticleemitter
//
// Description: An encapsulation of all the particle effects that are generated
//              by a vehicle in Simpsons2
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef VEHICLEPARTICLEEMITTER_H
#define VEHICLEPARTICLEEMITTER_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/particles/particlemanager.h>
#include <radmath/radmath.hpp>
#include <radmath/matrix.hpp>
#include <memory/map.h>

//===========================================================================
// Forward References
//===========================================================================
//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================
//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Bundles a large number of particle emitters together in a logical 
//      interface for a single Vehicle. Each Vehicle that has an VehicleParticleEmitter
//      object can easily generate all the 
//
// Constraints:
//      Requires the particle manager to be instanciated before any particles
//      can be generated.
//
//      The following types of particles can be emitted
//
//      Engine - ParticleEnum::eEngineSmokeHeavy
//               ParticleEnum::eEngineSmokeLight
//               ParticleEnum::eEngineSmokeMedium
//
//      Tires -  ParticleEnum::eGrassSpray
//				 ParticleEnum::eSmokeSpray
//				 ParticleEnum::eWaterSpray
//				 ParticleEnum::eDirtSpray
//  
//      Tailpipes - none
//		SpecialFX - none
//
//      More will appear here as they are made
//===========================================================================

class VehicleParticleEmitter
{
    public:
        VehicleParticleEmitter();
        ~VehicleParticleEmitter();

        enum VehiclePartEnum
        {
            eEngine,
            eLeftBackTire,
            eRightBackTire,
            eLeftTailPipe,
			eRightTailPipe,
			eSpecialEmitter
        };

        // Plays the given particle system (EMISSION set to 1) for the given vehicle part
        // and the given particle type
        void Generate( VehiclePartEnum partEnum, 
                       const ParticleAttributes& attr, 
                       const rmt::Matrix& vehicleTransform );

        // sets the location of this part relative to the vehicle's origin
        void SetPartLocation( VehiclePartEnum part, const rmt::Vector& partOffset );

    protected:

    private:
        // An emitter that encapsulates all the types of particles that
        // can come from a certain part of the vehicle (tailpipe, engine, etc).
        class PartEmitter
        {
        public:
            PartEmitter( const rmt::Vector& location );
            ~PartEmitter();

            void AddParticleType( ParticleEnum::ParticleID type );
            void SetLocation( const rmt::Vector& location );

            void Generate( const ParticleAttributes& attr, const rmt::Matrix& localMatrix );
                   
        private:
            
            Map < ParticleEnum::ParticleID, ParticlePlayerID > mParticlePlayers;
            rmt::Vector mLocation;
        };
        PartEmitter mEngineEmitter;
        PartEmitter mLeftBackTireEmitter;
        PartEmitter mRightBackTireEmitter;
		PartEmitter mLeftTailPipeEmitter;
		PartEmitter mRightTailPipeEmitter;
		PartEmitter mSpecialEmitter;

        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow VehicleParticleEmitter from being copied and assigned.
        VehicleParticleEmitter( const VehicleParticleEmitter& );
        VehicleParticleEmitter& operator=( const VehicleParticleEmitter& );

};


#endif



