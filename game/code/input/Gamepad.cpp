//**********************************************************
// C++ Class Name : Gamepad 
// ---------------------------------------------------------
// Filetype: (SOURCE)
// Filepath: D:/simpsonsrr2/game/code/input/Gamepad.cpp 
// 
// 
// GDPro Properties 
// ---------------------------------------------------
//  - GD Symbol Type    : CLD_Class 
//  - GD Method         : UML ( 5.0 ) 
//  - GD System Name    : Simpsons Controller System 
//  - GD Diagram Type   : Class Diagram 
//  - GD Diagram Name   : Input Device 
// ---------------------------------------------------  
//  Author         : nharan 
//  Creation Date  : Tues - May 20, 2003 
// 
//  Change Log     : 
// 
//**********************************************************
#include <input/Gamepad.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//==============================================================================
// Gamepad constructor
//==============================================================================
//
// Description: Creates an empty gamepad.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================

Gamepad::Gamepad()
: RealController( GAMEPAD )
{
    ClearMappedButtons();
}

//==============================================================================
// Gamepad destructor
//==============================================================================
//
// Description: Destroy the gamepad.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================

Gamepad::~Gamepad()
{
}

//==============================================================================
// Gamepad::IsInputAxis
//==============================================================================
//
// Description: Identifies any keys that are input axes. (can go in two directions)
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if input is an axis
//
//==============================================================================

bool Gamepad::IsInputAxis( int dxKey ) const
{
    switch( dxKey )
    {
        case DIJOFS_X:
        case DIJOFS_Y:
        case DIJOFS_Z:
        case DIJOFS_RX:
        case DIJOFS_RY:
        case DIJOFS_RZ:
        case DIJOFS_SLIDER(0):
        case DIJOFS_SLIDER(1):
            GetButtonEnum( dxKey );
            return true;
        default:
            return false;
    }
    
}

//==============================================================================
// Gamepad::IsPovHat
//==============================================================================
//
// Description: Identifies any keys that are pov hats (go in 4 directions)
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if input is a pov hat
//
//==============================================================================

bool Gamepad::IsPovHat( int dxKey ) const
{
    switch( dxKey )
    {
        case DIJOFS_POV(0):
        case DIJOFS_POV(1):
            return true;
        default:
            return false;
    }
}

//==============================================================================
// Gamepad::IsValidInput
//==============================================================================
//
// Description: Returns true if this is a valid input for the controller.
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if key it's a valid input
//
//==============================================================================

bool Gamepad::IsValidInput( int dxKey ) const
{
    return GetButtonEnum( dxKey ) != NUM_GAMEPAD_BUTTONS;
}

//==============================================================================
// Gamepad::CalculatePOV
//==============================================================================
//
// Description: Takes a POV input point value and calculates the corresponding
//              up/down/left/right directional values.
//
// Parameters:	POV value
//
// Return:      Up/Down/Left/Right values.
//
//==============================================================================

void Gamepad::CalculatePOV( float povValue, float* up, float* down, float* right, float* left ) const
{
    // Reset all the directional values to 0.
    *up = *down = *left = *right = 0.0f;

    // Convert the POV value to directional values.
    // A value of 1 means that the pov is untouched.
    if( povValue < 1)
    {
        // Convert the POV value (0.0f - 1.0f) into radians (0.0f - 2PI) for use with radmath Trig functions.
        float angle = ((povValue * 360)/180)*rmt::PI;

        // Calculate the corresponding x and y axis values.
        float xValue = rmt::Sin(angle);
        float yValue = rmt::Cos(angle);

        // Now assign the value to all the directions.
        // Note that we are making the POV a digital button with values of only 0 and 1.
        // This is so that walking diagonally is doable, instead of annoyingly slow.
        *right = ( xValue >  0.1f ) ? 1.0f : 0.0f;
        *left =  ( xValue < -0.1f ) ? 1.0f : 0.0f;
        *up =    ( yValue >  0.1f ) ? 1.0f : 0.0f;
        *down =  ( yValue < -0.1f ) ? 1.0f : 0.0f;
    }
}

//==============================================================================
// Gamepad::SetMap
//==============================================================================
//
// Description: Sets up a mapping from a dxkey/direction to a virtual button
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//              direction - the direction of the input
//              virtualbutton - the virtual button that has been mapped
//
// Return:      true if mapped successfully
//
//==============================================================================

