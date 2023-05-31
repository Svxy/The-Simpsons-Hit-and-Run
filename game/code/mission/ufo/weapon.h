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
#ifndef WEAPON_H
#define WEAPON_H


//===========================================================================
// Nested Includes
//===========================================================================

class DynaPhysDSG;

//===========================================================================
// Forward References
//===========================================================================

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
class Weapon : public tEntity
{
    public:
        Weapon();
        virtual ~Weapon();

        virtual bool LoadSetup( const char* statePropDSGName )=0;
        virtual void Update( float timeInMS ) = 0;
        virtual void Display() = 0;
        virtual bool IsValidTarget( DynaPhysDSG* pDSG )const = 0;
        virtual bool CanFire( DynaPhysDSG* pDSG )const = 0;
        virtual void Fire( DynaPhysDSG* pDSG ) = 0;
        virtual void SetPosition( const rmt::Vector& position ) = 0;
        
            
    protected:



    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow Weapon from being copied and assigned.
        Weapon( const Weapon& );
        Weapon& operator=( const Weapon& );

 };

inline Weapon::Weapon()
{

}
inline Weapon::~Weapon()
{

}

#endif