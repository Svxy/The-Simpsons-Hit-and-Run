#ifndef __IntersectManager_H__
#define __IntersectManager_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   IntersectManager
//
// Description: The IntersectManager does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/05]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <p3d/p3dtypes.hpp>


//=================================================
// Project Includes
//=================================================
#include <render/Culling/ReserveArray.h>
class StaticPhysDSG;
class FenceEntityDSG;
class DynaPhysDSG;
class AnimCollisionEntityDSG;
class TriggerVolume;
class RoadSegment;
class PathSegment;
class StaticEntityDSG;
class IntersectDSG;

enum eTerrainType
{
	TT_Road,	// Default road terrain. Also used for sidewalk. This is default. If not set, it's this.
	TT_Grass,	// Grass type terrain most everything else which isn't road or sidewalk.
	TT_Sand,	// Sand type terrain.
	TT_Gravel,	// Loose gravel type terrain.
	TT_Water,	// Water on surface type terrain.
	TT_Wood,	// Boardwalks, docks type terrain.
	TT_Metal,  // Powerplant and other structures.
    TT_Dirt,   // Dirt type terrain.
	TT_NumTerrainTypes
};

//========================================================================
//
// Synopsis:   The IntersectManager; Synopsis by Inspection.
//
//========================================================================
class IntersectManager 
{
public:
    // Static Methods (for creating, destroying and acquiring an instance 
    // of the IntersectManager)
    static IntersectManager* CreateInstance();
    static IntersectManager* GetInstance();
    static void  DestroyInstance();

    // "Meat" Interface; it's raison d'etre
    void FindClosestAnyRoad(  const rmt::Vector&   irPosn, 
                              float          iRadius,
                              RoadSegment*&  orpRoad,
                              float&         oDistSqr );

    void FindClosestRoad(     const rmt::Vector&   irPosn, 
                              float          iRadius,
                              RoadSegment*&  orpRoad,
                              float&         oDistSqr );

    void FindFenceElems(      rmt::Vector&   irPosn, 
                              float          iRadius,
                              ReserveArray<FenceEntityDSG*>& orList );

    void FindStaticPhysElems( rmt::Vector&   irPosn, 
                              float          iRadius,
                              ReserveArray<StaticPhysDSG*>& orList );

    void FindStaticElems(rmt::Vector&   irPos,
                              float          iRadius,
                              ReserveArray<StaticEntityDSG*>& orList );


    void FindDynaPhysElems(   rmt::Vector&   irPosn, 
                              float          iRadius,
                              ReserveArray<DynaPhysDSG*>& orList );

    void FindAnimPhysElems(   rmt::Vector&   irPosn, 
                              float          iRadius,
                              ReserveArray<AnimCollisionEntityDSG*>& orList );

    void FindTrigVolElems(    rmt::Vector&   irPosn, 
                              float          iRadius,
                              ReserveArray<TriggerVolume*>& orList );

    void FindRoadSegmentElems(rmt::Vector&   irPos,
                              float          iRadius,
                              ReserveArray<RoadSegment*>& orList );

    void FindPathSegmentElems(rmt::Vector&   irPos,
                              float          iRadius,
                              ReserveArray<PathSegment*>& orList );


//#ifndef RAD_RELEASE
    IntersectDSG* FindIntersectionTri( rmt::Vector& irPosn, 
                                       rmt::Vector* opTriPoints,
                                       rmt::Vector& orIntersectPosn,
                                       rmt::Vector& orIntersectNorm
                                       );
    IntersectDSG* FindIntersectionTriNew( rmt::Vector& irPosn, 
                                       rmt::Vector* opTriPoints,
                                       rmt::Vector& orIntersectPosn,
                                       rmt::Vector& orIntersectNorm
                                       );
//#endif
#if 1
 // The interior flag is now bit packed in as the highest order bit in the TerrainType. The terrain type is returned.
    int FindIntersection(  rmt::Vector&   irPosn, 
                            bool&          obFoundPlane,
                            rmt::Vector&   orGroundPlaneNorm,
                            rmt::Vector&   orGroundPlanePosn );
#else
  // The interior flag is now bit packed in as the highest order bit in the TerrainType.
  // The terrain type is returned, or -1 for no intersection.
    int FindIntersection(  rmt::Vector& irPosn, float iRadius,
                            rmt::Vector& orDeepestIntersectPosn,
                            rmt::Vector& orDeepestIntersectNorm );
#endif

    // Determine if there is a clear line of sight between start and end
    // if specified, avoid object will be ignored in the testing
    // All height values will be ignored in this function
    bool LineOfSightXZ( const rmt::Vector& start, const rmt::Vector& end, const DynaPhysDSG* avoidObject = NULL );
    bool LineOfSight( const rmt::Vector& start, const rmt::Vector& end, const DynaPhysDSG* avoidObject = NULL );


    static bool IntersectsXZ( const rmt::Vector2& direction, 
                        const rmt::Vector2& midPoint,
                        float halfLen,
                        const rmt::Box3D& box );

    static bool Intersects( const rmt::Vector& direction, 
                    const rmt::Vector& midPoint,
                    float halfLen,
                    const rmt::Box3D& box );


    rmt::Vector mCachedPosn;
    float       mCachedRadius;
    bool        mbSameFrame;

protected:
    inline bool CheckRayCache(   rmt::Vector& orPoint,
                                 rmt::Vector& orNorm,
                                 float& orDistToPlane );
    inline bool CheckSphereCache(rmt::Vector& orPoint,
                                 rmt::Vector& orNorm,
                                 float& orDistToPlane );
    inline void RayCache(     rmt::Vector* ipTriPts, rmt::Vector& irNorm );
    inline void SphereCache(  rmt::Vector* ipTriPts, rmt::Vector& irNorm );
    inline bool IsInTri(  rmt::Vector* ipTriPts, 
                          rmt::Vector& irTriNorm, 
                          rmt::Vector& irPoint );

    enum
    {
       msCacheSize = 4
    };
    rmt::Vector mSphereCache[msCacheSize*4];
    rmt::Vector mRayCache[msCacheSize*4];
    int mNextSphereCache, mNextRayCache;


private:
    void ResetCache( const rmt::Vector& irPosn, float iRadius );

    IntersectManager();
    ~IntersectManager();

    bool IntersectWithPlane( rmt::Vector planeOrigin, 
                             rmt::Vector planeNormal,
                             rmt::Vector rayOrigin,
                             rmt::Vector rayVector,
                             float& time );

    
    // Static Private Render Data
    static IntersectManager* mspInstance;
};

//
// A little syntactic sugar for getting at this singleton.
//
inline IntersectManager* GetIntersectManager() 
{ 
   return( IntersectManager::GetInstance() ); 
}

#endif
