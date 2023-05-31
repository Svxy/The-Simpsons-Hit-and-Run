//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Name of subsystem or component this class belongs to.
//
// Description: This file contains...
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef TRACTORBEAM_H
#define TRACTORBEAM_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <mission/ufo/weapon.h>
#include <vector>
#include <render/culling/swaparray.h>

//===========================================================================
// Forward References
//===========================================================================

class StatePropDSG;
class IEntityDSG;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Generally, describe what behaviour this class possesses that
//      clients can rely on, and the actions that this service
//      guarantees to clients.
//
// Constraints:
//      Describe here what you require of the client which uses or
//      has this class - essentially, the converse of the description
//      above. A constraint is an expression of some semantic
//      condition that must be preserved, an invariant of a class or
//      relationship that must be preserved while the system is in a
//      steady state.
//
//===========================================================================
class TractorBeam : public Weapon
{
    public:
        TractorBeam();
        virtual ~TractorBeam();

        virtual bool LoadSetup( const char* statePropDSGName );
        virtual void Update( float timeInMS );
        virtual void Display();
        virtual bool IsValidTarget( DynaPhysDSG* pDSG )const;
        virtual bool CanFire( DynaPhysDSG* pTarget )const;
        virtual void Fire( DynaPhysDSG* pTarget );
        virtual void SetPosition( const rmt::Vector& position );

    protected:

        tName           mStatePropName;

        StatePropDSG*   mpDrawable;

        SwapArray< DynaPhysDSG* > mCaughtObjects;
        
        rmt::Matrix mTransform;
        DynaPhysDSG* mNextTarget;

    protected:
    
        // Removes the object from the world, accompanied by a particle effect
        // on state change
        void SwallowObject( DynaPhysDSG* );
        bool IsWithinBeam( const rmt::Vector& targetVector )const;

        rmt::Quaternion Slerp( const rmt::Matrix m1, const rmt::Matrix m2, float deltaTime, float angularVelocity );

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow TractorBeam from being copied and assigned.
        TractorBeam( const TractorBeam& );
        TractorBeam& operator=( const TractorBeam& );

};


#endif