#include <input/Keyboard.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//--------------------------------------------------------
// Constructor/Destructor 
//--------------------------------------------------------

Keyboard::Keyboard()
: RealController( KEYBOARD )
{
    ClearMappedButtons();
}

Keyboard::~Keyboard()
{
}

//==============================================================================
// Keyboard::IsValidInput
//==============================================================================
//
// Description: Returns true if this is a valid input for the controller.
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if key it's a valid input
//
//==============================================================================

bool Keyboard::IsValidInput( int dxKey ) const
{
    return dxKey >= DIK_ESCAPE && dxKey <= DIK_MEDIASELECT;
}

//==============================================================================
// Keyboard::IsBannedInput
//==============================================================================
//
// Description: Returns true if the key is banned for mapping.
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if key is banned for mapping
//
//==============================================================================

bool Keyboard::IsBannedInput( int dxKey ) const
{
    switch( dxKey )
    {
        case DIK_LMENU:
        case DIK_RMENU:
        case DIK_LWIN:
        case DIK_RWIN:
            return true;
        default:
            return false;
    }
}

//==============================================================================
// Keyboard::SetMap
//==============================================================================
//
// Description: Sets up a mapping from a dxkey/direction to a virtual button
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//              direction - for keyboard this must be DIR_UP
//              virtualbutton - the virtual button that has been mapped
//
// Return:      true if mapped successfully
//
//==============================================================================

bool Keyboard::SetMap( int dxKey, eDirectionType dir, int virtualButton )
{
    rAssert( dxKey >= 0 && dxKey < NUM_KEYBOARD_BUTTONS );
    rAssert( virtualButton >= 0 && virtualButton < Input::MaxPhysicalButtons );
    rAssert( dir == DIR_UP );

    eMapType maptype = VirtualInputs::GetType( virtualButton );
    
    if( dxKey >= 0 && dxKey < NUM_KEYBOARD_BUTTONS )
    {
        m_ButtonMap[ maptype ][ dxKey ] = virtualButton;
        return true;
    }
    else
    {
        return false;
    }
}

//==============================================================================
// Keyboard::ClearMap
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

void Keyboard::ClearMap( int dxKey, eDirectionType dir, int virtualButton )
{
    rAssert( dxKey >= 0 && dxKey < NUM_KEYBOARD_BUTTONS );
    rAssert( virtualButton >= 0 && virtualButton < Input::MaxPhysicalButtons );
    rAssert( dir == DIR_UP );

    eMapType maptype = VirtualInputs::GetType( virtualButton );

    if( dxKey >= 0 && dxKey < NUM_KEYBOARD_BUTTONS )
    {
        m_ButtonMap[ maptype ][ dxKey ] = Input::INVALID_CONTROLLERID;
    }
}

//==============================================================================
// Keyboard::GetMap
//==============================================================================
//
// Description: Retrieves the virtual button of the given type mapped to
//              a dxKey, direction
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//              direction - for keyboard only DIR_UP is used.
//              maptype - the type of virtual button to look up
//
// Return:      n/a
//
//==============================================================================

int Keyboard::GetMap( int dxKey, eDirectionType dir, eMapType map ) const
{
    rAssert( dxKey >= 0 && dxKey < NUM_KEYBOARD_BUTTONS );
 
    if( dir == DIR_UP )
    {
        return m_ButtonMap[ map ][ dxKey ];
    }
    else
    {
        return Input::INVALID_CONTROLLERID;
    }
}

//==============================================================================
// Keyboard::ClearMappedButtons
//==============================================================================
//
// Description: Clears the cached button mappings
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

void Keyboard::ClearMappedButtons()
{
    memset( &m_ButtonMap, Input::INVALID_CONTROLLERID, sizeof( m_ButtonMap ) );
}

//==============================================================================
// Keyboard::MapInputToDICode
//==============================================================================
//
// Description: Creates an input point -> direct input virtual key code mapping
//              for the keyboard
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

void Keyboard::MapInputToDICode()
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

        for( int i = 0; i < m_numInputPoints; i++ )
        {
            m_InputToDICode[ i ] = Input::INVALID_CONTROLLERID;
        }

        // Reverse the di -> index map into what we want.
        for( int di = DIK_ESCAPE; di <= NUM_KEYBOARD_BUTTONS; di++ )
        {
            int inputpoint = VirtualKeyToIndex[ di ];

            if( inputpoint >= 0 &&
                inputpoint < m_numInputPoints )
            {
                m_InputToDICode[ inputpoint ] = di;
            }
        }
    }
}
