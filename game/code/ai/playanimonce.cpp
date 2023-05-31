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


//---------------------------------------------------------------------------
// Includes
//===========================================================================
#include <p3d/anim/multicontroller.hpp>
#include <ai/playanimonce.h>
#include <ai/sequencer/action.h>
#include <ai/sequencer/sequencer.h>
#include <ai/actionbuttonmanager.h>
#include <meta/actioneventlocator.h>
#include <meta/triggervolumetracker.h>
#include <events/eventmanager.h>
#include <events/eventdata.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================




//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// PlayAnimOnce::PlayAnimOnce
//===========================================================================
// Description:
//      PlayAnimOnce - ctor
//
// Constraints:
//      Needs certain things in the inventory
//
// Parameters:
//      ActionEventLocator, can't be NULL
//
// Return:
//      
//
//===========================================================================

using namespace ActionButton;

PlayAnimOnce::PlayAnimOnce( ActionEventLocator* pActionEventLocator )
: PlayAnim( pActionEventLocator ),
mWasPressed( false )
{
        
}

//===========================================================================
// PlayAnimOnce::~PlayAnimOnce
//===========================================================================
// Description:
//      PlayAnimOnce - dtor
//
// Constraints:
//
// Parameters:
//
// Return:
//      
//
//===========================================================================
PlayAnimOnce::~PlayAnimOnce()
{
    //
    // Make good and sure this thing isn't going to play sound anymore
    //
    GetEventManager()->TriggerEvent( EVENT_STOP_ANIMATION_SOUND, this );
}

bool PlayAnimOnce::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    bool success;
    if ( mWasPressed == false )
    {
        // Don't think, play the animation, then remove this actionbuttonhandler from the manager

        // Force the animation to be NOT cyclic.
        //
        tMultiController* pAnimController = GetAnimController();

        pAnimController->SetCycleMode( FORCE_NON_CYCLIC );
        pAnimController->SetFrame( 0.0f );
        
        // Action will assign this value which will start the animation.
        //
        Action* pAction = 0;
        float fDirection = 1.0f;
        pSeq->BeginSequence();
        pAction = new AssignValueToFloat( GetAnimationDirection( ), fDirection );
        pSeq->AddAction( pAction );
        pSeq->EndSequence( );

        // Disallow the pressing of this button ever again
        mWasPressed = true;
        for (unsigned int i = 0 ; i < GetActionEventLocator()->GetNumTriggers() ; i++)
        {
            TriggerVolume* pTriggerVolume = GetActionEventLocator()->GetTriggerVolume( i );
            GetTriggerVolumeTracker()->RemoveTrigger( pTriggerVolume );  
        }
        if( mSoundName != NULL )
        {
            AnimSoundData data( mSoundName, mSettingsName );

            GetEventManager()->TriggerEvent( EVENT_START_ANIMATION_SOUND, &data );
        }
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}

//===========================================================================
// PlayAnimOnce::PositionCharacter
//===========================================================================
// Description:
//      PositionCharacter
//
// Constraints:
//
// Parameters:
//
// Return:
//      
//
//===========================================================================

void 
PlayAnimOnce::PositionCharacter( Character* pCharacter, Sequencer* pSeq )
{
    if ( mWasPressed == false )
    {
        AnimSwitch::PositionCharacter( pCharacter, pSeq );
    }
}



