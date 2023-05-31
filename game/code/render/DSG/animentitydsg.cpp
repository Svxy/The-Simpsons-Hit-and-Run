//===========================================================================
// Copyright (C) 1999 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   animentitydsg
//
// Description: Animated object without collision detection for use in Devin's 
//              scene graph
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/dsg/animentitydsg.h>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/animate.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <render/RenderManager/RenderManager.h>
#include <render/Culling/WorldScene.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/IntersectManager/IntersectManager.h>
#include <p3d/effects/particlesystem.hpp>
#include <main/game.h>
#include <mission/gameplaymanager.h>

#include <radtime.hpp>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const int MAX_NUM_PARTICLE_SYSTEMS = 10;

static unsigned sUpdateIndex = 0;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// AnimEntityDSG::AnimEntityDSG
//===========================================================================
// Description:
//      AnimEntityDSG constructor
//
// Constraints:
//      None.
//
// Parameters:
//      None.
//
// Return:
//      None.
//
//===========================================================================

AnimEntityDSG::AnimEntityDSG()
:
mbAddToUpdateList( true ),
mTrackSeparately( 0 ),
mpDrawable( NULL ),
mpMultiController( NULL ),
mIsVisible( true ),
mIsAnimationPlaying( true ),
mRenderLayer( RenderEnums::LevelSlot ),
mParticleSystemBoundingBox( NULL ),
m_TimeLastParticleUpdate(0),
mTimeSinceLastUpdate(0.0f),
updateIndex(sUpdateIndex++)
{
    mEffectElements.Allocate( MAX_NUM_PARTICLE_SYSTEMS );
    
}

//===========================================================================
// AnimEntityDSG::~AnimEntityDSG
//===========================================================================
// Description:
//      AnimEntityDSG destructor
//
// Constraints:
//      None.
//
// Parameters:
//      None.
//
// Return:
//      None.
//
//===========================================================================

AnimEntityDSG::~AnimEntityDSG()
{
BEGIN_PROFILE( "Anim Destroy" );
    if (mpDrawable != NULL)
    {
        mpDrawable->Release();   
        mpDrawable = NULL;
    }
    if (mpMultiController != NULL)
    {
        mpMultiController->Release();   
        mpMultiController = NULL;
    }
    if ( mParticleSystemBoundingBox != NULL )
    {
        delete mParticleSystemBoundingBox;
        mParticleSystemBoundingBox = NULL;
    }

END_PROFILE( "Anim Destroy" );

}
//===========================================================================
// AnimEntityDSG::Display
//===========================================================================
// Description:
//      Draws the composite drawable object
//
// Constraints:
//      None.
//
// Parameters:
//      None.
//
// Return:
//      None.
//
//===========================================================================

void AnimEntityDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  AnimEntityDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    rAssert( mpDrawable != NULL);

	if ( mIsVisible )
	{
        p3d::pddi->PushMultMatrix(PDDI_MATRIX_MODELVIEW, &mTransformOffset);
        mpDrawable->Display();
        p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);
	}

    DSG_END_PROFILE(profileName)
}

void AnimEntityDSG::Reset()
{    
    mpMultiController->Reset();
}

//===========================================================================
// AnimEntityDSG::pPosition
//===========================================================================
// Description:
//      Returns a mutable pointer to the position of the entity
//
// Constraints:
//      Breaks encapsulation? 
//
// Parameters:
//      None.
//
// Return:
//      non-const (!) pointer to the position vector in mTransform
//
//===========================================================================

rmt::Vector* AnimEntityDSG::pPosition()
{
    return (rmt::Vector*)(mTransformOffset.m[3]);   
}
//===========================================================================
// AnimEntityDSG::rPosition
//===========================================================================
// Description:
//      Returns the position of the entity
//
// Constraints:
//
// Parameters:
//      None.
//
// Return:
//      Const reference to the entity position
//
//===========================================================================

const rmt::Vector& AnimEntityDSG::rPosition()
{
    return mTransformOffset.Row( 3 );     
}
//===========================================================================
// AnimEntityDSG::GetPosition
//===========================================================================
// Description:
//      
//
// Constraints:
//      
//
// Parameters:
//      None.
//
// Return:
//      None.
//
//===========================================================================

void AnimEntityDSG::GetPosition( rmt::Vector* ipPosn )
{
    *ipPosn = *(rmt::Vector*)(mTransformOffset.m[3]);
    ipPosn->Add(*(rmt::Vector*)(mTransform.m[3]));
}
//========================================================================
// animentitydsg::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void AnimEntityDSG::SetAnimRootHeadingYUp( rmt::Vector& irHeading )
{
    mTransformOffset.FillHeading( irHeading, rmt::Vector(0.0f,1.0f,0.0f));
}

void AnimEntityDSG::GetAnimRootHeading( rmt::Vector& orHeading )
{
    mTransformOffset.GetHeading(orHeading);
}

