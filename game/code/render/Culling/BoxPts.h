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
   virtual float CompareToXZ( const Vector3f& irPoint );

   virtual float  TestNotOutside( ISpatialProxyAA& irSpatialProxy );

   virtual int       nPts();
   virtual Vector3f  mPt( int iIndex );

   virtual bool DoesIntersect(     AAPlane3f& irClipPlane );
   virtual bool DoesntIntersect(   AAPlane3f& irClipPlane );

   //Get a point representing some point within the SpatialProxy
   virtual Vector3f GetPoint();

   void CutOffGT( AAPlane3f& irPlane3f );
   void CutOffLT( AAPlane3f& irPlane3f );

   virtual void SetTo( Bounds3f& irBounds );

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
