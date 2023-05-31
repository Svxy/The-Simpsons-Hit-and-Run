//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        geometry.cpp
//
// Description: Some linear algebra/geometry stuff mostly used in traffic
//              Also contains some useful structures.
//
// History:     09/09/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================



#include <roads/geometry.h>
#include <raddebug.hpp>

/*
//////////////////////////////////////////////////////////////////////////////
// OLD OLD OLD STUFF
//////////////////////////////////////////////////////////////////////////////

bool fequals(float a, float b)
{
    return (rmt::Fabs(a-b)<=MYEPSILON)? true: false;
}
bool fequals(float a, float b, float epsilon)
{
    return (rmt::Fabs(a-b)<=epsilon)? true: false;
}
bool isVerticalLine( Line line )
{
	if( fequals(line.x1,line.x2) )
    {
		return true;
    }
	return false;
}
Line getLine( float x1, float y1, float x2, float y2, bool isInfinite ) 
{
	Line line;
	line.x1 = x1;
	line.y1 = y1;
	line.x2 = x2;
	line.y2 = y2;
	line.isVertical = isVerticalLine( line );
	if( !line.isVertical ) 
    {
		line.slope = (line.y2 - line.y1)/(line.x2 - line.x1);
		line.b = line.y2 - line.slope * line.x2;
	}
	line.isInfinite = isInfinite;
	line.isFinishLine = false;
	return line;
}
bool isPointOnLine( Line line, Point p ) 
{
	// test first if point lies in line equation
	if( !line.isVertical ) 
    {
		if( fequals(p.y, line.slope * p.x + line.b) ) 
        {
			// Ok, so we've verified that p lies on the line.
			// now if the line is infinite, we're done
			if( line.isInfinite ) 
            {
				return true;
            }

			// else test if point lies on line itself we always make sure 
			// x1 < x2 and y1 < y2, swapping values as needed.
			if( line.x2 < line.x1 ) 
            {
				float temp = line.x2;
				line.x2 = line.x1;
				line.x1 = temp;
			}
			if( line.y2 < line.y1 ) 
            {
				float temp = line.y2;
				line.y2 = line.y1;
				line.y1 = temp;
			}

			// now check if p.x lies between acceptable values of x 
			//       and if p.y lies between acceptable values of y
			if( (line.x1 - MYEPSILON < p.x && p.x < line.x2 + MYEPSILON ) &&
				(line.y1 - MYEPSILON < p.y && p.y < line.y2 + MYEPSILON ) )
            {
				return true;
            }
		}
	}
	else 
    { // if vertical line
		if( fequals(p.x,line.x1) ) 
        {
			if( line.isInfinite )
            {
				return true;
            }
			if( line.y2 < line.y1 ) 
            {
				float temp = line.y2;
				line.y2 = line.y1;
				line.y1 = temp;
			}

			if( line.y1 - MYEPSILON < p.y && p.y < line.y2 + MYEPSILON )
            {
				return true;
            }
		}
	}
	return false;
}

//==============================================================================
// Helper function:  IntersectLines2D
//===============================================================================
// Description: Intersects two co-planar lines 
// 
// Parameters: (rmt::Vector) (rmt::Vector) (rmt::Vector) (rmt::Vector) (rmt::Vector&)
//
// Return: bool
//
//==============================================================================
bool IntersectLines2D( rmt::Vector p1, 
                       rmt::Vector dir1, 
                       rmt::Vector p2, 
                       rmt::Vector dir2,
                       rmt::Vector& p )
{
	// These will temporarily contain our intersection point coords
    // Remember that Point and Line work on x-y plane, not x-z plane
    // so let y = z
    //
    Point pTemp;

    rmt::Vector q1 = p1 + dir1;
    rmt::Vector q2 = p2 + dir2;

    Line line1 = getLine( p1.x, p1.z, q1.x, q1.z, true );
    Line line2 = getLine( p2.x, p2.z, q2.x, q2.z, true );


	// treat vertical cases separately
	if( line1.isVertical && line2.isVertical ) 
    {
		return false;
	}
	else if( line1.isVertical && !line2.isVertical ) 
    {
		// line1 is vertical and line2 is not, so pluck line1's 
		// x-value into line2's equation to find y coord of 
		// the potential intersection point.
		pTemp.x = line1.x1;
		pTemp.y = line2.slope * pTemp.x + line2.b;
		if( isPointOnLine(line1,pTemp) && isPointOnLine(line2,pTemp) ) 
        {
			p.x = pTemp.x;
            p.y = p1.y;
			p.z = pTemp.y;
			return true;
		}
		return false;
	}
	//else if( !isVerticalLine(line1) && isVerticalLine(line2) ) 
	else if( !line1.isVertical && line2.isVertical ) 
    {
		// same as case above, but switch line1, line2
		pTemp.x = line2.x1;
		pTemp.y = line1.slope * pTemp.x + line1.b;
		if( isPointOnLine(line1,pTemp) && isPointOnLine(line2,pTemp) ) 
        {
			p.x = pTemp.x;
			p.y = p1.y;
            p.z = pTemp.y;
			return true;
		}
		return false;
	}
	else 
    { //both non vertical

		if( line1.slope == line2.slope )
			return false;

		// find intersection point
		pTemp.x = (line2.b-line1.b)/(line1.slope-line2.slope);
		pTemp.y = line1.slope* pTemp.x + line1.b;

		// make sure this point lies within the bounds of both lines
		if( isPointOnLine(line1,pTemp) && isPointOnLine(line2,pTemp) ) 
        {
			p.x = pTemp.x;
			p.y = p1.y;
            p.z = pTemp.y;
			return true;
		}
		else 
        {
			return false;
        }
	}
	return false; // just in case
}
*/


