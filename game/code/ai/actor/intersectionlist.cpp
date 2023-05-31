//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   intersectionlist
//
// Description: Holds a list of pointers to sim collision bounding boxes
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai\actor\intersectionlist.h>
#include <simcollision\proximitydetection.hpp>
#include <simcollision\collisionvolume.hpp>
#include <simcollision\collisionobject.hpp>
#include <simcommon\simstate.hpp>
#include <simcommon\simstatearticulated.hpp>
#include <float.h>
#include <render\DSG\DynaPhysDSG.h>
#include <render\DSG\fenceentitydsg.h>
#include <render\DSG\AnimCollisionEntityDSG.h>
#include <render\intersectmanager\intersectmanager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const unsigned int SIZE_INTERSECTION_LIST_ARRAYS = 50;
const unsigned int SIZE_FENCE_PIECE_ARRAY = 100;
const float INTERSECTION_TEST_RAY_THICKNESS = 1.0f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// IntersectionList::IntersectionList
//===========================================================================
// Description:
//      IntersectionList ctor - presizes internal arrays
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

IntersectionList::IntersectionList()
{
    m_StaticCollisionList.reserve( SIZE_INTERSECTION_LIST_ARRAYS );
    m_DynamicCollisionList.reserve( SIZE_INTERSECTION_LIST_ARRAYS );
    m_AnimPhysCollisionList.reserve( SIZE_INTERSECTION_LIST_ARRAYS );
    // Allocate the fence list array
    m_FenceList.reserve( SIZE_FENCE_PIECE_ARRAY );

}


//===========================================================================
// IntersectionList::~IntersectionList
//===========================================================================
// Description:
//      IntersectionList dtor - removes all data from the list
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

IntersectionList::~IntersectionList()
{
    Clear();
}

void
IntersectionList::Clear()
{
    ClearStatics();
    ClearDynamics();
    ClearAnimPhys();
    ClearFencePieces();
}

void
IntersectionList::ClearDynamics()
{
    DynaCollListIt it;
    for ( it = m_DynamicCollisionList.begin() ; it != m_DynamicCollisionList.end() ; it++ )
    {
        it->first->Release();
        if ( it->second )
            it->second->Release();
    }

    m_DynamicCollisionList.clear();
}

void IntersectionList::ClearAnimPhys()
{
    for ( unsigned int i = 0 ; i < m_AnimPhysCollisionList.size() ; i++ )
    {
        m_AnimPhysCollisionList[i]->Release();
    }
    m_AnimPhysCollisionList.resize(0);      
}

void 
IntersectionList::ClearStatics()
{
    for ( unsigned int i = 0 ; i < m_StaticCollisionList.size() ; i++ )
    {
        m_StaticCollisionList[i]->Release();
    }
    m_StaticCollisionList.resize(0);      
}

void
IntersectionList::ClearFencePieces()
{
    m_FenceList.resize(0);
}

bool 
IntersectionList::TestIntersectionStatics( const rmt::Vector& sourceOfRay, const rmt::Vector& endOfRay, rmt::Vector* intersection )
{
    rAssert( m_StaticCollisionList.capacity() == SIZE_INTERSECTION_LIST_ARRAYS );

    bool foundIntersection = false;
    // Lets iterate through the boxes and see if any hit

    rmt::Vector sray;
    sray.Sub(endOfRay, sourceOfRay);
    rmt::Vector ray = sray;
    float rayLen = ray.NormalizeSafe();

    float closestDistance = FLT_MAX;
    sim::RayIntersectionInfo info;
    info.SetMethod( sim::RayIntersectionVolume );   
    info.sRayThickness = INTERSECTION_TEST_RAY_THICKNESS;

    for ( unsigned int i = 0 ; i < m_StaticCollisionList.size() ; i++ )
    {
        sim::CollisionVolume* vol = m_StaticCollisionList[i]->GetCollisionVolume();
        if ( vol )
        {
            info.mCollisionVolume = vol;
            if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
            {
                if( info.mDistFromSource < closestDistance )
                {
                    // This volume is the closest yet
                    // make not of it and keep going
                    foundIntersection = true;
                    closestDistance = info.mDistFromSource;
                }
            }
        }
    }


    // Test fence pieces
    LineSegment2D rayLineSeg;
    rayLineSeg.p1.x = sourceOfRay.x;
    rayLineSeg.p1.y = sourceOfRay.z;
    rayLineSeg.p2.x = endOfRay.x;
    rayLineSeg.p2.y = endOfRay.z;

    for ( unsigned int i = 0 ; i < m_FenceList.size() ; i++ )
    {
        float t;
        if ( LineSegmentIntersection( rayLineSeg, m_FenceList[i], &t ) )
        {
            LineSegment2D f = m_FenceList[i];
            // Get the distance based upon t
            rmt::Vector fenceIntersection = sourceOfRay + t * ( endOfRay - sourceOfRay );
            float dist = ( sourceOfRay - fenceIntersection ).Magnitude();
            
            if ( dist < closestDistance )
            {
                closestDistance = dist;
                foundIntersection = true;
            }
        }
    }


    if ( foundIntersection )
    {
        // The intersection point is going to be 
        // sourceOfRay + ray * closestDistance
        closestDistance += INTERSECTION_TEST_RAY_THICKNESS;
        *intersection = sourceOfRay + ray * closestDistance;  
    }

    return foundIntersection;
}

