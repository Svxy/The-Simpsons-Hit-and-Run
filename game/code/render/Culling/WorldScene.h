#ifndef __WORLD_SCENE_H__
#define __WORLD_SCENE_H__

#include <render/Culling/UseArray.h>
#include <render/Culling/ReserveArray.h>
#include <render/Culling/SpatialTreeIter.h>
#include <vector>
#include <memory/stlallocators.h>
#include <events/eventlistener.h>
#include <events/eventmanager.h>
#include <meta/eventlocator.h>

//#include <render/DSG/IntersectDSG.h>
//#include <render/DSG/StaticPhysDSG.h>
//#include <render/DSG/StaticEntityDSG.h>

#include <raddebugwatch.hpp>


class IntersectDSG;
class StaticEntityDSG;
class StaticPhysDSG;
class SpatialTree;
class DynaPhysDSG;
class IEntityDSG;
class FenceEntityDSG;
class DynaPhysDSG;
class RoadSegmentData;
class PathSegment;
class NodeFLL;
class AnimEntityDSG;
class tGeometry;
class tPointCamera;
class tShader;
//class SpatialTreeIter;

/////////////////////////////////////////////////////////////////////////
// This class couples renderables (currently tGeometries) and such to 
// the Spatial Graphs used to maintain them.
// 
// Currently it handles:
//    -the collection of renderables (tGeometry)
//    -the generation of a Spatial Tree
//       -whose topology is influenced by the spatial distribution of 
//          primgroups
//    -the rendering of all elements within the tree marked for render
//    
/////////////////////////////////////////////////////////////////////////
class WorldScene : EventListener
{
public:
   WorldScene();
   ~WorldScene();

   ////////////////////////////////////////////////////////////
   // Pure3D Data Reference Interface
   ////////////////////////////////////////////////////////////
   //const char* GetStaticInventorySection();

   ////////////////////////////////////////////////////////////
   // Manipulation Interface
   ////////////////////////////////////////////////////////////
   void SetTree( SpatialTree* ipSpatialTree );
   void Init( int nRenderables );
   void Add( tGeometry*                 pGeometry );
   void Add( IntersectDSG*              ipIntersectDSG );
   void Add( StaticPhysDSG*             ipStaticPhysDSG );
   void Add( StaticEntityDSG*           ipStaticEntityDSG );
   void Add( FenceEntityDSG*            ipFenceEntityDSG );
   void Add( AnimCollisionEntityDSG*    ipAnimCollDSG );
   void Add( AnimEntityDSG*             ipAnimDSG );
   void Add( DynaPhysDSG*               ipDynaPhysDSG );
   void Add( TriggerVolume*             ipTriggerVolume );
   void Add( RoadSegment*               ipRoadSegment );
   void Add( PathSegment*               ipPathSegment );

   void GenerateSpatialReps();

   void Move( rmt::Box3D& irOldBBox, IEntityDSG* ipEDSG );
   void Remove( IEntityDSG* ipEDSG );
   void RemoveQuietFail( IEntityDSG* ipEDSG );

   void Render( unsigned int viewIndex );
   void RenderOpaque( void );
   void RenderTranslucent( void );
   void RenderShadows();
   void RenderSimpleShadows( void );
   void RenderShadowCasters();
   
   ////////////////////////////////////////////////////////////
   // Public Tree Masks
   ////////////////////////////////////////////////////////////
   enum
   {
      msClear        = ~SpatialTreeIter::msFilterAll,
      msVisible0     = 0x01,
      msVisible1     = 0x02,
      msVisible2     = 0x04,
      msVisible3     = 0x08,
      msStaticPhys   = 0x0f,
      msDynaPhys     = 0x10
   };
   ////////////////////////////////////////////////////////////
   // Public Members
   ////////////////////////////////////////////////////////////
   SpatialTreeIter mStaticTreeWalker;
   rmt::Vector mEpsilonOffset;

   struct zSortBlah
   {
       IEntityDSG* entityPtr;
       tUID shaderUID;
   };

   void SetRenderAll(bool iRenderAll){ mRenderAll = iRenderAll; }

   void HandleEvent( EventEnum id, void* pEventData );

protected:
    float mDrawDist;
    bool mRenderAll;
   ////////////////////////////////////////////////////////////
   // State Methods
   ////////////////////////////////////////////////////////////
   bool IsPreTreeGen();
   bool IsPostTreeGen();

   ////////////////////////////////////////////////////////////
   // Helper Methods
   ////////////////////////////////////////////////////////////
   void GenerateStaticTree();
   void PopulateStaticTree();

    bool RemoveFromLeaf( IEntityDSG* ipEDSG );
    bool RemovePlace(IEntityDSG* ipEDSG, SpatialNode& irNode);
   //void PlaceStaticGeo(       tGeometry*        pGeometry );
   void Place( IntersectDSG*            ipIntersectDSG );
   void Place( StaticEntityDSG*         ipStaticEntity );
   void Place( StaticPhysDSG*           ipStaticPhys );
   void Place( DynaPhysDSG*             ipDynaPhys );
   void Place( FenceEntityDSG*          ipFence );
   void Place( AnimCollisionEntityDSG*  ipAnimColl );
   void Place( AnimEntityDSG*           ipAnim );
   void Place( TriggerVolume*           ipTriggerVolume );
   void Place( RoadSegment*             ipRoadSegment );
   void Place( PathSegment*             ipPathSegment );

   void MarkCameraVisible( tPointCamera* pCam, unsigned int iFilter );
   void BuildFrustumPlanes( tPointCamera* pCam, FixedArray<rmt::Vector4>& orCamPlanes );

   void RenderScene( unsigned int iFilter, tPointCamera* ipCam );

   ////////////////////////////////////////////////////////////
   // Private Members
   ////////////////////////////////////////////////////////////
   SpatialTree* mpStaticTree;

   tShader* mpTempShader;
   std::vector< IEntityDSG*, s2alloc<IEntityDSG*> > mpZSortsPassShadowCasters;
   std::vector< zSortBlah, s2alloc<zSortBlah> > mpZSorts;
   std::vector< IEntityDSG*, s2alloc<IEntityDSG*> > mpZSortsPass2;
   ReserveArray<IEntityDSG*> mShadowCastersPass1;      
   //ReserveArray<IEntityDSG*> mShadowCastersPass2;      
   FixedArray< rmt::Vector4 > mCamPlanes; 

#ifdef DEBUGWATCH
   unsigned int mDebugMarkTiming,  mDebugWalkTiming, mDebugRenderTiming, 
                mDebugZSWalkTiming,mDebugZSAddTiming,mDebugZSSortTiming;

    bool mDebugSimCollisionVolumeDrawing;
    bool mDebugFenceCollisionVolumeDrawing;
    bool mDebugVehicleCollisionDrawing;
    bool mDebugSimStatsDisplay;
    bool mDebugShowTree;
    int mDebugWatchNumCollisionPairs;
#endif

    void SetVisCone( rmt::Vector& irView, rmt::Vector& irPosn, float iAngleRadians );
    bool IsSphereInCone( rmt::Vector& irCenter, float iRadius );
    rmt::Vector mViewVector, mViewPosn;
    float mViewSinInv, mViewSinSqr, mViewCosSqr;
   ////////////////////////////////////////////////////////////
   // Private Statics
   ////////////////////////////////////////////////////////////
};

#endif