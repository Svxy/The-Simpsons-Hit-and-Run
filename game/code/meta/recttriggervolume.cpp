//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement RectTriggerVolume
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

//========================================
// Project Includes
//========================================
#ifndef WORLD_BUILDER
#include <meta/recttriggervolume.h>
#include <memory/srrmemory.h>
#include <debug/profiler.h>
#else
#define BEGIN_PROFILE(s)
#define END_PROFILE(s)
#include "recttriggervolume.h"
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
// RectTriggerVolume::RectTriggerVolume
//==============================================================================
// Description: Constructor.
//
// Parameters:	( const rmt::Vector& center, 
//                const rmt::Vector& axisX, 
//                const rmt::Vector& axisY, 
//                const rmt::Vector& axisZ, 
//                float extentX, 
//                float extentY, 
//                float extentZ )
//
// Return:      N/A.
//
//==============================================================================
RectTriggerVolume::RectTriggerVolume( const rmt::Vector& center, 
                                      const rmt::Vector& axisX, 
                                      const rmt::Vector& axisY, 
                                      const rmt::Vector& axisZ, 
                                      float extentX, 
                                      float extentY, 
                                      float extentZ ) :
    mAxisX( axisX ),
    mAxisY( axisY ),
    mAxisZ( axisZ ),
    mExtentX( extentX ),
    mExtentY( extentY ),
    mExtentZ( extentZ )
{
    SetPosition( center );
    UpdateW2T();

    mRadius = rmt::Sqrt(extentX*extentX+extentY*extentY+extentZ*extentZ);
}

//==============================================================================
// RectTriggerVolume::~RectTriggerVolume
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RectTriggerVolume::~RectTriggerVolume()
{
}

//=============================================================================
// RectTriggerVolume::Contains 
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& point, float epsilon )
//
// Return:      bool 
//
//=============================================================================
bool RectTriggerVolume::Contains ( const rmt::Vector& point, 
                                   float epsilon ) const
{
//BEGIN_PROFILE( "Rect Contains" );

    //Transform the point to rect space
    rmt::Vector temp;
    temp.Sub( GetPosition(), point );

    temp.Transform( mWorld2Trigger );

    if( (temp.x  >= -mExtentX ) &&
        (temp.x  <= mExtentX ) &&
        (temp.y  >= -mExtentY ) &&
        (temp.y  <= mExtentY) &&
        (temp.z  >= -mExtentZ ) &&
        (temp.z  <= mExtentZ ) )
    {
//END_PROFILE( "Rect Contains" );
        return true;
    }

//END_PROFILE( "Rect Contains" );
    return false;
}

//=============================================================================
// RectTriggerVolume::IntersectsBox
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& p1, 
//                const rmt::Vector& p2, 
//                const rmt::Vector& p3, 
//                const rmt::Vector& p4 )
//
// Return:      bool 
//
//=============================================================================
bool RectTriggerVolume::IntersectsBox( const rmt::Vector& p1, 
                                       const rmt::Vector& p2, 
                                       const rmt::Vector& p3, 
                                       const rmt::Vector& p4 ) const
{
    if ( Contains( p1 ) || Contains( p2 ) || Contains( p3 ) || Contains( p4 ) )
    {
        return true;
    }

    return false;
}

//=============================================================================
// RectTriggerVolume::IntersectsBox
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Box3D& box )
//
// Return:      bool 
//
//=============================================================================
bool RectTriggerVolume::IntersectsBox( const rmt::Box3D& box )
{
    rmt::Vector p1, p2, p3, p4;

    p1 = box.low;
    p2 = box.high;
    p3 = box.low;
    p3.x = box.high.x;
    p4 = box.high;
    p4.x = box.low.x;

    return IntersectsBox( p1, p2, p3, p4 );
}


//=============================================================================
// RectTriggerVolume::IntersectsSphere
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& position, float radius )
//
// Return:      bool 
//
//=============================================================================
bool RectTriggerVolume::IntersectsSphere( const rmt::Vector& position, 
                                          float radius ) const
{
    return Contains( position, radius );
}

//=============================================================================
// RectTriggerVolume::IntersectLine
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& p1, const rmt::Vector& p2 )
//
// Return:      bool 
//
//=============================================================================
bool RectTriggerVolume::IntersectLine( const rmt::Vector& p1, const rmt::Vector& p2 ) const
{
    if ( Contains( p1 ) || Contains( p2 ) )
    {
        return true;
    }

    return false;
}


//=============================================================================
// RectTriggerVolume::GetBoundingBox 
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& p1, rmt::Vector& p2)
//
// Return:      bool 
//
//=============================================================================
bool RectTriggerVolume::GetBoundingBox (rmt::Vector& p1, rmt::Vector& p2) const
{
    //TODO:  Make this work.
    return false;
}

