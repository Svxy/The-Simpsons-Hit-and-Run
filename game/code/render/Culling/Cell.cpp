#include <render/culling/Cell.h>
#include <raddebug.hpp>

Cell::Cell()
{
   mRenderWeight = 0;

   mpCentroid[0] = 0.0f;
   mpCentroid[1] = 0.0f;
   mpCentroid[2] = 0.0f;

   mBounds.mMax.x = 0.0f;
   mBounds.mMin.x = 0.0f;

   mBlockIndex[0] = -1;
}

Cell::~Cell()
{
}

////////////////////////////////////////////////////////////////////////
// What:
//    Accumulator functions use negative numbers to accumulate
//    Value is inverted on call to Generate Cell
// Why:
//    This is used to ensure access doesn't happen previous to generation
// Note:
//    The only holes in this type of assertion is where all the vertices 
//    are at X = 0, or there are no Vertices.
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// TODO:
//    Eliminate a linear factor out of these for loops:
//    Incriment RenderWeight by size at the end; reorganize.
////////////////////////////////////////////////////////////////////////


void Cell::AccVertexList( FixedArray<Vector3f>& irVertexList )
{
   int i;
   for( i=irVertexList.mSize; i>-1; i-- )
   {
      AccVertex( irVertexList[i] );
   }
}

void Cell::AccVertexList( rmt::Vector* ipVector, int iCount )
{
   int i;
   for( i=0; i<iCount; i++ )
   {
      AccVertex( ipVector[i] );
   }
}

void Cell::AccVertexList( float* ipVertex, int iCount )
{
   int i;
   iCount = iCount*3;
   
   for( i=0; i<iCount; i+=3 )
   {
      AccVertex( &(ipVertex[i]) );
   }
}

void Cell::AccVertex( Vector3f& irVector )
{
   mpCentroid[0] -= irVector.x;
   mpCentroid[1] -= irVector.y;
   mpCentroid[2] -= irVector.z;
   mRenderWeight++;
}

void Cell::AccVertex( Vector3f& irVertex, int& irWeight )
{
   float tmpWeight = (float)irWeight;
   mpCentroid[0] -= irVertex.x * tmpWeight;
   mpCentroid[1] -= irVertex.y * tmpWeight;
   mpCentroid[2] -= irVertex.z * tmpWeight;
   mRenderWeight += irWeight;
}


void Cell::AccVertex( rmt::Vector& irVector )
{
   mpCentroid[0] -= irVector.x;
   mpCentroid[1] -= irVector.y;
   mpCentroid[2] -= irVector.z;
   mRenderWeight++;
}

void Cell::AccVertex( float iX, float iY, float iZ )
{
   mpCentroid[0] -= iX;
   mpCentroid[1] -= iY;
   mpCentroid[2] -= iZ;
   mRenderWeight++;
}

void Cell::AccVertex( float* ipVertex )
{
   mpCentroid[0] -= ipVertex[0];
   mpCentroid[1] -= ipVertex[1];
   mpCentroid[2] -= ipVertex[2];
   mRenderWeight++;
}

void Cell::GenerateCell()
{
   if( mRenderWeight != 0 )
   {
      mpCentroid[0] = mpCentroid[0]/(-mRenderWeight);
      mpCentroid[1] = mpCentroid[1]/(-mRenderWeight);
      mpCentroid[2] = mpCentroid[2]/(-mRenderWeight);
   }
   
}

float* Cell::Centroid()
{
   rAssert( IsGenerated() );
   return mpCentroid;
}

float Cell::Centroid( int iAxis )
{
   rAssert( IsGenerated() );
   rAssert( iAxis < 3 );
   return mpCentroid[iAxis];
}

void Cell::Centroid( float* ipCentroid )
{
   rAssert( IsGenerated() );
 
   ipCentroid[0] = mpCentroid[0];
   ipCentroid[1] = mpCentroid[1];
   ipCentroid[2] = mpCentroid[2];
}

int Cell::RenderWeight()
{
   rAssert( IsGenerated() );
   return mRenderWeight;
}


void Cell::SetBounds( Bounds3f& irBounds )
{
   mBounds = irBounds;

   rAssert( mBounds.mMin.x != mBounds.mMax.x );
   rAssert( mBounds.mMin.y != mBounds.mMax.y );
   rAssert( mBounds.mMin.z != mBounds.mMax.z );
}

float Cell::MaxPlane( int iAxis )
{
   rAssert( IsGenerated() );
   rAssert( IsBoundsSet() );
   return mBounds.mMax[iAxis];
}

float Cell::MinPlane( int iAxis )
{
   rAssert( IsGenerated() );
   rAssert( IsBoundsSet() );
   return mBounds.mMin[iAxis];
}


void Cell::SetBlockIndex( Vector3i& irBlockIndex )
{
   mBlockIndex = irBlockIndex;
}

int& Cell::BlockIndex( int iAxis )
{
   rAssert( IsGenerated() );
   rAssert( IsIndexSet() );
   return mBlockIndex[iAxis];
}

Vector3i& Cell::BlockIndex()
{
   rAssert( IsGenerated() );
   rAssert( IsIndexSet() );
   return mBlockIndex;
}

bool Cell::IsGenerated()
{
  // return (mpCentroid[0] >= 0.0f);
   return true;
}

bool Cell::IsBoundsSet()
{
   return ( !((mBounds.mMin.x == 0.0f)&&(mBounds.mMax.x == 0.0f)) );
}

bool Cell::IsIndexSet()
{
   return (mBlockIndex[0] >= 0);
}
