//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        StaticPhysDSG.cpp
//
// Description: Implementation for StaticPhysDSG class.
//
// History:     Implemented	                         --Devin [5/27/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/camera.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/utility.hpp>
#include <p3d/view.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/StaticPhysDSG.h>
#include <render/particles/particlemanager.h>
#include <render/breakables/breakablesmanager.h>
#include <render/IntersectManager/IntersectManager.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

// Bias that determines how much force is required to emit particles during a 
// collision
const float STAT_PHYS_MASS_IMPULSE_PARTICLE_BIAS = 10.0f;

//************************************************************************
//
// Public Member Functions : StaticPhysDSG Interface
//
//************************************************************************

//========================================================================
// StaticPhysDSG::
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
StaticPhysDSG::StaticPhysDSG() : 
mpShadow( NULL ),
mpShadowMatrix( NULL )
{
   mpSimStateObj = NULL;
}

//========================================================================
// StaticPhysDSG::
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
StaticPhysDSG::~StaticPhysDSG()
{
BEGIN_PROFILE( "StaticPhysDSG Destroy" );
   if(mpSimStateObj != NULL)
   {
      mpSimStateObj->Release();
   }
    if (mpShadow)
    {
        mpShadow->Release();
        mpShadow = 0;
    }
    if (mpShadowMatrix != NULL )
    {
        delete mpShadowMatrix;
        mpShadowMatrix = NULL;
    }
END_PROFILE( "StaticPhysDSG Destroy" );
}
//========================================================================
// StaticPhysDSG::
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
void StaticPhysDSG::OnSetSimState( sim::SimState* ipSimState )
{
    tRefCounted::Assign( mpSimStateObj, ipSimState );
    
    //mpSimStateObj->mAIRefIndex = StaticPhysDSG::GetAIRef();
    mpSimStateObj->mAIRefIndex = this->GetAIRef();

    SetInternalState();
}
//========================================================================
// StaticPhysDSG::
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
sim::SimState* StaticPhysDSG::GetSimState() const
{
   return mpSimStateObj;
}
///////////////////////////////////////////////////////////////////////
// Drawable
///////////////////////////////////////////////////////////////////////
//========================================================================
// StaticPhysDSG::
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
void StaticPhysDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  StaticPhysDSG Display";
#endif
   DSG_BEGIN_PROFILE(profileName)
   //Currently unsupported. Contact Devin.
   //rAssert(false);
   //Do nothing, but allow inst stat phys to render their pGeo's
   DSG_END_PROFILE(profileName)
}
//========================================================================
// StaticPhysDSG::
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
void StaticPhysDSG::DisplayBoundingBox(tColour colour)
{
#ifndef RAD_RELEASE
   //Currently unsupported. Contact Devin.
   //rAssert(false);
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
//========================================================================
// StaticPhysDSG::
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
void StaticPhysDSG::DisplayBoundingSphere(tColour colour)
{
   //Currently unsupported. Contact Devin.
   rAssert(false);
}
//========================================================================
// StaticPhysDSG::
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
void StaticPhysDSG::GetBoundingBox(rmt::Box3D* box)
{
   (*box) = mBBox;
}
//========================================================================
// StaticPhysDSG::
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
void StaticPhysDSG::GetBoundingSphere(rmt::Sphere* sphere)
{
   (*sphere) = mSphere;
}

///////////////////////////////////////////////////////////////////////
// IEntityDSG
///////////////////////////////////////////////////////////////////////
//========================================================================
// StaticPhysDSG::
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
rmt::Vector* StaticPhysDSG::pPosition()
{
   return &mPosn;
}
//========================================================================
// StaticPhysDSG::
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
const rmt::Vector& StaticPhysDSG::rPosition()
{
   return mPosn;
}
//========================================================================
// StaticPhysDSG::
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
void StaticPhysDSG::GetPosition( rmt::Vector* ipPosn )
{
   *ipPosn = mPosn;
}
//========================================================================
// StaticPhysDSG::
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



void StaticPhysDSG::RenderUpdate()
{
   //do Nothing
}


//************************************************************************
//
// Protected Member Functions : StaticPhysDSG 
//
//************************************************************************
void StaticPhysDSG::SetInternalState()
{
   mPosn = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mPosition;
   
   mBBox.low   = mPosn;
   mBBox.high  = mPosn;
   mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
   mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

   mSphere.centre.Sub(mBBox.high,mBBox.low);
   mSphere.centre *= 0.5f;
   mSphere.centre.Add(mBBox.low);
   mSphere.radius = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mSphereRadius;
}





//=============================================================================
// StaticPhysDSG::PreReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* pCollidedObj, sim::Collision& inCollision )
//
// Return:      sim
//
//=============================================================================
sim::Solving_Answer StaticPhysDSG::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{
    return sim::Solving_Continue;
}


//=============================================================================
// StaticPhysDSG::PostReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* pCollidedObj, sim::Collision& inCollision )
//
// Return:      sim
//
//=============================================================================
sim::Solving_Answer StaticPhysDSG::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{
    
    // subclass-specific shit here

    // If it is a breakable object and has an assicated particle animation with it (it should)
    // play the associated particle effect, if the impulse magnitude is greater than a certain threshold

    if ( mpCollisionAttributes != NULL )
    {
        float threshold = STAT_PHYS_MASS_IMPULSE_PARTICLE_BIAS * mpCollisionAttributes->GetMass();
        if( impulse.MagnitudeSqr() > (threshold*threshold) )
        {
            if (mpCollisionAttributes->GetParticle() != ParticleEnum::eNull )
            {
                ParticleAttributes attr;
                attr.mType = mpCollisionAttributes->GetParticle();
                GetParticleManager()->Add( attr, inCollision.GetPositionA() );          
            }
        }
    }
    return CollisionEntityDSG::PostReactToCollision(impulse, inCollision);
}



void StaticPhysDSG::SetShadow( tDrawable* ipShadow )
{
    tRefCounted::Assign( mpShadow, ipShadow );

	if ( ipShadow != NULL )
	{
		// Hang onto the shadow drawable
		rAssert( mpShadowMatrix == NULL );
		mpShadowMatrix = CreateShadowMatrix( rPosition() );
	}

}

rmt::Matrix* StaticPhysDSG::CreateShadowMatrix( const rmt::Vector& objectPosition )
{
    rmt::Matrix* pResult;
    rmt::Matrix shadowMat;
    if ( ComputeShadowMatrix( objectPosition, &shadowMat ) )
    {
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        pResult = new rmt::Matrix();
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER);
        *pResult = shadowMat;

    }
    else
    {
        pResult = NULL ;
    }
    return pResult;
}
    
