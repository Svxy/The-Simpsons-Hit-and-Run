//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiUserInputHandler
//
// Description: Implementation of the CGuiUserInputHandler class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions        Date            Author      Revision
//                  2000/10/20      DChau       Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/guiuserinputhandler.h>
#include <presentation/gui/guisystem.h>

#include <gameflow/gameflow.h>

#include <raddebug.hpp>

#include <contexts/context.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

struct ControlMap
{
    char* inputName;
    GuiInput::eGuiInput inputID;
};

const ControlMap GUI_CONTROL_MAP[] =
{
#ifdef RAD_GAMECUBE
    { "LeftStickX",     GuiInput::XAxis },
    { "LeftStickY",     GuiInput::YAxis },
    { "RightStickX",    GuiInput::XAxisRight },
    { "RightStickY",    GuiInput::YAxisRight },
    { "DPadLeft",       GuiInput::Left },
    { "DPadRight",      GuiInput::Right },
    { "DPadUp",         GuiInput::Up },
    { "DPadDown",       GuiInput::Down },
    { "Menu",           GuiInput::Start },
    { "A",              GuiInput::Select },
    { "B",              GuiInput::Back },
    { "X",              GuiInput::AuxX },
    { "Y",              GuiInput::AuxY },
    { "TriggerL",       GuiInput::L1 },
    { "TriggerR",       GuiInput::R1 },
#endif

#ifdef RAD_PS2
    { "LeftStickX",     GuiInput::XAxis },
    { "LeftStickY",     GuiInput::YAxis },
    { "RightStickX",    GuiInput::XAxisRight },
    { "RightStickY",    GuiInput::YAxisRight },
    { "DPadLeft",       GuiInput::Left },
    { "DPadRight",      GuiInput::Right },
    { "DPadUp",         GuiInput::Up },
    { "DPadDown",       GuiInput::Down },
    { "Start",          GuiInput::Start },
    { "X",              GuiInput::Select },
    { "Triangle",       GuiInput::Back },
    { "Square",         GuiInput::AuxX },
    { "Circle",         GuiInput::AuxY },
    { "L1",             GuiInput::L1 },
    { "R1",             GuiInput::R1 },
    { "LGA",            GuiInput::AuxStart },   //Only on the GT Wheel
    { "LGX",            GuiInput::AuxSelect },  //Only on the GT Wheel
    { "LGY",            GuiInput::AuxBack },    //Only on the GT Wheel
    { "Wheel",          GuiInput::AuxXAxis },   //Only on the GT Wheel
    { "LGR1",           GuiInput::AuxUp },      //Only on the GT Wheel
    { "LGL1",           GuiInput::AuxDown },    //Only on the GT Wheel
    
#endif

#ifdef RAD_XBOX
    { "LeftStickX",     GuiInput::XAxis },
    { "LeftStickY",     GuiInput::YAxis },
    { "RightStickX",    GuiInput::XAxisRight },
    { "RightStickY",    GuiInput::YAxisRight },
    { "DPadLeft",       GuiInput::Left },
    { "DPadRight",      GuiInput::Right },
    { "DPadUp",         GuiInput::Up },
    { "DPadDown",       GuiInput::Down },
    { "Start",          GuiInput::Start },
    { "Back",           GuiInput::Back },
    { "A",              GuiInput::Select },
    { "B",              GuiInput::Back },
    { "X",              GuiInput::AuxX },
    { "Y",              GuiInput::AuxY },
    { "LeftTrigger",    GuiInput::L1 },
    { "RightTrigger",   GuiInput::R1 },
#endif

#ifdef RAD_WIN32
    { "feMoveLeft",     GuiInput::Left },
    { "feMoveRight",    GuiInput::Right },
    { "feMoveUp",       GuiInput::Up },
    { "feMoveDown",     GuiInput::Down },
    { "feStart",        GuiInput::Start },
    { "feBack",         GuiInput::Back },
    { "feSelect",       GuiInput::Select },
    { "feFunction1",    GuiInput::AuxX },
    { "feFunction2",    GuiInput::L1 },

    { "P1_KBD_Start", GuiInput::P1_KBD_Start },
    { "P1_KBD_Gas", GuiInput::P1_KBD_Select },
    { "P1_KBD_Brake", GuiInput::P1_KBD_Back },
    { "P1_KBD_Left", GuiInput::P1_KBD_Left },
    { "P1_KBD_Right", GuiInput::P1_KBD_Right },
#endif

    { "",               GuiInput::UNKNOWN }
};