//////////////////////////////////////////////////////////////////////////////////
// CUBIC BEZIER SHEEYATSU
//////////////////////////////////////////////////////////////////////////////////

bool CubicBezier::sIsInitialized = false;
float CubicBezier::B0[CubicBezier::MAX_CURVE_POINTS] = {0.0f};
float CubicBezier::B1[CubicBezier::MAX_CURVE_POINTS] = {0.0f};
float CubicBezier::B2[CubicBezier::MAX_CURVE_POINTS] = {0.0f};
float CubicBezier::B3[CubicBezier::MAX_CURVE_POINTS] = {0.0f};


void CubicBezier::InitOnceLUTs()
{
    if( sIsInitialized )
    {
        return;
    }

    float bias = 1.0f / (float)(MAX_CURVE_POINTS - 1);

    int i = MAX_CURVE_POINTS - 1;
    float t = 0.0f, t2, t3;
	for( i = MAX_CURVE_POINTS-1; i>=0; i-- )
	{
        t2 = t*t; 
        t3 = t2*t;

        // Fill the LUTs
        B0[i] = t3;
        B1[i] = 3.0f * t2 * (1.0f-t);
        B2[i] = 3.0f * t * (1.0f-t)*(1.0f-t);
        B3[i] = (1.0f-t)*(1.0f-t)*(1.0f-t);

        t += bias;
    }

    rAssert( i == -1);

    sIsInitialized = true;
}



CubicBezier::CubicBezier()
{
    if( !CubicBezier::sIsInitialized )
    {
        CubicBezier::InitOnceLUTs();
    }

    mCurveIsCreated = false;
    mCurveIsCreated2D = false;
    mNumControlPointsAdded = 0;

    rmt::Vector dummy( 0.0f, 0.0f, 0.0f );
    int i;
    for( i=0; i<CubicBezier::MAX_CONTROL_POINTS; i++ )
    {
        AddControlPoint( dummy );
    }
}

CubicBezier::~CubicBezier()
{
}

void CubicBezier::GetCubicBezierCurve(rmt::Vector*& pts, int& nPts)
{
    if( !mCurveIsCreated )
    {
        CreateCubicBezierCurve();
    }
    pts = mCurve;
    nPts = MAX_CURVE_POINTS;
}

void CubicBezier::GetCubicBezierCurve2D(rmt::Vector*& pts, int& nPts)
{
    if( !mCurveIsCreated2D )
    {
        CreateCubicBezierCurve2D();
    }
    pts = mCurve2D;
    nPts = MAX_CURVE_POINTS;
}

void CubicBezier::SetControlPoint( const rmt::Vector &cp, const int index )
{
    rAssert( 0 <= index && index < mNumControlPointsAdded );

    mCurveIsCreated = false;
    mCurveIsCreated2D = false;
    mControlPoints[index] = cp;
}

