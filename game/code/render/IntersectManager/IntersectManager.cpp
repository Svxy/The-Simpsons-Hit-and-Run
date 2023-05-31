//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        IntersectManager.cpp
//
// Description: Implementation for IntersectManager class.
//
// History:     Implemented	                         --Devin [5/5/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>
//#include <radtime.hpp>
//#include <raddebugwatch.hpp>
#include <radmath/radmath.hpp>
#include <simcollision/proximitydetection.hpp>

//========================================
// Project Includes
//========================================
#include <debug/profiler.h>

#include <render/IntersectManager/IntersectManager.h>
#include <render/RenderManager/RenderManager.h>
#include <render/Culling/SphereSP.h>
#include <render/Culling/WorldScene.h>
#include <render/DSG/IntersectDSG.h>
#include <render/DSG/StaticPhysDSG.h>
#include <render/DSG/DynaPhysDSG.h>
#include <render/DSG/FenceEntityDSG.h>
#include <render/DSG/animcollisionentitydsg.h>
#include <meta/triggervolume.h>
#include <roads/roadsegment.h>
#include <roads/geometry.h>
#include <roads/road.h>
#include <pedpaths/pathsegment.h>
#include <render/dsg/staticentitydsg.h>

#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <camera/supercammanager.h>


//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************
//
// Static pointer to instance of this singleton.
//
IntersectManager* IntersectManager::mspInstance = NULL;

//******************************************************************************
// Public Member Functions : Instance Interface
//******************************************************************************
//==============================================================================
// IntersectManager::CreateInstance
//==============================================================================
//
// Description: Create the IntersectManager controller if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the created IntersectManager controller.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
IntersectManager* IntersectManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "IntersectManager" );
   rAssert( mspInstance == NULL );
   mspInstance = new(GMA_PERSISTENT) IntersectManager();
MEMTRACK_POP_GROUP( "IntersectManager" );

   return mspInstance;
}

//==============================================================================
// IntersectManager::GetInstance
//==============================================================================
//
// Description: Get the IntersectManager controller if exists.
//
// Parameters:	None.
//
// Return:      Pointer to the created IntersectManager controller.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
IntersectManager* IntersectManager::GetInstance()
{
   rAssert( mspInstance != NULL );
   
   return mspInstance;
}


//==============================================================================
// IntersectManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the IntersectManager controller.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void IntersectManager::DestroyInstance()
{
    //
    // Make sure this doesn't get called twice.
    //
    rAssert( mspInstance != NULL );
    delete mspInstance;
    mspInstance = NULL;
}

//************************************************************************
// Public Member Functions : IntersectManager "Meat" Interface
//************************************************************************

bool IntersectManager::IntersectWithPlane( rmt::Vector planeOrigin, 
                                           rmt::Vector planeNormal,
                                           rmt::Vector rayOrigin,
                                           rmt::Vector rayVector,
                                           float& time )
{
    float numer, denom;

    denom = rayVector.Dot( planeNormal );

    if( rmt::Fabs( denom ) <= 0.0001f)
    {
        return( false );
    }

    numer = planeNormal.Dot( rayOrigin ) - planeNormal.Dot( planeOrigin );

    time = -numer / denom;

    return( true );
}

//========================================================================
// IntersectManager::
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
void IntersectManager::ResetCache( const rmt::Vector& irPosn, float iRadius )
{
    mCachedPosn = irPosn;
    mCachedRadius = iRadius;
    mbSameFrame = true;
}
#define SPHERE_TEST
//#define VIEW_FRUSTUM_TEST

//========================================================================
// intersectmanager::
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
void IntersectManager::FindClosestRoad
(     
    const rmt::Vector&   irPosn, 
    float          iRadius,
    RoadSegment*&  orpRoad,
    float&         oDistSqr 
)
{
BEGIN_PROFILE("::FindClosestRoad")  
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
   
   rmt::Sphere segmentSphere;

   orpRoad = NULL;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);

   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }

   unsigned int itCount = 0;

   oDistSqr = 100000.0f;

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mRoadSegmentElems.mUseSize-1; i>-1; i-- )
      {
         RoadSegment* segment = pTreeIter->rCurrent().mRoadSegmentElems[i];

         if( segment->GetRoad()->GetShortCut() )
             continue;

         itCount++;

         segment->GetBoundingSphere( &segmentSphere );
         
         rmt::Vector vec0, vec1, vec2, vec3;
         rmt::Vector start, end;
         segment->GetCorner( 0, vec0 );
         segment->GetCorner( 1, vec1 );
         segment->GetCorner( 2, vec2 );
         segment->GetCorner( 3, vec3 );

         start = ( vec0 + vec3 ) * 0.5f;
         end = ( vec1 + vec2 ) * 0.5f;

         rmt::Vector closestPtOnSeg;
         FindClosestPointOnLine( start, end, irPosn, closestPtOnSeg );

         float distSqr = ( closestPtOnSeg - irPosn ).MagnitudeSqr();
         if( distSqr < oDistSqr  )
         {
             orpRoad = segment;
             oDistSqr = distSqr;
         }
      }
   }
END_PROFILE("::FindClosestRoad")  
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void IntersectManager::FindClosestAnyRoad
(     
    const rmt::Vector&   irPosn, 
    float          iRadius,
    RoadSegment*&  orpRoad,
    float&         oDistSqr 
)
{
BEGIN_PROFILE("::FindClosestAnyRoad")  
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
   
   rmt::Sphere segmentSphere;

   orpRoad = NULL;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);

   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }

   unsigned int itCount = 0;

   oDistSqr = 100000.0f;

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mRoadSegmentElems.mUseSize-1; i>-1; i-- )
      {
         RoadSegment* segment = pTreeIter->rCurrent().mRoadSegmentElems[i];

         itCount++;

         segment->GetBoundingSphere( &segmentSphere );
         
         rmt::Vector vec0, vec1, vec2, vec3;
         rmt::Vector start, end;
         segment->GetCorner( 0, vec0 );
         segment->GetCorner( 1, vec1 );
         segment->GetCorner( 2, vec2 );
         segment->GetCorner( 3, vec3 );

         start = ( vec0 + vec3 ) * 0.5f;
         end = ( vec1 + vec2 ) * 0.5f;

         rmt::Vector closestPtOnSeg;
         FindClosestPointOnLine( start, end, irPosn, closestPtOnSeg );

         float distSqr = ( closestPtOnSeg - irPosn ).MagnitudeSqr();
         if( distSqr < oDistSqr )
         {
             orpRoad = segment;
             oDistSqr = distSqr;
         }
      }
   }
END_PROFILE("::FindClosestAnyRoad")  
}
//========================================================================
// IntersectManager::
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
void IntersectManager::FindFenceElems    
(
   rmt::Vector&   irPosn, 
   float          iRadius,
   ReserveArray<FenceEntityDSG*>& orList 
)
{
BEGIN_PROFILE("::FindFenceElems")  
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);

