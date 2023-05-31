//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        geometry.h
//
// Description: Some linear algebra/geometry stuff mostly used in traffic
//              Also contains some useful structures.
//
// History:     09/09/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================



#ifndef GEOMETRY_H
#define GEOMETRY_H

// *************************
// 2D & 3D GEOMETRY HELPERS
// *************************
#include <radmath/radmath.hpp>
#include <raddebug.hpp> // for rAssert & other debug print outs

/* 
//////////////////////////////////////////////////////////////////////////////
// OLD OLD OLD STUFF
//////////////////////////////////////////////////////////////////////////////
#ifndef PI_F
#define PI_F 3.1415926535897932384626433832795f
#endif
#define MYEPSILON 0.001
#ifndef NULL
    #define NULL 0
#endif
struct Line 
{
	float x1;
	float y1;
	float x2;
	float y2;
	float slope;
	float b;
	bool isVertical;
	bool isInfinite;
	bool isFinishLine;
};

struct Point 
{
	float x;
	float y;
	int id;
};
bool fequals(float a, float b);
bool fequals(float a, float b, float epsilon);
bool isVerticalLine( Line line );
Line getLine( float x1, float y1, float x2, float y2, bool isInfinite );
bool isPointOnLine( Line line, Point p );
bool IntersectLines2D( rmt::Vector p1, 
                       rmt::Vector dir1, 
                       rmt::Vector p2, 
                       rmt::Vector dir2,
                       rmt::Vector& p );
*/

//////////////////////////////////////////////////////////////////////////////
// CUBIC BEZIER SHEEYATSU
//////////////////////////////////////////////////////////////////////////////

class CubicBezier
{
public:
    enum 
    {
        MAX_CONTROL_POINTS = 4, // total control points (including start & end points)
        MAX_CURVE_POINTS = 30   // total curve points including start & end points
    };

    static void InitOnceLUTs();

    static bool sIsInitialized;
    static float B0[MAX_CURVE_POINTS];
    static float B1[MAX_CURVE_POINTS];
    static float B2[MAX_CURVE_POINTS];
    static float B3[MAX_CURVE_POINTS];

    CubicBezier();
    ~CubicBezier();

    void GetCubicBezierCurve(rmt::Vector*& pts, int& nCurvePts);
    void GetCubicBezierCurve2D(rmt::Vector*& pts, int& nCurvePts);
    void AddControlPoint(const rmt::Vector& cp);
    void SetControlPoint(const rmt::Vector& cp, int index);


protected:

    void CreateCubicBezierCurve();
    void CreateCubicBezierCurve2D();

    rmt::Vector mCurve[MAX_CURVE_POINTS];
    rmt::Vector mCurve2D[MAX_CURVE_POINTS];
    rmt::Vector mControlPoints[MAX_CONTROL_POINTS];
    int mNumControlPointsAdded;
    bool mCurveIsCreated;
    bool mCurveIsCreated2D;

    //Prevent wasteful constructor creation.
    CubicBezier( const CubicBezier& CubicBezier );
    CubicBezier& operator=( const CubicBezier& CubicBezier );

};



//////////////////////////////////////////////////////////////////////////////
// DListArray
//////////////////////////////////////////////////////////////////////////////

class DListArray
{

public:

    enum
    {
        MAX_ELEMS = 20
    };

    DListArray();
    void Clear();

    // returns index value of found element, -1 on error
    int Find( void* data );

    // returns the index value of the newly added element
    // or -1 on error
    int AddLast( void* data );

    // returns the index value of the newly added element
    // or -1 on error
    int AddFirst( void* data );

    // returns index value of the newly inserted element
    // or -1 on error
    int InsertAfter( void* data, int i );

    // Note: this incurs a linear search
    bool Remove( void* data );

    bool Remove( int i );

    int GetNumElems() const;

    int GetFree() const;

    void* GetDataAt( int i ) const;

    int GetNextOf( int i ) const;

