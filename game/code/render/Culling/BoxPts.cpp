#include <render/culling/BoxPts.h>
#include <debug/profiler.h>
   // ISpatialProxyAA

// <0.0   -   Less than comparison object in posn
// =0.0   -   Intersection with comparison object 
// >0.0   -   Greater than comparison object in posn 
float BoxPts::CompareTo( AAPlane3f&       irPlane ) 
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

bool BoxPts::DoesIntersect(     AAPlane3f& irClipPlane )
{
   rAssert(false);
   return true;
}

bool BoxPts::DoesntIntersect(   AAPlane3f& irClipPlane )
{
   rAssert(false);
   return true;
}


float BoxPts::TestNotOutsideMinX( ISpatialProxyAA& irSpatialProxy )
{
   for( int i=irSpatialProxy.nPts()-2; i>-1; i-- )
   {
      if( irSpatialProxy.mPt(i).x >= mBounds.mMin.x )
      {
         //Inside
         return -1.0f;
      }
   }
   //Outside
   return 1.0f;
}

float BoxPts::TestNotOutsideMinY( ISpatialProxyAA& irSpatialProxy )
{
   for( int i=irSpatialProxy.nPts()-2; i>-1; i-- )
   {
      if( irSpatialProxy.mPt(i).y >= mBounds.mMin.y )
      {
         //Inside
         return -1.0f;
      }
   }
   //Outside
   return 1.0f;
}

float BoxPts::TestNotOutsideMinZ( ISpatialProxyAA& irSpatialProxy )
{
   for( int i=irSpatialProxy.nPts()-2; i>-1; i-- )
   {
      if( irSpatialProxy.mPt(i).z >= mBounds.mMin.z )
      {
         //Inside
         return -1.0f;
      }
   }
   //Outside
   return 1.0f;
}

float BoxPts::TestNotOutsideMaxX( ISpatialProxyAA& irSpatialProxy )
{
   for( int i=irSpatialProxy.nPts()-2; i>-1; i-- )
   {
      if( irSpatialProxy.mPt(i).x <= mBounds.mMax.x )
      {
         //Inside
         return -1.0f;
      }
   }
   //Outside
   return 1.0f;
}

float BoxPts::TestNotOutsideMaxY( ISpatialProxyAA& irSpatialProxy )
{
   for( int i=irSpatialProxy.nPts()-2; i>-1; i-- )
   {
      if( irSpatialProxy.mPt(i).y <= mBounds.mMax.y )
      {
         //Inside
         return -1.0f;
      }
   }
   //Outside
   return 1.0f;
}

float BoxPts::TestNotOutsideMaxZ( ISpatialProxyAA& irSpatialProxy )
{
   for( int i=irSpatialProxy.nPts()-2; i>-1; i-- )
   {
      if( irSpatialProxy.mPt(i).z <= mBounds.mMax.z )
      {
         //Inside
         return -1.0f;
      }
   }
   //Outside
   return 1.0f;
}

int BoxPts::nPts()
{
   return msPtCount;
}

Vector3f BoxPts::mPt( int iIndex )
{
   switch(iIndex)
   {
   case 0:
      return mBounds.mMin;
      break;

   case 1:
      return Vector3f( mBounds.mMin.x, mBounds.mMin.y, mBounds.mMax.z );
      break;

   case 2:
      return Vector3f( mBounds.mMin.x, mBounds.mMax.y, mBounds.mMin.z );
      break;

   case 3:
      return Vector3f( mBounds.mMax.x, mBounds.mMin.y, mBounds.mMin.z );
      break;

   case 4:
      return Vector3f( mBounds.mMin.x, mBounds.mMax.y, mBounds.mMax.z );
      break;

   case 5:
      return Vector3f( mBounds.mMax.x, mBounds.mMax.y, mBounds.mMin.z );
      break;

   case 6:
      return Vector3f( mBounds.mMax.x, mBounds.mMin.y, mBounds.mMax.z );
      break;

   case 7:
      return mBounds.mMax;
      break;

   default:
      rAssert(false);
      return mBounds.mMin;
      break;

      /*
      m[0] = irBounds.mMin;

      m[1].x = irBounds.mMin.x;
      m[1].y = irBounds.mMin.y;
      m[1].z = irBounds.mMax.z;

      m[2].x = irBounds.mMin.x;
      m[2].y = irBounds.mMax.y;
      m[2].z = irBounds.mMin.z;

      m[3].x = irBounds.mMax.x;
      m[3].y = irBounds.mMin.y;
      m[3].z = irBounds.mMin.z;

      m[4].x = irBounds.mMin.x;
      m[4].y = irBounds.mMax.y;
      m[4].z = irBounds.mMax.z;

      m[5].x = irBounds.mMax.x;
      m[5].y = irBounds.mMax.y;
      m[5].z = irBounds.mMin.z;

      m[6].x = irBounds.mMax.x;
      m[6].y = irBounds.mMin.y;
      m[6].z = irBounds.mMax.z;

      m[7] = irBounds.mMax;
      */
   }
}

Vector3f BoxPts::GetPoint()
{
   return mBounds.mMin;
}