void CubicBezier::AddControlPoint( const rmt::Vector &cp )
{
    rAssert( 0 <= mNumControlPointsAdded && mNumControlPointsAdded < MAX_CONTROL_POINTS );

    mCurveIsCreated = false;
    mCurveIsCreated2D = false;
    mControlPoints[mNumControlPointsAdded] = cp;
    mNumControlPointsAdded++;
}


void CubicBezier::CreateCubicBezierCurve()
{
    rAssert( mNumControlPointsAdded == MAX_CONTROL_POINTS );

    if( mCurveIsCreated )
    {
        return;
    }

    int i;
    for( i=0; i<MAX_CURVE_POINTS; i++ )
    {
        mCurve[i] = mControlPoints[0] * B0[i] + 
                    mControlPoints[1] * B1[i] + 
                    mControlPoints[2] * B2[i] + 
                    mControlPoints[3] * B3[i];
    }
    mCurveIsCreated = true;

}

void CubicBezier::CreateCubicBezierCurve2D()
{
    rAssert( mNumControlPointsAdded == MAX_CONTROL_POINTS );

    if( mCurveIsCreated2D )
    {
        return;
    }

    float ep = 0.1f;

    // **** ASSERT INFORMATION ***
    // If you encounter this assert, it means that the road segments in a nearby
    // intersection don't have same values of y where they hit the intersection
    // (intersection isn't horizontal). This is due to BAD ROAD DATA ON WORLD BUILDER SIDE. 
    //
    // Because we're using CubicBezier in 2D now, the intersection must be FLAT!
    // Or cars will "hop" when they make the turn. 
    // 
    // Please notify Sheik to check the road data around nearby intersections.
    rAssert( rmt::Epsilon( mControlPoints[0].y, mControlPoints[1].y, ep ) &&
             rmt::Epsilon( mControlPoints[0].y, mControlPoints[2].y, ep ) &&
             rmt::Epsilon( mControlPoints[0].y, mControlPoints[3].y, ep ) );

    int i;
    for( i=0; i<MAX_CURVE_POINTS; i++ )
    {
        mCurve2D[i].x = mControlPoints[0].x * B0[i] + 
                        mControlPoints[1].x * B1[i] + 
                        mControlPoints[2].x * B2[i] + 
                        mControlPoints[3].x * B3[i];

        mCurve2D[i].y = mControlPoints[0].y;

        mCurve2D[i].z = mControlPoints[0].z * B0[i] + 
                        mControlPoints[1].z * B1[i] + 
                        mControlPoints[2].z * B2[i] + 
                        mControlPoints[3].z * B3[i];
    }
    mCurveIsCreated2D = true;

}

/////////////////////////////////////////////////////////////////
// DListArray Class Definition
//////////////////////////////////////////////////////////////////

DListArray::DListArray()
{
    this->Clear();
}


void DListArray::Clear()
{
    int i=0;
    for( i ; i<(MAX_ELEMS-1) ; ++i )
    {
        mElems[i].data = NULL;
        mElems[i].next = i+1;
        mElems[i].prev = -1;
    }
    mElems[i].data = NULL;
    mElems[i].next = -1;
    mElems[i].prev = -1;

    mHead = -1;
    mTail = -1;
    mFree = 0;

    mnElems = 0;
}

// returns the index value of the newly added element
// or -1 on error
int DListArray::AddLast( void* data )
{
    assert( data != NULL );

    if( mFree == -1 )
    {
        return -1;
    }

    mElems[mFree].data = data;
    mElems[mFree].prev = mTail;

    if( mnElems > 0 )
    {
        mElems[mTail].next = mFree;
    }
    else
    {
        mHead = mFree;
    }

    mTail = mFree;
    mFree = mElems[mFree].next;
    mElems[mTail].next = -1;

    mnElems++;
    return mTail;
}

// returns the index value of the newly added element
// or -1 on error
int DListArray::AddFirst( void* data )
{
    assert( data != NULL );

    if( mFree == -1 )
    {
        return -1;
    }

    int newIndex = mFree;
    mFree = mElems[mFree].next;

    mElems[newIndex].data = data;
    mElems[newIndex].next = mHead;

    if( mnElems > 0 )
    {
        mElems[mHead].prev = newIndex;
    }
    else 
    {
        mTail = newIndex;
    }

    mHead = newIndex;

    mnElems++;
    return mHead;
}


