//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CheatInputHandler
//
// Description: Implementation of the CheatInputHandler class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/19      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <cheats/cheatinputhandler.h>
#include <cheats/cheatinputsystem.h>

//===========================================================================
// Local Constants
//===========================================================================

struct CheatInputMapping
{
    char* inputName;
    int inputID;
};

static const CheatInputMapping CHEAT_INPUT_MAPPINGS[] =
{
#ifdef RAD_GAMECUBE
    { "A",              CHEAT_INPUT_0 },
    { "B",              CHEAT_INPUT_1 },
    { "X",              CHEAT_INPUT_2 },
    { "Y",              CHEAT_INPUT_3 },
    { "TriggerL",       CHEAT_INPUT_LTRIGGER },
    { "TriggerR",       CHEAT_INPUT_RTRIGGER },
#endif

#ifdef RAD_PS2
    { "X",              CHEAT_INPUT_0 },
    { "Circle",         CHEAT_INPUT_1 },
    { "Square",         CHEAT_INPUT_2 },
    { "Triangle",       CHEAT_INPUT_3 },
    { "L1",             CHEAT_INPUT_LTRIGGER },
    { "R1",             CHEAT_INPUT_RTRIGGER },
#endif

#ifdef RAD_XBOX
    { "A",              CHEAT_INPUT_0 },
    { "B",              CHEAT_INPUT_1 },
    { "X",              CHEAT_INPUT_2 },
    { "Y",              CHEAT_INPUT_3 },
    { "LeftTrigger",    CHEAT_INPUT_LTRIGGER },
    { "RightTrigger",   CHEAT_INPUT_RTRIGGER },
#endif

#ifdef RAD_WIN32        // these are not laid out yet
    { "Attack",         CHEAT_INPUT_0 },
    { "Jump",           CHEAT_INPUT_1 },
    { "Sprint",         CHEAT_INPUT_2 },
    { "DoAction",       CHEAT_INPUT_3 },
    { "CameraFunc1",    CHEAT_INPUT_LTRIGGER },
    { "CameraFunc2",    CHEAT_INPUT_RTRIGGER },
#endif

    { "",               UNKNOWN_CHEAT_INPUT }
};

static const int unsigned NUM_CHEAT_INPUT_MAPPINGS =
    sizeof( CHEAT_INPUT_MAPPINGS ) / sizeof( CHEAT_INPUT_MAPPINGS[ 0 ] );

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CheatInputHandler::CheatInputHandler
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CheatInputHandler::CheatInputHandler()
:   m_LTriggerBitMask( 0 ),
    m_RTriggerBitMask( 0 ),
    m_currentInputIndex( 0 )
{
    this->ResetInputSequence();
}

//===========================================================================
// CheatInputHandler::~CheatInputHandler
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CheatInputHandler::~CheatInputHandler()
{
}

//===========================================================================
// CheatInputHandler::ResetInputSequence
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputHandler::ResetInputSequence()
{
    m_currentInputIndex = 0;

    // just to be sure
    //
    for( unsigned int i = 0; i < NUM_CHEAT_SEQUENCE_INPUTS; i++ )
    {
        m_inputSequence[ i ] = UNKNOWN_CHEAT_INPUT;
    }
}

//===========================================================================
// CheatInputHandler::GetInputName
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
const char*
CheatInputHandler::GetInputName( eCheatInput cheatInput )
{
    rAssert( cheatInput < static_cast<int>( NUM_CHEAT_INPUT_MAPPINGS ) );

    return CHEAT_INPUT_MAPPINGS[ cheatInput ].inputName;
}

//===========================================================================
// CheatInputHandler::OnButton
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CheatInputHandler::OnButton( int controllerId,
                                  int buttonId,
                                  const IButton* pButton )
{
}

//===========================================================================
// CheatInputHandler::OnButtonDown
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CheatInputHandler::OnButtonDown( int controllerId,
                                      int buttonId,
                                      const IButton* pButton )
{
    switch( buttonId )
    {
        case CHEAT_INPUT_LTRIGGER:
        {
            m_LTriggerBitMask |= (1 << controllerId);

            bool isRTriggerDown = ((m_RTriggerBitMask & (1 << controllerId)) > 0);
            GetCheatInputSystem()->SetActivated( controllerId,
                                                 isRTriggerDown );

            break;
        }
        case CHEAT_INPUT_RTRIGGER:
        {
            m_RTriggerBitMask |= (1 << controllerId);

            bool isLTriggerDown = ((m_LTriggerBitMask & (1 << controllerId)) > 0);
            GetCheatInputSystem()->SetActivated( controllerId,
                                                 isLTriggerDown );

            break;
        }
        default:
        {
            if( GetCheatInputSystem()->IsActivated( controllerId ) )
            {
                rAssert( buttonId < NUM_CHEAT_INPUTS );

                rReleasePrintf( "Received Cheat Input [%d] = [%d]\n",
                                m_currentInputIndex, buttonId );

                // add input to current sequence
                //
                m_inputSequence[ m_currentInputIndex++ ] = static_cast<eCheatInput>( buttonId );
                m_currentInputIndex %= NUM_CHEAT_SEQUENCE_INPUTS;

                // if this is the last input for the current sequence,
                // send current sequence to cheat input system for
                // validation
                //
                if( m_currentInputIndex == 0 )
                {
                    GetCheatInputSystem()->ReceiveInputs( m_inputSequence );
                }
            }

            break;
        }
    }
}

//===========================================================================
// CheatInputHandler::OnButtonUp
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CheatInputHandler::OnButtonUp( int controllerId,
                                    int buttonId,
                                    const IButton* pButton )
{
    switch( buttonId )
    {
        case CHEAT_INPUT_LTRIGGER:
        {
            m_LTriggerBitMask &= ~(1 << controllerId);

            GetCheatInputSystem()->SetActivated( controllerId, false );

            break;
        }
        case CHEAT_INPUT_RTRIGGER:
        {
            m_RTriggerBitMask &= ~(1 << controllerId);

            GetCheatInputSystem()->SetActivated( controllerId, false );

            break;
        }
        default:
        {
            break;
        }
    }
}

//===========================================================================
// CheatInputHandler::LoadControllerMappings
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CheatInputHandler::LoadControllerMappings( unsigned int controllerId )
{
    for( unsigned int i = 0; i < NUM_CHEAT_INPUT_MAPPINGS; i++ )
    {
        this->Map( CHEAT_INPUT_MAPPINGS[ i ].inputName,
                   CHEAT_INPUT_MAPPINGS[ i ].inputID,
                   0,
                   controllerId );
/*
        rTunePrintf( "Load Mapping: %s, %d, %d\n",
                     CHEAT_INPUT_MAPPINGS[ i ].inputName,
                     CHEAT_INPUT_MAPPINGS[ i ].inputID,
                     controllerId );
*/
    }
}


//===========================================================================
// Private Member Functions
//===========================================================================

