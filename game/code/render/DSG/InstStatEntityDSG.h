#ifndef __InstStatEntityDSG_H__
#define __InstStatEntityDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstStatEntityDSG
//
// Description: The InstStatEntityDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/06/17]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <simphysics/physicsobject.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/DSG/StaticEntityDSG.h>

//========================================================================
//
// Synopsis:   The InstStatEntityDSG; Synopsis by Inspection.
//
//========================================================================
class InstStatEntityDSG 
: public StaticEntityDSG
{
public:
   InstStatEntityDSG();
   virtual ~InstStatEntityDSG();

   //////////////////////////////////////////////////////////////////////////
   // tDrawable
   //////////////////////////////////////////////////////////////////////////
   void Display();
   void GetBoundingBox(rmt::Box3D* box);
   void GetBoundingSphere(rmt::Sphere* pSphere);

   virtual rmt::Vector*       pPosition();
   virtual const rmt::Vector& rPosition();
   virtual void GetPosition( rmt::Vector* ipPosn );
   
   virtual void SetShader(tShader* pShader, int i)
   {
   }
   ///////////////////////////////////////////////////////////////////////
   // Accessors
   ///////////////////////////////////////////////////////////////////////
   rmt::Matrix*        pMatrix(); 
   tGeometry*          pGeo();     
   
   ///////////////////////////////////////////////////////////////////////
   // Load interface
   ///////////////////////////////////////////////////////////////////////
   virtual void LoadSetUp(  rmt::Matrix*        ipMatrix, 
                    tGeometry*          ipGeo,
                    tDrawable* shadow = NULL );
   virtual void LoadSetUp(  rmt::Matrix*        ipMatrix, 
                    tDrawable*          ipGeo,
                    tDrawable* shadow = NULL );

   virtual void DisplaySimpleShadow();
   virtual int CastsShadow() { if(mpShadowDrawable != NULL ) return 2; else return 0; }
    // Used to recompute the shadow position on the ground when the object moves
   virtual void RecomputeShadowPosition();
protected:
    rmt::Matrix*        mpMatrix;
    tDrawable*          mpShadowDrawable;
    rmt::Matrix*        mpShadowMatrix;

    rmt::Matrix* CreateShadowMatrix( const rmt::Vector& position );
    bool ComputeShadowMatrix( const rmt::Vector& in_position, rmt::Matrix* out_pMatrix );

private:
};

#endif
