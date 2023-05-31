//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   NotAbductedCondition
//
// Description: Mission condition - player must not have been abducted by aliens
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef NOTABDUCTEDCONDITION_H
#define NOTABDUCTEDCONDITION_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <mission/conditions/missioncondition.h>

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
//      If the player is sucked into the UFO this condition will be false
//
// Constraints:
//
//
//===========================================================================
class NotAbductedCondition : public MissionCondition
{
    public:

        NotAbductedCondition();
        virtual ~NotAbductedCondition();

    protected:
        
        virtual void HandleEvent( EventEnum id, void* pEventData );
        virtual void OnInitialize();
        virtual void OnFinalize();

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow NotAbductedCondition from being copied and assigned.
        NotAbductedCondition( const NotAbductedCondition& );
        NotAbductedCondition& operator=( const NotAbductedCondition& );     
};


#endif