const int NUM_GUI_CONTROL_MAPPINGS = sizeof( GUI_CONTROL_MAP ) /
                                     sizeof( GUI_CONTROL_MAP[ 0 ] );

// time between repeated inputs
const int INPUT_REPEAT_PERIOD = 166; // in milliseconds

// time before first repeated input
const int INPUT_REPEAT_WAIT = INPUT_REPEAT_PERIOD; // in milliseconds

const float ANALOG_BUTTON_THRESHOLD = 0.5f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiUserInputHandler::CGuiUserInputHandler
//===========================================================================
// Description: Constructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiUserInputHandler::CGuiUserInputHandler( void )
:   Mappable( Input::ACTIVE_ALL ),
    m_XAxisValue( 0.0f ),
    m_YAxisValue( 0.0f ),
    m_XAxisDuration( 0 ),
    m_YAxisDuration( 0 ),
#ifdef RAD_WIN32
    m_RightValue( 0 ),
    m_LeftValue( 0 ),
    m_UpValue( 0 ),
    m_DownValue( 0 ),
    m_RightDuration( 0 ),
    m_LeftDuration( 0 ),
    m_UpDuration( 0 ),
    m_DownDuration( 0 ),
#endif
    m_isStartToSelectMappingEnabled( true )
{
    this->ResetRepeatableButtons();
}


//===========================================================================
// CGuiUserInputHandler::~CGuiUserInputHandler
//===========================================================================
// Description: Destructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiUserInputHandler::~CGuiUserInputHandler( void )
{
}


