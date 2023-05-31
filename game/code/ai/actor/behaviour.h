//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   none
//
// Description: Interface for actor behaviours
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/refcounted.hpp>

//===========================================================================
// Forward References
//===========================================================================

class Actor;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

class Behaviour : public tRefCounted
{
    public:
        Behaviour():m_IsMutuallyExclusive( false ){ }
        virtual ~Behaviour() { }

        virtual void Apply( Actor*, unsigned int timeInMS )=0;
        
        // Can this behaviour be deactivated? or are we busy
        virtual bool IsExclusive()const            { return m_IsMutuallyExclusive; }
    
        // Enable this behaviour
        virtual void Activate(){}
        // Disable this behaviour
        virtual void Deactivate(){}

    protected:
        virtual void SetExclusive( bool exclusive ) { m_IsMutuallyExclusive = exclusive; }
        bool m_IsMutuallyExclusive;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow Behaviour from being copied and assigned.
        Behaviour( const Behaviour& );
        Behaviour& operator=( const Behaviour& );

};


#endif