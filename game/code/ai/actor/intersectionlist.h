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

// Recompilation protection flag.
#ifndef INTERSECTIONLIST_H
#define INTERSECTIONLIST_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <vector>
#include <memory\srrmemory.h> // Needed for my STL allocations to go on the right heap
#include <memory\map.h>

//===========================================================================
// Forward References
//===========================================================================

namespace sim
{
    class SimState;
    class SimStateArticulated;
    class CollisionVolume;
    class CollisionObject;
    class OBBoxVolume;
}

class DynaPhysDSG;
class FenceEntityDSG;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//
// Constraints:
//      
//
//===========================================================================
class IntersectionList
{
    public:
        IntersectionList();
        virtual ~IntersectionList();

        // Clear all data from the internal arrays and decrement their refcounts
        void Clear();
        void ClearStatics(); // clears only the statics list
        void ClearDynamics(); // only clears the dynamics list
        void ClearAnimPhys(); // only clear animated phys
        void ClearFencePieces(); // only clears the fence list

        // Does the given line segment intersect with a volume?
        // percent indicates the % distance along the line segment of the intersection
        bool TestIntersectionStatics( const rmt::Vector& start, const rmt::Vector& end, rmt::Vector* intersection );
        bool TestIntersectionDynamics( const rmt::Vector& start, const rmt::Vector& end, rmt::Vector* intersection, DynaPhysDSG** objectHit = NULL );
        bool TestIntersectionAnimPhys( const rmt::Vector& start, const rmt::Vector& end, rmt::Vector* intersection );

        // Performs intersection against both dynamics and statics
        bool TestIntersection( const rmt::Vector& start, const rmt::Vector& end, rmt::Vector* intersection, float rayWidthForFences = 0.0f );
        // Perform a line of sight test against statics and dynamics. Faster than TestIntersection
        bool LineOfSight( const rmt::Vector& start, const rmt::Vector& end, float rayWidth = 1.0f, bool staticsOnly = false );


        // Add objects to the internal list
        void AddStatic( sim::SimState* );
        void AddDynamic( sim::SimState*, DynaPhysDSG* dsg = NULL );
        void AddFencePiece( FenceEntityDSG* );
        void AddAnimPhys( sim::SimStateArticulated* );

        // Add all objects in the given sphere to the list
        // Returns the number of objects added to the list
        // Note that fences are considered statics by this class
        // so FillIntersectionListStatics also calls FillIntersectionListFence
        int FillIntersectionListStatics( const rmt::Vector& position, float radius );
        int FillIntersectionListFence( const rmt::Vector& position, float radius );
        // As above for dynamics, but adds a Source dynaphys, one that will never be
        // added to the list. Useful for kicking where the character would be returned from a query
        int FillIntersectionListDynamics( const rmt::Vector& position, float radius, bool addDynaPhys, DynaPhysDSG* source = NULL );
        int FillIntersectionListAnimPhys( const rmt::Vector& position, float radius );

    protected:

        // Arrays of pointers to dynamics and static collision volumes that will
        // be used for intersection testing
        std::vector< sim::CollisionObject*, s2alloc<sim::CollisionObject*> > m_StaticCollisionList;
        typedef Map< sim::CollisionObject*, DynaPhysDSG* > DynaCollList;
        typedef DynaCollList::iterator DynaCollListIt;

        DynaCollList m_DynamicCollisionList;

        std::vector< sim::CollisionObject*, s2alloc<sim::CollisionObject*> > m_AnimPhysCollisionList;

        struct LineSegment2D
        {
            rmt::Vector2 p1, p2;
        };

        std::vector< LineSegment2D, s2alloc< LineSegment2D > > m_FenceList;

        bool LineSegmentIntersection( const LineSegment2D& line1, const LineSegment2D& line2, float* t );

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow IntersectionList from being copied and assigned.
        IntersectionList( const IntersectionList& );
        IntersectionList& operator=( const IntersectionList& );
};


#endif