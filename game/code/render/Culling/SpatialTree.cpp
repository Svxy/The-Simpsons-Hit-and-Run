//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SpatialTree.cpp
//
// Description: Implement SpatialTree
//
// History:     16/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <render/culling/SpatialTree.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//////////////////////////////////////////////////////////////////////////
//
// TODO: remove rAssert associated code
//
////////////////////////////////////////////////////////////////////////// 
void SpatialTree::SetNodes( int& orSubTreeSize, int iNodeIndex, int iParentIndex, OctTreeNode* ipTreeRoot, BoxPts& irBoxPts )
{
   int rightSubTreeSize = 0;

   mTreeNodes[iNodeIndex].mData.mSubDivPlane.Set( ipTreeRoot->mAxis, ipTreeRoot->mPlanePosn );
   mTreeNodes[iNodeIndex].mData.mBBox = irBoxPts;
   mTreeNodes[iNodeIndex].LinkParent(iParentIndex - iNodeIndex);

   if( ipTreeRoot->mpChildLT != NULL )
   {
      rAssert( ipTreeRoot->mpChildGT != NULL );

      BoxPts BBoxLT( irBoxPts );
      BBoxLT.CutOffGT(     mTreeNodes[iNodeIndex].mData.mSubDivPlane );
      irBoxPts.CutOffLT(   mTreeNodes[iNodeIndex].mData.mSubDivPlane );

      SetNodes( orSubTreeSize,      iNodeIndex+1,                iNodeIndex, ipTreeRoot->mpChildLT, BBoxLT   );
      SetNodes( rightSubTreeSize,   iNodeIndex+orSubTreeSize+1,  iNodeIndex, ipTreeRoot->mpChildGT, irBoxPts );

      orSubTreeSize = orSubTreeSize + rightSubTreeSize;
      mTreeNodes[iNodeIndex].SetSubTreeSize( orSubTreeSize );
   }
   else
   {
      rAssert( ipTreeRoot->mpChildGT == NULL );

      mTreeNodes[iNodeIndex].SetSubTreeSize( ContiguousBinNode< SpatialNode >::msNoChildren );
   }
   
   orSubTreeSize++;
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
void SpatialTree::CountNodes( int& orCount, OctTreeNode* ipTreeRoot )
{
   orCount++;

   if( ipTreeRoot->mpChildLT != NULL )
   {
      rAssert( ipTreeRoot->mpChildGT != NULL );
      CountNodes( orCount, ipTreeRoot->mpChildLT );
      CountNodes( orCount, ipTreeRoot->mpChildGT );
   }
   else
   {
      rAssert( ipTreeRoot->mpChildGT == NULL );
   }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
