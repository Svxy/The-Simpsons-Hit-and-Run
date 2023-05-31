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
#ifndef UFO_H
#define UFO_H


//===========================================================================
// Nested Includes
//===========================================================================
#include <p3d/refcounted.hpp>
#include <radmath/radmath.hpp>
#include <p3d/p3dtypes.hpp>

#include <vector>
#include <render/culling/ReserveArray.h>
#include <mission/boss.h>
#include <events/eventlistener.h>

//===========================================================================
// Forward References
//===========================================================================

class Weapon;
class StatePropDSG;
class DynaPhysDSG;

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
/*
class UFOState
{
public:
    UFOState();
    ~UFOState();

    bool IsStateCompleted()=0;
    void Update( float timeInMS )=0;

private:
};

class UFOMovingState
{
public:
    UFOMovingState();
    ~UFOMovingState();

    bool IsStateCompleted();
    void Update( float timeInMS );
    void SetTarget( const rmt::Vector& target );
    
    
private:
    // Where the UFO should move to next
    rmt::Vector mMoveTo;    
};

class UFOID4AttackState
{
public:
    UFOID4AttackState();
    ~UFOID4AttackState();

private:
};*/

struct UFOWeapon
{
    Weapon* weapon;
    rmt::Vector offset;
};


class UFO : public Boss, public EventListener
{
    public:
        UFO();
        virtual ~UFO();

        virtual void Update( float timeInMS );
        virtual bool LoadSetup( const char* statePropDSGName, const rmt::Vector& position );

        virtual void AddWeapon( Weapon* pWeapon, const rmt::Vector& offset );
        
        void HandleEvent( EventEnum id, void* pEventData );


        enum State { eScanning, eMoving, eID4Attack, eTractorBeam };

    protected:

        std::vector< UFOWeapon, s2alloc<UFOWeapon> > mWeaponList;
        StatePropDSG*   mpDrawable;


        ReserveArray< DynaPhysDSG* > mTargets;
        tName mStatePropName;

    protected:
        
        void ScanForTargets();
        bool IsStateCompleted();
        UFO::State ChooseNextState();

        // Figure out the correct acceleration to get to where we want to go
        // Apply the acceleration to the velocity
        void Navigate( float timeInMS );
        // Apply velocity and move the object, also take care of the DSG move call
        void Move( float timeInMS ); 



   protected:

        // Wish I could reuse some of the vehicle stuff for this.
        rmt::Vector mVelocity;
        rmt::Vector mPosition;
        
        DynaPhysDSG* mTarget;
        

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow UFO from being copied and assigned.
        UFO( const UFO& );
        UFO& operator=( const UFO& );
};



#endif