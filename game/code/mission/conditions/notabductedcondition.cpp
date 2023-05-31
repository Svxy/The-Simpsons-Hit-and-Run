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

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission/conditions/notabductedcondition.h>
#include <worldsim/character/character.h>
#include <worldsim/avatarmanager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

NotAbductedCondition::NotAbductedCondition()
{

}

NotAbductedCondition::~NotAbductedCondition()
{

}

void NotAbductedCondition::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_ABDUCTED:
        {
            // Check to see if the character that was abducted was the user
            Character* character = reinterpret_cast< Character* >( pEventData );
            if ( character )
            {
                Avatar* avatar = GetAvatarManager()->FindAvatarForCharacter( character );
                if ( avatar )
                {
                    // It was the player that was sucked up
                    // this condition has failed
                    SetIsViolated( true );
                }
            }
        }
        break;

    default:
        break;
    }

    MissionCondition::HandleEvent( id, pEventData );
}

void NotAbductedCondition::OnInitialize()
{
    GetEventManager()->AddListener( this, EVENT_ABDUCTED );
}

void NotAbductedCondition::OnFinalize()
{
    GetEventManager()->RemoveAll( this );
}