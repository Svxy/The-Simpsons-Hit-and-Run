#ifndef __StaticEntityDSG_H__
#define __StaticEntityDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StaticEntityDSG
//
// Description: The StaticEntityDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/27]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <p3d/p3dtypes.hpp>
#include <p3d/Geometry.hpp>

//=================================================
// Project Includes
//=================================================
#include <render/DSG/IEntityDSG.h>
#include <p3d/shader.hpp>
//========================================================================
//
// Synopsis:   The StaticEntityDSG; Synopsis by Inspection.
//
//========================================================================
class StaticEntityDSG : public IEntityDSG
{
public:
   StaticEntityDSG();
   ~StaticEntityDSG();

   void SetGeometry(tGeometry* ipGeo);
   tGeometry* mpGeo();
   void SetDrawable(tDrawable* ipDraw);
   tDrawable* mpDraw();
   virtual void SetRank(rmt::Vector& irRefPosn, rmt::Vector& mViewVector);
   ///////////////////////////////////////////////////////////////////////
   // Drawable
   ///////////////////////////////////////////////////////////////////////
   void Display();    

    #ifndef RAD_RELEASE
    void DisplayBoundingBox(tColour colour = tColour(0,255,0));
    void DisplayBoundingSphere(tColour colour = tColour(0,255,0));
    #endif

   void GetBoundingBox(rmt::Box3D* box);
   void GetBoundingSphere(rmt::Sphere* sphere);

   ///////////////////////////////////////////////////////////////////////
   // IEntityDSG
   ///////////////////////////////////////////////////////////////////////
   rmt::Vector*       pPosition();
   const rmt::Vector& rPosition();
   void               GetPosition( rmt::Vector* ipPosn );

   void RenderUpdate();

   virtual void SetShader(tShader* pShader, int i)
   {
       if(mIsGeo)
       {
           ((tGeometry*)mpDrawstuff)->SetShader(i,pShader);
       }
   }

protected:
   void SetInternalState();

   rmt::Vector mPosn;

   //tGeometry* mpGeometry;
   // Downcasting with extreme prejudice; these objects will be used
   // responsibly, and with foreknowledge of type. It's ugly. 
   // But it's also almost midnight 
   // MS 11
   enum
   {
       NOT_GEO      = 0x0,
       GEO          = 0x1,
       IS_SHADOW    = 0x2
   };

   int mIsGeo;
   tDrawable* mpDrawstuff;
private:
};

#endif
