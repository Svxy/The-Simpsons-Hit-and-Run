#include <input/SteeringWheel.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//--------------------------------------------------------
// Constructor/Destructor 
//--------------------------------------------------------

SteeringWheel::SteeringWheel()
: RealController( STEERINGWHEEL ),
  m_bPedalInverted(false),
  m_InputToDICode( NULL )
{
}

SteeringWheel::~SteeringWheel()
{
    if( m_InputToDICode != NULL )
    {
        delete [] m_InputToDICode;
    }
}

//==============================================================================
// SteeringWheel::Init
//==============================================================================
//
// Description: Initializes the wheel and figures out what special type it is.
//
// Parameters:	controller - the rad controller for the steering wheel
//
// Return:      None.
//
//==============================================================================

void SteeringWheel::Init( IRadController* pController )
{
    // Clear the properties
    m_bPedalInverted = false;

    if( pController != NULL )
    {
        // Check for the wingman - a bad bad wheel.
        m_bPedalInverted = (_stricmp( pController->GetType(), "Logitech WingMan Formula Force GP USB") == 0);
    }

    // Connect
    RealController::Init( pController );
}

//==============================================================================
// SteeringWheel::MapInputToDICode
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

void SteeringWheel::MapInputToDICode()
{
    if( m_InputToDICode != NULL )
    {
        delete [] m_InputToDICode;
        m_InputToDICode = NULL;
    }

    if( m_radController != NULL )
    {
        // Get the number of input points
        unsigned num = m_radController->GetNumberOfInputPoints();

        // Set up a cleared index -> di map.
        m_InputToDICode = new int[ num ];

        for( unsigned i = 0; i < num; i++ )
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
            // Now make the input code controller independent.
            m_InputToDICode[ i ] = GetIndependentDICode( m_InputToDICode[ i ] );
        }
    }
}

//==============================================================================
// SteeringWheel::GetDICode
//==============================================================================
//
// Description: Returns the direct input keycode associated with an input point,
//              or -1 if there is no controller or input code.
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

int SteeringWheel::GetDICode( int inputpoint ) const
{
    return m_InputToDICode == NULL ? Input::INVALID_CONTROLLERID : m_InputToDICode[ inputpoint ];
}

//==============================================================================
// SteeringWheel::GetIndependentDICode
//==============================================================================
//
// Description: Same as gamepad
//
// Parameters:	The direct input code for the specific controller.
//
// Return:      The inpendent & real direct input code they should have assigned
//              it.  Often this is the same code.
//
//==============================================================================

int SteeringWheel::GetIndependentDICode( int diCode ) const
{
    switch( diCode )
    {
    case DIJOFS_SLIDER(0):
        {
            return m_bPedalInverted ? DIJOFS_Y : DIJOFS_SLIDER(0);
        }
    default:
        {
            return diCode;
        }
    }
}