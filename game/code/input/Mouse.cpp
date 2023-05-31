#include <input/Mouse.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

/******************************************************************************
	Construction/Destruction
 *****************************************************************************/

Mouse::Mouse()
: RealController(MOUSE),
  m_fSensitivityX(10.0f),
  m_fSensitivityY(10.0f)
{
    memset( &m_absPosition, 0, sizeof( m_absPosition ) );
    memset( &m_relPosition, 0, sizeof( m_relPosition ) );

    ClearMappedButtons();
}

Mouse::~Mouse()
{
}

//==============================================================================
// Mouse::IsMouseAxis
//==============================================================================
//
// Description: Identifies any keys that are input axes. (can go in two directions)
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if input is an axis
//
//==============================================================================

bool Mouse::IsMouseAxis( int dxKey ) const
{
    switch( dxKey )
    {
        case DIMOFS_X:
        case DIMOFS_Y:
        case DIMOFS_Z:
            return true;
        default:
            return false;
    }
}

//==============================================================================
// Mouse::IsValidInput
//==============================================================================
//
// Description: Returns true if this is a valid input for the controller.
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if key it's a valid input
//
//==============================================================================

bool Mouse::IsValidInput( int dxKey ) const
{
    return GetButtonEnum( dxKey ) != NUM_MOUSE_BUTTONS;
}

//==============================================================================
// Mouse::CalculateMouseAxis
//==============================================================================
//
// Description: Calculates a direct input axis value for a mouse axis based on
//              its current mouse value.  Mouse values can cover -1000..1000,
//              and this gets mapped to an axis value of -1..1.
//
// Parameters:	mouseAxis - the axis that's changed.
//              value - the value of the mouse axis
//
// Return:      input point value of -1..1
//
//==============================================================================

float Mouse::CalculateMouseAxis( int mouseAxis, float value )
{
    const float MAX_THRESH = 1.0f;
    const float MIN_THRESH = -1.0f;

    if( value == 0.0f )
    {
        return 0.0f;
    }

    switch( mouseAxis ) 
    {
    case DIMOFS_X:
        {
            //take the delta
            m_relPosition.x = (value - m_absPosition.x) / m_fSensitivityX;

            //re-center the cursor pos.
            m_absPosition.x = value;

            if( m_relPosition.x > MAX_THRESH ) m_relPosition.x = MAX_THRESH;
            else if( m_relPosition.x < MIN_THRESH ) m_relPosition.x = MIN_THRESH;

            return m_relPosition.x;
        }
    	break;
    case DIMOFS_Y:
    case DIMOFS_Z:
        {
            //take the delta
            m_relPosition.y = (value - m_absPosition.y) / m_fSensitivityY;

            //re-center the cursor pos.
            m_absPosition.y = value;

            if( m_relPosition.y > MAX_THRESH ) m_relPosition.y = MAX_THRESH;
            else if( m_relPosition.y < MIN_THRESH ) m_relPosition.y = MIN_THRESH;

            return -m_relPosition.y;
        }
    	break;
    default:
        break;
    }
    return 0.0f;
}

//==============================================================================
// Mouse::SetMap
//==============================================================================
//
// Description: Sets up a mapping from a dxkey/direction to a virtual button
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//              direction - the direction of the input
//              virtualbutton - the virtual button that has been mapped
//
// Return:      true if button was mapped successfully
//
//==============================================================================

bool Mouse::SetMap( int dxKey, eDirectionType dir, int virtualButton )
{
    rAssert( virtualButton >= 0 && virtualButton < Input::MaxPhysicalButtons );

    eMouseButton mbutton = GetButtonEnum( dxKey );
    eMapType maptype = VirtualInputs::GetType( virtualButton );

    if( mbutton != NUM_MOUSE_BUTTONS )
    {
        m_ButtonMap[ maptype ][ mbutton ][ dir ] = virtualButton;
    }

    return mbutton != NUM_MOUSE_BUTTONS;
}

