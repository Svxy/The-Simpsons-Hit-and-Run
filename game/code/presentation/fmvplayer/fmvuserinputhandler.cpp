//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   FMVUserInputHandler
//
// Description: Implementation of the FMVUserInputHandler class.
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

#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/fmvplayer/fmvuserinputhandler.h>

#include <presentation/presentation.h>
#include <presentation/gui/guiscreenmessage.h>
#include <input/inputmanager.h>
#include <gameflow/gameflow.h>
#include <contexts/context.h>
#include <main/game.h>
#include <main/platform.h>

#include <raddebug.hpp>

static const float MIN_MOVIE_TIME = 0.125f;

// this will allow any buttons to skip FMV
//
//#define ANY_BUTTON_SKIP 

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

struct ControlMap
{
    char* inputName;
    FMVInput::FMVInputEnum inputID;
};

const ControlMap FMV_CONTROL_MAP[] =
{
#ifdef RAD_GAMECUBE
    { "Menu",           FMVInput::Skip },
    { "A",              FMVInput::Skip },
    #ifdef ANY_BUTTON_SKIP
    { "LeftStickX",     FMVInput::Skip },
    { "LeftStickY",     FMVInput::Skip },
    { "RightStickX",    FMVInput::Skip },
    { "RightStickY",    FMVInput::Skip },
    { "DPadLeft",       FMVInput::Skip },
    { "DPadRight",      FMVInput::Skip },
    { "DPadUp",         FMVInput::Skip },
    { "DPadDown",       FMVInput::Skip },
    { "B",              FMVInput::Skip },
    { "X",              FMVInput::Skip },
    { "Y",              FMVInput::Skip },
    { "Z",              FMVInput::Skip },
    { "TriggerL",       FMVInput::Skip },
    { "TriggerR",       FMVInput::Skip },
    #endif
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
    { "Start",          FMVInput::Start},
    { "X",              FMVInput::Skip },
    { "DPadLeft",       FMVInput::UNKNOWN },
    { "DPadRight",      FMVInput::UNKNOWN },
    { "DPadUp",         FMVInput::UNKNOWN },
    { "DPadDown",       FMVInput::UNKNOWN },
    { "Select",         FMVInput::UNKNOWN },
    { "Triangle",       FMVInput::UNKNOWN },
    { "Circle",         FMVInput::UNKNOWN },
    { "Square",         FMVInput::UNKNOWN },
    { "L1",             FMVInput::UNKNOWN },
    { "R1",             FMVInput::UNKNOWN },
    { "L2",             FMVInput::UNKNOWN },
    { "R2",             FMVInput::UNKNOWN },
   #ifdef ANY_BUTTON_SKIP
    { "LeftStickX",     FMVInput::Skip },
    { "LeftStickY",     FMVInput::Skip },
    { "RightStickX",    FMVInput::Skip },
    { "RightStickY",    FMVInput::Skip },
    #endif
#endif // RAD_PS2

#ifdef RAD_XBOX
    { "A",              FMVInput::Skip },
    { "Start",          FMVInput::Start},
    #ifdef ANY_BUTTON_SKIP
    { "LeftStickX",     FMVInput::Skip },
    { "LeftStickY",     FMVInput::Skip },
    { "RightStickX",    FMVInput::Skip },
    { "RightStickY",    FMVInput::Skip },
    { "DPadLeft",       FMVInput::Skip },
    { "DPadRight",      FMVInput::Skip },
    { "DPadUp",         FMVInput::Skip },
    { "DPadDown",       FMVInput::Skip },
    { "Back",           FMVInput::Skip },
    { "B",              FMVInput::Skip },
    { "X",              FMVInput::Skip },
    { "Y",              FMVInput::Skip },
    { "White",          FMVInput::Skip },
    { "Black",          FMVInput::Skip },
    { "LeftTrigger",    FMVInput::Skip },
    { "RightTrigger",   FMVInput::Skip },
    #endif
#endif // RAD_XBOX

#ifdef RAD_WIN32
    { "feStart",         FMVInput::Skip },
    { "feBack",          FMVInput::Skip },
    { "feKeyboardBack",  FMVInput::Skip },
    { "feSelect",        FMVInput::Skip },
#endif // RAD_WIN32

    { "",               FMVInput::UNKNOWN }
};

const int NUM_FMV_CONTROL_MAPPINGS = sizeof( FMV_CONTROL_MAP ) /
                                     sizeof( FMV_CONTROL_MAP[ 0 ] );

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// FMVUserInputHandler::FMVUserInputHandler
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
FMVUserInputHandler::FMVUserInputHandler( void )
:   m_isEnabled( true ),
    m_controllerPromptShown( false ),
    m_controllerReconnect( false )
{
}


//===========================================================================
// FMVUserInputHandler::~FMVUserInputHandler
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
FMVUserInputHandler::~FMVUserInputHandler( void )
{
}



//===========================================================================
// FMVUserInputHandler::OnButton
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
void FMVUserInputHandler::OnButton( int controllerId, int buttonId, const IButton* pButton )
{
}

void FMVUserInputHandler::OnButtonUp( int controllerId, int buttonId, const IButton* pButton )
{
}

void FMVUserInputHandler::OnControllerDisconnect( int id )
{
#ifndef RAD_GAMECUBE
    int controllerID = GetInputManager()->GetControllerIDforPlayer( 0 );
    if (GetGameFlow()->GetCurrentContext() == CONTEXT_GAMEPLAY
        && controllerID == id )
    {
        m_controllerPromptShown = true;
        char str_buffer[256];
        CGuiScreenMessage::GetControllerDisconnectedMessage(controllerID, str_buffer,  255);
        GetGame()->GetPlatform()->OnControllerError(str_buffer);
    }
#endif
}
void FMVUserInputHandler::OnControllerConnect( int id )
{
#ifndef RAD_GAMECUBE
    if (m_controllerPromptShown && id==GetInputManager()->GetControllerIDforPlayer( 0 ))
    {
        m_controllerReconnect = true;
    }
#endif
}


void FMVUserInputHandler::OnButtonDown( int controllerId, int buttonId, const IButton* pButton )
{
    bool button_skip = buttonId == FMVInput::Skip;

    if (buttonId==FMVInput::Start) // start also skip
        button_skip = true;

    if (buttonId==FMVInput::Start && m_controllerReconnect)
    {
        m_controllerReconnect = false;
        GetGame()->GetPlatform()->ClearControllerError();
        m_controllerPromptShown = false;
    }
    else if( m_isEnabled && button_skip )
    {
        if( GetPresentationManager()->GetFMVPlayer()->IsPlaying() &&
            GetPresentationManager()->GetFMVPlayer()->GetElapsedTime() > MIN_MOVIE_TIME )
        {
            GetPresentationManager()->GetFMVPlayer()->Abort();
        }
    }
}

void FMVUserInputHandler::LoadControllerMappings( unsigned int controllerId )
{
    // now set controller mappings
    for( int i = 0; i < NUM_FMV_CONTROL_MAPPINGS; i++ )
    {
        this->Map( FMV_CONTROL_MAP[ i ].inputName,
                   FMV_CONTROL_MAP[ i ].inputID,
                   0,
                   controllerId );
    }
}

