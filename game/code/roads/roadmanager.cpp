//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        RoadManager.cpp
//
// Description: Implement RoadManager
//
// History:     26/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/entity.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <render/intersectmanager/intersectmanager.h>
#include <roads/RoadManager.h>
#include <roads/road.h>
#include <roads/intersection.h>
#include <roads/roadsegmentdata.h>
#include <roads/roadsegment.h>
#include <roads/roadrendertest.h>

#ifndef TOOLS
#include <memory/srrmemory.h>
#else
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP()
void* operator new[](size_t size)
{
    return malloc(size);
}
#endif

#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
RoadManager* RoadManager::mInstance = NULL;
const float RoadManager::AGAINST_TRAFFIC_COST_MULTIPLIER = 1.30f;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

RoadManager* RoadManager::GetInstance()
{
MEMTRACK_PUSH_GROUP( "RoadManager" );
    if ( !mInstance )
    {
#ifndef TOOLS
        mInstance = new(GMA_LEVEL_OTHER) RoadManager();
#else
        mInstance = new RoadManager();
#endif
        //TODO: REMOVE
        mInstance->Init( STARTUP );
    }
MEMTRACK_POP_GROUP( "RoadManager" );

    return mInstance;
}

//=============================================================================
// RoadManager::Destroy
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RoadManager::Destroy()
{
    delete mInstance;
    mInstance = NULL;
}


//==============================================================================
// RoadManager::RoadManager
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadManager::RoadManager()
{
    mRoads = NULL;
    mNumRoads = 0;
    mNumRoadsUsed = 0;
    mIntersections = NULL;
    mNumIntersections = 0;
    mNumIntersections = 0;
    mRoadSegmentData = NULL;
    mNumRoadSegmentData = 0;
    mNumRoadSegmentDataUsed = 0;
    mRoadSegments = NULL;
    mNumRoadSegments = 0;
    mNumRoadSegmentsUsed = 0;

#ifndef RAD_RELEASE
#ifdef DEBUGWATCH
    mRender = new(GMA_LEVEL_OTHER) RoadRenderTest;

    RenderManager* rm = GetRenderManager();
    RenderLayer* rl = rm->mpLayer( RenderEnums::LevelSlot );
    rAssert( rl );

    rl->AddGuts( mRender );
#endif
#endif
}

//==============================================================================
// RoadManager::~RoadManager
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadManager::~RoadManager()
{
    Init( SHUTDOWN );
}

//=============================================================================
// RoadManager::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RoadManager::Init( bool shutdown )
{
    if ( mRoads )
    {
        delete[] mRoads;
        mRoads = NULL;
    }
    mNumRoads = 0;
    mNumRoadsUsed = 0;

    if ( mIntersections )
    {
        delete[] mIntersections;
        mIntersections = NULL;
    }
    mNumIntersections = 0;
    mNumIntersectionsUsed = 0;

    if ( mRoadSegmentData )
    {
        delete[] mRoadSegmentData;
        mRoadSegmentData = NULL;
    }
    mNumRoadSegmentData = 0;
    mNumRoadSegmentDataUsed = 0;

    if ( mRoadSegments )
    {
        for(unsigned int x = 0; x < mNumRoadSegments; x++)
        {
            mRoadSegments[x]->Release ();
        }
        delete[] mRoadSegments;
        mRoadSegments = NULL;
    }
    mNumRoadSegments = 0;
    mNumRoadSegmentsUsed = 0;

    if( !shutdown )
    {
        InitializeRoadSegmentDataMemory( 1200 );
        //TODO: base this on incoming chunk data.
        InitializeRoadMemory( 150 );
        InitializeIntersectionMemory( 60 );
        InitializeRoadSegmentMemory( 1200 );
    }
}
void RoadManager::InitializeRoadMemory( unsigned int numRoads )
{

#ifndef TOOLS
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
#endif
#endif

    if( mRoads )
    {
        delete[] mRoads;
    }

    mRoads = new Road[numRoads]; 
    mNumRoads = numRoads;
    mNumRoadsUsed = 0;

#ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    #endif
#endif
}


void RoadManager::InitializeIntersectionMemory( unsigned int numIntersections )
{
#ifndef TOOLS
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
#endif
#endif

    if( mIntersections )
    {
        delete[] mIntersections;
    }

    mIntersections = new Intersection[numIntersections];
    mNumIntersections = numIntersections;
    mNumIntersectionsUsed = 0;

#ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    #endif
#endif
}


void RoadManager::InitializeRoadSegmentDataMemory( unsigned int numSegments )
{
#ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_TEMP );
    #endif
#endif

    if( mRoadSegmentData )
    {
        delete[] mRoadSegmentData;
    }

    mRoadSegmentData = new RoadSegmentData[numSegments];
    mNumRoadSegmentData = numSegments;
    mNumRoadSegmentDataUsed = 0;

#ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_TEMP );
    #endif
#endif
}

void RoadManager::DumpRoadSegmentDataMemory()
{
    if( mRoadSegmentData )
    {
        delete[] mRoadSegmentData;
    }
    mRoadSegmentData = NULL;
    mNumRoadSegmentData = 0;
    mNumRoadSegmentDataUsed = 0;
}


void RoadManager::InitializeRoadSegmentMemory( unsigned int numRoadSegments )
{
#ifndef TOOLS
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
#endif
#endif

    if( mRoadSegments )
    {
        delete[] mRoadSegments;
    }

    mRoadSegments = new RoadSegment*[numRoadSegments];

    unsigned int i = 0;
    for( i ; i < numRoadSegments ; i++ )
    {
        mRoadSegments[i] = new RoadSegment;
        mRoadSegments[i]->AddRef();
    }
    mNumRoadSegments = numRoadSegments;
    mNumRoadSegmentsUsed = 0;

#ifndef TOOLS
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    #endif
#endif
}



Road* RoadManager::GetFreeRoadMemory()
{
    if( 0 <= mNumRoadsUsed && mNumRoadsUsed < mNumRoads )
    {
        Road* road;
        road = &(mRoads[mNumRoadsUsed]);
        return road;
    }

    return NULL;
}

Intersection* RoadManager::GetFreeIntersectionMemory()
{
    if( 0 <= mNumIntersectionsUsed && mNumIntersectionsUsed < mNumIntersections )
    {
        Intersection* intersection;
        intersection = &(mIntersections[mNumIntersectionsUsed]);
        return intersection;
    }

    return NULL;
}

RoadSegmentData* RoadManager::GetFreeRoadSegmentDataMemory()
{
    if( 0 <= mNumRoadSegmentsUsed && mNumRoadSegmentDataUsed < mNumRoadSegmentData )
    {
        RoadSegmentData* rsd;
        rsd = &(mRoadSegmentData[mNumRoadSegmentDataUsed]);
        return rsd;
    }
    else 
    {
        rDebugPrintf( "NUMROADSUSED = %d, NUMROADS = %d\n", 
            mNumRoadSegmentDataUsed, mNumRoadSegmentData );
    }

    return NULL;
}

RoadSegment* RoadManager::GetFreeRoadSegmentMemory()
{
    if( 0 <= mNumRoadSegmentsUsed && mNumRoadSegmentsUsed < mNumRoadSegments )
    {
        return mRoadSegments[mNumRoadSegmentsUsed];
    }
    return NULL;
}


int RoadManager::GetMaxPathElements()
{
    // max elems we could ever have include: 
    //  - sourceroad, 
    //  - targetroad, 
    //  - total number of intersections + their adjoining roads
    return (1 + 1 + (GetNumIntersectionsUsed() * 2) - 1);
}

bool RoadManager::FindClosestPointOnRoad( const Road* pRoad, 
                                          const rmt::Vector& pos, 
                                          rmt::Vector& closestPos, 
                                          float& closestDistSqr,
                                          int& segmentIndex )
{
    rAssert( pRoad );

    rmt::Vector vec0, vec1, vec2, vec3;
    rmt::Vector start, end;

    // Take the destination intersection as the closest point so far
    //
    closestDistSqr = NEAR_INFINITY;
    segmentIndex = -1;

    for( int i = 0; i < (int)(pRoad->GetNumRoadSegments()); i++ )
    {
        RoadSegment* segment = pRoad->GetRoadSegment( i );
        segment->GetCorner( 0, vec0 );
        segment->GetCorner( 1, vec1 );
        segment->GetCorner( 2, vec2 );
        segment->GetCorner( 3, vec3 );

        // Calc the midpoints at the start and end of the segment
        //
        start = (vec0+vec3) * 0.5f;
        end = (vec1+vec2) * 0.5f;

        // Calc the vector along the centre of the segment
        //
        rmt::Vector candidatePt;
        FindClosestPointOnLine( start, end, pos, candidatePt );

        // Calc vector between closest point on segment and target
        //
        float distSqr = (candidatePt - pos).MagnitudeSqr();
        if( distSqr < closestDistSqr )
        {
            closestDistSqr = distSqr;
            closestPos = candidatePt;
            segmentIndex = i;
        }
    }

    return true;
}

float RoadManager::DetermineRoadT( RoadSegment* seg, float segT )
{
    rAssert( seg );

    unsigned int segIndex = seg->GetSegmentIndex();
    Road* road = seg->GetRoad();
    float roadT = 0.0f;
    
    // determine progress "t" along the road
    float segLen = 0.0f;
    RoadSegment* prevSeg = NULL;
    for( unsigned int j=0; j<segIndex; j++ )
    {
        prevSeg = road->GetRoadSegment( j );
        segLen = prevSeg->GetSegmentLength();
        roadT += segLen;
    }
    segLen = seg->GetSegmentLength();
    roadT += segLen * segT;
    roadT /= ((Road*)road)->GetRoadLength();

    return roadT;
}
float RoadManager::DetermineSegmentT( const rmt::Vector& pos, RoadSegment* seg )
{
    rAssert( seg );

    float segT = 0.0f;

    // determine progress "t" along segment
    rmt::Vector vec0, vec1, vec2, vec3;
    rmt::Vector segStart, segEnd, closestPt;

    seg->GetCorner( 0, vec0 );
    seg->GetCorner( 1, vec1 );
    seg->GetCorner( 2, vec2 );
    seg->GetCorner( 3, vec3 );

    segStart = (vec0 + vec3) * 0.5f;
    segEnd = (vec1 + vec2) * 0.5f;

    FindClosestPointOnLine( segStart, segEnd, pos, closestPt );

    segT = GetLineSegmentT( segStart, segEnd, closestPt );
    rAssert( 0.0f <= segT && segT <= 1.0f ); 
    return segT;
}


