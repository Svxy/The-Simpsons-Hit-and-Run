//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InstStatEntityDSG.cpp
//
// Description: Implementation for InstStatEntityDSG class.
//
// History:     Implemented	                         --Devin [6/17/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <p3d/utility.hpp>
#include <render/DSG/InstStatEntityDSG.h>
#include <memory/srrmemory.h>
#include <render/IntersectManager/IntersectManager.h>
#include <p3d/matrixstack.hpp>
#include <p3d/view.hpp>
#include <p3d/camera.hpp>
#include <p3d/billboardobject.hpp>
//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : InstStatEntityDSG Interface
//
//************************************************************************
//========================================================================
// InstStatEntityDSG::
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
InstStatEntityDSG::InstStatEntityDSG()
: mpMatrix( NULL ),
mpShadowDrawable( NULL ),
mpShadowMatrix( NULL )
{
}
//========================================================================
// InstStatEntityDSG::
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
InstStatEntityDSG::~InstStatEntityDSG()
{
BEGIN_PROFILE( "InstStatEntityDSG Destroy" );
    if(mpMatrix)
    {
        delete mpMatrix;
    }
    if ( mpShadowDrawable )
    {
        mpShadowDrawable->Release();
        mpShadowDrawable = NULL;
    }
    if ( mpShadowMatrix )
    {
        delete mpShadowMatrix;
    }

END_PROFILE( "InstStatEntityDSG Destroy" );
}
//========================================================================
// InstStatEntityDSG::
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

void InstStatEntityDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  InstStatEntityDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    if(CastsShadow())
    {
        BillboardQuadManager::Enable();
    }
    p3d::pddi->PushMultMatrix(PDDI_MATRIX_MODELVIEW, mpMatrix);
        mpDrawstuff->Display();
    p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);

    if(CastsShadow())
    {
        BillboardQuadManager::Disable();
        DisplaySimpleShadow();
    }
    
    DSG_END_PROFILE(profileName)
}

//========================================================================
// InstStatEntityDSG::DisplaySimpleShadow
//========================================================================
//
// Description: Draws the given shadow drawable after translation by the 
//              shadow matrix
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================

void InstStatEntityDSG::DisplaySimpleShadow()
{
    BEGIN_PROFILE("DisplaySimpleShadow")
    p3d::pddi->SetZWrite(false);
    if ( mpShadowDrawable != NULL  )
    {
        if ( mpShadowMatrix == NULL )
        {
            mpShadowMatrix = CreateShadowMatrix( mpMatrix->Row( 3 ) );
        }
        if ( mpShadowMatrix )
        {

        	// Create a camera that pushes the shadow a meter towards
	        // the camera
	        rmt::Vector camPos;
	        p3d::context->GetView()->GetCamera()->GetWorldPosition( &camPos );
	        camPos.Sub( mpShadowMatrix->Row(3) );
	        camPos.Normalize();
        	
            // Move the shadow towards the camera by the following distance.
            const float Z_FIGHTING_OFFSET = 0.2f;
            camPos.Scale( Z_FIGHTING_OFFSET );

	        // Final shadow transform = position/orientation * tocamera translation
	        rmt::Matrix shadowTransform( *mpShadowMatrix );
	        shadowTransform.Row( 3 ).Add( camPos );

	        // Display
	        p3d::stack->PushMultiply( shadowTransform );
	        mpShadowDrawable->Display();
            p3d::stack->Pop();
        }
    }
    p3d::pddi->SetZWrite(true);
    END_PROFILE("DisplaySimpleShadow")
}
//========================================================================
// InstStatEntityDSG::RecomputeShadowPosition
//========================================================================
//
// Description: Finds the intersection point and computes mpShadowMatrix
//  
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void InstStatEntityDSG::RecomputeShadowPosition()
{
    if ( mpShadowMatrix )
    {
        rmt::Vector position;
        GetPosition( &position );
        ComputeShadowMatrix( position, mpShadowMatrix );
    }
}

