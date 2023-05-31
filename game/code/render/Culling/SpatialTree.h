#ifndef __SPATIAL_TREE_H__
#define __SPATIAL_TREE_H__

#include <p3d/entity.hpp>

#include <render/culling/OctTreeNode.h>
#include <render/culling/FixedArray.h>
#include <render/culling/SpatialNode.h>
#include <render/culling/BoxPts.h>

class SpatialTree 
: public tEntity
{
public:
   SpatialTree();
   ~SpatialTree();

   void Generate( OctTreeNode* ipTreeRoot, Bounds3f& irTreeBounds );
   
   ContiguousBinNode< SpatialNode >* GetRoot();

   void SetTo( int iNumNodes, Bounds3f iTreeBounds );

   Bounds3f& GetBounds();

protected:
   FixedArray< ContiguousBinNode< SpatialNode > > mTreeNodes;
   Bounds3f mTreeBounds;

   void CountNodes(  int& orCount,                                         OctTreeNode* ipTreeRoot );
   void SetNodes(    int& orSubTreeSize, int iNodeIndex, int iParentIndex, OctTreeNode* ipTreeRoot, BoxPts& irBoxPts );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline 
SpatialTree::SpatialTree()
{
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline 
SpatialTree::~SpatialTree()
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline 
void SpatialTree::Generate( OctTreeNode* ipTreeRoot, Bounds3f& irTreeBounds )
{
   int nNodes=0, SubTreeSize=0, NodeIndex=0;

   mTreeBounds = irTreeBounds;

   CountNodes( nNodes, ipTreeRoot );

   mTreeNodes.Allocate(nNodes);

   BoxPts rootBBox;
   rootBBox.SetTo( mTreeBounds );

   SetNodes( SubTreeSize, NodeIndex, ContiguousBinNode< SpatialNode >::msNoParent, ipTreeRoot, rootBBox );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline 
ContiguousBinNode< SpatialNode >* SpatialTree::GetRoot()
{
   return mTreeNodes.mpData;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline 
Bounds3f& SpatialTree::GetBounds()
{
   return mTreeBounds;
}

//========================================================================
// SpatialTree::
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
void SpatialTree::SetTo( int iNumNodes, Bounds3f iTreeBounds )
{
   mTreeNodes.Allocate(iNumNodes);
   mTreeBounds = iTreeBounds;
}

#endif