// returns index value of the newly inserted element
// or -1 on error
int DListArray::InsertAfter( void* data, int i ) 
{
    assert( mnElems >= 1 );
    assert( data != NULL );
    assert( 0 <= i && i < MAX_ELEMS );
    assert( mElems[i].data != NULL );

    if( mFree == -1 )
    {
        return -1;
    }

    int newIndex = mFree;
    mFree = mElems[mFree].next;

    mElems[newIndex].data = data;
    mElems[newIndex].prev = i;
    mElems[newIndex].next = mElems[i].next;

    if( mElems[i].next != -1 )
    {
        mElems[ mElems[i].next ].prev = newIndex;
    }
    else 
    {
        mTail = newIndex;
    }

    mElems[i].next = newIndex;
        
    mnElems++;
    return newIndex;
}

// 
bool DListArray::Remove( void* data ) 
{
    assert( data != NULL );

    int i = 0;
    bool res = false;
    for( i ; i<mnElems ; i++ ) 
    {
        if( mElems[i].data == data )
        {
            res = true;
            break;
        }
    }
    if( res )
    {
        res = Remove(i);
    }
    return res;
}

bool DListArray::Remove( int i ) 
{
    assert( 0 <= i && i < MAX_ELEMS );
    assert( mElems[i].data != NULL );

    if( mnElems <= 0 )
    {
        return false;
    }

    if( mElems[i].next != -1 )
    {
        mElems[ mElems[i].next ].prev = mElems[i].prev;
    }
    else
    {
        mTail = mElems[i].prev;
    }

    if( mElems[i].prev != -1 )
    {
        mElems[ mElems[i].prev ].next = mElems[i].next;
    }
    else
    {
        mHead = mElems[i].next;
    }

    mElems[i].next = mFree;
    mElems[i].prev = -1;
    mElems[i].data = NULL;

    mFree = i;

    mnElems--;
    return true;
}

int DListArray::Find( void* data )
{
    rAssert( data != NULL );

    int i = 0;
    for( i ; i<mnElems ; i++ ) 
    {
        if( mElems[i].data == data )
        {
            return i;
        }
    }
    return -1;
}



//////////////////////////////////////////////////////////////////////////////
// MISC
//////////////////////////////////////////////////////////////////////////////

// for Ray p1 to p2 and sphere 2
// return number of intersection points and the intersection points
// q1 and q2
int IntersectLineSphere( const rmt::Vector& p1, 
                         const rmt::Vector& p2, 
                         const rmt::Sphere& s, 
                         rmt::Vector* intPts )
{

    float X1 = (float)p1.x;
    float X2 = (float)p2.x;
    float X3 = (float)s.centre.x;
    float Y1 = (float)p1.y;
    float Y2 = (float)p2.y;
    float Y3 = (float)s.centre.y;
    float Z1 = (float)p1.z;
    float Z2 = (float)p2.z;
    float Z3 = (float)s.centre.z;
    float Sr = (float)s.radius;

    float A, B, C;
    A = (X2 - X1)*(X2 - X1) + (Y2 - Y1)*(Y2 - Y1) + (Z2 - Z1)*(Z2 - Z1); 
    B = (X2 - X1)*(X1 - X3) + (Y2 - Y1)*(Y1 - Y3) + (Z2 - Z1)*(Z1 - Z3);
    C = (X1 - X3)*(X1 - X3) + (Y1 - Y3)*(Y1 - Y3) + (Z1 - Z3)*(Z1 - Z3) - Sr*Sr;
    //B = 2 * ((X2 - X1)*(X1 - X3) + (Y2 - Y1)*(Y1 - Y3) + (Z2 - Z1)*(Z1 - Z3));
    //C = X3*X3 + Y3*Y3 + Z3*Z3 + X1*X1 + Y1*Y1 + Z1*Z1 - 2*(X3*X1 + Y3*Y1 + Z3*Z1) - Sr*Sr;

    float discriminant = B*B - A*C;
    //float discriminant = B*B - 4*A*C;
    float t[2];


    if( discriminant < 0.0f )
    {
        return 0;
    }
    else if( rmt::Epsilon(discriminant, 0.0f, 0.001f) )
    {
        //t[0] = (-1*B)/2*A;
        t[0] = (-1*B)/A;
    
        if( 0.0f <= t[0] && t[0] <= 1.0f )
        {
            intPts[0].Set( 
                (float)(X1+t[0]*(X2-X1)), 
                (float)(Y1+t[0]*(Y2-Y1)), 
                (float)(Z1+t[0]*(Z2-Z1)));
            return 1;
        }
        return 0;
    }
    else 
    {
        float sqrtDiscriminant = rmt::Sqrt(discriminant); // *** SQUARE ROOT! *** 
//        t[0] = (-1*B - sqrtDiscriminant) / 2*A;
//        t[1] = (-1*B + sqrtDiscriminant) / 2*A;
        t[0] = (-1*B - sqrtDiscriminant) / A;
        t[1] = (-1*B + sqrtDiscriminant) / A;

        rmt::Vector testVec;
        int i=0, j=0;
        for( i; i<2; i++)
        {
            if( 0.0f <= t[i] && t[i] <= 1.0f )
            {
                intPts[j].Set( 
                    (float)(X1+t[i]*(X2-X1)), 
                    (float)(Y1+t[i]*(Y2-Y1)), 
                    (float)(Z1+t[i]*(Z2-Z1)));
                j++;
            }
        } 
        return j;
    }
    return -1;
}


