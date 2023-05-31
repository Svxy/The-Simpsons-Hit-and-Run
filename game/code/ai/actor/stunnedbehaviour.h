//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StunnedBehaviour
//
// Description: Handles being stunned
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef STUNNEDBEHAVIOUR_H
#define STUNNEDBEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/behaviour.h>


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
//
// Constraints:
//
//===========================================================================
class StunnedBehaviour : public Behaviour
{
    public:
        // Ctor - stun time in seconds
        StunnedBehaviour( float stunTime );
        virtual ~StunnedBehaviour();
        virtual void Apply( Actor*, unsigned int timeInMS );

    protected:
        
        // Time to be stunned in milliseconds
        float m_StunTime;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow StunnedBehaviour from being copied and assigned.
        StunnedBehaviour( const StunnedBehaviour& );
        StunnedBehaviour& operator=( const StunnedBehaviour& );

    private:
    
   
};



#endif