#ifndef __SphereSP_H__
#define __SphereSP_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SphereSP
//
// Description: The SphereSP does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/29]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/culling/Vector3f.h>
#include <render/culling/Plane3f.h>
#include <render/culling/Bounds.h>

#include <render/culling/ISpatialProxy.h>
#include <render/culling/BoxPts.h>

//========================================================================
//
// Synopsis:   The SphereSP; Synopsis by Inspection.
//
//========================================================================
class SphereSP : public BoxPts
{
public:
   SphereSP();
   ~SphereSP();

   // This SpatialProxy is...
   // <0.0   -   Less than comparison object in posn
   // =0.0   -   Intersection with comparison object 
   // >0.0   -   Greater than comparison object in posn 
   float CompareTo( AAPlane3f&       irPlane );
   // ISpatialProxyAA
   // <0.0   -   Inside Spatial Proxy
   // =0.0   -   On Spatial Proxy Surface
   // >0.0   -   Outside Spatial Proxy
   virtual float CompareTo( const Vector3f& irPoint );
   virtual float CompareToXZ( const Vector3f& irPoint );
   virtual float  TestNotOutside( ISpatialProxyAA& irSpatialProxy );

   virtual void SetTo( Bounds3f& irBounds );
   void SetTo( rmt::Vector irCenter, float iRadius );

   rmt::Vector mCenter;
   float       mRadius;
private:
};

#endif
/*
#ifndef __BOX_PTS_H__
#define __BOX_PTS_H__

#include <render/culling/Vector3f.h>
#include <render/culling/Plane3f.h>
#include <render/culling/Bounds.h>

#include <render/culling/ISpatialProxy.h>

class BoxPts : public ISpatialProxyAA
{
public:
   BoxPts(){}
   ~BoxPts(){}

   // ISpatialProxyAA
   // <0.0   -   Inside Spatial Proxy
   // =0.0   -   On Spatial Proxy Surface
   // >0.0   -   Outside Spatial Proxy
	virtual float CompareTo( AAPlane3f&       irPlane );
   virtual float CompareTo( const Vector3f& irPoint );

   virtual float  TestNotOutside( ISpatialProxyAA& irSpatialProxy );

   virtual int       nPts();
   virtual Vector3f  mPt( int iIndex );

   virtual bool DoesIntersect(     AAPlane3f& irClipPlane );
   virtual bool DoesntIntersect(   AAPlane3f& irClipPlane );

   //Get a point representing some point within the SpatialProxy
   virtual Vector3f GetPoint();

   void CutOffGT( AAPlane3f& irPlane3f );
   void CutOffLT( AAPlane3f& irPlane3f );

   void SetTo( Bounds3f& irBounds );

   Vector3f operator[]( int i );


   float TestNotOutsideMinX( ISpatialProxyAA& irSpatialProxy );
   float TestNotOutsideMinY( ISpatialProxyAA& irSpatialProxy );
   float TestNotOutsideMinZ( ISpatialProxyAA& irSpatialProxy );
   float TestNotOutsideMaxX( ISpatialProxyAA& irSpatialProxy );
   float TestNotOutsideMaxY( ISpatialProxyAA& irSpatialProxy );
   float TestNotOutsideMaxZ( ISpatialProxyAA& irSpatialProxy );

   enum
   {
      msPtCount = 8
   };

   Bounds3f mBounds;
protected:
};
#endif
*/