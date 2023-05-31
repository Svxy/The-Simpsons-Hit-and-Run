//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     14/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

#include <radmath/radmath.hpp>
#include <p3d/drawable.hpp>

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WorldObject : public tDrawable
{
    public:
        WorldObject( tDrawable* drawable );
        virtual ~WorldObject();

        void SetPosition( rmt::Vector &position );

        virtual void Display();

        tDrawable* GetDrawable() { return( mDrawable ); }
    private:
        //Prevent wasteful constructor creation.
        WorldObject( const WorldObject& worldObject );
        WorldObject& operator=( const WorldObject& worldObject );

        tDrawable* mDrawable;
        rmt::Matrix mTransform;
};


// Hack name for inventory section to hold sim library caches of SkeletonInfo.
// Tracked to ATG as bug 1259.
//
const char SKELCACHE[] = "SkeletonInfoCache";


#endif // WORLDOBJECT_H

