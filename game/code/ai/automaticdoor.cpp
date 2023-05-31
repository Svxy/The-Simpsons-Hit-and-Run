#include <ai/automaticdoor.h>
#include <events/eventmanager.h>
#include <events/eventdata.h>
#include <p3d/anim/multicontroller.hpp>

ActionButton::AutomaticDoor::AutomaticDoor( ActionEventLocator* pActionEventLocator ) 
: AnimSwitch( pActionEventLocator ),
mCharacterCount( 0 )
{
    SetActionButton( CharacterController::NONE );    
}

ActionButton::AutomaticDoor::~AutomaticDoor()
{
    //
    // Make good and sure this thing isn't going to play sound anymore
    //
    GetEventManager()->TriggerEvent( EVENT_STOP_ANIMATION_SOUND, this );
}

void ActionButton::AutomaticDoor::OnEnter( Character* pCharacter )
{
    if ( mCharacterCount == 0 )
    {
        tMultiController* pAnimController = GetAnimController( );

        // This will start the animation.
        //
        SetAnimationDirection( 1.0f );

        // Bit of a sanity check. Automatic doors shouldn't door crazy and start looping
        pAnimController->SetCycleMode( FORCE_NON_CYCLIC );

        if( mSoundName != NULL )
        {
            AnimSoundData data( mSoundName, mSettingsName );
            GetEventManager()->TriggerEvent( EVENT_START_ANIMATION_SOUND, &data );
        }
    }
    mCharacterCount++;
}

void ActionButton::AutomaticDoor::OnExit( Character* pCharacter )
{
    mCharacterCount--;
    if ( mCharacterCount == 0 )
    {
        tMultiController* pAnimController = GetAnimController( );      
        // The last person has left the trigger volume, shut the door by reversing the 
        // animation
        SetAnimationDirection( -1.0f );
    }
}


