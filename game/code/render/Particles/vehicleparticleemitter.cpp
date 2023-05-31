//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Name of subsystem or component this class belongs to.
//
// Description: This file contains the implementation of...
//
// Authors:     Name Here
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render\Particles\vehicleparticleemitter.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================




//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================


//===========================================================================
// VehicleParticleEmitter::PartEmitter::PartEmitter
//===========================================================================
// Description:
//      PartEmitter ctor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
VehicleParticleEmitter::PartEmitter::PartEmitter( const rmt::Vector& location)
: mLocation( location )
{
	mParticlePlayers.reserve( 10 );

}
//===========================================================================
// VehicleParticleEmitter::PartEmitter::~PartEmitter
//===========================================================================
// Description:
//      PartEmitter dtor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
VehicleParticleEmitter::PartEmitter::~PartEmitter()
{

}
//===========================================================================
// VehicleParticleEmitter::PartEmitter::AddParticleType
//===========================================================================
// Description:
//      Adds a new particle effect that can be emittted from this Vehicle Part
//
// Constraints:
//
// Parameters:
//      ParticleEnum::ParticleID indicating the type of particle effects
//      that can be emitted by this engine type
//
// Return:
//      None.
//===========================================================================
void VehicleParticleEmitter::PartEmitter::AddParticleType( ParticleEnum::ParticleID type )
{
    mParticlePlayers.insert( type, ParticleManager::GetInstance()->GetUniqueID() );
}
//===========================================================================
// VehicleParticleEmitter::SetPartLocation
//===========================================================================
// Description:
//      Sets the location of the vehicle part relative to the vehicle's origin
//
// Constraints:
//      No orientation with this function
//
// Parameters:
//      VehiclePartEnum - indicating which part we are setting the location offset of
//      partOffset - translation vector indicating offset from the vehicle origin
//
// Return:
//      None.
//===========================================================================
void VehicleParticleEmitter::SetPartLocation( VehiclePartEnum part, 
                                                           const rmt::Vector& partOffset )
{
    switch( part )
    {
    case eEngine:
        mEngineEmitter.SetLocation( partOffset );
        break;
    case eLeftBackTire: 
        mLeftBackTireEmitter.SetLocation( partOffset );
        break;
    case eRightBackTire:
        mRightBackTireEmitter.SetLocation( partOffset );
        break;
    case eLeftTailPipe:
		mLeftTailPipeEmitter.SetLocation( partOffset );
        break;
    case eRightTailPipe:
		mRightTailPipeEmitter.SetLocation( partOffset );
        break;
    default:
        // Unhandled case!
        rAssert(0);
        break;
    }

}
//===========================================================================
// VehicleParticleEmitter::SetPartLocation
//===========================================================================
// Description:
//      Sets the location of the vehicle part relative to the vehicle's origin
//
// Constraints:
//      No orientation with this function
//
// Parameters:
//      VehiclePartEnum - indicating which part we are setting the location offset of
//      partOffset - translation vector indicating offset from the vehicle origin
//
// Return:
//      None.
//===========================================================================
void VehicleParticleEmitter::PartEmitter::SetLocation( const rmt::Vector& location )
{
    mLocation = location;
}

//===========================================================================
// VehicleParticleEmitter::PartEmitter::Generate
//===========================================================================
// Description:
//          Tells this part of the vehicle to emit particles
//
// Constraints:
//
// Parameters:
//          ParticleAttributes struct indicating the particle effect that we want this vehicle part to emit
//          Matrix indicating the vehicle's location in the world
//
// Return:
//
//===========================================================================
void VehicleParticleEmitter::PartEmitter::Generate( const ParticleAttributes& attr, const rmt::Matrix& vehicleMatrix )
{
	Map< ParticleEnum::ParticleID, ParticlePlayerID >::iterator it = mParticlePlayers.find( attr.mType );
	
	rAssert( it != mParticlePlayers.end() );

    // Translate particle generation point form the origin to the parts location
    rmt::Matrix offsetLocationMatrix;
    offsetLocationMatrix.Identity();
    offsetLocationMatrix.FillTranslate( mLocation );

    rmt::Matrix partMatrix;
    partMatrix.Mult( offsetLocationMatrix, vehicleMatrix);

    ParticleManager::GetInstance()->PlayCyclic( it->second, attr, partMatrix );
}
            

