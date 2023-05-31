#ifndef __WorldSphereDSG_H__
#define __WorldSphereDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   WorldSphereDSG
//
// Description: The WorldSphereDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/27]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <p3d/p3dtypes.hpp>
#include <p3d/billboardobject.hpp>
#include <p3d/Geometry.hpp>
#include <p3d/anim/multicontroller.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/DSG/IEntityDSG.h>
#include <render/Culling/SwapArray.h>
#include <render/DSG/LensFlareDSG.h>


class tCompositeDrawable;

//========================================================================
//
// Synopsis:   The WorldSphereDSG; Synopsis by Inspection.
//
//========================================================================
class WorldSphereDSG : public IEntityDSG
{
public:
   WorldSphereDSG();
   ~WorldSphereDSG();

   void AddMesh(tGeometry* ipGeo);
   void AddBillBoardQuadGroup( tBillboardQuadGroup* );
   void SetNumMeshes(int iNumMeshes);
   void SetNumBillBoardQuadGroups( int iNumGroups );
   void SetMultiController(tMultiController* ipMultiController);
   void SetCompositeDrawable( tCompositeDrawable* ipCompDraw );
   void SetFlare( LensFlareDSG* pFlare );
   void Activate();
   void Deactivate();

   ///////////////////////////////////////////////////////////////////////
   // Drawable
   ///////////////////////////////////////////////////////////////////////
   void Display();    

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


protected:
   void SetInternalState();

   bool mbActive;
   rmt::Vector mPosn;
   tCompositeDrawable* mpCompDraw;
   SwapArray<tGeometry*> mpGeos;
   SwapArray<tBillboardQuadGroup*> mpBillBoards;
   tMultiController* mpMultiCon;
   LensFlareDSG*	mpFlare;

private:
};

#endif