//   rmt::Sphere tempSphere;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msStaticPhys );
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msDynaPhys );
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
//   pTreeIter->SetIterFilter( WorldScene::msDynaPhys );
//   pTreeIter->SetIterFilter( WorldScene::msStaticPhys );

   orList.Allocate(200);

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mFenceElems.mUseSize-1; i>-1; i-- )
      {
#ifdef SPHERE_TEST
          //pTreeIter->rCurrent().mFenceElems[i]->GetBoundingSphere(&tempSphere);
         rmt::Vector pt1 = pTreeIter->rCurrent().mFenceElems[i]->mStartPoint;
         rmt::Vector pt2 = pTreeIter->rCurrent().mFenceElems[i]->mEndPoint;
         
         rmt::Vector center = pt1;
         center.Add( pt2 );
         center.Scale(0.5f);
         
         pt1.y = 0.0f;
         pt2.y = 0.0f;
         center.y = 0.0f;

         pt1.Sub(center,pt2);

         float circleRadiusSqr = pt1.MagnitudeSqr();
         
         float maxDistSqr = (iRadius*iRadius)+circleRadiusSqr;//* (iRadius+circleRadius);
         
         //tempSphere.centre.Sub(tempSphere.centre,irPosn);
         center.y = irPosn.y;
         center.Sub(irPosn);

         if( center.MagnitudeSqr() < maxDistSqr )
#endif
         {
            orList.Add( pTreeIter->rCurrent().mFenceElems[i] );
         }
      }
   }
END_PROFILE("::FindFenceElems")  
}
//========================================================================
// IntersectManager::
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
void IntersectManager::FindStaticPhysElems
(  
   rmt::Vector&   irPosn, 
   float          iRadius,
   ReserveArray<StaticPhysDSG*>& orList 
)
{
BEGIN_PROFILE("::FindStaticPhysElems")  
   //SpatialNode<StaticEntityDSG,StaticPhysDSG,IntersectDSG>& rCurrentLeaf = 
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
   
   int i,j;
   rmt::Sphere tempSphere;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
   //BEGIN_PROFILE( "AndTree" );
   //pTreeIter->AndTree( ~WorldScene::msStaticPhys );
   //END_PROFILE( "AndTree" );
//   BEGIN_PROFILE( "MarkAll" );
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msDynaPhys );
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
//   pTreeIter->SetIterFilter( WorldScene::msDynaPhys );
//   END_PROFILE( "MarkAll" );
   //pTreeIter->SetIterFilter( WorldScene::msStaticPhys );

   orList.Allocate(200);

   //BEGIN_PROFILE( "Iter" );
   //for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   //{
   //   for( i=pTreeIter->rCurrent().mSPhysElems.mUseSize-1; i>-1; i-- )
   //   {
   //      pTreeIter->rCurrent().mSPhysElems[i]->GetBoundingSphere(&tempSphere);
   //      
   //      maxDistSqr = (iRadius+tempSphere.radius) * (iRadius+tempSphere.radius);
   //      
   //      tempSphere.centre.Sub(tempSphere.centre,irPosn);
   //
   //      if( tempSphere.centre.MagnitudeSqr() < maxDistSqr )
   //      {
   //         orList.Add( pTreeIter->rCurrent().mSPhysElems[i] );
   //      }
   //   }
   //}
   //END_PROFILE( "Iter" );
   j= pTreeIter->mCurNodes.mUseSize-1;
    if(j>100)
    {
       rReleasePrintf("\n\nWTF?  %d nodes \n\n", j);
    }

//   BEGIN_PROFILE( "Quack" );
   for(j=pTreeIter->mCurNodes.mUseSize-1; j>-1; j--)
   {
      for( i=pTreeIter->mCurNodes[j]->mSPhysElems.mUseSize-1; i>-1; i-- )
      {
#ifdef SPHERE_TEST
         pTreeIter->mCurNodes[j]->mSPhysElems[i]->GetBoundingSphere(&tempSphere);
         
         float maxDistSqr = (iRadius+tempSphere.radius) * (iRadius+tempSphere.radius);
         
         tempSphere.centre.Sub(tempSphere.centre,irPosn);

         if( tempSphere.centre.MagnitudeSqr() < maxDistSqr )
#endif
         {
            orList.Add( pTreeIter->mCurNodes[j]->mSPhysElems[i] );
         }
      }
   }
//   END_PROFILE( "Quack" );
END_PROFILE("::FindStaticPhysElems")  
}


//========================================================================
// IntersectManager::
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
void IntersectManager::FindStaticElems
(  
   rmt::Vector&   irPosn, 
   float          iRadius,
   ReserveArray<StaticEntityDSG*>& orList 
)
{
BEGIN_PROFILE("::FindStaticElems")  
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
  
   int i,j;
   rmt::Sphere tempSphere;
   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
   orList.Allocate(200);

   //BEGIN_PROFILE( "Iter" );
   //for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   //{
   //   for( i=pTreeIter->rCurrent().mSPhysElems.mUseSize-1; i>-1; i-- )
   //   {
   //      pTreeIter->rCurrent().mSPhysElems[i]->GetBoundingSphere(&tempSphere);
   //      
   //      maxDistSqr = (iRadius+tempSphere.radius) * (iRadius+tempSphere.radius);
   //      
   //      tempSphere.centre.Sub(tempSphere.centre,irPosn);
   //
   //      if( tempSphere.centre.MagnitudeSqr() < maxDistSqr )
   //      {
   //         orList.Add( pTreeIter->rCurrent().mSPhysElems[i] );
   //      }
   //   }
   //}
   //END_PROFILE( "Iter" );
   j= pTreeIter->mCurNodes.mUseSize-1;
    if(j>100)
    {
       rReleasePrintf("\n\nWTF?  %d nodes \n\n", j);
    }

//   BEGIN_PROFILE( "Quack" );
   for(j=pTreeIter->mCurNodes.mUseSize-1; j>-1; j--)
   {
       for( i=pTreeIter->mCurNodes[j]->mSEntityElems.mUseSize-1; i>-1; i-- )
      {
#ifdef SPHERE_TEST
         pTreeIter->mCurNodes[j]->mSEntityElems[i]->GetBoundingSphere(&tempSphere);
         
         float maxDistSqr = (iRadius+tempSphere.radius) * (iRadius+tempSphere.radius);
         
         tempSphere.centre.Sub(tempSphere.centre,irPosn);

         if( tempSphere.centre.MagnitudeSqr() < maxDistSqr )
#endif
         {
            orList.Add( pTreeIter->mCurNodes[j]->mSEntityElems[i] );
         }
      }
   }
//   END_PROFILE( "Quack" );
END_PROFILE("::FindStaticElems")  
}

//========================================================================
// IntersectManager::
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
void IntersectManager::FindDynaPhysElems
(   
    rmt::Vector&   irPosn, 
    float          iRadius,
    ReserveArray<DynaPhysDSG*>& orList 
)
{
BEGIN_PROFILE("::FindDynaPhysElems")  
   //SpatialNode<StaticEntityDSG,StaticPhysDSG,IntersectDSG>& rCurrentLeaf = 
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
   
   rmt::Sphere tempSphere;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msDynaPhys );
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
//   pTreeIter->SetIterFilter( WorldScene::msDynaPhys );

   orList.Allocate(200);

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mDPhysElems.mUseSize-1; i>-1; i-- )
      {
#ifdef SPHERE_TEST
         pTreeIter->rCurrent().mDPhysElems[i]->GetBoundingSphere(&tempSphere);
         
         float maxDistSqr = (iRadius+tempSphere.radius) * (iRadius+tempSphere.radius);
         
         tempSphere.centre.Sub(tempSphere.centre,irPosn);

         if( tempSphere.centre.MagnitudeSqr() < maxDistSqr )
#endif
         {
            orList.Add( pTreeIter->rCurrent().mDPhysElems[i] );
         }
      }
   }
END_PROFILE("::FindDynaPhysElems")  
}

