#ifndef __CELL_H__
#define __CELL_H__

#include <radmath/radmath.hpp>
#include <render/culling/FixedArray.h>
#include <render/culling/Vector3f.h>
#include <render/culling/Vector3i.h>
#include <render/culling/Bounds.h>
//
// Cell
//
//    A Cell is a collection of geometry or other render-salient data
// which is considered as a single chunk in space. It has static bounds
// as it is used to drive the OctTree culling algorithms. These bounds
// are unknown to the Cell, as it can be more effecient to combine the
// indexing scheme with the bounds.
//
// mRenderWeight  - Currently the number of vertices in the cell
// mCentroid      - A Vector representing the centroid of the cell
// 
class Cell
{
public:
   Cell();
   ~Cell();

   void AccVertexList( FixedArray<Vector3f>& irVertexList );
   void AccVertexList( rmt::Vector*   ipVector, int iCount );
   void AccVertexList( float*    ipVertex, int iCount );

   void AccVertex( Vector3f& irVertex );
   void AccVertex( Vector3f& irVertex, int& irWeight );
   void AccVertex( rmt::Vector& iVector );
   void AccVertex( float* ipVertex);
   void AccVertex( float iX, float iY, float iZ );

   void GenerateCell();

   float*   Centroid();
   float    Centroid( int iAxis );
   void     Centroid( float* ipCentroid );
   int      RenderWeight();

   void     SetBounds( Bounds3f& irBounds );
   float    MaxPlane( int iAxis );
   float    MinPlane( int iAxis );

   void        SetBlockIndex( Vector3i& irBlockIndex );
   int&        BlockIndex( int iAxis );
   Vector3i&   BlockIndex();

   //
   // Debug checking
   //
   bool IsGenerated();
   bool IsBoundsSet();
   bool IsIndexSet();
protected:
   float mpCentroid[3];
   int   mRenderWeight;

   Bounds3f mBounds;
   Vector3i mBlockIndex;
};

#endif