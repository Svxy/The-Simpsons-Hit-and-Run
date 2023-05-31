//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AutomaticDoor
//
// Description: AutomaticDoor is an AnimSwitch (See actionbuttonhandler) where, 
//              upon entering the associated trigger volume, the animation will
//              play forward until the end. On exiting the trigger volume, the 
//              animation will play in the reverse direction
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef AUTOMATIC_H
#define AUTOMATIC_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actionbuttonhandler.h>


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
//      Behaves like a sliding door would
//
// Constraints:
//
//===========================================================================

namespace ActionButton
{

class AutomaticDoor : public AnimSwitch
{
    public:
        AutomaticDoor( ActionEventLocator* pActionEventLocator );
        virtual ~AutomaticDoor();

        static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
        {
            return new AutomaticDoor( pActionEventLocator );  
        }  

    protected:

        virtual void OnEnter( Character* pCharacter );
        virtual void OnExit( Character* pCharacter );
        virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq )  { return false; }

    protected:
        // Number of characters currently residing in the trigger volume
        int mCharacterCount;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow AutomaticDoor from being copied and assigned.
        AutomaticDoor( const AutomaticDoor& );
        AutomaticDoor& operator=( const AutomaticDoor& );

};

};
#endif