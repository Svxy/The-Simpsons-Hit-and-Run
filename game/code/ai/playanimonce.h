//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   PlayAnimOnce
//
// Description: PlayAnimOnce is an AnimSwitch (See actionbuttonhandler) where, button
//              button press, the associated animation (AnimCollDSG object) is played
//              once, then the animation stops and the button disappears
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef PLAYANIMONCE_H
#define PLAYANIMONCE_H


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
//      Nearly identical to PlayAnim, PlayAnimOnce will remove the actionbuttonhandler
//      after button press, so that the user can't press it again
//
// Constraints:
//
//===========================================================================

namespace ActionButton
{

class PlayAnimOnce : public PlayAnim
{
    public:
        PlayAnimOnce( ActionEventLocator* pActionEventLocator );
        virtual ~PlayAnimOnce();
        virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
        virtual void PositionCharacter( Character* pCharacter, Sequencer* pSeq );

        static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
        {
            return new PlayAnimOnce( pActionEventLocator );  
        }  

    protected:

        bool    mWasPressed : 1;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow PlayAnimOnce from being copied and assigned.
        PlayAnimOnce( const PlayAnimOnce& );
        PlayAnimOnce& operator=( const PlayAnimOnce& );

};

};
#endif