void BoxPts::CutOffGT( AAPlane3f& irPlane3f )
{
   if( mBounds.mMax[(int)irPlane3f.mAxis] > irPlane3f.mPosn )
   {
      mBounds.mMax[(int)irPlane3f.mAxis] = irPlane3f.mPosn; 

      if( mBounds.mMin[(int)irPlane3f.mAxis] > irPlane3f.mPosn )
      {
         mBounds.mMin[(int)irPlane3f.mAxis] = irPlane3f.mPosn; 
      }
   }
   //Temporary debug;
   //Though it's conceivable that you'd want to cut a box down into a plane, 
   //SpatialIterators don't want to.
   rAssert( mBounds.mMin[(int)irPlane3f.mAxis] != mBounds.mMax[(int)irPlane3f.mAxis] );
}

void BoxPts::CutOffLT( AAPlane3f& irPlane3f )
{
   if( mBounds.mMin[(int)irPlane3f.mAxis] < irPlane3f.mPosn )
   {
      mBounds.mMin[(int)irPlane3f.mAxis] = irPlane3f.mPosn; 

      if( mBounds.mMax[(int)irPlane3f.mAxis] < irPlane3f.mPosn )
      {
         mBounds.mMax[(int)irPlane3f.mAxis] = irPlane3f.mPosn; 
      }
   }
   //Temporary debug;
   //Though it's conceivable that you'd want to cut a box down into a plane, 
   //SpatialIterators don't want to.
   rAssert( mBounds.mMin[(int)irPlane3f.mAxis] != mBounds.mMax[(int)irPlane3f.mAxis] );
}


float BoxPts::CompareTo( const Vector3f& irPoint )
{
   if( irPoint.x < mBounds.mMin.x )
      return 1.0f;

   if( irPoint.y < mBounds.mMin.y )
      return 1.0f;

   if( irPoint.z < mBounds.mMin.z )
      return 1.0f;

   if( irPoint.x > mBounds.mMax.x )
      return 1.0f;

   if( irPoint.y > mBounds.mMax.y )
      return 1.0f;

   if( irPoint.z > mBounds.mMax.z )
      return 1.0f;

   return -1.0f;
}
float BoxPts::CompareToXZ( const Vector3f& irPoint )
{
   if( irPoint.x < mBounds.mMin.x )
      return 1.0f;

   if( irPoint.z < mBounds.mMin.z )
      return 1.0f;

   if( irPoint.x > mBounds.mMax.x )
      return 1.0f;

   if( irPoint.z > mBounds.mMax.z )
      return 1.0f;

   return -1.0f;
}

void BoxPts::SetTo( Bounds3f& irBounds )
{
   mBounds = irBounds;
}

Vector3f BoxPts::operator[]( int i )
{
   return mPt(i);

}

// <0.0   -   Inside Spatial Proxy
// =0.0   -   On Spatial Proxy Surface
// >0.0   -   Outside Spatial Proxy
float BoxPts::TestNotOutside( ISpatialProxyAA& irSpatialProxy )
{
//BEGIN_PROFILE("BoxPts::TestNotOutside")
   Vector3f testPt( irSpatialProxy.mPt(irSpatialProxy.nPts()-1) );

   //Test the first Pt, if it's outside a plane, 
   //ensure all other pts are outside the same plane
   //Gives false positives for crossing the corner on the outside
   /*
   if( testPt.x < mBounds.mMin.x )
      return TestNotOutsideMinX( irSpatialProxy );
   
   if( testPt.y < mBounds.mMin.y )
      return TestNotOutsideMinY( irSpatialProxy );

   if( testPt.z < mBounds.mMin.z )
      return TestNotOutsideMinZ( irSpatialProxy );

   if( testPt.x > mBounds.mMax.x )
      return TestNotOutsideMaxX( irSpatialProxy );

   if( testPt.y > mBounds.mMax.y )
      return TestNotOutsideMaxY( irSpatialProxy );

   if( testPt.z > mBounds.mMax.z )
      return TestNotOutsideMinZ( irSpatialProxy );
   */

   if( testPt.x < mBounds.mMin.x )
   {
      if( TestNotOutsideMinX( irSpatialProxy ) > 0.0f )
      {
//END_PROFILE("BoxPts::TestNotOutside")
         return 1.0f;
      }
   }   
   else if( testPt.x > mBounds.mMax.x )
   {
      if( TestNotOutsideMaxX( irSpatialProxy ) > 0.0f )
      {
//END_PROFILE("BoxPts::TestNotOutside")
         return 1.0f;
      }
   }
   
   if( testPt.y < mBounds.mMin.y )
   {
      if( TestNotOutsideMinY( irSpatialProxy ) > 0.0f )
      {
//END_PROFILE("BoxPts::TestNotOutside")
         return 1.0f;
      }
   }
   else if( testPt.y > mBounds.mMax.y )
   {
      if( TestNotOutsideMaxY( irSpatialProxy ) > 0.0f )
      {
//END_PROFILE("BoxPts::TestNotOutside")
         return 1.0f;
      }
   }

   if( testPt.z < mBounds.mMin.z )
   {
      if( TestNotOutsideMinZ( irSpatialProxy ) > 0.0f )
      {
//END_PROFILE("BoxPts::TestNotOutside")
         return 1.0f;
      }
   }
   else if( testPt.z > mBounds.mMax.z )
   {
      if( TestNotOutsideMaxZ( irSpatialProxy ) > 0.0f )
      {
//END_PROFILE("BoxPts::TestNotOutside")
         return 1.0f;
      }
   }
//END_PROFILE("BoxPts::TestNotOutside")
   return -1.0f;
}
