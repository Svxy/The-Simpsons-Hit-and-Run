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

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/Particles/particlesystemdsg.h>
#include <p3d/effects/effect.hpp>
#include <p3d/effects/particlesystem.hpp>
#include <p3d/utility.hpp>

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
// ParticleSystemDSG::ParticleSystemDSG
//===========================================================================
// Description:
//      ParticleSystemDSG constructor
//
// Constraints:
//      Constructor can only be called by the ParticleManager
//
// Parameters:
//      None
//
// Return:
//      None
//  //===========================================================================
ParticleSystemDSG::ParticleSystemDSG()
: mpSystem( NULL ), 
mpController( NULL )
{
	IEntityDSG::mTranslucent = true;
}
//===========================================================================
// ParticleSystemDSG::~ParticleSystemDSG
//===========================================================================
// Description:
//      ParticleSystemDSG destructor
//
// Constraints:
//      Destructor can only be called by ParticleManager
//
// Parameters:
//      None
//
// Return:
//      None
//
//===========================================================================
ParticleSystemDSG::~ParticleSystemDSG()
{
BEGIN_PROFILE( "ParticleSystemDSG Destroy" );
    if( mpController != NULL )
    {
        // Releasing the controller will release its attached effect
        mpController->Release();
		mpController = NULL;
		mpSystem = NULL;
    }
END_PROFILE( "ParticleSystemDSG Destroy" );
}
void ParticleSystemDSG::Init( tParticleSystemFactory* pFactory, tEffectController* pController )
{
    mpController = static_cast< tEffectController* >( pController->Clone() );
	rAssert( mpController != NULL );
    mpController->AddRef();

    mpSystem = static_cast< tParticleSystem*> ( pFactory->CreateEffect( mpController ) );
    mpSystem->SetCycleMode(FORCE_CYCLIC);
    rAssert( mpSystem != NULL );
   
};

//===========================================================================
// ParticleSystemDSG::SetBias
//===========================================================================
// Description:
//      Sets the contained tParticleSystem to a new bias
//
// Constraints:
//      None.
//
// Parameters:
//      p3dParticleSystemConstants:: identifier that indicates the bias
//      float b = bias value
//
// Return:
//      None.
//
//===========================================================================
void ParticleSystemDSG::SetBias(unsigned bias, float b)
{
    mpSystem->SetBias( bias, b );
}

//===========================================================================
// ParticleSystemDSG::GetNumLiveParticles
//===========================================================================
// Description:
//      Returns the number of active particles
//
// Constraints:
//      None.
//
// Parameters:
//      None
//
// Return:
//      integer with the number of active particles
//
//===========================================================================
int ParticleSystemDSG::GetNumLiveParticles()const
{
    return mpSystem->GetNumLiveParticles();
}

//===========================================================================
// ParticleSystemDSG::LastFrameReached
//===========================================================================
// Description:
//      Whether the effect controller has reached the end of the animation
//
// Constraints:
//      None.
//
// Parameters:
//      None
//
// Return:
//      int
//
//===========================================================================
int ParticleSystemDSG::LastFrameReached()const
{
    return mpController->LastFrameReached();
}
//===========================================================================
// ParticleSystemDSG::Display
//===========================================================================
// Description:
//      Draws the particle system
//
// Constraints:
//      None.
//
// Parameters:
//      None
//
// Return:
//      None
//
//===========================================================================
void ParticleSystemDSG::Display() 
{
#ifdef PROFILER_ENABLED
    char profileName[] = "  ParticleSystemDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    mpSystem->Display();
    DSG_END_PROFILE(profileName)
}
//===========================================================================
// ParticleSystemDSG::Update
//===========================================================================
// Description:
//      Advances effect animation
//
// Constraints:
//      None.
//
// Parameters:
//      Time to advance effect in milliseconds
//
// Return:
//      None
//
//===========================================================================

void ParticleSystemDSG::DisplayBoundingBox( tColour colour )
{
#ifndef RAD_RELEASE
	rmt::Box3D mBBox;
	GetBoundingBox( &mBBox );
	pddiPrimStream* stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.low.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.low.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.high.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.high.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.low.y, mBBox.low.z);
	p3d::pddi->EndPrims(stream);

	stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.high.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.high.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.low.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.low.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.high.y, mBBox.high.z);
	p3d::pddi->EndPrims(stream);

	stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.high.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.low.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.low.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.high.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.high.x, mBBox.high.y, mBBox.high.z);
	p3d::pddi->EndPrims(stream);

	stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.high.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.low.y, mBBox.high.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.low.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.high.y, mBBox.low.z);
	stream->Colour(colour);
	stream->Coord(mBBox.low.x, mBBox.high.y, mBBox.high.z);
	p3d::pddi->EndPrims(stream);
#endif
}
//===========================================================================
// ParticleSystemDSG::Update
//===========================================================================
// Description:
//		Advances animations and moves all particles in the world
//
// Constraints:
//      None.
//
// Parameters:
//      Time elapsed in milliseconds
//
// Return:
//      None
//
//===========================================================================
void ParticleSystemDSG::Update( float deltaTime )
{
    mpController->Advance( deltaTime );
	rmt::Matrix ident;
	ident.Identity();
	mpSystem->Update( &ident );
}
//===========================================================================
// ParticleSystemDSG::SetTransform
//===========================================================================
// Description:
//      Sets the transform matrix
//
// Constraints:
//      None.
//
// Parameters:
//      Transformation matrix
//
// Return:
//      None
//
//===========================================================================
void ParticleSystemDSG::SetTransform( const rmt::Matrix& transform )
{
    mpSystem->SetMatrix( transform );
    mPosition = transform.Row(3);
}
//===========================================================================
// ParticleSystemDSG::Reset
//===========================================================================
// Description:
//      Resets the frame controller
//
// Constraints:
//      None.
//
// Parameters:
//      None
//
// Return:
//      None
//
//===========================================================================
void ParticleSystemDSG::Reset()
{
    mpController->Reset();
}

void ParticleSystemDSG::GetPosition( rmt::Vector* ipPosn )
{
    //mpSystem->GetPosition( ipPosn );
    *ipPosn = mPosition;
}
const rmt::Vector& ParticleSystemDSG::rPosition()
{
    return mPosition;//mpSystem->GetMatrix().Row( 3 );
}
void ParticleSystemDSG::GetBoundingBox( rmt::Box3D* box )
{
	// Bounding Boxes not overloaded for tParticleSystems!!!!!!!!
	// Use a BB that is 2 m^3
	rmt::Vector position = rPosition();
	rmt::Vector low = position - rmt::Vector( 1.0f, 1.0f, 1.0f );
	rmt::Vector high = position + rmt::Vector( 1.0f, 1.0f, 1.0f );
	box->Set( low, high );	

}
void ParticleSystemDSG::GetBoundingSphere( rmt::Sphere* sphere )
{
    sphere->centre = rPosition();
    sphere->radius = 1.0f;
}

void ParticleSystemDSG::SetVelocity( const rmt::Vector& velocity )
{
    rAssert( mpSystem != NULL );
    mpSystem->SetVelocity( velocity );
}


