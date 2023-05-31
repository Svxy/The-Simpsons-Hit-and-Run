//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ufobeamalwayson
//
// Description: code for the ai (if you can call it that for l7m3)
//              UFO stays in one spot and its beam is always on
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef UFOBEAMONBEHAVIOUR_H
#define UFOBEAMONBEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai\actor\ufobehaviour.h>

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

//===========================================================================
//
// Description: 
//      Actor never moves. Beam is on
//
// Constraints:
//
//
//===========================================================================
class UFOBeamAlwaysOn : public UFOBehaviour
{
    public:
        UFOBeamAlwaysOn();
        virtual ~UFOBeamAlwaysOn();

        virtual void Apply( Actor*, unsigned int timeInMS );
        
    protected:

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow UFOBeamAlwaysOn from being copied and assigned.
        UFOBeamAlwaysOn( const UFOBeamAlwaysOn& );
        UFOBeamAlwaysOn& operator=( const UFOBeamAlwaysOn& );


};


#endif