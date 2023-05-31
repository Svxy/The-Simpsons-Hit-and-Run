#ifndef __SPATIAL_TREE_ITER_H__
#define __SPATIAL_TREE_ITER_H__

#include <p3d/utility.hpp>
#include <render/culling/SpatialTree.h>
#include <render/culling/ISpatialProxy.h>
#include <render/culling/SphereSP.h>
#include <render/culling/Bounds.h>
#include <render/culling/BoxPts.h>
#include <render/culling/FixedArray.h>
#include <render/culling/UseArray.h>

//#include <render/culling/../../profiler/profiler.hpp>

typedef int tMark;


class SpatialTreeIter
{
public:
   SpatialTreeIter();
   ~SpatialTreeIter();

   void SetToRoot( SpatialTree& irTree );
   void Clear();

   //Allocation&Placement Navigation
/*
   void              Place(          ISpatialProxyAA& irTVolume, T*  ipT );
   void              ReservePlaceT(  ISpatialProxyAA& irTVolume, int iCount );
   void              Place(          ISpatialProxyAA& irTVolume, D*  ipD );
   void              ReservePlaceD(  ISpatialProxyAA& irTVolume, int iCount );
   void              Place(          ISpatialProxyAA& irIVolume, I*  ipI );
   void              ReservePlaceI(  ISpatialProxyAA& irIVolume, int iCount );
   void              AllocateAllReservations();
*/

   //Node Retrieval Methods
   SpatialNode&   rSeekNode(     ISpatialProxyAA& irTVolume, 
                                        int              iCurNodeOffset=0 );
   SpatialNode&   rIthNode(      int              iIth );
   int                   NumNodes();

   void                  SeekSubNodes(  ISpatialProxyAA& irTVolume,
                                        UseArray<int>&   orNodeOffsets, 
                                        int              iCurNodeOffset=0 );
   void                  SeekAllNodes(  ISpatialProxyAA& irTVolume,
                                        UseArray<int>&   orNodeOffsets, 
                                        int              iCurNodeOffset=0 );
   void                  SetUpNodeList( UseArray<int>&   orNodeOffsets );

   //Iteration Initialization
   void              MarkAll( SphereSP& irDesiredVolume, tMark iMark );
   void              MarkAllSphere( SphereSP& irSphere, tMark iMark );
   void              MarkSubTrees( ISpatialProxyAA& irDesiredVolume, tMark iMark );
   void              MarkTree( tMark iMark );
   void              AndTree(  tMark iMark );
   void              OrTree(   tMark iMark );
   void              OrTreeVis(   tMark iMark );


   //Iteration Navigation
   SpatialNode&   rSeekLeaf( Vector3f& irPt );

   void              SetIterFilter( tMark iMark );
   void              MoveToFirst();
   
   SpatialNode&   rCurrent();
   inline SpatialNode*   pCurrent();

   void              MoveToNext(bool ibIncludeVis=false);
   SpatialNode&   rMoveToNext(bool ibIncludeVis=false);
   SpatialNode*   pMoveToNext(bool ibIncludeVis=false);
   
   bool              NotDone();
   bool              IsCurrentLeaf();

   //Debug Functionality
//   void              DisplayBoundingBox();
   void              DisplayCurrentBoundingBox( const tColour& irColour );
   bool              IsSetUp();
   BoxPts*           CurrentBBox();
   
   BoxPts&           rBBox();
   void              BuildBBoxes( BoxPts iBoxPts, int iCurNodeOffset = 0);

   enum 
   {
      msFilterInvisible = 0xF0000000,  
      msFilterVisible   = 0x0F000000,
      msFilterAll       = 0x00FFFFFF
   };

   SwapArray<SpatialNode*> mCurNodes;
   SwapArray<SpatialNode*> mCurAlwaysVisNodes;

protected:
   SwapArray<SpatialNode*>* mpCurNodeList;
   ContiguousBinNode< SpatialNode >* mpRootNode;
   ContiguousBinNode< SpatialNode >* mpCurNode;
   
   int mCurNodeOffset;

   FixedArray<tMark>   mNodeMarks;
   tMark               mCurMarkFilter;