//===========================================================================
// VehicleParticleEmitter::VehicleParticleEmitter
//===========================================================================
// Description:
//      VehicleParticleEmitter ctor
//      Initializes all the emitters and instructs them what type of particles that they
//      can allocate.
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

VehicleParticleEmitter::VehicleParticleEmitter()
: 
    mEngineEmitter( rmt::Vector( 0,1,0 ) ),
    mLeftBackTireEmitter( rmt::Vector(-0.8f, 0.5f, -1.5f) ),
    mRightBackTireEmitter( rmt::Vector(0.8f, 0.5f, -1.5f) ),
    mLeftTailPipeEmitter( rmt::Vector( -0.8f, 0, -1.5f ) ),
    mRightTailPipeEmitter( rmt::Vector( 0.8f, 0, -1.5f ) ),
    mSpecialEmitter( rmt::Vector( 0, 0 , 0 ) )
{
    // Add all the particles that can be emitted by the engine
    mEngineEmitter.AddParticleType( ParticleEnum::eEngineSmokeLight );
    mEngineEmitter.AddParticleType( ParticleEnum::eEngineSmokeHeavy );
    mEngineEmitter.AddParticleType( ParticleEnum::eEngineSmokeMedium );

    // Add all the particles that can be emitted by the tires
    mLeftBackTireEmitter.AddParticleType( ParticleEnum::eGrassSpray );
    mLeftBackTireEmitter.AddParticleType( ParticleEnum::eDirtSpray );
    mLeftBackTireEmitter.AddParticleType( ParticleEnum::eWaterSpray );
    mLeftBackTireEmitter.AddParticleType( ParticleEnum::eSmokeSpray );
    mLeftBackTireEmitter.AddParticleType( ParticleEnum::eFireSpray );

    mRightBackTireEmitter.AddParticleType( ParticleEnum::eGrassSpray );
    mRightBackTireEmitter.AddParticleType( ParticleEnum::eDirtSpray );
    mRightBackTireEmitter.AddParticleType( ParticleEnum::eWaterSpray );
    mRightBackTireEmitter.AddParticleType( ParticleEnum::eSmokeSpray );
    mRightBackTireEmitter.AddParticleType( ParticleEnum::eFireSpray );

	// Add all the particles that can be emitted by the special effects emitter
	mSpecialEmitter.AddParticleType( ParticleEnum::eFrinksCarSpecialEffect );
}
//===========================================================================
// VehicleParticleEmitter::~VehicleParticleEmitter
//===========================================================================
// Description:
//      VehicleParticleEmitter dtor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

VehicleParticleEmitter::~VehicleParticleEmitter()
{

}
//===========================================================================
// VehicleParticleEmitter::Generate
//===========================================================================
// Description:
//      Generates particles from the specified vehicle bodypart of the specified type 
//
// Constraints:
//      Can't generate just any particle type, it has to be assigned to that bodypart
//
// Parameters:
//      VehicleParts enumeration indicating which part of the vehicle is making the particles
//      ParticleAttributes with particle type and emission bias
//      Matrix indicating the base vehicle position / orientation
//
// Return:
//
//      None.
//
//===========================================================================

void VehicleParticleEmitter::Generate( VehiclePartEnum part, 
                                       const ParticleAttributes& attr,
                                       const rmt::Matrix& vehicleTransform )
{
    switch( part )
    {

    case eLeftBackTire:
        mLeftBackTireEmitter.Generate( attr, vehicleTransform );
        break;

    case eRightBackTire:
        mRightBackTireEmitter.Generate( attr, vehicleTransform );
        break;

    case eLeftTailPipe:
		mLeftTailPipeEmitter.Generate( attr, vehicleTransform );
        break;

    case eRightTailPipe:
		mRightTailPipeEmitter.Generate( attr, vehicleTransform );
        break;

	case eSpecialEmitter:
		mSpecialEmitter.Generate( attr, vehicleTransform );
		break;

    case eEngine:
        mEngineEmitter.Generate( attr, vehicleTransform );
        break;
    default:
        // Unhandled case
        rAssert( false );
        break;
    };
}