    int GetPrevOf( int i ) const;

    void* GetFirst() const;

    void* GetLast() const;

    int GetHead() const;
    
    int GetTail() const;
    
private:

    struct DLAElem 
    {
        void* data;
        int next;
        int prev;
    };

    DLAElem mElems[MAX_ELEMS];
    int mnElems;
    int mHead;
    int mTail;
    int mFree;

};

inline int DListArray::GetNumElems() const
{
    return mnElems;
}
inline int DListArray::GetFree() const 
{
    return mFree;
}
inline void* DListArray::GetDataAt(int i) const
{
    return mElems[i].data;
}

inline void* DListArray::GetFirst() const
{
    if( mHead != -1 )
    {
        return mElems[mHead].data;
    }
    return NULL;
}
inline void* DListArray::GetLast() const
{
    if( mTail != -1 )
    {
        return mElems[mTail].data;
    }
    return NULL;
}
inline int DListArray::GetHead() const
{
    return mHead;
}
inline int DListArray::GetTail() const
{
    return mTail;
}

inline int DListArray::GetNextOf( int i ) const
{
    rAssert( 0 <= i && i < MAX_ELEMS );
    return mElems[i].next;
}

inline int DListArray::GetPrevOf( int i ) const
{
    rAssert( 0 <= i && i < MAX_ELEMS );
    return mElems[i].prev;
}


// history tracking
template <class T, int HISTORY_SIZE> class History
{
public:
    History() : mNextSpot(0) {}
    ~History() {}

    void Init( const T& t )
    {
        mNextSpot = 0;

        for( int i=0; i< HISTORY_SIZE; i++ )
        {
            mHistory[i] = t;
        }

        mAverage = t;
    }

    void UpdateHistory( const T& t)
    {
        // first get the old value & recalculate our average
        mAverage -= (mHistory[mNextSpot] - t) / (float)(HISTORY_SIZE);
        mHistory[mNextSpot] = t;
        mNextSpot = (mNextSpot + 1) % HISTORY_SIZE;
    }

    void GetAverage( T& t )
    {
        t = mAverage;
    }

    T GetEntry( int i )
    {
        rAssert( 0 <= i && i < HISTORY_SIZE );
        return mHistory[i];
    }

    T GetLastEntry()
    {
        int i = mNextSpot - 1;
        if( i == -1 )
        {
            i = HISTORY_SIZE - 1;
        }
        rAssert( 0 <= i && i < HISTORY_SIZE );
        return mHistory[i];
    }

    int GetSize()
    {
        return HISTORY_SIZE;
    }

private:
    int mNextSpot;
    T mHistory[HISTORY_SIZE];
    T mAverage;
};

template <int HISTORY_SIZE> class VectorHistory : public History<rmt::Vector, HISTORY_SIZE>
{
public:
    VectorHistory() {}
    ~VectorHistory() {}

    void Init( const rmt::Vector& t )
    {
        History<rmt::Vector,HISTORY_SIZE>::Init(t);
        GetAverage(mNormalizedAverage);
    }

    void GetNormalizedAverage( rmt::Vector& vec )
    {
        if( rmt::Epsilon( mNormalizedAverage.MagnitudeSqr(), 1.0f, 0.0005f ) )
        {
            vec = mNormalizedAverage;
        }
        else
        {
            mNormalizedAverage.NormalizeSafe(); 
            vec = mNormalizedAverage;
        }
    }

    void UpdateHistory( const rmt::Vector& vec )
    {
        History<rmt::Vector,HISTORY_SIZE>::UpdateHistory(vec);
        GetAverage(mNormalizedAverage);
    }

protected:

    rmt::Vector mNormalizedAverage;
};

//////////////////////////////////////////////////////////////////////////////
// MISC
//////////////////////////////////////////////////////////////////////////////

const float KPH_2_MPS = 1.0f/3.60f;