   int mCurNodeI;

   //Bounding Box is left as series of defining points to make iteration simpler
   BoxPts mBBox;


   SpatialNode& rSeekLeaf( int iCurNodeOffset, Vector3f& irPt );

   void MarkAll( int                ipCurNode,
                 BoxPts&            irBoxPts, 
                 SphereSP&   irDesiredVolume, 
                 tMark              iMark );

   void MarkSubTrees( int                ipCurNode,
                      BoxPts&            irBoxPts, 
                      ISpatialProxyAA&   irDesiredVolume, 
                      tMark              iMark );

   void MarkAllSphere(  int         ipCurNode,
                        BoxPts&     irBoxPts, 
                        SphereSP&   irSphere, 
                        tMark       iMark );

   void MarkAllSphereIntersect(  int        ipCurNode,
                                BoxPts&     irBoxPts, 
                                SphereSP&   irSphere, 
                                tMark       iMark );

   void MarkSubTree( int   iCurNodeOffset,
                     tMark iMark );

/*
   void Place(           int iCurNodeOffset, ISpatialProxyAA& irTVolume, T*  ipT );
   void ReservePlaceT(   int iCurNodeOffset, ISpatialProxyAA& irTVolume, int iCount );
   void Place(           int iCurNodeOffset, ISpatialProxyAA& irTVolume, D*  ipD );
   void ReservePlaceD(   int iCurNodeOffset, ISpatialProxyAA& irTVolume, int iCount );
   void Place(           int iCurNodeOffset, ISpatialProxyAA& irIVolume, I*  ipI );
   void ReservePlaceI(   int iCurNodeOffset, ISpatialProxyAA& irIVolume, int iCount );
*/
   //Debug functionality
//   void DisplayBoundingBox( int iCurNodeOffset, BoxPts& orBBox, tColour colour );
   void DisplayBoundingBox( BoxPts& irBBox, tColour colour );

};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline 
SpatialTreeIter::SpatialTreeIter()
:    mpRootNode( NULL )
{
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
bool SpatialTreeIter::IsSetUp()
{
   return ( mpRootNode != NULL );
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
SpatialTreeIter::~SpatialTreeIter()
{
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::SetToRoot( SpatialTree& irTree )
{
   mpRootNode = irTree.GetRoot();

   mBBox.SetTo( irTree.GetBounds() );

   mNodeMarks.Allocate( mpRootNode->GetSubTreeSize()+1 );
   mCurNodes.Allocate( mpRootNode->GetSubTreeSize()+1 );
   mCurAlwaysVisNodes.Allocate( mpRootNode->GetSubTreeSize()+1 );
   mpCurNodeList = &mCurNodes;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::Clear()
{
   mpRootNode  = NULL;
}

//========================================================================
// SpatialTreeIter::
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
inline 
SpatialNode&   SpatialTreeIter::rIthNode
(
   int              iIth
)
{
   return (mpRootNode+iIth)->mData;
}
//========================================================================
// SpatialTreeIter::
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
inline  
int   SpatialTreeIter::NumNodes
(
)
{
   return (mpRootNode->GetSubTreeSize()+1);
}

//========================================================================
// SpatialTreeIter::
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
inline  
void SpatialTreeIter::SetUpNodeList
( 
   UseArray<int>&   orNodeOffsets 
)
{
   orNodeOffsets.Allocate( mpRootNode->GetSubTreeSize()+1 );
}
/*
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::ReservePlaceD(   ISpatialProxyAA& irDVolume, int iCount)
{
   ReservePlaceD( 0, irDVolume, iCount );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::ReservePlaceD
( 
   int               iCurNodeOffset, 
   ISpatialProxyAA&  irDVolume, 
   int               iCount
)
{
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);

   if( pCurNode->GetSubTreeSize() > 0 )
   {
      float PlaneVolResult = irDVolume.CompareTo( pCurNode->mData.mSubDivPlane );

      if( PlaneVolResult > 0.0f ) //The Plane is greater than TVolume in Posn, so TVolume is in the LT Partitiion..
      {
         ReservePlaceD( iCurNodeOffset + pCurNode->LChildOffset(), irDVolume, iCount );
      } 
      else
      {
         if( PlaneVolResult < 0.0f ) //The Plane is less han TVolume in Posn, so TVolume is in the GT Partitiion..
         {
            ReservePlaceD( iCurNodeOffset + pCurNode->RChildOffset(), irDVolume, iCount );
         }
         else //  PlaneVolResult == 0 
         {
            //pCurNode = pCurNode->Parent();
            pCurNode->mData.mDynamicElems.Reserve(iCount);
         }
      }
   }
   else
   {
      pCurNode->mData.mDynamicElems.Reserve(iCount);
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::ReservePlaceT(   ISpatialProxyAA& irTVolume, int iCount)
{
   ReservePlaceT( 0, irTVolume, iCount );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::ReservePlaceT
( 
   int               iCurNodeOffset, 
   ISpatialProxyAA&  irTVolume, 
   int               iCount
)
{
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);

   if( pCurNode->GetSubTreeSize() > 0 )
   {
      float PlaneVolResult = irTVolume.CompareTo( pCurNode->mData.mSubDivPlane );

      if( PlaneVolResult > 0.0f ) //The Plane is greater than TVolume in Posn, so TVolume is in the LT Partitiion..
      {
         ReservePlaceT( iCurNodeOffset + pCurNode->LChildOffset(), irTVolume, iCount );
      } 
      else
      {
         if( PlaneVolResult < 0.0f ) //The Plane is less han TVolume in Posn, so TVolume is in the GT Partitiion..
         {
            ReservePlaceT( iCurNodeOffset + pCurNode->RChildOffset(), irTVolume, iCount );
         }
         else //  PlaneVolResult == 0 
         {
            //pCurNode = pCurNode->Parent();
            pCurNode->mData.mSpatialElems.Reserve(iCount);
         }
      }
   }
   else
   {
      pCurNode->mData.mSpatialElems.Reserve(iCount);
   }
}
*/
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
SpatialNode& SpatialTreeIter::rSeekLeaf( Vector3f& irPt )
{
   return rSeekLeaf( 0, irPt );
}
/*
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::ReservePlaceI(   ISpatialProxyAA& irIVolume, int iCount)
{
   ReservePlaceI( 0, irIVolume, iCount );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::ReservePlaceI
( 
   int               iCurNodeOffset, 
   ISpatialProxyAA&  irIVolume, 
   int               iCount
)
{
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);

   if( pCurNode->GetSubTreeSize() > 0 )
   {
      float PlaneVolResult = irIVolume.CompareTo( pCurNode->mData.mSubDivPlane );

      if( PlaneVolResult > 0.0f ) //The Plane is greater than TVolume in Posn, so TVolume is in the LT Partitiion..
      {
         ReservePlaceI( iCurNodeOffset + pCurNode->LChildOffset(), irIVolume, iCount );
      } 
      else
      {
         if( PlaneVolResult < 0.0f ) //The Plane is less han TVolume in Posn, so TVolume is in the GT Partitiion..
         {
            ReservePlaceI( iCurNodeOffset + pCurNode->RChildOffset(), irIVolume, iCount );
         }
         else //  PlaneVolResult == 0 
         {
            //pCurNode = pCurNode->Parent();
            pCurNode->mData.mIntersectElems.Reserve(iCount);
         }
      }
   }
   else
   {
      pCurNode->mData.mIntersectElems.Reserve(iCount);
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::Place(   ISpatialProxyAA& irDVolume,       D* ipD )
{
   Place( 0, irDVolume, ipD );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::Place
( 
   int               iCurNodeOffset, 
   ISpatialProxyAA&  irDVolume, 
   D*                ipD
)
{
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);

   if( pCurNode->GetSubTreeSize() > 0 )
   {
      float PlaneVolResult = irDVolume.CompareTo( pCurNode->mData.mSubDivPlane );

      if( PlaneVolResult > 0.0f ) //The Plane is greater than TVolume in Posn, so TVolume is in the LT Partitiion..
      {
         Place( iCurNodeOffset + pCurNode->LChildOffset(), irDVolume, ipD );
      } 
      else
      {
         if( PlaneVolResult < 0.0f ) //The Plane is less han TVolume in Posn, so TVolume is in the GT Partitiion..
         {
            Place( iCurNodeOffset + pCurNode->RChildOffset(), irDVolume, ipD );
         }
         else //  PlaneVolResult == 0 
         {
            //pCurNode = pCurNode->Parent();
            pCurNode->mData.mDynamicElems.Add(ipD);
         }
      }
   }
   else
   {
      pCurNode->mData.mDynamicElems.Add(ipD);
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::Place(   ISpatialProxyAA& irTVolume,       T* ipT )
{
   Place( 0, irTVolume, ipT );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::Place
( 
   int               iCurNodeOffset, 
   ISpatialProxyAA&  irTVolume, 
   T*                ipT
)
{
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);

   if( pCurNode->GetSubTreeSize() > 0 )
   {
      float PlaneVolResult = irTVolume.CompareTo( pCurNode->mData.mSubDivPlane );

      if( PlaneVolResult > 0.0f ) //The Plane is greater than TVolume in Posn, so TVolume is in the LT Partitiion..
      {
         Place( iCurNodeOffset + pCurNode->LChildOffset(), irTVolume, ipT );
      } 
      else
      {
         if( PlaneVolResult < 0.0f ) //The Plane is less han TVolume in Posn, so TVolume is in the GT Partitiion..
         {
            Place( iCurNodeOffset + pCurNode->RChildOffset(), irTVolume, ipT );
         }
         else //  PlaneVolResult == 0 
         {
            //pCurNode = pCurNode->Parent();
            pCurNode->mData.mSpatialElems.Add(ipT);
         }
      }
   }
   else
   {
      pCurNode->mData.mSpatialElems.Add(ipT);
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::Place(   ISpatialProxyAA& irIVolume,       I* ipI )
{
   Place( 0, irIVolume, ipI );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::Place
( 
   int               iCurNodeOffset, 
   ISpatialProxyAA&  irIVolume, 
   I*                ipI
)
{
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);

   if( pCurNode->GetSubTreeSize() > 0 )
   {
      float PlaneVolResult = irIVolume.CompareTo( pCurNode->mData.mSubDivPlane );

      if( PlaneVolResult > 0.0f ) //The Plane is greater than TVolume in Posn, so TVolume is in the LT Partitiion..
      {
         Place( iCurNodeOffset + pCurNode->LChildOffset(), irIVolume, ipI );
      } 
      else
      {
         if( PlaneVolResult < 0.0f ) //The Plane is less han TVolume in Posn, so TVolume is in the GT Partitiion..
         {
            Place( iCurNodeOffset + pCurNode->RChildOffset(), irIVolume, ipI );
         }
         else //  PlaneVolResult == 0 
         {
            //pCurNode = pCurNode->Parent();
            pCurNode->mData.mIntersectElems.Add(ipI);
         }
      }
   }
   else
   {
      pCurNode->mData.mIntersectElems.Add(ipI);
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::AllocateAllReservations()
{
   int TreeSize = mpRootNode->GetSubTreeSize();

   for( int i=0; i<=TreeSize; i++ )
   {
      (mpRootNode+i)->mData.mSpatialElems.Allocate();
      (mpRootNode+i)->mData.mDynamicElems.Allocate();
      (mpRootNode+i)->mData.mIntersectElems.Allocate();
   }
}
*/
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::MarkAll( SphereSP& irDesiredVolume, tMark iMark )
{
   mCurNodes.ClearUse();
   BoxPts tmpBBox(mBBox);
   MarkAll( 0, tmpBBox, irDesiredVolume, iMark );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::MarkAllSphere( SphereSP& irSphere, tMark iMark )
{
   mCurNodes.ClearUse();
   BoxPts tmpBBox(mBBox);
   MarkAllSphere( 0, tmpBBox, irSphere, iMark );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::MarkSubTrees( ISpatialProxyAA& irDesiredVolume, tMark iMark )
{
   BoxPts tmpBBox(mBBox);
   MarkSubTrees( 0, tmpBBox, irDesiredVolume, iMark );   
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/*inline  
void SpatialTreeIter::CallBackAll( ISpatialProxyAA& irDesiredVolume, void (*pCB)(SpatialNode*) )
{
   BoxPts tmpBBox(mBBox);
   CallBackAll( 0, tmpBBox, irDesiredVolume, pCB );   
}
*/
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
BoxPts* SpatialTreeIter::CurrentBBox()
{
   return &(mpCurNode->mData.mBBox);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/*
 
void SpatialTreeIter::DisplayBoundingBox( )
{
   BoxPts tmpBBox( mBBox );

   DisplayBoundingBox( 0, tmpBBox, tColour(0,255,0) );
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
 
void SpatialTreeIter::DisplayBoundingBox( int iCurNodeOffset, BoxPts& orBBox, tColour colour )
{
   DisplayBoundingBox( orBBox, colour );

   
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);

   if( pCurNode->GetSubTreeSize() > 0 )
   {
      //split subspace representation, and recurse
      BoxPts BBoxLT;
      BBoxLT = orBBox;

      BBoxLT.CutOffGT( pCurNode->mData.mSubDivPlane );

      DisplayBoundingBox( iCurNodeOffset + pCurNode->LChildOffset(), BBoxLT, colour );   

      orBBox.CutOffLT( pCurNode->mData.mSubDivPlane );

      DisplayBoundingBox( iCurNodeOffset + pCurNode->RChildOffset(), orBBox, colour );
   }
}
*/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline 
BoxPts& SpatialTreeIter::rBBox()
{
   return mBBox;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::DisplayBoundingBox( BoxPts& irBBox, tColour colour )
{
//#if PURE3D_VERSION_MAJOR < 15
//   tShader* testMat = p3d::find<tShader>("p3d_default");
//#endif

//#if PURE3D_VERSION_MAJOR < 15
//   pddiPrimStream* stream = p3d::pddi->BeginPrims(testMat->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C );
//#else
   pddiPrimStream* stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
//#endif
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMin.z);
   p3d::pddi->EndPrims(stream);

//#if PURE3D_VERSION_MAJOR < 15
//   stream = p3d::pddi->BeginPrims(testMat->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C );
//#else
   stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
//#endif
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMax.z);
   p3d::pddi->EndPrims(stream);

//#if PURE3D_VERSION_MAJOR < 15
//   stream = p3d::pddi->BeginPrims(testMat->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C );
//#else
   stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
//#endif
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMax.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMax.z);
   p3d::pddi->EndPrims(stream);

//#if PURE3D_VERSION_MAJOR < 15
//   stream = p3d::pddi->BeginPrims(testMat->GetShader(), PDDI_PRIM_LINESTRIP, PDDI_V_C );
//#else
   stream = p3d::pddi->BeginPrims(NULL, PDDI_PRIM_LINESTRIP, PDDI_V_C, 5);
//#endif
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMax.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMin.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMin.z);
   stream->Colour(colour);
   stream->Coord(irBBox.mBounds.mMin.x, irBBox.mBounds.mMax.y, irBBox.mBounds.mMax.z);
   p3d::pddi->EndPrims(stream);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
SpatialNode*   SpatialTreeIter::pCurrent()
{
   return (*mpCurNodeList)[mCurNodeI];
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::DisplayCurrentBoundingBox( const tColour& irColour )
{
   DisplayBoundingBox( pCurrent()->mBBox, irColour );
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::MarkTree( tMark iMark )
{
   for( int i=mNodeMarks.mSize-1; i>= 0; i-- )
   {
      mNodeMarks[i] = iMark;
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::AndTree(  tMark iMark )
{
    //If we're clearing all flags including the visibility flags, then we want 
    //to clear away our extra visible set of nodes
   if(iMark==0x00000000) 
   {
      mCurAlwaysVisNodes.ClearUse();
   }

   for( int i=mNodeMarks.mSize-1; i>= 0; i-- )
   {
      mNodeMarks[i] &= iMark;
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::OrTree(   tMark iMark )
{
   for( int i=mNodeMarks.mSize-1; i>= 0; i-- )
   {
      mNodeMarks[i] |= iMark;
   }
}
inline  
void SpatialTreeIter::OrTreeVis(   tMark iMark )
{
   mCurNodes.ClearUse();
   SpatialNode* pFuckinCast;
   for( int i=mNodeMarks.mSize-1; i>= 0; i-- )
   {
      mNodeMarks[i] |= iMark;
      pFuckinCast = &(mpRootNode[i].mData);
      mCurNodes.Add( pFuckinCast );
   }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline 
void SpatialTreeIter::MarkSubTree(  int   iCurNodeOffset,
                                       tMark iMark )
{
  SpatialNode* pSNCastPtr;
   ContiguousBinNode< SpatialNode >* pCurNode = (mpRootNode+iCurNodeOffset);
   int StopCondition = iCurNodeOffset+pCurNode->GetSubTreeSize();
   for( int i=iCurNodeOffset; i<=StopCondition; i++, pCurNode++ )
   {
      if((iMark & msFilterVisible)&&(mNodeMarks[i] & msFilterInvisible))
      {
          //Skip over invisible branches, if we're doing an visible pass
          i+=pCurNode->GetSubTreeSize();
          pCurNode+=pCurNode->GetSubTreeSize();
      }
      else
      {
          mNodeMarks[i] |= iMark;
          pSNCastPtr = &(pCurNode->mData);
          (*mpCurNodeList).Add( pSNCastPtr );
      }
   }
  
 
/*     SpatialNode* pSNCastPtr;
 
    for( int i=mpRootNode->GetSubTreeSize(); i>=iCurNodeOffset; i-- )
    {
        mNodeMarks[i] |= iMark;
        pSNCastPtr = &(this->rIthNode(i));
        mCurNodes.Add(pSNCastPtr);
    }
  */  
/*
   mNodeMarks[iCurNodeOffset] = iMark;
   
   if( (mpRootNode+iCurNodeOffset)->GetSubTreeSize() > 0 )
   {
      int newOffset =  (mpRootNode+iCurNodeOffset)->LChildOffset();
      MarkSubTree( newOffset + iCurNodeOffset, iMark ); 

      newOffset =  (mpRootNode+iCurNodeOffset)->RChildOffset();
      MarkSubTree( newOffset + iCurNodeOffset, iMark ); 
   }
*/
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::SetIterFilter( tMark iMark )
{
   mCurMarkFilter = iMark;
}
/////////////////////////////////////////////////////////////////////////////////////
//-------------------------Iteration Navigation------------------------------------//
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::MoveToFirst()
{
   mpCurNode = mpRootNode;
   mCurNodeOffset = 0;

   mCurNodeI = 0;
   mpCurNodeList = &mCurNodes;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
SpatialNode&   SpatialTreeIter::rCurrent()
{
   return *(*mpCurNodeList)[mCurNodeI];
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
void SpatialTreeIter::MoveToNext(bool ibIncludeVis)
{
    mCurNodeI++;
    if(ibIncludeVis 
        && (mCurNodeI >= (*mpCurNodeList).mUseSize)
        && (mpCurNodeList != &mCurAlwaysVisNodes))
    {
        mpCurNodeList= &mCurAlwaysVisNodes;
        mCurNodeI=0;
    }
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
SpatialNode&   SpatialTreeIter::rMoveToNext(bool ibIncludeVis)
{
   MoveToNext(ibIncludeVis);
   return *(*mpCurNodeList)[mCurNodeI];
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
SpatialNode*   SpatialTreeIter::pMoveToNext(bool ibIncludeVis)
{
   MoveToNext(ibIncludeVis);
   return (*mpCurNodeList)[mCurNodeI];
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
bool SpatialTreeIter::NotDone()
{
    return (mCurNodeI < (*mpCurNodeList).mUseSize);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
inline  
bool SpatialTreeIter::IsCurrentLeaf()
{
    return ((*mpCurNodeList)[mCurNodeI]->mSubDivPlane.mAxis == -1);
}


#endif