void AnimEntityDSG::SetPosition( rmt::Vector& irNewPosition )
{
    //mTransformOffset.Identity();
    mTransformOffset.FillTranslate( irNewPosition );
}

//===========================================================================
// AnimEntityDSG::Update
//===========================================================================
// Description:
//      Advances animation and retrieves the new transform matrix
//
// Constraints:
//      
//
// Parameters:
//      Time elapsed in seconds (animcollisionentitydsg::update uses seconds)
//
// Return:
//      None.
//
//===========================================================================


void AnimEntityDSG::Update( float timeElapsedSec )
{
    bool doUpdate = true;
    rmt::Sphere sphere;
    
    GetBoundingSphere(&sphere);

    bool vis = GetGameplayManager()->TestPosInFrustrumOfPlayer(sphere.centre, 0, sphere.radius);

    doUpdate = vis || ((GetGame()->GetFrameCount() % 7) == (updateIndex % 7));
    
    if(mIsAnimationPlaying)
    {
        mTimeSinceLastUpdate += timeElapsedSec;
    }

	if ( mIsAnimationPlaying && doUpdate)
	{
		// Save the bounding box information before the animation
		//
		rmt::Box3D oldBox;
		GetBoundingBox( &oldBox );

		// advance the animation and update transform!
		//

		float timeElapsedMS = mTimeSinceLastUpdate * 1000.0f;

		mpMultiController->Advance( timeElapsedMS );        
		mTransform = mpDrawable->GetPose()->GetJoint( 0 )->objectMatrix;

        // Move the object in the scenegraph.
        //
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mRenderLayer ));
        // Sanity check
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
        rAssert( mRenderLayer != RenderEnums::numLayers );
        pWorldRenderLayer->pWorldScene()->Move( oldBox, this );   
        mTimeSinceLastUpdate = 0.0f;
    }
}




//===========================================================================
// AnimEntityDSG::LoadSetUp
//===========================================================================
// Description:
//      Initializes animation, frame controller and drawable object and
//      the transform matrix
//
// Constraints:
//      
//
// Parameters:
//      Valid tCompositeDrawable and tAnimationFrameController objects
//
// Return:
//      None.
//
//===========================================================================

void AnimEntityDSG::LoadSetUp( tCompositeDrawable* pDrawable, tMultiController* pMultiController, tEntityStore* store, rmt::Vector& irPosition )
{
    mpDrawable = pDrawable;
    mpMultiController = pMultiController;

    char outputbuffer [255];

    if (mpDrawable == NULL)
    {
        sprintf(outputbuffer,"%s is missing a drawable \n",this->GetName());
        rTuneAssertMsg( mpDrawable != NULL,outputbuffer );
    }
    
    if( mpMultiController == NULL)
    {
        sprintf(outputbuffer,"%s is missing a Multicontroller \n",this->GetName());
        rTuneAssertMsg( pMultiController !=NULL, outputbuffer);
    }

    mpDrawable->AddRef();
    pMultiController->AddRef();
	pMultiController->Reset();
	pMultiController->SetFrame(0.0f);


    mTransform.Identity();

    mTransformOffset.Identity();
    mTransformOffset.FillTranslate( irPosition );

   
    tPose* pPose = mpDrawable->GetPose();
    pPose->Evaluate();
    mTransform = pPose->GetJoint( 0 )->worldMatrix;
    
    mpDrawable->ProcessShaders(*this);

	//      Whether or not the animentity is cyclic or not.
	//		If any of the animations referenced are not cyclic, then assume the animation
	//		is non-cyclic


    FindEffectElements( mpDrawable );
    rmt::Box3D particlebox;
    bool particleBBFound = GetParticleSystemBoundingBox( mpDrawable, &particlebox );
    if ( particleBBFound )
    {
        delete mParticleSystemBoundingBox;
        mParticleSystemBoundingBox = new rmt::Box3D;
        *mParticleSystemBoundingBox = particlebox;  
    }   
}


//===========================================================================
// AnimEntityDSG::GetBoundingBox
//===========================================================================
// Description:
//     
//
// Constraints:
//      
//
// Parameters:
//      pointer to valid rmt::Box3D that will be filled with the box info
//
// Return:
//      None.
//
//===========================================================================

void AnimEntityDSG::GetBoundingBox( rmt::Box3D* box )
{

    mpDrawable->GetBoundingBox( &mBoundingBox );
    

    //mTransformOffset.Transform( mBoundingBox.low,  &mBoundingBox.low );
    //mTransformOffset.Transform( mBoundingBox.high, &mBoundingBox.high );
    //Only mod the box with translation; don't want to have to move the box in
    //the tree for every rotation
    mBoundingBox.low.Add( *(rmt::Vector*)(mTransformOffset.m[3]) );
    mBoundingBox.high.Add( *(rmt::Vector*)(mTransformOffset.m[3]) );


    if ( mParticleSystemBoundingBox )
    {
        mParticleSystemBoundingBox->low.Add( *(rmt::Vector*)(mTransformOffset.m[3]) );
        mParticleSystemBoundingBox->high.Add( *(rmt::Vector*)(mTransformOffset.m[3]) );
        mBoundingBox.Expand( mParticleSystemBoundingBox->low );
        mBoundingBox.Expand( mParticleSystemBoundingBox->high );
    }

    *box = mBoundingBox;
}
//===========================================================================
// AnimEntityDSG::GetBoundingSphere
//===========================================================================
// Description:
//      Gets the bounding sphere
//
// Constraints:
//      
//
// Parameters:
//      pointer to valid rmt::Sphere that will be filled with the sphere info
//
// Return:
//      None.
//
//===========================================================================

