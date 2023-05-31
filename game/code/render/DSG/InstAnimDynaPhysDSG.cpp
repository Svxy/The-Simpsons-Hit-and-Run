//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstAnimDynaPhysDSG
//
// Description: An InstDynaPhysDSG object that has a simple cyclic animation
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================
#include <memory/classsizetracker.h>
#include <render/DSG/InstAnimDynaPhysDSG.h>
#include <p3d/anim/compositedrawable.hpp>
#include <render/AnimEntityDSGManager/AnimEntityDSGManager.h>
#include <p3d/anim/pose.hpp>
#include <p3d/anim/poseanimation.hpp>
#include <p3d/anim/animate.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <poser/poseengine.hpp>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/Culling/WorldScene.h>
#include <ai/actionbuttonhandler.h>
#include <stateprop/statepropdata.hpp>
#include <p3d/billboardobject.hpp>

//#include <worldsim/worldphysicsmanager.h>
#include <worldsim/character/character.h>

#include <render/particles/particlemanager.h>
#include <render/breakables/breakablesmanager.h>


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
// InstAnimDynaPhysDSG::InstAnimDynaPhysDSG
//===========================================================================
// Description:
//      InstAnimDynaPhysDSG ctor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

InstAnimDynaPhysDSG::InstAnimDynaPhysDSG()
: mpCompDraw( NULL ),
mpMultiController( NULL ),
mpActionButton( NULL )
{
    CLASSTRACKER_CREATE( InstAnimDynaPhysDSG );
}
//===========================================================================
// InstAnimDynaPhysDSG::~InstAnimDynaPhysDSG
//===========================================================================
// Description:
//      InstAnimDynaPhysDSG dtor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

InstAnimDynaPhysDSG::~InstAnimDynaPhysDSG()
{
    CLASSTRACKER_DESTROY( InstAnimDynaPhysDSG );
	if ( mpCompDraw != NULL )
	{
		mpCompDraw->Release();
		mpCompDraw = NULL;
	}
	if ( mpMultiController != NULL )
	{
		GetAnimEntityDSGManager()->Remove( mpMultiController );

		mpMultiController->Release();
		mpMultiController = NULL;
	}
    if ( mpActionButton != NULL )
    {
        // We don't addref this mpactionbutton (though perhaps we should)
        // The actionbuttonhandler owns this DSG object, if we addrefed the button
        // we would have problems trying to destroy both if the kept references to
        // each other and only released in the dtors.
        // Wierd Travis-inherited code and an annoying cyclic dependency. 
        mpActionButton = NULL;
    }
    if ( mpPose != NULL )
    {
        mpPose->Release();
        mpPose = NULL ;
    }
}

//===========================================================================
// InstAnimDynaPhysDSG::Display
//===========================================================================
// Description:
//      Draws the object
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

void InstAnimDynaPhysDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  InstAnimDynaPhysDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    if(CastsShadow()) 
    {
        BillboardQuadManager::Enable();
    }

	p3d::pddi->PushMultMatrix(PDDI_MATRIX_MODELVIEW, &mMatrix);
	mpCompDraw->Display();
	p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);

    if(CastsShadow())
    {
        BillboardQuadManager::Disable();
        DisplaySimpleShadow();
    }
    DSG_END_PROFILE(profileName)
}
//===========================================================================
// InstAnimDynaPhysDSG::DisplayBoundingBox
//===========================================================================
// Description:
//      Draws the object's bounding box
//
// Constraints:
//		Slow as hell
//
// Parameters:
//
// Return:
//
//===========================================================================

void InstAnimDynaPhysDSG::DisplayBoundingBox( tColour colour )
{
#ifndef RAD_RELEASE
	p3d::pddi->PushMatrix(PDDI_MATRIX_MODELVIEW);
	p3d::pddi->MultMatrix(PDDI_MATRIX_MODELVIEW,&mMatrix);


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

	p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);
#endif
}

//===========================================================================
// InstAnimDynaPhysDSG::GetBoundingBox
//===========================================================================
// Description:
//		Fills out the given bounding box pointer
//
// Constraints:
//
//
// Parameters:
//
// Return:
//
//===========================================================================
/*
void InstAnimDynaPhysDSG::GetBoundingBox(rmt::Box3D* box)
{
	rAssert( box != NULL );
    (box->low).Add(mBBox.low, mPosn);
    (box->high).Add(mBBox.high, mPosn);

}*/

//===========================================================================
// InstAnimDynaPhysDSG::GetBoundingSphere
//===========================================================================
// Description:
//		Fills out the given bounding sphere pointer
//
// Constraints:
//
//
// Parameters:
//
// Return:
//
//===========================================================================
/*
void InstAnimDynaPhysDSG::GetBoundingSphere( rmt::Sphere* pSphere )
{
    (pSphere->centre).Add(mSphere.centre, mPosn);
    pSphere->radius = mSphere.radius;
}*/