bool Gamepad::SetMap( int dxKey, eDirectionType dir, int virtualButton )
{
    rAssert( virtualButton >= 0 && virtualButton < Input::MaxPhysicalButtons );

    eGamepadButton gbutton = GetButtonEnum( dxKey );
    eMapType maptype = VirtualInputs::GetType( virtualButton );

    if( gbutton != NUM_GAMEPAD_BUTTONS )
    {
        m_ButtonMap[ maptype ][ gbutton ][ dir ] = virtualButton;
    }

    return gbutton != NUM_GAMEPAD_BUTTONS;
}

//==============================================================================
// Gamepad::ClearMap
//==============================================================================
//
// Description: Clears the specified mapping so it no longer exists.
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//              direction - the direction of the input
//
// Return:      n/a
//
//==============================================================================

void Gamepad::ClearMap( int dxKey, eDirectionType dir, int virtualButton )
{
    eGamepadButton gbutton = GetButtonEnum( dxKey );
    eMapType maptype = VirtualInputs::GetType( virtualButton );

    if( gbutton != NUM_GAMEPAD_BUTTONS )
    {
        m_ButtonMap[ maptype ][ gbutton ][ dir ] = Input::INVALID_CONTROLLERID;
    }
}

//==============================================================================
// Gamepad::GetMap
//==============================================================================
//
// Description: Retrieves the virtual button of the given type mapped to
//              a dxKey, direction
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//              direction - the direction of the input.
//              maptype - the type of virtual button to look up
//
// Return:      n/a
//
//==============================================================================

int Gamepad::GetMap( int dxKey, eDirectionType dir, eMapType map ) const
{
    eGamepadButton gbutton = GetButtonEnum( dxKey );

    if( gbutton != NUM_GAMEPAD_BUTTONS )
    {
        return m_ButtonMap[ map ][ gbutton ][ dir ];
    }
    else
    {
        return Input::INVALID_CONTROLLERID;
    }
}

//==============================================================================
// Gamepad::ClearMappedButtons
//==============================================================================
//
// Description: Clears the cached button mappings
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

void Gamepad::ClearMappedButtons()
{
    memset( &m_ButtonMap, Input::INVALID_CONTROLLERID, sizeof( m_ButtonMap ) );
}

//==============================================================================
// Gamepad::MapInputToDICode
//==============================================================================
//
// Description: Creates an input point -> direct input virtual key code mapping
//              for the gamepad. Note: this method is really slow, but it only
//              should get called once per game.
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

