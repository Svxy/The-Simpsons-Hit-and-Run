#ifndef __SPATIAL_TREE_FACTORY_H__
#define __SPATIAL_TREE_FACTORY_H__

#include <render/culling/UseArray.h>
#include <render/culling/FixedArray.h>
#include <render/culling/srrRenderTypes.h>
#include <render/culling/Bounds.h>
#include <render/culling/SpatialTree.h>
#include <render/culling/SpatialTreeIter.h>
#include <render/culling/HexahedronP.h>
//#include <render/culling/SpatialFruit.h>
#include <render/culling/CellBlock.h>

//////////////////////////////////////////////////////////////////////
//
// The Purpose of the class is to provide a black box abstraction
// of the various processes involved in the creation of a runtime
// spatial tree from a set of inputs ( posn & weighting )
//
// Support for Fruit (automatic queuing of fruit/payload and adding 
// to generated tree) will be added in sub classes
//
//////////////////////////////////////////////////////////////////////
class SpatialTreeFactory
{
public:
   SpatialTreeFactory();
   ~SpatialTreeFactory();

   void Clear();

   void Reset( int inSeedMax );

   void Seed( Vector3f& irPosn, int iWeight );

   void Generate( Vector3f& irGranularity );

   void ExtractTree( SpatialTree** oppRuntimeTree );   

protected:
   
   bool TreeGenerated();

   ////////////////////////////////////////////////////////////
   // Data neccessary for the entire tree creation lifecycle
   ////////////////////////////////////////////////////////////
   SpatialTree*                mpRuntimeTree;
   SpatialTreeIter             mTreeWalker;

   UseArray<Vector3f>          mSeedPosns;
   UseArray<int>               mSeedWeights;
};
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SpatialTreeFactory::SpatialTreeFactory()
{
   mpRuntimeTree = NULL;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SpatialTreeFactory::~SpatialTreeFactory()
{
}
//////////////////////////////////////////////////////////////////////
// Removes all state-based allocations (ie the allocations neccessary
// for the creation of a Tree)
//////////////////////////////////////////////////////////////////////
void SpatialTreeFactory::Clear()
{
   if( mpRuntimeTree != NULL )
   {
      delete mpRuntimeTree;
      mpRuntimeTree = NULL;
   }

   mTreeWalker.Clear();
   mSeedPosns.Clear();
   mSeedWeights.Clear();
}
//////////////////////////////////////////////////////////////////////
// Initialise the factory for the creation of a new runtime tree of 
// the same type (T). Where each spatial node within the final tree
// contains a ReserveArray of T's.
//
// The SeedMax determines the maximum size of the queue of Seeds
// (see Seed)
//
//////////////////////////////////////////////////////////////////////
void SpatialTreeFactory::Reset( int inSeedMax )
{
   Clear();

   mSeedPosns.Allocate(    inSeedMax );
   mSeedWeights.Allocate(  inSeedMax );
}
//////////////////////////////////////////////////////////////////////
// Declare a position and a weight which will influence the placement
// of Axis Aligned Planes and the general Topology of the RuntimeTree
//////////////////////////////////////////////////////////////////////
void SpatialTreeFactory::Seed( Vector3f& irPosn, int iWeight )
{
   rAssert( !TreeGenerated() );

   mSeedPosns.Add(   irPosn  );
   mSeedWeights.Add( iWeight );
}
//////////////////////////////////////////////////////////////////////
// This call:
//    -finalises the Seed'ing,
//    -generates the Runtime Tree Topology
//
// Calls to AttachFruit can still be made up until the Tree is 
// extracted.
//////////////////////////////////////////////////////////////////////
void SpatialTreeFactory::Generate( Vector3f& irGranularity )
{
   rAssert( !TreeGenerated() );

   Bounds3f    WorldBounds;
   Vector3i    ArrayDims;
   CellBlock   CellMatrix;
   OctTreeNode RootNode;

   FixedArray<Cell>     DataCellsArray;

   //
   // Although it'd be 'nice' to have UseArray Inherit 
   // from FixedArray, it'd be vtable badness in inner loop atomics
   // and extra overhead. 
   //
   // This alternative is still safe, cheap, and only a little sticky
   //
   FixedArray<Vector3f> seedPosnsFacade;
   FixedArray<int>      seedWeightsFacade;

   seedPosnsFacade.mpData = mSeedPosns.mpData;
   seedPosnsFacade.mSize  = mSeedPosns.mUseSize;

   seedWeightsFacade.mpData = mSeedWeights.mpData;
   seedWeightsFacade.mSize  = mSeedWeights.mUseSize;

   ///////////////////////////////////////////////////////////////////////
   CellMatrix.Init( seedPosnsFacade, seedWeightsFacade, irGranularity );
   
   CellMatrix.GenerateCells();

   ////////////////////////////////////////////////////////////////////
   CellMatrix.ExtractNonEmptyCells(   DataCellsArray );
   CellMatrix.ExtractDims(    ArrayDims );
   CellMatrix.ExtractBounds(  WorldBounds );

   ////////////////////////////////////////////////////////////////////
   CoordSubList RootList( DataCellsArray.mpData, DataCellsArray.mSize, ArrayDims );

   BoxPts DebugBBox;
   DebugBBox.SetTo( WorldBounds );

   //   RootNode.GrowTreeHeuristic( -1, &RootList, irCellGranularity, 12, 1 );
   //Debug
//   RootNode.GrowTreeHeuristicDebug( -1, &RootList, irGranularity, 12, 1, DebugBBox );
   RootNode.GrowTreeHeuristicDebug( -1, &RootList, irGranularity, 32, 1, DebugBBox );

   mpRuntimeTree = new SpatialTree;
   mpRuntimeTree->Generate( &RootNode, WorldBounds );

   mTreeWalker.SetToRoot( *mpRuntimeTree );


   ////////////////////////////////////////////////////////////////////
   mSeedPosns.mpData = seedPosnsFacade.mpData;
   mSeedPosns.mUseSize = seedPosnsFacade.mSize;

   mSeedWeights.mpData = seedWeightsFacade.mpData;
   mSeedWeights.mUseSize = seedWeightsFacade.mSize;

   seedPosnsFacade.mpData = NULL;
   seedWeightsFacade.mpData = NULL;
}
//////////////////////////////////////////////////////////////////////
// Once the Tree is extracted, SpatialTreeFactory divorces
// all responsability for said data; and removes internal 
// reference for it.
//
// When the Factory is deleted, it deletes any data it has 
// maintained reference to.
//////////////////////////////////////////////////////////////////////
void SpatialTreeFactory::ExtractTree( SpatialTree** oppRuntimeTree )
{
   rAssert( TreeGenerated() );

   *oppRuntimeTree = mpRuntimeTree;
   mpRuntimeTree = NULL;

   Clear();
}
//////////////////////////////////////////////////////////////////////
// mpRuntimeTree should only be non-null between the calls to 
// Generate and ExtractTree (or, more specifically, Clear)
//////////////////////////////////////////////////////////////////////
bool SpatialTreeFactory::TreeGenerated()
{
   if( mpRuntimeTree == NULL )
      return false;
   return true;
}


#endif