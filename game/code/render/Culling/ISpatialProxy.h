#ifndef __I_SPATIAL_PROXY_H__
#define __I_SPATIAL_PROXY_H__

#include <render/culling/Plane3f.h>


class ISpatialProxyAA
{
public:
   ISpatialProxyAA(){}
   virtual ~ISpatialProxyAA() {}

   //
   // Implment this!
   //
   // Return values for all Comparison functions:
   //
   // This SpatialProxy is...
   // <0.0   -   Less than comparison object in posn
   // =0.0   -   Intersection with comparison object 
   // >0.0   -   Greater than comparison object in posn 
   //
   //    For normalized references, normal side is Greater, inverse is lesser
   //    For axis aligned refs, the Origin is the smallest value
   //    For axis aligned normalized refs, normal takes precedence
   //
	virtual float CompareTo( AAPlane3f&       irPlane ) = 0;

   // ISpatialProxyAA
   // <0.0   -   Inside Spatial Proxy
   // =0.0   -   On Spatial Proxy Surface
   // >0.0   -   Outside Spatial Proxy
   virtual float CompareTo( const Vector3f& irPoint ) =0;
   virtual float CompareToXZ( const Vector3f& irPoint ) =0;

   virtual float    TestNotOutside( ISpatialProxyAA& irSpatialProxy ) =0;

   virtual int      nPts() =0;
   virtual Vector3f mPt( int iIndex ) =0;


   virtual bool DoesIntersect(     AAPlane3f& irClipPlane ) =0;
   virtual bool DoesntIntersect(   AAPlane3f& irClipPlane ) =0;

   //Get a point representing some point within the SpatialProxy
   virtual Vector3f GetPoint() =0;

protected:
   static float msIntersectionEpsilon;
};


class IValidity
{
public:
   IValidity(){}
   virtual ~IValidity(){}

   virtual bool IsValid()     =0;
   virtual bool IsInvalid()   =0;
   virtual void Invalidate()  =0;

protected:
};

template <class T>
class IMutableSP :   public ISpatialProxyAA,
                     public IValidity
{
public:
   IMutableSP(){}
   virtual ~IMutableSP() {}

   //
   // Implment this!
   //
   // Return values for all Comparison functions:
   //
   // <0.0   -   Less than comparison object in posn
   // =0.0   -   Intersection with comparison object 
   // >0.0   -   Greater than comparison object in posn 
   //
   //    For normalized references, normal side is Greater, inverse is lesser
   //    For axis aligned refs, the Origin is the smallest value
   //    For axis aligned normalized refs, normal takes precedence
   //
	//virtual float CompareTo( AAPlane3f&       irPlane ) = 0;
	//virtual float CompareTo( AANormPlane3f&   irPlane ) = 0;

   //virtual void KeepLT( AAPlane3f& irClipPlane ) =0;
   //virtual void KeepGT( AAPlane3f& irClipPlane ) =0;

   //virtual void ClipOffLT( AAPlane3f& irClipPlane ) =0;
   //virtual void ClipOffGT( AAPlane3f& irClipPlane ) =0;

   virtual void ClipOffLT( IMutableSP<T>& irSpatialProxyLT, AAPlane3f& irClipPlane ) =0;
   virtual void ClipOffGT( IMutableSP<T>& irSpatialProxyGT, AAPlane3f& irClipPlane ) =0;

   virtual T* pT() =0;

protected:
};

/*
template <class T>
class ISpatialProxy : public ISpatialProxyAA<T>
{
public:
   ISpatialProxy(){}
   virtual ~ISpatialProxy() {}

   //
   // Implment this!
   //
   // Return values for all Comparison functions:
   //
   // <0.0   -   Less than comparison object in posn
   // =0.0   -   On/Inside comparison object 
   // >0.0   -   Greater than comparison object in posn 
   //
   //    For normalized references, normal side is Greater, inverse is lesser
   //    For axis aligned refs, the Origin is the smallest value
   //    For axis aligned normalized refs, normal takes precedence
   //
	virtual float CompareTo( Plane3f& irPlane ) = 0;
protected:
};
*/
/*
class PointSP : public Vector3f, public ISpatialProxy
{
   PointSP(){}
   ~PointSP(){}

	virtual float CompareTo( AAPlane3f& irPlane )
   {
      switch(irPlane.mAxis )
      {
      case 0:
         return irPlane.mPosn - x;
         break;
      case 1:
         return irPlane.mPosn - y;
         break;
      case 2:
         return irPlane.mPosn - z;
         break;
      }
   }

	virtual float CompareTo( AANormPlane3f& irPlane )
   {
      if( irPlane.mNorm == 0 )
      {
         switch(irPlane.mAxis )
         {
         case 0:
            return x - irPlane.mPosn;
            break;
         case 1:
            return y - irPlane.mPosn;
            break;
         case 2:
            return z - irPlane.mPosn;
            break;
         }
      }
      else
      {
         switch(irPlane.mAxis )
         {
         case 0:
            return irPlane.mPosn - x;
            break;
         case 1:
            return irPlane.mPosn - y;
            break;
         case 2:
            return irPlane.mPosn - z;
            break;
         }
      }
   }

	virtual float CompareTo( Plane3f& irPlane )
   {
      to do ........
   }
};
*/
#endif