//========================================================================
// IntersectManager::
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
void IntersectManager::FindAnimPhysElems
(   
    rmt::Vector&   irPosn, 
    float          iRadius,
    ReserveArray<AnimCollisionEntityDSG*>& orList 
)
{
#ifndef RAD_RELEASE
    char temp[100];
    sprintf(temp, "::FindAnimPhysElems %f", iRadius);
#endif
BEGIN_PROFILE("::FindAnimPhysElems")  
   //SpatialNode<StaticEntityDSG,StaticPhysDSG,IntersectDSG>& rCurrentLeaf = 
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
  
   rmt::Sphere tempSphere;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msDynaPhys );
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
//   pTreeIter->SetIterFilter( WorldScene::msDynaPhys );

   orList.Allocate(200);

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mAnimCollElems.mUseSize-1; i>-1; i-- )
      {
          bool addToList = false;

#ifdef VIEW_FRUSTUM_TEST
       // Test the object and see if it is within any of the view frustums
          pTreeIter->rCurrent().mAnimCollElems[i]->GetBoundingSphere(&tempSphere);      

          int numPlayers = GetGameplayManager()->GetNumPlayers();
          for ( int player = 0 ; player < numPlayers ; player++)
          {
		    if (GetSuperCamManager()->GetSCC( player )->GetCamera()->SphereVisible( tempSphere.centre, tempSphere.radius ) )
            {
                addToList = true;
                break;
            }
          }
#else  
#ifdef SPHERE_TEST
          // Use the sphere test if the view frustum test is not enabled
         pTreeIter->rCurrent().mAnimCollElems[i]->GetBoundingSphere(&tempSphere);
         
         float maxDistSqr = (iRadius+tempSphere.radius) * (iRadius+tempSphere.radius);
         
         tempSphere.centre.Sub(tempSphere.centre,irPosn);

         if( tempSphere.centre.MagnitudeSqr() < maxDistSqr )
         {
            addToList = true;
         }
#else 
          // Neither test is activated, always add it to the list
        addToList = true;            
#endif

#endif
         if ( addToList )
         {
            orList.Add( pTreeIter->rCurrent().mAnimCollElems[i] );
         }
      }
   }
END_PROFILE("::FindAnimPhysElems")  
}
//========================================================================
// IntersectManager::
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
void IntersectManager::FindTrigVolElems
(   
    rmt::Vector&   irPosn, 
    float          iRadius,
    ReserveArray<TriggerVolume*>& orList 
)
{
#ifndef RAD_RELEASE
    char temp[100];
    sprintf(temp, "::FindTrigVolElems %f", iRadius);
#endif
BEGIN_PROFILE(temp)  
   //SpatialNode<StaticEntityDSG,StaticPhysDSG,IntersectDSG>& rCurrentLeaf = 
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
   

   rmt::Sphere tempSphere;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msDynaPhys );
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
//   pTreeIter->SetIterFilter( WorldScene::msDynaPhys );

   orList.Allocate(200);

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mTrigVolElems.mUseSize-1; i>-1; i-- )
      {
#ifdef SPHERE_TEST
         pTreeIter->rCurrent().mTrigVolElems[i]->GetBoundingSphere(&tempSphere);
         
         float maxDistSqr = (iRadius+tempSphere.radius) * (iRadius+tempSphere.radius);
         
         tempSphere.centre.Sub(tempSphere.centre,irPosn);

         if( tempSphere.centre.MagnitudeSqr() < maxDistSqr )
#endif
         {
            orList.Add( pTreeIter->rCurrent().mTrigVolElems[i] );
         }
      }
   }
END_PROFILE(temp)  
}


//========================================================================
// IntersectManager::
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
void IntersectManager::FindRoadSegmentElems
(   
    rmt::Vector&   irPosn, 
    float          iRadius,
    ReserveArray<RoadSegment*>& orList 
)
{
BEGIN_PROFILE("::FindRoadSegmentElems")  
   //SpatialNode<StaticEntityDSG,StaticPhysDSG,IntersectDSG>& rCurrentLeaf = 
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
   
   rmt::Sphere segmentSphere;
   //rmt::Vector segmentPos;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msDynaPhys );
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
//   pTreeIter->SetIterFilter( WorldScene::msDynaPhys );

   orList.Allocate(200);

   unsigned int itCount = 0;

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mRoadSegmentElems.mUseSize-1; i>-1; i-- )
      {
         RoadSegment* segment = pTreeIter->rCurrent().mRoadSegmentElems[i];
#ifdef SPHERE_TEST
         itCount++;

         segment->GetBoundingSphere( &segmentSphere );
         //segment->GetPosition( &segmentPos );
         
         float halfrad = segmentSphere.radius;// / 2.0f;
         float maxDistSqr = (iRadius+halfrad) * (iRadius+halfrad);
         float minDistSqr;
         if( halfrad > iRadius )
         {
             minDistSqr = 0.0f;
         }
         else
         {
             minDistSqr = (iRadius-halfrad) * (iRadius-halfrad); 
         }
         
         rmt::Vector temp;
         //temp.Sub(segmentPos,irPosn);
         temp.Sub( segmentSphere.centre, irPosn );

         if( temp.MagnitudeSqr() < maxDistSqr && 
             temp.MagnitudeSqr() > minDistSqr)
#endif
         {
            orList.Add( segment );
         }
      }
   }
END_PROFILE("::FindRoadSegmentElems")  
}

//========================================================================
// IntersectManager::
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
void IntersectManager::FindPathSegmentElems
(   
    rmt::Vector&   irPosn, 
    float          iRadius,
    ReserveArray<PathSegment*>& orList 
)
{
BEGIN_PROFILE("::FindPathSegmentElems")  
   //SpatialNode<StaticEntityDSG,StaticPhysDSG,IntersectDSG>& rCurrentLeaf = 
   SphereSP desiredVol;
   desiredVol.SetTo(irPosn,iRadius);
   

   rmt::Sphere segmentSphere;
   //rmt::Vector segmentPos;

   SpatialTreeIter* pTreeIter = &(GetRenderManager()->pWorldScene()->mStaticTreeWalker);
//   pTreeIter->OrTree( WorldScene::msStaticPhys );
//   pTreeIter->AndTree( ~WorldScene::msDynaPhys );
   if( !mbSameFrame || 
       (mCachedPosn != irPosn) || 
       (mCachedRadius != iRadius) )
   {
      ResetCache( irPosn, iRadius );
      pTreeIter->MarkAll( desiredVol, WorldScene::msStaticPhys );
   }
//   pTreeIter->SetIterFilter( WorldScene::msDynaPhys );

   orList.Allocate(200);

   unsigned int itCount = 0;

   for(pTreeIter->MoveToFirst(); pTreeIter->NotDone(); pTreeIter->MoveToNext())
   {
      for( int i=pTreeIter->rCurrent().mPathSegmentElems.mUseSize-1; i>-1; i-- )
      {
         PathSegment* segment = pTreeIter->rCurrent().mPathSegmentElems[i];
#ifdef SPHERE_TEST
         itCount++;

         segment->GetBoundingSphere( &segmentSphere );
         //segment->GetPosition( &segmentPos );
         
         float halfrad = segmentSphere.radius;// / 2.0f;
         float maxDistSqr = (iRadius+halfrad) * (iRadius+halfrad);
         float minDistSqr;
         if( halfrad > iRadius )
         {
             minDistSqr = 0.0f;
         }
         else
         {
             minDistSqr = (iRadius-halfrad) * (iRadius-halfrad); 
         }
         
         rmt::Vector temp;
         //temp.Sub(segmentPos,irPosn);
         temp.Sub( segmentSphere.centre, irPosn );

         if( temp.MagnitudeSqr() < maxDistSqr && 
             temp.MagnitudeSqr() > minDistSqr)
#endif
         {
            orList.Add( segment );
         }
      }
   }
END_PROFILE("::FindPathSegmentElems")  
}






