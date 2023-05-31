#ifndef ACTION_LIST_H_
#define ACTION_LIST_H_

#include <ai/actionbuttonhandler.h>
#include <ai/playanimonce.h>
#include <ai/actionnames.h>
#include <ai/automaticdoor.h>

#include <radkey.hpp>

class EventLocator;

namespace ActionButton
{
    // Create a function ptr type for easy handling of new actions.
    //
    typedef ButtonHandler* (*newActionPtr)( ActionEventLocator* pActionEventLocator);


    // Keep the names around in debug mode.
    //
    #ifdef RAD_DEBUG
        #define MAKE_ACTION_KEY( s ) s
        typedef const char* ACTION_KEY;
        bool CompareActionType( const char* typeName, const char* actionName )
        {
            return ( ::radMakeCaseInsensitiveKey( typeName ) == ::radMakeCaseInsensitiveKey( actionName ) );
        }
    #else
        #define MAKE_ACTION_KEY( s ) radMakeCaseInsensitiveKey( s )
        typedef radInt64 ACTION_KEY;
        bool CompareActionType( const char* typeName, tUID actionUID )
        {
            return ( ::radMakeCaseInsensitiveKey( typeName ) == actionUID );
        }
    #endif

    // A key, function ptr pair.
    //
    struct ActionIndex
    {
        ACTION_KEY mActionKey;
        newActionPtr actionPtr;
    };

    // Add entries to this table when you want to create new actions.
    //
    static int nameIndex = 0;
    ActionIndex theListOfActions[ ] =
    {
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), ToggleAnim::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), ReverseAnim::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), PlayAnim::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), PlayAnimLoop::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), AutoPlayAnim::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), AutoPlayAnimLoop::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), AutoPlayAnimInOut::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), DestroyObject::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), DestroyObject::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), UseVendingMachine::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), PrankPhone::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), SummonVehiclePhone::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), Doorbell::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), OpenDoor::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), TalkFood::NewAction },
        // NB switch to TalkCollectible::NewAction
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), CollectibleCard::NewAction },  
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), TalkDialog::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), TalkMission::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), CollectibleFood::NewFoodSmallAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), CollectibleFood::NewFoodLargeAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), CollectibleCard::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), DestroyObject::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), PlayAnimOnce::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), AutomaticDoor::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), WrenchIcon::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), TeleportAction::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), PurchaseCar::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), PurchaseSkin::NewAction },
        { MAKE_ACTION_KEY( ActionName[ nameIndex++ ] ), NitroIcon::NewAction }

    };

    const int ActionListSize = ( sizeof(ActionButton::theListOfActions) / sizeof(ActionButton::theListOfActions[0]) );  
};
#endif