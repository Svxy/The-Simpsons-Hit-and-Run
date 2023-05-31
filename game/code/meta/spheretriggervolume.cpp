//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement SphereTriggerVolume
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#ifdef WORLD_BUILDER
#include "main/toolhack.h"
#endif

//========================================
// Project Includes
//========================================
#ifndef WORLD_BUILDER
#include <meta/spheretriggervolume.h>
#include <memory/srrmemory.h>
#include <debug/profiler.h>
#else
#define BEGIN_PROFILE(s)
#define END_PROFILE(s)
#include "spheretriggervolume.h"
#endif

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

//==============================================================================
// SphereTriggerVolume::SphereTriggerVolume
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SphereTriggerVolume::SphereTriggerVolume() :
    mRadius( 0.0f )
{
}

//==============================================================================
// SphereTriggerVolume::SphereTriggerVolume
//==============================================================================
// Description: Constructor.
//
// Parameters:	( const rmt::Vector& centre, float radius )
//
// Return:      N/A.
//
//==============================================================================
SphereTriggerVolume::SphereTriggerVolume( const rmt::Vector& centre, 
                                          float radius ) :
    mRadius( radius )
{
    SetPosition( centre );
}

//==============================================================================
// SphereTriggerVolume::~SphereTriggerVolume
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SphereTriggerVolume::~SphereTriggerVolume()
{
}

//=============================================================================
// SphereTriggerVolume::Contains 
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& point, float epsilon )
//
// Return:      bool 
//
//=============================================================================
bool SphereTriggerVolume::Contains( const rmt::Vector& point, 
                                    float epsilon ) const
{
//BEGIN_PROFILE( "Sphere Contains" );

    register float dist_sq;
    rmt::Vector diff;
    diff.Sub( point, GetPosition() );
    dist_sq = diff.MagnitudeSqr();

//END_PROFILE( "Sphere Contains" );

    return( dist_sq < mRadius * mRadius );
}

//=============================================================================
// SphereTriggerVolume::IntersectsBox
//=============================================================================
// Description: Comment
//
// Parameters: ( const rmt::Vector& p1, 
//               const rmt::Vector& p2, 
//               const rmt::Vector& p3, 
//               const rmt::Vector& p4 )
//
// Return:      bool 
//
//=============================================================================
bool SphereTriggerVolume::IntersectsBox( const rmt::Vector& p1, 
                                         const rmt::Vector& p2, 
                                         const rmt::Vector& p3, 
                                         const rmt::Vector& p4 ) const
{

    return ( Contains( p1 ) || Contains( p2 ) || Contains( p3 ) || Contains( p4 ) );

/*
    //Build line segments from the box.
    rmt::Vector l1Dir;
    rmt::Vector l2Dir;
    rmt::Vector l3Dir;
    rmt::Vector l4Dir;

    //This makes the vector directions of the lines making the bounding box.
    l1Dir.Sub(p2, p1);
    l2Dir.Sub(p3, p2);
    l3Dir.Sub(p4, p3);
    l4Dir.Sub(p1, p4);

    if ( (IntersectLineSphere(p1, l1Dir)) ||
         (IntersectLineSphere(p2, l2Dir)) ||
         (IntersectLineSphere(p3, l3Dir)) ||
         (IntersectLineSphere(p4, l4Dir)) )
    {
        return true;
    }       

    return false;
*/
}

//=============================================================================
// SphereTriggerVolume::IntersectsBox
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Box3D& box )
//
// Return:      bool 
//
//=============================================================================
bool SphereTriggerVolume::IntersectsBox( const rmt::Box3D& box )
{
    rmt::Sphere thisAsSphere;
    GetBoundingSphere( &thisAsSphere );
    return box.Intersects( thisAsSphere );
}

//=============================================================================
// SphereTriggerVolume::IntersectLine
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& p1, const rmt::Vector& p2 )
//
// Return:      bool 
//
//=============================================================================
bool SphereTriggerVolume::IntersectLine( const rmt::Vector& p1, const rmt::Vector& p2 ) const
{
    rmt::Vector l1Dir;

    //This makes the vector directions of the lines making the bounding box.
    l1Dir.Sub(p2, p1);

    if ( IntersectLineSphere(p1, l1Dir) )
    {
        return true;
    }       

    return false;
}

//=============================================================================
// SphereTriggerVolume::IntersectsSphere
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& position, float radius )
//
// Return:      bool 
//
//=============================================================================
bool SphereTriggerVolume::IntersectsSphere( const rmt::Vector& position, float radius ) const
{
    register float dist_sq;
    rmt::Vector diff;
    diff.Sub( position, GetPosition() );
    dist_sq = diff.MagnitudeSqr();

    return( dist_sq <= (mRadius + radius) * (mRadius + radius) );
}

//=============================================================================
// SphereTriggerVolume::GetBoundingBox 
//=============================================================================
// Description: Comment
//
// Parameters:  (const rmt::Vector& p1, rmt::Vector& p2)
//
// Return:      bool 
//
//=============================================================================
bool SphereTriggerVolume::GetBoundingBox(rmt::Vector& p1, rmt::Vector& p2) const
{
    rmt::Vector center = GetPosition();

    p1 = rmt::Vector (center.x - mRadius, 
                      center.y - mRadius, 
                      center.z - mRadius);
    p2 = rmt::Vector (center.x + mRadius, 
                      center.y + mRadius, 
                      center.z + mRadius);
    return true;
}