bool 
IntersectionList::TestIntersectionDynamics( const rmt::Vector& sourceOfRay, 
                                            const rmt::Vector& endOfRay, 
                                            rmt::Vector* intersection,
                                            DynaPhysDSG** objectHit )
{
    rAssert( m_DynamicCollisionList.capacity() == SIZE_INTERSECTION_LIST_ARRAYS );

    bool foundIntersection;

    // Lets iterate through the boxes and see if any hit

    rmt::Vector sray;
    sray.Sub(endOfRay, sourceOfRay);
    rmt::Vector ray = sray;
    float rayLen = ray.NormalizeSafe();

    float closestDistance = FLT_MAX;
    sim::RayIntersectionInfo info;
    info.SetMethod( sim::RayIntersectionVolume );
    info.sRayThickness = INTERSECTION_TEST_RAY_THICKNESS;

    DynaCollListIt it;

    for ( it = m_DynamicCollisionList.begin() ; it != m_DynamicCollisionList.end() ; it++ )
    {
        info.mCollisionVolume = it->first->GetCollisionVolume();
        if ( info.mCollisionVolume )
        {
            if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
            {
                if( info.mDistFromSource < closestDistance )
                {
                    // This volume is the closest yet
                    // make not of it and keep going
                    closestDistance = info.mDistFromSource;
                    if ( objectHit )
                        *objectHit = it->second;
                }
            }
        }
    }
    if ( closestDistance != FLT_MAX )
    {
        // The intersection point is going to be 
        // sourceOfRay + ray * closestDistance
        closestDistance += INTERSECTION_TEST_RAY_THICKNESS;
        *intersection = sourceOfRay + ray * closestDistance;
        foundIntersection = true;        
    }
    else
    {
        foundIntersection = false;
    }

    return foundIntersection;
}

bool 
IntersectionList::TestIntersectionAnimPhys( const rmt::Vector& sourceOfRay, 
                                            const rmt::Vector& endOfRay, 
                                            rmt::Vector* intersection )
{
    rAssert( m_AnimPhysCollisionList.capacity() == SIZE_INTERSECTION_LIST_ARRAYS );

    bool foundIntersection = false;
    // Lets iterate through the boxes and see if any hit

    rmt::Vector sray;
    sray.Sub(endOfRay, sourceOfRay);
    rmt::Vector ray = sray;
    float rayLen = ray.NormalizeSafe();

    float closestDistance = FLT_MAX;
    sim::RayIntersectionInfo info;
    info.SetMethod( sim::RayIntersectionVolume );   
    info.sRayThickness = INTERSECTION_TEST_RAY_THICKNESS;

    for ( unsigned int i = 0 ; i < m_AnimPhysCollisionList.size() ; i++ )
    {
        sim::CollisionVolume* vol = m_AnimPhysCollisionList[i]->GetCollisionVolume();
        if ( vol )
        {
            info.mCollisionVolume = vol;
            if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
            {
                if( info.mDistFromSource < closestDistance )
                {
                    // This volume is the closest yet
                    // make not of it and keep going
                    foundIntersection = true;
                    closestDistance = info.mDistFromSource;
                }
            }
        }
    }


    // Test fence pieces
    LineSegment2D rayLineSeg;
    rayLineSeg.p1.x = sourceOfRay.x;
    rayLineSeg.p1.y = sourceOfRay.z;
    rayLineSeg.p2.x = endOfRay.x;
    rayLineSeg.p2.y = endOfRay.z;

    for ( unsigned int i = 0 ; i < m_FenceList.size() ; i++ )
    {
        float t;
        if ( LineSegmentIntersection( rayLineSeg, m_FenceList[i], &t ) )
        {
            LineSegment2D f = m_FenceList[i];
            // Get the distance based upon t
            rmt::Vector fenceIntersection = sourceOfRay + t * ( endOfRay - sourceOfRay );
            float dist = ( sourceOfRay - fenceIntersection ).Magnitude();

            if ( dist < closestDistance )
            {
                closestDistance = dist;
                foundIntersection = true;
            }
        }
    }


    if ( foundIntersection )
    {
        // The intersection point is going to be 
        // sourceOfRay + ray * closestDistance
        closestDistance += INTERSECTION_TEST_RAY_THICKNESS;
        *intersection = sourceOfRay + ray * closestDistance;  
    }

    return foundIntersection;
}

