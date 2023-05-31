//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        recttriggervolume.h
//
// Description: Blahblahblah
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef RECTTRIGGERVOLUME_H
#define RECTTRIGGERVOLUME_H

//========================================
// Nested Includes
//========================================
#include <raddebugwatch.hpp>
#include <radmath/radmath.hpp>

#ifndef WORLD_BUILDER
#include <meta/triggervolume.h>
#else
#include "triggervolume.h"
#endif
//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RectTriggerVolume : public TriggerVolume
{
public:
    RectTriggerVolume( const rmt::Vector& center, 
                       const rmt::Vector& axisX, 
                       const rmt::Vector& axisY, 
                       const rmt::Vector& axisZ, 
                       float extentX, 
                       float extentY, 
                       float extentZ );
    virtual ~RectTriggerVolume();


    // Intersection information
    bool Contains ( const rmt::Vector& point, float epsilon = 0.00f ) const;
    
    // Intersection of bounding box with trigger volume.
    //p1 - p4 are the four corners of the box.
    bool IntersectsBox( const rmt::Vector& p1, 
                        const rmt::Vector& p2, 
                        const rmt::Vector& p3, 
                        const rmt::Vector& p4 ) const;

    virtual bool IntersectsBox( const rmt::Box3D& box );

    bool IntersectsSphere( const rmt::Vector& position, 
                           float radius ) const;

    bool IntersectLine( const rmt::Vector& p1,
                        const rmt::Vector& p2 ) const;

    // Bounding box
    bool GetBoundingBox ( rmt::Vector& p1, rmt::Vector& p2 ) const;

    Type GetType() const;

    void SetTransform(rmt::Matrix&);

    const rmt::Vector& GetAxisX() const;
    const rmt::Vector& GetAxisY() const;
    const rmt::Vector& GetAxisZ() const;

    float   GetExtentX() const;
    void    SetExtentX( float extent );
    
    float   GetExtentY() const;
    void    SetExtentY( float extent );

    float   GetExtentZ() const;
    void    SetExtentZ( float extent );

    const rmt::Matrix& GetW2T();
    const rmt::Matrix& GetT2W();

    //////////////////////////////////////////////////////////////////////////
    // tDrawable interface
    //////////////////////////////////////////////////////////////////////////
    void GetBoundingBox(rmt::Box3D* box);
    void GetBoundingSphere(rmt::Sphere* sphere);
protected:
    virtual void InitPoints();
    virtual void CalcPoints();

private:
    RectTriggerVolume();
    bool IntersectLineRect(  const rmt::Vector& lineOrig, 
                             const rmt::Vector& lineDir ) const;

    void UpdateW2T();

    rmt::Vector mAxisX;
    rmt::Vector mAxisY;
    rmt::Vector mAxisZ;

    float mExtentX;
    float mExtentY;
    float mExtentZ;

    float mRadius;

    rmt::Matrix mWorld2Trigger;
    rmt::Matrix mTrigger2World;

    //Prevent wasteful constructor creation.
    RectTriggerVolume( const RectTriggerVolume& recttriggervolume );
    RectTriggerVolume& operator=( const RectTriggerVolume& recttriggervolume );
};

//=============================================================================
// RectTriggerVolume::SetExtentX
//=============================================================================
// Description: Comment
//
// Parameters:  ( float extent )
//
// Return:      inline 
//
//=============================================================================
inline void RectTriggerVolume::SetExtentX( float extent )
{
    mExtentX = extent;
}

//=============================================================================
// RectTriggerVolume::SetExtentY
//=============================================================================
// Description: Comment
//
// Parameters:  ( float extent )
//
// Return:      inline 
//
//=============================================================================
inline void RectTriggerVolume::SetExtentY( float extent )
{
    mExtentY = extent;
}

//=============================================================================
// RectTriggerVolume::SetExtentZ
//=============================================================================
// Description: Comment
//
// Parameters:  ( float extent )
//
// Return:      inline 
//
//=============================================================================
inline void RectTriggerVolume::SetExtentZ( float extent )
{
    mExtentZ = extent;
}

//=============================================================================
// RectTriggerVolume::GetExtentX
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline float RectTriggerVolume::GetExtentX() const
{
    return( mExtentX );
}

//=============================================================================
// RectTriggerVolume::GetExtentY
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline float RectTriggerVolume::GetExtentY() const
{
    return( mExtentY );
}

//=============================================================================
// RectTriggerVolume::GetExtentZ
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline float RectTriggerVolume::GetExtentZ() const
{
    return( mExtentZ );
}

#endif //RECTTRIGGERVOLUME_H