//===========================================================================
// InstAnimDynaPhysDSG::LoadSetUp
//===========================================================================
// Description:
//		Sets up internal state. Required before any use
//
// Constraints:
//
//
// Parameters:
//
// Return:
//
//===========================================================================


void InstAnimDynaPhysDSG::LoadSetUp(	CollisionAttributes*  ipCollAttr,
										const rmt::Matrix&    iMatrix, 
										tCompositeDrawable*	  ipCompDrawable,
										tMultiController*	  ipMultiController,
										tEntityStore*		  ipSearchStore )
{

	rAssert( ipCollAttr != NULL );
	rAssert( ipCompDrawable != NULL );
	rAssert( ipMultiController != NULL );

	// Clone the composite drawable, and most importantly, clone its pose. We want a 
	// seperate pose for each composite drawable.
	mpCompDraw = ipCompDrawable->Clone();
	mpCompDraw->SetPose( ipCompDrawable->GetPose () );
	
	//	mpCompDraw = ipCompDrawable;
	
	mpCompDraw->AddRef();


	mpMultiController = ipMultiController;
	mpMultiController->AddRef();
	GetAnimEntityDSGManager()->Add( mpMultiController );

	mMatrix = iMatrix;
	// Setup the physics
    tPose* p3dPose = mpCompDraw->GetPose();
    p3dPose->Evaluate();

	// the sim library doesn't have RTTI enabled. When I upcast this, there will be no RTTI check, if
	// someone changes this to something other than SimStateArticulated, make absolutely sure that everything changes in this file
	// I can only imagine what horrific errors will result from a bad static_cast downcast.

	// The current sim library doesn't appear to dump anything in the store, it only uses
	// it to find the collision and physics objects out of it using the name as the parameter to p3d::find<>
	mpPose = new poser::Pose( p3dPose );
    mpPose->AddRef();

    // The sim library does in fact add something to the store, the above comment notwithstanding.
    // So we wrap this call in a PushSection/PopSection.
    //
    p3d::inventory->PushSection( );
    p3d::inventory->SelectSection( "Default" );

	sim::SimState* pSimState = sim::SimStateArticulated::CreateSimStateArticulated( mpCompDraw->GetUID(),mpPose, sim::SimStateAttribute_Default, ipSearchStore );
    if ( pSimState != NULL )
    {
	    // Convert the articulated object into a rigid body, requiring fewer CPU computations
	    // and avoiding a lot of the nonsense involved with bounding box sub-volumes
	    sim::SimStateArticulated* pArty = static_cast< sim::SimStateArticulated* > ( pSimState );
        // Sanity check on type
        rAssert( dynamic_cast< sim::SimStateArticulated* > (pSimState) != NULL );
        pArty->ConvertToRigidBody();
	    rAssertMsg( pArty->ConvertedToRigidBody(), "This articulated object cannot be converted to a rigid body, aborting."  );
    }
    else
    {
        pSimState = sim::SimState::CreateSimState( mpCompDraw->GetUID(), sim::SimStateAttribute_Default, ipSearchStore );
        rAssertMsg( pSimState != NULL, "Cannot load object as either a simstate or simstatearticulated!" );
    }

    p3d::inventory->PopSection( );

	pSimState->SetControl(sim::simAICtrl);
    pSimState->SetTransform(mMatrix);   



	pSimState->GetCollisionObject()->GetCollisionVolume()->UpdateAll();
    // Needed for SetSimState to work because of tRefCounted::Assign
    pSimState->AddRef();
	
    mpPhysObj = (sim::PhysicsObject*)(pSimState->GetSimulatedObject());
	pSimState->mAIRefIndex = PhysicsAIRef::redBrickPhizMoveableAnim;

    SetCollisionAttributes(ipCollAttr);

    pSimState->mAIRefPointer = this;
    SetSimState( pSimState );
    mpCompDraw->ProcessShaders(*this);

    pSimState->Release();

    
}
bool InstAnimDynaPhysDSG::Break()
{
    bool success;
    success = InstDynaPhysDSG::Break();
    if ( mpActionButton != NULL )
    {
        mpActionButton->GameObjectDestroyed();
    }
    return success;
}

