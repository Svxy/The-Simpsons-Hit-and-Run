//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        LensFlareDSG.cpp
//
// Description: Implementation for LensFlareDSG class.
//
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <render/DSG/LensFlareDSG.h>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/billboardobject.hpp>
#include <p3d/utility.hpp>
#include <pddi/pddiext.hpp>

// DEBUG stuff
#include <worldsim/avatarmanager.h>
#include <worldsim/character/character.h>

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

const float INTENSITY_CHANGE_PER_FRAME = 0.1f;
const int MAX_NUM_FLARES = 20;

SwapArray< LensFlareDSG* > LensFlareDSG::spDrawQueue(MAX_NUM_FLARES);
SwapArray< LensFlareDSG* > LensFlareDSG::spVisTestQueue(MAX_NUM_FLARES);

unsigned LensFlareDSG::sP3DVisibilityCounter = 0;

static rmt::Vector s_PixelPos(320,240,0);

#ifdef RAD_PS2
const tColour PS2_VISIBILITY_MESH_COLOUR = tColour( 0, 0,0,0 );
#endif

static rmt::Vector dstart,dend;


// Clears the flare draw queue. Should be done after they are drawn
void LensFlareDSG::ClearAllFlares()
{
	for (int i = 0 ; i < spDrawQueue.mUseSize ; i++)
	{
		spDrawQueue[ i ]->Release();
	}
	spDrawQueue.ClearUse();
}
// Draws every flare in the draw queue
void LensFlareDSG::DisplayAllFlares()
{
//    rTuneAssert( spDrawQueue.mUseSize < 2 );

    for (int i = 0 ; i < spDrawQueue.mUseSize ; i++)
	{
		spDrawQueue[ i ]->DisplayImmediate();
	}
	ClearAllFlares();
}
// Adds a flare to the draw queue
void LensFlareDSG::PostDisplayFlare( LensFlareDSG* pFlare )
{


	spDrawQueue.Add( pFlare );
	pFlare->AddRef();
}

// Checks all queued flares to see if they are visible
void LensFlareDSG::ReadFrameBufferIntensities()
{
#ifndef RAD_PS2
	pddiExtVisibilityTest* mVisibilityTestExtension = static_cast<pddiExtVisibilityTest*> (p3d::pddi->GetExtension( PDDI_EXT_VISIBILITY_TEST ) );
	if ( mVisibilityTestExtension != NULL )
	{
		for (int i = 0 ; i < spVisTestQueue.mUseSize ; i++)
		{		
			spVisTestQueue[ i ]->ReadFrameBufferIntensity();
			spVisTestQueue[ i ]->Release();
		}
		// Clear the array
		spVisTestQueue.ClearUse();
	}
#endif

}
// LensFlareDSG constructor
LensFlareDSG::LensFlareDSG()
: mPosn( 0,0,0 ),
mpCompDraw( NULL ),
mVisTestGeoIndex( -1 ),
mP3DVisibilityId( sP3DVisibilityCounter++ ),
mNumPixelsVisible( 1 ),
mCurrentIntensity( 0 )
{

}
// LensFlareDSG destructor
LensFlareDSG::~LensFlareDSG()
{
   int i;
   for (i=mpBillBoards.mUseSize - 1 ; i > -1 ; i--)
   {
		mpBillBoards[i]->Release();
   }
   if ( mpCompDraw != NULL )
   {
		mpCompDraw->Release();
   }

}
// Checks to see if this flare is visible to the camera
void LensFlareDSG::ReadFrameBufferIntensity()
{
#ifndef RAD_GAMECUBE

	pddiExtVisibilityTest* mVisibilityTestExtension = static_cast<pddiExtVisibilityTest*> (p3d::pddi->GetExtension( PDDI_EXT_VISIBILITY_TEST ) );
	if ( mVisibilityTestExtension != NULL )
	{
		int count = mVisibilityTestExtension->Result( mP3DVisibilityId );		
		mNumPixelsVisible = count;
	}
#endif

}
// Called by the loader - allocates space for iNumGroups of billboards
void LensFlareDSG::SetNumBillBoardQuadGroups( int iNumGroups )
{
	if (iNumGroups > 0)
	{
		mpBillBoards.Allocate( iNumGroups );
	}
}