bool TestIntersectLineSphere( const rmt::Vector& lOrig,
                              const rmt::Vector& lDir,
                              const rmt::Sphere& s )
{
    rmt::Vector closestPtOnLine;
    FindClosestPointOnLine( lOrig, lOrig+lDir, s.centre, closestPtOnLine );

    float distSqr = (s.centre - closestPtOnLine).MagnitudeSqr();
    bool res = distSqr <= (s.radius * s.radius);
    return res;
}



// Test using (normalized) myHeading DOT vectorFromMyHeadingToTarget
bool WillCollide( const rmt::Vector& myPos, 
                  const rmt::Vector& myHeading, // Must be normalized
                  const rmt::Vector& mySide,    // Must be normalized
                  float myRadius, 
                  float myLookAheadDist,
                  const rmt::Vector& targetPos,
                  bool& targetOnMyRightSide )
{
    rmt::Vector toTarget = targetPos - myPos;
    float myLookAheadDistSqr = myLookAheadDist * myLookAheadDist;

    // if target lies within my look-ahead distance
    if( toTarget.MagnitudeSqr() < myLookAheadDistSqr )
    {
        float frontOrBehindTest = myHeading.Dot( toTarget );
        if( frontOrBehindTest > 0.0f )
        {
            // target is in front, which is a concern...
            float lateralDist = mySide.Dot( toTarget );
            if( lateralDist > 0.0f )
            {
                targetOnMyRightSide = true;
            }
            else
            {
                targetOnMyRightSide = false;
            }
            lateralDist = rmt::Fabs( lateralDist );

            // if target is in our path
            if( lateralDist <= myRadius )
            {
                return true;
            }
        }
    }
    return false;
}


rmt::Vector UpdateVUP( const rmt::Vector& position, const rmt::Vector& target )
{
    const float epsilon = 0.01f;
    //Set the vUP by projecting the heading into the ZX plane and creating a 
    //crossproduct of a right angle to the projected heading along the X axis 
    //and the heading.
    rmt::Vector X, Y, Z;
    Z.Sub(target, position);
    X.Set(Z.z, 0, -Z.x);

    if ( rmt::Epsilon( X.x, 0, epsilon ) &&
         rmt::Epsilon( X.y, 0, epsilon ) &&
         rmt::Epsilon( X.z, 0, epsilon ) )
    {
        //Then the camera is looking straight down.
        Y.Set( 0, 0, 1.0f ); //Up along the Z...
    }
    else
    {
        Y.CrossProduct(Z, X);
        Y.Normalize(); // *** SQUARE ROOT! ***
    }

	return Y;
}

