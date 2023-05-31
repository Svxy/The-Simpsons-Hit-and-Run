#ifndef __HEXAHEDRON_P_H___
#define __HEXAHEDRON_P_H___

#include <render/culling/ISpatialProxy.h>

class HexahedronP : public ISpatialProxyAA
{
public:
   HexahedronP();
   ~HexahedronP();

   /////////////////////////////////////////////////////////////////////
   //------------------ISpatialProxyAA methods------------------------//
   // <0.0   -   Less than comparison object in posn
   // =0.0   -   Intersection with comparison object 
   // >0.0   -   Greater than comparison object in posn 
	virtual float  CompareTo(       AAPlane3f& irPlane );
   virtual bool   DoesIntersect(   AAPlane3f& irClipPlane );
   virtual bool   DoesntIntersect( AAPlane3f& irClipPlane );

	virtual float  CompareTo(   const Vector3f& irPoint );

   virtual float     TestNotOutside( ISpatialProxyAA& irSpatialProxy );
   virtual int       nPts();
   virtual Vector3f  mPt( int iIndex );


   /////////////////////////////////////////////////////////////////////
   //----------------HexahedronP methods------------------------------//
   void SetPlane( int inPlane, float iX, float iY, float iZ, float iD );

   NormPlane3f& mPlane( int inPlane );

   //
   // Generate all the points, 
   // where the planes intersect
   //
   void GeneratePoints();

   virtual Vector3f GetPoint();

   enum
   {
      msFront,
      msBack,
      msTop,
      msBottom,
      msRight,
      msLeft
   };

   enum
   {
      msPtCount = 8,
      msEdgeCount = 12,
      msSideCount = 6
   };
protected:
   NormPlane3f mPlanes[msSideCount];
   Vector3f    mPoints[msPtCount];

   void GeneratePoint( int inPoint, int inPlane1, int inPlane2, int inPlane3 );

};
#endif