void InstAnimDynaPhysDSG::Update( float deltaTime )
{

    // If an action button is tied to this thing, update it
	if ( mpActionButton )
	{
		//mpActionButton->Update( deltaTime );
	}
     mpPhysObj->Update( deltaTime );    
    
	sim::CollisionObject* collObj = mpSimStateObj->GetCollisionObject();
    if ( collObj->HasMoved( ) || collObj->HasRelocated( )  )
    {
        collObj->Update();
		// Save the old matrix
        rmt::Box3D oldBox;
		//GetBoundingBox( &oldBox );
        (oldBox.low).Add(mBBox.low, mPosn);
        (oldBox.high).Add(mBBox.high, mPosn);


		// Bounding box automatically changes when mPosn changes
        mMatrix = mpSimStateObj->GetTransform();
        mPosn = mpSimStateObj->GetPosition();
		// Move the object in the DSG
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mRenderLayer ));
        // Sanity check
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
        pWorldRenderLayer->pWorldScene()->Move( oldBox, this );   
	}    
}




void InstAnimDynaPhysDSG::SetTransform( const rmt::Matrix& transform )
{
	mMatrix = transform;
	// Matrix is reset, recompute the bounding box and move it
	// in the DSG( Warning, this assumes that the user actually has it IN
	// the DSG. This was always the case for DSG objects so lets not break 
	// tradition
	rmt::Box3D oldBox;
	GetBoundingBox( &oldBox );

	// Bounding box relies on mPosn, lets update it 
	mPosn = transform.Row(3);

    WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mRenderLayer ));
    // Sanity check
    rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
    pWorldRenderLayer->pWorldScene()->Move( oldBox, this );   

	// Finally, tell physics what our new transform matrix 
	mpSimStateObj->SetTransform( transform );
	mpSimStateObj->GetCollisionObject()->SetHasRelocated( true );
	mpSimStateObj->GetCollisionObject()->SetHasMoved( true );
	mpSimStateObj->GetCollisionObject()->Update();

    sim::CollisionObject* collObj = mpSimStateObj->GetCollisionObject();

}

void InstAnimDynaPhysDSG::SetAction( ActionButton::AnimSwitch* pActionButton )
{
    // Don't increment refcount. TBJ's animcolldsg does increment the refcount, however
    // the bee cameras are in essence owned by the actionbuttonhandler thats calling
    // SetAction. Refcounting in the reverse direction will cause major problems when the 
    // ActionButtonManager tries to kill the Handler, which tries to kill the DSG object
    mpActionButton = pActionButton;
}

AnimDynaPhysWrapper::AnimDynaPhysWrapper():
mCompDraw( NULL ),
mMultiController( NULL ),
mPhysicsObject( NULL ),
mCollisionObject( NULL ),
mHasAlpha( false ),
mStatePropData( NULL ),
mHasAnimations( false )
{

}

AnimDynaPhysWrapper::~AnimDynaPhysWrapper()
{
	if ( mCompDraw != NULL )
	{
		mCompDraw->Release();
		mCompDraw = NULL;
	}
	if ( mMultiController != NULL )
	{
		mMultiController->Release();
		mMultiController = NULL;
	}
	if ( mPhysicsObject != NULL )
	{
		mPhysicsObject->Release();
		mPhysicsObject = NULL;
	}
	if ( mCollisionObject != NULL )
	{
		mCollisionObject->Release();
		mCollisionObject = NULL;
	}
    if ( mStatePropData != NULL )
    {
        mStatePropData->Release();
        mStatePropData = NULL;
    }
}

float AnimDynaPhysWrapper::GetVolume()const
{
	rAssert( mPhysicsObject != NULL );
    
#ifndef FINAL
    if (mPhysicsObject == NULL)
    {
        char outbuffer [255];
        sprintf(outbuffer,"Error: %s is missing a Dynamic Bounding Volume \n",this->GetName()); 
        rTuneAssertMsg( 0,outbuffer );
    }
#endif

	return mPhysicsObject->GetVolume();    
}

tCompositeDrawable* AnimDynaPhysWrapper::GetDrawable()const
{
	return mCompDraw;
}

tMultiController* AnimDynaPhysWrapper::GetController()const
{
	return mMultiController;
}

CStatePropData* AnimDynaPhysWrapper::GetStatePropData()const
{
    return mStatePropData;
}


bool AnimDynaPhysWrapper::HasAlpha()const
{
	return mHasAlpha;
}

InstAnimDynaPhysDSG* AnimDynaPhysWrapper::CreateDSGObject( CollisionAttributes *pAttr, const rmt::Matrix& transform, GameMemoryAllocator heap )
{
	// allocate a new dynaphys object
	InstAnimDynaPhysDSG* pDSG = new (heap) InstAnimDynaPhysDSG;	

	// set it up via loadsetup
	pDSG->LoadSetUp( pAttr, transform, 
					 mCompDraw, mMultiController, NULL ); 

	pDSG->mTranslucent = true;

	return pDSG;
}