// returns true if projection point is on line segment
bool PointToLineProjection2D( const rmt::Vector& in, 
                              const rmt::Vector& linePt1, 
                              const rmt::Vector& linePt2, 
                              rmt::Vector& out  );

rmt::Vector GetProjectionVector( const rmt::Vector& source, 
                                 const rmt::Vector& target );

// MAYA:                           GAME:
// Right-hand coord                 Left-hand coord
//                              
//         +y (forefinger)                  +y (forefinger)
//          |                                |
//          |                                |
//          |                                |
//          /\                               /\
//        /    \                           /    \
//      /        \                       /        \
//   +z (middle)  +x (thumb)        +x(thumb)     +z(middle)
//

// In Lefthand coordinate system, turn vector to
// the left (counter-clockwise) 90 degrees & return new vector
rmt::Vector Get90DegreeLeftTurn( const rmt::Vector& orig );

// In Lefthand coordinate system, turn vector to
// the right (clockwise) 90 degrees & return new vector
rmt::Vector Get90DegreeRightTurn( const rmt::Vector& orig );

float GetRotationAboutY( float x, float z );

// returns the number of intersections and po   ints q1 & q2
int IntersectLineSphere( const rmt::Vector& p1, 
                         const rmt::Vector& p2, 
                         const rmt::Sphere& s, 
                         rmt::Vector* intPts);

// just test if line segment intersects sphere... don't bother 
// finding the intersection point(s)
bool TestIntersectLineSphere( const rmt::Vector& lOrig,
                              const rmt::Vector& lDir,
                              const rmt::Sphere& s );



// Test using (normalized) myHeading DOT vectorFromMyHeadingToTarget
bool WillCollide( const rmt::Vector& myPos, 
                  const rmt::Vector& myHeading, // Must be normalized
                  const rmt::Vector& mySide,    // Must be normalized
                  float myRadius, 
                  float myLookAheadDist,
                  const rmt::Vector& targetPos,
                  bool& targetOnMyRightSide );

rmt::Vector UpdateVUP( const rmt::Vector& position, const rmt::Vector& target );

// Given points P1 and P2, and two points that define a line, A and B
// P1 and P2 are on the same side of the line, if the Normals for BAxP1A
// and BAxP2A are pointing on the same side of the plane (i.e. 
// N1-dot-N2 >= 0)

bool PointsOnSameSideOfLine( const rmt::Vector& P1,
                             const rmt::Vector& P2,
                             const rmt::Vector& A,
                             const rmt::Vector& B );



// Given triangle with vertices v1, v2, v3 and a point p
// p is inside triangle if it is on the same side of line v1v2 as v3
// and on the same side of line v2v3 as v1, 
// and on the same side of line v1v3 as v2
//
bool PointLiesInTriangle ( const rmt::Vector& p, 
                           const rmt::Vector& v1, 
                           const rmt::Vector& v2, 
                           const rmt::Vector& v3 );


// Given a point "p", and a line starting at point "start" and ending 
// at point "end", determine if p lies on the left side of the line
// (assuming that the line is looking in the direction of start-to-end
//
bool PointOnLeftSideOfLine( const rmt::Vector& p, 
                            const rmt::Vector& start,
                            const rmt::Vector& end );

// Ditto.. but for the right side
//
bool PointOnRightSideOfLine( const rmt::Vector& p, 
                            const rmt::Vector& start,
                            const rmt::Vector& end );

// Given a line segment described by vector from start to end,
// and an arbitrary point... return the point on the line segment
// closest to this arbitrary point and the float parameter along
// the line segment at which this closest point occurs
float FindClosestPointOnLine( const rmt::Vector& start,
                             const rmt::Vector& end,
                             const rmt::Vector& p,
                             rmt::Vector& closestPt );

float GetLineSegmentT( const rmt::Vector& segStart, 
                       const rmt::Vector& segEnd, 
                       const rmt::Vector& pt );

#endif // GEOMETRY_H
