//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        roadmanager.h
//
// Description: Blahblahblah
//
// History:     26/06/2002 + Created -- Cary Brisebois (based on TBJ's work)
//
//=============================================================================

#ifndef ROADMANAGER_H
#define ROADMANAGER_H

//========================================
// Nested Includes
//========================================
#include <render/culling/swaparray.h>

//========================================
// Forward References
//========================================
class Road;
class Intersection;
class RoadSegmentData;
class RoadSegment;
class Lane;

class RoadRenderTest;

static const float NEAR_INFINITY = 100000.0f;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RoadManager
{
public:
    enum { STARTUP = false, SHUTDOWN = true };

    static const float AGAINST_TRAFFIC_COST_MULTIPLIER;

    struct ShortestRoad
    {
        Road* road;
        bool isOutRoad;
        float cost;
    };
    struct NodeData
    {
        Intersection* destIn;
        ShortestRoad* roadToIn;
        ShortestRoad* roadJustBeforeIn;
        float dist; // total distance to the destination intersection
    };
    struct BigIntersection
    {
        Intersection* in;
        SwapArray<NodeData> routesWithMultiplier;
        SwapArray<NodeData> routesNoMultiplier;
    };
    enum ElementType
    {
        ET_INTERSECTION, // Intersection*
        ET_NORMALROAD    // Road*
    };
    struct PathElement
    {
        ElementType type; 
        void* elem;
        bool operator==( const PathElement& right ) const 
        { 
            return(elem==right.elem);
        }
        bool operator!=( const PathElement& right ) const
        {
            return(elem!=right.elem);
        }
    };


    static RoadManager* GetInstance();
    static void Destroy();

    //---------------Initialization
    void Init( bool shutdown );

    void InitializeRoadMemory( unsigned int numRoads );
    void InitializeIntersectionMemory( unsigned int numIntersections );
    void InitializeRoadSegmentDataMemory( unsigned int numSegments );
    void InitializeRoadSegmentMemory( unsigned int numRoadSegments );

    void DumpRoadSegmentDataMemory();

    Road* GetFreeRoadMemory( );
    Intersection* GetFreeIntersectionMemory( );
    RoadSegmentData* GetFreeRoadSegmentDataMemory( );
    RoadSegment* GetFreeRoadSegmentMemory( );

    void AddRoad( Road* pRoad );
    void AddIntersection( Intersection* pIntersection );
    void AddRoadSegmentData( RoadSegmentData* pRoadSegmentData );
    void AddRoadSegment( RoadSegment* pRoadSegment );

    int GetMaxPathElements();

    void CreateRoadNetwork( void );


    //---------------Data Aquisition
    Intersection* FindIntersection( const char* name );
    Intersection* FindIntersection( tUID name );
    Intersection* FindIntersection( rmt::Vector& point );

    Intersection* FindIntersection( int iIndex );

    int GetNumIntersectionsUsed( );
    unsigned int GetNumRoads( );

    bool FindRoad( const rmt::Vector& point, 
        const Road** ppRoad, 
        RoadSegment** ppOutRoadSegment, 
        int& segmentIndex,
        float& in, 
        float& lateral,
        bool considerShortCuts=true ) const;

    static bool FindClosestPointOnRoad( const Road* pRoad, 
                                        const rmt::Vector& pos, 
                                        rmt::Vector& closestPos, 
                                        float& closestDist, 
                                        int& segmentIndex );

    static float DetermineRoadT( RoadSegment* seg, float segT );
    static float DetermineSegmentT( const rmt::Vector& pos, RoadSegment* seg );



    RoadSegmentData* FindRoadSegmentData( const char* name );
    RoadSegmentData* FindRoadSegmentData( tUID name );

    RoadRenderTest* GetRoadRenderTest();

    // fully pathfind from source to target, returning float distance
    float FindPathElementsBetween( 
        bool useMultiplier,                 // IN: direction-biased?
        PathElement& sourceElem,            // IN: starting element
        float sourceT,                      // IN: used only if sourceElem is a road
        const rmt::Vector& sourcePos,       // IN: used only if sourceElem is an intersection
        PathElement& targetElem,            // IN: terminating element
        float targetT,                      // IN: used only if targetElem is a road
        const rmt::Vector& targetPos,       // IN: used only if targetElem is an intersection
        SwapArray<PathElement>& elems );    // OUT: accumulate roads or intersections

    enum PathfindingOption
    {
        PO_SEARCH_SHORTCUTS     = 1 << 0, // whether or not to take into account shortcut roads
        PO_SEARCH_ROADS         = 1 << 1, // whether or not to search for roads
        PO_SEARCH_INTERSECTIONS = 1 << 2, // whether or not to search for intersections

        NUM_POS = ( PO_SEARCH_SHORTCUTS | 
                    PO_SEARCH_ROADS | 
                    PO_SEARCH_INTERSECTIONS ) 
    };

    typedef char PathfindingOptions;

    // search roads and intersections for whichever's closest, given a position
    void FindClosestPathElement(
        const rmt::Vector& pos,         // IN: search center
        float searchRadius,             // IN: search radius
        PathfindingOptions options,        // IN: search options
        PathElement& closestElem,       // OUT: closest element (road or intersection)
        RoadSegment*& closestRoadSeg,   // OUT: if closest element is road, this is closest seg
        float& closestRoadSegT,         // OUT: if closest element is road, this is segment t value
        float& closestRoadT );          // OUT: if closest element is road, this is road's t value

    enum ErrorValue
    {
        DEAD_END,
        STILL_LOOKING,
        FOUND_TARGET,
        FOUND_BIGINTERSECTION,
        UNEXPECTED
    };
    struct DistErrMap
    {
        int ID;
        ErrorValue errVal;
        float dist;
    };

    float FindDistToTargetInOneDirection( 
        bool useMultiplier,
        Intersection* targetInt, 
        Intersection* currInt,
        Intersection* lastInt,
        ShortestRoad* shortestRoadFromLastInt,
        SwapArray<PathElement>& elems,
        ShortestRoad*& firstShortRoad, // needed for traversal dist from src road to srcInt
        ShortestRoad*& lastShortRoad,  // needed for traversal dist from targetInt to target road
        ErrorValue& errVal );
    
    void TraverseRoads( 
        bool useMultiplier,
        Intersection* targetInt, 
        Intersection* currInt,
        Intersection* lastInt,
        SwapArray<PathElement>& elems,
        ErrorValue& errVal );



private:

    static RoadManager* mInstance;

    Road* mRoads;
    unsigned int mNumRoads;
    unsigned int mNumRoadsUsed;

    Intersection* mIntersections;
    unsigned int mNumIntersections;
    unsigned int mNumIntersectionsUsed;

    struct DijkstraNode
    {
        struct AdjacencyData
        {
            DijkstraNode* adjacentNode;
            ShortestRoad* shortestRoadThere;
        };

        Intersection* in;
        float distToSrc;
        DijkstraNode* predecessor;
        ShortestRoad* shortestRoadFromPred;
        bool addedToS;
        SwapArray<AdjacencyData> adjacents;

        DijkstraNode()
        {
            in = NULL;

            distToSrc = NEAR_INFINITY;
            predecessor = NULL;
            shortestRoadFromPred = NULL;
            addedToS = false;
        }

        ~DijkstraNode()
        {
            in = NULL;
            adjacents.Clear();

            distToSrc = NEAR_INFINITY;
            predecessor = NULL;
            shortestRoadFromPred = NULL;
            addedToS = false;
        }

        void Init( float dist, DijkstraNode* pred )
        {
            distToSrc = dist;
            predecessor = pred;

            shortestRoadFromPred = NULL;
            addedToS = false;
        }
    };
    SwapArray<BigIntersection*> mBigIntersections;
    void PopulateConnectivityData( bool useMultiplier, Intersection* intersections, int numInts );
    void VisitAll( SwapArray<DijkstraNode>& nodes );

    RoadSegmentData* mRoadSegmentData;
    unsigned int mNumRoadSegmentData;
    unsigned int mNumRoadSegmentDataUsed;

    RoadSegment** mRoadSegments; // dynamically allocated array of pointers to RoadSegments
    unsigned int mNumRoadSegments;
    unsigned int mNumRoadSegmentsUsed;
    /*
    TransformRoadSegment** mRoadSegments;
    unsigned int mNumRoadSegments;
    unsigned int mNumRoadSegmentsUsed;
    */

    RoadRenderTest* mRender;

    float GetTraversalDistance( ShortestRoad* fromRoad, ShortestRoad* toRoad );


    //Singleton
    RoadManager();
    virtual ~RoadManager();

    //Prevent wasteful constructor creation.
    RoadManager( const RoadManager& roadmanager );
    RoadManager& operator=( const RoadManager& roadmanager );
};

inline unsigned int RoadManager::GetNumRoads()
{
    return mNumRoadsUsed;
}
inline int RoadManager::GetNumIntersectionsUsed()
{
    return mNumIntersectionsUsed;
}
inline RoadRenderTest* RoadManager::GetRoadRenderTest()
{
    return mRender;
}

#endif //ROADMANAGER_H