bool 
IntersectionList::TestIntersection( const rmt::Vector& sourceOfRay, const rmt::Vector& endOfRay, rmt::Vector* intersection, float rayWidthForFences )
{
    rAssert( m_StaticCollisionList.capacity() == SIZE_INTERSECTION_LIST_ARRAYS );
    rAssert( m_DynamicCollisionList.capacity() == SIZE_INTERSECTION_LIST_ARRAYS );


    bool foundIntersection;

    // Lets iterate through the boxes and see if any hit
    rmt::Vector sray;
    sray.Sub(endOfRay, sourceOfRay);
    rmt::Vector ray = sray;
    float rayLen = ray.NormalizeSafe();

    float closestDistance = FLT_MAX;
    sim::RayIntersectionInfo info;
    info.SetMethod( sim::RayIntersectionVolume );
    info.sRayThickness = INTERSECTION_TEST_RAY_THICKNESS;

    DynaCollListIt it;

    for ( it = m_DynamicCollisionList.begin() ; it != m_DynamicCollisionList.end() ; it++ )
    {
        info.mCollisionVolume = it->first->GetCollisionVolume();
        if ( info.mCollisionVolume )
        {
            if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
            {
                if( info.mDistFromSource < closestDistance )
                {
                    // This volume is the closest yet
                    // make note of it and keep going
                    closestDistance = info.mDistFromSource;
                }
            }
        }
    }
    for ( unsigned int i = 0 ; i < m_StaticCollisionList.size() ; i++ )
    {
        info.mCollisionVolume = m_StaticCollisionList[i]->GetCollisionVolume();
        if ( info.mCollisionVolume )
        {
            if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
            {
                if( info.mDistFromSource < closestDistance )
                {
                    // This volume is the closest yet
                    // make note of it and keep going
                    closestDistance = info.mDistFromSource;
                }
            }
        }
    }
    LineSegment2D rayLineSeg;
    rayLineSeg.p1.x = sourceOfRay.x;
    rayLineSeg.p1.y = sourceOfRay.z;
    rayLineSeg.p2.x = endOfRay.x;
    rayLineSeg.p2.y = endOfRay.z;

    for ( unsigned int i = 0 ; i < m_FenceList.size() ; i++ )
    {
        float t;
        if ( LineSegmentIntersection( rayLineSeg, m_FenceList[i], &t ) )
        {
            // Get the distance based upon t
            rmt::Vector fenceIntersection = sourceOfRay + t * ( endOfRay - sourceOfRay );
            float dist = ( sourceOfRay - fenceIntersection ).Magnitude();

            if ( dist < closestDistance )
            {
                closestDistance = dist - rayWidthForFences;
                foundIntersection = true;
            }
        }
    }

    if ( closestDistance != FLT_MAX )
    {
        // The intersection point is going to be 
        // sourceOfRay + ray * closestDistance
        closestDistance += INTERSECTION_TEST_RAY_THICKNESS;
        *intersection = sourceOfRay + ray * closestDistance;
        foundIntersection = true;        
    }
    else
    {
        foundIntersection = false;
    }

    return foundIntersection;
}


