#include <worldsim/character/charactermappable.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactercontroller.h>
#include <p3d/camera.hpp>

#include <input/inputmanager.h>
#include <input/usercontrollerwin32.h>

//
// Temp
#include <worldsim/avatarmanager.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>


// Constructor.
//
/*
==============================================================================
CharacterMappable::CharacterMappable
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterMappable

=============================================================================
*/
CharacterMappable::CharacterMappable( void )
:
Mappable(Input::ACTIVE_GAMEPLAY),
mpCharacterController( 0 )
{
}

// Destructor
//
/*
==============================================================================
CharacterMappable::~CharacterMappable
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterMappable

=============================================================================
*/
CharacterMappable::~CharacterMappable( void )
{
    if ( mpCharacterController )
    {
        mpCharacterController->Release( );
        mpCharacterController = 0;
    }
}
/*
==============================================================================
CharacterMappable::GetCharacterController
==============================================================================
Description:    Comment

Parameters:     ()

Return:         CharacterController

=============================================================================
*/
CharacterController* CharacterMappable::GetCharacterController() const
{
    return mpCharacterController;
}

void CharacterMappable::SetCharacterController( CharacterController* pCharacterController )
{
    tRefCounted::Assign( mpCharacterController, pCharacterController );
}
// This method is called when ever a button state changes.
//void
void CharacterMappable::OnButton( int controllerId, int id, const IButton* pButton )
{
}

// This method is called when a button changes state from "Pressed" to "Released".
//
void CharacterMappable::OnButtonUp( int controllerId, int buttonId, const IButton* pButton )
{
}

// This method is called when a button changes state from "Released" to "Pressed".
//
void CharacterMappable::OnButtonDown( int controllerId, int buttonId, const IButton* pButton )
{
}

// This is how we create our controller device mappings to logical game mappings.
// The mappings set up in this method are platform specific.
//
// The basic format of the calls is to "Map" a input, to a enumerated output id.
// The output of the specified input will be contained in the Button[] array.
// This id will also be sent as a the second parameter in the OnButton... messages.
//
void CharacterMappable::LoadControllerMappings( unsigned int controllerId )
{
#ifdef RAD_XBOX
	ClearMap( 0 );
	Map( "LeftStickX", CharacterController::LeftStickX, 0, controllerId );
    Map( "LeftStickY", CharacterController::LeftStickY, 0, controllerId );
    Map( "DPadUp", CharacterController::DPadUp, 0, controllerId );
    Map( "DPadDown", CharacterController::DPadDown, 0, controllerId );
    Map( "DPadLeft", CharacterController::DPadLeft, 0, controllerId );
    Map( "DPadRight", CharacterController::DPadRight, 0, controllerId );
    Map( "Y", CharacterController::DoAction, 0, controllerId );
    Map( "A", CharacterController::Jump, 0, controllerId );
    Map( "B", CharacterController::Dash, 0, controllerId );
    Map( "X", CharacterController::Attack, 0, controllerId );
#endif
#ifdef RAD_PS2
	ClearMap( 0 );
	Map( "LeftStickX", CharacterController::LeftStickX, 0, controllerId );
    Map( "LeftStickY", CharacterController::LeftStickY, 0, controllerId );
    Map( "DPadUp", CharacterController::DPadUp, 0, controllerId );
    Map( "DPadDown", CharacterController::DPadDown, 0, controllerId );
    Map( "DPadLeft", CharacterController::DPadLeft, 0, controllerId );
    Map( "DPadRight", CharacterController::DPadRight, 0, controllerId );
    Map( "Triangle", CharacterController::DoAction, 0, controllerId );
    Map( "X", CharacterController::Jump, 0, controllerId );
    Map( "Circle", CharacterController::Dash, 0, controllerId );
    Map( "Square", CharacterController::Attack, 0, controllerId );
#endif
#ifdef RAD_GAMECUBE
	ClearMap( 0 );
	Map( "LeftStickX", CharacterController::LeftStickX, 0, controllerId );
    Map( "LeftStickY", CharacterController::LeftStickY, 0, controllerId );
    Map( "DPadUp", CharacterController::DPadUp, 0, controllerId );
    Map( "DPadDown", CharacterController::DPadDown, 0, controllerId );
    Map( "DPadLeft", CharacterController::DPadLeft, 0, controllerId );
    Map( "DPadRight", CharacterController::DPadRight, 0, controllerId );
    Map( "Y", CharacterController::DoAction, 0, controllerId );
    Map( "A", CharacterController::Jump, 0, controllerId );
    Map( "X", CharacterController::Dash, 0, controllerId );
    Map( "B", CharacterController::Attack, 0, controllerId );
#endif
#ifdef RAD_WIN32
    ClearMap( 0 );
    Map( "MoveUp", CharacterController::DPadUp, 0, controllerId );
    Map( "MoveDown", CharacterController::DPadDown, 0, controllerId );
    Map( "MoveLeft", CharacterController::DPadLeft, 0, controllerId );
    Map( "MoveRight", CharacterController::DPadRight, 0, controllerId );
    Map( "DoAction", CharacterController::DoAction, 0, controllerId );
    Map( "GetOutCar", CharacterController::GetOutCar, 0, controllerId );
    Map( "Jump", CharacterController::Jump, 0, controllerId );
    Map( "Sprint", CharacterController::Dash, 0, controllerId );
    Map( "Attack", CharacterController::Attack, 0, controllerId );
    Map( "feMouseRight", CharacterController::MouseLookRight, 0, controllerId );
    Map( "feMouseLeft", CharacterController::MouseLookLeft, 0, controllerId );
#endif
}

