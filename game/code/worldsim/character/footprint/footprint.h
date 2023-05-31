//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   footprint
//
// Description: Simple class inherited from tDrawable. Draws a foot print
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef FOOTPRINT_H
#define FOOTPRINT_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d\drawable.hpp>

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
class Footprint : public tDrawable
{
    public:
        Footprint();
        virtual ~Footprint();
            
        virtual void Display();   


    protected:

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow Footprint from being copied and assigned.
        Footprint( const Footprint& );
        Footprint& operator=( const Footprint& );

};



#endif