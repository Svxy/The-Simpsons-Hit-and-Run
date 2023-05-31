//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        WorldSphereDSG.cpp
//
// Description: Implementation for WorldSphereDSG class.
//
// History:     Implemented	                         --Devin [5/27/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <render/DSG/WorldSphereDSG.h>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/camera.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/utility.hpp>
#include <p3d/view.hpp>
#include <render/animentitydsgmanager/animentitydsgmanager.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : WorldSphereDSG Interface
//
//************************************************************************
//========================================================================
// WorldSphereDSG::
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
WorldSphereDSG::WorldSphereDSG()
: mbActive(FALSE),
mpCompDraw( NULL ),
mpMultiCon( NULL ),
mpFlare( NULL )
{
}
//========================================================================
// WorldSphereDSG::
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
WorldSphereDSG::~WorldSphereDSG()
{
BEGIN_PROFILE( "WorldSphereDSG Destroy" );
   int i;
   for(i=mpGeos.mUseSize-1; i>-1; i--)
   {
      mpGeos[i]->Release();
   }
   for (i=mpBillBoards.mUseSize - 1 ; i > -1 ; i--)
   {
		mpBillBoards[i]->Release();
   }
   if ( mpCompDraw != NULL )
   {
	   mpCompDraw->Release();
   }

   if(mpMultiCon)
   {
       mpMultiCon->Release();
	   GetAnimEntityDSGManager()->Remove( mpMultiCon );
   }
   if( mpFlare )
   {
	   mpFlare->Release();
   }
END_PROFILE( "WorldSphereDSG Destroy" );
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::Activate()
{
    mbActive = TRUE;
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::Deactivate()
{
    mbActive = FALSE;
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::SetNumMeshes(int iNumMeshes)
{
	if ( iNumMeshes > 0 )
	{
		mpGeos.Allocate(iNumMeshes);
	}
}

void WorldSphereDSG::SetNumBillBoardQuadGroups( int iNumGroups )
{
	if (iNumGroups > 0)
	{
		mpBillBoards.Allocate( iNumGroups );
	}
}

void WorldSphereDSG::SetFlare( LensFlareDSG* pFlare )
{
	rAssert( mpFlare == NULL );
	rAssert( pFlare != NULL );
	mpFlare = pFlare;
	mpFlare->AddRef();
}

//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::AddMesh(tGeometry* ipGeo)
{
    mpGeos.Add(ipGeo);
    ipGeo->AddRef();

    SetInternalState();
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::AddBillBoardQuadGroup( tBillboardQuadGroup* pGroup )
{
	mpBillBoards.Add( pGroup );
	pGroup->AddRef();
}

//========================================================================
// WorldSphereDSG::SetCompositeDrawable
//========================================================================
//
// Description: Sets the single composite drawable
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void WorldSphereDSG::SetCompositeDrawable( tCompositeDrawable* ipCompDraw )
{
	mpCompDraw = ipCompDraw;
	mpCompDraw->AddRef();
    rmt::Sphere sphere;
    mpCompDraw->GetBoundingSphere( &sphere );
	mPosn = sphere.centre;
}

//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::SetMultiController(tMultiController* ipMultiController)
{
    mpMultiCon = ipMultiController;
    mpMultiCon->AddRef();
	mpMultiCon->SetCycleMode( FORCE_CYCLIC );
	GetAnimEntityDSGManager()->Add( mpMultiCon );
}
///////////////////////////////////////////////////////////////////////
// Drawable
///////////////////////////////////////////////////////////////////////
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::Display()
{
    
#ifdef PROFILER_ENABLED
    char profileName[] = "  WorldSphereDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    if( mbActive )
    {
		int i;
        // Translate the worldsphere to the camera position
        tCamera* pCurrentCamera = p3d::context->GetView()->GetCamera();
        rAssert( pCurrentCamera != NULL );
        rmt::Vector cameraPosition;
        pCurrentCamera->GetWorldPosition( &cameraPosition );

        rmt::Matrix toCameraPosition;
        toCameraPosition.Identity();
        toCameraPosition.FillTranslate( cameraPosition );

        p3d::stack->PushMultiply( toCameraPosition );

		if ( mpCompDraw != NULL )
		{
			mpCompDraw->Display();
		}
		else
		{
			for(i=0; i<mpGeos.mUseSize; i++)
			{
			    mpGeos[i]->Display();
			}
		}
        // Pop the toCamera matrix transformation
        p3d::stack->Pop();
    }
	if (mpFlare)
	{
		mpFlare->Display();
	}
    DSG_END_PROFILE(profileName)
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::DisplayBoundingBox(tColour colour)
{
    rTuneAssert(false);
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::DisplayBoundingSphere(tColour colour)
{
    rTuneAssert(false);
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::GetBoundingBox(rmt::Box3D* box)
{
    rTuneAssert(false);
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::GetBoundingSphere(rmt::Sphere* sphere)
{
    rTuneAssert(false);
}
///////////////////////////////////////////////////////////////////////
// IEntityDSG
///////////////////////////////////////////////////////////////////////
//========================================================================
// WorldSphereDSG::
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
rmt::Vector* WorldSphereDSG::pPosition()
{
    rTuneAssert(false);
    return NULL;
}
//========================================================================
// WorldSphereDSG::
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
const rmt::Vector& WorldSphereDSG::rPosition()
{
    rTuneAssert(false);
    return mPosn;
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::GetPosition( rmt::Vector* ipPosn )
{
    rTuneAssert(false);
}
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::RenderUpdate()
{
   //Do Nothing
}
//************************************************************************
//
// Protected Member Functions : WorldSphereDSG 
//
//************************************************************************
//========================================================================
// WorldSphereDSG::
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
void WorldSphereDSG::SetInternalState()
{
    rmt::Sphere sphere;
    mpGeos[0]->GetBoundingSphere(&sphere);
    mPosn = sphere.centre;
}
//************************************************************************
//
// Private Member Functions : WorldSphereDSG 
//
//************************************************************************


