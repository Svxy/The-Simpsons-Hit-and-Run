#ifndef __OCT_TREE_NODE_H__
#define __OCT_TREE_NODE_H__

#include <render/culling/OctTreeData.h>
#include <render/culling/BoxPts.h>

class OctTreeNode
{
public:
   OctTreeNode();
   ~OctTreeNode();

   //
   // Grow Functions recursively build the Tree,
   // Calling appropriate SubDivide functions on their way down,
   // Managing stopping conditions.
   //
   // Input Constraints:
   // World must start at (0,0) (X,Z) and run contiguously to
   // positive max's in X and Z
   //
   void GrowTreeHeuristic( int           iAxis, 
                           CoordSubList*  ipCoordList, 
                           Vector3f&      irGranularity,
                           int            iRecursionsRemaining,
                           int            iMinVertexCount );
                           
   void GrowTreeUniform(   int           iAxis, 
                           CoordSubList*  ipCoordList, 
                           int            iRecursionsRemaining,
                           int            iMinVertexCount );

   //Debug
   bool AllElemsLT( CoordSubList *pListLT, AAPlane3f& irDivPlane );
   bool AllElemsGT( CoordSubList *pListGT, AAPlane3f& irDivPlane );
   void GrowTreeHeuristicDebug(   
                           int            iAxis, 
                           CoordSubList*  ipCoordList, 
                           Vector3f&      irGranularity,
                           int            iRecursionsRemaining,
                           int            iMinVertexCount,
                           BoxPts&        irBoxPts);

   //
   // Get All elements in the subdivision
   // that occupies irPosn
   //
   void AccessSubDiv( Vector3f& irPosn, CoordSubList& orSubList );

   //
   // Generate X,Z tuples which enumerate the Cells in the World
   // Ranges:
   // 0 <-> max(X)/iSpaceGranularity 
   // 0 <-> max(Z)/iSpaceGranularity 
   //
   //void UpdateVisibleCellList(   rmt::Vector*   opVisibleCellList,
   //                              float          iSpaceGranularity ); 

   int  mAxis;
   float mPlanePosn;

   OctTreeNode* mpChildLT;
   OctTreeNode* mpChildGT;
protected:
   //
   // Divides the space by a plane which cuts iAxis, such that
   // as close as possible to half the geometry is on each side
   // of the plane.
   //
   //void SubDivideHeuristic( int iAxis, rmt::Vector* ipVertexList, float iSpaceGranularity );
   
   //
   // Divides the space by a plane which cuts iAxis, such that
   // as close as possible to half the euclidean space is on 
   // each side of the plane.
   //
   //void SubDivideUniform( int iAxis, rmt::Vector* ipVertexList, float iSpaceGranularity );

   
   bool RoomToGrow( int iRecursionsRemaining, int iMinVertexCount );
   void CascadeDelete();



   // TODO - Put some sort of UID list here for final tree output
   //        This will link up with the actual geometry in-game
   //        This will be used by UpdateVisibleCellList
   CoordSubList* mpCoordList;

};

#endif