void StaticPhysDSG::RecomputeShadowPosition( float height_radius_bias )
{
    if ( mpShadowMatrix )
    {
        rmt::Vector position;
        GetPosition( &position );
        ComputeShadowMatrix( position, mpShadowMatrix );
    }
}

void StaticPhysDSG::RecomputeShadowPositionNoIntersect( float height, const rmt::Vector& normal, float height_radius_bias, float scale )
{
    if ( mpShadowMatrix )
    {
        rmt::Vector position;
        GetPosition( &position );
        rmt::Vector shadowPosition( position.x, height, position.z );


	    mpShadowMatrix->Identity();
	    mpShadowMatrix->FillTranslate( shadowPosition );
        rmt::Vector worldRight( 1,0,0 );
        rmt::Vector forward;
        forward.CrossProduct( worldRight, normal );
	    mpShadowMatrix->FillHeading( forward, normal );
/*
        if ( height_radius_bias != 1.0f )
        {
            // scale = (objheight - groundY) * bias
            float matrixScale = 1.0f - ( position.y - shadowPosition.y ) * height_radius_bias;
            matrixScale *= scale;
            if ( matrixScale < 0.0f )
            {
                matrixScale = 0.0f;
            }
            mpShadowMatrix->FillScale( matrixScale );
        }*/
    }
}

void StaticPhysDSG::DisplaySimpleShadow()
{
    p3d::pddi->SetZWrite(false);
    BEGIN_PROFILE("DisplaySimpleShadow")
	rAssert( mpShadow != NULL );

	// Translate the shadow towards the camera slightly, instead of moving it off the
	//ground in the direction of the the ground normal. Hopefully this will cause less distortion of the shadow.
	
    if ( mpShadow != NULL && mpShadowMatrix != NULL )
    {

	// Create a camera that pushes the shadow a meter towards
	// the camera
	rmt::Vector camPos;
	p3d::context->GetView()->GetCamera()->GetWorldPosition( &camPos );
	camPos.Sub( mpShadowMatrix->Row(3) );
	camPos.Normalize();
    // Distance to raise the object 
    const float Z_FIGHTING_OFFSET = 1.0f;
    camPos.Scale( Z_FIGHTING_OFFSET );
	
	// Final shadow transform = position/orientation * tocamera translation
	rmt::Matrix shadowTransform( *mpShadowMatrix );
	shadowTransform.Row( 3 ).Add( camPos );

	// Display
	p3d::stack->PushMultiply( shadowTransform );
	mpShadow->Display();
    p3d::stack->Pop();
    }
    else
    {
        mpShadowMatrix = CreateShadowMatrix( rPosition() );
    }
    END_PROFILE("DisplaySimpleShadow")
    p3d::pddi->SetZWrite(true);
}


//************************************************************************
//
// Private Member Functions : StaticPhysDSG 
//
//************************************************************************


bool 
StaticPhysDSG::ComputeShadowMatrix( const rmt::Vector& in_position, rmt::Matrix* out_pMatrix )
{
   	// Determine where our shadow casting object intersects the ground plane
	rmt::Vector groundNormal(0,1,0);
	rmt::Vector groundPlaneIntersectionPoint;

	const float INTERSECT_TEST_RADIUS = 10.0f;
	bool foundPlane;
	rmt::Vector deepestIntersectPos, deepestIntersectNormal;

    // Get rid of the fact that FindIntersection doesn't want a const value
	// and I'm above casting away constness

    rmt::Vector searchPosition = in_position;
    searchPosition.y += 10.0f;

	GetIntersectManager()->FindIntersection( searchPosition, 
											foundPlane,
											groundNormal,
											groundPlaneIntersectionPoint );

    if ( foundPlane )
    {
	    out_pMatrix->Identity();
	    out_pMatrix->FillTranslate( groundPlaneIntersectionPoint );
        rmt::Vector worldRight( 1,0,0 );
        rmt::Vector forward;
        forward.CrossProduct( worldRight, groundNormal );
	    out_pMatrix->FillHeading( forward, groundNormal );

    }
    return foundPlane;
    
}

