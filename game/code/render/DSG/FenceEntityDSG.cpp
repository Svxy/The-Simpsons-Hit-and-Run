//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        FenceEntityDSG.cpp
//
// Description: Implementation for FenceEntityDSG class.
//
// History:     Implemented	                         --Devin [7/6/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/FenceEntityDSG.h>
#include <render/Culling/Bounds.h>


//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : FenceEntityDSG Interface
//
//************************************************************************
FenceEntityDSG::FenceEntityDSG( void ){}
FenceEntityDSG::~FenceEntityDSG( void ){}

///////////////////////////////////////////////////////////////////////
// Drawable
///////////////////////////////////////////////////////////////////////
//========================================================================
// FenceEntityDSG::
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
void FenceEntityDSG::Display()
{
#ifndef RAD_RELEASE
    if(IS_DRAW_LONG) return;
    //rAssert(false);
    pddiPrimStream* stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 2);

    tColour colour(0, 0, 255);

    stream->Colour(colour);
    stream->Coord(mStartPoint.x, mStartPoint.y, mStartPoint.z);
    stream->Colour(colour);
    stream->Coord(mEndPoint.x, mEndPoint.y, mEndPoint.z);
    p3d::pddi->EndPrims(stream);
#endif
}
//========================================================================
// FenceEntityDSG::
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
void FenceEntityDSG::DisplayBoundingBox(tColour colour)
{
   rAssert(false);
}
//========================================================================
// FenceEntityDSG::
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
void FenceEntityDSG::DisplayBoundingSphere(tColour colour)
{
   rAssert(false);
}

//========================================================================
// FenceEntityDSG::
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
void FenceEntityDSG::GetBoundingBox(rmt::Box3D* box)
{
   Bounds3f bounds;

   bounds.mMin.SetTo(mStartPoint);
   bounds.mMax.SetTo(mStartPoint);
   bounds.Accumulate(mEndPoint);
   
   box->low    = (bounds.mMin);
   box->high   = (bounds.mMax);
}
//========================================================================
// FenceEntityDSG::
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
void FenceEntityDSG::GetBoundingSphere(rmt::Sphere* sphere)
{
   rmt::Vector tmp(mStartPoint);
   tmp += mEndPoint;
   tmp /= 2.0f;

   sphere->centre = tmp;

   tmp.Sub(mStartPoint,tmp);
   sphere->radius = tmp.Magnitude();
}

///////////////////////////////////////////////////////////////////////
// IEntityDSG
///////////////////////////////////////////////////////////////////////
//========================================================================
// FenceEntityDSG::
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
rmt::Vector*       FenceEntityDSG::pPosition()
{
   return &mStartPoint;
}
//========================================================================
// FenceEntityDSG::
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
const rmt::Vector& FenceEntityDSG::rPosition()
{
   return mStartPoint;
}
//========================================================================
// FenceEntityDSG::
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
void FenceEntityDSG::GetPosition( rmt::Vector* ipPosn )
{
   *ipPosn = mStartPoint;
}

//////////////////////////////////////////////////////////////////////////
// override these methods so we can stub them out
sim::Solving_Answer FenceEntityDSG::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{
   //Greg, do the stuff you need here
   return sim::Solving_Continue;
}


sim::Solving_Answer FenceEntityDSG::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{
   
    // subclass-specific shit here

    return CollisionEntityDSG::PostReactToCollision(impulse, inCollision);
}


//************************************************************************
//
// Protected Member Functions : FenceEntityDSG 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : FenceEntityDSG 
//
//************************************************************************
