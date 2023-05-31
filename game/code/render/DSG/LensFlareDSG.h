#ifndef LENSFLAREDSG_H
#define LENSFLAREDSG_H

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   LensFlareDSG
//
// Description: A compdrawable with a number of billboards. Also drawn AFTER everything else
//
// History:     + Initial Implementation		      -- Michael Riegger
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <p3d/p3dtypes.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/DSG/InstStatEntityDSG.h>
#include <render/Culling/SwapArray.h>


class tCompositeDrawable;
class tBillboardQuadGroup;
class tGeometry;

//========================================================================
//
// The LensFlareDSG - Draws lens flares 
// There could be a problem with the lens flares in split screen mode
// Each flare is tested once after swapbuffer, they should be tested
// Once per viewport
//
//========================================================================
class LensFlareDSG : public StaticEntityDSG
{
public:
	LensFlareDSG();
	~LensFlareDSG();

	// Lens flares are composed of a varying number of billboarded quads
	// This functions adds a new quad to the flare (typically from the lensflareloader)
	void AddBillBoardQuadGroup( tBillboardQuadGroup* );
	// Allocates the number of billboards, must be called before addbillboardquadgroup
	void SetNumBillBoardQuadGroups( int iNumGroups );
	// Sets the compdrawable. The compdraw contains the billboards
	void SetCompositeDrawable( tCompositeDrawable* ipCompDraw );
	// Sets the visibility geometry. NOTE - THIS IS NEVER ACTUALLY DRAWN!!!
	// It is only used to determine if the lens flare is visible or not
	// It represents the actual object that is generating the flare, which
	// could be occulded by something, hence we test to see if this is actually
	// visible before drawing the flares
    // Not used, I iterate through the comp drawable and find the desired mesh
//	void SetVisTestMesh( tGeometry* ipGeo );

	///////////////////////////////////////////////////////////////////////
	// Drawable
	///////////////////////////////////////////////////////////////////////

	// Does not draw immediately!!!! Calls PostDisplayFlare() internally to set
	// up the flare to be drawn at a later date.
	void Display();    
	// Immediately displays the flares. 
	void DisplayImmediate();

	// Checks the visibility test to see if the vistest mesh is visible or not
	// used to determine flare intensity (not visible - flare fades to zero)
	void ReadFrameBufferIntensity();

	void DisplayBoundingBox(tColour colour = tColour(0,255,0));
	void DisplayBoundingSphere(tColour colour = tColour(0,255,0));

	void GetBoundingBox(rmt::Box3D* box);
	void GetBoundingSphere(rmt::Sphere* sphere);


	///////////////////////////////////////////////////////////////////////
	// IEntityDSG
	///////////////////////////////////////////////////////////////////////
	rmt::Vector*       pPosition();
	const rmt::Vector& rPosition();
	void               GetPosition( rmt::Vector* ipPosn );

	void RenderUpdate();

	// Draw all flares, immediately (usually called after all other objects in the world have been drawn )
	static void DisplayAllFlares();

	// Clears the list of PostDisplayed flares
	static void ClearAllFlares();

	// Flags a flare to be drawn during DisplayAllFlares(). Display() defaults to calling 
	// PostDisplayFlare()
	static void PostDisplayFlare( LensFlareDSG* pFlare );

	// Reads all flare intensities
	static void ReadFrameBufferIntensities();
 
    virtual void SetShader(tShader* pShader, int i)
    {
    }

protected:

	// listing of all available lens flares

	rmt::Vector mPosn;
	tCompositeDrawable* mpCompDraw;
//	tGeometry* mpTestVisGeo;
    int mVisTestGeoIndex;

	SwapArray< tBillboardQuadGroup* > mpBillBoards;
	unsigned mP3DVisibilityId;
	int mNumPixelsVisible;
	float mCurrentIntensity;

	void DrawVisibilityChecker();
	void EnableZWrites( bool enable );

	void SetBillBoardIntensity( float intensity );


private:

	// Listing of all flares in the world that want to be drawn 
	static SwapArray< LensFlareDSG* > spDrawQueue;
	static SwapArray< LensFlareDSG* > spVisTestQueue;
	static unsigned sP3DVisibilityCounter; 


};

#endif
