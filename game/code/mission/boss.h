//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Name of subsystem or component this class belongs to.
//
// Description: This file contains...
//
// Authors:     Name Here
//
//===========================================================================

// Recompilation protection flag.
#ifndef BOSS_H
#define BOSS_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d\entity.hpp>

//===========================================================================
// Forward References
//===========================================================================

class Weapon;


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
class Boss : public tEntity
{
    public:
        Boss();
        virtual ~Boss();

        virtual void Update( float timeInMS )=0;
        virtual bool LoadSetup( const char* statePropDSGName, const rmt::Vector& position )=0;
        virtual void AddWeapon( Weapon* pWeapon, const rmt::Vector& offset )=0;

    protected:
        


    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow Boss from being copied and assigned.
        Boss( const Boss& );
        Boss& operator=( const Boss& );

    
};

inline Boss::Boss()
{

}
inline Boss::~Boss()
{

}

#endif