//===========================================================================
// CGuiUserInputHandler::Left
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::Left( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_LEFT, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::Right
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::Right( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_RIGHT, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::Up
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::Up( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_UP, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::Down
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::Down( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_DOWN, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::Start
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::Start( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_START, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::Select
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::Select( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_SELECT, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::Back
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::Back( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_BACK, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::AuxX
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::AuxX( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_AUX_X, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::AuxY
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::AuxY( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_AUX_Y, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::L1
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::L1( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_L1, controllerId );
}


//===========================================================================
// CGuiUserInputHandler::R1
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void CGuiUserInputHandler::R1( int controllerId )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_R1, controllerId );
}


void CGuiUserInputHandler::OnControllerDisconnect( int id )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_DISCONNECT, id );

    this->ResetRepeatableButtons();

    Mappable::OnControllerDisconnect( id );
}


void CGuiUserInputHandler::OnControllerConnect( int id )
{
    GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_CONNECT, id );

    Mappable::OnControllerConnect( id );
}

//===========================================================================
// CGuiUserInputHandler::OnButton
//===========================================================================
// Description:
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

//////////////////////////////////////////////////////////////////////////////
// IButtonedObject declarations
//
void CGuiUserInputHandler::OnButton( int controllerId, int buttonId, const IButton* pButton )
{
    rAssert( pButton != NULL );

    switch ( buttonId )
    {
        case GuiInput::AuxXAxis:
        case GuiInput::XAxis:
        {
            if ( pButton->GetValue() > ANALOG_BUTTON_THRESHOLD && !( m_XAxisValue > ANALOG_BUTTON_THRESHOLD ) )
            {
                Right( controllerId );

                m_XAxisDuration = -INPUT_REPEAT_WAIT;
            }

            if ( pButton->GetValue() < -ANALOG_BUTTON_THRESHOLD && !( m_XAxisValue < -ANALOG_BUTTON_THRESHOLD ) )
            {
                Left( controllerId );

                m_XAxisDuration = -INPUT_REPEAT_WAIT;
            }

            m_XAxisValue = pButton->GetValue();

            break;
        }
        case GuiInput::YAxis:
        {
            if ( pButton->GetValue() > ANALOG_BUTTON_THRESHOLD && !( m_YAxisValue > ANALOG_BUTTON_THRESHOLD ) )
            {
                Up( controllerId );

                m_YAxisDuration = -INPUT_REPEAT_WAIT;
            }

            if ( pButton->GetValue() < -ANALOG_BUTTON_THRESHOLD && !( m_YAxisValue < -ANALOG_BUTTON_THRESHOLD ) )
            {
                Down( controllerId );

                m_YAxisDuration = -INPUT_REPEAT_WAIT;
            }

            m_YAxisValue = pButton->GetValue();

            break;
        }
        case GuiInput::XAxisRight:
        case GuiInput::YAxisRight:
        {
            // TC: *** temporary for now ***
            //
            GetGuiSystem()->HandleMessage( GUI_MSG_CONTROLLER_AUX_LEFT, controllerId );

            break;
        }
#ifdef RAD_WIN32
        case GuiInput::Left:
        {
            if ( pButton->GetValue() > ANALOG_BUTTON_THRESHOLD && !( m_LeftValue > ANALOG_BUTTON_THRESHOLD ) )
            {
                Left( controllerId );

                m_LeftDuration = -INPUT_REPEAT_WAIT;
            }
            m_LeftValue = pButton->GetValue();
            break;
        }
        case GuiInput::Right:
        {
            if ( pButton->GetValue() > ANALOG_BUTTON_THRESHOLD && !( m_RightValue > ANALOG_BUTTON_THRESHOLD ) )
            {
                Right( controllerId );

                m_RightDuration = -INPUT_REPEAT_WAIT;
            }
            m_RightValue = pButton->GetValue();
            break;
        }
        case GuiInput::Up:
        {
            if ( pButton->GetValue() > ANALOG_BUTTON_THRESHOLD && !( m_UpValue > ANALOG_BUTTON_THRESHOLD ) )
            {
                Up( controllerId );

                m_UpDuration = -INPUT_REPEAT_WAIT;
            }
            m_UpValue = pButton->GetValue();
            break;
        }
        case GuiInput::Down:
        {
            if ( pButton->GetValue() > ANALOG_BUTTON_THRESHOLD && !( m_DownValue > ANALOG_BUTTON_THRESHOLD ) )
            {
                Down( controllerId );

                m_DownDuration = -INPUT_REPEAT_WAIT;
            }
            m_DownValue = pButton->GetValue();
            break;
        }
#endif
        default:
        {
            break;
        }
    }
}

void CGuiUserInputHandler::OnButtonUp( int controllerId, int buttonId, const IButton* pButton )
{
    rAssert( buttonId >= 0 && buttonId < GuiInput::NUM_GUI_INPUTS );

    if ( static_cast<unsigned int>( buttonId ) < sizeof( m_buttonDownDuration ) /
                                                 sizeof( m_buttonDownDuration[ 0 ] ) )
    {
        // reset button down duration time
        m_buttonDownDuration[ buttonId ] = -INPUT_REPEAT_WAIT;
    }
}

void CGuiUserInputHandler::OnButtonDown( int controllerId, int buttonId, const IButton* pButton )
{
    ContextEnum context = GetGameFlow()->GetCurrentContext();

    switch( buttonId )
    {
#ifndef RAD_WIN32  // for windows we handle them in onbutton()
        case GuiInput::Left:
        {
            this->Left( controllerId );

            break;
        }
        case GuiInput::Right:
        {
            this->Right( controllerId );

            break;
        }
        case GuiInput::AuxUp:
        {
            if ( context != CONTEXT_SUPERSPRINT_FE &&
                 context != CONTEXT_SUPERSPRINT )
            {
                break;
            }

            //Fall through
        }
        case GuiInput::Up:
        {
            this->Up( controllerId );

            break;
        }
        case GuiInput::AuxDown:
            {
                if ( context != CONTEXT_SUPERSPRINT_FE &&
                    context != CONTEXT_SUPERSPRINT )
                {
                    break;
                }

                //Fall through
            }
        case GuiInput::Down:
        {
            this->Down( controllerId );

            break;
        }
#endif
        case GuiInput::AuxStart:
        {
            if ( context != CONTEXT_SUPERSPRINT_FE &&
                 context != CONTEXT_SUPERSPRINT )
            {
                break;
            }

            //Fall through
        }
        case GuiInput::Start:
        {
            this->Start( controllerId );

#ifdef RAD_XBOX
            if( m_isStartToSelectMappingEnabled )
            {
                // for Xbox only, START is mapped to same functionality as SELECT
                this->Select( controllerId );
            }
#endif

            break;
        }
        case GuiInput::AuxSelect:
        {
            if ( context != CONTEXT_SUPERSPRINT_FE &&
                 context != CONTEXT_SUPERSPRINT )
            {
                break;
            }

            //Fall through
        }
        case GuiInput::Select:
        {
            this->Select( controllerId );

            break;
        }
        case GuiInput::AuxBack:
        {
            if ( context != CONTEXT_SUPERSPRINT_FE &&
                 context != CONTEXT_SUPERSPRINT )
            {
                break;
            }

            //Fall through
        }
        case GuiInput::Back:
        {
            this->Back( controllerId );

            break;
        }
        case GuiInput::AuxX:
        {
            this->AuxX( controllerId );

            break;
        }
        case GuiInput::AuxY:
        {
            this->AuxY( controllerId );

            break;
        }
        case GuiInput::L1:
        {
            this->L1( controllerId );

            break;
        }
        case GuiInput::R1:
        {
            this->R1( controllerId );

            break;
        }
        default:
        {
#ifdef RAD_WIN32
            if ( buttonId >= GuiInput::P1_KBD_Start && buttonId <= GuiInput::P1_KBD_Right )
            {
                //This is a super sprint Key.
                if ( context == CONTEXT_SUPERSPRINT_FE || context == CONTEXT_SUPERSPRINT )
                {
                    unsigned int button = (buttonId - GuiInput::P1_KBD_Start);
                    int player = 3;

                    enum 
                    {
                        Start,
                        Select,
                        Back,
                        Left,
                        Right
                    };

                    switch( button )
                    {
                    case Start:
                        this->Start( player ); 
                        break;
                    case Select:
                        this->Select( player );
                        break;
                    case Back:
                        this->Back( player );
                        break;
                    case Left:
                        this->Left( player );
                        break;
                    case Right:
                        this->Right( player );
                        break;
                    default:
                        break;
                    }
                }
            }
#endif
            break;
        }
    }
}

void CGuiUserInputHandler::LoadControllerMappings( unsigned int controllerId )
{
    // now set controller mappings
    for( int i = 0; i < NUM_GUI_CONTROL_MAPPINGS; i++ )
    {
        this->Map( GUI_CONTROL_MAP[ i ].inputName,
                   GUI_CONTROL_MAP[ i ].inputID,
                   0,
                   controllerId );
    }
}

void CGuiUserInputHandler::Update( unsigned int elapsedTime, unsigned int controllerId )
{
    if( !this->IsActive() )
    {
        this->ResetRepeatableButtons();
    }

#ifndef RAD_WIN32
    // check for repeated DPad inputs
    //
    for( unsigned int i = 0; i < sizeof( m_buttonDownDuration ) /
                                 sizeof( m_buttonDownDuration[ 0 ] ); i++ )
    {
        // check if button is still down
        if( this->IsButtonDown( i ) )
        {
            m_buttonDownDuration[ i ] += elapsedTime;

            if( m_buttonDownDuration[ i ] > INPUT_REPEAT_PERIOD )
            {
                // repeat button down event
                this->OnButtonDown( controllerId, i, NULL );

                m_buttonDownDuration[ i ] = (m_buttonDownDuration[ i ] + elapsedTime ) % INPUT_REPEAT_PERIOD;
            }
        }
    }
#endif

    // check for repeated Thumbstick inputs
    //
    if( m_XAxisValue > ANALOG_BUTTON_THRESHOLD )
    {
        m_XAxisDuration += elapsedTime;

        if( m_XAxisDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat right input
            this->Right( controllerId );

            m_XAxisDuration = (m_XAxisDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }

    if( m_XAxisValue < -ANALOG_BUTTON_THRESHOLD )
    {
        m_XAxisDuration += elapsedTime;

        if( m_XAxisDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat left input
            this->Left( controllerId );

            m_XAxisDuration = (m_XAxisDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }

    if( m_YAxisValue > ANALOG_BUTTON_THRESHOLD )
    {
        m_YAxisDuration += elapsedTime;

        if( m_YAxisDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat up input
            this->Up( controllerId );

            m_YAxisDuration = (m_YAxisDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }

    if( m_YAxisValue < -ANALOG_BUTTON_THRESHOLD )
    {
        m_YAxisDuration += elapsedTime;

        if( m_YAxisDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat down input
            this->Down( controllerId );

            m_YAxisDuration = (m_YAxisDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }

#ifdef RAD_WIN32
    if( m_LeftValue > ANALOG_BUTTON_THRESHOLD )
    {
        m_LeftDuration += elapsedTime;

        if( m_LeftDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat down input
            Left( controllerId );

            m_LeftDuration = (m_LeftDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }
    if( m_RightValue > ANALOG_BUTTON_THRESHOLD )
    {
        m_RightDuration += elapsedTime;

        if( m_RightDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat down input
            Right( controllerId );

            m_RightDuration = (m_RightDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }
    if( m_UpValue > ANALOG_BUTTON_THRESHOLD )
    {
        m_UpDuration += elapsedTime;

        if( m_UpDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat down input
            Up( controllerId );

            m_UpDuration = (m_UpDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }
    if( m_DownValue > ANALOG_BUTTON_THRESHOLD )
    {
        m_DownDuration += elapsedTime;

        if( m_DownDuration > INPUT_REPEAT_PERIOD )
        {
            // repeat down input
            Down( controllerId );

            m_DownDuration = (m_DownDuration + elapsedTime ) % INPUT_REPEAT_PERIOD;
        }
    }
#endif
}

bool
CGuiUserInputHandler::IsXAxisOnLeft() const
{
#ifdef RAD_WIN32
    return( m_LeftValue > ANALOG_BUTTON_THRESHOLD );
#else
    return( m_XAxisValue < -ANALOG_BUTTON_THRESHOLD );
#endif
}

bool
CGuiUserInputHandler::IsXAxisOnRight() const
{
#ifdef RAD_WIN32
    return( m_RightValue > ANALOG_BUTTON_THRESHOLD );
#else
    return( m_XAxisValue > ANALOG_BUTTON_THRESHOLD );
#endif
}

bool
CGuiUserInputHandler::IsYAxisOnUp() const
{
#ifdef RAD_WIN32
    return( m_UpValue > ANALOG_BUTTON_THRESHOLD );
#else
    return( m_YAxisValue > ANALOG_BUTTON_THRESHOLD );
#endif
}

bool
CGuiUserInputHandler::IsYAxisOnDown() const
{
#ifdef RAD_WIN32
    return( m_DownValue > ANALOG_BUTTON_THRESHOLD );
#else
    return( m_YAxisValue < -ANALOG_BUTTON_THRESHOLD );
#endif
}

//===========================================================================
// Private Member Functions
//===========================================================================

void
CGuiUserInputHandler::ResetRepeatableButtons()
{
    for( unsigned int i = 0; i < sizeof( m_buttonDownDuration ) /
                                 sizeof( m_buttonDownDuration[ 0 ] ); i++ )
    {
        m_buttonDownDuration[ i ] = -INPUT_REPEAT_WAIT;
    }

    m_XAxisValue = 0.0f;
    m_YAxisValue = 0.0f;
    m_XAxisDuration = -INPUT_REPEAT_WAIT;
    m_YAxisDuration = -INPUT_REPEAT_WAIT;

#ifdef RAD_WIN32
    m_RightValue = 0;
    m_LeftValue = 0;
    m_UpValue = 0;
    m_DownValue = 0;
    m_RightDuration = 0;
    m_LeftDuration = 0;
    m_UpDuration = 0;
    m_DownDuration = 0;
#endif
}

