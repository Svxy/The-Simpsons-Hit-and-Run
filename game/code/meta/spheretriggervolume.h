//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        spheretriggervolume.h
//
// Description: Sphere Trigger Volume class
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SPHERETRIGGERVOLUME_H
#define SPHERETRIGGERVOLUME_H

//========================================
// Nested Includes
//========================================
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

class SphereTriggerVolume : public TriggerVolume
{
public:
    SphereTriggerVolume();
    SphereTriggerVolume( const rmt::Vector& centre, float radius );
    virtual ~SphereTriggerVolume();

    // Intersection information
    bool Contains( const rmt::Vector& point, float epsilon = 0.00f ) const;
    
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
    bool GetBoundingBox( rmt::Vector& p1, rmt::Vector& p2 ) const;

    float               GetSphereRadius() const;
    void                SetSphereRadius( float radius );

    virtual Type GetType() const;
    
    //////////////////////////////////////////////////////////////////////////
    // tDrawable interface
    //////////////////////////////////////////////////////////////////////////
    void GetBoundingBox(rmt::Box3D* box);
    void GetBoundingSphere(rmt::Sphere* sphere);


protected:
    virtual void InitPoints();
    virtual void CalcPoints();

private:
    bool IntersectLineSphere( const rmt::Vector& lineOrig, 
                              const rmt::Vector& lineDir ) const;

    // For sphere type
    float       mRadius;

    //Prevent wasteful constructor creation.
    SphereTriggerVolume( const SphereTriggerVolume& spheretriggervolume );
    SphereTriggerVolume& operator=( const SphereTriggerVolume& spheretriggervolume );
};

inline float SphereTriggerVolume::GetSphereRadius() const
{
    return( mRadius );
}

inline void SphereTriggerVolume::SetSphereRadius( float radius )
{
    mRadius = radius;
}

#endif //SPHERETRIGGERVOLUME_H