void Gamepad::MapInputToDICode()
{
    if( m_InputToDICode != NULL )
    {
        delete [] m_InputToDICode;
        m_InputToDICode = NULL;
    }

    if( m_radController != NULL )
    {
        // Get the number of input points
        m_numInputPoints = m_radController->GetNumberOfInputPoints();

        // Set up a cleared index -> di map.
        m_InputToDICode = new int[ m_numInputPoints ];

        // Get/set each input point.
        // We unfortunately have to special case each direct input code.
        for( int i = 0; i < m_numInputPoints; i++ )
        {
            m_InputToDICode[ i ] = Input::INVALID_CONTROLLERID;

            const char *type = m_radController->GetInputPointByIndex( i )->GetType();
 
            if( strcmp( type, "XAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIJOFS_X;
            }
            else if( strcmp( type, "YAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIJOFS_Y;
            }
            else if( strcmp( type, "ZAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIJOFS_Z;
            }
            else if( strcmp( type, "RxAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIJOFS_RX;
            }
            else if( strcmp( type, "RyAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIJOFS_RY;
            }
            else if( strcmp( type, "RzAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIJOFS_RZ;
            }
            else if( strcmp( type, "Slider" ) == 0 )
            {
                // figure out which slider it is.
                for( int j = 0; j < 3; j++ )
                {
                    if( m_radController->GetInputPointByTypeAndIndex( "Slider", j ) ==
                        m_radController->GetInputPointByIndex( i ) )
                    {
                        m_InputToDICode[ i ] = DIJOFS_SLIDER(j);
                        break;
                    }
                }   
            }
            else if( strcmp( type, "POV" ) == 0 )
            {
                // figure out which pov it is.
                for( int j = 0; j < 3; j++ )
                {
                    if( m_radController->GetInputPointByTypeAndIndex( "POV", j ) ==
                        m_radController->GetInputPointByIndex( i ) )
                    {
                        m_InputToDICode[ i ] = DIJOFS_POV(j);
                        break;
                    }
                }
            }
            else if( strcmp( type, "Button" ) == 0 )
            {
                // figure out which button it is
                for( int j = 0; j < 32; j++ )
                {
                    if( m_radController->GetInputPointByTypeAndIndex( "Button", j ) ==
                        m_radController->GetInputPointByIndex( i ) )
                    {
                        m_InputToDICode[ i ] = DIJOFS_BUTTON(j);
                        break;
                    }
                }
            }
        }
    }
}

//==============================================================================
// Gamepad::GetButtonEnum
//==============================================================================
//
// Description: Returns the gamepad button enum for the given input, or
//              NUM_GAMEPAD_BUTTONS if it's not a valid gamepad input.
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

eGamepadButton Gamepad::GetButtonEnum( int dxKey ) const
{
    switch( dxKey )
    {
        case DIJOFS_X:
        {
            return GAMEPAD_X;
        }
        case DIJOFS_Y:
        {
            return GAMEPAD_Y;
        }
        case DIJOFS_Z:
        {
            return GAMEPAD_Z;
        }
        case DIJOFS_RX:
        {
            return GAMEPAD_RX;
        }
        case DIJOFS_RY:
        {
            return GAMEPAD_RY;
        }
        case DIJOFS_RZ:
        {
            return GAMEPAD_RZ;
        }
        case DIJOFS_SLIDER(0):
        {
            return GAMEPAD_SLIDER0;
        }
        case DIJOFS_SLIDER(1):
        {
            return GAMEPAD_SLIDER1;
        }
        case DIJOFS_POV(0):
        {
            return GAMEPAD_POV0;
        }
        case DIJOFS_POV(1):
        {
            return GAMEPAD_POV1;
        }
        case DIJOFS_BUTTON0:
        {
            return GAMEPAD_BUTTON0;
        }
        case DIJOFS_BUTTON1:
        {
            return GAMEPAD_BUTTON1;
        }
        case DIJOFS_BUTTON2:
        {
            return GAMEPAD_BUTTON2;
        }
        case DIJOFS_BUTTON3:
        {
            return GAMEPAD_BUTTON3;
        }
        case DIJOFS_BUTTON4:
        {
            return GAMEPAD_BUTTON4;
        }
        case DIJOFS_BUTTON5:
        {
            return GAMEPAD_BUTTON5;
        }
        case DIJOFS_BUTTON6:
        {
            return GAMEPAD_BUTTON6;
        }
        case DIJOFS_BUTTON7:
        {
            return GAMEPAD_BUTTON7;
        }
        case DIJOFS_BUTTON8:
        {
            return GAMEPAD_BUTTON8;
        }
        case DIJOFS_BUTTON9:
        {
            return GAMEPAD_BUTTON9;
        }
        case DIJOFS_BUTTON10:
        {
            return GAMEPAD_BUTTON10;
        }
        case DIJOFS_BUTTON11:
        {
            return GAMEPAD_BUTTON11;
        }
        case DIJOFS_BUTTON12:
        {
            return GAMEPAD_BUTTON12;
        }
        case DIJOFS_BUTTON13:
        {
            return GAMEPAD_BUTTON13;
        }
        case DIJOFS_BUTTON14:
        {
            return GAMEPAD_BUTTON14;
        }
        case DIJOFS_BUTTON15:
        {
            return GAMEPAD_BUTTON15;
        }
        case DIJOFS_BUTTON16:
        {
            return GAMEPAD_BUTTON16;
        }
        case DIJOFS_BUTTON17:
        {
            return GAMEPAD_BUTTON17;
        }
        case DIJOFS_BUTTON18:
        {
            return GAMEPAD_BUTTON18;
        }
        case DIJOFS_BUTTON19:
        {
            return GAMEPAD_BUTTON19;
        }
        case DIJOFS_BUTTON20:
        {
            return GAMEPAD_BUTTON20;
        }
        case DIJOFS_BUTTON21:
        {
            return GAMEPAD_BUTTON21;
        }
        case DIJOFS_BUTTON22:
        {
            return GAMEPAD_BUTTON22;
        }
        case DIJOFS_BUTTON23:
        {
            return GAMEPAD_BUTTON23;
        }
        case DIJOFS_BUTTON24:
        {
            return GAMEPAD_BUTTON24;
        }
        case DIJOFS_BUTTON25:
        {
            return GAMEPAD_BUTTON25;
        }
        case DIJOFS_BUTTON26:
        {
            return GAMEPAD_BUTTON26;
        }
        case DIJOFS_BUTTON27:
        {
            return GAMEPAD_BUTTON27;
        }
        case DIJOFS_BUTTON28:
        {
            return GAMEPAD_BUTTON28;
        }
        case DIJOFS_BUTTON29:
        {
            return GAMEPAD_BUTTON29;
        }
        case DIJOFS_BUTTON30:
        {
            return GAMEPAD_BUTTON30;
        }
        case DIJOFS_BUTTON31:
        {
            return GAMEPAD_BUTTON31;
        }
        default:
        {
            return NUM_GAMEPAD_BUTTONS;
        }
    };
}