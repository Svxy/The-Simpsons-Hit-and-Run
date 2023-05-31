//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pathsegment.cpp
//
// Description: Implements PathSegment class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#include <pedpaths/pathsegment.h>
#include <render/Culling/Bounds.h>

void PathSegment::Initialize( Path* parent, int index, rmt::Vector start, rmt::Vector end )
{
    rAssert( parent != NULL );
    rAssert( 0 <= index && index < parent->GetNumPathSegments() );

    mParentPath = parent;
    mIndexToParentPath = index;
    mStartPos = start;
    mEndPos = end;
    mRadius = (end - start).Length() / 2.0f;
}



PathSegment::PathSegment() :
    mParentPath( NULL ),
    mIndexToParentPath( -1 )
{
}

PathSegment::~PathSegment()
{
    mParentPath = NULL;
}


PathSegment::PathSegment( Path* parent, int index, rmt::Vector start, rmt::Vector end )
{
    rAssert( parent != NULL );
    rAssert( 0 < index && index <= parent->GetNumPathSegments() );

    Initialize( parent, index, start, end );
}


//////////////////////////////////////////////////////////////////////////
// Implementing IEntityDSG
//////////////////////////////////////////////////////////////////////////
void PathSegment::DisplayBoundingBox(tColour colour)
{
    rAssert( false );
}
void PathSegment::DisplayBoundingSphere(tColour colour)
{
    rAssert( false );
}
void PathSegment::GetBoundingBox(rmt::Box3D* box)
{
    Bounds3f tempBounds;
    tempBounds.mMin.SetTo(mStartPos);
    tempBounds.mMax.SetTo(mStartPos);
    tempBounds.Accumulate(mEndPos);

    box->Set( tempBounds.mMin, tempBounds.mMax );
}
void PathSegment::GetBoundingSphere(rmt::Sphere* sphere)
{
    sphere->centre = (mStartPos + mEndPos) * 0.5f;
    sphere->radius = mRadius;
}
void PathSegment::Display()
{
    rAssert( false );
}
rmt::Vector* PathSegment::pPosition()
{
    rAssert( false );
    return NULL;
}
const rmt::Vector& PathSegment::rPosition()
{
    rAssert( false );
    return mStartPos; // return some garbage
}
void PathSegment::GetPosition( rmt::Vector* ipPosn )
{
    rAssert( false );
}
///////////////////////////////////////////////////////////////