void AnimEntityDSG::GetBoundingSphere( rmt::Sphere* sphere )
{/*
    mpDrawable->GetBoundingSphere( &mBoundingSphere );

    //mTransformOffset.Transform( mBoundingSphere.centre,  &mBoundingSphere.centre );
    //Only mod the box with translation; don't want to have to move the box in
    //the tree for every rotation
    mBoundingSphere.centre.Add( *(rmt::Vector*)(mTransformOffset.m[3]) );

   // rmt::Sphere particleSphere;
   // mParticleSystemBoundingBox.GetBoundingSphere( &particleSphere );
    *sphere = mBoundingSphere;*/

    rmt::Box3D box;
    GetBoundingBox( &box );
    *sphere = box.GetBoundingSphere();
}
//===========================================================================
// AnimEntityDSG::PlaceOnGround
//===========================================================================
// Description:
//      Moves object on the ground
//
// Constraints:
//      
//
// Parameters:
//      float heightAboveGround ( typically z fighting offset )
//
// Return:
//      None.
//
//===========================================================================
bool AnimEntityDSG::PlaceOnGround( float heightAboveGround, bool ibMoveOnFail )
{

    bool foundPlane;
    rmt::Vector searchPosn = rPosition();
    searchPosn.y += 100.0f;
    rmt::Vector groundPlaneNormal, groundPlanePosn;
    GetIntersectManager()->FindIntersection( searchPosn, foundPlane, groundPlaneNormal, groundPlanePosn );
    if ( foundPlane )
    {
        rmt::Vector newPosition = groundPlanePosn;
        newPosition.y += heightAboveGround;
        SetPosition( newPosition );
    }
    else if( ibMoveOnFail )
    {
        rmt::Vector newPosition = rPosition();
        newPosition.y += heightAboveGround;
        SetPosition( newPosition );        
    }
   
    return foundPlane;
}


// Searches the given drawable and inits the mEffectElements array
void AnimEntityDSG::FindEffectElements( tCompositeDrawable* drawable )
{
    mEffectElements.ClearUse();
    rAssert( drawable != NULL );
    // Iterate through the elements, search for effect elements
    // which get stuffed into the array
    for ( int i = 0 ; i < drawable->GetNumDrawableElement() ; i++ )
    {
        tCompositeDrawable::DrawableElement* element = drawable->GetDrawableElement( i );
        // Only insert effect elements, the others dont contain particle systems
        if ( element->GetType() == tCompositeDrawable::DrawableElement::EFFECT_ELEMENT )
        {
            // Better do a cast check
            tCompositeDrawable::DrawableEffectElement* effectElement = static_cast< tCompositeDrawable::DrawableEffectElement* >( element );
            rAssert( dynamic_cast< tCompositeDrawable::DrawableEffectElement* >(element) != NULL );

            mEffectElements.Add( effectElement );

            effectElement->SetPose( drawable->GetPose() );
        }
    }
}

bool AnimEntityDSG::GetParticleSystemBoundingBox( tCompositeDrawable* drawable, rmt::Box3D* out_box )
{
    rmt::Box3D rootBox;
    rAssert( drawable != NULL );

    drawable->GetPose()->Evaluate();

    bool boxFound = false;
    // Lets find the particle systems
    for ( int i = 0 ; i < drawable->GetNumDrawableElement() ; i++ )
    {
        tCompositeDrawable::DrawableElement* element = drawable->GetDrawableElement( i );
        if ( element )
        {
            if ( element->GetType() == tCompositeDrawable::DrawableElement::EFFECT_ELEMENT )
            {
                // Its a tEffect
                tParticleSystem* particleSystem = static_cast< tParticleSystem* >( element->GetDrawable() );
                rAssert( dynamic_cast< tParticleSystem* >( element->GetDrawable() ) );
                if ( particleSystem )
                {
                    rmt::Box3D subbox;
                    particleSystem->ComputePreciseBoundingBox( &subbox );
                    const rmt::Matrix* worldMatrix = element->GetWorldMatrix();
                    subbox.low.Add( worldMatrix->Row(3) );
                    subbox.high.Add( worldMatrix->Row(3) );

                    rootBox.Expand( subbox.low );
                    rootBox.Expand( subbox.high );

                    boxFound = true;
                }
            }
        }
    }
    *out_box = rootBox;
    return boxFound;
}   