//=============================================================================
// SphereTriggerVolume::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerVolume
//
//=============================================================================
TriggerVolume::Type SphereTriggerVolume::GetType() const
{
    return SPHERE;
}

//////////////////////////////////////////////////////////////////////////
// tDrawable interface
//////////////////////////////////////////////////////////////////////////
//========================================================================
// spheretriggervolume::
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
void SphereTriggerVolume::GetBoundingBox(rmt::Box3D* box)
{
    GetBoundingBox(box->low, box->high);
}
//========================================================================
// spheretriggervolume::
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
void SphereTriggerVolume::GetBoundingSphere(rmt::Sphere* sphere)
{
    sphere->centre = GetPosition();
    sphere->radius = mRadius;
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SphereTriggerVolume::IntersectLineSphere
//=============================================================================
// Description: Comment
//
// Parameters:  (const rmt::Vector lOrig, const rmt::Vector lDir)
//
// Return:      bool 
//
//=============================================================================
bool SphereTriggerVolume::IntersectLineSphere( const rmt::Vector& lOrig, 
                                               const rmt::Vector& lDir ) const
{ 
    rmt::Vector lineOrig = lOrig;
    rmt::Vector lineDir = lDir;

    rmt::Vector kDiff, spherePos;

    spherePos = GetPosition();

    kDiff.Sub(spherePos, lineOrig);
    float fSqrLen = lineDir.MagnitudeSqr();
    float fT = (kDiff.DotProduct(lineDir)) / fSqrLen;

    rmt::Vector closestSegPt;
    if( fT > 1.0f )
    {
        closestSegPt = lOrig + lDir;
    }
    else if( fT < 0.0f )
    {
        closestSegPt = lOrig;
    }
    else 
    {
        closestSegPt = lOrig + lDir * fT;
    }

    kDiff.Sub( closestSegPt, spherePos );
    
    return kDiff.MagnitudeSqr() <= (mRadius*mRadius);
}

//=============================================================================
// SphereTriggerVolume::InitPoints
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
static const int numLong = 12;
static const int numLat = 8;

void SphereTriggerVolume::InitPoints()
{
#ifndef WORLD_BUILDER
#ifndef RAD_RELEASE
MEMTRACK_PUSH_GROUP( "SphereTriggerVolume" );
    // === Initialize ===
    numVerts = numLong * (numLat - 1) + 2;
    numFaces = ((numLong * 2) + (numLong * (numLat-2) * 2)) * 3;

    verts = new(GMA_LEVEL_OTHER) pddiVector[numVerts];
    faces = new(GMA_LEVEL_OTHER) unsigned short[numFaces];

    // === Faces ===
    int curVert, curFace;

    // Faces adjacent to north pole
    curFace = 0;
    for (curVert = 1; curVert <= numLong; curVert++)
    {
        faces[curFace++] = 0;
        faces[curFace++] = curVert;
        if (curVert == numLong)
            faces[curFace++] = 1;
        else
            faces[curFace++] = curVert+1;
    }

    // Faces not adjacent to poles
    int y;

    for (y = 0; y < numLat-2; y++)
    {
        for (curVert = y * numLong + 1;
             curVert < ((y+1) * numLong + 1);
             curVert++)
        {
            bool boundary = (curVert == ((y+1) * numLong));
            int idxNW, idxNE, idxSW, idxSE;
            idxNW = curVert;
            idxNE = curVert + 1;
            idxSW = curVert + numLong;
            idxSE = curVert + numLong + 1;
            if (boundary)
            {
                idxNE -= numLong;
                idxSE -= numLong;
            }

            faces[curFace++] = idxNW;
            faces[curFace++] = idxSW;
            faces[curFace++] = idxSE;

            faces[curFace++] = idxNW;
            faces[curFace++] = idxSE;
            faces[curFace++] = idxNE;
        }
    }

    // Faces adjacent to south pole
    for (curVert = numVerts-numLong-1; curVert <= numVerts-2; curVert++)
    {
        faces[curFace++] = numVerts-1;
        if (curVert == numVerts-2)
            faces[curFace++] = numVerts-numLong-1;
        else
            faces[curFace++] = curVert+1;
        faces[curFace++] = curVert;
    }

    rAssert (curFace == numFaces);
    for (int i = 0; i < numFaces; i++)
    {
        rAssert (faces[i] < numVerts);
    }

    CalcPoints();
MEMTRACK_POP_GROUP( "SphereTriggerVolume" );
#endif
#endif
}

void SphereTriggerVolume::CalcPoints()
{
#ifndef RAD_RELEASE
    rmt::Vector centre;
    float radius;
    centre = GetPosition ();
    radius = GetSphereRadius ();

    // === Vertices ===
    // Poles
    verts[0].Set          (centre.x, centre.y + radius, centre.z);
    verts[numVerts-1].Set (centre.x, centre.y - radius, centre.z);

    // Other points
    int curIdx = 1;

    int y = 0;

    for (y = 1; y < numLat; y++)
    {
        float angLat = ((float)y / (float)numLat) * rmt::PI;
        for (int r = 0; r < numLong; r++)
        {
            float angLong = ((float)r / (float)numLong) * rmt::PI_2;
            rmt::Vector point ((float)(rmt::Cos(angLong)*rmt::Sin(angLat)), (float)rmt::Cos(angLat), (float)(rmt::Sin(angLong)*rmt::Sin(angLat)));
            point.Scale (radius);
            point.Add (centre);

            verts[curIdx++].Set (point.x, point.y, point.z);

        }
    }
    rAssert (curIdx == numVerts-1);

#endif
}