//#ifndef RAD_RELEASE
IntersectDSG* IntersectManager::FindIntersectionTri
(  
    rmt::Vector& irPosn, 
    rmt::Vector* opTriPoints,
    rmt::Vector& orIntersectPosn,
    rmt::Vector& orIntersectNorm
)
{
    SpatialNode& rCurrentLeaf = GetRenderManager()->pWorldScene()->mStaticTreeWalker.rSeekLeaf((Vector3f&)irPosn);

    rmt::Vector tmpVect, tmpVect2, TriPts[3], TriNorm;//, TriCtr;
    float  DistToPlane, ClosestDistToPlane = 20000.0f;//TriRadius,
    int foundTerrainType;

    //   iRadius *= 2.5f;
    //MS7 Default, in case we don't find an intersection

    for( int i=rCurrentLeaf.mIntersectElems.mUseSize-1; i>-1; i-- )
    {
        for( int j=rCurrentLeaf.mIntersectElems[i]->nTris()-1; j>-1; j-- )
        {
            foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mTri(j,TriPts,TriNorm );//TriCtr, 
            // This first test finds the ground plane directly beneath the sphere (on the y axis)

            // make tmpVect a vector from the sphere position towards the ground
            tmpVect2.Set( 0.0f, -1.0f, 0.0f );

            //if(( !obFoundPlane ) && ( tmpVect2.Dot( TriNorm ) < 0 ))
            {
                tmpVect.Set( irPosn.x, 10000.0f, irPosn.z );
                
                if( IntersectWithPlane( TriPts[ 0 ], TriNorm, tmpVect, tmpVect2, DistToPlane ) )
                {
                    if(( DistToPlane >= 0.0f ) /*&& ( DistToPlane <= iRadius )*/)
                    {
                        rmt::Vector pointOnPlane = tmpVect2;
                        pointOnPlane.Scale( DistToPlane );

                        pointOnPlane.Add( tmpVect );
//                        pointOnPlane.Add( irPosn );

                        tmpVect.Sub(TriPts[0],TriPts[1]);
                        tmpVect.CrossProduct(TriNorm);

                        tmpVect2.Sub(pointOnPlane,TriPts[1]);
                        if( tmpVect.Dot(tmpVect2) >= -0.00f )
                        {
                            tmpVect.Sub(TriPts[1],TriPts[2]);
                            tmpVect.CrossProduct(TriNorm);

                            tmpVect2.Sub(pointOnPlane,TriPts[2]);
                            if( tmpVect.Dot(tmpVect2) >= -0.00f )
                            {
                                tmpVect.Sub(TriPts[2],TriPts[0]);
                                tmpVect.CrossProduct(TriNorm);

                                tmpVect2.Sub(pointOnPlane,TriPts[0]);
                                if( tmpVect.Dot(tmpVect2) >= -0.00f )
                                {
                                    opTriPoints[0] = TriPts[0];
                                    opTriPoints[1] = TriPts[1];
                                    opTriPoints[2] = TriPts[2];
                                    orIntersectPosn = pointOnPlane;
                                    orIntersectNorm = TriNorm;

                                    return rCurrentLeaf.mIntersectElems[i];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return NULL;
}
IntersectDSG* IntersectManager::FindIntersectionTriNew
(  
    rmt::Vector& irPosn, 
    rmt::Vector* opTriPoints,
    rmt::Vector& orIntersectPosn,
    rmt::Vector& orIntersectNorm
)
{
    SpatialNode& rCurrentLeaf = GetRenderManager()->pWorldScene()->mStaticTreeWalker.rSeekLeaf((Vector3f&)irPosn);

    rmt::Vector tmpVect, tmpVect2, TriPts[3], TriNorm;//, TriCtr;
    float  DistToPlane, ClosestDistToPlane = 20000.0f;//TriRadius,
    int foundTerrainType;

    //   iRadius *= 2.5f;
    //MS7 Default, in case we don't find an intersection

#if defined(RAD_PS2) && !defined(RAD_MW)
    radTime64 time = radTimeGetMicroseconds64();
static rmt::Vector4 alignedVertices[3] __attribute__((aligned(16))); //vf1,vf2,vf3
static rmt::Vector4 alignedNormal __attribute__((aligned(16)));      //vf4

static rmt::Vector4 alignedRayOrigin __attribute__((aligned(16)));   //vf5

static rmt::Vector4 alignedPointOnPlane __attribute__((aligned(16))); //vf7
static rmt::Vector4 alignedDistFromPlane __attribute__((aligned(16)));//vf8

    for( int i=rCurrentLeaf.mIntersectElems.mUseSize-1; i>-1; i-- )
    {
        rCurrentLeaf.mIntersectElems[i]->IntoTheVoid_WithGoFastaStripes();
        for( int j=rCurrentLeaf.mIntersectElems[i]->nTris()-1; j>-1; j-- )
        {
/*
            foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mTri(j,alignedVertices,alignedNormal);//TriCtr, 

            alignedRayVector.Set( 0.0f, -1.0f, 0.0f );
            alignedRayOrigin.Set( irPosn.x, 10000.0f, irPosn.z );
            
            if( IntersectWithPlane( alignedVertices[0], alignedNormal, alignedRayOrigin, alignedRayVector, alignedDistFromPlane.x) )
            {
*/
            TriPts[0] = irPosn;
            foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mFlatTriFast(j,TriPts,TriNorm );//TriCtr, 
            if(foundTerrainType==-1) continue;
            alignedVertices[0] = TriPts[0];
            alignedVertices[1] = TriPts[1];
            alignedVertices[2] = TriPts[2];
            alignedNormal      = TriNorm;

            TriPts[2].Set( 0.0f, -1.0f, 0.0f );
            TriPts[1].Set( irPosn.x, 10000.0f, irPosn.z );
            if( IntersectWithPlane( TriPts[ 0 ], TriNorm, TriPts[1], TriPts[2], DistToPlane ) )
            {
                if( DistToPlane >= 0.0f )//alignedDistFromPlane.x >= 0.0f  )
                {
                    alignedDistFromPlane.Set( DistToPlane, DistToPlane, DistToPlane, 1.0f );
                    //alignedDistFromPlane.y = alignedDistFromPlane.x;
                    //alignedDistFromPlane.z = alignedDistFromPlane.x;

                    alignedPointOnPlane = TriPts[2];
                    alignedRayOrigin    = TriPts[1];

                    asm __volatile__("
                        lqc2        vf7, 0(%5)     # load pointOnPlane
                        lqc2        vf8, 0(%6)     # load distFromPlane
                        lqc2        vf5, 0(%4)     # load rayOrigin
                        vmul.xyz    vf7, vf7,  vf8 # pointOnPlane.Scale( DistToPlane );
                        lqc2        vf1, 0(%0)     # load vertex0
                        lqc2        vf2, 0(%1)     # load vertex1
                        vadd.xyz    vf7, vf7,  vf5 # pointOnPlane.Add( tmpVect ); tempVect == alignedRayOrigin, tempVect2 == alignedRayVector
                        lqc2        vf4, 0(%3)     # load normal
                        vsub.xyz    vf9, vf1,  vf2 # tmpVect.Sub(TriPts[0],TriPts[1]);
                        lqc2        vf3, 0(%2)     # load vertex2
                        vopmula.xyz ACC, vf9,  vf4 # outer product stage 1 tmpVect.CrossProduct(TriNorm); 
                        vopmsub.xyz vf9, vf4,  vf9 # outer product stage 2
                        vsub.xyz    vf10,vf7,  vf2 # tmpVect2.Sub(pointOnPlane,TriPts[1]);
                        vmul.xyz    vf20,vf10, vf9 # ==if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        sqc2        vf20, 0(%0)     # store result in vertex 0
                        vsub.xyz    vf9, vf2,  vf3 # tmpVect.Sub(TriPts[1],TriPts[2]);
                        vopmula.xyz ACC, vf9,  vf4 # outer product stage 1 tmpVect.CrossProduct(TriNorm); 
                        vopmsub.xyz vf9, vf4,  vf9 # outer product stage 2
                        vsub.xyz    vf10,vf7,  vf3 # tmpVect2.Sub(pointOnPlane,TriPts[2]);
                        vmul.xyz    vf21,vf10, vf9 # ==if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        sqc2        vf21, 0(%1)     # store result in vertex 1
                        vsub.xyz    vf9, vf3,  vf1 # tmpVect.Sub(TriPts[2],TriPts[0]);
                        vopmula.xyz ACC, vf9,  vf4 # outer product stage 1 tmpVect.CrossProduct(TriNorm); 
                        vopmsub.xyz vf9, vf4,  vf9 # outer product stage 2
                        vsub.xyz    vf10,vf7,  vf1 # tmpVect2.Sub(pointOnPlane,TriPts[0]);
                        vmul.xyz    vf22,vf10, vf9 # ==if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        sqc2        vf22, 0(%2)     # store result in vertex 2
                        sqc2        vf7,  0(%5)     # store result in vertex 2
                   ": // no outputs
                    : "r" (&(alignedVertices[0])),
                      "r" (&(alignedVertices[1])),
                      "r" (&(alignedVertices[2])), 
                      "r" (&alignedNormal), 
                      "r" (&alignedRayOrigin), 
                      "r" (&alignedPointOnPlane), 
                      "r" (&alignedDistFromPlane)
                    : "memory" );

                    if(     ((alignedVertices[0].x+alignedVertices[0].y+alignedVertices[0].z) >= 0.00f)
                        &&  ((alignedVertices[1].x+alignedVertices[1].y+alignedVertices[1].z) >= 0.00f)
                        &&  ((alignedVertices[2].x+alignedVertices[2].y+alignedVertices[2].z) >= 0.00f)
                        )
                    {
                        orIntersectNorm = alignedNormal;
                        orIntersectPosn = alignedPointOnPlane;
                        rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();

                        //time = radTimeGetMicroseconds64()-time;
                        //rReleasePrintf("vu0 found t=%d  \t-=- ",(int)time);

                        return rCurrentLeaf.mIntersectElems[i];
                    }
                }
            }
        }
        rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();
    }

    //time = radTimeGetMicroseconds64()-time;
    //rReleasePrintf("vu0 miss t=%d  \t-=- ",(int)time);
    return NULL;

#else
//    time = radTimeGetMicroseconds64();
    for( int i=rCurrentLeaf.mIntersectElems.mUseSize-1; i>-1; i-- )
    {
        rCurrentLeaf.mIntersectElems[i]->IntoTheVoid_WithGoFastaStripes();
        for( int j=rCurrentLeaf.mIntersectElems[i]->nTris()-1; j>-1; j-- )
        {
            foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mTri(j,TriPts,TriNorm );//TriCtr, 
            // This first test finds the ground plane directly beneath the sphere (on the y axis)

            // make tmpVect a vector from the sphere position towards the ground
            tmpVect2.Set( 0.0f, -1.0f, 0.0f );

            //if(( !obFoundPlane ) && ( tmpVect2.Dot( TriNorm ) < 0 ))
            {
                tmpVect.Set( irPosn.x, 10000.0f, irPosn.z );
                
                if( IntersectWithPlane( TriPts[ 0 ], TriNorm, tmpVect, tmpVect2, DistToPlane ) )
                {
                    if(( DistToPlane >= 0.0f ) )
                    {
                        rmt::Vector pointOnPlane = tmpVect2;
                        pointOnPlane.Scale( DistToPlane );

                        pointOnPlane.Add( tmpVect );

                        tmpVect.Sub(TriPts[0],TriPts[1]);
                        tmpVect.CrossProduct(TriNorm);

                        tmpVect2.Sub(pointOnPlane,TriPts[1]);
                        if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        {
                            tmpVect.Sub(TriPts[1],TriPts[2]);
                            tmpVect.CrossProduct(TriNorm);

                            tmpVect2.Sub(pointOnPlane,TriPts[2]);
                            if( tmpVect.Dot(tmpVect2) >= 0.00f)
                            {
                                tmpVect.Sub(TriPts[2],TriPts[0]);
                                tmpVect.CrossProduct(TriNorm);

                                tmpVect2.Sub(pointOnPlane,TriPts[0]);
                                if( tmpVect.Dot(tmpVect2) >= 0.00f)
                                {
                                    orIntersectNorm = TriNorm;
                                    orIntersectPosn = pointOnPlane;
                                    rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();

                                    //time = radTimeGetMicroseconds64()-time;
                                    //rReleasePrintf("normal found t=%d\n",(int)time);
                                    
                                    return rCurrentLeaf.mIntersectElems[i];
                                }
                            }
                        }
                    }
                }
            }
        }
        rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();
    }

    return NULL;
#endif
}
//#endif

#if 1
//========================================================================
// IntersectManager::FindIntersection
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
int IntersectManager::FindIntersection
(  
    rmt::Vector&   irPosn, 
    bool&          obFoundPlane,
    rmt::Vector&   orGroundPlaneNorm,
    rmt::Vector&   orGroundPlanePosn
)
{
    SpatialNode& rCurrentLeaf = GetRenderManager()->pWorldScene()->mStaticTreeWalker.rSeekLeaf((Vector3f&)irPosn);

    rmt::Vector tmpVect, tmpVect2, TriPts[3], TriNorm;//, TriCtr;
    float  DistToPlane, ClosestDistToPlane = 20000.0f;//TriRadius,
    int foundTerrainType;

    //   iRadius *= 2.5f;
    //MS7 Default, in case we don't find an intersection
    obFoundPlane = false;

#if defined(RAD_PS2) && !defined(RAD_MW)
    //radTime64 time = radTimeGetMicroseconds64();
static rmt::Vector4 alignedVertices[3] __attribute__((aligned(16))); //vf1,vf2,vf3
static rmt::Vector4 alignedNormal __attribute__((aligned(16)));      //vf4

static rmt::Vector4 alignedRayOrigin __attribute__((aligned(16)));   //vf5

static rmt::Vector4 alignedPointOnPlane __attribute__((aligned(16))); //vf7
static rmt::Vector4 alignedDistFromPlane __attribute__((aligned(16)));//vf8

    for( int i=rCurrentLeaf.mIntersectElems.mUseSize-1; i>-1; i-- )
    {
        rCurrentLeaf.mIntersectElems[i]->IntoTheVoid_WithGoFastaStripes();
        for( int j=rCurrentLeaf.mIntersectElems[i]->nTris()-1; j>-1; j-- )
        {
/*
            foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mTri(j,alignedVertices,alignedNormal);//TriCtr, 

            alignedRayVector.Set( 0.0f, -1.0f, 0.0f );
            alignedRayOrigin.Set( irPosn.x, 10000.0f, irPosn.z );
            
            if( IntersectWithPlane( alignedVertices[0], alignedNormal, alignedRayOrigin, alignedRayVector, alignedDistFromPlane.x) )
            {
*/
            TriPts[0] = irPosn;
            foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mFlatTriFast(j,TriPts,TriNorm );//TriCtr, 
            if(foundTerrainType==-1) continue;
            alignedVertices[0] = TriPts[0];
            alignedVertices[1] = TriPts[1];
            alignedVertices[2] = TriPts[2];
            alignedNormal      = TriNorm;

            TriPts[2].Set( 0.0f, -1.0f, 0.0f );
            TriPts[1].Set( irPosn.x, 10000.0f, irPosn.z );
            if( IntersectWithPlane( TriPts[ 0 ], TriNorm, TriPts[1], TriPts[2], DistToPlane ) )
            {
                if( DistToPlane >= 0.0f )//alignedDistFromPlane.x >= 0.0f  )
                {
                    alignedDistFromPlane.Set( DistToPlane, DistToPlane, DistToPlane, 1.0f );
                    //alignedDistFromPlane.y = alignedDistFromPlane.x;
                    //alignedDistFromPlane.z = alignedDistFromPlane.x;

                    alignedPointOnPlane = TriPts[2];
                    alignedRayOrigin    = TriPts[1];

                    asm __volatile__("
                        lqc2        vf7, 0(%5)     # load pointOnPlane
                        lqc2        vf8, 0(%6)     # load distFromPlane
                        lqc2        vf5, 0(%4)     # load rayOrigin
                        vmul.xyz    vf7, vf7,  vf8 # pointOnPlane.Scale( DistToPlane );
                        lqc2        vf1, 0(%0)     # load vertex0
                        lqc2        vf2, 0(%1)     # load vertex1
                        vadd.xyz    vf7, vf7,  vf5 # pointOnPlane.Add( tmpVect ); tempVect == alignedRayOrigin, tempVect2 == alignedRayVector
                        lqc2        vf4, 0(%3)     # load normal
                        vsub.xyz    vf9, vf1,  vf2 # tmpVect.Sub(TriPts[0],TriPts[1]);
                        lqc2        vf3, 0(%2)     # load vertex2
                        vopmula.xyz ACC, vf9,  vf4 # outer product stage 1 tmpVect.CrossProduct(TriNorm); 
                        vopmsub.xyz vf9, vf4,  vf9 # outer product stage 2
                        vsub.xyz    vf10,vf7,  vf2 # tmpVect2.Sub(pointOnPlane,TriPts[1]);
                        vmul.xyz    vf20,vf10, vf9 # ==if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        sqc2        vf20, 0(%0)     # store result in vertex 0
                        vsub.xyz    vf9, vf2,  vf3 # tmpVect.Sub(TriPts[1],TriPts[2]);
                        vopmula.xyz ACC, vf9,  vf4 # outer product stage 1 tmpVect.CrossProduct(TriNorm); 
                        vopmsub.xyz vf9, vf4,  vf9 # outer product stage 2
                        vsub.xyz    vf10,vf7,  vf3 # tmpVect2.Sub(pointOnPlane,TriPts[2]);
                        vmul.xyz    vf21,vf10, vf9 # ==if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        sqc2        vf21, 0(%1)     # store result in vertex 1
                        vsub.xyz    vf9, vf3,  vf1 # tmpVect.Sub(TriPts[2],TriPts[0]);
                        vopmula.xyz ACC, vf9,  vf4 # outer product stage 1 tmpVect.CrossProduct(TriNorm); 
                        vopmsub.xyz vf9, vf4,  vf9 # outer product stage 2
                        vsub.xyz    vf10,vf7,  vf1 # tmpVect2.Sub(pointOnPlane,TriPts[0]);
                        vmul.xyz    vf22,vf10, vf9 # ==if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        sqc2        vf22, 0(%2)     # store result in vertex 2
                        sqc2        vf7,  0(%5)     # store result in vertex 2
                   ": // no outputs
                    : "r" (&(alignedVertices[0])),
                      "r" (&(alignedVertices[1])),
                      "r" (&(alignedVertices[2])), 
                      "r" (&alignedNormal), 
                      "r" (&alignedRayOrigin), 
                      "r" (&alignedPointOnPlane), 
                      "r" (&alignedDistFromPlane)
                    : "memory" );

                    if(     ((alignedVertices[0].x+alignedVertices[0].y+alignedVertices[0].z) >= 0.00f)
                        &&  ((alignedVertices[1].x+alignedVertices[1].y+alignedVertices[1].z) >= 0.00f)
                        &&  ((alignedVertices[2].x+alignedVertices[2].y+alignedVertices[2].z) >= 0.00f)
                        )
                    {
                        orGroundPlaneNorm = alignedNormal;
                        orGroundPlanePosn = alignedPointOnPlane;
                        obFoundPlane = true;
                        rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();

                        //time = radTimeGetMicroseconds64()-time;
                        //rReleasePrintf("vu0 found t=%d  \t-=- ",(int)time);

                        return foundTerrainType;
                        //j=-1; //i=-1;
                    }
                }
            }
        }
        rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();
    }

    //time = radTimeGetMicroseconds64()-time;
    //rReleasePrintf("vu0 miss t=%d  \t-=- ",(int)time);
    return 0;

#else
    obFoundPlane = false;
//    time = radTimeGetMicroseconds64();
    for( int i=rCurrentLeaf.mIntersectElems.mUseSize-1; i>-1; i-- )
    {
        rCurrentLeaf.mIntersectElems[i]->IntoTheVoid_WithGoFastaStripes();
        for( int j=rCurrentLeaf.mIntersectElems[i]->nTris()-1; j>-1; j-- )
        {
            //foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mTri(j,TriPts,TriNorm );//TriCtr, 
            TriPts[0] = irPosn;
            foundTerrainType = rCurrentLeaf.mIntersectElems[i]->mFlatTriFast(j,TriPts,TriNorm );
            if(foundTerrainType==-1) continue;
            // This first test finds the ground plane directly beneath the sphere (on the y axis)

            // make tmpVect a vector from the sphere position towards the ground
            tmpVect2.Set( 0.0f, -1.0f, 0.0f );

            //if(( !obFoundPlane ) && ( tmpVect2.Dot( TriNorm ) < 0 ))
            {
                tmpVect.Set( irPosn.x, 10000.0f, irPosn.z );
                
                if( IntersectWithPlane( TriPts[ 0 ], TriNorm, tmpVect, tmpVect2, DistToPlane ) )
                {
                    if(( DistToPlane >= 0.0f ) )
                    {
                        rmt::Vector pointOnPlane = tmpVect2;
                        pointOnPlane.Scale( DistToPlane );

                        pointOnPlane.Add( tmpVect );

                        tmpVect.Sub(TriPts[0],TriPts[1]);
                        tmpVect.CrossProduct(TriNorm);

                        tmpVect2.Sub(pointOnPlane,TriPts[1]);
                        if( tmpVect.Dot(tmpVect2) >= 0.00f)
                        {
                            tmpVect.Sub(TriPts[1],TriPts[2]);
                            tmpVect.CrossProduct(TriNorm);

                            tmpVect2.Sub(pointOnPlane,TriPts[2]);
                            if( tmpVect.Dot(tmpVect2) >= 0.00f)
                            {
                                tmpVect.Sub(TriPts[2],TriPts[0]);
                                tmpVect.CrossProduct(TriNorm);

                                tmpVect2.Sub(pointOnPlane,TriPts[0]);
                                if( tmpVect.Dot(tmpVect2) >= 0.00f)
                                {
                                    orGroundPlaneNorm = TriNorm;
                                    orGroundPlanePosn = pointOnPlane;
                                    obFoundPlane = true;
                                    rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();

                                    //time = radTimeGetMicroseconds64()-time;
                                    //rReleasePrintf("normal found t=%d\n",(int)time);
                                    
                                    return foundTerrainType;
                                }
                            }
                        }
                    }
                }
            }
        }
        rCurrentLeaf.mIntersectElems[i]->OutOfTheVoid_WithGoFastaStripes();
    }

    return 0;
#endif
}

#else
//========================================================================
// IntersectManager::FindIntersection
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
int IntersectManager::FindIntersection
(  
   rmt::Vector&   irPosn, 
   float          iRadius,
   rmt::Vector&   orDeepestIntersectPosn,
   rmt::Vector&   orDeepestIntersectNorm
)
{
    SpatialNode<tGeometry,tGeometry,IntersectDSG>& rCurrentLeaf = GetRenderManager()->GetWorldScene()->mStaticTreeWalker.rSeekLeaf((Vector3f&)irPosn);

    rmt::Vector tmpVect, tmpVect2, TriPts[3], TriNorm, TriCtr;
    float TriRadius, DistToPlane, ClosestDistToPlane = 20000.0f;
    int foundTerrainType = -1;
    //   iRadius *= 2.5f;
    //MS7 Default, in case we don't find an intersection
    orDeepestIntersectPosn     = irPosn;
    orDeepestIntersectPosn.y  -= 2.0f*iRadius;

    orDeepestIntersectNorm.Set(0.0f,1.0f,0.0f);

    for( int i=rCurrentLeaf.mIntersectElems.mUseSize-1; i>-1; i-- )
    {
        for( int j=rCurrentLeaf.mIntersectElems[i]->nTris()-1; j>-1; j-- )
        {
            TriRadius = rCurrentLeaf.mIntersectElems[i]->mTri(j,TriPts,TriNorm,TriCtr, &foundTerrainType );

            // make tmpVect a vector from the wheel position towards the ground
            tmpVect2.Set( 0.0f, -1.0f, 0.0f );

            if( TriNorm.y < 0 )
            {
                int i = 0;
            }

            if( tmpVect2.Dot( TriNorm ) < 0 )
            {
                if( IntersectWithPlane( TriPts[ 0 ], TriNorm, irPosn, tmpVect2, DistToPlane ) )
                {
//                    if( (DistToPlane >= 0) && (DistToPlane <= iRadius) && (DistToPlane < ClosestDistToPlane) )
                    {
                        rmt::Vector pointOnPlane = tmpVect2;
                        pointOnPlane.Scale( DistToPlane );
                        pointOnPlane.Add( irPosn );

                        (tmpVect.Sub(TriPts[0],TriPts[1])).CrossProduct(TriNorm);

                        if( tmpVect.Dot(tmpVect2.Sub(pointOnPlane,TriPts[1])) >= 0.0f )
                        {
                            (tmpVect.Sub(TriPts[1],TriPts[2])).CrossProduct(TriNorm);

                            if( tmpVect.Dot(tmpVect2.Sub(pointOnPlane,TriPts[1])) >= 0.0f )
                            {
                                (tmpVect.Sub(TriPts[2],TriPts[0])).CrossProduct(TriNorm);

                                if( tmpVect.Dot(tmpVect2.Sub(pointOnPlane,TriPts[0])) >= 0.0f )
                                {
                                    ClosestDistToPlane = DistToPlane;

                                    orDeepestIntersectNorm = TriNorm;
                                    orDeepestIntersectPosn = pointOnPlane;
                                    return foundTerrainType;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
/*
    for( int i=rCurrentLeaf.mIntersectElems.mUseSize-1; i>-1; i-- )
    {
        for( int j=rCurrentLeaf.mIntersectElems[i]->nTris()-1; j>-1; j-- )
        {
            TriRadius = rCurrentLeaf.mIntersectElems[i]->mTri(j,TriPts,TriNorm,TriCtr);
            //if( tmpVect.Sub(irPosn,TriCtr).MagnitudeSqr() < (TriRadius+iRadius)*(TriRadius+iRadius))
            {
                DistToPlane = TriNorm.Dot(tmpVect.Sub(irPosn,TriPts[0])); 
                //if( (DistToPlane >= 0.0f)&&(DistToPlane <= iRadius)&&(DistToPlane < ClosestDistToPlane) )
                {
                    {
                        (tmpVect.Sub(TriPts[0],TriPts[1])).CrossProduct(TriNorm);

                        //rAssert( tmpVect.Dot(tmpVect2.Sub(TriCtr,TriPts[1])) >= 0.0f );

                        if( tmpVect.Dot(tmpVect2.Sub(irPosn,TriPts[1])) >= 0.0f )
                        {
                            (tmpVect.Sub(TriPts[1],TriPts[2])).CrossProduct(TriNorm);

                            //rAssert( tmpVect.Dot(tmpVect2.Sub(TriCtr,TriPts[2])) >= 0.0f );

                            if( tmpVect.Dot(tmpVect2.Sub(irPosn,TriPts[1])) >= 0.0f )
                            {
                                (tmpVect.Sub(TriPts[2],TriPts[0])).CrossProduct(TriNorm);

                                //rAssert( tmpVect.Dot(tmpVect2.Sub(TriCtr,TriPts[0])) >= 0.0f );

                                if( tmpVect.Dot(tmpVect2.Sub(irPosn,TriPts[0])) >= 0.0f )
                                {
                                    tmpVect2 = TriNorm;
                                    tmpVect.Sub(irPosn,TriNorm.Scale(DistToPlane));               

                                    ClosestDistToPlane = DistToPlane;

                                    orDeepestIntersectNorm = tmpVect2;
                                    orDeepestIntersectPosn = tmpVect;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
*/
   if( ClosestDistToPlane == 20000.0f )
   {
      return -1;
   }
   else if( orDeepestIntersectNorm.y < 0.0001f )
   {
      return -1;
   }
   else
   {
      return 0;
   }
}
#endif


bool IntersectManager::LineOfSightXZ( const rmt::Vector& start, const rmt::Vector& end, const DynaPhysDSG* avoidObject )
{

    rmt::Vector segmentCenter =  ( start + end ) / 2;
    float radius = ( segmentCenter - start ).Magnitude();

    rmt::Vector2 midPoint;    
    midPoint.x = ( start.x + end.x ) * 0.5f;
    midPoint.y = ( start.z + end.z ) * 0.5f;

    float length;
    rmt::Vector2 direction;
    {
        float diffX = end.x - start.x;
        float diffZ = end.z - start.z;
        length = rmt::Sqrt( diffX * diffX + diffZ * diffZ );
        float scale = 1.0f / length;
        direction.x = diffX * scale;
        direction.y = diffZ * scale;
    }
    float halfLen = length * 0.5f;

    ReserveArray< StaticPhysDSG* > staticsList( 200 );
    
    bool lineOfSight = true;
    // Test static objects
    FindStaticPhysElems( segmentCenter, radius, staticsList );     
    for ( int i = 0 ; i < staticsList.mUseSize ; i++ )
    {
        rmt::Box3D box;
        staticsList[i]->GetBoundingBox( &box );
        if ( IntersectsXZ( direction, midPoint, halfLen, box ) )
        {   
            lineOfSight = false;
            break;
        }
    }
    // Test fence pieces
    if ( lineOfSight )
    {
        ReserveArray< FenceEntityDSG* > fenceList(400);
        FindFenceElems( segmentCenter, radius, fenceList );
        for ( int i = 0 ; i < fenceList.mUseSize ; i++ )
        {
            rmt::Box3D box;
            fenceList[i]->GetBoundingBox( &box );
            if ( IntersectsXZ( direction, midPoint, halfLen, box ) )
            {   
                lineOfSight = false;
                break;
            }
        }
    }
    // Test dynamic objects
    if ( lineOfSight )
    {
        ReserveArray< DynaPhysDSG* > dynaList(200);
        FindDynaPhysElems( segmentCenter, radius, dynaList );
        for ( int i = 0 ; i < dynaList.mUseSize ; i++ )
        {
            // Avoid testing line of sight with the actor's DSG object
            if ( dynaList[i] == avoidObject )
            {
                continue;
            }

            rmt::Box3D box;
            dynaList[i]->GetBoundingBox( &box );
            if ( IntersectsXZ( direction, midPoint, halfLen, box ) )
            {   
                lineOfSight = false;
                break;
            }
        }
    }
    return lineOfSight;

}
bool IntersectManager::LineOfSight( const rmt::Vector& start, const rmt::Vector& end, const DynaPhysDSG* avoidObject )
{

    rmt::Vector segmentCenter =  ( start + end ) / 2;
    float radius = ( segmentCenter - start ).Magnitude();

    rmt::Vector midPoint;    
    midPoint = ( end + start ) * 0.5f;

    float length;
    rmt::Vector direction = end - start;
    length = direction.Length();
    direction.Normalize();
    float halfLen = length * 0.5f;

    ReserveArray< StaticPhysDSG* > staticsList( 200 );
    
    bool lineOfSight = true;
    // Test static objects
    FindStaticPhysElems( segmentCenter, radius, staticsList );     
    for ( int i = 0 ; i < staticsList.mUseSize ; i++ )
    {
        rmt::Box3D box;
        staticsList[i]->GetBoundingBox( &box );
        if ( Intersects( direction, midPoint, halfLen, box ) )
        {   
            lineOfSight = false;
            break;
        }
    }
    // Test fence pieces
    if ( lineOfSight )
    {
        ReserveArray< FenceEntityDSG* > fenceList(400);
        FindFenceElems( segmentCenter, radius, fenceList );
        for ( int i = 0 ; i < fenceList.mUseSize ; i++ )
        {
            rmt::Box3D box;
            fenceList[i]->GetBoundingBox( &box );
            if ( Intersects( direction, midPoint, halfLen, box ) )
            {   
                lineOfSight = false;
                break;
            }
        }
    }
    // Test dynamic objects
    if ( lineOfSight )
    {
        ReserveArray< DynaPhysDSG* > dynaList(200);
        FindDynaPhysElems( segmentCenter, radius, dynaList );
        for ( int i = 0 ; i < dynaList.mUseSize ; i++ )
        {
            // Avoid testing line of sight with the actor's DSG object
            if ( dynaList[i] == avoidObject )
            {
                continue;
            }

            rmt::Box3D box;
            dynaList[i]->GetBoundingBox( &box );
            if ( Intersects( direction, midPoint, halfLen, box ) )
            {   
                lineOfSight = false;
                break;
            }
        }
    }
    return lineOfSight;

}

//************************************************************************
//
// Protected Member Functions : IntersectManager 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : IntersectManager 
//
//************************************************************************


//========================================================================
// IntersectManager::IntersectManager
//========================================================================
//
// Description: 
//                  Determines if a line segment intersects a bounding box
//                  line segment is broken into a direction vector, the midway point of the segment
//                  and the length / 2. height values are ignored when calculating 
//                  for a minor performance boost
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================

bool IntersectManager::IntersectsXZ( const rmt::Vector2& direction, 
                                        const rmt::Vector2& midPoint,
                                        float halfLen,
                                        const rmt::Box3D& box )
{
    rmt::Vector boxMid = box.Mid();

    rmt::Vector2 boxExtents;
    boxExtents.x = box.high.x - boxMid.x;
    boxExtents.y = box.high.z - boxMid.z;

    rmt::Vector2 t;
    t.x = boxMid.x - midPoint.x;
    t.y = boxMid.z - midPoint.y;

    if ( fabsf( t.x ) > boxExtents.x + halfLen * fabsf( direction.x ) )
    {
        return false;        
    }
    if ( fabsf( t.y ) > boxExtents.y + halfLen * fabsf( direction.y ) )
    {
        return false;
    }
    float r = boxExtents.x * fabsf( direction.y ) +
              boxExtents.y * fabsf( direction.x );
    
    if ( fabsf( t.x * direction.y - t.y * direction.x ) > r )
    {
        return false;
    }
    return true;
}


bool IntersectManager::Intersects( const rmt::Vector& direction, 
                                        const rmt::Vector& midPoint,
                                        float halfLen,
                                        const rmt::Box3D& box )
{
    rmt::Vector boxMid = box.Mid();
  
    rmt::Vector boxExtents;
    boxExtents.x = box.high.x - boxMid.x;
    boxExtents.y = box.high.y - boxMid.y;
    boxExtents.z = box.high.z - boxMid.z;

    rmt::Vector t;
    t.x = boxMid.x - midPoint.x;
    t.y = boxMid.y - midPoint.y;
    t.z = boxMid.z - midPoint.z;

    if ( fabsf( t.x ) > boxExtents.x + halfLen * fabsf( direction.x ) )
    {
        return false;        
    }
    if ( fabsf( t.y ) > boxExtents.y + halfLen * fabsf( direction.y ) )
    {
        return false;
    }
    if ( fabsf( t.z ) > boxExtents.z + halfLen * fabsf( direction.z ) )
    {
        return false;
    }

    /////////////////////////////////////////////
    float r = boxExtents.y * fabsf( direction.z ) +
              boxExtents.z * fabsf( direction.y );
    
    if ( fabsf( t.y * direction.z - t.z * direction.y ) > r )
        return false;

    /////////////////////////////////////////////
    r = boxExtents.x * fabsf( direction.z ) +
              boxExtents.z * fabsf( direction.x );
    
    if ( fabsf( t.z * direction.x - t.x * direction.z ) > r )
        return false;
    /////////////////////////////////////////////
    r = boxExtents.x * fabsf( direction.y ) +
              boxExtents.y * fabsf( direction.x );
    
    if ( fabsf( t.x * direction.y - t.y * direction.x ) > r )
        return false;

    return true;
}


//========================================================================
// IntersectManager::IntersectManager
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

IntersectManager::IntersectManager()
:    mbSameFrame(false)
{
}

//========================================================================
// IntersectManager::~IntersectManager
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
IntersectManager::~IntersectManager()
{
}