//=============================================================================
// RoadManager::CreateRoadNetwork
//=============================================================================
// Description: Comment
//
// Parameters:  ( void )
//
// Return:      void 
//
//=============================================================================
void RoadManager::CreateRoadNetwork( void )
{
    for( unsigned int i = 0; i < mNumIntersectionsUsed; ++i )
    {
        // sort which ones are my IN & OUT roads, determine adjacency, 
        // determine if it's a big intersection (3 or more different adjacent 
        // intersections) and do some error checking
        mIntersections[i].SortRoads();
        mIntersections[i].mIndex = i;
    }
    PopulateConnectivityData( true, mIntersections, (int)(mNumIntersectionsUsed) );
    PopulateConnectivityData( false, mIntersections, (int)(mNumIntersectionsUsed) );
}


void RoadManager::PopulateConnectivityData( bool useMultiplier, Intersection* intersections, int numInts )
{
    // nothing to do if there are no intersections
    if( numInts <= 0 )
    {
        return;
    }

    //
    // Create our temporary representation of each intersection:
    // an array of Dijkstra nodes
    // 
    SwapArray<DijkstraNode> nodes;

    HeapMgr()->PushHeap(GMA_TEMP);
    nodes.Allocate( numInts );
    HeapMgr()->PopHeap(GMA_TEMP);

    nodes.mUseSize = numInts;

    for( int i=0; i<numInts; i++ )
    {
         Intersection* in = &(intersections[i]);
         nodes[i].in = in;
    }

    // 
    // Populate adjacency data for each Dijkstra node &
    // store the cost for reachability
    //
    for( int i=0; i<numInts; i++ )
    {
        Intersection* in = nodes[i].in;
        rAssert( in );

        SwapArray<ShortestRoad>* shortestRoads = useMultiplier ?
            &(in->mShortestRoadsToAdjacentIntersectionsWithMultiplier) :
            &(in->mShortestRoadsToAdjacentIntersectionsNoMultiplier) ;

        int numAdjacents = shortestRoads->mUseSize;
        rAssert( numAdjacents > 0 );

        HeapMgr()->PushHeap(GMA_TEMP);
        nodes[i].adjacents.Allocate( numAdjacents );
        HeapMgr()->PopHeap(GMA_TEMP);

        nodes[i].adjacents.mUseSize = numAdjacents;

        int count = 0;

        // for each adjacent intersection, fill in the corresponding
        // dijkstra node pointer and distance
        for( int j=0; j<shortestRoads->mUseSize; j++ )
        {
            ShortestRoad* sr = &((*shortestRoads)[j]);
            rAssert( sr );
            Road* road = sr->road;
            rAssert( road );

            Intersection* adjInt = NULL;
            if( sr->isOutRoad )
            {
                adjInt = (Intersection*) road->GetDestinationIntersection();
            }
            else
            {
                adjInt = (Intersection*) road->GetSourceIntersection();
            }
            rAssert( adjInt );
            rAssert( adjInt != in );

            bool found = false;
            for( int k=0; k<numInts; k++ )
            {
                if( adjInt == nodes[k].in )
                {
                    found = true;
                    nodes[i].adjacents[count].adjacentNode = &(nodes[k]);
                    nodes[i].adjacents[count].shortestRoadThere = sr;
                    count++;
                    break;
                }
            }
            rAssert( found );
        }
        rAssert( count == numAdjacents );
    }

    // foreach BIG intersection, populate its routing table 
    for( int i=0; i<numInts; i++ )
    {
        Intersection* src = &(intersections[i]);
        if( !src->mBigIntersection )
        {
            continue;
        }

        SwapArray<NodeData>* routes = useMultiplier ?
            &(src->mBigIntersection->routesWithMultiplier) :
            &(src->mBigIntersection->routesNoMultiplier) ;

        HeapMgr()->PushHeap(GMA_LEVEL_OTHER);
        routes->Allocate( numInts );
        HeapMgr()->PopHeap(GMA_LEVEL_OTHER);

        routes->mUseSize = numInts;

        // initialize every Dijkstra node 
        for( int j=0; j<numInts; j++ )
        {
            nodes[j].Init( NEAR_INFINITY, NULL );
        }
        // initialize distance to source to zero
        DijkstraNode* s = &(nodes[src->mIndex]);
        s->distToSrc = 0.0f;

        // determine best route to get from s to every other node
        VisitAll( nodes );

        // 
        // Populate routes:
        // For each node pointer "a" at index "i" in array "nodes", 
        //    follow predecessor pointers back to some node pointer "n" 
        //      adjacent to src node, and set routes[i].destIn = n->in
        //    set routes[i].dist = a->distToSrc
        //
        for( int j=0; j<numInts; j++ )
        {
            DijkstraNode* a = &(nodes[j]);
            rAssert( a->in->mIndex == j );
            // if this node is the source node...
            if( a == s )
            {
                (*routes)[j].destIn = NULL;
                (*routes)[j].roadToIn = NULL;
                (*routes)[j].roadJustBeforeIn = NULL;
            }
            else
            {
                SwapArray<ShortestRoad>* shortestRoads = NULL;

                // find the last road just before reaching destIn
                DijkstraNode* n = a;
                rAssert( n );
                rAssert( n->predecessor );

                shortestRoads = useMultiplier ?
                    &(n->predecessor->in->mShortestRoadsToAdjacentIntersectionsWithMultiplier) :
                    &(n->predecessor->in->mShortestRoadsToAdjacentIntersectionsNoMultiplier) ;

                ShortestRoad* roadToTake = NULL;
                for( int k=0; k<shortestRoads->mUseSize; k++ )
                {
                    ShortestRoad* candidate = &((*shortestRoads)[k]);
                    rAssert( candidate );

                    Intersection* candidateInt = NULL;
                    if( candidate->isOutRoad )
                    {
                        candidateInt = (Intersection*) 
                            candidate->road->GetDestinationIntersection();
                    }
                    else
                    {
                        candidateInt = (Intersection*) 
                            candidate->road->GetSourceIntersection();
                    }
                    if( candidateInt == n->in )
                    {
                        roadToTake = candidate;
                        break;
                    }
                }
                rAssert( roadToTake );
                (*routes)[j].roadJustBeforeIn = roadToTake;

                // find the road immediately leaving this big intersection
                // that will put us on the correct path towards destIn

                while( n->predecessor != s )
                {
                    n = n->predecessor;
                    rAssert( n != NULL );
                }
                rAssert( n->in != NULL );
                (*routes)[j].destIn = n->in;

                // find the road that will take us to dest intersection
                shortestRoads = useMultiplier ?
                    &(src->mShortestRoadsToAdjacentIntersectionsWithMultiplier) :
                    &(src->mShortestRoadsToAdjacentIntersectionsNoMultiplier) ;

                roadToTake = NULL;
                for( int k=0; k<shortestRoads->mUseSize; k++ )
                {
                    ShortestRoad* candidate = &((*shortestRoads)[k]);
                    rAssert( candidate );

                    Intersection* candidateInt = NULL;
                    if( candidate->isOutRoad )
                    {
                        candidateInt = (Intersection*) 
                            candidate->road->GetDestinationIntersection();
                    }
                    else
                    {
                        candidateInt = (Intersection*) 
                            candidate->road->GetSourceIntersection();
                    }
                    if( candidateInt == n->in )
                    {
                        roadToTake = candidate;
                        break;
                    }
                }
                rAssert( roadToTake );
                (*routes)[j].roadToIn = roadToTake;
            }
            (*routes)[j].dist = a->distToSrc;
        }
    }
    nodes.Clear();
}

void RoadManager::VisitAll( SwapArray<DijkstraNode>& nodes )
{
    int numInts = nodes.mUseSize;
    rAssert( numInts > 0 );

    ///////////////////
    // Dijkstra
    ///////////////////

    // Create "VS": a list of pointers to nodes that haven't been visited
    SwapArray<DijkstraNode*> VS;

    HeapMgr()->PushHeap(GMA_TEMP);
    VS.Allocate( numInts );
    HeapMgr()->PopHeap(GMA_TEMP);

    VS.mUseSize = numInts;
    int i;
    for( i=0; i<numInts; i++ )
    {
        VS[i] = &(nodes[i]);
    }

    // Create "S": a list of pointers to nodes that we have visited
    // which is initially empty
    SwapArray<DijkstraNode*> S;

    HeapMgr()->PushHeap(GMA_TEMP);
    S.Allocate( numInts );
    HeapMgr()->PopHeap(GMA_TEMP);

    // Greedy-extract cheapest node one by one from VS and put in S
    while( VS.mUseSize > 0 )
    {
        // Find the cheapest, remove from VS, put in S
        int cheapest = -1;
        float cheapestDist = NEAR_INFINITY;
        DijkstraNode* u = NULL;

        for( i=0; i<VS.mUseSize; i++ )
        {
            DijkstraNode* node = VS[i];
            if( node->distToSrc < cheapestDist )
            {
                u = node;
                cheapestDist = node->distToSrc;
                cheapest = i;
            }
        }
        rAssert( 0 <= cheapest && cheapest < VS.mUseSize );
        S.Add( u ); // Dijkstra giveth
        VS.Remove( cheapest ); // Dijkstra taketh away
        u->addedToS = true;

        // Relax the cost for all nodes adjacent to cheapestNode
        for( int i=0; i<u->adjacents.mUseSize; i++ )
        {
            DijkstraNode* v = u->adjacents[i].adjacentNode;
            rAssert( v );

            if( v->addedToS ) // skip v is already in S
            {
                continue;
            }

            /////////////////////////////////////////////////////////////
            // the cost is the length of the shortest road to the 
            // adjacent intersection PLUS the cost of traversal 
            // through intersection "u" from the road from our 
            // predecessor to this shortest road.

            float adjacentDist = u->adjacents[i].shortestRoadThere->cost;
            if( u->shortestRoadFromPred )
            {
                ShortestRoad* fromRoad = u->shortestRoadFromPred;
                ShortestRoad* toRoad = u->adjacents[i].shortestRoadThere;
                adjacentDist += GetTraversalDistance( fromRoad, toRoad );
            }

            if( v->distToSrc > (u->distToSrc + adjacentDist) )
            {
                v->distToSrc = u->distToSrc + adjacentDist;
                v->predecessor = u;
                v->shortestRoadFromPred = u->adjacents[i].shortestRoadThere;
            }
        }
    }

    VS.Clear();
    S.Clear();
}