//========================================================================
// InstStatEntityDSG::
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
void InstStatEntityDSG::GetBoundingBox(rmt::Box3D* box)
{
    rmt::Box3D fuckinTempBox;
    mpDrawstuff->GetBoundingBox(&fuckinTempBox);

    if ( rmt::Fabs(fuckinTempBox.Height()) > 10000.0f && 
         rmt::Fabs(fuckinTempBox.Width()) > 10000.0f && 
         rmt::Fabs(fuckinTempBox.Length()) > 10000.0f )
    {
        fuckinTempBox.low = rmt::Vector( -1.0f, -1.0f, -1.0f );
        fuckinTempBox.high = rmt::Vector( 1.0f, 1.0f, 1.0f );
    }
    
    mpMatrix->Transform(fuckinTempBox.low, &box->low);
    mpMatrix->Transform(fuckinTempBox.high, &box->high);
}
//========================================================================
// InstStatEntityDSG::
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
void InstStatEntityDSG::GetBoundingSphere(rmt::Sphere* pSphere)
{
    rmt::Sphere fuckinTempSphere;
    mpDrawstuff->GetBoundingSphere(&fuckinTempSphere);

    mpMatrix->Transform(fuckinTempSphere.centre, &pSphere->centre);
    pSphere->radius = fuckinTempSphere.radius;
}

   ///////////////////////////////////////////////////////////////////////
   // Accessors
   ///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatEntityDSG::
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
rmt::Matrix* InstStatEntityDSG::pMatrix()
{
    return mpMatrix;
}
//========================================================================
// InstStatEntityDSG::
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
tGeometry* InstStatEntityDSG::pGeo()
{
    rAssert(false);
    return (tGeometry*)mpDrawstuff;
}
   
   ///////////////////////////////////////////////////////////////////////
   // Load interface
   ///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatEntityDSG::
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
void InstStatEntityDSG::LoadSetUp
(  
    rmt::Matrix*        ipMatrix, 
    tGeometry*          ipGeo,
    tDrawable*          ipShadow
)
{
    mpMatrix    = ipMatrix;
    mpDrawstuff = ipGeo;

    mpDrawstuff->AddRef();

    //mShaderUID = ipGeo->GetShader(0)->GetUID();
    ipGeo->ProcessShaders(*this);

    if ( ipShadow )
    {
        tRefCounted::Assign( mpShadowDrawable, ipShadow );

        mpShadowMatrix = CreateShadowMatrix( ipMatrix->Row( 3 ) );
    }
}
//========================================================================
// InstStatEntityDSG::
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
void InstStatEntityDSG::LoadSetUp
(  
    rmt::Matrix*        ipMatrix, 
    tDrawable*          ipGeo,
    tDrawable*          ipShadow
)
{
    mpMatrix    = ipMatrix;
    mpDrawstuff = ipGeo;

    mpDrawstuff->AddRef();
    ipGeo->ProcessShaders(*this);

    if ( ipShadow )
    {
        tRefCounted::Assign( mpShadowDrawable, ipShadow );

        mpShadowMatrix = CreateShadowMatrix( ipMatrix->Row( 3 ) );
    }

}

//=============================================================================
// InstStatEntityDSG::pPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      rmt
//
//=============================================================================
rmt::Vector*  InstStatEntityDSG::pPosition()
{
    rAssert( false );

    return NULL;
}

//=============================================================================
// InstStatEntityDSG::rPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const rmt::Vector& InstStatEntityDSG::rPosition()
{
    rAssert( false );

    return mPosn;
}

//=============================================================================
// InstStatEntityDSG::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* ipPosn )
//
// Return:      void 
//
//=============================================================================
void InstStatEntityDSG::GetPosition( rmt::Vector* ipPosn )
{
   rmt::Sphere sphere;
   mpDrawstuff->GetBoundingSphere(&sphere);
   *ipPosn = sphere.centre;

   ipPosn->Transform( *mpMatrix );
}

//************************************************************************
//
// Protected Member Functions : InstStatEntityDSG 
//
//************************************************************************


rmt::Matrix* 
InstStatEntityDSG::CreateShadowMatrix( const rmt::Vector& position )
{

    rmt::Matrix* pResult;
    rmt::Matrix shadowMat;
    if ( ComputeShadowMatrix( position, &shadowMat ) )
    {
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        pResult = new rmt::Matrix();
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
        *pResult = shadowMat;

    }
    else
    {
        pResult = NULL ;
    }
    return pResult;
    
}

bool 
InstStatEntityDSG::ComputeShadowMatrix( const rmt::Vector& in_position, rmt::Matrix* out_pMatrix )
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


//************************************************************************
//
// Private Member Functions : InstStatEntityDSG 
//
//************************************************************************
