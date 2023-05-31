//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SphereSP.cpp
//
// Description: Implementation for SphereSP class.
//
// History:     Implemented	                         --Devin [5/29/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <render/Culling/SphereSP.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : SphereSP Interface
//
//************************************************************************
//========================================================================
// SphereSP::
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
SphereSP::SphereSP()
{
}
//========================================================================
// SphereSP::
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
SphereSP::~SphereSP()
{
}
//////////////////////////////////////////////////////////////////////////
// ISpatialProxyAA
// <0.0   -   Inside Spatial Proxy
// =0.0   -   On Spatial Proxy Surface
// >0.0   -   Outside Spatial Proxy
//////////////////////////////////////////////////////////////////////////
//========================================================================
// SphereSP::
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
/*
float SphereSP::CompareTo( AAPlane3f&       irPlane )
{
   if( irPlane.mPosn - msIntersectionEpsilon <= mBounds.mMin[(int)irPlane.mAxis] )
   {
      return -1.0f;
   }
   if( irPlane.mPosn + msIntersectionEpsilon >= mBounds.mMax[(int)irPlane.mAxis] )
   {
      return 1.0f;
   }

   return 0.0f;
}
*/
//========================================================================
// SphereSP::
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
float SphereSP::CompareTo( const Vector3f& irPoint )
{
   float distance = (mRadius+msIntersectionEpsilon)*(mRadius+msIntersectionEpsilon);
   Vector3f temp;
   temp.Sub(irPoint,mCenter);
   float result = temp.MagnitudeSqr();
   if( result < distance )
   {
      return -1.0f;
   }
   if( result > distance )
   {
      return 1.0f;
   }
   return 0.0f;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
float SphereSP::CompareToXZ( const Vector3f& irPoint )
{
   float distance = (mRadius+msIntersectionEpsilon)*(mRadius+msIntersectionEpsilon);
   Vector3f temp;
   temp.Sub(irPoint,mCenter);
   temp.y=0.0f;
   float result = temp.MagnitudeSqr();
   if( result < distance )
   {
      return -1.0f;
   }
   if( result > distance )
   {
      return 1.0f;
   }
   return 0.0f;
}
//========================================================================
// SphereSP::
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
float SphereSP::CompareTo( AAPlane3f&       irPlane )
{
    if( irPlane.mPosn > mCenter[(int)irPlane.mAxis]+mRadius+msIntersectionEpsilon )
    {
        return 1.0f;
    }
    if( irPlane.mPosn < mCenter[(int)irPlane.mAxis]-mRadius-msIntersectionEpsilon )
    {
        return -1.0f;
    }
    return 0.0f;
}
//========================================================================
// SphereSP::
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
float  SphereSP::TestNotOutside( ISpatialProxyAA& irSpatialProxy )
{
   for( int i=irSpatialProxy.nPts()-1; i>-1; i-- )
   {
      if( CompareTo(irSpatialProxy.mPt(i)) < 0.0f )
      {
         //Inside
         return -1.0f;
      }
   }
   //Outside
   return 1.0f;
   
}
//========================================================================
// SphereSP::
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
void SphereSP::SetTo( Bounds3f& irBounds )
{
   rAssert(false);
}
//========================================================================
// SphereSP::
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
void SphereSP::SetTo( rmt::Vector irCenter, float iRadius )
{
   mCenter = irCenter;
   mRadius = iRadius;

   mBounds.mMin.SetTo( irCenter ); 
   mBounds.mMin -= iRadius;
   mBounds.mMax.SetTo( irCenter ); 
   mBounds.mMax += iRadius;
}

//************************************************************************
//
// Protected Member Functions : SphereSP 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : SphereSP 
//
//************************************************************************