float RoadManager::FindPathElementsBetween( 
    bool useMultiplier, 
    PathElement& sourceElem, 
    float sourceT,                  // used only if sourceElem is a road
    const rmt::Vector& sourcePos,   // used only if sourceElem is an intersection
    PathElement& targetElem, 
    float targetT,                  // used only if targetElem is a road
    const rmt::Vector& targetPos,   // used only if targetElem is an intersection
    SwapArray<PathElement>& elems ) // accumulate roads
{
    rAssert( sourceElem.elem != NULL );
    rAssert( targetElem.elem != NULL );
    rAssert( 0.0f <= sourceT && sourceT <= 1.0f );
    rAssert( elems.IsSetUp() );

    float totalDist = 0.0f;

    // for safety's sake... make this check...
    // we need to make sure that the last element's type was
    // not the same as sourceElem's type... this maintains the nice
    // "...-int-road-int-road-int-..." series
    if( elems.mUseSize > 0 )
    {
        rAssert( elems[ elems.mUseSize-1 ].type != sourceElem.type );
        if( elems[ elems.mUseSize-1 ].type == sourceElem.type )
        {
            return totalDist;
        }
    }

    // 
    // We will be returning a swaparray of PathElements that lie between
    // the source and the target PathElements. A PathElement can be a
    // shortest road or an intersection.
    // 

    // the first thing we add should be the source road!
    if( sourceElem.type == ET_NORMALROAD )
    {
        elems.Add( sourceElem );

        // if it's a shortcut, and target road isn't this road, 
        // call this function again with the destination intersection.. 
        Road* srcRd = (Road*) sourceElem.elem;
        if( srcRd->GetShortCut() && sourceElem != targetElem )
        {
            Intersection* srcInt = (Intersection*) srcRd->GetDestinationIntersection();

            rmt::Vector intPos;
            srcInt->GetLocation( intPos );

            float distToDestInt = (1.0f - sourceT) * srcRd->GetRoadLength();

            rmt::Vector vec1,vec2,endOfRdPos;
            RoadSegment* endOfRdSeg = srcRd->GetRoadSegment( srcRd->GetNumRoadSegments()-1 );
            endOfRdSeg->GetCorner( 1, vec1 );
            endOfRdSeg->GetCorner( 2, vec2 );
            endOfRdPos = (vec1+vec2) * 0.5f;

            distToDestInt += (endOfRdPos - intPos).Magnitude(); // *** SQUARE ROOT! *** 
            

            /*
            // TODO:
            // This is a big problem. We're CHANGING sourceElem here... which
            // changes the value for whoever passed in sourceElem (cuz sourceElem
            // was passed in by reference .. BIATCH!)... Somehow things have been 
            // miraculously working, I'm not sure why.. Basically the only case
            // where this bug applies is when the somebody (e.g. the avater, the AI,
            // the HUD map, light path, etc.) is on a shortcut and
            // he's pathfinding to some target. The sourceElem will be set to the
            // destination intersection of that shortcut, so the Avatar and Ai's
            // mLastPathElement members will be inaccurate... Aiya!
            // Too late to change this at this point. Just leave it unless the
            // problems are OVERWHELMINGLY bad...
            // 
            PathElement tmpElem;
            tmpElem.type = ET_INTERSECTION;
            tmpElem.elem = srcInt; 
            */
            sourceElem.type = ET_INTERSECTION;
            sourceElem.elem = srcInt; 

            SwapArray<PathElement> tmpElems;

            HeapMgr()->PushHeap( GMA_TEMP );
            tmpElems.Allocate( elems.mSize );
            HeapMgr()->PopHeap( GMA_TEMP );

            totalDist += distToDestInt + FindPathElementsBetween( useMultiplier,
                sourceElem, 0.0f, intPos, targetElem, targetT, targetPos, tmpElems );

            for( int i=0; i<tmpElems.mUseSize; i++ )
            {
                elems.Add( tmpElems[i] );
            }
            tmpElems.Clear();

            return totalDist;
        }
    }


    // if source and target are the same, then return dist between them!
    if( sourceElem == targetElem  )
    {
        // if road, use T values to compute dist betw them
        if( sourceElem.type == ET_NORMALROAD )
        {
            // TODO:
            // Can't just happily return here... Not right to just
            // compute the totalDist this way either... 
            // What if it's closer to go the other way (i.e. not along the
            // given road, but via another road)? Hmm??
            // Careful... Fixing this might change behavior below 
            // where some asserts assume that we have returned at this point...
            // 
            // Here's the problem in detail:
            // 
            // One of the other nuances with the pathfinding algo was quite 
            // evident in SuperSprint before we hacked the data to never produce 
            // the problem.. The bug still exists in code, however, but I haven't
            // really seen it expressed anywhere else in the world.
            //
            // The algorithm "tries to be smart" and says, "Hey, if S is on the 
            // same road as T, then the distance between them must be just the 
            // distance along the road between S and T"... This is not necessarily 
            // the case.
            //
            // For example, take a particularly long road, with S and T near 
            // either ends of the road... But the road's either ends are connected 
            // via another road...like so (A and B are intersections):
            //
            //  -<--S--A--<--B--T--<- 
		    // |    		 	     |
		    // |	    		     |
		    //  ---->---------->-----
            // 
            // In this case, it's actually closer to get from S to T via 
            // intersection A, then to B, then to T... but the algorithm returns
            // the closest distance to be along the road from S to T
            // 

            Road* someRoad = (Road*)sourceElem.elem;
            float someRoadLen = someRoad->GetRoadLength();
            float sourceProgress = someRoadLen * sourceT;
            float targetProgress = someRoadLen * targetT;
            totalDist += rmt::Fabs(sourceProgress - targetProgress);

            return totalDist;
        }
        else // both in same intersection
        {
            totalDist += (sourcePos - targetPos).Length(); // *** SQUARE ROOT! ***
            elems.Add( targetElem );
            return totalDist;
        }
    }
    else if( sourceElem.type == ET_NORMALROAD && targetElem.type == ET_NORMALROAD )
    {
        // figure out if the source is actually a road on the opposite lane.. 
        // if so, ignore it if we're still close enough to the previous segment.
        // This is to fix the problem where you're on the same physical road as
        // your target, but you're on the incoming road and it's on the outgoing
        // road, so you're actually doing unnecessary pathfinding around your
        // own road to get to the correct road... stupid...
        // 
        // Example:
        //      <--------------- you -----------------------------------
        //      -------AI ---------------------------- checkpoint ----->
        //
        // Here you are physically closer, but you're ranked second, behind the AI
        // because you have to take extra pathfinding steps around your own road, 
        // to reach either intersections before getting on the correct road. Blegh.
        //

        Road* srcRoad = (Road*)sourceElem.elem;
        Road* targRoad = (Road*)targetElem.elem;

        rAssert( srcRoad != targRoad );


        const Intersection* srcSrcInt = srcRoad->GetSourceIntersection();
        const Intersection* srcDestInt = srcRoad->GetDestinationIntersection();
        const Intersection* targSrcInt = targRoad->GetSourceIntersection();
        const Intersection* targDestInt = targRoad->GetDestinationIntersection();

        if( srcSrcInt == targDestInt && srcDestInt == targSrcInt )
        {
            // whoa, these roads are connecting the same intersections! 
            // figure out how far we really are from last road... 

            rmt::Vector closestPtOnSrc;
            float distFromClosestPtOnSrcSqr = NEAR_INFINITY;
            int closestSrcSegIndex = -1;

            FindClosestPointOnRoad( srcRoad, sourcePos, closestPtOnSrc, 
                distFromClosestPtOnSrcSqr, closestSrcSegIndex );

            rmt::Vector closestPtOnTarg;
            float distFromClosestPtOnTargSqr = NEAR_INFINITY;
            int closestTargSegIndex = -1;

            FindClosestPointOnRoad( targRoad, closestPtOnSrc, closestPtOnTarg, 
                distFromClosestPtOnTargSqr, closestTargSegIndex );
            /*
            FindClosestPointOnRoad( targRoad, sourcePos, closestPtOnTarg, 
                distFromClosestPtOnTargSqr, closestTargSegIndex );
            */

            rAssert( closestTargSegIndex != -1 );

            // if within 10 meters from road center, source and target roads 
            // must be part of the same road... 
            const float CLOSE_ENOUGH_TO_BE_ADJACENT_ROAD_SQR = 100.0f; 

            if( distFromClosestPtOnTargSqr < CLOSE_ENOUGH_TO_BE_ADJACENT_ROAD_SQR )
            {
                // TODO:
                // Can't just happily return here... Not right to just
                // compute the totalDist this way either... 
                // What if it's closer to go the other way (i.e. not along the
                // given road, but via another road)? Hmm??
                // Careful... Fixing this might change behavior below 
                // where some asserts assume that we have returned at this point...
                // 
                // Here's the problem in detail:
                // 
                // One of the other nuances with the pathfinding algo was quite 
                // evident in SuperSprint before we hacked the data to never produce 
                // the problem.. The bug still exists in code, however, but I haven't
                // really seen it expressed anywhere else in the world.
                //
                // The algorithm "tries to be smart" and says, "Hey, if S is on the 
                // same road as T, then the distance between them must be just the 
                // distance along the road between S and T"... This is not necessarily 
                // the case.
                //
                // For example, take a particularly long road, with S and T near 
                // either ends of the road... But the road's either ends are connected 
                // via another road...like so (A and B are intersections):
                //
                //  -<--S--A--<--B--T--<- 
		        // |    		 	     |
		        // |	    		     |
		        //  ---->---------->-----
                // 
                // In this case, it's actually closer to get from S to T via 
                // intersection A, then to B, then to T... but the algorithm returns
                // the closest distance to be along the road from S to T
                //
                RoadSegment* closestTargSeg = (RoadSegment*) targRoad->GetRoadSegment( 
                    (unsigned int)closestTargSegIndex );

                float closestTargSegT = RoadManager::DetermineSegmentT( closestPtOnTarg, closestTargSeg );
                float closestTargRoadT = RoadManager::DetermineRoadT( closestTargSeg, closestTargSegT );

                float targRoadLen = targRoad->GetRoadLength();
                float sourceProgress = targRoadLen * closestTargRoadT;
                float targetProgress = targRoadLen * targetT;
                totalDist += rmt::Fabs(sourceProgress - targetProgress);

                elems.Remove( elems.mUseSize - 1 );
                elems.Add( targetElem );

                return totalDist;
            }
        }
    }


            

    // determine what we know about the target
    float distFromTargetToInt = 0.0f;
    float distFromTargetToOtherInt = 0.0f;
    Intersection* targetInt = NULL;  // the one we're heading towards 
    Intersection* targetOtherInt = NULL; // if targetElem is a road, it's the other intersection
    ShortestRoad targetShortRoad;
    targetShortRoad.cost = NEAR_INFINITY;

    if( targetElem.type == ET_INTERSECTION )
    {
        targetInt = (Intersection*) targetElem.elem;
        targetShortRoad.road = NULL;
    }
    else
    {
        rAssert( targetElem.type == ET_NORMALROAD );

        Road* targetRoad = (Road*) targetElem.elem;
        targetShortRoad.road = targetRoad;

        targetInt = (Intersection*) targetRoad->GetSourceIntersection();
        distFromTargetToInt = targetT * targetRoad->GetRoadLength();

        if( !targetRoad->GetShortCut() )
        {
            targetOtherInt = (Intersection*) targetRoad->GetDestinationIntersection();
            // if we're getting to target via dest int, we're going against traffic
            // so use the multiplier 
            distFromTargetToOtherInt = (1.0f - targetT) * targetRoad->GetRoadLength() *
                ((useMultiplier)? AGAINST_TRAFFIC_COST_MULTIPLIER : 1.0f);
        }
        else
        {
            targetOtherInt = NULL;
            distFromTargetToOtherInt = NEAR_INFINITY;
        }


    }
    // at least targetInt should exist...
    rAssert( targetInt );

    bool goingToTargetOther = false;

    ShortestRoad* firstShortRoad = NULL;
    ShortestRoad* lastShortRoad = NULL;
    //////////////////////////////////////////////////////////////////////////////////


    // deal with the simplest case first...
    bool foundSimpleCase = false;
    if( sourceElem.type == ET_INTERSECTION )
    {
        // if I'm a big intersection or if I only got one way to go...
        // just call TraverseRoads...
        Intersection* sourceInt = (Intersection*) sourceElem.elem;
        SwapArray<ShortestRoad>* shortestRoads = useMultiplier ?
            &(sourceInt->mShortestRoadsToAdjacentIntersectionsWithMultiplier) :
            &(sourceInt->mShortestRoadsToAdjacentIntersectionsNoMultiplier) ;

        if( sourceInt->mBigIntersection || shortestRoads->mUseSize == 1 )
        {
            foundSimpleCase = true;
    
            SwapArray<PathElement> elemsToTargetInt, elemsToTargetOtherInt;

            HeapMgr()->PushHeap(GMA_TEMP);
            elemsToTargetInt.Allocate( elems.mSize );
            elemsToTargetOtherInt.Allocate( elems.mSize );
            HeapMgr()->PopHeap(GMA_TEMP);

            ErrorValue errVal;

            float distViaTargetInt = NEAR_INFINITY;
            float distViaTargetOtherInt = NEAR_INFINITY;

            firstShortRoad = NULL;
            lastShortRoad = NULL;

            // PATH: sourceInt -> ... -> targetRoad's srcInt -> targetRoad
            distViaTargetInt = FindDistToTargetInOneDirection( useMultiplier,
                targetInt, sourceInt, sourceInt, NULL, elemsToTargetInt, 
                firstShortRoad, lastShortRoad, errVal ) + 
                distFromTargetToInt;
            rAssert( errVal == FOUND_BIGINTERSECTION || errVal == FOUND_TARGET );
            if( targetElem.type == ET_NORMALROAD ) 
            {
                if( lastShortRoad )
                {
                    targetShortRoad.isOutRoad = true; // at target road's source intersection, the target road is an OUT road
                    distViaTargetInt += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                }
            }
            
            if( targetOtherInt )
            {
                firstShortRoad = NULL;
                lastShortRoad = NULL;

                // PATH: sourceInt -> ... -> targetRoad's destInt -> targetRoad
                distViaTargetOtherInt = FindDistToTargetInOneDirection( useMultiplier,
                    targetOtherInt, sourceInt, sourceInt, NULL, elemsToTargetOtherInt, 
                    firstShortRoad, lastShortRoad, errVal ) +
                    distFromTargetToOtherInt;
                rAssert( errVal == FOUND_BIGINTERSECTION || errVal == FOUND_TARGET );
                if( targetElem.type == ET_NORMALROAD ) 
                {
                    if( lastShortRoad )
                    {
                        targetShortRoad.isOutRoad = false; // at target road's dest intersection, the target road is an OUT road
                        distViaTargetOtherInt += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                    }
                }
                
            }

            if( distViaTargetInt < distViaTargetOtherInt )
            {
                // don't need to copy over the roads, we're visiting sourceInt
                // again
                TraverseRoads( useMultiplier, targetInt, sourceInt, sourceInt, elems, errVal );
                totalDist += distViaTargetInt;
            }
            else
            {
                rAssert( targetOtherInt );
                // don't need to copy over the roads, we're visiting sourceInt
                // again
                goingToTargetOther = true;
                TraverseRoads( useMultiplier, targetOtherInt, sourceInt, sourceInt, elems, errVal );
                totalDist += distViaTargetOtherInt;
            }
            rAssert( errVal == FOUND_TARGET );
        }
    }

    // 
    // Well we either got source being a road or a small intersection with 
    // 2 adjacent neighbors. Either way, we have to traverse in both directions...
    //
    if( !foundSimpleCase )
    {
        float distToTargetIntInOneDir = NEAR_INFINITY;
        float distToTargetIntInOtherDir = NEAR_INFINITY;
        float distToTargetOtherIntInOneDir = NEAR_INFINITY;
        float distToTargetOtherIntInOtherDir = NEAR_INFINITY;

        SwapArray<PathElement> elemsToTargetIntInOneDir;
        SwapArray<PathElement> elemsToTargetIntInOtherDir;

        HeapMgr()->PushHeap(GMA_TEMP);
        elemsToTargetIntInOneDir.Allocate( elems.mSize );
        elemsToTargetIntInOtherDir.Allocate( elems.mSize );
        HeapMgr()->PopHeap(GMA_TEMP);

        SwapArray<PathElement> elemsToTargetOtherIntInOneDir;
        SwapArray<PathElement> elemsToTargetOtherIntInOtherDir;
        if( targetOtherInt )
        {
            HeapMgr()->PushHeap(GMA_TEMP);
            elemsToTargetOtherIntInOneDir.Allocate( elems.mSize );
            elemsToTargetOtherIntInOtherDir.Allocate( elems.mSize );
            HeapMgr()->PopHeap(GMA_TEMP);
        }
        ErrorValue targetIntOneErr = DEAD_END;
        ErrorValue targetIntOtherErr = DEAD_END;
        ErrorValue targetOtherIntOneErr = DEAD_END;
        ErrorValue targetOtherIntOtherErr = DEAD_END;

        if( sourceElem.type == ET_NORMALROAD )
        {
            // Explore in both directions along our source road:
            //  - head toward the source intersection and keep going
            //  - head toward the dest intersection and keep going
            //
            Road* sourceRoad = (Road*) sourceElem.elem;

            float distToSrcInt, distToDestInt;
            distToSrcInt = sourceT * sourceRoad->GetRoadLength() *
                ((useMultiplier)? AGAINST_TRAFFIC_COST_MULTIPLIER : 1.0f);
            distToDestInt = (1.0f - sourceT) * sourceRoad->GetRoadLength();

            Intersection* sourceSrcInt = (Intersection*) sourceRoad->GetSourceIntersection();
            Intersection* sourceDestInt = (Intersection*) sourceRoad->GetDestinationIntersection();

            ShortestRoad sourceShortRoad;
            sourceShortRoad.road = sourceRoad;
            sourceShortRoad.cost = NEAR_INFINITY;

            if( !sourceRoad->GetShortCut() ) // shortcuts only go one way!!
            {
                firstShortRoad = NULL;
                lastShortRoad = &sourceShortRoad;

                // PATH: sourceRoad -> sourceRoad's srcInt -> ... -> targetRoad's srcInt -> targetRoad
                sourceShortRoad.isOutRoad = false;
                distToTargetIntInOneDir = FindDistToTargetInOneDirection( useMultiplier,
                    targetInt, sourceSrcInt, sourceDestInt, NULL, elemsToTargetIntInOneDir, 
                    firstShortRoad, lastShortRoad, targetIntOneErr ) + distToSrcInt + distFromTargetToInt;
                if( firstShortRoad && targetIntOneErr != DEAD_END )
                {
                    distToTargetIntInOneDir += GetTraversalDistance( &sourceShortRoad, firstShortRoad );
                }
                if( targetElem.type == ET_NORMALROAD && targetIntOneErr != DEAD_END ) 
                {
                    rAssert( lastShortRoad );
                    targetShortRoad.isOutRoad = true; 
                    distToTargetIntInOneDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                }

            }

            firstShortRoad = NULL;
            lastShortRoad = &sourceShortRoad;

            // PATH: sourceRoad -> sourceRoad's destInt -> ... -> targetRoad's srcInt -> targetRoad
            sourceShortRoad.isOutRoad = true;
            distToTargetIntInOtherDir = FindDistToTargetInOneDirection( useMultiplier,
                targetInt, sourceDestInt, sourceSrcInt, NULL, elemsToTargetIntInOtherDir, 
                firstShortRoad, lastShortRoad, targetIntOtherErr ) + distToDestInt + distFromTargetToInt;
            if( firstShortRoad && targetIntOtherErr != DEAD_END )
            {
                distToTargetIntInOtherDir += GetTraversalDistance( &sourceShortRoad, firstShortRoad );
            }
            if( targetElem.type == ET_NORMALROAD && targetIntOtherErr != DEAD_END ) 
            {
                rAssert( lastShortRoad );
                targetShortRoad.isOutRoad = true; 
                distToTargetIntInOtherDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
            }

            if( targetOtherInt )
            {
                if( !sourceRoad->GetShortCut() ) // shortcuts only go one way!!
                {
                    firstShortRoad = NULL;
                    lastShortRoad = &sourceShortRoad;

                    // PATH: sourceRoad -> sourceRoad's srcInt -> ... -> targetRoad's destInt -> targetRoad
                    sourceShortRoad.isOutRoad = false;
                    distToTargetOtherIntInOneDir = FindDistToTargetInOneDirection( useMultiplier,
                        targetOtherInt, sourceSrcInt, sourceDestInt, NULL, elemsToTargetOtherIntInOneDir, 
                        firstShortRoad, lastShortRoad, targetOtherIntOneErr ) + 
                        distToSrcInt + distFromTargetToOtherInt;
                    if( firstShortRoad && targetOtherIntOneErr != DEAD_END )
                    {
                        distToTargetOtherIntInOneDir += GetTraversalDistance( &sourceShortRoad, firstShortRoad );
                    }
                    if( targetElem.type == ET_NORMALROAD && targetOtherIntOneErr != DEAD_END ) 
                    {
                        rAssert( lastShortRoad );
                        targetShortRoad.isOutRoad = false; 
                        distToTargetOtherIntInOneDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                    }
                }

                firstShortRoad = NULL;
                lastShortRoad = &sourceShortRoad;

                // PATH: sourceRoad -> sourceRoad's destInt -> ... -> targetRoad's destInt -> targetRoad
                sourceShortRoad.isOutRoad = true;
                distToTargetOtherIntInOtherDir = FindDistToTargetInOneDirection( useMultiplier,
                    targetOtherInt, sourceDestInt, sourceSrcInt, NULL, elemsToTargetOtherIntInOtherDir, 
                    firstShortRoad, lastShortRoad, targetOtherIntOtherErr ) + 
                    distToDestInt + distFromTargetToOtherInt;
                if( firstShortRoad && targetOtherIntOtherErr != DEAD_END )
                {
                    distToTargetOtherIntInOtherDir += GetTraversalDistance( &sourceShortRoad, firstShortRoad );
                }
                if( targetElem.type == ET_NORMALROAD && targetOtherIntOtherErr != DEAD_END ) 
                {
                    rAssert( lastShortRoad );
                    targetShortRoad.isOutRoad = false; 
                    distToTargetOtherIntInOtherDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                }
            }
        }
        else
        {
            // I'm a small intersection with 2 adjacent neighbor intersections
            // Visit both intersections on either sides of me...
            rAssert( sourceElem.type == ET_INTERSECTION );

            Intersection* sourceInt = (Intersection*) sourceElem.elem;
            SwapArray<ShortestRoad>* shortestRoads = useMultiplier ?
                &(sourceInt->mShortestRoadsToAdjacentIntersectionsWithMultiplier) :
                &(sourceInt->mShortestRoadsToAdjacentIntersectionsNoMultiplier) ;

            rAssert( shortestRoads->mUseSize == 2 );

            // visit neighborB by feeding neighborA in as last intersection
            Intersection* neighborA = NULL;
            ShortestRoad* shortRoad = &((*shortestRoads)[0]);
            if( shortRoad->isOutRoad )
            {
                neighborA = (Intersection*) shortRoad->road->GetDestinationIntersection();
            }
            else
            {
                neighborA = (Intersection*) shortRoad->road->GetSourceIntersection();
            }

            // now visit neighborA by feeding neighborB in as last intersection
            Intersection* neighborB = NULL;
            sourceInt->GetOtherIntersection( useMultiplier, neighborA, neighborB, shortRoad );

            firstShortRoad = NULL;
            lastShortRoad = NULL;

            // PATH: sourceInt -> neighborB -> ... -> targetRoad's srcInt -> targetRoad
            distToTargetIntInOneDir = FindDistToTargetInOneDirection( useMultiplier,
                targetInt, sourceInt, neighborA, NULL, elemsToTargetIntInOneDir,
                firstShortRoad, lastShortRoad, targetIntOneErr ) + distFromTargetToInt;
            if( targetElem.type == ET_NORMALROAD && targetIntOneErr != DEAD_END ) 
            {
                if( lastShortRoad )
                {
                    targetShortRoad.isOutRoad = true; 
                    distToTargetIntInOneDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                }
            }


            firstShortRoad = NULL;
            lastShortRoad = NULL;

            // PATH: sourceInt -> neighborA -> ... -> targetRoad's srcInt -> targetRoad
            distToTargetIntInOtherDir = FindDistToTargetInOneDirection( useMultiplier,
                targetInt, sourceInt, neighborB, NULL, elemsToTargetIntInOtherDir, 
                firstShortRoad, lastShortRoad, targetIntOtherErr ) + distFromTargetToInt;
            if( targetElem.type == ET_NORMALROAD && targetIntOtherErr != DEAD_END ) 
            {
                if( lastShortRoad )
                {
                    targetShortRoad.isOutRoad = true; 
                    distToTargetIntInOtherDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                }
            }


            // now do the other 2 alternatives...
            if( targetOtherInt )
            {
                firstShortRoad = NULL;
                lastShortRoad = NULL;

                // PATH: sourceInt -> neighborB -> ... -> targetRoad's destInt -> targetRoad
                distToTargetOtherIntInOneDir = FindDistToTargetInOneDirection( useMultiplier,
                    targetOtherInt, sourceInt, neighborA, NULL, elemsToTargetOtherIntInOneDir, 
                    firstShortRoad, lastShortRoad, targetOtherIntOneErr ) + distFromTargetToOtherInt;
                if( targetElem.type == ET_NORMALROAD && targetOtherIntOneErr != DEAD_END ) 
                {
                    if( lastShortRoad )
                    {
                        targetShortRoad.isOutRoad = false; 
                        distToTargetOtherIntInOneDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                    }
                }


                firstShortRoad = NULL;
                lastShortRoad = NULL;

                // PATH: sourceInt -> neighborA -> ... -> targetRoad's destInt -> targetRoad
                distToTargetOtherIntInOtherDir = FindDistToTargetInOneDirection( useMultiplier,
                    targetOtherInt, sourceInt, neighborB, NULL, elemsToTargetOtherIntInOtherDir, 
                    firstShortRoad, lastShortRoad, targetOtherIntOtherErr ) + distFromTargetToOtherInt;
                if( targetElem.type == ET_NORMALROAD && targetOtherIntOtherErr != DEAD_END ) 
                {
                    if( lastShortRoad )
                    {
                        targetShortRoad.isOutRoad = false; 
                        distToTargetOtherIntInOtherDir += GetTraversalDistance( lastShortRoad, &targetShortRoad );
                    }
                }

            }
        }

        // 
        // One or all of these ways should make it to either a big intersection 
        // (where we decide whether or not the total cost to the target road
        // is less in this direction than if we were to go in the other
        // directions) or the target road itself (where we consider total dist
        // to the target in this dir versus total dist to target in other dirs)
        //
        // shouldn't have found dead end in all directions!
        rAssert( targetIntOneErr != DEAD_END || 
                 targetIntOtherErr != DEAD_END  ||
                 targetOtherIntOneErr != DEAD_END ||
                 targetOtherIntOtherErr != DEAD_END );

        SwapArray<DistErrMap> mappings;

        HeapMgr()->PushHeap(GMA_TEMP);
        mappings.Allocate( 4 );
        HeapMgr()->PopHeap(GMA_TEMP);

        mappings.mUseSize = 4;

        mappings[0].dist = distToTargetIntInOneDir;
        mappings[0].errVal = targetIntOneErr;
        mappings[0].ID = 0;

        mappings[1].dist = distToTargetIntInOtherDir;
        mappings[1].errVal = targetIntOtherErr;
        mappings[1].ID = 1;

        mappings[2].dist = distToTargetOtherIntInOneDir;
        mappings[2].errVal = targetOtherIntOneErr;
        mappings[2].ID = 2;

        mappings[3].dist = distToTargetOtherIntInOtherDir;
        mappings[3].errVal = targetOtherIntOtherErr;
        mappings[3].ID = 3;

        int i;
        // get rid of all the ones that never panned out
        for( i=0; i<mappings.mUseSize; )
        {
            rAssert( mappings[i].errVal != STILL_LOOKING &&
                     mappings[i].errVal != UNEXPECTED );

            if( mappings[i].errVal == DEAD_END )
            {
                mappings.Remove(i);
            }
            else
            {
                i++;
            }
        }
        rAssert( mappings.mUseSize >= 1 );

        // of whatever's left, we find the smallest dist...
        int minIndex = -1;
        float minDist = NEAR_INFINITY;
        for( i=0; i<mappings.mUseSize; i++ )
        {
            if( mappings[i].dist < minDist )
            {
                minDist = mappings[i].dist;
                minIndex = i;
            }
        }
        rAssert( minIndex != -1 );
        rAssert( minDist < NEAR_INFINITY );

        ErrorValue errValToUse = mappings[minIndex].errVal;

        totalDist += mappings[minIndex].dist;

        switch( mappings[minIndex].ID )
        {
        case 0: 
            {
                // copy over the roads
                for( int i=0; i<elemsToTargetIntInOneDir.mUseSize; i++ )
                {
                    elems.Add( elemsToTargetIntInOneDir[i] );
                }
            }
            break;
        case 1:
            {
                // copy over the roads
                for( int i=0; i<elemsToTargetIntInOtherDir.mUseSize; i++ )
                {
                    elems.Add( elemsToTargetIntInOtherDir[i] );
                }
            }
            break;
        case 2:
            {
                // copy over the roads
                rAssert( elemsToTargetOtherIntInOneDir.IsSetUp() );
                for( int i=0; i<elemsToTargetOtherIntInOneDir.mUseSize; i++ )
                {
                    elems.Add( elemsToTargetOtherIntInOneDir[i] );
                }
                goingToTargetOther = true;
            }
            break;
        case 3:
            {
                // copy over the roads
                rAssert( elemsToTargetOtherIntInOtherDir.IsSetUp() );
                for( int i=0; i<elemsToTargetOtherIntInOtherDir.mUseSize; i++ )
                {
                    elems.Add( elemsToTargetOtherIntInOtherDir[i] );
                }
                goingToTargetOther = true;
            }
            break;
        default:
            {
                rAssert( false );
            }
            break;
        }

        // clean out the temporary structures
        elemsToTargetIntInOneDir.Clear();
        elemsToTargetIntInOtherDir.Clear();
        elemsToTargetOtherIntInOneDir.Clear();
        elemsToTargetOtherIntInOtherDir.Clear();

        // our choice in the error value to use (implies choice in direction of search)
        // will take us to either the target or a big intersection, along the shortest 
        // path to the target
        switch( errValToUse )
        {
        case FOUND_TARGET:
            {
                // We found targetInt before running into a big intersection.
                // Just move on...
            }
            break;
        case FOUND_BIGINTERSECTION:
            {
                rAssert( elems[elems.mUseSize-1].type == ET_INTERSECTION );

                Intersection* bigInt = (Intersection*) elems[elems.mUseSize-1].elem;
                rAssert( bigInt->mBigIntersection );
                
                // pop it off the array, since TraverseRoads will add it again
                // on first visit
                elems.Remove( elems.mUseSize-1 );

                // call TraverseRoads on it to fill the elems list with path elements
                ErrorValue returnErr;
                if( goingToTargetOther )
                {
                    rAssert( targetOtherInt );
                    TraverseRoads( useMultiplier, targetOtherInt, bigInt, bigInt, elems, returnErr );
                }
                else
                {
                    TraverseRoads( useMultiplier, targetInt, bigInt, bigInt, elems, returnErr );
                }
                rAssert( returnErr == FOUND_TARGET );
            }
            break;
        case DEAD_END: // fall thru... the errVal we're using shouldn't lead to dead end
        case STILL_LOOKING: // fall thru... only a tmp state for use before reaching end condition!
        case UNEXPECTED: // fall thru
        default:
            {
                // Shouldn't be here. We should have found either the target 
                // intersection (no more path finding necessary), or a big 
                // intersection.
                rAssert( false );
            }
            break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    //
    // Deal with adding the target element, if necessary...
    // 
    // Given that source and target are not identical, at the very least, 
    // sourceElem (first thing we added) and targetInt (the last thing 
    // we should have found before getting to this point) exist in the list.
    // But they could be the same element!
    // 
#ifdef RAD_DEBUG
    rAssert( elems.mUseSize >= 1 ); 
    rAssert( elems[0] == sourceElem );
    rAssert( elems[elems.mUseSize-1].type == ET_INTERSECTION );
    if( goingToTargetOther )
    {
        rAssert( targetOtherInt );
        rAssert( ((Intersection*)elems[elems.mUseSize-1].elem) == targetOtherInt );
    }
    else
    {
        rAssert( ((Intersection*)elems[elems.mUseSize-1].elem) == targetInt );
    }
#endif


    // If targetElem is an intersection, then we're already done, because
    // targetInt is already the last element in the list
    if( targetElem.type == ET_INTERSECTION )
    {
        // Because of our earlier RETURN for when src == target, we know: 
        // - we have at least 2 elems, targetElem (this intersection) and source
        rAssert( elems.mUseSize >= 2 ); 
        rAssert( targetElem == elems[elems.mUseSize-1] );
        rAssert( elems[elems.mUseSize-1].type == ET_INTERSECTION );
        rAssert( elems[elems.mUseSize-2].type == ET_NORMALROAD ); 

        // the intersection 
        Intersection* targetElemInt = (Intersection*)targetElem.elem;

        // add dist from endpoint of last road to targetPos
        rmt::Vector lastRoadEndPos, vec0, vec1, vec2, vec3;
        Road* lastRoad = (Road*)elems[elems.mUseSize-2].elem;
        RoadSegment* lastRoadSeg = NULL;
        if( lastRoad->GetDestinationIntersection() == targetElemInt )
        {
            lastRoadSeg = lastRoad->GetRoadSegment( lastRoad->GetNumRoadSegments()-1 );

            lastRoadSeg->GetCorner( 1, vec1 );
            lastRoadSeg->GetCorner( 2, vec2 );
            lastRoadEndPos = (vec1 + vec2) * 0.5f;
        }
        else
        {
            rAssert( lastRoad->GetSourceIntersection() == targetElemInt );

            lastRoadSeg = lastRoad->GetRoadSegment( 0 );

            lastRoadSeg->GetCorner( 0, vec0 );
            lastRoadSeg->GetCorner( 3, vec3 );
            lastRoadEndPos = (vec0 + vec3) * 0.5f;
        }
        totalDist += (lastRoadEndPos - targetPos).Magnitude(); // *** SQUARE ROOT! ***
    }
    else
    {
        // ok, target is a road... gotta add it
    #ifdef RAD_DEBUG
        rAssert( targetElem.type == ET_NORMALROAD );
        if( ((Road*)(targetElem.elem))->GetShortCut() )
        {
            // we don't consider approaching target from its destination intersection
            // when the target is on a shortcut road because a shortcut is one-way.
            rAssert( targetOtherInt == NULL ); 
        }
        else
        {
            // if not a shortcut, of course we have to consider approaching the 
            // target from the target road's destination intersection
            rAssert( targetOtherInt );
        }
    #endif

        bool needToAddTargetRoad = true;
        // well do some more asserting...
        if( elems.mUseSize > 1 )
        {
            rAssert( elems[elems.mUseSize-2].type == ET_NORMALROAD );

            rAssert( elems[elems.mUseSize-2] != targetElem );
            /*
            if( elems[elems.mUseSize-2] == targetElem )
            {
                // we already added the target road!
                
                // TODO:
                // Subtract away the added cost of traversing the target
                // road... Shouldn't have to do any more adding to totalDist 
                // since we already added the traversal cost in the process
                // of getting to the target road, and we already added the 
                // cost of the target's actual distance from the intersection
                totalDist -= elems[elems.mUseSize-2].cost;

                // Now just pop the redundant target intersection 
                elems.Remove( elems.mUseSize-1 );

                needToAddTargetRoad = false;
            }
            */

        }

        if( needToAddTargetRoad )
        {
            // add the target road
            elems.Add( targetElem );
        }
    }



    // If source element is an intersection, we have to take into account
    // the dist from sourcePos to the first road element...
    if( sourceElem.type == ET_INTERSECTION )
    {
        // if source != target, source is an intersection, and target has been added, 
        // then we have at least 2 elems
        rAssert( elems.mUseSize >= 2 );
        rAssert( elems[1].type == ET_NORMALROAD );

        Intersection* sourceElemInt = (Intersection*) sourceElem.elem;

        // now add the dist from source to the second element (which should be a road)
        Road* firstRoad = (Road*) elems[1].elem;

        rmt::Vector firstRoadEndPos, vec0, vec1, vec2, vec3;
        RoadSegment* firstRoadSeg = NULL;
        if( firstRoad->GetDestinationIntersection() == sourceElemInt )
        {
            firstRoadSeg = firstRoad->GetRoadSegment( firstRoad->GetNumRoadSegments()-1 );

            firstRoadSeg->GetCorner( 1, vec1 );
            firstRoadSeg->GetCorner( 2, vec2 );
            firstRoadEndPos = (vec1 + vec2) * 0.5f;
        }
        else
        {
            rAssert( firstRoad->GetSourceIntersection() == sourceElemInt );

            firstRoadSeg = firstRoad->GetRoadSegment( 0 );

            firstRoadSeg->GetCorner( 0, vec0 );
            firstRoadSeg->GetCorner( 3, vec3 );
            firstRoadEndPos = (vec0 + vec3) * 0.5f;
        }
        totalDist += (firstRoadEndPos - sourcePos).Magnitude(); // *** SQUARE ROOT! ***
    }



    /* NOTE: The cases we postulated below never happen... Thank god...
    // 
    // Ok, damn.. targetElem is a road... 
    // we really want the last thing in the list to be targetRoad... but we
    // have to be careful that we haven't added it already in the process 
    // of getting to target int...
    // 
    // So if the second last elem in the list exists (had to have been a road)
    // and was targetRoad, then trim off the last elem (target Int)
    //
    // If it wasn't... then the road needs to be added to the list after
    // target Int...
    //

    // if only one item in there, then it was the sourceElem/target Int (they 
    // are one and the same), so add targetElem (which is an adjacent road)
    if( elems.mUseSize == 1 )
    {
        // add in the target 
        elems.Add( targetElem );

        // NOTE:
        // Don't need to augment totalDist here since the
        // distance from target to targetInt road has already 
        // been accounted for
    }
    else 
    {
        // the second last elem must be a road (only the sequence
        // "...-int-road-int-road-..." is allowed)
        rAssert( elems[elems.mUseSize-2].type == ET_NORMALROAD );

        rAssert( elems[elems.mUseSize-2] != targetElem );
          
        ////////////////////////////////////////////////////////////
        // NOTE: 
        // This case should NEVER happen! It means we chose the 
        // wrong target intersection to head to in our earlier
        // distance comparison. 
        //
        // If we already added the target elem in the process of
        // getting to the target Int, then pop the target Int
        // so the last thing in elems is targetElem
        if( elems[elems.mUseSize-2] == targetElem )
        {
            // Here, there should be at least 3 elems in our list, since target
            // is a road and source is not target and the last item added
            // was an intersection and we maintain the int-road-int 
            // sequence
            rAssert( elems.mUseSize >= 3 );

            // since we found targetelem to be our second last elem, 
            // and since we just pathfinded to targetInt (or targetOtherInt)
            // the third last should have been the targetOtherInt (or targetInt)
            float distToAdd = 0.0f;
            float distToSubtract = 0.0f;
            if( goingToTargetOther )
            {
                rAssert( (Intersection*)(elems[elems.mUseSize-3].elem) == targetInt );
                distToAdd = distFromTargetToInt;
                distToSubtract = distFromTargetToOtherInt;
            }
            else
            {
                rAssert( (Intersection*)(elems[elems.mUseSize-3].elem) == targetOtherInt );
                distToAdd = distFromTargetToOtherInt;
                distToSubtract = distFromTargetToInt;
            }

            // take out the unnecessary intersection
            elems.Remove( elems.mUseSize-1 );

            // Need to adjust totalDist...
            // So far totalDist has overcounted: 
            // A) distance from source to one target intersection a, 
            // B) plus the length of the target road element all the way to 
            //    the other target intersection b,
            // C) plus the distance from intersection b back to target roadT
            //
            // Now that we're removing the last element (the wrong target int)
            // we must take away B and C and add the dist from target roadT to 
            totalDist -= ((Road*)targetElem.elem)->GetRoadLength();
            totalDist -= distToSubtract;
            totalDist += distToAdd;
        }

        ////////////////////////////////////////////////////////////
        // NOTE: 
        // We dont' need to do this ELSE IF case at all!
        // Our algorithm already ensures that this case doesn't happen
        // where going along the longer target road is more beneficial.
        //
        // otherwise, check if the second last elem joins the same
        // two intersections (target Int and some third last 
        // intersection element "otherInt") as targetElem
        else if( elems.mUseSize >= 3 )
        {
            rAssert( elems[elems.mUseSize-3].type == ET_INTERSECTION );

            Intersection* intA = (Intersection*) elems[elems.mUseSize-3].elem;
            Intersection* intB = (Intersection*) elems[elems.mUseSize-1].elem;

            // if the road at elems[usesize-2] was performing the same function
            // as targetElem road in joining the same intersections,
            // then replace it with targetElem road...
            Intersection* testIntB = (goingToTargetOther)? targetInt : targetOtherInt;
            Intersection* testIntA = (goingToTargetOther)? targetOtherInt : targetInt;

            if( testIntB == intB && testIntA == intA )
            {
                // remove target Int
                elems.Remove( elems.mUseSize-1 ); 

                // remove the shortestroad before targetInt because
                // we'll be replacing it with targetRoad
                elems.Remove( elems.mUseSize-1 ); 
            }
            // add the target element
            elems.Add( targetElem );

            // we made changes recently that means we have to reexamine the logic
            // of this case should we have re-enabled this case.
            rAssert( false );
        }
        else 
        {
            // still need to add target
            elems.Add( targetElem );

            // NOTE:
            // Don't need to augment totalDist here since the
            // distance from target to targetInt road has already 
            // been accounted for
        }
    }
    */

#ifdef RAD_DEBUG
    // Do some checking
    rAssert( elems.mUseSize >= 1 );
    rAssert( elems[0].elem == sourceElem.elem );
    rAssert( elems[elems.mUseSize-1].elem == targetElem.elem );
    for( int i=1; i<elems.mUseSize; i++ )
    {
        PathElement* lastElem = &(elems[i-1]);
        PathElement* currElem = &(elems[i]);

        Intersection* in = NULL;
        Road* rd = NULL;

        if( currElem->type == ET_INTERSECTION )
        {
            rAssert( lastElem->type == RoadManager::ET_NORMALROAD );

            in = (Intersection*) currElem->elem;
            rd = (Road*) lastElem->elem;
        }
        else if( currElem->type == ET_NORMALROAD )
        {
            rAssert( lastElem->type == ET_INTERSECTION );
            
            in = (Intersection*) lastElem->elem;
            rd = (Road*) currElem->elem;
        }

        // find currRoad in lastElem
        bool found = false;
        unsigned int j;
        for( j=0; j<in->GetNumRoadsIn(); j++ )
        {
            Road* inRoad = (Road*) in->GetRoadIn( j );
            rAssert( inRoad );

            if( inRoad == rd )
            {
                rAssert( (Intersection*) rd->GetDestinationIntersection() == in );
                found = true;
                break;
            }
        }
        for( j=0; j<in->GetNumRoadsOut(); j++ )
        {
            Road* outRoad = (Road*) in->GetRoadOut( j );
            rAssert( outRoad );

            if( outRoad == rd )
            {
                rAssert( (Intersection*) rd->GetSourceIntersection() == in );
                found = true;
                break;
            }
        }
        rAssert( found );
    }
#endif

    return totalDist;
}

float RoadManager::GetTraversalDistance( ShortestRoad* fromRoad, ShortestRoad* toRoad )
{   
    rAssert( fromRoad );
    rAssert( toRoad );

    rmt::Vector vec0, vec1, vec2, vec3, start, end;
    const Intersection* traversedInt = NULL;
    RoadSegment* fromSeg = NULL;
    RoadSegment* toSeg = NULL;
    if( fromRoad->isOutRoad ) 
    {
        // fromRoad is an OUT road at lastInt, so it's an IN road at currInt, so 
        // its last segment is at currInt
        
        traversedInt = fromRoad->road->GetDestinationIntersection();

        fromSeg = fromRoad->road->GetRoadSegment( fromRoad->road->GetNumRoadSegments()-1 );
        fromSeg->GetCorner( 1, vec1 );
        fromSeg->GetCorner( 2, vec2 );
        start = (vec1+vec2) * 0.5f;
    }
    else
    {
        // fromRoad is an IN road at lastInt, so its an OUT road at currInt and therefore the
        // first segment is at currInt

        traversedInt = fromRoad->road->GetSourceIntersection();

        fromSeg = fromRoad->road->GetRoadSegment( 0 );
        fromSeg->GetCorner( 0, vec0 );
        fromSeg->GetCorner( 3, vec3 );
        start = (vec0+vec3) * 0.5f;
    }

    if( toRoad->isOutRoad ) 
    {
        // toRoad is an OUT road at currInt, so its first segment is at currInt

        // make sure this is an OUT road belonging to the traversedInt (our currInt)
        rAssert( toRoad->road->GetSourceIntersection() == traversedInt );

        toSeg = toRoad->road->GetRoadSegment( 0 );
        toSeg->GetCorner( 0, vec0 );
        toSeg->GetCorner( 3, vec3 );
        end = (vec0+vec3) * 0.5f;
    }
    else
    {
        // toRoad is an IN road at currInt, so its last segment is at currInt

        // make sure this is an IN road belonging to the traversedInt (our currInt)
        rAssert( toRoad->road->GetDestinationIntersection() == traversedInt );

        toSeg = toRoad->road->GetRoadSegment( toRoad->road->GetNumRoadSegments()-1 );
        toSeg->GetCorner( 1, vec1 );
        toSeg->GetCorner( 2, vec2 );
        end = (vec1+vec2) * 0.5f;
    }

    // now the traversal distance can be either obtained more accurately
    // by us building a spline... or just by linear straight line dist..
    // let's try the linear dist first.
    float traversalDist = (start - end).Length(); // *** SQUARE ROOT! ***

    // If our from road is the same as our to road, we're being foolish...
    // so we penalize the request by augmenting the distance by the 
    // intersection's radius...
    if( fromRoad->road == toRoad->road )
    {
        traversalDist += traversedInt->GetRadius();
    }

    return traversalDist;
}


//
// Follows current intersection in the given direction till we reach 
// a big intersection, where we ask how far it is to get to the target intersection.
// Returns this distance.
//
float RoadManager::FindDistToTargetInOneDirection( 
    bool useMultiplier,
    Intersection* targetInt, 
    Intersection* currInt,
    Intersection* lastInt,
    ShortestRoad* shortestRoadFromLastInt,
    SwapArray<PathElement>& elems,
    ShortestRoad*& firstShortRoad, // needed for traversal dist from src road to srcInt
    ShortestRoad*& lastShortRoad,  // needed for traversal dist from targetInt to target road
    ErrorValue& errVal )
{
    rAssert( targetInt );
    rAssert( currInt );
    rAssert( lastInt );
    rAssert( elems.IsSetUp() );

    // add current intersection to list
    PathElement intElem;
    intElem.type = ET_INTERSECTION;
    intElem.elem = currInt;
    elems.Add( intElem );

    float cost = 0.0f;

    // found target
    if( targetInt == currInt )
    {
        errVal = FOUND_TARGET;
        return cost;
    }
    // Found "Big" Intersection
    if( currInt->mBigIntersection ) 
    {
        NodeData* nodeData = useMultiplier ? 
            &(currInt->mBigIntersection->routesWithMultiplier[ targetInt->mIndex ]) :
            &(currInt->mBigIntersection->routesNoMultiplier[ targetInt->mIndex ]) ;

        errVal = FOUND_BIGINTERSECTION;

        // First add the total dist from this BigInt to target intersection
        cost = nodeData->dist; 

        // Now if there was a previous road... 
        // Compute the traversal cost through the currInt, which is the
        // distance from the end of the shortest road from lastInt to the
        // start of the shortest road to destInt (stored in nodeData)
        if( shortestRoadFromLastInt )
        {
            ShortestRoad* fromRoad = shortestRoadFromLastInt;
            ShortestRoad* toRoad = nodeData->roadToIn;
            cost += GetTraversalDistance( fromRoad, toRoad );
        }

        // update the first and last shortroad data..
        if( firstShortRoad == NULL )
        {
            firstShortRoad = nodeData->roadToIn;
        }
        lastShortRoad = nodeData->roadJustBeforeIn;

        return cost;
    }
    // Found "linear" Intersection
    else 
    {
        // Last intersection becomes current, current becomes last
        ShortestRoad* shortRoad = NULL;
        Intersection* nextInt = NULL;

        // given last int, get the nextInt and the shortestRoad to nextInt
        currInt->GetOtherIntersection( useMultiplier, lastInt, nextInt, shortRoad );
        if( nextInt == NULL )
        {
            // if deadend
            errVal = DEAD_END;
            return NEAR_INFINITY;
        }

        lastInt = currInt;
        currInt = nextInt;

        // add road to list
        PathElement roadElem;
        roadElem.type = ET_NORMALROAD; 
        roadElem.elem = shortRoad->road;
        elems.Add( roadElem );

        errVal = STILL_LOOKING;

        // the cost begins with the cost of the road to nextInt
        // and, if there was a previous road, is augmented by the 
        // traversal cost through currInt, from end of shortestRoadToLastInt 
        // to start of shortestRoad to nextInt)
        //
        cost = shortRoad->cost; 
        if( shortestRoadFromLastInt )
        {
            ShortestRoad* fromRoad = shortestRoadFromLastInt;
            ShortestRoad* toRoad = shortRoad;
            cost += GetTraversalDistance( fromRoad, toRoad );
        }

        // update the first and last shortroad data..
        if( firstShortRoad == NULL )
        {
            firstShortRoad = shortRoad;
        }
        lastShortRoad = shortRoad;

        return cost + FindDistToTargetInOneDirection( useMultiplier,
            targetInt, currInt, lastInt, shortRoad, elems, 
            firstShortRoad, lastShortRoad, errVal );
    }

    // shouldn't be here
    rAssert( false );
    errVal = UNEXPECTED;
    return NEAR_INFINITY;
}


void RoadManager::TraverseRoads( 
    bool useMultiplier,
    Intersection* targetInt, 
    Intersection* currInt,
    Intersection* lastInt,
    SwapArray<PathElement>& elems,
    ErrorValue& errVal )
{
    rAssert( targetInt );
    rAssert( currInt );
    rAssert( lastInt );
    rAssert( elems.IsSetUp() );

    // add current intersection to list
    PathElement intElem;
    intElem.type = ET_INTERSECTION;
    intElem.elem = currInt;
    elems.Add( intElem );

    // found target
    if( targetInt == currInt )
    {
        errVal = FOUND_TARGET;
        return;
    }

    ShortestRoad* shortRoad = NULL;

    // Found "Big" Intersection
    if( currInt->mBigIntersection ) 
    {
        SwapArray<NodeData>* routes = useMultiplier ?
            &(currInt->mBigIntersection->routesWithMultiplier) :
            &(currInt->mBigIntersection->routesNoMultiplier) ;

        // find the adjacent intersection that will take us to the target 
        // & the shortest road that will get us there
        Intersection* adjacentInt = (*routes)[ targetInt->mIndex ].destIn;
        shortRoad = (*routes)[ targetInt->mIndex ].roadToIn;

        // Move current Int... 
        Intersection* tmp = currInt;
        currInt = adjacentInt;
        lastInt = tmp;

        errVal = FOUND_BIGINTERSECTION;
    }
    else //Linear Intersection
    {
        // Last intersection becomes current, current becomes last
        Intersection* nextInt = NULL;

        // given last int
        currInt->GetOtherIntersection( useMultiplier, lastInt, nextInt, shortRoad );
        if( nextInt == NULL )
        {
            // DEAD END??? Can't be... we only call TraverseRoads when
            // we are SURE it will take us to the target
            rAssert( false );
            errVal = DEAD_END;
            return;
        }

        lastInt = currInt;
        currInt = nextInt;

        errVal = STILL_LOOKING;

    }

    rAssert( shortRoad );
    rAssert( currInt );
    rAssert( lastInt );
    rAssert( errVal == STILL_LOOKING || errVal == FOUND_BIGINTERSECTION );

    // add road to list
    PathElement roadElem;
    roadElem.type = ET_NORMALROAD; 
    roadElem.elem = shortRoad->road;
    elems.Add( roadElem );
    
    return TraverseRoads( useMultiplier, targetInt, currInt, lastInt, elems, errVal );

}

Intersection* RoadManager::FindIntersection( const char* name )
{
    return FindIntersection( tEntity::MakeUID( name ) );
}

Intersection* RoadManager::FindIntersection( tUID name )
{
    unsigned int i;
    for ( i = 0; i < mNumIntersectionsUsed; ++i )
    {
        if ( mIntersections[i].GetNameUID() == name )
        {
            //Found it!
            return &(mIntersections[i]);
        }
    }

    return NULL;
}

Intersection* RoadManager::FindIntersection( rmt::Vector& point )
{
    unsigned int i;
    for ( i = 0; i < mNumIntersectionsUsed; ++i )
    {
        if ( (mIntersections[i]).IsPointInIntersection( point ) )
        {
            return &(mIntersections[i]);
        }
    }

    return NULL;
}

Intersection* RoadManager::FindIntersection( int iIndex )
{
    return &(mIntersections[iIndex]);
}




bool RoadManager::FindRoad( const rmt::Vector& point, 
                            const Road** ppRoad, 
                            RoadSegment** ppOutRoadSegment, 
                            int& segmentIndex, 
                            float& in, 
                            float& lateral,
                            bool considerShortCuts ) const
{
    unsigned int i; 
    for ( i = 0; i < mNumRoadsUsed; i++ )
    {
        if( !considerShortCuts && mRoads[i].GetShortCut() )
        {
            continue;
        }

        segmentIndex = mRoads[i].GetRoadSegmentAtPoint( point, ppOutRoadSegment, in, lateral, 0 );
        if ( segmentIndex >= 0 )
        {
            // We found our road.
            //
            *ppRoad = &(mRoads[i]);

            return true;
            break;
        }
    }
    *ppRoad = 0;

    return false;
}


RoadSegmentData* RoadManager::FindRoadSegmentData( const char* name )
{
    tUID nameUID = tEntity::MakeUID( name );
    return FindRoadSegmentData( nameUID );
}

RoadSegmentData* RoadManager::FindRoadSegmentData( tUID name )
{
    unsigned int i;
    for ( i = 0; i < mNumRoadSegmentDataUsed; ++i )
    {
        if ( mRoadSegmentData[i].GetNameUID() == name )
        {
            //This is it!
            return &(mRoadSegmentData[i]);
        }
    }

    return NULL;
}

void RoadManager::AddRoad( Road* pRoad )
{
    rAssert( mNumRoadsUsed < mNumRoads );

    //Make sure this is the current road...
    rAssert( pRoad == &(mRoads[mNumRoadsUsed]) );

    ++mNumRoadsUsed;
}

void RoadManager::AddIntersection( Intersection* pIntersection )
{
    rAssert( mNumIntersectionsUsed < mNumIntersections );

    //Make sure this is the current intersection...
    rAssert( pIntersection == &(mIntersections[mNumIntersectionsUsed]) );

    ++(mNumIntersectionsUsed);
}

void RoadManager::AddRoadSegmentData( RoadSegmentData* pRoadSegmentData )
{
    rAssert( mNumRoadSegmentDataUsed < mNumRoadSegmentData );

    //Make sure this is the current intersection...
    rAssert( pRoadSegmentData == &(mRoadSegmentData[mNumRoadSegmentDataUsed]) );

    ++(mNumRoadSegmentDataUsed);
}

void RoadManager::AddRoadSegment( RoadSegment* pRoadSegment )
{
    rAssert( mNumRoadSegmentsUsed < mNumRoadSegments);

    //Make sure this is the current intersection...
    //rAssert( pRoadSegment == &(mRoadSegments[mNumRoadSegmentsUsed]) );
    rAssert( pRoadSegment == mRoadSegments[mNumRoadSegmentsUsed] );

    ++(mNumRoadSegmentsUsed);
}


// search roads and intersections for whichever's closest, given a position
void RoadManager::FindClosestPathElement
(
    const rmt::Vector& pos,         // IN: search center
    float searchRadius,             // IN: search radius
    PathfindingOptions options,     // IN: search options

    PathElement& closestElem,       // OUT: closest element (road or intersection)
    RoadSegment*& closestRoadSeg,   // OUT: if closest element is road, this is closest seg
    float& closestRoadSegT,         // OUT: if closest element is road, this is segment t value
    float& closestRoadT             // OUT: if closest element is road, this is road's t value
)
{
    rAssert( 0 <= options && options <= NUM_POS );

    // must search for SOMETHING... default to all
    if( !(options & PO_SEARCH_ROADS) && !(options & PO_SEARCH_INTERSECTIONS) )
    {
        options |= PO_SEARCH_ROADS | PO_SEARCH_INTERSECTIONS;
    }

    // APPROACH
    // ========
    // Search all roads in given radius to find:
    //   the closest road segment, and 
    //   the distance to this road segment
    //
    // Search all intersections in given radius to find:
    //   the closest intersection, and
    //   the distance to this intersection
    // 
    // If distance to closest road segment <= distance to closest intersection
    //   use the road segment
    // else
    //   use the intersection
    // 

    // find closets road segment
    RoadSegment* closestSeg = NULL;
    float distSqrToClosestSeg = 100000000.0f;

    if( options & PO_SEARCH_ROADS )
    {
        if( options & PO_SEARCH_SHORTCUTS )
        {
            GetIntersectManager()->FindClosestAnyRoad( 
                pos, 
                searchRadius, 
                closestSeg, 
                distSqrToClosestSeg );
        }
        else
        {
            GetIntersectManager()->FindClosestRoad( 
                pos, 
                searchRadius, 
                closestSeg, 
                distSqrToClosestSeg );
        }
        rAssert( closestSeg );
    }


    // find closest intersection
    Intersection* closestInt = NULL;
    float distSqrToClosestInt = 100000000.0f;

    if( options & PO_SEARCH_INTERSECTIONS )
    {
        for( unsigned int i = 0; i < mNumIntersectionsUsed; ++i )
        {
            rmt::Vector intPos;
            mIntersections[i].GetLocation( intPos );

            float distSqr = (intPos - pos).MagnitudeSqr();
            if( distSqr < distSqrToClosestInt ) 
            {
                closestInt = &mIntersections[i];
                distSqrToClosestInt = distSqr;
            }
        }

        rAssert( closestInt );
    }

    // Populate return values
    if( distSqrToClosestSeg <= distSqrToClosestInt )
    {
        rAssert( closestSeg );

        closestElem.type = ET_NORMALROAD;
        closestElem.elem = closestSeg->GetRoad();
        closestRoadSeg = closestSeg;
        closestRoadSegT = DetermineSegmentT( pos, closestRoadSeg );
        closestRoadT = DetermineRoadT( closestRoadSeg, closestRoadSegT );
    }
    else
    {
        rAssert( closestInt );

        closestElem.type = ET_INTERSECTION;
        closestElem.elem = closestInt;
        closestRoadSeg = NULL;
        closestRoadSegT = 0.0f;
        closestRoadT = 0.0f;
    }
}



//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
