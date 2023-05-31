//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   VirtualInputs
//
// Description: Implementation of the virtual input class.
//
// History:     03/28/2003 + Created -- Ziemek
//
//===========================================================================

#include <input/virtualinputs.hpp>
#include <input/inputmanager.h>

static const char* szVirtualInputs[] =
{
    "MoveUp",
    "MoveDown",
    "MoveLeft",
    "MoveRight",
    "Attack",
    "Jump",
    "Sprint",
    "DoAction",

    "Accelerate",
    "Reverse",
    "SteerLeft",
    "SteerRight",
    "GetOutCar",
    "HandBrake",
    "Horn",
    "ResetCar",

    "CameraLeft",
    "CameraRight",
    "CameraMoveIn",
    "CameraMoveOut",
    "CameraZoom",
    "CameraLookUp",
    "CameraCarLeft",
    "CameraCarRight",
    "CameraCarLookUp",
    "CameraCarLookBack",
    "CameraToggle",
                            
    "feBack",               // Do not allow the user to configure any fe buttons!
    "feMoveUp",             // These are standard, unconfigurable
    "feMoveDown",
    "feMoveLeft",
    "feMoveRight",
    "feSelect",
    "feFunction1",
    "feFunction2",
    "feMouseLeft",
    "feMouseRight",
    "feMouseUp",
    "feMouseDown",

    "P1_KBD_Start",
    "P1_KBD_Gas",
    "P1_KBD_Brake",
    "P1_KBD_EBrake",
    "P1_KBD_Nitro",
    "P1_KBD_Left",
    "P1_KBD_Right"
};

//==============================================================================
// VirtualInputs::GetName
//==============================================================================
//
// Description: Returns the name of the given input
//
// Parameters:	virtualinput - input to look up
//
// Return:      name of input
//
//==============================================================================

const char* VirtualInputs::GetName( int VirtualInput )
{
    rAssert( VirtualInput >= 0 && VirtualInput < GetNumber() );

    return szVirtualInputs[ VirtualInput ];
}

//==============================================================================
// VirtualInputs::GetType
//==============================================================================
//
// Description: Returns the type of a virtual input
//
// Parameters:	virtualinput - input to look up
//
// Return:      type of input
//
//==============================================================================

eMapType VirtualInputs::GetType( int VirtualInput )
{
    rAssert( VirtualInput >= 0 && VirtualInput < GetNumber() );

    switch( VirtualInput )
    {
        case InputManager::MoveUp:
        case InputManager::MoveDown:
        case InputManager::MoveLeft:
        case InputManager::MoveRight:
        case InputManager::Attack:
        case InputManager::Jump:
        case InputManager::Sprint:
        case InputManager::DoAction:
        case InputManager::CameraLeft:
        case InputManager::CameraRight:
        case InputManager::CameraMoveIn:
        case InputManager::CameraMoveOut:
        case InputManager::CameraZoom:
        case InputManager::CameraLookUp:
            return MAP_CHARACTER;
        case InputManager::Accelerate:
        case InputManager::Reverse:
        case InputManager::SteerLeft:
        case InputManager::SteerRight:
        case InputManager::GetOutCar:
        case InputManager::HandBrake:
        case InputManager::Horn:
        case InputManager::ResetCar:
        case InputManager::CameraCarLeft:
        case InputManager::CameraCarRight:
        case InputManager::CameraCarLookUp:
        case InputManager::CameraCarLookBack:
        case InputManager::CameraToggle:
            return MAP_VEHICLE;
        case InputManager::feBack:
        case InputManager::feMoveUp:
        case InputManager::feMoveDown:
        case InputManager::feMoveLeft:
        case InputManager::feMoveRight:
        case InputManager::feSelect:
        case InputManager::feFunction1:
        case InputManager::feFunction2:
        case InputManager::feMouseLeft:
        case InputManager::feMouseRight:
        case InputManager::feMouseUp:
        case InputManager::feMouseDown:
            return MAP_FRONTEND;
        default:
            {
                if ( VirtualInput >= InputManager::P1_KBD_Start && VirtualInput <= InputManager::P1_KBD_Right )
                {
                    return MAP_FRONTEND;
                }
                else
                {
                    return MAP_CHARACTER;
                }
            }
    }
}
//==============================================================================
// VirtualInputs::GetNumber
//==============================================================================
//
// Description: Returns the number of virtual inputs
//
// Parameters:	n/a
//
// Return:      number of virtual inputs
//
//==============================================================================

int VirtualInputs::GetNumber()
{
    static int NumVirtualInputs = sizeof( szVirtualInputs ) / sizeof( szVirtualInputs[0] );

    return NumVirtualInputs;
}