// Given points P1 and P2, and two points that define a line, A and B
// P1 and P2 are on the same side of the line, if the Normals for BAxP1A
// and BAxP2A are pointing on the same side of the plane (i.e. 
// N1-dot-N2 >= 0)

bool PointsOnSameSideOfLine( const rmt::Vector& P1,
                             const rmt::Vector& P2,
                             const rmt::Vector& A,
                             const rmt::Vector& B )
{
    rmt::Vector BA, P1A, P2A, crossP1, crossP2;

    BA.Sub( B, A );
    P1A.Sub( P1, A );
    P2A.Sub( P2, A );

    crossP1.CrossProduct( BA, P1A );
    crossP2.CrossProduct( BA, P2A );
    if( crossP1.Dot(crossP2) >= 0 )
    {
        return true;
    }
    return false;
}



// Given triangle with vertices v1, v2, v3 and a point p
// p is inside triangle if it is on the same side of line v1v2 as v3
// and on the same side of line v2v3 as v1, 
// and on the same side of line v1v3 as v2
//
// NOTE: Because we use Which-Side-of-Line solution, the point
//       that is "within" a triangle is not necessarily on the
//       same plane as the triangle (it could still be above or
//       below the actual triangle, as long as it lies within the infinite
//       planes formed by the 3 sides of the triangle.
//
bool PointLiesInTriangle ( const rmt::Vector& p, 
                           const rmt::Vector& v1, 
                           const rmt::Vector& v2, 
                           const rmt::Vector& v3 ) 
{
    if( PointsOnSameSideOfLine( p, v1, v2, v3 ) &&
        PointsOnSameSideOfLine( p, v2, v1, v3 ) &&
        PointsOnSameSideOfLine( p, v3, v1, v2 ) )
    {
        return true;
    }
    return false;
}


rmt::Vector GetProjectionVector( const rmt::Vector& source, const rmt::Vector& target )
{
    return target * ( target.Dot(source) / target.Dot(target) );
}    

// In Lefthand coordinate system, turn vector to
// the left (counter-clockwise) 90 degrees about Y axis & return new vector
rmt::Vector Get90DegreeLeftTurn( const rmt::Vector& orig )
{
    rmt::Vector newVec;
    newVec.Set( -1 * orig.z, orig.y, orig.x );
    return newVec;
}

// In Lefthand coordinate system, turn vector to
// the right (clockwise) 90 degrees about Y axis & return new vector
rmt::Vector Get90DegreeRightTurn( const rmt::Vector& orig )
{
    rmt::Vector newVec;
    newVec.Set( orig.z, orig.y, -1 * orig.x );
    return newVec;
}


float GetRotationAboutY( float x, float z )
{
    float angle = 0.0f;
    if( rmt::Epsilon(x, 0.0f) && rmt::Epsilon(z, 0.0f) )
    {
        angle = 0.0f;
    }
    else 
    {
        // generate angle from x-z vector
        // - assumes DEFAULT_FACING_VECTOR is (0,0,-1)
        // - assumes UP VECTOR is (0,1,0)
        angle = rmt::ATan2(x, z);
        if( rmt::IsNan(angle) )
        {
            angle = 0.0f;
        }

        /*
        angle = rmt::ATan2(-x, -z);

        // wrap to [0, 2*pi)
        if (angle < 0.0f)
        {
            angle += rmt::PI_2;
        }
        */
    }
    return angle;
}


