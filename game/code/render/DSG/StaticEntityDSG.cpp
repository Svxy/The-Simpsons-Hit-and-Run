//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        StaticEntityDSG.cpp
//
// Description: Implementation for StaticEntityDSG class.
//
// History:     Implemented	                         --Devin [5/27/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <render/DSG/StaticEntityDSG.h>
#include <memory/srrmemory.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : StaticEntityDSG Interface
//
//************************************************************************
//========================================================================
// StaticEntityDSG::
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
StaticEntityDSG::StaticEntityDSG()
{
   mpDrawstuff = NULL;
}
//========================================================================
// StaticEntityDSG::
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
StaticEntityDSG::~StaticEntityDSG()
{
BEGIN_PROFILE( "StaticEntityDSG Destroy" );
   if(mpDrawstuff != NULL)
   {
      mpDrawstuff->Release();
   }
END_PROFILE( "StaticEntityDSG Destroy" );
}
//========================================================================
// StaticEntityDSG::SetRank
//========================================================================
//
// Description: Sets rank, defaults to default SetRank for normal geo
//              however, shadows always get drawn first in the translucent pass
//
// Parameters:  rmt::Vector& irRefPosn, rmt::Vector& mViewVector.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void StaticEntityDSG::SetRank(rmt::Vector& irRefPosn, rmt::Vector& mViewVector)
{
    if ( ( mIsGeo & IS_SHADOW ) == false )
    {
        IEntityDSG::SetRank( irRefPosn, mViewVector );
    }
    else
    {
        mRank = FLT_MAX;
    }
}

//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::SetGeometry(tGeometry* ipGeo)
{
   if(mpDrawstuff != NULL)
   {
      mpDrawstuff->Release();
   }
   
   mpDrawstuff = ipGeo;

   if(mpDrawstuff != NULL)
   {
      mpDrawstuff->AddRef();
   }
   mIsGeo = GEO;

   if(ipGeo->CastsShadow())
   {
       mIsGeo = mIsGeo | IS_SHADOW;
   }
   
//   mShaderUID = ipGeo->GetShader(0)->GetUID();
   ipGeo->ProcessShaders(*this);

   SetInternalState();
}
//========================================================================
// StaticEntityDSG::
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
tGeometry* StaticEntityDSG::mpGeo()
{
   return (tGeometry*)mpDrawstuff;
}
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::SetDrawable(tDrawable* ipDraw)
{
   if(mpDrawstuff != NULL)
   {
      mpDrawstuff->Release();
   }
   
   mpDrawstuff = ipDraw;

   if(mpDrawstuff != NULL)
   {
      mpDrawstuff->AddRef();
   }

   mIsGeo = NOT_GEO;

   SetInternalState();
}
//========================================================================
// StaticEntityDSG::
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
tDrawable* StaticEntityDSG::mpDraw()
{
    return mpDrawstuff;

}

///////////////////////////////////////////////////////////////////////
// Drawable
///////////////////////////////////////////////////////////////////////
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::Display()
{
#ifdef PROFILER_ENABLED
    char profileName[] = "  StaticEntityDSG Display";
#endif
    if(IS_DRAW_LONG) return;
    DSG_BEGIN_PROFILE(profileName)

    if(mIsGeo & IS_SHADOW)
    {
        p3d::pddi->SetZWrite(false);
        mpDrawstuff->Display();
        p3d::pddi->SetZWrite(true);
    }
    else
    {
        mpDrawstuff->Display();
    }
    DSG_END_PROFILE(profileName)
}

#ifndef RAD_RELEASE
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::DisplayBoundingBox(tColour colour)
{
#ifndef RAD_RELEASE
   mpDrawstuff->DisplayBoundingBox(colour);
#endif
}
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::DisplayBoundingSphere(tColour colour)
{
   mpDrawstuff->DisplayBoundingSphere(colour);
}
#endif

//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::GetBoundingBox(rmt::Box3D* box)
{
   mpDrawstuff->GetBoundingBox(box);
}
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::GetBoundingSphere(rmt::Sphere* sphere)
{
   mpDrawstuff->GetBoundingSphere(sphere);
}
///////////////////////////////////////////////////////////////////////
// IEntityDSG
///////////////////////////////////////////////////////////////////////
//========================================================================
// StaticEntityDSG::
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
rmt::Vector* StaticEntityDSG::pPosition()
{
   return &mPosn;
}
//========================================================================
// StaticEntityDSG::
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
const rmt::Vector& StaticEntityDSG::rPosition()
{
   return mPosn;
}
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::GetPosition( rmt::Vector* ipPosn )
{
   *ipPosn = mPosn;
}
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::RenderUpdate()
{
   //Do Nothing
}
//************************************************************************
//
// Protected Member Functions : StaticEntityDSG 
//
//************************************************************************
//========================================================================
// StaticEntityDSG::
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
void StaticEntityDSG::SetInternalState()
{
   rmt::Sphere sphere;

   mpDrawstuff->GetBoundingSphere(&sphere);
   mPosn = sphere.centre;
}
//************************************************************************
//
// Private Member Functions : StaticEntityDSG 
//
//************************************************************************