bool 
IntersectionList::LineOfSight( const rmt::Vector& sourceOfRay, const rmt::Vector& endOfRay, float rayWidth, bool staticsOnly )
{
    rmt::Vector sray;
    sray.Sub(endOfRay, sourceOfRay);
    rmt::Vector ray = sray;
    float rayLen = ray.NormalizeSafe();


    unsigned int i;
    sim::RayIntersectionInfo info;
    info.SetMethod( sim::RayIntersectionVolume );
    info.sRayThickness = rayWidth;


    // Test the statics
    for ( i = 0 ; i < m_StaticCollisionList.size() ; i++ )
    {
        info.mCollisionVolume = m_StaticCollisionList[i]->GetCollisionVolume();
        if ( info.mCollisionVolume )
        {
            if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
            {
                // Hit
                return false;
            }
        }
    }

    if ( !staticsOnly )
    {
        // Test the dynamics
        DynaCollListIt it;
        for ( it = m_DynamicCollisionList.begin() ; it != m_DynamicCollisionList.end() ; it++ )    
        {
            info.mCollisionVolume = it->first->GetCollisionVolume();
            if ( info.mCollisionVolume )
            {
                if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
                {
                    // Hit
                    return false;
                }
            }
        }

        for ( i = 0 ; i < m_AnimPhysCollisionList.size() ; i++ )
        {
            info.mCollisionVolume = m_AnimPhysCollisionList[i]->GetCollisionVolume();
            if ( info.mCollisionVolume )
            {
                if ( sim::RayIntersectVolume( sourceOfRay, ray, sray, rayLen, info ) )
                {
                    // Hit
                    return false;
                }
            }
        }
    }

    LineSegment2D rayLineSeg;
    rayLineSeg.p1.x = sourceOfRay.x;
    rayLineSeg.p1.y = sourceOfRay.z;
    rayLineSeg.p2.x = endOfRay.x;
    rayLineSeg.p2.y = endOfRay.z;

    // Test the fence pieces
    for ( unsigned int i = 0 ; i < m_FenceList.size() ; i++ )
    {
        float t;
        if ( LineSegmentIntersection( rayLineSeg, m_FenceList[i], &t ) )
        {
            return false;
        }
    }

    return true;
}

void 
IntersectionList::AddStatic( sim::SimState* simstate )
{
    if ( m_StaticCollisionList.size() >= SIZE_INTERSECTION_LIST_ARRAYS )
        return;

    if ( simstate )
    {
        sim::CollisionObject* collisionObject = simstate->GetCollisionObject();
        if ( collisionObject )
        {
            collisionObject->AddRef();
            m_StaticCollisionList.push_back( collisionObject );
        }
    }
}

void 
IntersectionList::AddDynamic( sim::SimState* simstate, DynaPhysDSG* object )
{
    if ( m_DynamicCollisionList.size() >= SIZE_INTERSECTION_LIST_ARRAYS )
        return;


    if ( simstate )
    {
        sim::CollisionObject* collisionObject = simstate->GetCollisionObject();
        if ( collisionObject )
        {
            if(m_DynamicCollisionList.find(collisionObject) == m_DynamicCollisionList.end())
            {
                collisionObject->AddRef();

                if ( object ) 
                    object->AddRef();

                m_DynamicCollisionList.insert( collisionObject, object );
            }
            else
            {
//                rAssert(0);
            }
        }
    }
}

void 
IntersectionList::AddAnimPhys( sim::SimStateArticulated* simstate )
{
    if ( m_AnimPhysCollisionList.size() >= SIZE_INTERSECTION_LIST_ARRAYS )
        return;

    if ( simstate )
    {
        sim::CollisionObject* collisionObject = simstate->GetCollisionObject();
        if ( collisionObject )
        {
            collisionObject->AddRef();
            m_AnimPhysCollisionList.push_back( collisionObject );
        }
    }
}


void 
IntersectionList::AddFencePiece( FenceEntityDSG* fencePiece )
{
    static int maxcount = 0;
    // Prevent any dynamic allocations because of the push_back contained in this function
    if ( m_FenceList.size() >= SIZE_FENCE_PIECE_ARRAY )
    {
        return;
    }
    LineSegment2D lineSegment;

    lineSegment.p1.x = fencePiece->mStartPoint.x;
    lineSegment.p1.y = fencePiece->mStartPoint.z;

    lineSegment.p2.x = fencePiece->mEndPoint.x;
    lineSegment.p2.y = fencePiece->mEndPoint.z;

    m_FenceList.push_back( lineSegment );

}