bool PointToLineProjection2D( const rmt::Vector& inPt, 
                              const rmt::Vector& linePt1, 
                              const rmt::Vector& linePt2, 
                              rmt::Vector& outPt  )
{
    // The beauty of calling this 2D function is that we break it down into components
    // so we don't do unnecessary float ops for the y values
    float x1,x2,x3,z1,z2,z3;
    x1 = linePt1.x;
    x2 = linePt2.x;
    x3 = inPt.x;
    z1 = linePt1.z;
    z2 = linePt2.z;
    z3 = inPt.z;

    // make sure we were given a line, not a point dammit!
    rAssertMsg( !( rmt::Epsilon( x1,x2,0.0005f ) && rmt::Epsilon( z1,z2,0.0005f )),
        "PointToLineProjection2D: The two points of the \"line\" are too close together.\n" );

    // THEORY
    // ======
    // Let outPt be the point resulting from projecting inPt onto 
    // linesegment [linePt2 - linePt1]:
    //
    // 1) outPt = linePt1 + u * [linePt2 - linePt1];
    //
    // Since the projection is at Right Angle, we can say that the lines  
    // [linePt2 - linePt1] and [inPt - outPt] have zero-length dotproduct:
    // 
    // 2) [inPt - outPt] dot [linePt2 - linePt1] = 0
    //
    // Substituting 1) into 2) gives:
    //
    // 3) [ inPt - [linePt1+u*[linePt2 - linePt1]] ] dot [linePt2 - linePt1] = 0
    //
    // Solving for u gives:
    //
    float x2MINUSx1 = x2-x1;
    float z2MINUSz1 = z2-z1;
    float u = ( (x3-x1)*x2MINUSx1 + (z3-z1)*z2MINUSz1 ) / 
              ( (x2MINUSx1*x2MINUSx1)+(z2MINUSz1*z2MINUSz1) );
 
    // Populate the return point
    outPt.Set( x1 + u*x2MINUSx1, inPt.y, z1 + u*z2MINUSz1 );

    // If u is not on the line segment, outPt will not be in bounds
    if( u < 0.0f || u > 1.0f )
    {
        return false;
    }
    return true;
}


bool PointOnLeftSideOfLine( const rmt::Vector& p, 
                            const rmt::Vector& start,
                            const rmt::Vector& end )
{
    rmt::Vector start2p = p - start;
    rmt::Vector start2end = end - start;
    rmt::Vector leftVec = Get90DegreeLeftTurn( start2end );

    float dp = leftVec.Dot( start2p );
    if( dp > 0.0f ) // +ve means on the left
    {
        return true;
    }
    return false;
}

bool PointOnRightSideOfLine( const rmt::Vector& p, 
                             const rmt::Vector& start,
                             const rmt::Vector& end )
{
    rmt::Vector start2p = p - start;
    rmt::Vector start2end = end - start;
    rmt::Vector rightVec = Get90DegreeRightTurn( start2end );

    float dp = rightVec.Dot( start2p );
    if( dp > 0.0f ) // +ve means on the right
    {
        return true;
    }
    return false;
}

float FindClosestPointOnLine( const rmt::Vector& start,
                             const rmt::Vector& end,
                             const rmt::Vector& p,
                             rmt::Vector& closestPt )
{

    rmt::Vector start2p = p - start;
    rmt::Vector lDir = end - start;

    float lDirMagSqr = lDir.Dot(lDir);

    // if end and start are basically the same point, 
    // then lDir is zero. So the closest point returned 
    // is that point
    //
    if( rmt::Epsilon( lDirMagSqr, 0.0f, 0.001f ) )
    {
        closestPt = end;
        return 0.0f;
    }

    float scale = lDir.Dot( start2p ) / lDirMagSqr;
    if( scale > 1.0f )
    {
        closestPt = end;
    }
    else if( scale < 0.0f )
    {
        closestPt = start;
    }
    else
    {
        closestPt = start + lDir * scale;
    }
    return scale;
}

float GetLineSegmentT
( 
    const rmt::Vector& segStart, 
    const rmt::Vector& segEnd, 
    const rmt::Vector& pt 
)
{
    float e = 0.0001f;

#if defined( RAD_DEBUG ) || defined( RAD_TUNE )
    // make sure this point is on the line
    rmt::Vector closestPt;
    FindClosestPointOnLine( segStart, segEnd, pt, closestPt );
    rAssert( closestPt.Equals( pt, e ) );
#endif

    float segT = 0.0f;
    if( rmt::Epsilon( segEnd.x, segStart.x, e ) )
    {
        if( rmt::Epsilon( segEnd.y, segStart.y, e ) )
        {
            if( rmt::Epsilon( segEnd.z, segStart.z, e ) )
            {
                segT = 0.0f;
            }
            else
            {
                segT = (pt.z - segStart.z) / (segEnd.z - segStart.z);
            }
        }
        else
        {
            segT = (pt.y - segStart.y) / (segEnd.y - segStart.y);
        }
    }
    else
    {
        segT = (pt.x - segStart.x) / (segEnd.x - segStart.x);
    }
    rAssert( 0.0f <= segT && segT <= 1.0f ); 
    return segT;
}