//=============================================================================
// RectTriggerVolume::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerVolume
//
//=============================================================================
TriggerVolume::Type RectTriggerVolume::GetType() const
{
    return RECTANGLE;
}
//////////////////////////////////////////////////////////////////////////
// tDrawable interface
//////////////////////////////////////////////////////////////////////////
//========================================================================
// recttriggervolume::
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
void RectTriggerVolume::GetBoundingBox(rmt::Box3D* box)
{
    box->low.x = GetPosition().x-mExtentX;
    box->low.y = GetPosition().y-mExtentY;
    box->low.z = GetPosition().z-mExtentZ;

    box->high.x = GetPosition().x+mExtentX;
    box->high.y = GetPosition().y+mExtentY;
    box->high.z = GetPosition().z+mExtentZ;
}
//========================================================================
// recttriggervolume::
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
void RectTriggerVolume::GetBoundingSphere(rmt::Sphere* sphere)
{
    sphere->centre = GetPosition();
    sphere->radius = mRadius;
}

void RectTriggerVolume::SetTransform(rmt::Matrix& m)
{
    mAxisX = m.Row(0);
    mAxisY = m.Row(1);
    mAxisZ = m.Row(2);
    SetPosition(m.Row(3));

    UpdateW2T();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// RectTriggerVolume::UpdateW2T
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RectTriggerVolume::UpdateW2T()
{
    mTrigger2World.Row(0) = mAxisX;
    mTrigger2World.Row(1) = mAxisY;
    mTrigger2World.Row(2) = mAxisZ;
    mTrigger2World.Row(3) = GetPosition();

    mTrigger2World.m[0][3] = 0.0f;
    mTrigger2World.m[1][3] = 0.0f;
    mTrigger2World.m[2][3] = 0.0f;
    mTrigger2World.m[3][3] = 0.0f;

    mWorld2Trigger = mTrigger2World;
    mWorld2Trigger.Invert();  //This is orthonormal!!!
    
    //We only use the rot/scale...
    mWorld2Trigger.IdentityTranslation();
}

//=============================================================================
// RectTriggerVolume::InitPoints
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RectTriggerVolume::InitPoints()
{
#ifndef WORLD_BUILDER
#ifndef RAD_RELEASE
MEMTRACK_PUSH_GROUP( "RectTriggerVolume" );
    numFaces = 12 * 3;
    numVerts = 8;

    verts = new(GMA_LEVEL_OTHER) pddiVector[numVerts];
    faces = new(GMA_LEVEL_OTHER) unsigned short[numFaces];

    unsigned int face = 0;

    // Joel made the sphere normals inverted, 
    // so all these normals have to be inverted too
    faces[face++] = 0;
    faces[face++] = 1;
    faces[face++] = 2;

    faces[face++] = 2;
    faces[face++] = 3;
    faces[face++] = 0;

    faces[face++] = 0;
    faces[face++] = 6;
    faces[face++] = 7;

    faces[face++] = 7;
    faces[face++] = 1;
    faces[face++] = 0;

    faces[face++] = 1;
    faces[face++] = 7;
    faces[face++] = 4;

    faces[face++] = 4;
    faces[face++] = 2;
    faces[face++] = 1;

    faces[face++] = 2;
    faces[face++] = 4;
    faces[face++] = 5;

    faces[face++] = 5;
    faces[face++] = 3;
    faces[face++] = 2;

    faces[face++] = 3;
    faces[face++] = 5;
    faces[face++] = 6;

    faces[face++] = 6;
    faces[face++] = 0;
    faces[face++] = 3;

    faces[face++] = 6;
    faces[face++] = 5;
    faces[face++] = 4;

    faces[face++] = 4;
    faces[face++] = 7;
    faces[face++] = 6;

    rAssert( static_cast< int >( face ) == numFaces );
MEMTRACK_POP_GROUP( "RectTriggerVolume" );
#endif
#endif
}

void RectTriggerVolume::CalcPoints()
{
#ifndef RAD_RELEASE
    rmt::Vector radius( GetExtentX(), GetExtentY(), GetExtentZ() );

    verts[0].Set( -radius.x, -radius.y, -radius.z );
    verts[4].Set( radius.x, radius.y, radius.z );

    // counter-clockwise from verts[0]
    verts[1].Set( verts[0].x, verts[0].y, verts[4].z );
    verts[2].Set( verts[4].x, verts[0].y, verts[4].z );
    verts[3].Set( verts[4].x, verts[0].y, verts[0].z );

    // counter-clockwise from verts[4]
    verts[5].Set( verts[4].x, verts[4].y, verts[0].z );
    verts[6].Set( verts[0].x, verts[4].y, verts[0].z );
    verts[7].Set( verts[0].x, verts[4].y, verts[4].z );

    for( unsigned int i = 0; i < 8; i++ )
    {
        mTrigger2World.Transform( verts[ i ], &verts[ i ] );
    }
#endif
}

//==============================================================================
// RectTriggerVolume::RectTriggerVolume
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RectTriggerVolume::RectTriggerVolume() : 
    mAxisX( rmt::Vector( 1.0f, 0, 0 ) ),
    mAxisY( rmt::Vector( 0, 1.0f, 0 ) ),
    mAxisZ( rmt::Vector( 0, 0, 1.0f ) ),
    mExtentX( 1.0f ),
    mExtentY( 1.0f ),
    mExtentZ( 1.0f )
{
}
