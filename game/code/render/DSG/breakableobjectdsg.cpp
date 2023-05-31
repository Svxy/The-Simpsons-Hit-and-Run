//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   BreakableObjectDSG
//
// Description: Breakleobjects
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/DSG/breakableobjectdsg.h>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/matrixstack.hpp>
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
// BreakableObjectDSG::BreakableObjectDSG
//===========================================================================
// Description:
//		A breakable DSG object's ctor
//
// Constraints:
//		Most other DSG objects expect a preallocated matrix to be placed handed 
//		to the object on loadsetup. This class allocates its own matrix here
//		mTranslucent flag set to true. Probably a pretty safe assumption, since 
//		breakables would have to fade out to look decent
//
// Parameters:
//
//
// Return:
//
//
//===========================================================================

BreakableObjectDSG::BreakableObjectDSG()
: mpObject( NULL ),
mpController( NULL )
{
	IEntityDSG::mTranslucent = true;
    mpMatrix = new rmt::Matrix;   
    mpMatrix->Identity();
}

//===========================================================================
// BreakableObjectDSG::~BreakableObjectDSG
//===========================================================================
// Description:
//		A breakable DSG object's dtor
//
// Constraints:
//		
//	
// Parameters:
//
//
// Return:
//
//
//===========================================================================


BreakableObjectDSG::~BreakableObjectDSG()
{
BEGIN_PROFILE( "BreakableObjectDSG Destroy" );
    // The controller should also release the attached effect, use the debugger to 
    // make sure this happens!
    if (mpController != NULL)
    {
        mpController->Release();
        mpController = NULL;
    }

    delete mpMatrix;
    mpMatrix = NULL;
END_PROFILE( "BreakableObjectDSG Destroy" );
}
//===========================================================================
// BreakableObjectDSG::Init
//===========================================================================
// Description:
//		Create a breakable from a tAnimatedObject via cloning
//
// Constraints:
//		Only call it once
//	
// Parameters:
//
//
// Return:
//
//
//===========================================================================

void BreakableObjectDSG::Init( tAnimatedObjectFactory* pFactory, tAnimatedObjectFrameController* pController)
{
    rAssert( pFactory != NULL );
    rAssert( pController != NULL );

    rAssert( mpController == NULL );
    rAssert( mpObject == NULL );

    // Clone a new frame controller off the given controller
    mpController = static_cast<tAnimatedObjectFrameController*> (pController->Clone() );
    // Check that the clone and downcast both worked correctly
    rAssert( mpController != NULL );
          
    // Clone a new animated object from the factory
    mpObject = pFactory->CreateObject( mpController );

    mpObject->ProcessShaders(*this);
}
//===========================================================================
// BreakableObjectDSG::Display
//===========================================================================
// Description:
//		Draw the breakable
//
// Constraints:
//		
//	
// Parameters:
//
//
// Return:
//
//
//===========================================================================

void BreakableObjectDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  BreakableObjectDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    p3d::stack->PushMultiply( *mpMatrix );
    mpObject->Display(); 
    p3d::stack->Pop();
    DSG_END_PROFILE(profileName)
}

//===========================================================================
// BreakableObjectDSG::GetBoundingBox
//===========================================================================
// Description:
//		Fetches the bounding box
//
// Constraints:
//		Its not accurate!
//	
// Parameters:
//
//
// Return:
//
//
//===========================================================================

void BreakableObjectDSG::GetBoundingBox( rmt::Box3D* box )
{

    mpObject->GetBaseObject()->GetBoundingBox( box );
    // Lets inflate the bounding box, as particle systems do not have accurate ones
    rmt::Vector inflate( 5.0f,5.0f,5.0f );
	box->low += ( rPosition() - inflate );
	box->high += ( rPosition() + inflate );

}
void BreakableObjectDSG::GetBoundingSphere( rmt::Sphere* pSphere )
{
    mpObject->GetBaseObject()->GetBoundingSphere( pSphere );
    pSphere->centre += rPosition();
    // Lets inflate the bounding sphere, as particle systems do not have accurate ones
    pSphere->radius += 5.0f;
}


//===========================================================================
// BreakableObjectDSG::Update
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
void BreakableObjectDSG::Update( float deltaTime )
{
    mpController->Advance( deltaTime, true );
}
rmt::Vector* BreakableObjectDSG::pPosition()
{
    rAssert( 0 ); // no breaking encapsulation!
    return &mPosn;
}
const rmt::Vector& BreakableObjectDSG::rPosition()
{
    return mpMatrix->Row( 3 );
}
void BreakableObjectDSG::GetPosition( rmt::Vector* ipPosn )
{
    *ipPosn = mpMatrix->Row( 3 );
}
void BreakableObjectDSG::SetTransform( const rmt::Matrix& transform )
{
    *mpMatrix = transform;
}
void BreakableObjectDSG::Reset()
{
    mpController->Reset();
}
int BreakableObjectDSG::LastFrameReached()
{
    return mpController->LastFrameReached();
}