void CharacterMappable::GetDirection( rmt::Vector& outDirection ) const
{
}

BipedCharacterMappable::BipedCharacterMappable( void )
:
CharacterMappable( )
{
}

BipedCharacterMappable::~BipedCharacterMappable( )
{
}
void BipedCharacterMappable::OnButtonDown( int controllerId, int buttonId, const IButton* pButton )
{
    CharacterMappable::OnButtonDown( controllerId, buttonId, pButton );

    switch ( buttonId )
    {
    case CharacterController::DoAction:
        {
            GetCharacterController()->SetIntention( CharacterController::DoAction );
            break;
        }
    case CharacterController::Jump:
        {
            GetCharacterController()->SetIntention( CharacterController::Jump );
            break;
        }
    case CharacterController::Dash:
        {
            GetCharacterController()->SetIntention( CharacterController::Dash );
            break;
        }
    case CharacterController::Attack:
        {
            GetCharacterController()->SetIntention( CharacterController::Attack );
            break;
        }
    default:
        {
            break;
        }
    }
}
void BipedCharacterMappable::GetDirection( rmt::Vector& outDirection ) const
{
#ifdef RAD_WIN32
    if ( GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam()->GetType() == SuperCam::PC_CAM ) //Mouse look enabled
    {
        float right = GetValue( CharacterController::MouseLookRight );
        float left = GetValue( CharacterController::MouseLookLeft );
        outDirection.x = ( right > left ) ? right : -left;

        float dirPad = GetValue( CharacterController::DPadUp ) - GetValue( CharacterController::DPadDown );
        float dirAnalog = GetValue( CharacterController::LeftStickY );

        outDirection.z = rmt::Fabs( dirPad ) > rmt::Fabs( dirAnalog ) ? dirPad : dirAnalog;
    }
    else
    {
#endif
    rmt::Vector tempDir;
    tempDir.x = GetValue( CharacterController::LeftStickX );
    tempDir.y = 0.0f;
	tempDir.z = GetValue( CharacterController::LeftStickY );

    rmt::Vector tempDir2;
    tempDir2.x = GetValue( CharacterController::DPadRight ) - GetValue( CharacterController::DPadLeft );
    tempDir2.y = 0.0f;
    tempDir2.z = GetValue( CharacterController::DPadUp ) - GetValue( CharacterController::DPadDown );

    //The DPad overrides the analog stick.
    outDirection = tempDir2.MagnitudeSqr() != 0.0f ? tempDir2 : tempDir;
#ifdef RAD_WIN32
    }
#endif
}

InCarCharacterMappable::InCarCharacterMappable( void )
:
CharacterMappable( )
{
}

InCarCharacterMappable::~InCarCharacterMappable( )
{
}

void InCarCharacterMappable::OnButtonDown( int controllerId, int buttonId, const IButton* pButton )
{
    CharacterMappable::OnButtonDown( controllerId, buttonId, pButton );

    switch ( buttonId )
    {
    case CharacterController::DoAction:
        {
            GetCharacterController()->SetIntention( CharacterController::DoAction );
            break;
        }
#ifdef RAD_WIN32
    case CharacterController::GetOutCar:
        {
            GetCharacterController()->SetIntention( CharacterController::GetOutCar );
            break;
        }
#endif
    default:
        {
            break;
        }
    }
}

void InCarCharacterMappable::GetDirection( rmt::Vector& outDirection ) const
{
    outDirection.Set( 0.0f, 0.0f, 0.0f );
}
