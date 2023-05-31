#include <render/culling/OctTreeNode.h>
#include <render/culling/srrRenderTypes.h>
#include <raddebug.hpp>
//
// int  mAxis;
// float mPlanePosn;
//
// CoordSubList* mpCoordList
//
// OctTreeNode* mpChildLT;
// OctTreeNode* mpChildGT;
//

OctTreeNode::OctTreeNode()
{
   mpChildLT   = NULL;
   mpChildGT   = NULL;
   mpCoordList = NULL;
}

OctTreeNode::~OctTreeNode()
{
   CascadeDelete();
}

//
// Grow Functions recursively build the Tree,
// Calling appropriate SubDivide functions on their way down,
// Managing stopping conditions.
//
// Input Constraints:
// World must start at (0,0) (X,Z) and run contiguously to
// positive max's in X and Z
//
void OctTreeNode::GrowTreeHeuristic(   int            iAxis, 
                                       CoordSubList*  ipCoordList, 
                                       Vector3f&      irGranularity,
                                       int            iRecursionsRemaining,
                                       int            iMinVertexCount )
{
   rAssert( mpChildLT     == NULL );
   rAssert( mpChildGT     == NULL );
   rAssert( mpCoordList   == NULL );

   mpCoordList = ipCoordList;

   if( RoomToGrow( iRecursionsRemaining, iMinVertexCount ) )
   {
      mpChildLT = new(GMA_TEMP) OctTreeNode();
      mpChildGT = new(GMA_TEMP) OctTreeNode();

      CoordSubList *pListLT = NULL;
      CoordSubList *pListGT = NULL;

      mAxis = -1;

      mpCoordList->SubDivideHeuristic( irGranularity, &pListLT, &pListGT, mAxis, mPlanePosn );

      rAssert( pListLT->Size() > 0 );
      rAssert( pListGT->Size() > 0 );

      mpChildLT->GrowTreeHeuristic( -1, pListLT, irGranularity, iRecursionsRemaining--, iMinVertexCount );
      mpChildGT->GrowTreeHeuristic( -1, pListGT, irGranularity, iRecursionsRemaining--, iMinVertexCount );                                    
   }
   else
   {
      mpChildLT   = NULL;
      mpChildGT   = NULL;
      mPlanePosn  = SRR_ERR_WS_COORD;
      mAxis       = SRR_ERR_INDEX;
   }
}
                        
bool OctTreeNode::AllElemsLT( CoordSubList *pListLT, AAPlane3f& irDivPlane )
{
   BoxPts CurBBox;

   for( int i=pListLT->Size()-1; i>=0; i--)
   {
      if( pListLT->Get(i).MaxPlane( irDivPlane.mAxis ) > irDivPlane.mPosn )
         return false;
   }
   return true;
}

bool OctTreeNode::AllElemsGT( CoordSubList *pListGT, AAPlane3f& irDivPlane )
{
   BoxPts CurBBox;

   for( int i=pListGT->Size()-1; i>=0; i--)
   {
      if( pListGT->Get(i).MinPlane( irDivPlane.mAxis ) < irDivPlane.mPosn )
         return false;
   }
   return true;
}

void OctTreeNode::GrowTreeHeuristicDebug
(   
   int            iAxis, 
   CoordSubList*  ipCoordList, 
   Vector3f&      irGranularity,
   int            iRecursionsRemaining,
   int            iMinVertexCount,
   BoxPts&        irBoxPts
)
{
   rAssert( mpChildLT     == NULL );
   rAssert( mpChildGT     == NULL );
   rAssert( mpCoordList   == NULL );

   mpCoordList = ipCoordList;

   if( RoomToGrow( iRecursionsRemaining, iMinVertexCount ) )
   {
      mpChildLT = new(GMA_TEMP) OctTreeNode();
      mpChildGT = new(GMA_TEMP) OctTreeNode();

      CoordSubList *pListLT = NULL;
      CoordSubList *pListGT = NULL;

      mAxis = -1;

      mpCoordList->SubDivideHeuristic( irGranularity, &pListLT, &pListGT, mAxis, mPlanePosn );

      //Debug begin
      rAssert( pListLT->Size() > 0 );
      rAssert( pListGT->Size() > 0 );

      AAPlane3f DivPlane;
      
      DivPlane.mAxis = mAxis;
      DivPlane.mPosn = mPlanePosn;

      BoxPts BoxPtsLT( irBoxPts );
      BoxPtsLT.CutOffGT( DivPlane );
      
      irBoxPts.CutOffLT( DivPlane );

      if( !AllElemsLT( pListLT, DivPlane ) )
      {
         bool fucked = true;
      } 
      if( !AllElemsGT( pListGT, DivPlane ) )
      {
         bool fucked = true;
      } 

      mpChildLT->GrowTreeHeuristicDebug( -1, pListLT, irGranularity, iRecursionsRemaining--, iMinVertexCount, BoxPtsLT );
      mpChildGT->GrowTreeHeuristicDebug( -1, pListGT, irGranularity, iRecursionsRemaining--, iMinVertexCount, irBoxPts );                                    

      rAssert( pListLT->Size() > 0 );
      rAssert( pListGT->Size() > 0 );
      //Debug end
   }
   else
   {
      mpChildLT   = NULL;
      mpChildGT   = NULL;
      mPlanePosn  = SRR_ERR_WS_COORD;
      mAxis       = SRR_ERR_INDEX;
   }
}

void OctTreeNode::GrowTreeUniform(  int           iAxis, 
                                    CoordSubList*  ipCoordList, 
                                    int            iRecursionsRemaining,
                                    int            iMinVertexCount )
{
}


//////////////////////////////////////////////////////////////////////////////
// Get All elements in the subdivision
// that occupies irPosn
//////////////////////////////////////////////////////////////////////////////
void OctTreeNode::AccessSubDiv( Vector3f& irPosn, CoordSubList& orSubList )
{

}

//
// Generate X,Z tuples which enumerate the Cells in the World
// Ranges:
// 0 <-> max(X)/iSpaceGranularity 
// 0 <-> max(Z)/iSpaceGranularity 
//
//void OctTreeNode::UpdateVisibleCellList(  rmt::Vector*   opVisibleCellList,
//                                          float          iSpaceGranularity )
//{ 
//}

////////////////////////////////// protected: ////////////////////////////////////////////

//
// Divides the space by a plane which cuts iAxis, such that
// as close as possible to half the geometry is on each side
// of the plane.
//
//void OctTreeNode::SubDivideHeuristic( int iAxis, rmt::Vector* ipVertexList, float iSpaceGranularity )
//{
//}

//
// Divides the space by a plane which cuts iAxis, such that
// as close as possible to half the euclidean space is on 
// each side of the plane.
//
//void OctTreeNode::SubDivideUniform( int iAxis, rmt::Vector* ipVertexList, float iSpaceGranularity )
//{
//}


bool OctTreeNode::RoomToGrow( int iRecursionsRemaining, int iMinVertexCount )
{
   if( iRecursionsRemaining > 0 )
   {
      if( mpCoordList->WeightedSize() > iMinVertexCount )
      {
         return mpCoordList->CanBeSplit();
         //return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      return false;
   }
}

void OctTreeNode::CascadeDelete()
{
   //
   // They should either all be NULL or all be valid.
   //
   if( mpChildLT != NULL && mpChildGT != NULL )
   {
      delete mpChildLT;
      delete mpChildGT;
   }

}

