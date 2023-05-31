#include <render/culling/HexahedronP.h>

HexahedronP::HexahedronP()
{
}

HexahedronP::~HexahedronP()
{
}

// ISpatialProxyAA
// <0.0   -   Less than comparison object in posn
// =0.0   -   Intersection with comparison object 
// >0.0   -   Greater than comparison object in posn 

float  HexahedronP::CompareTo( AAPlane3f& irPlane )
{
   rAssert(false);
   return 0.0f;
}

bool   HexahedronP::DoesIntersect(     AAPlane3f& irClipPlane )
{
   rAssert(false);
   return false;
}

bool   HexahedronP::DoesntIntersect(   AAPlane3f& irClipPlane )
{
   rAssert(false);
   return false;
}

// ISpatialProxyAA
// <0.0   -   Inside Spatial Proxy
// =0.0   -   On Spatial Proxy Surface
// >0.0   -   Outside Spatial Proxy

float  HexahedronP::CompareTo( const Vector3f& irPoint )
{
   for( int i = 0; i < 6; i++ )
   {
      if(   mPlanes[i].mNorm.x * irPoint.x + 
            mPlanes[i].mNorm.y * irPoint.y + 
            mPlanes[i].mNorm.z * irPoint.z -
            mPlanes[i].mOffset                 < 0.0f )
      {
         return 1.0f; //Outside
      }
   }
   return -1.0f; //Inside
}


float HexahedronP::TestNotOutside( ISpatialProxyAA& irSpatialProxy )
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

int HexahedronP::nPts()
{
   return msPtCount;
}

Vector3f HexahedronP::mPt( int iIndex )
{
   return mPoints[iIndex];
}

NormPlane3f& HexahedronP::mPlane( int inPlane )
{
   return mPlanes[inPlane];
}

void HexahedronP::SetPlane( int inPlane, float iX, float iY, float iZ, float iD )
{
   mPlanes[inPlane].mNorm.x = iX;
   mPlanes[inPlane].mNorm.y = iY;
   mPlanes[inPlane].mNorm.z = iZ;
   mPlanes[inPlane].mOffset = iD;
//TODO: I'm pretty sure just the sign matters for our tests, check the validity of this,
 //  mPlanes[inPlane].Normalise();
}

//TODO: Currently I only care whether *any* point is within the bounding box of a tree
//       So, later, I can optimise this to just generate one point, and test vs that one
void HexahedronP::GeneratePoints()
{
   GeneratePoint( 0, msFront, msRight, msTop    );
   GeneratePoint( 1, msFront, msLeft,  msTop    );
   GeneratePoint( 2, msFront, msRight, msBottom );
   GeneratePoint( 3, msFront, msLeft,  msBottom );
   GeneratePoint( 4, msBack,  msRight, msTop    );
   GeneratePoint( 5, msBack,  msLeft,  msTop    );
   GeneratePoint( 6, msBack,  msRight, msBottom );
   GeneratePoint( 7, msBack,  msLeft,  msBottom );
}

//     - d1 (  N2 x N3 ) - d2 ( N3 x N1 ) - d3 ( N1 x N2 )  
//P =  -------------------------------------------------------------------------  
//           N1 . ( N2 x N3 )  
void HexahedronP::GeneratePoint( int inPoint, int inPlane1, int inPlane2, int inPlane3 )
{
   Vector3f tmp, tmp2;
   float div;

   tmp.CrossProduct( mPlanes[inPlane2].mNorm, mPlanes[inPlane3].mNorm );
   tmp.Mult( -1.0f * mPlanes[inPlane1].mOffset );

   tmp2.CrossProduct( mPlanes[inPlane3].mNorm, mPlanes[inPlane1].mNorm );
   tmp2.Mult( -1.0f * mPlanes[inPlane2].mOffset ); 

   tmp.Add( tmp2 );

   tmp2.CrossProduct( mPlanes[inPlane1].mNorm, mPlanes[inPlane2].mNorm );
   tmp2.Mult( -1.0f * mPlanes[inPlane3].mOffset );

   tmp.Add( tmp2 );

   tmp2.CrossProduct( mPlanes[inPlane2].mNorm, mPlanes[inPlane3].mNorm );
   div = mPlanes[inPlane1].mNorm.DotProduct( tmp2 );

   rAssert( div != 0.0f );

   tmp.Div( tmp, div );
   mPoints[inPoint] = tmp;
}


Vector3f HexahedronP::GetPoint()
{
   return mPoints[0];
}