//==============================================================================
// Mouse::ClearMap
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

void Mouse::ClearMap( int dxKey, eDirectionType dir, int virtualButton )
{
    eMouseButton mbutton = GetButtonEnum( dxKey );
    eMapType maptype = VirtualInputs::GetType( virtualButton );

    if( mbutton != NUM_MOUSE_BUTTONS )
    {
        m_ButtonMap[ maptype ][ mbutton ][ dir ] = Input::INVALID_CONTROLLERID;
    }
}

//==============================================================================
// Mouse::GetMap
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

int Mouse::GetMap( int dxKey, eDirectionType dir, eMapType map ) const
{
    eMouseButton mbutton = GetButtonEnum( dxKey );

    if( mbutton != NUM_MOUSE_BUTTONS )
    {
        return m_ButtonMap[ map ][ mbutton ][ dir ];
    }
    else
    {
        return Input::INVALID_CONTROLLERID;
    }
}

//==============================================================================
// Mouse::ClearMappedButtons
//==============================================================================
//
// Description: Clears the cached button mappings
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

void Mouse::ClearMappedButtons()
{
    memset( &m_ButtonMap, Input::INVALID_CONTROLLERID, sizeof( m_ButtonMap ) );
}

//==============================================================================
// Mouse::MapInputToDICode
//==============================================================================
//
// Description: Creates an input point -> direct input virtual key code mapping
//              for the mouse
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

void Mouse::MapInputToDICode()
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
            const char *name = m_radController->GetInputPointByIndex( i )->GetName();

            if( strcmp( type, "RelXAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIMOFS_X;
            }
            else if( strcmp( type, "RelYAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIMOFS_Y;
            }
            else if( strcmp( type, "RelZAxis" ) == 0 )
            {
                m_InputToDICode[ i ] = DIMOFS_Z;
            }
            else if( strcmp( type, "Button" ) == 0 )
            {
                int button;

                if( sscanf( name, "Button %d", &button ) == 1 )
                {
                    switch( button )
                    {
                    case 0:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON0;
                            break;
                        }
                    case 1:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON1;
                            break;
                        }
                    case 2:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON2;
                            break;
                        }
                    case 3:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON3;
                            break;
                        }
                    case 4:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON4;
                            break;
                        }
                    case 5:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON5;
                            break;
                        }
                    case 6:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON6;
                            break;
                        }
                    case 7:
                        {
                            m_InputToDICode[ i ] = DIMOFS_BUTTON7;
                            break;
                        }
                    }
                }
            }
        }
    }
}

//==============================================================================
// Mouse::GetButtonEnum
//==============================================================================
//
// Description: Returns the mouse button enum for the given input, or
//              NUM_MOUSE_BUTTONS if it's not a valid mouse input.
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

eMouseButton Mouse::GetButtonEnum( int dxKey ) const
{
    switch( dxKey )
    {
        case DIMOFS_X:
        {
            return MOUSE_X;
        }
        case DIMOFS_Y:
        {
            return MOUSE_Y;
        }
        case DIMOFS_Z:
        {
            return MOUSE_Z;
        }
        case DIMOFS_BUTTON0:
        {
            return MOUSE_BUTTON0;
        }
        case DIMOFS_BUTTON1:
        {
            return MOUSE_BUTTON1;
        }
        case DIMOFS_BUTTON2:
        {
            return MOUSE_BUTTON2;
        }
        case DIMOFS_BUTTON3:
        {
            return MOUSE_BUTTON3;
        }
        case DIMOFS_BUTTON4:
        {
            return MOUSE_BUTTON4;
        }
        case DIMOFS_BUTTON5:
        {
            return MOUSE_BUTTON5;
        }
        case DIMOFS_BUTTON6:
        {
            return MOUSE_BUTTON6;
        }
        case DIMOFS_BUTTON7:
        {
            return MOUSE_BUTTON7;
        }
        default:
        {
            return NUM_MOUSE_BUTTONS;
        }
    }
}