//========================================================================
// LensFlare::
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
void LensFlareDSG::AddBillBoardQuadGroup( tBillboardQuadGroup* pGroup )
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
void LensFlareDSG::SetCompositeDrawable( tCompositeDrawable* ipCompDraw )
{


	mpCompDraw = ipCompDraw;
	mpCompDraw->AddRef();
    rmt::Sphere sphere;
    mpCompDraw->GetBoundingSphere( &sphere );
	mPosn = sphere.centre;

    // Find the visibility mesh. It will be the only piece of geo in this thing
    for ( int i = 0 ; i <mpCompDraw->GetNumDrawableElement() ; i++ )
    {
        tCompositeDrawable::DrawableElement* elem = mpCompDraw->GetDrawableElement(i);
        if ( elem->GetType() == tCompositeDrawable::DrawableElement::PROP_ELEMENT )
        {
            tCompositeDrawable::DrawablePropElement* propElem = static_cast< tCompositeDrawable::DrawablePropElement* >(elem);
            rAssert( dynamic_cast < tCompositeDrawable::DrawablePropElement* >( elem ) != NULL );
            if ( dynamic_cast< tGeometry* >( propElem->GetDrawable() ) != NULL )
            {
                // Found it, record the index of the composite drawable
                mVisTestGeoIndex = i;
                // Turn off visibility for this thing
                propElem->SetVisibility( false );
                propElem->SetLockVisibility( true );
                break;
            }
        }
    }
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
void LensFlareDSG::Display()
{
#ifdef PROFILER_ENABLED
    char profileName[] = "  LensFlareDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
	LensFlareDSG::PostDisplayFlare( this );
    DSG_END_PROFILE(profileName)
}
void LensFlareDSG::DisplayImmediate()
{
#ifdef RAD_WIN32
    return;
#endif

#ifdef PROFILER_ENABLED
    char profileName[] = "  LensFlareDSG DisplayImmediate";
#endif

    tView* view = p3d::context->GetView();
    if ( view == NULL )
        return;
    tCamera* camera = view->GetCamera();
    rmt::Vector cameraPosition;
    camera->GetWorldPosition( &cameraPosition );
    rmt::Matrix toCameraPosition;
    toCameraPosition.Identity();
    toCameraPosition.FillTranslate( cameraPosition );
    p3d::stack->PushMultiply( toCameraPosition );


    DSG_BEGIN_PROFILE(profileName)
	DrawVisibilityChecker();
#ifdef RAD_PS2
    // turn z compare to always
	SetBillBoardIntensity( 1.0f );
    pddiCompareMode origZCompare = p3d::pddi->GetZCompare();
    p3d::pddi->SetZCompare(PDDI_COMPARE_ALWAYS);
    mpCompDraw->Display();
    p3d::pddi->SetZCompare(origZCompare);
#else
	// Visible but not at full intensity, ramp up
	if (mNumPixelsVisible > 0 && mCurrentIntensity < 1.0f)
	{
		mCurrentIntensity += INTENSITY_CHANGE_PER_FRAME;
        if ( mCurrentIntensity > 1.0f )
            mCurrentIntensity = 1.0f;
	}
	else if (mNumPixelsVisible == 0 && mCurrentIntensity > 0) 
	{
		mCurrentIntensity -= INTENSITY_CHANGE_PER_FRAME;	
        if ( mCurrentIntensity < 0 ) 
            mCurrentIntensity = 0;
	}

	SetBillBoardIntensity( mCurrentIntensity );

    pddiCompareMode origZCompare = p3d::pddi->GetZCompare();
    p3d::pddi->SetZCompare(PDDI_COMPARE_ALWAYS);
    mpCompDraw->Display();
    p3d::pddi->SetZCompare(origZCompare);

#endif


    p3d::stack->Pop();

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
void LensFlareDSG::DisplayBoundingBox(tColour colour)
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
void LensFlareDSG::DisplayBoundingSphere(tColour colour)
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
void LensFlareDSG::GetBoundingBox(rmt::Box3D* box)
{
    mpCompDraw->GetBoundingBox( box );
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
void LensFlareDSG::GetBoundingSphere(rmt::Sphere* pSphere)
{
    mpCompDraw->GetBoundingSphere( pSphere );
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
rmt::Vector* LensFlareDSG::pPosition()
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
const rmt::Vector& LensFlareDSG::rPosition()
{
    return mPosn;
}
//==============================6==========================================
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
void LensFlareDSG::GetPosition( rmt::Vector* ipPosn )
{
	*ipPosn = mPosn;
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
void LensFlareDSG::RenderUpdate()
{
   //Do Nothing
}
//************************************************************************
//
// Protected Member Functions : WorldSphereDSG 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : WorldSphereDSG 
//
//************************************************************************

#ifdef RAD_PS2
void LensFlareDSG::DrawVisibilityChecker()
{

}
#endif

#if defined( RAD_XBOX ) || defined( RAD_WIN32 )

void LensFlareDSG::DrawVisibilityChecker()
{
	// We need to use a PDDI extension to draw the billboards 
	// with colour write off at a specific location 
	// This will fill the Z buffer (make sure to draw enable Z buffering!)
	// with the billboard distances
	// Then draw the scene as usual
	// Fetch the extension from PDDI
	pddiExtVisibilityTest* mVisibilityTestExtension = static_cast<pddiExtVisibilityTest*> (p3d::pddi->GetExtension( PDDI_EXT_VISIBILITY_TEST ) );
	if ( mVisibilityTestExtension != NULL &&
		 mVisTestGeoIndex != -1 )
	{
        bool origZWrite = p3d::pddi->GetZWrite();
		p3d::pddi->SetZWrite(false);

		// Tell PDDI to begin logging my Z writes

		// Force Z writes to true
		// We know that lens flares never write into the Z buffer, so we don't have to save 
		// their Z write values
	
        mpCompDraw->GetPose()->Evaluate();
		p3d::pddi->SetColourWrite( false, false, false, false );
        rTuneAssert( p3d::pddi->GetZCompare() != PDDI_COMPARE_ALWAYS );
        tCompositeDrawable::DrawableElement* elem = mpCompDraw->GetDrawableElement( mVisTestGeoIndex );
        elem->SetPose( mpCompDraw->GetPose() );
        elem->SetLockVisibility( false );
        elem->SetVisibility( true );
    	mVisibilityTestExtension->Begin();
        elem->Display();
        // Tell PDDI to stop logging z writes
		mVisibilityTestExtension->End( mP3DVisibilityId );

        elem->SetVisibility( false );
        elem->SetLockVisibility( true );

		// Restore Z write values
		p3d::pddi->SetColourWrite( true, true, true, true );
		p3d::pddi->SetZWrite(origZWrite);
		
		// Insert a pointer to this DSG object into the vis test array
		// so we can set it up to be retreived upon DSG removal
		this->AddRef();
		LensFlareDSG* pLensFlare = this;
		spVisTestQueue.Add( pLensFlare );
	}    
}
#endif

#ifdef RAD_GAMECUBE
void LensFlareDSG::DrawVisibilityChecker()
{
    rTuneAssert( p3d::pddi->GetZCompare() != PDDI_COMPARE_ALWAYS );

    // Gamecube
    // Have to read the Z value of the center of the visibility mesh before and after it 
    // was drawn

    // Read Z value before it was drawn

    // Determine position to read from
    // Evaluate the composite drawable
    mpCompDraw->GetPose()->Evaluate();
    // and fetch the element's matrix
    tCompositeDrawable::DrawableElement* elem = mpCompDraw->GetDrawableElement( mVisTestGeoIndex );
    elem->SetPose( mpCompDraw->GetPose() );
    const rmt::Matrix* pWorldMat = elem->GetWorldMatrix();
    // Position is determined by using tContext::ObjectToDevice
    p3d::stack->PushMultiply( *pWorldMat );
    rmt::Vector deviceCoords;
    p3d::context->ObjectToDevice( rmt::Vector(0,0,0), &deviceCoords );
    p3d::stack->Pop();
    // Check that the deviceCoords fall within valid Gamecube API range
    // x from 0 to 639 inclusive
    // y from 0 to 527 inclusive
    // If either coordinate is not in this range, bail, its not visible
    if ( deviceCoords.x < 0 || deviceCoords.x > 639 || deviceCoords.y < 0 || deviceCoords.y > 527 )
    {
        mNumPixelsVisible = 0;
    }
    else
    {

        // Make sure drawing is completed so far and read the z buffer depth at the point where
        // the mesh will be drawn
        GXDrawDone(); 
        u32 depthBeforeVisMesh;
        GXPeekZ( deviceCoords.x, deviceCoords.y, &depthBeforeVisMesh );   


        // Now draw the visibility mesh and see if it was actually written
	    p3d::pddi->SetColourWrite( false, false, false, true );
        elem->SetLockVisibility( false );
        elem->SetVisibility( true );    
        elem->Display();
        elem->SetVisibility( false );
        elem->SetLockVisibility( true );
        GXDrawDone();
        // Vis mesh has been drawn, read the pixel
        u32 depthAfterVisMesh;
        GXPeekZ( deviceCoords.x, deviceCoords.y, &depthAfterVisMesh );

        if ( depthAfterVisMesh < depthBeforeVisMesh )
            mNumPixelsVisible = 1;
        else
            mNumPixelsVisible = 0;

	    p3d::pddi->SetColourWrite( true, true, true, true );


    }

}
#endif


void LensFlareDSG::SetBillBoardIntensity( float intensity )
{
	for (int i = 0 ; i < mpBillBoards.mUseSize ; i++)
	{
		mpBillBoards[ i ]->SetIntensityBias( intensity );
	}
}


