//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundcollisiondata.h
//
// Description: Declaration for SoundCollisionData structure, which carries
//              collision information through the event manager to the sound
//              system
//
// History:     04/08/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDCOLLISIONDATA_H
#define SOUNDCOLLISIONDATA_H

//========================================
// Nested Includes
//========================================
#include <render/DSG/collisionentitydsg.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    SoundCollisionData
//
//=============================================================================

class SoundCollisionData 
{
    public:
        SoundCollisionData( float intensity, CollisionEntityDSG* objA, CollisionEntityDSG* objB )
        {
            //impulse = vector;
            mIntensity = intensity;
            collObjA = objA;
            collObjB = objB;
        }
        ~SoundCollisionData() {}

        float mIntensity;    // 0.0f - 1.0f
        CollisionEntityDSG* collObjA;
        CollisionEntityDSG* collObjB;

    private:
        //
        // In this case, allow the shallow-copy assignment and copy constructors.
        // We don't want the default constructor, though.
        //
        SoundCollisionData();
};


#endif // SOUNDCOLLISIONDATA_H