// Add all objects in the given sphere to the list
int 
IntersectionList::FillIntersectionListStatics( const rmt::Vector& position, float radius )
{
    int numAdded = FillIntersectionListFence( position, radius );
    
    ClearStatics();
    // Iterate through the statics and grab them all
    ReserveArray< StaticPhysDSG* > spList(200);
    GetIntersectManager()->FindStaticPhysElems( const_cast< rmt::Vector& >( position ), radius, spList );
    for ( int i = 0 ; i < spList.mUseSize ; i++ )
    {
        StaticPhysDSG* object = spList[i];
        sim::SimState* simState = object->GetSimState();
        AddStatic( simState );
    }        
    numAdded += spList.mUseSize;

    return numAdded;
}

int 
IntersectionList::FillIntersectionListFence( const rmt::Vector& position, float radius )
{
    ClearFencePieces();

    ReserveArray< FenceEntityDSG* > feList(200);
    // Perhaps intersectmanager should be changed to accept a const rmt::Vector& instead?
    GetIntersectManager()->FindFenceElems( const_cast< rmt::Vector& > (position), radius, feList );
    for ( int i = 0 ; i < feList.mUseSize ; i++ )
    {
        FenceEntityDSG* object = feList[i];
        AddFencePiece( object );
    }

    return feList.mUseSize;
}

int IntersectionList::FillIntersectionListDynamics( const rmt::Vector& position, float radius, bool addDynaPhys, DynaPhysDSG* source )
{
    ClearDynamics();

    ReserveArray< DynaPhysDSG* > dpList(200);
    // Perhaps intersectmanager should be changed to accept a const rmt::Vector& instead?
    GetIntersectManager()->FindDynaPhysElems( const_cast< rmt::Vector& > (position), radius, dpList );
    for ( int i = 0 ; i < dpList.mUseSize ; i++ )
    {
        DynaPhysDSG* object = dpList[i];

        if ( object != source )
        {
            sim::SimState* simState = object->GetSimState();
            // [Dusit: June 16th, 2003]
            // Don't seem to be using object anywhere, and there seemed to be an addref/release 
            // mismatch coming from this system on traffic vehicles (supercam uses this on the
            // traffic)... So this is a quick hack to solve that little problem.
            AddDynamic( simState, (addDynaPhys? object : NULL) );
        }
    }

    return dpList.mUseSize;
}

// Add all objects in the given sphere to the list
int 
IntersectionList::FillIntersectionListAnimPhys( const rmt::Vector& position, float radius )
{
    int numAdded = FillIntersectionListFence( position, radius );

    ClearAnimPhys();
    // Iterate through the statics and grab them all
    ReserveArray< AnimCollisionEntityDSG* > spList(200);
    GetIntersectManager()->FindAnimPhysElems( const_cast< rmt::Vector& >( position ), radius, spList );
    for ( int i = 0 ; i < spList.mUseSize ; i++ )
    {
        AnimCollisionEntityDSG* object = spList[i];
        sim::SimStateArticulated* simState = object->GetSimState();
        AddAnimPhys( simState );
    }        
    numAdded += spList.mUseSize;

    return numAdded;
}

bool 
IntersectionList::LineSegmentIntersection( const LineSegment2D& line1, const LineSegment2D& line2, float* t )
{

    
    float x1 = line1.p1.x;
    float x2 = line1.p2.x;
    float x3 = line2.p1.x;
    float x4 = line2.p2.x;
    float y1 = line1.p1.y;
    float y2 = line1.p2.y;
    float y3 = line2.p1.y;
    float y4 = line2.p2.y;

    float a_denom = (y4-y3)*(x2-x1)-(x4-x3)*(y2-y1);
    if ( rmt::Fabs( a_denom ) < 0.001f )
        return false;

    float ua = ( (x4 - x3) * ( y1 - y3 ) - ( y4 - y3 ) * ( x1 - x3 ) ) / a_denom ;
    if ( ua < 0 || ua > 1.0f )
        return false;

    float b_denom = (y4-y3)*(x2-x1)-(x4-x3)*(y2-y1);
    if ( rmt::Fabs( b_denom ) < 0.001f )
        return false;

    float ub = ( (x2 - x1) * ( y1 - y3 ) - ( y2 - y1 ) * ( x1 - x3 ) ) / b_denom;
    if ( ub < 0 || ub > 1.0f )
        return false;

    *t = ua;

    return true;
}
